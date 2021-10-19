#include "snixembedproxy.h"

#include <QCoreApplication>
#include <QTimer>
#include <QX11Info>
#include <QDebug>

#include <KSelectionOwner>

#include <xcb/xcb.h>
#include <xcb/xcb_event.h>
#include <xcb/composite.h>

#include "xcbutilss.h"
#include "sniproxy.h"

#define SYSTEM_TRAY_REQUEST_DOCK    0
#define SYSTEM_TRAY_BEGIN_MESSAGE   1
#define SYSTEM_TRAY_CANCEL_MESSAGE  2
SniXembedProxy::SniXembedProxy()
{
     m_selectionOwner = new KSelectionOwner(Xcb::atoms->selectionAtom, -1, this);
    QTimer::singleShot(0, this, &SniXembedProxy::init);
}

SniXembedProxy::~SniXembedProxy()
{
}

void SniXembedProxy::init()
{
    //load damage extension
    xcb_connection_t *c = QX11Info::connection();
    xcb_prefetch_extension_data(c, &xcb_damage_id);
    const auto *reply = xcb_get_extension_data(c, &xcb_damage_id);
    if (reply && reply->present) {
        m_damageEventBase = reply->first_event;
        xcb_damage_query_version_unchecked(c, XCB_DAMAGE_MAJOR_VERSION, XCB_DAMAGE_MINOR_VERSION);
    } else {
        //no XDamage means
        qDebug() << "could not load damage extension. Quitting";
        qApp->exit(-1);
    }

    qApp->installNativeEventFilter(this);

    connect(m_selectionOwner, &KSelectionOwner::claimedOwnership, this, &SniXembedProxy::onClaimedOwnership);
    connect(m_selectionOwner, &KSelectionOwner::failedToClaimOwnership, this, &SniXembedProxy::onFailedToClaimOwnership);
    connect(m_selectionOwner, &KSelectionOwner::lostOwnership, this, &SniXembedProxy::onLostOwnership);
    m_selectionOwner->claim(false);
}

bool SniXembedProxy::nativeEventFilter(const QByteArray &eventType, void *message, long int *result)
{
    Q_UNUSED(result)

    if (eventType != "xcb_generic_event_t") {
        return false;
    }

    xcb_generic_event_t *ev = static_cast<xcb_generic_event_t *>(message);

    const auto responseType = XCB_EVENT_RESPONSE_TYPE(ev);
    if (responseType == XCB_CLIENT_MESSAGE) {
        const auto ce = reinterpret_cast<xcb_client_message_event_t *>(ev);
        if (ce->type == Xcb::atoms->opcodeAtom) {
            switch (ce->data.data32[1]) {
            case SYSTEM_TRAY_REQUEST_DOCK:
                dock(ce->data.data32[2]);
                return true;
            }
        }
    } else if (responseType == XCB_UNMAP_NOTIFY) {
        const auto unmappedWId = reinterpret_cast<xcb_unmap_notify_event_t *>(ev)->window;
        if (m_proxies.contains(unmappedWId)) {
            undock(unmappedWId);
        }
    } else if (responseType == XCB_DESTROY_NOTIFY) {
        const auto destroyedWId = reinterpret_cast<xcb_destroy_notify_event_t *>(ev)->window;
        if (m_proxies.contains(destroyedWId)) {
            undock(destroyedWId);
        }
    } else if (responseType == m_damageEventBase + XCB_DAMAGE_NOTIFY) {
        const auto damagedWId = reinterpret_cast<xcb_damage_notify_event_t *>(ev)->drawable;
        const auto sniProxy = m_proxies.value(damagedWId);
        if (sniProxy) {
            sniProxy->update();
            xcb_damage_subtract(QX11Info::connection(), m_damageWatches[damagedWId], XCB_NONE, XCB_NONE);
        }
    } else if (responseType == XCB_CONFIGURE_REQUEST) {
        const auto event = reinterpret_cast<xcb_configure_request_event_t *>(ev);
        const auto sniProxy = m_proxies.value(event->window);
        if (sniProxy) {
            // The embedded window tries to move or resize. Ignore move, handle resize only.
            if ((event->value_mask & XCB_CONFIG_WINDOW_WIDTH) || (event->value_mask & XCB_CONFIG_WINDOW_HEIGHT)) {
                sniProxy->resizeWindow(event->width, event->height);
            }
        }
    } else if (responseType == XCB_VISIBILITY_NOTIFY) {
        const auto event = reinterpret_cast<xcb_visibility_notify_event_t *>(ev);
        // it's possible that something showed our container window, we have to hide it
        // workaround for BUG 357443: when KWin is restarted, container window is shown on top
        if (event->state == XCB_VISIBILITY_UNOBSCURED) {
            for (auto sniProxy : m_proxies.values()) {
                sniProxy->hideContainerWindow(event->window);
            }
        }
    }

    return false;
}


