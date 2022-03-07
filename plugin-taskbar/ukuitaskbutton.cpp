/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Copyright: 2011 Razor team
 *            2014 LXQt team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.com>
 *   Kuzma Shapran <kuzma.shapran@gmail.com>
 *
 * Copyright: 2019 Tianjin KYLIN Information Technology Co., Ltd. *
 *
 * This program or library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */

#include "ukuitaskbutton.h"
#include "ukuitaskgroup.h"
#include "ukuitaskbar.h"

//#include <UKUi/Settings>
#include "../panel/common/ukuisettings.h"

#include <QDebug>
#include <QTimer>
#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>
#include <QPainter>
#include <QDrag>
#include <QMouseEvent>
#include <QMimeData>
#include <QApplication>
#include <QDragEnterEvent>
#include <QStylePainter>
#include <QStyleOptionToolButton>
#include <QDesktopWidget>

#include <unistd.h>
#include "ukuitaskgroup.h"
#include "ukuitaskbar.h"
#include "../panel/customstyle.h"
#include <KWindowSystem/KWindowSystem>
// Necessary for closeApplication()
#include <KWindowSystem/NETWM>
#include <QtX11Extras/QX11Info>

#include "../panel/iukuipanelplugin.h"
#include "../panel/highlight-effect.h"
#include <QAction>
#include <QDrag>
#include <QMenu>
#include <QMouseEvent>
#include <QStylePainter>
#include <QStyleOptionToolButton>
#include <QApplication>
#include <XdgIcon>
#include <string>

#define UKUI_PANEL_SETTINGS "org.ukui.panel.settings"
#define PANELPOSITION       "panelposition"

#define PANEL_SETTINGS      "org.ukui.panel.settings"
#define PANEL_SIZE_KEY      "panelsize"
#define ICON_SIZE_KEY       "iconsize"
#define PANEL_POSITION_KEY  "panelposition"

bool UKUITaskButton::m_draggging = false;

/************************************************

************************************************/
void LeftAlignedTextStyle::drawItemText(QPainter * painter, const QRect & rect, int flags
            , const QPalette & pal, bool enabled, const QString & text
            , QPalette::ColorRole textRole) const
{
    QString txt = QFontMetrics(painter->font()).elidedText(text, Qt::ElideRight, rect.width());
    return QProxyStyle::drawItemText(painter, rect, (flags & ~Qt::AlignHCenter) | Qt::AlignLeft, pal, enabled, txt, textRole);
}


/************************************************

************************************************/
UKUITaskButton::UKUITaskButton(QString appName,const WId window, UKUITaskBar * taskbar, QWidget *parent) :
    QToolButton(parent),
    m_window(window),
    m_appName(appName),
    m_urgencyHint(false),
    m_origin(Qt::TopLeftCorner),
    m_drawPixmap(false),
    m_parentTaskBar(taskbar),
    m_plugin(m_parentTaskBar->plugin()),
    m_DNDTimer(new QTimer(this))
{
    Q_ASSERT(taskbar);
    m_taskButtonStatus=NORMAL;
    setCheckable(true);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    setMinimumWidth(1);
    setMinimumHeight(1);
    setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    setAcceptDrops(true);

    updateText();
    updateIcon();

    m_DNDTimer->setSingleShot(true);
    m_DNDTimer->setInterval(700);
    connect(m_DNDTimer, SIGNAL(timeout()), this, SLOT(activateWithDraggable()));
    connect(UKUi::Settings::globalSettings(), SIGNAL(iconThemeChanged()), this, SLOT(updateIcon()));
    connect(m_parentTaskBar, &UKUITaskBar::iconByClassChanged, this, &UKUITaskButton::updateIcon);

    const QByteArray id(PANEL_SETTINGS);
    m_gsettings = new QGSettings(id);
    connect(m_gsettings, &QGSettings::changed, this, [=] (const QString &key) {
                if (key == PANEL_SIZE_KEY) {
                    updateIcon();
                }
            });
}

