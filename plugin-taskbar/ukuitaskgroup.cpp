/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Copyright: 2011 Razor team
 *            2014 LXQt team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.com>
 *   Maciej Płaza <plaza.maciej@gmail.com>
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

#include "ukuitaskgroup.h"
#include "ukuitaskbar.h"

#include <QDebug>
#include <QMimeData>
#include <QFocusEvent>
#include <QDragLeaveEvent>
#include <QStringBuilder>
#include <QMenu>
#include <KF5/KWindowSystem/KWindowSystem>
#include <functional>
#include <QProcess>

#include <QtX11Extras/QX11Info>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <QLabel>
#include <QScrollBar>

#include <qmainwindow.h>
#include <QWidget>
#include <QDesktopWidget>
#include <QApplication>
#include "../panel/iukuipanelplugin.h"
#include "../panel/highlight-effect.h"
#include <QSize>
#include <QScreen>
#include <XdgIcon>
#include <XdgDesktopFile>
#include <QMessageBox>
#include "../panel/customstyle.h"
#define UKUI_PANEL_SETTINGS "org.ukui.panel.settings"
#define PANELPOSITION       "panelposition"

#define UKUI_PANEL_DAEMON           "org.ukui.panel.daemon"
#define UKUI_PANEL_DAEMON_PATH      "/convert/desktopwid"
#define UKUI_PANEL_DAEMON_INTERFACE "org.ukui.panel.daemon"
#define UKUI_PANEL_DAEMON_METHOD    "WIDToDesktop"


#define WAYLAND_GROUP_HIDE     0
#define WAYLAND_GROUP_ACTIVATE 1
#define WAYLAND_GROUP_CLOSE    2


QPixmap qimageFromXImage(XImage* ximage)
{
    QImage::Format format = QImage::Format_ARGB32_Premultiplied;
    if (ximage->depth == 24)
        format = QImage::Format_RGB32;
    else if (ximage->depth == 16)
        format = QImage::Format_RGB16;

    QImage image = QImage(reinterpret_cast<uchar*>(ximage->data),
                          ximage->width, ximage->height,
                          ximage->bytes_per_line, format).copy();

    // 大端还是小端?
    if ((QSysInfo::ByteOrder == QSysInfo::LittleEndian && ximage->byte_order == MSBFirst)
            || (QSysInfo::ByteOrder == QSysInfo::BigEndian && ximage->byte_order == LSBFirst)) {

        for (int i = 0; i < image.height(); i++) {
            if (ximage->depth == 16) {
                ushort* p = reinterpret_cast<ushort*>(image.scanLine(i));
                ushort* end = p + image.width();
                while (p < end) {
                    *p = ((*p << 8) & 0xff00) | ((*p >> 8) & 0x00ff);
                    p++;
                }
            } else {
                uint* p = reinterpret_cast<uint*>(image.scanLine(i));
                uint* end = p + image.width();
                while (p < end) {
                    *p = ((*p << 24) & 0xff000000) | ((*p << 8) & 0x00ff0000)
                         | ((*p >> 8) & 0x0000ff00) | ((*p >> 24) & 0x000000ff);
                    p++;
                }
            }
        }
    }

    // 修复alpha通道
    if (format == QImage::Format_RGB32) {
        QRgb* p = reinterpret_cast<QRgb*>(image.bits());
        for (int y = 0; y < ximage->height; ++y) {
            for (int x = 0; x < ximage->width; ++x)
                p[x] |= 0xff000000;
            p += ximage->bytes_per_line / 4;
        }
    }
    return QPixmap::fromImage(image);
}

/************************************************

 ************************************************/
UKUITaskGroup::UKUITaskGroup(QuickLaunchAction * act, IUKUIPanelPlugin * plugin, UKUITaskBar * parent)
    : UKUITaskButton(act, plugin, parent),
      m_plugin(plugin),
      m_act(act),
      m_parent(parent)
{
    m_statFlag = false;
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setAcceptDrops(true);
    /*设置快速启动栏的按键不接受焦点*/
    setFocusPolicy(Qt::NoFocus);
    setAutoRaise(true);
    m_quickLanuchStatus = NORMAL;

    setDefaultAction(m_act);
    m_act->setParent(this);

    /*设置快速启动栏的菜单项*/
    const QByteArray id(UKUI_PANEL_SETTINGS);
    m_gsettings = new QGSettings(id);
    toDomodifyQuicklaunchMenuAction(true);
    connect(m_gsettings, &QGSettings::changed, this, [=] (const QString &key){
        if(key==PANELPOSITION){
            toDomodifyQuicklaunchMenuAction(true);
        }
    });
    setContextMenuPolicy(Qt::CustomContextMenu);

    m_fileName=act->m_settingsMap["desktop"];
    m_file = act->m_settingsMap["file"];
    m_exec = act->m_settingsMap["exec"];
    m_name = act->m_settingsMap["name"];
    this->setStyle(new CustomStyle());
    repaint();
}


UKUITaskGroup::UKUITaskGroup(const QString &groupName, WId window, UKUITaskBar *parent)
    : UKUITaskButton(groupName,window, parent, parent),
    m_groupName(groupName),
    m_popup(new UKUIGroupPopup(this)),
    m_preventPopup(false),
    m_singleButton(true),
    m_timer(new QTimer(this)),
    m_widget(NULL),
    m_parent(parent),
    m_isWaylandGroup(false)
{
    Q_ASSERT(parent);
    m_scrollArea = NULL;
    m_taskGroupStatus = NORMAL;

    initDesktopFileName(window);

    initActionsInRightButtonMenu();

    connect(this, SIGNAL(clicked(bool)), this, SLOT(onClicked(bool)));
    connect(KWindowSystem::self(), SIGNAL(activeWindowChanged(WId)), this, SLOT(onActiveWindowChanged(WId)));
    connect(parent, &UKUITaskBar::refreshIconGeometry, this, &UKUITaskGroup::refreshIconsGeometry);
    connect(parent, &UKUITaskBar::buttonStyleRefreshed, this, &UKUITaskGroup::setToolButtonsStyle);
    connect(parent, &UKUITaskBar::showOnlySettingChanged, this, &UKUITaskGroup::refreshVisibility);
    connect(parent, &UKUITaskBar::popupShown, this, &UKUITaskGroup::groupPopupShown);
    m_timer->setTimerType(Qt::PreciseTimer);
    connect(m_timer, SIGNAL(timeout()), SLOT(timeout()));
}