void SniXembedProxy::dock(xcb_window_t winId)
{
    qDebug() << "trying to dock window " << winId;

    if (m_proxies.contains(winId)) {
        return;
    }

    if (addDamageWatch(winId)) {
        m_proxies[winId] = new SNIProxy(winId, this);
    }
}

void SniXembedProxy::undock(xcb_window_t winId)
{
    qDebug() << "trying to undock window " << winId;

    if (!m_proxies.contains(winId)) {
        return;
    }
    m_proxies[winId]->deleteLater();
    m_proxies.remove(winId);
}

bool SniXembedProxy::addDamageWatch(xcb_window_t client)
{
    qDebug() << "adding damage watch for " << client;

    xcb_connection_t *c = QX11Info::connection();
    const auto attribsCookie = xcb_get_window_attributes_unchecked(c, client);

    const auto damageId = xcb_generate_id(c);
    m_damageWatches[client] = damageId;
    xcb_damage_create(c, damageId, client, XCB_DAMAGE_REPORT_LEVEL_NON_EMPTY);

    xcb_generic_error_t *error = nullptr;
    QScopedPointer<xcb_get_window_attributes_reply_t, QScopedPointerPodDeleter> attr(xcb_get_window_attributes_reply(c, attribsCookie, &error));
    QScopedPointer<xcb_generic_error_t, QScopedPointerPodDeleter> getAttrError(error);
    uint32_t events = XCB_EVENT_MASK_STRUCTURE_NOTIFY;
    if (!attr.isNull()) {
        events = events | attr->your_event_mask;
    }
    // if window is already gone, there is no need to handle it.
    if (getAttrError && getAttrError->error_code == XCB_WINDOW) {
        return false;
    }
    // the event mask will not be removed again. We cannot track whether another component also needs STRUCTURE_NOTIFY (e.g. KWindowSystem).
    // if we would remove the event mask again, other areas will break.
    const auto changeAttrCookie = xcb_change_window_attributes_checked(c, client, XCB_CW_EVENT_MASK, &events);
    QScopedPointer<xcb_generic_error_t, QScopedPointerPodDeleter> changeAttrError(xcb_request_check(c, changeAttrCookie));
    // if window is gone by this point, it will be caught by eventFilter, so no need to check later errors.
    if (changeAttrError && changeAttrError->error_code == XCB_WINDOW) {
        return false;
    }

    return true;
}


void SniXembedProxy::onClaimedOwnership()
{
    qDebug() << "Manager selection claimed";

    setSystemTrayVisual();
}

void SniXembedProxy::onFailedToClaimOwnership()
{
    qWarning() << "failed to claim ownership of Systray Manager";
    qApp->exit(-1);
}

void SniXembedProxy::onLostOwnership()
{
    qWarning() << "lost ownership of Systray Manager";
    qApp->exit(-1);
}

void SniXembedProxy::setSystemTrayVisual()
{
    xcb_connection_t *c = QX11Info::connection();
    auto screen = xcb_setup_roots_iterator(xcb_get_setup(c)).data;
    auto trayVisual = screen->root_visual;
    xcb_depth_iterator_t depth_iterator = xcb_screen_allowed_depths_iterator(screen);
    xcb_depth_t *depth = nullptr;

    while (depth_iterator.rem) {
        if (depth_iterator.data->depth == 32) {
            depth = depth_iterator.data;
            break;
        }
        xcb_depth_next(&depth_iterator);
    }

    if (depth) {
        xcb_visualtype_iterator_t visualtype_iterator = xcb_depth_visuals_iterator(depth);
        while (visualtype_iterator.rem) {
            xcb_visualtype_t *visualtype = visualtype_iterator.data;
            if (visualtype->_class == XCB_VISUAL_CLASS_TRUE_COLOR) {
                trayVisual = visualtype->visual_id;
                break;
            }
            xcb_visualtype_next(&visualtype_iterator);
        }
    }

    xcb_change_property(c,
                        XCB_PROP_MODE_REPLACE,
                        m_selectionOwner->ownerWindow(),
                        Xcb::atoms->visualAtom,
                        XCB_ATOM_VISUALID,
                        32,
                        1,
                        &trayVisual);
}