UKUITaskButton::UKUITaskButton(QString iconName,QString caption, const WId window, UKUITaskBar * taskbar, QWidget *parent) :
    QToolButton(parent),
    m_iconName(iconName),
    m_caption(caption),
    m_window(window),
    m_parentTaskBar(taskbar),
    m_plugin(m_parentTaskBar->plugin())
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_isWinActivate = true;
    m_icon = QIcon::fromTheme("application-x-desktop");
}
/************************************************

************************************************/
UKUITaskButton::~UKUITaskButton()
{
}

/************************************************

 ************************************************/
void UKUITaskButton::updateText()
{
    KWindowInfo info(m_window, NET::WMVisibleName | NET::WMName);
    QString title = info.visibleName().isEmpty() ? info.name() : info.visibleName();
    setText(title.replace("&", "&&"));
    setToolTip(title);
}

void UKUITaskButton::setLeaderWindow(WId leaderWindow) {
    m_window = leaderWindow;
}

/* int devicePixels = m_plugin->panel()->iconSize() * devicePixelRatioF()是由ico =KWindowSystem:ico(mwindow)更改的
 * 目的是为了能够显示正确的application-x-desktop的图标的大小
 *
*/
void UKUITaskButton::updateIcon()
{
    if (m_appName == QString("emo-system-ShellMethods") || m_appName == QString("Qq")) {
        sleep(1);
    }
    QIcon ico;
    int mIconSize=m_plugin->panel()->iconSize();
    if (m_parentTaskBar->isIconByClass()) {
        ico = QIcon::fromTheme(QString::fromUtf8(KWindowInfo{m_window, 0, NET::WM2WindowClass}.windowClassClass()).toLower());
    }
    if (ico.isNull()) {
#if QT_VERSION >= 0x050600
        int devicePixels = mIconSize * devicePixelRatioF();
#else
        int devicePixels = mIconSize * devicePixelRatio();
#endif
        ico = KWindowSystem::icon(m_window, devicePixels, devicePixels);
    }
    if (m_icon.isNull()) {
        m_icon = QIcon::fromTheme("application-x-desktop");
    }
    setIcon(ico.isNull() ? m_icon : ico);
    setIconSize(QSize(mIconSize,mIconSize));
}

void UKUITaskButton::setGroupIcon(QIcon ico)
{
    m_icon = ico;
}

/************************************************

 ************************************************/
void UKUITaskButton::refreshIconGeometry(QRect const & geom)
{
    NETWinInfo info(QX11Info::connection(),
                    windowId(),
                    (WId) QX11Info::appRootWindow(),
                    NET::WMIconGeometry,
                    0);
    NETRect const curr = info.iconGeometry();
    if (curr.pos.x != geom.x() || curr.pos.y != geom.y()
       || curr.size.width != geom.width() || curr.size.height != geom.height()) {
        NETRect nrect;
        nrect.pos.x = geom.x();
        nrect.pos.y = geom.y();
        nrect.size.height = geom.height();
        nrect.size.width = geom.width();
        info.setIconGeometry(nrect);
    }
}

/************************************************

 ************************************************/
void UKUITaskButton::dragEnterEvent(QDragEnterEvent *event)
{
    // It must be here otherwise dragLeaveEvent and dragMoveEvent won't be called
    // on the other hand drop and dragmove events of parent widget won't be called
    event->acceptProposedAction();
    if (event->mimeData()->hasFormat(mimeDataFormat())) {
        emit dragging(event->source(), event->pos());
        setAttribute(Qt::WA_UnderMouse, false);
    } else {
        m_DNDTimer->start();
    }
    QToolButton::dragEnterEvent(event);
}

void UKUITaskButton::dragMoveEvent(QDragMoveEvent * event)
{
    if (event->mimeData()->hasFormat(mimeDataFormat())) {
        emit dragging(event->source(), event->pos());
        setAttribute(Qt::WA_UnderMouse, false);
    }
}

void UKUITaskButton::dragLeaveEvent(QDragLeaveEvent *event)
{
    m_DNDTimer->stop();
    QToolButton::dragLeaveEvent(event);
}