UKUITaskGroup::UKUITaskGroup(const QString & iconName, const QString & caption, WId window, UKUITaskBar *parent) :
    UKUITaskButton(iconName, caption, window, parent, parent),
    m_timer(new QTimer(this)),
    m_popup(new UKUIGroupPopup(this)),
    m_preventPopup(false),
    m_widget(NULL),
    m_singleButton(false),
    m_isWaylandGroup(true)
{
    //setObjectName(caption);
    //setText(caption);
    Q_ASSERT(parent);
    m_iconName=iconName;
    m_taskGroupStatus = NORMAL;
    m_isWinActivate = true;
    setIcon(QIcon::fromTheme(iconName));
    connect(this, SIGNAL(clicked(bool)), this, SLOT(onClicked(bool)));
    connect(parent, &UKUITaskBar::buttonStyleRefreshed, this, &UKUITaskGroup::setToolButtonsStyle);
    //connect(parent, &UKUITaskBar::showOnlySettingChanged, this, &UKUITaskGroup::refreshVisibility);
    connect(parent, &UKUITaskBar::popupShown, this, &UKUITaskGroup::groupPopupShown);
    m_timer->setTimerType(Qt::PreciseTimer);
    connect(m_timer, SIGNAL(timeout()), SLOT(timeout()));
}

UKUITaskGroup::~UKUITaskGroup()
{
}

void UKUITaskGroup::initDesktopFileName(int window) {
    QDBusInterface iface(UKUI_PANEL_DAEMON,
                         UKUI_PANEL_DAEMON_PATH,
                         UKUI_PANEL_DAEMON_INTERFACE,
                         QDBusConnection::sessionBus());
    QDBusReply<QString> reply = iface.call(UKUI_PANEL_DAEMON_METHOD, window);
    QString processExeName = reply.value();
    if (!processExeName.isEmpty()) {
        m_fileName = processExeName;
    }
}

void UKUITaskGroup::initActionsInRightButtonMenu(){
    if (m_fileName.isEmpty()) return;
    const auto url=QUrl(m_fileName);
    QString fileName(url.isLocalFile() ? url.toLocalFile() : url.url());
    XdgDesktopFile xdg;
    if (xdg.load(fileName)){
        m_act = new QuickLaunchAction(&xdg, this);
        setGroupIcon(m_act->getIconfromAction());
    }
}


void UKUITaskGroup::contextMenuEvent(QContextMenuEvent *event)
{
    setPopupVisible(false, true);
    m_preventPopup = true;
    if (m_singleButton && !m_isWaylandGroup)
    {
        UKUITaskButton::contextMenuEvent(event);
        return;
    }

    QMenu * menu = new QMenu(tr("Group"));
    menu->setAttribute(Qt::WA_DeleteOnClose);
    if (!m_fileName.isEmpty()) {
        menu->addAction(m_act);
        menu->addActions(m_act->addtitionalActions());
        menu->addSeparator();
        menu->addSeparator();
        QAction *m_deleteAct = menu->addAction(HighLightEffect::drawSymbolicColoredIcon(QIcon::fromTheme("ukui-unfixed")), tr("delete from taskbar"));
        connect(m_deleteAct, SIGNAL(triggered()), this, SLOT(RemovefromTaskBar()));
        QAction *mAddAct = menu->addAction(HighLightEffect::drawSymbolicColoredIcon(QIcon::fromTheme("ukui-fixed")), tr("add to taskbar"));
        connect(mAddAct, SIGNAL(triggered()), this, SLOT(AddtoTaskBar()));
        if (m_existSameQckBtn) menu->removeAction(mAddAct);
        else menu->removeAction(m_deleteAct);
    }
    QAction *mCloseAct = menu->addAction(QIcon::fromTheme("application-exit-symbolic"), tr("close"));
    connect(mCloseAct, SIGNAL(triggered()), this, SLOT(closeGroup()));
    connect(menu, &QMenu::aboutToHide, [this] {
        m_preventPopup = false;
    });
    menu->setGeometry(plugin()->panel()->calculatePopupWindowPos(mapToGlobal(event->pos()), menu->sizeHint()));
    plugin()->willShowWindow(menu);
    menu->show();
}
void UKUITaskGroup::RemovefromTaskBar()
{
    emit WindowRemovefromTaskBar(m_fileName);
}
void UKUITaskGroup::AddtoTaskBar()
{
    emit WindowAddtoTaskBar(groupName());
}
/************************************************

 ************************************************/
void UKUITaskGroup::closeGroup()
{
    if (m_isWaylandGroup) {
        closeGroup_wl();
        return;
    }
#if (QT_VERSION < QT_VERSION_CHECK(5,7,0))
    for(auto it=m_buttonHash.begin();it!=m_buttonHash.end();it++)
    {
    UKUITaskWidget *button =it.value();
  if (button->isOnDesktop(KWindowSystem::currentDesktop()))
            button->closeApplication();
    }
#else
    for (UKUITaskWidget *button : qAsConst(m_buttonHash) )
        if (button->isOnDesktop(KWindowSystem::currentDesktop()))
            button->closeApplication();
#endif
}

/************************************************

 ************************************************/
QWidget * UKUITaskGroup::addWindow(WId id)
{

    if (m_buttonHash.contains(id))
        return m_buttonHash.value(id);
    UKUITaskWidget *btn = new UKUITaskWidget(id, parentTaskBar(), m_popup);
    m_buttonHash.insert(id, btn);
    connect(btn, SIGNAL(clicked()), this, SLOT(onChildButtonClicked()));
    connect(btn, SIGNAL(windowMaximize()), this, SLOT(onChildButtonClicked()));
    refreshVisibility();

    changeTaskButtonStyle();

    return btn;
}


/*changeTaskButtonStyle in class UKUITaskGroup not class UKUITaskButton
 * because class UKUITaskButton can not get m_buttonHash.size
 */
void UKUITaskGroup::changeTaskButtonStyle()
{
    if(m_visibleHash.size()>1)
        this->setStyle(new CustomStyle("taskbutton",true));
    else
        this->setStyle(new CustomStyle("taskbutton",false));
}

void UKUITaskGroup::onActiveWindowChanged(WId window)
{
    UKUITaskWidget *button = m_buttonHash.value(window, nullptr);
//    for (QWidget *btn : qAsConst(m_buttonHash))
//        btn->setChecked(false);

//    if (button)
//    {
//        button->setChecked(true);
//        if (button->hasUrgencyHint())
//            button->setUrgencyHint(false);
//    }
    setChecked(nullptr != button);
}

/************************************************

 ************************************************/
