#ifndef SNIXEMBEDPROXY_H
#define SNIXEMBEDPROXY_H

#include <QObject>
#include <QHash>
#include <QAbstractNativeEventFilter>

#include <xcb/xcb.h>

class KSelectionOwner;
class SNIProxy;

class SniXembedProxy : public QObject,public QAbstractNativeEventFilter
{
    Q_OBJECT

public:
    SniXembedProxy();
    ~SniXembedProxy();

protected:
    bool nativeEventFilter(const QByteArray &eventType, void *message, long *result) override;

private:
    void init();
    bool addDamageWatch(xcb_window_t client);
    void dock(xcb_window_t embed_win);
    void undock(xcb_window_t client);
    void setSystemTrayVisual();

private Q_SLOTS:
    void onClaimedOwnership();
    void onFailedToClaimOwnership();
    void onLostOwnership();

private:
    uint8_t m_damageEventBase;

    QHash<xcb_window_t, u_int32_t> m_damageWatches;
    QHash<xcb_window_t, SNIProxy *> m_proxies;
    KSelectionOwner *m_selectionOwner;

};
#endif // SNIXEMBEDPROXY_H