void UKUITaskButton::dropEvent(QDropEvent *event)
{
    m_DNDTimer->stop();
    if (event->mimeData()->hasFormat(mimeDataFormat())) {
        //emit dropped(event->source(), event->pos());
        setAttribute(Qt::WA_UnderMouse, false);
    }
    //QToolButton::dropEvent(event);
}

/************************************************

 ************************************************/
void UKUITaskButton::mousePressEvent(QMouseEvent* event)
{
    const Qt::MouseButton b = event->button();

    if (Qt::LeftButton == b) {
        m_dragStartPosition = event->pos();
    } else if (m_statFlag && Qt::MidButton == b && parentTaskBar()->closeOnMiddleClick()) {
        closeApplication();
    }

    QToolButton::mousePressEvent(event);
}

/************************************************

 ************************************************/
void UKUITaskButton::mouseReleaseEvent(QMouseEvent* event)
{
//    if (event->button() == Qt::LeftButton)
//    {
//        if (isChecked())
//            minimizeApplication();
//        else
//        {
//            raiseApplication();
//        }
//    }
    QToolButton::mouseReleaseEvent(event);

}

/************************************************

 ************************************************/
QMimeData * UKUITaskButton::mimeData()
{
    QMimeData *mimedata = new QMimeData;
    QByteArray ba;
    QDataStream stream(&ba,QIODevice::WriteOnly);
    stream << (qlonglong)(m_window);
    mimedata->setData(mimeDataFormat(), ba);
    return mimedata;
}

/************************************************

 ************************************************/
void UKUITaskButton::mouseMoveEvent(QMouseEvent* event)
{
    if (!(event->buttons() & Qt::LeftButton)) {
        return;
    }

    if ((event->pos() - m_dragStartPosition).manhattanLength() < QApplication::startDragDistance()) {
        return;
    }

    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData());
    QIcon ico = icon();
    QPixmap img = ico.pixmap(ico.actualSize({32, 32}));
    drag->setPixmap(img);
    switch (m_plugin->panel()->position()) {
        case IUKUIPanel::PositionLeft:
        case IUKUIPanel::PositionTop:
            drag->setHotSpot({0, 0});
            break;
        case IUKUIPanel::PositionRight:
        case IUKUIPanel::PositionBottom:
            drag->setHotSpot(img.rect().bottomRight());
            break;
    }
    m_draggging = true;
    drag->exec();

    // if button is dropped out of panel (e.g. on desktop)
    // it is not deleted automatically by Qt
    drag->deleteLater();
    m_draggging = false;

    QAbstractButton::mouseMoveEvent(event);
}

/************************************************

 ************************************************/
bool UKUITaskButton::isApplicationHidden() const
{
    KWindowInfo info(m_window, NET::WMState);
    return (info.state() & NET::Hidden);
}

/************************************************

 ************************************************/
bool UKUITaskButton::isApplicationActive() const
{
    return KWindowSystem::activeWindow() == m_window;
}

/************************************************

 ************************************************/
void UKUITaskButton::activateWithDraggable()
{
    // raise app in any time when there is a drag
    // in progress to allow drop it into an app
    if (m_statFlag) {
        raiseApplication();
        KWindowSystem::forceActiveWindow(m_window);
    }
}

/************************************************

 ************************************************/
void UKUITaskButton::raiseApplication()
{
    KWindowInfo info(m_window, NET::WMDesktop | NET::WMState | NET::XAWMState);
    if (parentTaskBar()->raiseOnCurrentDesktop() && info.isMinimized()) {
        KWindowSystem::setOnDesktop(m_window, KWindowSystem::currentDesktop());
    } else {
        int winDesktop = info.desktop();
        if (KWindowSystem::currentDesktop() != winDesktop) {
            KWindowSystem::setCurrentDesktop(winDesktop);
        }
    }
    KWindowSystem::activateWindow(m_window);

    setUrgencyHint(false);
}

/************************************************

 ************************************************/
void UKUITaskButton::minimizeApplication()
{
    KWindowSystem::minimizeWindow(m_window);
}

/************************************************

 ************************************************/