void UKUITaskGroup::onDesktopChanged()
{
    refreshVisibility();
    changeTaskButtonStyle();
}

/************************************************

 ************************************************/
void UKUITaskGroup::onWindowRemoved(WId window)
{
    if (m_buttonHash.contains(window))
    {
        UKUITaskWidget *button = m_buttonHash.value(window);
        m_buttonHash.remove(window);
        if (m_visibleHash.contains(window)) {
            m_showInTurn.removeOne(m_visibleHash.value(window));
            m_visibleHash.remove(window);
        }
        m_popup->removeWidget(button);
        button->deleteLater();
        if (!parentTaskBar()->getCpuInfoFlg())
            system(QString("rm -f /tmp/%1.png").arg(window).toLatin1());
        if (isLeaderWindow(window) && (m_showInTurn.size() > 0))
            setLeaderWindow(m_buttonHash.key(m_showInTurn.at(0)));
        if (m_buttonHash.count())
        {
            if(m_popup->isVisible())
            {
//                m_popup->hide(true);
                showPreview();
            }
            else
            {
                regroup();
            }
        }
        else
        {
            if (isVisible())
                emit visibilityChanged(false);
            hide();
            emit groupBecomeEmpty(groupName());

        }
        changeTaskButtonStyle();
    }
}

/************************************************

 ************************************************/
void UKUITaskGroup::onChildButtonClicked()
{
    setPopupVisible(false, true);
    parentTaskBar()->setShowGroupOnHover(true);
    //QToolButton::leaveEvent(event);
    m_taskGroupStatus = NORMAL;
    update();
}

/************************************************

 ************************************************/
Qt::ToolButtonStyle UKUITaskGroup::popupButtonStyle() const
{
    // do not set icons-only style in the buttons in the group,
    // as they'll be indistinguishable
    const Qt::ToolButtonStyle style = toolButtonStyle();
    return style == Qt::ToolButtonIconOnly ? Qt::ToolButtonTextBesideIcon : style;
}

/************************************************

 ************************************************/
void UKUITaskGroup::setToolButtonsStyle(Qt::ToolButtonStyle style)
{
    setToolButtonStyle(style);

//    const Qt::ToolButtonStyle styleInPopup = popupButtonStyle();
//    for (auto & button : m_buttonHash)
//    {
//        button->setToolButtonStyle(styleInPopup);
//    }
}

/************************************************

 ************************************************/
int UKUITaskGroup::buttonsCount() const
{
    return m_buttonHash.count();
}

void UKUITaskGroup::initVisibleHash()
{
   /* for (UKUITaskButtonHash::const_iterator it = m_buttonHash.begin();it != m_buttonHash.end();it++)
    {
        if (m_visibleHash.contains(it.key())) continue;
        if (it.value()->isVisibleTo(m_popup))
            m_visibleHash.insert(it.key(), it.value());
    }*/
}

/************************************************

 ************************************************/
int UKUITaskGroup::visibleButtonsCount() const
{
    int i = 0;
#if (QT_VERSION < QT_VERSION_CHECK(5,7,0))
    for (auto it=m_buttonHash.begin();it!=m_buttonHash.end();it++)
     {
        UKUITaskWidget *btn=it.value();
        if (btn->isVisibleTo(m_popup))
            i++;
    }
#else
    for (UKUITaskWidget *btn : qAsConst(m_buttonHash))
        if (btn->isVisibleTo(m_popup))
            i++;
#endif
    return i;
}

/************************************************

 ************************************************/
void UKUITaskGroup::draggingTimerTimeout()
{
    if (m_singleButton)
        setPopupVisible(false);
}

/************************************************

 ************************************************/
void UKUITaskGroup::onClicked(bool)
{
    if (m_isWaylandGroup) {
        winClickActivate_wl(!m_isWinActivate);
        return;
    }
    if (1 == m_visibleHash.size())
    {
        return singleWindowClick();
    }
    if(m_popup->isVisible())
    {
        if(HOVER == m_taskGroupStatus)
        {
            m_taskGroupStatus = NORMAL;
            return;
        }
        else
        {
            m_popup->hide();
            return;
        }
    }
    else
    {
        showPreview();
    }
    m_taskGroupEvent = OTHEREVENT;
    if(m_timer->isActive())
    {
       m_timer->stop();
    }
}


void UKUITaskGroup::singleWindowClick()
{
    UKUITaskWidget *btn = m_visibleHash.begin().value();
    if(btn)
    {
        if(!btn->isFocusState() || btn->isMinimized())
        {
            if(m_popup->isVisible())
            {
                m_popup->hide();
            }
            KWindowSystem::forceActiveWindow(m_visibleHash.begin().key());
        }
        else
        {
            btn->minimizeApplication();
            if(m_popup->isVisible())
            {
                m_popup->hide();
            }
        }
    }
    m_taskGroupEvent = OTHEREVENT;
    if(m_timer->isActive())
    {
       m_timer->stop();
    }
}

/************************************************

 ************************************************/
void UKUITaskGroup::regroup()
{
    int cont = visibleButtonsCount();
    recalculateFrameIfVisible();

//    if (cont == 1)
//    {
//        m_singleButton = false;
//        // Get first visible button
//        UKUITaskButton * button = NULL;
//        for (UKUITaskButton *btn : qAsConst(m_buttonHash))
//        {
//            if (btn->isVisibleTo(m_popup))
//            {
//                button = btn;
//                break;
//            }
//        }

//        if (button)
//        {
//            setText(button->text());
//            setToolTip(button->toolTip());
//            setWindowId(button->windowId());

//        }
//    }
    /*else*/ if (cont == 0) {
       // emit groupHidden(groupName());
        hide();
    }
    else
    {
        m_singleButton = false;
        QString t = QString("%1 - %2 windows").arg(m_groupName).arg(cont);
        setText(t);
        setToolTip(parentTaskBar()->isShowGroupOnHover() ? QString() : t);
    }

}

/************************************************

 ************************************************/
void UKUITaskGroup::recalculateFrameIfVisible()
{
    if (m_popup->isVisible())
    {
        recalculateFrameSize();
        if (plugin()->panel()->position() == IUKUIPanel::PositionBottom)
            recalculateFramePosition();
    }
}

/************************************************

 ************************************************/
void UKUITaskGroup::setAutoRotation(bool value, IUKUIPanel::Position position)
{
//    for (QWidget *button : qAsConst(m_buttonHash))
//        button->setAutoRotation(false, position);

    //UKUITaskWidget::setAutoRotation(value, position);
}

/************************************************

 ************************************************/