void UKUITaskButton::maximizeApplication()
{
    QAction* act = qobject_cast<QAction*>(sender());
    if (!act) {
        return;
    }

    int state = act->data().toInt();
    switch (state) {
        case NET::MaxHoriz:
            KWindowSystem::setState(m_window, NET::MaxHoriz);
            break;

        case NET::MaxVert:
            KWindowSystem::setState(m_window, NET::MaxVert);
            break;

        default:
            KWindowSystem::setState(m_window, NET::Max);
            break;
    }

    if (!isApplicationActive()) {
        raiseApplication();
    }
}

/************************************************

 ************************************************/
void UKUITaskButton::deMaximizeApplication()
{
    KWindowSystem::clearState(m_window, NET::Max);

    if (!isApplicationActive()) {
        raiseApplication();
    }
}

/************************************************

 ************************************************/
void UKUITaskButton::shadeApplication()
{
    KWindowSystem::setState(m_window, NET::Shaded);
}

/************************************************

 ************************************************/
void UKUITaskButton::unShadeApplication()
{
    KWindowSystem::clearState(m_window, NET::Shaded);
}

/************************************************

 ************************************************/
void UKUITaskButton::closeApplication()
{
    // FIXME: Why there is no such thing in KWindowSystem??
    NETRootInfo(QX11Info::connection(), NET::CloseWindow).closeWindowRequest(m_window);
}

/************************************************

 ************************************************/
void UKUITaskButton::setApplicationLayer()
{
    QAction* act = qobject_cast<QAction*>(sender());
    if (!act) {
        return;
    }

    int layer = act->data().toInt();
    switch(layer) {
        case NET::KeepAbove:
            KWindowSystem::clearState(m_window, NET::KeepBelow);
            KWindowSystem::setState(m_window, NET::KeepAbove);
            break;

        case NET::KeepBelow:
            KWindowSystem::clearState(m_window, NET::KeepAbove);
            KWindowSystem::setState(m_window, NET::KeepBelow);
            break;

        default:
            KWindowSystem::clearState(m_window, NET::KeepBelow);
            KWindowSystem::clearState(m_window, NET::KeepAbove);
            break;
    }
}

/************************************************

 ************************************************/
void UKUITaskButton::moveApplicationToDesktop()
{
    QAction* act = qobject_cast<QAction*>(sender());
    if (!act) {
        return;
    }

    bool ok;
    int desk = act->data().toInt(&ok);

    if (!ok) {
        return;
    }

    KWindowSystem::setOnDesktop(m_window, desk);
}

/************************************************

 ************************************************/
void UKUITaskButton::moveApplication()
{
    KWindowInfo info(m_window, NET::WMDesktop);
    if (!info.isOnCurrentDesktop()) {
        KWindowSystem::setCurrentDesktop(info.desktop());
    }
    if (isMinimized()) {
        KWindowSystem::unminimizeWindow(m_window);
    }
    KWindowSystem::forceActiveWindow(m_window);
    const QRect& g = KWindowInfo(m_window, NET::WMGeometry).geometry();
    int X = g.center().x();
    int Y = g.center().y();
    QCursor::setPos(X, Y);
    NETRootInfo(QX11Info::connection(), NET::WMMoveResize).moveResizeRequest(m_window, X, Y, NET::Move);
}

/************************************************

 ************************************************/
void UKUITaskButton::resizeApplication()
{
    KWindowInfo info(m_window, NET::WMDesktop);
    if (!info.isOnCurrentDesktop()) {
        KWindowSystem::setCurrentDesktop(info.desktop());
    }
    if (isMinimized()) {
        KWindowSystem::unminimizeWindow(m_window);
    }
    KWindowSystem::forceActiveWindow(m_window);
    const QRect& g = KWindowInfo(m_window, NET::WMGeometry).geometry();
    int X = g.bottomRight().x();
    int Y = g.bottomRight().y();
    QCursor::setPos(X, Y);
    NETRootInfo(QX11Info::connection(), NET::WMMoveResize).moveResizeRequest(m_window, X, Y, NET::BottomRight);
}

/************************************************

 ************************************************/
void UKUITaskButton::contextMenuEvent(QContextMenuEvent* event)
{
    if (!m_statFlag) {
        return;
    }
    if (event->modifiers().testFlag(Qt::ControlModifier)) {
        event->ignore();
        return;
    }

    KWindowInfo info(m_window, 0, NET::WM2AllowedActions);
    unsigned long state = KWindowInfo(m_window, NET::WMState).state();

    QMenu * menu = new QMenu(tr("Application"));
    menu->setAttribute(Qt::WA_DeleteOnClose);
    QAction* a;

    /* KDE menu *******

      + To &Desktop >
      +     &All Desktops
      +     ---
      +     &1 Desktop 1
      +     &2 Desktop 2
      + &To Current Desktop
        &Move
        Re&size
      + Mi&nimize
      + Ma&ximize
      + &Shade
        Ad&vanced >
            Keep &Above Others
            Keep &Below Others
            Fill screen
        &Layer >
            Always on &top
            &Normal
            Always on &bottom
      ---
      + &Close
    */

    /********** Desktop menu **********/
    int deskNum = KWindowSystem::numberOfDesktops();
    if (deskNum > 1) {
        int winDesk = KWindowInfo(m_window, NET::WMDesktop).desktop();
        QMenu* deskMenu = menu->addMenu(tr("To &Desktop"));

        a = deskMenu->addAction(tr("&All Desktops"));
        a->setData(NET::OnAllDesktops);
        a->setEnabled(winDesk != NET::OnAllDesktops);
        connect(a, SIGNAL(triggered(bool)), this, SLOT(moveApplicationToDesktop()));
        deskMenu->addSeparator();

        for (int i = 0; i < deskNum; ++i) {
            a = deskMenu->addAction(tr("Desktop &%1").arg(i + 1));
            a->setData(i + 1);
            a->setEnabled(i + 1 != winDesk);
            connect(a, SIGNAL(triggered(bool)), this, SLOT(moveApplicationToDesktop()));
        }

        int curDesk = KWindowSystem::currentDesktop();
        a = menu->addAction(tr("&To Current Desktop"));
        a->setData(curDesk);
        a->setEnabled(curDesk != winDesk);
        connect(a, SIGNAL(triggered(bool)), this, SLOT(moveApplicationToDesktop()));
    }

    /********** Move/Resize **********/
    menu->addSeparator();
    a = menu->addAction(tr("&Move"));
    a->setEnabled(info.actionSupported(NET::ActionMove) && !(state & NET::Max) && !(state & NET::FullScreen));
    connect(a, &QAction::triggered, this, &UKUITaskButton::moveApplication);
    a = menu->addAction(tr("Resi&ze"));
    a->setEnabled(info.actionSupported(NET::ActionResize) && !(state & NET::Max) && !(state & NET::FullScreen));
    connect(a, &QAction::triggered, this, &UKUITaskButton::resizeApplication);

    /********** State menu **********/
    menu->addSeparator();

    a = menu->addAction(tr("Ma&ximize"));
    a->setEnabled(info.actionSupported(NET::ActionMax) && (!(state & NET::Max) || (state & NET::Hidden)));
    a->setData(NET::Max);
    connect(a, SIGNAL(triggered(bool)), this, SLOT(maximizeApplication()));

    if (event->modifiers() & Qt::ShiftModifier) {
        a = menu->addAction(tr("Maximize vertically"));
        a->setEnabled(info.actionSupported(NET::ActionMaxVert) && !((state & NET::MaxVert) || (state & NET::Hidden)));
        a->setData(NET::MaxVert);
        connect(a, SIGNAL(triggered(bool)), this, SLOT(maximizeApplication()));

        a = menu->addAction(tr("Maximize horizontally"));
        a->setEnabled(info.actionSupported(NET::ActionMaxHoriz) && !((state & NET::MaxHoriz) || (state & NET::Hidden)));
        a->setData(NET::MaxHoriz);
        connect(a, SIGNAL(triggered(bool)), this, SLOT(maximizeApplication()));
    }

    a = menu->addAction(tr("&Restore"));
    a->setEnabled((state & NET::Hidden) || (state & NET::Max) || (state & NET::MaxHoriz) || (state & NET::MaxVert));
    connect(a, SIGNAL(triggered(bool)), this, SLOT(deMaximizeApplication()));

    a = menu->addAction(tr("Mi&nimize"));
    a->setEnabled(info.actionSupported(NET::ActionMinimize) && !(state & NET::Hidden));
    connect(a, SIGNAL(triggered(bool)), this, SLOT(minimizeApplication()));

    if (state & NET::Shaded) {
        a = menu->addAction(tr("Roll down"));
        a->setEnabled(info.actionSupported(NET::ActionShade) && !(state & NET::Hidden));
        connect(a, SIGNAL(triggered(bool)), this, SLOT(unShadeApplication()));
    } else {
        a = menu->addAction(tr("Roll up"));
        a->setEnabled(info.actionSupported(NET::ActionShade) && !(state & NET::Hidden));
        connect(a, SIGNAL(triggered(bool)), this, SLOT(shadeApplication()));
    }

    /********** Layer menu **********/
    menu->addSeparator();

    QMenu* layerMenu = menu->addMenu(tr("&Layer"));

    a = layerMenu->addAction(tr("Always on &top"));
    // FIXME: There is no info.actionSupported(NET::ActionKeepAbove)
    a->setEnabled(!(state & NET::KeepAbove));
    a->setData(NET::KeepAbove);
    connect(a, SIGNAL(triggered(bool)), this, SLOT(setApplicationLayer()));

    a = layerMenu->addAction(tr("&Normal"));
    a->setEnabled((state & NET::KeepAbove) || (state & NET::KeepBelow));
    // FIXME: There is no NET::KeepNormal, so passing 0
    a->setData(0);
    connect(a, SIGNAL(triggered(bool)), this, SLOT(setApplicationLayer()));

    a = layerMenu->addAction(tr("Always on &bottom"));
    // FIXME: There is no info.actionSupported(NET::ActionKeepBelow)
    a->setEnabled(!(state & NET::KeepBelow));
    a->setData(NET::KeepBelow);
    connect(a, SIGNAL(triggered(bool)), this, SLOT(setApplicationLayer()));

    /********** Kill menu **********/
    menu->addSeparator();
    a = menu->addAction(QIcon::fromTheme("process-stop"), tr("&Close"));
    connect(a, SIGNAL(triggered(bool)), this, SLOT(closeApplication()));
    menu->setGeometry(m_parentTaskBar->panel()->calculatePopupWindowPos(mapToGlobal(event->pos()), menu->sizeHint()));
    m_plugin->willShowWindow(menu);
    menu->show();
}