void UKUITaskGroup::refreshVisibility()
{
    bool will = false;
    UKUITaskBar const * taskbar = parentTaskBar();
    const int showDesktop = taskbar->showDesktopNum();

    for(UKUITaskButtonHash::const_iterator i=m_buttonHash.begin();i!=m_buttonHash.end();i++)
    {
        UKUITaskWidget * btn=i.value();
        bool visible = taskbar->isShowOnlyOneDesktopTasks() ? btn->isOnDesktop(0 == showDesktop ? KWindowSystem::currentDesktop() : showDesktop) : true;
        visible &= taskbar->isShowOnlyCurrentScreenTasks() ? btn->isOnCurrentScreen() : true;
        visible &= taskbar->isShowOnlyMinimizedTasks() ? btn->isMinimized() : true;
        btn->setVisible(visible);
        if (btn->isVisibleTo(m_popup) && !m_visibleHash.contains(i.key())) {
            m_visibleHash.insert(i.key(), i.value());
            m_showInTurn.push_back(i.value());
        } else if (!btn->isVisibleTo(m_popup) && m_visibleHash.contains(i.key())) {
                    m_visibleHash.remove(i.key());
                    m_showInTurn.removeOne(i.value());
        }
        will |= visible;
    }
    if (!m_showInTurn.isEmpty())
        setLeaderWindow(m_visibleHash.key(m_showInTurn.at(0)));

    bool is = isVisible();
  //  emit groupVisible(groupName(), will);
   // else setVisible(will);
  //  will &= this->isVisible();
    setVisible(will);
    if (this->m_existSameQckBtn)
        m_qckLchBtn->setHidden(will);
    if(!m_popup->isVisible())
    {
        regroup();
    }

    if (is != will)
        emit visibilityChanged(will);
}

/************************************************

 ************************************************/
QMimeData * UKUITaskGroup::mimeData()
{
    QMimeData *mimedata = new QMimeData;
    QByteArray byteArray;
    QDataStream stream(&byteArray, QIODevice::WriteOnly);
    stream << groupName();
    mimedata->setData(mimeDataFormat(), byteArray);
    return mimedata;
}

/************************************************

 ************************************************/
void UKUITaskGroup::setPopupVisible(bool visible, bool fast)
{
    if (!m_statFlag) return;
    if (visible && !m_preventPopup && !m_singleButton)
    {
//        QTimer::singleShot(400, this,SLOT(showPreview()));
        showPreview();
        /* for origin preview
        plugin()->willShowWindow(m_popup);
        m_popup->show();
        qDebug()<<"setPopupVisible ********";
        emit popupShown(this);*/
    }
    else
        m_popup->hide(fast);
}
/************************************************

 ************************************************/
void UKUITaskGroup::refreshIconsGeometry()
{
    float scale = qApp->devicePixelRatio();
    QRect rect = geometry();
    rect.moveTo(mapToGlobal(QPoint(0, 0)).x() * scale, mapToGlobal(QPoint(0, 0)).y() * scale);
    if (m_singleButton)
    {
        refreshIconGeometry(rect);
        return;
    }

    for(UKUITaskWidget *but : qAsConst(m_buttonHash))
    {
        but->refreshIconGeometry(rect);
//        but->setIconSize(QSize(plugin()->panel()->iconSize(), plugin()->panel()->iconSize()));
    }
}

/************************************************

 ************************************************/
QSize UKUITaskGroup::recalculateFrameSize()
{
    int height = 120;
    m_popup->setMaximumHeight(1000);
    m_popup->setMinimumHeight(0);

    int hh = recalculateFrameWidth();
    m_popup->setMaximumWidth(hh);
    m_popup->setMinimumWidth(0);

    QSize newSize(hh, height);
    m_popup->resize(newSize);

    return newSize;
}

int UKUITaskGroup::recalculateFrameWidth() const
{
    const QFontMetrics fm = fontMetrics();
    int max = 100 * fm.width (' '); // elide after the max width
    int txtWidth = 0;
//    for (UKUITaskButton *btn : qAsConst(m_buttonHash))
//        txtWidth = qMax(fm.width(btn->text()), txtWidth);
    return iconSize().width() + qMin(txtWidth, max) + 30/* give enough room to margins and borders*/;

}
void UKUITaskGroup::toDothis_customContextMenuRequested(const QPoint & pos)
{
    m_plugin->willShowWindow(m_menu);
    m_menu->popup(m_plugin->panel()->calculatePopupWindowPos(mapToGlobal({0, 0}), m_menu->sizeHint()).topLeft());
}
/************************************************

 ************************************************/
QPoint UKUITaskGroup::recalculateFramePosition()
{
    // Set position
    int x_offset = 0, y_offset = 0;
    switch (plugin()->panel()->position())
    {
    case IUKUIPanel::PositionTop:
        y_offset += height();
        break;
    case IUKUIPanel::PositionBottom:
        y_offset = -120;
        break;
    case IUKUIPanel::PositionLeft:
        x_offset += width();
        break;
    case IUKUIPanel::PositionRight:
        x_offset = -recalculateFrameWidth();
        break;
    }

    QPoint pos = mapToGlobal(QPoint(x_offset, y_offset));
    m_popup->move(pos);

    return pos;
}

void UKUITaskGroup::leaveEvent(QEvent *event)
{
    //QTimer::singleShot(300, this,SLOT(mouseLeaveOut()));
    m_taskGroupEvent = LEAVEEVENT;
    if (!m_statFlag) {
        update();
        return;
    }
    m_event = event;
    if(m_timer->isActive())
    {
        m_timer->stop();//stay time is no more than 400 ms need kill timer
    }
    else
    {
        m_timer->start(300);
    }
}

void UKUITaskGroup::enterEvent(QEvent *event)
{
    //QToolButton::enterEvent(event);
    m_taskGroupEvent = ENTEREVENT;
    if (!m_statFlag) {
        update();
        return;
    }
    m_event = event;
    m_timer->start(400);
}

void UKUITaskGroup::handleSavedEvent()
{
    if (m_draggging)
        return;
    if (!m_statFlag) return;
    if (m_statFlag && parentTaskBar()->isShowGroupOnHover())
    {
        setPopupVisible(true);
    }
    m_taskGroupStatus = HOVER;
    repaint();
    QToolButton::enterEvent(m_event);
}

void UKUITaskGroup::dragEnterEvent(QDragEnterEvent *event)
{
    // only show the popup if we aren't dragging a taskgroup
    if (!event->mimeData()->hasFormat(mimeDataFormat()))
    {
        setPopupVisible(true);
    }
    UKUITaskButton::dragEnterEvent(event);
}

void UKUITaskGroup::mouseReleaseEvent(QMouseEvent *event)
{
    // only show the popup if we aren't dragging a taskgroup

    UKUITaskButton::mouseReleaseEvent(event);
}

/************************************************

 ************************************************/
void UKUITaskGroup::dragLeaveEvent(QDragLeaveEvent *event)
{
    // if draggind something into the taskgroup or the taskgroups' popup,
    // do not close the popup
    if (!m_draggging)
        setPopupVisible(false);
    UKUITaskButton::dragLeaveEvent(event);
}

void UKUITaskGroup::mouseMoveEvent(QMouseEvent* event)
{
    // if dragging the taskgroup, do not show the popup
    setPopupVisible(false, true);
    UKUITaskButton::mouseMoveEvent(event);
}
/************************************************

 ************************************************/
bool UKUITaskGroup::onWindowChanged(WId window, NET::Properties prop, NET::Properties2 prop2)
{ // returns true if the class is preserved
    bool needsRefreshVisibility{false};
    QVector<QWidget *> buttons;
    if (m_buttonHash.contains(window))
        buttons.append(m_buttonHash.value(window));

    // If group is based on that window properties must be changed also on button group
    if (window == windowId())
        buttons.append(this);

    if (!buttons.isEmpty())
    {
        // if class is changed the window won't belong to our group any more
        if (parentTaskBar()->isGroupingEnabled() && prop2.testFlag(NET::WM2WindowClass))
        {
            KWindowInfo info(window, 0, NET::WM2WindowClass);
            if (info.windowClassClass() != m_groupName)
            {
                onWindowRemoved(window);
                return false;
            }
        }
        // window changed virtual desktop
        if (prop.testFlag(NET::WMDesktop) || prop.testFlag(NET::WMGeometry))
        {
            if (parentTaskBar()->isShowOnlyOneDesktopTasks()
                    || parentTaskBar()->isShowOnlyCurrentScreenTasks())
            {
                needsRefreshVisibility = true;
            }
        }

//        if (prop.testFlag(NET::WMVisibleName) || prop.testFlag(NET::WMName))
//            std::for_each(buttons.begin(), buttons.end(), std::mem_fn(&UKUITaskButton::updateText));

        // XXX: we are setting window icon geometry -> don't need to handle NET::WMIconGeometry
        // Icon of the button can be based on windowClass
//        if (prop.testFlag(NET::WMIcon) || prop2.testFlag(NET::WM2WindowClass))
//            std::for_each(buttons.begin(), buttons.end(), std::mem_fn(&UKUITaskButton::updateIcon));
        if (prop.testFlag(NET::WMIcon) || prop2.testFlag(NET::WM2WindowClass)){
            updateIcon();
            for(UKUITaskButtonHash::const_iterator i=m_visibleHash.begin();i!= m_visibleHash.end();i++)
                i.value()->updateIcon();

        }

        if (prop.testFlag(NET::WMState))
        {
            KWindowInfo info{window, NET::WMState};
            if (info.hasState(NET::SkipTaskbar))
                onWindowRemoved(window);
//            std::for_each(buttons.begin(), buttons.end(), std::bind(&UKUITaskButton::setUrgencyHint, std::placeholders::_1, info.hasState(NET::DemandsAttention)));

            if (parentTaskBar()->isShowOnlyMinimizedTasks())
            {
                needsRefreshVisibility = true;
            }
        }
    }
    if (needsRefreshVisibility)
        refreshVisibility();

    return true;
}

/************************************************

 ************************************************/
void UKUITaskGroup::groupPopupShown(UKUITaskGroup * const sender)
{
    //close all popups (should they be visible because of close delay)
    if (this != sender && isVisible())
            setPopupVisible(false, true/*fast*/);
}

void UKUITaskGroup::removeWidget()
{
    if(m_scrollArea)
    {
        removeSrollWidget();
    }
    if(m_widget)
    {
        m_popup->layout()->removeWidget(m_widget);
        QHBoxLayout *hLayout = dynamic_cast<QHBoxLayout*>(m_widget->layout());
        QVBoxLayout *vLayout = dynamic_cast<QVBoxLayout*>(m_widget->layout());
        if(hLayout != NULL)
        {
            hLayout->deleteLater();
            hLayout = NULL;
        }
        if(vLayout != NULL)
        {
            vLayout->deleteLater();
            vLayout = NULL;
        }
        //m_widget->setParent(NULL);
        m_widget->deleteLater();
        m_widget = NULL;
    }
}


void UKUITaskGroup::removeSrollWidget()
{
    if(m_scrollArea)
    {
        m_popup->layout()->removeWidget(m_scrollArea);
        m_popup->layout()->removeWidget(m_scrollArea->takeWidget());
    }
    if(m_widget)
    {
        m_popup->layout()->removeWidget(m_widget);
        QHBoxLayout *hLayout = dynamic_cast<QHBoxLayout*>(m_widget->layout());
        QVBoxLayout *vLayout = dynamic_cast<QVBoxLayout*>(m_widget->layout());
        if(hLayout != NULL)
        {
            hLayout->deleteLater();
            hLayout = NULL;
        }
        if(vLayout != NULL)
        {
            vLayout->deleteLater();
            vLayout = NULL;
        }
        //m_widget->setParent(NULL);
        m_widget->deleteLater();
        m_widget = NULL;
    }
    if(m_scrollArea)
    {
        m_scrollArea->deleteLater();
        m_scrollArea = NULL;
    }

}

void UKUITaskGroup::setLayOutForPostion()
{
    if(m_visibleHash.size() > 10)//more than 10 need
    {
        m_widget->setLayout(new QVBoxLayout);
        m_widget->layout()->setAlignment(Qt::AlignTop);
        m_widget->layout()->setSpacing(3);
        m_widget->layout()->setMargin(3);
        return;
    }

    if(plugin()->panel()->isHorizontal())
    {
        m_widget->setLayout(new QHBoxLayout);
        m_widget->layout()->setSpacing(3);
        m_widget->layout()->setMargin(3);
    }
    else
    {
        m_widget->setLayout(new QVBoxLayout);
        m_widget->layout()->setSpacing(3);
        m_widget->layout()->setMargin(3);
    }
}

bool UKUITaskGroup::isSetMaxWindow()
{
    int iScreenWidth = QApplication::screens().at(0)->size().width();
    int iScreenHeight = QApplication::screens().at(0)->size().height();
    if((iScreenWidth >= SCREEN_MID_WIDTH_SIZE)||((iScreenWidth > SCREEN_MAX_WIDTH_SIZE) && (iScreenHeight > SCREEN_MAX_HEIGHT_SIZE)))
    {
        return true;
    }
    else
    {
        return false;
    }
}