/************************************************

 ************************************************/
void UKUITaskButton::setUrgencyHint(bool set)
{
    if (m_urgencyHint == set) {
        return;
    }

    if (!set) {
        KWindowSystem::demandAttention(m_window, false);
    }

    m_urgencyHint = set;
    setProperty("urgent", set);
    style()->unpolish(this);
    style()->polish(this);
    update();
}

bool UKUITaskButton::isOnDesktop(int desktop) const
{
    return KWindowInfo(m_window, NET::WMDesktop).isOnDesktop(desktop);
}

bool UKUITaskButton::isOnCurrentScreen() const
{
    return QApplication::desktop()->screenGeometry(parentTaskBar()).intersects(KWindowInfo(m_window, NET::WMFrameExtents).frameGeometry());
}

bool UKUITaskButton::isMinimized() const
{
    return KWindowInfo(m_window,NET::WMState | NET::XAWMState).isMinimized();
}

Qt::Corner UKUITaskButton::origin() const
{
    return m_origin;
}

void UKUITaskButton::setOrigin(Qt::Corner newOrigin)
{
    if (m_origin != newOrigin) {
        m_origin = newOrigin;
        update();
    }
}

void UKUITaskButton::setAutoRotation(bool value, IUKUIPanel::Position position)
{
    if (value) {
        switch (position) {
            case IUKUIPanel::PositionTop:
            case IUKUIPanel::PositionBottom:
                setOrigin(Qt::TopLeftCorner);
                break;

            case IUKUIPanel::PositionLeft:
                setOrigin(Qt::BottomLeftCorner);
                break;

            case IUKUIPanel::PositionRight:
                setOrigin(Qt::TopRightCorner);
                break;
        }
    } else {
        setOrigin(Qt::TopLeftCorner);
    }
}