void UKUITaskGroup::showPreview()
{
    int n = 6;
    if (plugin()->panel()->isHorizontal()) n = 10;
    if(m_visibleHash.size() <= n)
    {
        showAllWindowByThumbnail();
    }
    else
    {
        showAllWindowByList();
    }
}

void UKUITaskGroup::adjustPopWindowSize(int winWidth, int winHeight)
{
    int size = m_visibleHash.size();
    if (m_isWaylandGroup) size = 1;
    if(plugin()->panel()->isHorizontal())
    {
        m_popup->setFixedSize(winWidth*size + (size + 1)*3, winHeight + 6);
    }
    else
    {
        m_popup->setFixedSize(winWidth + 6,winHeight*size + (size + 1)*3);
    }
    m_popup->adjustSize();
}

void UKUITaskGroup::v_adjustPopWindowSize(int winWidth, int winHeight, int v_all)
{
    int fixed_size = v_all;

    if(plugin()->panel()->isHorizontal())
    {
        int iScreenWidth = QApplication::screens().at(0)->size().width();
        if (fixed_size > iScreenWidth)
            fixed_size = iScreenWidth;
        m_popup->setFixedSize(fixed_size,  winHeight + 6);
    }
    else
    {
        int iScreenHeight = QApplication::screens().at(0)->size().height();
        if (fixed_size > iScreenHeight)
            fixed_size = iScreenHeight;
        m_popup->setFixedSize(winWidth + 6, fixed_size);
    }
    m_popup->adjustSize();
}

void UKUITaskGroup::timeout()
{

    if(m_taskGroupEvent == ENTEREVENT)
    {
        if(m_timer->isActive())
        {
            m_timer->stop();
        }
        handleSavedEvent();
    }
    else if(m_taskGroupEvent == LEAVEEVENT)
    {
        if(m_timer->isActive())
        {
            m_timer->stop();
        }
        setPopupVisible(false);
        QToolButton::leaveEvent(m_event);
        m_taskGroupStatus = NORMAL;
        update();
    }
    else
    {
        setPopupVisible(false);
    }
}

int UKUITaskGroup::calcAverageHeight()
{
    if(plugin()->panel()->isHorizontal())
    {
        return 0;
    }
    else
    {
        int size = m_visibleHash.size();
        int iScreenHeight = QApplication::screens().at(0)->size().height();
        int iMarginHeight = (size+1)*3;
        int iAverageHeight = (iScreenHeight - iMarginHeight)/size;//calculate average width of window
        return iAverageHeight;
    }
}

int UKUITaskGroup::calcAverageWidth()
{
    if(plugin()->panel()->isHorizontal())
    {
        int size = m_visibleHash.size();
        int iScreenWidth = QApplication::screens().at(0)->size().width();
        int iMarginWidth = (size+1)*3;
        int iAverageWidth;
        iAverageWidth =  (size == 0 ? size : (iScreenWidth - iMarginWidth)/size);//calculate average width of window
        return iAverageWidth;
    }
    else
    {
        return 0;
    }
}

void UKUITaskGroup::showAllWindowByList()
{
    int winWidth = 246;
    int winheight = 46;
    int iPreviewPosition = 0;
    int popWindowheight = (winheight) * (m_visibleHash.size());
    int screenAvailabelHeight = QApplication::screens().at(0)->size().height() - plugin()->panel()->panelSize();
    if(!plugin()->panel()->isHorizontal())
    {
        screenAvailabelHeight = QApplication::screens().at(0)->size().height();//panel is vect
    }
    if(m_popup->layout()->count() > 0)
    {
        removeSrollWidget();
    }
    m_scrollArea = new QScrollArea(this);

    m_scrollArea->setWidgetResizable(true);

    m_popup->layout()->addWidget(m_scrollArea);
    m_popup->setFixedSize(winWidth,  popWindowheight < screenAvailabelHeight? popWindowheight : screenAvailabelHeight);
    m_widget = new QWidget(this);
    m_scrollArea->setWidget(m_widget);
    setLayOutForPostion();
    /*begin catch preview picture*/
    for (QVector<UKUITaskWidget*>::iterator it = m_showInTurn.begin();it != m_showInTurn.end();it++)
    {
        UKUITaskWidget *btn = *it;
        btn->clearMask();
        btn->setTitleFixedWidth(m_widget->width());
        btn->updateTitle();
        btn->setParent(m_scrollArea);
        btn->removeThumbNail();
        btn->addThumbNail();
        btn->adjustSize();
        btn->setFixedHeight(winheight);

        connect(btn, &UKUITaskWidget::closeSigtoPop, [this] { m_popup->pubcloseWindowDelay(); });
        connect(btn, &UKUITaskWidget::closeSigtoGroup, [this] { closeGroup(); });
        m_widget->layout()->addWidget(btn);
    }
    /*end*/
    plugin()->willShowWindow(m_popup);
    if(plugin()->panel()->isHorizontal())
    {
        iPreviewPosition =  plugin()->panel()->panelSize()/2 - winWidth/2;
        m_popup->setGeometry(plugin()->panel()->calculatePopupWindowPos(mapToGlobal(QPoint(iPreviewPosition,0)), m_popup->size()));
    }
    else
    {
        iPreviewPosition = plugin()->panel()->panelSize()/2 - winWidth/2;
        m_popup->setGeometry(plugin()->panel()->calculatePopupWindowPos(mapToGlobal(QPoint(0,iPreviewPosition)), m_popup->size()));
    }

    m_popup->setStyle(new CustomStyle());
    m_scrollArea->setAttribute(Qt::WA_TranslucentBackground);
    m_scrollArea->setProperty("drawScrollBarGroove",false);
    m_scrollArea->verticalScrollBar()->setProperty("drawScrollBarGroove",false);
    m_scrollArea->show();
    m_popup->show();

   // emit popupShown(this);
}


void UKUITaskGroup::showAllWindowByThumbnail()
{
    if (m_isWaylandGroup) {
        int previewPosition = 0;
        m_widget = new QWidget();
        m_widget->setAttribute(Qt::WA_TranslucentBackground);
        for (UKUITaskButtonHash::const_iterator it = m_buttonHash.begin();it != m_buttonHash.end();it++)
        {
            QPixmap thumbnail;
            UKUITaskWidget *btn = it.value();
            thumbnail = QIcon::fromTheme(m_iconName).pixmap(THUMBNAIL_WIDTH, THUMBNAIL_HEIGHT - 160);
            btn->setThumbNail(thumbnail);
            btn->wl_updateTitle(m_caption);
            btn->wl_updateIcon(m_iconName);
            btn->setFixedSize(THUMBNAIL_WIDTH, THUMBNAIL_HEIGHT - 160);
            //m_widget->layout()->setContentsMargins(0,0,0,0);
            //m_widget->layout()->addWidget(btn);
        }
        adjustPopWindowSize(THUMBNAIL_WIDTH, THUMBNAIL_HEIGHT - 160);
        if(plugin()->panel()->isHorizontal())//set preview window position
        {
            if(m_popup->size().width()/2 < QCursor::pos().x())
            {
                previewPosition = 0 - m_popup->size().width()/2 + plugin()->panel()->panelSize()/2;
            }
            else
            {
                previewPosition = 0 -(QCursor::pos().x() + plugin()->panel()->panelSize()/2);
            }
            m_popup->setGeometry(plugin()->panel()->calculatePopupWindowPos(mapToGlobal(QPoint(previewPosition,0)), m_popup->size()));
        }
        else
        {
            if(m_popup->size().height()/2 < QCursor::pos().y())
            {
                previewPosition = 0 - m_popup->size().height()/2 + plugin()->panel()->panelSize()/2;
            }
            else
            {
                previewPosition = 0 -(QCursor::pos().y() + plugin()->panel()->panelSize()/2);
            }
            m_popup->setGeometry(plugin()->panel()->calculatePopupWindowPos(mapToGlobal(QPoint(0,previewPosition)), m_popup->size()));
        }
        if(m_popup->isVisible())
        {
    //        m_popup-
        }
        else
        {
            m_popup->show();
        }
        return;
    }

    XImage *img = NULL;
    Display *display = NULL;
    QPixmap thumbnail;
    XWindowAttributes attr;
    int previewPosition = 0;
    int winWidth = 0;
    int winHeight = 0;
    int truewidth = 0;
   // initVisibleHash();
    refreshVisibility();
    int iAverageWidth = calcAverageWidth();
    int iAverageHeight = calcAverageHeight();
    /*begin get the winsize*/
    bool isMaxWinSize = isSetMaxWindow();
    if(isMaxWinSize)
    {
        if(0 == iAverageWidth)
        {
            winHeight = PREVIEW_WIDGET_MAX_HEIGHT < iAverageHeight?PREVIEW_WIDGET_MAX_HEIGHT:iAverageHeight;
            winWidth = winHeight*PREVIEW_WIDGET_MAX_WIDTH/PREVIEW_WIDGET_MAX_HEIGHT;
        }
        else
        {
            winWidth = PREVIEW_WIDGET_MAX_WIDTH < iAverageWidth?PREVIEW_WIDGET_MAX_WIDTH:iAverageWidth;
            winHeight = winWidth*PREVIEW_WIDGET_MAX_HEIGHT/PREVIEW_WIDGET_MAX_WIDTH;
        }
    }
    else
    {
        if(0 == iAverageWidth)
        {
            winHeight = PREVIEW_WIDGET_MIN_HEIGHT < iAverageHeight?PREVIEW_WIDGET_MIN_HEIGHT:iAverageHeight;
            winWidth = winHeight*PREVIEW_WIDGET_MIN_WIDTH/PREVIEW_WIDGET_MIN_HEIGHT;
        }
        else
        {
            winWidth = PREVIEW_WIDGET_MIN_WIDTH < iAverageWidth?PREVIEW_WIDGET_MIN_WIDTH:iAverageWidth;
            winHeight = winWidth*PREVIEW_WIDGET_MIN_HEIGHT/PREVIEW_WIDGET_MIN_WIDTH;
        }
    }
    /*end get the winsize*/

    if(m_popup->layout()->count() > 0)
    {
        removeWidget();
    }
    m_widget = new QWidget(this);
    m_widget->setAttribute(Qt::WA_TranslucentBackground);
    setLayOutForPostion();
    /*begin catch preview picture*/

    int max_Height = 0;
    int max_Width = 0;
    int imgWidth_sum = 0;
    int changed = 0;
    int title_width = 0;
    int v_all = 0;
    int iScreenWidth = QApplication::screens().at(0)->size().width();
    float minimumHeight = THUMBNAIL_HEIGHT;
    for (UKUITaskButtonHash::const_iterator it = m_visibleHash.begin();it != m_visibleHash.end();it++)
    {
        it.value()->removeThumbNail();
        display = XOpenDisplay(nullptr);
        XGetWindowAttributes(display, it.key(), &attr);
        max_Height = attr.height > max_Height ? attr.height : max_Height;
        max_Width = attr.width > max_Width ? attr.width : max_Width;
        truewidth += attr.width;
        if(display)
            XCloseDisplay(display);
    }
    for (UKUITaskButtonHash::const_iterator it = m_buttonHash.begin();it != m_buttonHash.end();it++)
    {
        UKUITaskWidget *btn = it.value();
        btn->setParent(m_popup);
        connect(btn, &UKUITaskWidget::closeSigtoPop, [this] { m_popup->pubcloseWindowDelay(); });
        connect(btn, &UKUITaskWidget::closeSigtoGroup, [this] { closeGroup(); });
        btn->addThumbNail();
        display = XOpenDisplay(nullptr);
        XGetWindowAttributes(display, it.key(), &attr);
        img = XGetImage(display, it.key(), 0, 0, attr.width, attr.height, 0xffffffff,ZPixmap);
        float imgWidth = 0;
        float imgHeight = 0;
        if (plugin()->panel()->isHorizontal()) {
            float thmbwidth = (float)attr.width / (float)attr.height;
            imgWidth = thmbwidth * winHeight;
            imgHeight = winHeight;
            if (imgWidth > THUMBNAIL_WIDTH)
                imgWidth = THUMBNAIL_WIDTH;
        } else {
            imgWidth = THUMBNAIL_WIDTH;
            imgHeight = (float)attr.height / (float)attr.width * THUMBNAIL_WIDTH;
        }
        if (plugin()->panel()->isHorizontal())
        {
            if (m_visibleHash.contains(btn->windowId())) {
                v_all += (int)imgWidth;
                imgWidth_sum += (int)imgWidth;
            }
            if (m_visibleHash.size() == 1 ) {
                changed = (int)imgWidth;
            }
            btn->setThumbMaximumSize(MAX_SIZE_OF_Thumb);
            btn->setThumbScale(true);
        } else {
            if (attr.width != max_Width)
            {
                float tmp = (float)attr.width / (float)max_Width;
                imgWidth =  imgWidth * tmp;
            }
            if ((int)imgHeight > (int)minimumHeight)
            {
                imgHeight = minimumHeight;
            }
            if (m_visibleHash.contains(btn->windowId())) {
                v_all += (int)imgHeight;
            }
            if (m_visibleHash.size() == 1 ) changed = (int)imgHeight;
            if ((int)imgWidth < 150)
            {
                btn->setThumbFixedSize((int)imgWidth);
                btn->setThumbScale(false);
            } else {
                btn->setThumbMaximumSize(MAX_SIZE_OF_Thumb);
                btn->setThumbScale(true);
            }
        }
        if(img)
        {
            thumbnail = qimageFromXImage(img).scaled((int)imgWidth, (int)imgHeight, Qt::KeepAspectRatio,Qt::SmoothTransformation);
            if (!parentTaskBar()->getCpuInfoFlg()) thumbnail.save(QString("/tmp/%1.png").arg(it.key()));
        }
        else
        {
            qDebug()<<"can not catch picture";
            QPixmap pxmp;
            if (pxmp.load(QString("/tmp/%1.png").arg(it.key())))
                thumbnail = pxmp.scaled((int)imgWidth, (int)imgHeight, Qt::KeepAspectRatio,Qt::SmoothTransformation);
            else {
                thumbnail = QPixmap((int)imgWidth, (int)imgHeight);
                thumbnail.fill(QColor(0, 0, 0, 127));
            }
        }
        btn->setThumbNail(thumbnail);
        btn->updateTitle();
        btn->setFixedSize((int)imgWidth, (int)imgHeight);
        m_widget->layout()->setContentsMargins(0,0,0,0);
        m_widget->layout()->addWidget(btn);
        if(img)
        {
           XDestroyImage(img);
        }
        if(display)
        {
            XCloseDisplay(display);
        }
    }
    /*end*/
        for (UKUITaskButtonHash::const_iterator it = m_buttonHash.begin();it != m_buttonHash.end();it++)
        {
            UKUITaskWidget *btn = it.value();
            if (plugin()->panel()->isHorizontal())  {
                if (imgWidth_sum > iScreenWidth)
                    title_width = (int)(btn->width()  * iScreenWidth / imgWidth_sum - 80);
                else
                    title_width = btn->width() - 75;
            } else {
                 title_width = winWidth- 70;
            }
            btn->setTitleFixedWidth(title_width);
        }
    plugin()->willShowWindow(m_popup);
    m_popup->layout()->addWidget(m_widget);
    if (m_visibleHash.size() == 1 && changed != 0)
        if (plugin()->panel()->isHorizontal()) {
            adjustPopWindowSize(changed, winHeight);
        } else {
            adjustPopWindowSize(winWidth, changed);
        }
    else if (m_visibleHash.size() != 1)
        v_adjustPopWindowSize(winWidth, winHeight, v_all);
    else
        adjustPopWindowSize(winWidth, winHeight);

    if(plugin()->panel()->isHorizontal())//set preview window position
    {
        if(m_popup->size().width()/2 < QCursor::pos().x())
        {
            previewPosition = 0 - m_popup->size().width()/2 + plugin()->panel()->panelSize()/2;
        }
        else
        {
            previewPosition = 0 -(QCursor::pos().x() + plugin()->panel()->panelSize()/2);
        }
        m_popup->setGeometry(plugin()->panel()->calculatePopupWindowPos(mapToGlobal(QPoint(previewPosition,0)), m_popup->size()));
    }
    else
    {
        if(m_popup->size().height()/2 < QCursor::pos().y())
        {
            previewPosition = 0 - m_popup->size().height()/2 + plugin()->panel()->panelSize()/2;
        }
        else
        {
            previewPosition = 0 -(QCursor::pos().y() + plugin()->panel()->panelSize()/2);
        }
        m_popup->setGeometry(plugin()->panel()->calculatePopupWindowPos(mapToGlobal(QPoint(0,previewPosition)), m_popup->size()));
    }
    if(m_popup->isVisible())
    {
//        m_popup-
    }
    else
    {
        m_popup->show();
    }
//   emit popupShown(this);
}