void UKUITaskButton::enterEvent(QEvent *)
{
    m_taskButtonStatus=HOVER;
    update();
}

void UKUITaskButton::leaveEvent(QEvent *)
{
    m_taskButtonStatus=NORMAL;
    update();
}

/*在paintEvent中执行绘图事件会造成高分屏下图片模糊
 * 高分屏的图片模糊问题大概率与svg/png图片无关
 * */
void UKUITaskButton::paintEvent(QPaintEvent *event)
{
        QToolButton::paintEvent(event);
        return;
}

bool UKUITaskButton::hasDragAndDropHover() const
{
    return m_DNDTimer->isActive();
}



UKUITaskButton::UKUITaskButton(QuickLaunchAction * act, IUKUIPanelPlugin * plugin, QWidget * parent)
    : QToolButton(parent),
      m_act(act),
      m_plugin(plugin)
{
    m_DNDTimer = new QTimer(this);
    m_statFlag = false;
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    /*设置快速启动栏的按键不接受焦点*/
    setFocusPolicy(Qt::NoFocus);
    setAutoRaise(true);
    m_quickLanuchStatus = NORMAL;

    setDefaultAction(m_act);
    m_act->setParent(this);

    /*设置快速启动栏的菜单项*/
    const QByteArray id(UKUI_PANEL_SETTINGS);
    m_gsettingsQuickLaunch = new QGSettings(id);
    modifyQuicklaunchMenuAction(true);
    connect(m_gsettingsQuickLaunch, &QGSettings::changed, this, [=] (const QString &key){
                if (key==PANELPOSITION) {
                    modifyQuicklaunchMenuAction(true);
                }
            });

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
            this, SLOT(this_customContextMenuRequested(const QPoint&)));
    m_DNDTimer->setSingleShot(true);
    m_DNDTimer->setInterval(700);
    connect(m_DNDTimer, SIGNAL(timeout()), this, SLOT(activateWithDraggable()));
    m_fileName=act->m_settingsMap["desktop"];
    this->setStyle(new CustomStyle());
    repaint();

}


QHash<QString,QString> UKUITaskButton::settingsMap()
{
    Q_ASSERT(m_act);
    return m_act->settingsMap();
}

/*与鼠标右键的选项有关*/
void UKUITaskButton::this_customContextMenuRequested(const QPoint & pos)
{
    m_plugin->willShowWindow(m_menu);
    m_menu->popup(m_plugin->panel()->calculatePopupWindowPos(mapToGlobal({0, 0}), m_menu->sizeHint()).topLeft());
}

/*调整快速启动栏的菜单项*/
void UKUITaskButton::modifyQuicklaunchMenuAction(bool direction)
{

    m_deleteAct = new QAction(HighLightEffect::drawSymbolicColoredIcon(QIcon::fromTheme("ukui-unfixed")), tr("delete from quicklaunch"), this);
    connect(m_deleteAct, SIGNAL(triggered()), this, SLOT(selfRemove()));
    //addAction(m_deleteAct);
    m_menu = new QuicklaunchMenu();
    m_menu->addAction(m_act);
    m_menu->addActions(m_act->addtitionalActions());
    m_menu->addSeparator();
    m_menu->addSeparator();
    m_menu->addAction(m_deleteAct);
}

void UKUITaskButton::selfRemove()
{
    emit buttonDeleted();
}


QuicklaunchMenu::QuicklaunchMenu() { }

QuicklaunchMenu::~QuicklaunchMenu() { }

void QuicklaunchMenu::contextMenuEvent(QContextMenuEvent *) { }