void UKUITaskGroup::setActivateState_wl(bool _state) {
    m_isWinActivate = _state;
    m_taskGroupStatus = (_state ? HOVER : NORMAL);
    repaint();
}

void UKUITaskGroup::winClickActivate_wl(bool _getActive) {
    QDBusMessage message = QDBusMessage::createSignal("/", "com.ukui.kwin", "request");
    QList<QVariant> args;
    quint32 m_wid=windowId();
    args.append(m_wid);
    args.append((_getActive ? WAYLAND_GROUP_ACTIVATE : WAYLAND_GROUP_HIDE));
    m_isWinActivate = _getActive;
    m_taskGroupStatus = (_getActive ? HOVER : NORMAL);
    repaint();
    message.setArguments(args);
    QDBusConnection::sessionBus().send(message);
}

QWidget * UKUITaskGroup::wl_addWindow(WId id)
{

    if (m_buttonHash.contains(id))
        return m_buttonHash.value(id);
    UKUITaskWidget *btn;
    if (m_isWaylandGroup) {
        btn = new UKUITaskWidget(QString("kyiln-video"), id, parentTaskBar(), m_popup);
        m_buttonHash.insert(id, btn);
        return btn;
    }
    else
        btn = new UKUITaskWidget(id, parentTaskBar(), m_popup);
    m_buttonHash.insert(id, btn);
    connect(btn, SIGNAL(clicked()), this, SLOT(onClicked(bool)));
    connect(btn, SIGNAL(windowMaximize()), this, SLOT(onChildButtonClicked()));

    this->setStyle(new CustomStyle("taskbutton",true));
    return btn;
}

void UKUITaskGroup::closeGroup_wl() {
    QDBusMessage message = QDBusMessage::createSignal("/", "com.ukui.kwin", "request");
    QList<QVariant> args;
    quint32 m_wid=windowId();
    args.append(m_wid);
    args.append(WAYLAND_GROUP_CLOSE);
    message.setArguments(args);
    QDBusConnection::sessionBus().send(message);
}

void UKUITaskGroup::wl_widgetUpdateTitle(QString caption) {
    if (caption.isNull())
        return;
    for (UKUITaskWidget *button : qAsConst(m_buttonHash) )
        button->wl_updateTitle(caption);
}
