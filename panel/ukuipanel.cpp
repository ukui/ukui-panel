/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.com>
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


#include "ukuipanel.h"
#include "common/common.h"
#include "iukuipanelplugin.h"
#include "ukuipanelapplication.h"
#include "ukuipanellayout.h"
#include "plugin.h"
#include "panelpluginsmodel.h"
#include "windownotifier.h"
#include "common/ukuiplugininfo.h"

#include <QScreen>
#include <QWindow>
#include <QtX11Extras/QX11Info>
#include <QDebug>
#include <QString>
#include <QDesktopWidget>
#include <QMenu>
#include <QMessageBox>
#include <QDropEvent>
#include <XdgIcon>
#include <XdgDirs>
#include <QPainter>
#include <QMetaEnum>
#include <QtDBus>

#include <KWindowSystem/KWindowSystem>
#include <KWindowSystem/NETWM>
//#include <glib.h>
//#include <gio/gio.h>
#include <QGSettings>
// Turn on this to show the time required to load each plugin during startup
// #define DEBUG_PLUGIN_LOADTIME

#include "common_fun/ukuipanel_infomation.h"
#include "common_fun/dbus-adaptor.h"
#include "common_fun/panel_commission.h"

#ifdef DEBUG_PLUGIN_LOADTIME
#include <QElapsedTimer>
#endif


/************************************************
 Returns the Position by the string.
 String is one of "Top", "Left", "Bottom", "Right", string is not case sensitive.
 If the string is not correct, returns defaultValue.
 ************************************************/
IUKUIPanel::Position UKUIPanel::strToPosition(const QString& str, IUKUIPanel::Position defaultValue)
{
    if (str.toUpper() == "TOP")    return UKUIPanel::PositionTop;
    if (str.toUpper() == "LEFT")   return UKUIPanel::PositionLeft;
    if (str.toUpper() == "RIGHT")  return UKUIPanel::PositionRight;
    if (str.toUpper() == "BOTTOM") return UKUIPanel::PositionBottom;
    return defaultValue;
}

IUKUIPanel::Position UKUIPanel::intToPosition(const int position, IUKUIPanel::Position defaultValue)
{
    if (position == 1)    return UKUIPanel::PositionTop;
    if (position == 2)   return UKUIPanel::PositionLeft;
    if (position == 3)  return UKUIPanel::PositionRight;
    if (position == 0) return UKUIPanel::PositionBottom;
    return defaultValue;
}

/************************************************
 Return  string representation of the position
 *******************************************connect(QApplication::desktop(), &QDesktopWidget::resized, this, &UKUIPanel::ensureVisible);*****/
QString UKUIPanel::positionToStr(IUKUIPanel::Position position)
{
    switch (position)
    {
    case UKUIPanel::PositionTop:
        return QString("Top");
    case UKUIPanel::PositionLeft:
        return QString("Left");
    case UKUIPanel::PositionRight:
        return QString("Right");
    case UKUIPanel::PositionBottom:
        return QString("Bottom");
    }

    return QString();
}


/************************************************

 ************************************************/
UKUIPanel::UKUIPanel(const QString &configGroup, UKUi::Settings *settings, QWidget *parent) :
    QFrame(parent),
    m_settings(settings),
    m_configGroup(configGroup),
    m_plugins{nullptr},
    m_standaloneWindows{new WindowNotifier},
    m_panelSize(46),
    m_iconSize(32),
    m_lineCount(0),
    m_length(0),
    m_alignment(AlignmentLeft),
    m_position(IUKUIPanel::PositionBottom),
    m_screenNum(0), //whatever (avoid conditional on uninitialized value)
    m_actualScreenNum(0),
    m_hidable(false),
    m_visibleMargin(true),
    m_hidden(false),
    m_animationTime(0),
    m_reserveSpace(true),
    m_animation(nullptr),
    m_lockPanel(false)
{
    qDebug()<<"Panel :: Constructor start";
    //You can find information about the flags and widget attributes in your
    //Qt documentation or at https://doc.qt.io/qt-5/qt.html
    //Qt::FramelessWindowHint = Produces a borderless window. The user cannot
    //move or resize a borderless window via the window system. On X11, ...
    //Qt::WindowStaysOnTopHint = Informs the window system that the window
    //should stay on top of all other windows. Note that on ...
    Qt::WindowFlags flags = Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint;

    // NOTE: by PCMan:
    // In Qt 4, the window is not activated if it has Qt::WA_X11NetWmWindowTypeDock.
    // Since Qt 5, the default behaviour is changed. A window is always activated on mouse click.
    // Please see the source code of Qt5: src/plugins/platforms/xcb/qxcbwindow.cpp.
    // void QXcbWindow::handleButtonPressEvent(const xcb_button_press_event_t *event)
    // This new behaviour caused ukui bug #161 - Cannot minimize windows from panel 1 when two task managers are open
    // Besides, this breaks minimizing or restoring windows when clicking on the taskbar buttons.
    // To workaround this regression bug, we need to add this window flag here.
    // However, since the panel gets no keyboard focus, this may decrease accessibility since
    // it's not possible to use the panel with keyboards. We need to find a better solution later.

    /*部分组建在点击任务栏空白位置的时候，无法收回窗口，想要正常收回窗口，需要取消下面的窗口属性或者其他应用监听点击taskbar的点击信号
     * 不使用此窗口属性则需要在开始菜单，任务视图，快速启动栏三个界面组件中设置　setFocusPolicy(Qt::NoFocus);
     */
    flags |= Qt::WindowDoesNotAcceptFocus;

    setWindowFlags(flags);
    //Adds _NET_WM_WINDOW_TYPE_DOCK to the window's _NET_WM_WINDOW_TYPE X11 window property. See https://standards.freedesktop.org/wm-spec/ for more details.
    setAttribute(Qt::WA_X11NetWmWindowTypeDock);
    //Enables tooltips for inactive windows.
    setAttribute(Qt::WA_AlwaysShowToolTips);
    //Indicates that the widget should have a translucent background, i.e., any non-opaque regions of the widgets will be translucent because the widget will have an alpha channel. Setting this ...
    setAttribute(Qt::WA_TranslucentBackground);
    //Allows data from drag and drop operations to be dropped onto the widget (see QWidget::setAcceptDrops()).
    setAttribute(Qt::WA_AcceptDrops);

    setWindowTitle("UKUI Panel");
    setObjectName(QString("UKUIPanel %1").arg(configGroup));
    qDebug()<<"Panel :: UKuiPanel setAttribute finished";

    //初始化参数调整
    PanelCommission::panelConfigFileValueInit(true);
    PanelCommission::panelConfigFileReset(true);
    qDebug()<<"Panel :: PanelCommission config finished";


    //UKUIPanel (inherits QFrame) -> lav (QGridLayout) -> UKUIPanelWidget (QFrame) -> UKUIPanelLayout
    UKUIPanelWidget = new QFrame(this);
    UKUIPanelWidget->setObjectName("BackgroundWidget");
    QGridLayout* lav = new QGridLayout();
    lav->setContentsMargins(0, 0, 0, 0);
    setLayout(lav);
    this->layout()->addWidget(UKUIPanelWidget);

    m_layout = new UKUIPanelLayout(UKUIPanelWidget);
    connect(m_layout, &UKUIPanelLayout::pluginMoved, this, &UKUIPanel::pluginMoved);
    UKUIPanelWidget->setLayout(m_layout);
    m_layout->setLineCount(m_lineCount);

    mDelaySave.setSingleShot(true);
    mDelaySave.setInterval(SETTINGS_SAVE_DELAY);
    connect(&mDelaySave, SIGNAL(timeout()), this, SLOT(saveSettings()));

    m_hideTimer.setSingleShot(true);
    m_hideTimer.setInterval(PANEL_HIDE_DELAY);
    connect(&m_hideTimer, SIGNAL(timeout()), this, SLOT(hidePanelWork()));

    connectToServer();
    m_showDelayTimer.setSingleShot(true);
    m_showDelayTimer.setInterval(PANEL_SHOW_DELAY);
    connect(&m_showDelayTimer, &QTimer::timeout, [this] { showPanel(m_animationTime > 0); });

        /* 监听屏幕分辨路改变resized　和屏幕数量改变screenCountChanged
         * 或许存在无法监听到分辨率改变的情况（qt5.6），若出现则可换成
         * connect(QApplication::primaryScreen(),&QScreen::geometryChanged, this,&UKUIPanel::ensureVisible);
         */
    connect(QApplication::desktop(), &QDesktopWidget::resized, this, &UKUIPanel::ensureVisible);
    connect(QApplication::desktop(), &QDesktopWidget::screenCountChanged, this, &UKUIPanel::ensureVisible);
    connect(qApp,&QApplication::primaryScreenChanged,this,&UKUIPanel::ensureVisible);

    // connecting to QDesktopWidget::workAreaResized shouldn't be necessary,
    // as we've already connceted to QDesktopWidget::resized, but it actually
    connect(QApplication::desktop(), &QDesktopWidget::workAreaResized,
            this, &UKUIPanel::ensureVisible);
    UKUIPanelApplication *a = reinterpret_cast<UKUIPanelApplication*>(qApp);
    connect(a, &UKUIPanelApplication::primaryScreenChanged, [=]{
        setPanelGeometry();
    });

    connect(UKUi::Settings::globalSettings(), SIGNAL(settingsChanged()), this, SLOT(update()));

    connect(m_standaloneWindows.data(), &WindowNotifier::firstShown, [this] { showPanel(true); });
    connect(m_standaloneWindows.data(), &WindowNotifier::lastHidden, this, &UKUIPanel::hidePanel);

    const QByteArray id(PANEL_SETTINGS);
    m_gsettings = new QGSettings(id);
    setPosition(0,intToPosition(m_gsettings->get(PANEL_POSITION_KEY).toInt(),PositionBottom),true);

    connect(m_gsettings, &QGSettings::changed, this, [=] (const QString &key){
        if(key==ICON_SIZE_KEY) {
            setIconSize(m_gsettings->get(ICON_SIZE_KEY).toInt(),true);
        }
        if(key==PANEL_SIZE_KEY) {
            setPanelSize(m_gsettings->get(PANEL_SIZE_KEY).toInt(),true);
        }
        if(key == PANEL_POSITION_KEY) {
            setPosition(0,intToPosition(m_gsettings->get(PANEL_POSITION_KEY).toInt(),PositionBottom),true);
        }
    });

    setPanelSize(m_gsettings->get(PANEL_SIZE_KEY).toInt(),true);
    setIconSize(m_gsettings->get(ICON_SIZE_KEY).toInt(),true);
    setPosition(0,intToPosition(m_gsettings->get(PANEL_POSITION_KEY).toInt(),PositionBottom),true);

    m_time = new QTimer(this);
    connect(m_time, &QTimer::timeout, this,[=] (){
        m_showDelayTimer.stop();
        hidePanel();
        m_time->stop();
    });
    qDebug()<<"Panel :: setGeometry finished";

    readSettings();

    ensureVisible();
	
    qDebug()<<"Panel :: loadPlugins start";
    loadPlugins();
    qDebug()<<"Panel :: loadPlugins finished";

    m_interface = new QDBusInterface("com.kylin.statusmanager.interface","/",
                                    "com.kylin.statusmanager.interface",
                                    QDBusConnection::sessionBus(),this);

    if (m_interface->isValid()) {
        QDBusReply<bool> status = m_interface->call("get_current_tabletmode");
        setPanelHide(status);
        QDBusConnection::sessionBus().connect("com.kylin.statusmanager.interface","/",
                                 "com.kylin.statusmanager.interface","mode_change_signal",
                                 this,SLOT(setPanelHide(bool)));
    } else {
        setPanelHide(false);
    }
    qDebug()<<"Panel :: show UKuiPanel finished";	
    //给session发信号，告知任务栏已经启动完成，可以启动下一个组件
    QDBusInterface interface(UKUI_SERVICE,
                             UKUI_PATH,
                             UKUI_INTERFACE,
                             QDBusConnection::sessionBus());
    interface.call("startupfinished","ukui-panel","finish");
    // show it the first time, despite setting
    if (m_hidable) {
        showPanel(false);
        QTimer::singleShot(PANEL_HIDE_FIRST_TIME, this, SLOT(hidePanel()));
    }

    styleAdjust();
    qDebug()<<"Panel :: UKuiPanel  finished";

    UKuiPanelInformation* dbus=new UKuiPanelInformation;
    new PanelAdaptor(dbus);
    QDBusConnection con=QDBusConnection::sessionBus();
    if(!con.registerService("org.ukui.panel") ||
            !con.registerObject("/panel/position",dbus)) {
        qDebug()<<"fail";
    }
}

void UKUIPanel::setPanelHide(bool model)
{
    if (model) {
        hide();
    } else {
        show();
    }
}

void UKUIPanel::readSettings()
{
    // Read settings ......................................
    m_settings->beginGroup(m_configGroup);

    // Let Hidability be the first thing we read
    // so that every call to realign() is without side-effect
    m_hidable = m_settings->value(CFG_KEY_HIDABLE, m_hidable).toBool();
    m_hidden = m_hidable;
    m_visibleMargin = m_settings->value(CFG_KEY_VISIBLE_MARGIN, m_visibleMargin).toBool();
    m_animationTime = m_settings->value(CFG_KEY_ANIMATION, m_animationTime).toInt();
    m_showDelayTimer.setInterval(m_settings->value(CFG_KEY_SHOW_DELAY, m_showDelayTimer.interval()).toInt());

    // By default we are using size & count from theme.
    setLineCount(m_settings->value(CFG_KEY_LINECNT, PANEL_DEFAULT_LINE_COUNT).toInt(), false);

    setLength(m_settings->value(CFG_KEY_LENGTH, 100).toInt(),
              m_settings->value(CFG_KEY_PERCENT, true).toBool(),
              false);
    m_screenNum = m_settings->value(CFG_KEY_SCREENNUM, QApplication::desktop()->primaryScreen()).toInt();
//    setPosition(mScreenNum,
//                strToPosition(mSettings->value(CFG_KEY_POSITION).toString(), PositionBottom),
//                false);
    setAlignment(Alignment(m_settings->value(CFG_KEY_ALIGNMENT, m_alignment).toInt()), false);
    m_reserveSpace = m_settings->value(CFG_KEY_RESERVESPACE, true).toBool();
    m_lockPanel = m_settings->value(CFG_KEY_LOCKPANEL, false).toBool();

    m_settings->endGroup();

}


/************************************************

 ************************************************/
void UKUIPanel::saveSettings(bool later)
{
    mDelaySave.stop();
    if (later) {
        mDelaySave.start();
        return;
    }

    m_settings->beginGroup(m_configGroup);

    //Note: save/load of plugin names is completely handled by mPlugins object
    //mSettings->setValue(CFG_KEY_PLUGINS, mPlugins->pluginNames());

    m_settings->setValue(CFG_KEY_LINECNT, m_lineCount);

    m_settings->setValue(CFG_KEY_LENGTH, m_length);
    m_settings->setValue(CFG_KEY_PERCENT, m_lengthInPercents);

    m_settings->setValue(CFG_KEY_SCREENNUM, m_screenNum);
//    mSettings->setValue(CFG_KEY_POSITION, positionToStr(mPosition));

    m_settings->setValue(CFG_KEY_ALIGNMENT, m_alignment);

    m_settings->setValue(CFG_KEY_RESERVESPACE, m_reserveSpace);

    m_settings->setValue(CFG_KEY_HIDABLE, m_hidable);
    m_settings->setValue(CFG_KEY_VISIBLE_MARGIN, m_visibleMargin);
    m_settings->setValue(CFG_KEY_ANIMATION, m_animationTime);
    m_settings->setValue(CFG_KEY_SHOW_DELAY, m_showDelayTimer.interval());

    m_settings->setValue(CFG_KEY_LOCKPANEL, m_lockPanel);

    m_settings->endGroup();
}


/*确保任务栏在调整分辨率和增加·屏幕之后能保持显示正常*/
void UKUIPanel::ensureVisible()
{
    if (!canPlacedOn(m_screenNum, m_position))
        setPosition(findAvailableScreen(m_position), m_position, false);
    else
        m_actualScreenNum = m_screenNum;

    // the screen size might be changed
    realign();
}


UKUIPanel::~UKUIPanel()
{
    m_layout->setEnabled(false);
    delete m_animation;
    delete m_interface;
//    delete mConfigDialog.data();
    // do not save settings because of "user deleted panel" functionality saveSettings();
}


void UKUIPanel::show()
{
    QWidget::show();
    KWindowSystem::setOnDesktop(effectiveWinId(), NET::OnAllDesktops);
}


QStringList pluginDesktopDirs()
{
    QStringList dirs;
    dirs << QString(getenv("UKUI_PANEL_PLUGINS_DIR")).split(':', QString::SkipEmptyParts);
    dirs << QString("%1/%2").arg(XdgDirs::dataHome(), "/ukui/ukui-panel");
    dirs << PLUGIN_DESKTOPS_DIR;
    return dirs;
}

/*加载pluginDesktopDirs 获取到的列表中的插件*/
void UKUIPanel::loadPlugins()
{
    QString names_key(m_configGroup);
    names_key += '/';
    names_key += QLatin1String(CFG_KEY_PLUGINS);
    m_plugins.reset(new PanelPluginsModel(this, names_key, pluginDesktopDirs()));

    connect(m_plugins.data(), &PanelPluginsModel::pluginAdded, m_layout, &UKUIPanelLayout::addPlugin);
    connect(m_plugins.data(), &PanelPluginsModel::pluginMovedUp, m_layout, &UKUIPanelLayout::moveUpPlugin);
    //reemit signals
    connect(m_plugins.data(), &PanelPluginsModel::pluginAdded, this, &UKUIPanel::pluginAdded);
    connect(m_plugins.data(), &PanelPluginsModel::pluginRemoved, this, &UKUIPanel::pluginRemoved);

    const auto plugins = m_plugins->plugins();
    for (auto const & plugin : plugins) {
        m_layout->addPlugin(plugin);
        connect(plugin, &Plugin::dragLeft, [this] { m_showDelayTimer.stop(); hidePanel();});
    }
}

void UKUIPanel::reloadPlugins(QString model){

    QStringList list=readConfig(model);
    checkPlugins(list);
    movePlugins(list);
}

QStringList UKUIPanel::readConfig(QString model){

    QStringList list;
    m_settings->beginGroup(m_configGroup);
    if(model=="pc") {
        list = m_settings->value(CFG_KEY_PLUGINS_PC).toStringList();
    } else {
        list = m_settings->value(CFG_KEY_PLUGINS_PAD).toStringList();
    }
    m_settings->endGroup();
    return list;
}

void UKUIPanel::checkPlugins(QStringList list){

    const auto plugins = m_plugins->plugins();
    for (auto const & plugin : plugins) {
        plugin->hide();
    }

    for(int i=0;i<list.size();i++) {
        if(m_plugins->pluginNames().contains(list[i])) {
            if(m_plugins->pluginByName(list[i])) {
                m_plugins->pluginByName(list[i])->show();
            }
        }
    }
}

void UKUIPanel::movePlugins(QStringList list)
{
    for(int i=0;i<m_layout->count();i++) {
        m_layout->removeItem(m_layout->itemAt(i));
    }
    const auto plugins = m_plugins->plugins();
    for (auto const & plugin : plugins) {
        if(!plugin->isHidden()) {
            m_layout->addWidget(plugin);
        }
    }
}


int UKUIPanel::getReserveDimension()
{
    return m_hidable ? PANEL_HIDE_SIZE : qMax(PANEL_MINIMUM_SIZE, m_panelSize);
}

/* get primary screen changed in  990*/
void UKUIPanel::priScreenChanged(int x, int y, int width, int height)
{

    m_currentScreenRect.setRect(x, y, width, height);
    setPanelGeometry();
    realign();
}

/*
 The setting frame of the old panel does not follow the main screen
 but can be displayed on any screen
 but the current desktop environment of ukui is set to follow the main screen
 All default parameters desktop()->screenGeometry are 0
 */
void UKUIPanel::setPanelGeometry(bool animate)
{
    QRect currentScreen;
    QRect rect;

    currentScreen=QGuiApplication::screens().at(0)->geometry();

    if (isHorizontal()) {
        rect.setHeight(qMax(PANEL_MINIMUM_SIZE, m_panelSize));
        if (m_lengthInPercents)
            rect.setWidth(currentScreen.width() * m_length / 100.0);
        else{
            if (m_length <= 0)
                rect.setWidth(currentScreen.width() + m_length);
            else
                rect.setWidth(m_length);
        }
        rect.setWidth(qMax(rect.size().width(), m_layout->minimumSize().width()));

        // Horiz ......................
        switch (m_alignment)
        {
        case UKUIPanel::AlignmentLeft:
            rect.moveLeft(currentScreen.left());
            break;

        case UKUIPanel::AlignmentCenter:
            rect.moveCenter(currentScreen.center());
            break;

        case UKUIPanel::AlignmentRight:
            rect.moveRight(currentScreen.right());
            break;
        }

        // Vert .......................
        if (m_position == IUKUIPanel::PositionTop) {
            if (m_hidden)
                rect.moveBottom(currentScreen.top() + PANEL_HIDE_SIZE);
            else
                rect.moveTop(currentScreen.top());
        } else {
            if (m_hidden)
                rect.moveTop(currentScreen.bottom() - PANEL_HIDE_SIZE);
            else
                rect.moveBottom(currentScreen.bottom());
        }
        qDebug()<<"ukui-panel Rect is :"<<rect;
    } else {
        // Vert panel ***************************
        rect.setWidth(qMax(PANEL_MINIMUM_SIZE, m_panelSize));
        if (m_lengthInPercents)
            rect.setHeight(currentScreen.height() * m_length / 100.0);
        else {
            if (m_length <= 0)
                rect.setHeight(currentScreen.height() + m_length);
            else
                rect.setHeight(m_length);
        }

        rect.setHeight(qMax(rect.size().height(), m_layout->minimumSize().height()));

        // Vert .......................
        switch (m_alignment)
        {
        case UKUIPanel::AlignmentLeft:
            rect.moveTop(currentScreen.top());
            break;

        case UKUIPanel::AlignmentCenter:
            rect.moveCenter(currentScreen.center());
            break;

        case UKUIPanel::AlignmentRight:
            rect.moveBottom(currentScreen.bottom());
            break;
        }

        // Horiz ......................
        if (m_position == IUKUIPanel::PositionLeft) {
            if (m_hidden)
                rect.moveRight(currentScreen.left() + PANEL_HIDE_SIZE);
            else
                rect.moveLeft(currentScreen.left());
        } else {
            if (m_hidden)
                rect.moveLeft(currentScreen.right() - PANEL_HIDE_SIZE);
            else
                rect.moveRight(currentScreen.right());
        }
    }
    if (rect != geometry()) {
        setFixedSize(rect.size());
        if (animate) {
            if (m_animation == nullptr) {
                m_animation = new QPropertyAnimation(this, "geometry");
                m_animation->setEasingCurve(QEasingCurve::Linear);
                //Note: for hiding, the margins are set after animation is finished
                connect(m_animation, &QAbstractAnimation::finished, [this] { if (m_hidden) setMargins(); });
            }
            m_animation->setDuration(m_animationTime);
            m_animation->setStartValue(geometry());
            m_animation->setEndValue(rect);
            //Note: for showing-up, the margins are removed instantly
            if (!m_hidden)
                setMargins();
            m_animation->start();
        } else {
            setMargins();
            setGeometry(rect);
        }
    }
    QDBusMessage message =QDBusMessage::createSignal("/panel/position", "org.ukui.panel", "UKuiPanelPosition");
    QList<QVariant> args;
    args.append(currentScreen.x());
    args.append(currentScreen.y());
    args.append(currentScreen.width());
    args.append(currentScreen.height());
    args.append(panelSize());
    args.append(m_gsettings->get(PANEL_POSITION_KEY).toInt());
    message.setArguments(args);
    QDBusConnection::sessionBus().send(message);
}

/*设置边距*/
void UKUIPanel::setMargins()
{
    if (m_hidden) {
        if (isHorizontal()) {
            if (m_position == IUKUIPanel::PositionTop)
                m_layout->setContentsMargins(0, 0, 0, PANEL_HIDE_SIZE);
            else
                m_layout->setContentsMargins(0, PANEL_HIDE_SIZE, 0, 0);
        } else {
            if (m_position == IUKUIPanel::PositionLeft)
                m_layout->setContentsMargins(0, 0, PANEL_HIDE_SIZE, 0);
            else
                m_layout->setContentsMargins(PANEL_HIDE_SIZE, 0, 0, 0);
        }
        if (!m_visibleMargin)
            setWindowOpacity(0.0);
    } else {
        m_layout->setContentsMargins(0, 0, 0, 0);
        if (!m_visibleMargin)
            setWindowOpacity(1.0);
    }
}

/*
 *　ukui-panel　的实时调整功能
 *　QEvent::LayoutRequest　能监听到widget should be relayouted时候的信号
 *　emit realigned()　信号在PanelPluginsModel类中传给插件的realign函数
 * 所有的插件类都需要重写这个函数用以跟任务栏的位置保持一致
 * UKUIPanel::event -> UKUIPanel::realign() -> Plugin::realign() ->UKUITrayPlugin:realign
*/
void UKUIPanel::realign()
{
    emit realigned();
    if (!isVisible())
        return;
#if 0
    qDebug() << "** Realign *********************";
    qDebug() << "PanelSize:   " << mPanelSize;
    qDebug() << "IconSize:      " << mIconSize;
    qDebug() << "LineCount:     " << mLineCount;
    qDebug() << "Length:        " << mLength << (mLengthInPercents ? "%" : "px");
    qDebug() << "Alignment:     " << (mAlignment == 0 ? "center" : (mAlignment < 0 ? "left" : "right"));
    qDebug() << "Position:      " << positionToStr(mPosition) << "on" << mScreenNum;
    qDebug() << "Plugins count: " << mPlugins.count();
#endif

    setPanelGeometry();

    // Reserve our space on the screen ..........
    // It's possible that our geometry is not changed, but screen resolution is changed,
    // so resetting WM_STRUT is still needed. To make it simple, we always do it.
    updateWmStrut();
}


// Update the _NET_WM_PARTIAL_STRUT and _NET_WM_STRUT properties for the window
void UKUIPanel::updateWmStrut()
{
    WId wid = effectiveWinId();
    if(wid == 0 || !isVisible())
        return;

    if (m_reserveSpace) {
        const QRect wholeScreen = QApplication::desktop()->geometry();
        const QRect rect = geometry();
        // NOTE: https://standards.freedesktop.org/wm-spec/wm-spec-latest.html
        // Quote from the EWMH spec: " Note that the strut is relative to the screen edge, and not the edge of the xinerama monitor."
        // So, we use the geometry of the whole screen to calculate the strut rather than using the geometry of individual monitors.
        // Though the spec only mention Xinerama and did not mention XRandR, the rule should still be applied.
        // At least openbox is implemented like this.
        switch (m_position)
        {
        case UKUIPanel::PositionTop:
            KWindowSystem::setExtendedStrut(wid,
                                            /* Left   */  0, 0, 0,
                                            /* Right  */  0, 0, 0,
                                            /* Top    */  getReserveDimension(), rect.left(), rect.right(),
                                            /* Bottom */  0, 0, 0
                                            );
            break;

        case UKUIPanel::PositionBottom:
            KWindowSystem::setExtendedStrut(wid,
                                            /* Left   */  0, 0, 0,
                                            /* Right  */  0, 0, 0,
                                            /* Top    */  0, 0, 0,
                                            /* Bottom */  getReserveDimension(), rect.left(), rect.right()
                                            );
            break;

        case UKUIPanel::PositionLeft:
            KWindowSystem::setExtendedStrut(wid,
                                            /* Left   */  getReserveDimension(), rect.top(), rect.bottom(),
                                            /* Right  */  0, 0, 0,
                                            /* Top    */  0, 0, 0,
                                            /* Bottom */  0, 0, 0
                                            );

            break;

        case UKUIPanel::PositionRight:
            KWindowSystem::setExtendedStrut(wid,
                                            /* Left   */  0, 0, 0,
                                            /* Right  */  getReserveDimension(), rect.top(), rect.bottom(),
                                            /* Top    */  0, 0, 0,
                                            /* Bottom */  0, 0, 0
                                            );
            break;
        }
    } else
    {
        KWindowSystem::setExtendedStrut(wid,
                                        /* Left   */  0, 0, 0,
                                        /* Right  */  0, 0, 0,
                                        /* Top    */  0, 0, 0,
                                        /* Bottom */  0, 0, 0
                                        );
    }
}


/************************************************
  The panel can't be placed on boundary of two displays.
  This function checks if the panel can be placed on the display
  @screenNum on @position.
 ************************************************/
bool UKUIPanel::canPlacedOn(int screenNum, UKUIPanel::Position position)
{
    QDesktopWidget* dw = QApplication::desktop();

    switch (position)
    {
    case UKUIPanel::PositionTop:
        for (int i = 0; i < dw->screenCount(); ++i)
            if (dw->screenGeometry(i).bottom() < dw->screenGeometry(screenNum).top())
                return false;
        return true;

    case UKUIPanel::PositionBottom:
        for (int i = 0; i < dw->screenCount(); ++i)
            if (dw->screenGeometry(i).top() > dw->screenGeometry(screenNum).bottom())
                return false;
        return true;

    case UKUIPanel::PositionLeft:
        for (int i = 0; i < dw->screenCount(); ++i)
            if (dw->screenGeometry(i).right() < dw->screenGeometry(screenNum).left())
                return false;
        return true;

    case UKUIPanel::PositionRight:
        for (int i = 0; i < dw->screenCount(); ++i)
            if (dw->screenGeometry(i).left() > dw->screenGeometry(screenNum).right())
                return false;
        return true;
    }

    return false;
}


/************************************************

 ************************************************/
int UKUIPanel::findAvailableScreen(UKUIPanel::Position position)
{
    int current = m_screenNum;

    for (int i = current; i < QApplication::desktop()->screenCount(); ++i)
        if (canPlacedOn(i, position))
            return i;

    for (int i = 0; i < current; ++i)
        if (canPlacedOn(i, position))
            return i;

    return 0;
}

/*右键　系统监视器选项*/
void UKUIPanel::systeMonitor()
{
    QProcess *process =new QProcess(this);
    process->startDetached("/usr/bin/ukui-system-monitor");
    process->deleteLater();
}

/*任务栏大小和方向的调整*/
void UKUIPanel::adjustPanel()
{
    QAction *pmenuaction_s;
    QAction *pmenuaction_m;
    QAction *pmenuaction_l;

    pmenuaction_s=new QAction(this);
    pmenuaction_s->setText(tr("Small"));
    pmenuaction_m=new QAction(this);
    pmenuaction_m->setText(tr("Medium"));
    pmenuaction_l=new QAction(this);
    pmenuaction_l->setText(tr("Large"));


    QMenu *pmenu_panelsize;
    pmenu_panelsize=new QMenu(this);
    pmenu_panelsize->setTitle(tr("Adjustment Size"));
    pmenu_panelsize->addAction(pmenuaction_s);
    pmenu_panelsize->addAction(pmenuaction_m);
    pmenu_panelsize->addAction(pmenuaction_l);
    pmenu_panelsize->setWindowOpacity(0.9);
    m_menu->addMenu(pmenu_panelsize);

    pmenuaction_s->setCheckable(true);
    pmenuaction_m->setCheckable(true);
    pmenuaction_l->setCheckable(true);
    pmenuaction_s->setChecked(m_gsettings->get(PANEL_SIZE_KEY).toInt()==PANEL_SIZE_SMALL);
    pmenuaction_m->setChecked(m_gsettings->get(PANEL_SIZE_KEY).toInt()==PANEL_SIZE_MEDIUM);
    pmenuaction_l->setChecked(m_gsettings->get(PANEL_SIZE_KEY).toInt()==PANEL_SIZE_LARGE);

    connect(pmenuaction_s,&QAction::triggered,[this] {
        m_gsettings->set(PANEL_SIZE_KEY,PANEL_SIZE_SMALL);
        m_gsettings->set(ICON_SIZE_KEY,ICON_SIZE_SMALL);
        setIconSize(ICON_SIZE_SMALL,true);
    });
    connect(pmenuaction_m,&QAction::triggered,[this] {
        m_gsettings->set(PANEL_SIZE_KEY,PANEL_SIZE_MEDIUM);
        m_gsettings->set(ICON_SIZE_KEY,ICON_SIZE_MEDIUM);
        setIconSize(ICON_SIZE_MEDIUM,true);
    });
    connect(pmenuaction_l,&QAction::triggered,[this] {
        m_gsettings->set(PANEL_SIZE_KEY,PANEL_SIZE_LARGE);
        m_gsettings->set(ICON_SIZE_KEY,ICON_SIZE_LARGE);
        setIconSize(ICON_SIZE_LARGE,true);
    });
    pmenu_panelsize->setDisabled(m_lockPanel);

    QAction *pmenuaction_top;
    QAction *pmenuaction_bottom;
    QAction *pmenuaction_left;
    QAction *pmenuaction_right;
    pmenuaction_top=new QAction(this);
    pmenuaction_top->setText(tr("Up"));
    pmenuaction_bottom=new QAction(this);
    pmenuaction_bottom->setText(tr("Bottom"));
    pmenuaction_left=new QAction(this);
    pmenuaction_left->setText(tr("Left"));
    pmenuaction_right=new QAction(this);
    pmenuaction_right->setText(tr("Right"));
    QMenu *pmenu_positon;
    pmenu_positon=new QMenu(this);
    pmenu_positon->setTitle(tr("Adjustment Position"));
    pmenu_positon->addAction(pmenuaction_top);
    pmenu_positon->addAction(pmenuaction_bottom);
    pmenu_positon->addAction(pmenuaction_left);
    pmenu_positon->addAction(pmenuaction_right);
    m_menu->addMenu(pmenu_positon);

    pmenuaction_top->setCheckable(true);
    pmenuaction_bottom->setCheckable(true);
    pmenuaction_left->setCheckable(true);
    pmenuaction_right->setCheckable(true);
    pmenuaction_top->setChecked(m_gsettings->get(PANEL_POSITION_KEY).toInt()==1);
    pmenuaction_bottom->setChecked(m_gsettings->get(PANEL_POSITION_KEY).toInt()==0);
    pmenuaction_left->setChecked(m_gsettings->get(PANEL_POSITION_KEY).toInt()==2);
    pmenuaction_right->setChecked(m_gsettings->get(PANEL_POSITION_KEY).toInt()==3);


    connect(pmenuaction_top,&QAction::triggered, [this] { setPosition(0,PositionTop,true);});
    connect(pmenuaction_bottom,&QAction::triggered, [this] { setPosition(0,PositionBottom,true);});
    connect(pmenuaction_left,&QAction::triggered, [this] { setPosition(0,PositionLeft,true);});
    connect(pmenuaction_right,&QAction::triggered, [this] { setPosition(0,PositionRight,true);});
    pmenu_positon->setWindowOpacity(0.9);
    pmenu_positon->setDisabled(m_lockPanel);


    m_settings->beginGroup(m_configGroup);
    QAction * hidepanel = m_menu->addAction(tr("Hide Panel"));
    hidepanel->setDisabled(m_lockPanel);
    hidepanel->setCheckable(true);
    hidepanel->setChecked(m_hidable);
    connect(hidepanel, &QAction::triggered, [this] {
        m_settings->beginGroup(m_configGroup);
        m_hidable = m_settings->value(CFG_KEY_HIDABLE, m_hidable).toBool();
        m_settings->endGroup();
        if(m_hidable)
            m_hideTimer.stop();
        setHidable(!m_hidable,true);
        m_hidden = m_hidable;
        m_showDelayTimer.start();
        m_time->start(1000);
    });
    m_settings->endGroup();
}
/*右键　显示桌面选项*/
void UKUIPanel::showDesktop()
{
    KWindowSystem::setShowingDesktop(!KWindowSystem::showingDesktop());
}

/*右键　显示任务视图　选项*/
void UKUIPanel::showTaskView()
{
//    system("ukui-window-switch --show-workspace");
    if(m_gsettings->keys().contains(SHOW_TASKVIEW)) {
        if(m_gsettings->get(SHOW_TASKVIEW).toBool()) {
            m_gsettings->set(SHOW_TASKVIEW,false);
        } else {
            m_gsettings->set(SHOW_TASKVIEW,true);
        }
    }
}

/*右键　显示夜间模式按钮　选项*/
void UKUIPanel::showNightModeButton()
{
//    system("ukui-window-switch --show-workspace");
    if(m_gsettings->keys().contains(SHOW_NIGHTMODE)) {
        if(m_gsettings->get(SHOW_NIGHTMODE).toBool()) {
            m_gsettings->set(SHOW_NIGHTMODE,false);
        } else {
            m_gsettings->set(SHOW_NIGHTMODE,true);
        }
    }
}

/*设置任务栏高度*/
void UKUIPanel::setPanelSize(int value, bool save)
{
    if (m_panelSize != value) {
        m_panelSize = value;
        realign();

        if (save)
            saveSettings(true);
    }
}

/*设置任务栏图标大小*/
void UKUIPanel::setIconSize(int value, bool save)
{
    if (m_iconSize != value) {
        m_iconSize = value;
        m_layout->setLineSize(m_iconSize);

        if (save)
            saveSettings(true);

        realign();
    }
}

void UKUIPanel::setLineCount(int value, bool save)
{
    if (m_lineCount != value) {
        m_lineCount = value;
        m_layout->setEnabled(false);
        m_layout->setLineCount(m_lineCount);
        m_layout->setEnabled(true);

        if (save)
            saveSettings(true);

        realign();
    }
}


/************************************************

 ************************************************/
void UKUIPanel::setLength(int length, bool inPercents, bool save)
{
    if (m_length == length &&
            m_lengthInPercents == inPercents)
        return;

    m_length = length;
    m_lengthInPercents = inPercents;

    if (save)
        saveSettings(true);

    realign();
}


/************************************************

 ************************************************/
void UKUIPanel::setPosition(int screen, IUKUIPanel::Position position, bool save)
{
    if (m_screenNum == screen &&
            m_position == position)
        return;

    m_actualScreenNum = screen;
    m_position = position;
    m_layout->setPosition(m_position);

    if (save) {
        m_screenNum = screen;
        saveSettings(true);
    }

    // Qt 5 adds a new class QScreen and add API for setting the screen of a QWindow.
    // so we had better use it. However, without this, our program should still work
    // as long as XRandR is used. Since XRandR combined all screens into a large virtual desktop
    // every screen and their virtual siblings are actually on the same virtual desktop.
    // So things still work if we don't set the screen correctly, but this is not the case
    // for other backends, such as the upcoming wayland support. Hence it's better to set it.
    if(windowHandle()) {
        // QScreen* newScreen = qApp->screens().at(screen);
        // QScreen* oldScreen = windowHandle()->screen();
        // const bool shouldRecreate = windowHandle()->handle() && !(oldScreen && oldScreen->virtualSiblings().contains(newScreen));
        // Q_ASSERT(shouldRecreate == false);

        // NOTE: When you move a window to another screen, Qt 5 might recreate the window as needed
        // But luckily, this never happen in XRandR, so Qt bug #40681 is not triggered here.
        // (The only exception is when the old screen is destroyed, Qt always re-create the window and
        // this corner case triggers #40681.)
        // When using other kind of multihead settings, such as Xinerama, this might be different and
        // unless Qt developers can fix their bug, we have no way to workaround that.
        windowHandle()->setScreen(qApp->screens().at(screen));
    }

    realign();
    m_gsettings->set(PANEL_POSITION_KEY,position);
    setPanelGeometry(true);
}

/************************************************
 *
 ************************************************/
void UKUIPanel::setAlignment(Alignment value, bool save)
{
    if (m_alignment == value)
        return;

    m_alignment = value;

    if (save)
        saveSettings(true);

    realign();
}



QRect UKUIPanel::globalGeometry() const
{
    // panel is the the top-most widget/window, no calculation needed
    return geometry();
}

bool UKUIPanel::event(QEvent *event)
{
    switch (event->type())
    {
    case QEvent::ContextMenu:
        showPopupMenu();
        break;

    case QEvent::LayoutRequest:
        emit realigned();
        break;

    case QEvent::WinIdChange:
    {
        // qDebug() << "WinIdChange" << hex << effectiveWinId();
        if(effectiveWinId() == 0)
            break;

        // Sometimes Qt needs to re-create the underlying window of the widget and
        // the winId() may be changed at runtime. So we need to reset all X11 properties
        // when this happens.
        //qDebug() << "WinIdChange" << hex << effectiveWinId() << "handle" << windowHandle() << windowHandle()->screen();

        // Qt::WA_X11NetWmWindowTypeDock becomes ineffective in Qt 5
        // See QTBUG-39887: https://bugreports.qt-project.org/browse/QTBUG-39887
        // Let's use KWindowSystem for that
        KWindowSystem::setType(effectiveWinId(), NET::Dock);

        updateWmStrut(); // reserve screen space for the panel
        KWindowSystem::setOnAllDesktops(effectiveWinId(), true);
        break;
    }
    case QEvent::DragEnter:
        dynamic_cast<QDropEvent *>(event)->setDropAction(Qt::IgnoreAction);
        event->accept();
        //no break intentionally
    case QEvent::Enter:
        m_showDelayTimer.start();
        break;

    case QEvent::Leave:
        setCursor(Qt::ArrowCursor);
        m_showDelayTimer.stop();
        hidePanel();
        break;
    case QEvent::DragLeave:
        m_showDelayTimer.stop();
        hidePanel();
        break;

    default:
        break;
    }

    return QFrame::event(event);
}

/************************************************

 ************************************************/

void UKUIPanel::showEvent(QShowEvent *event)
{
    QFrame::showEvent(event);
    realign();
}

void UKUIPanel::styleAdjust()
{
    QString filename = QDir::homePath() + "/.config/ukui/panel-commission.ini";
    QSettings m_settings(filename, QSettings::IniFormat);
    m_settings.setIniCodec("UTF-8");

    m_settings.beginGroup("Transparency");
    QString transparency_action = m_settings.value("transparency", "").toString();
    if (transparency_action.isEmpty()) {
        transparency_action = "open";
    }
    m_settings.endGroup();

    const QByteArray transparency_id(TRANSPARENCY_SETTINGS);
    if(QGSettings::isSchemaInstalled(transparency_id)) {
        m_transparencyGsettings = new QGSettings(transparency_id);

    m_transparency=m_transparencyGsettings->get(TRANSPARENCY_KEY).toDouble()*255;
    this->update();
    connect(m_transparencyGsettings, &QGSettings::changed, this, [=] (const QString &key){
        if(key==TRANSPARENCY_KEY && transparency_action=="open") {
            m_transparency=m_transparencyGsettings->get(TRANSPARENCY_KEY).toDouble()*255;
            this->update();
        }
     });
    } else {
        m_transparency=0.75;
    }
}

/* 使用paintEvent 对panel进行绘制的时候有如下问题：
 * 1.绘制速度需要鼠标事件触发，明显的切换不流畅
 * 2.部分区域绘制不能生效，调整任务栏高度之后才能生效
 */

void UKUIPanel::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    p.setPen(Qt::NoPen);
//    p.setBrush(QBrush(QColor(19,22,28,transparency)));
    QColor color= palette().color(QPalette::Base);
    color.setAlpha(m_transparency);
    QBrush brush = QBrush(color);
    p.setBrush(brush);

    p.setRenderHint(QPainter::Antialiasing);
    p.drawRoundedRect(opt.rect,0,0);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}


/*Right-Clicked Menu of ukui-panel
 * it's a Popup Menu
*/
void UKUIPanel::showPopupMenu(Plugin *plugin)
{
    m_menu = new QMenu(this);
    m_menu->setAttribute(Qt::WA_DeleteOnClose);

    /* @new features
     * //Plugin Menu  负责显示插件的菜单项，ukui３.0的设计暂时不需要插件菜单项
     * 关于后续的插件右键菜单的详细调整
     * 如果需要在任务栏菜单项上面添加 插件的菜单选项就放开此功能
     * 放开此功能可以丰富插件右键菜单，windows是这样做的
     */
    if (plugin) {
        QMenu *m = plugin->popupMenu();

        if (m) {
            //menu->addTitle(plugin->windowTitle());
            const auto actions = m->actions();
            for (auto const & action : actions) {
                action->setParent(m_menu);
                action->setDisabled(m_lockPanel);
                m_menu->addAction(action);
            }
            delete m;
        }
    }

    m_menu->addSeparator();

    QAction * showtaskview = m_menu->addAction(tr("Show Taskview"));
    showtaskview->setCheckable(true);
    showtaskview->setChecked(m_gsettings->get(SHOW_TASKVIEW).toBool());
    connect(showtaskview, &QAction::triggered, [this] { showTaskView(); });

    QString filename = QDir::homePath() + "/.config/ukui/panel-commission.ini";
    QSettings m_settings(filename, QSettings::IniFormat);
    m_settings.setIniCodec("UTF-8");

    m_settings.beginGroup("NightMode");
    QString nightmode_action = m_settings.value("nightmode", "").toString();
    if (nightmode_action.isEmpty()) {
        nightmode_action = "show";
    }
    m_settings.endGroup();

    if(nightmode_action == "show") {
        QAction * shownightmode = m_menu->addAction(tr("Show Nightmode"));
        shownightmode->setCheckable(true);
        shownightmode->setChecked(m_gsettings->get(SHOW_NIGHTMODE).toBool());
        connect(shownightmode, &QAction::triggered, [this] { showNightModeButton(); });
    }

    m_menu->addAction(XdgIcon::fromTheme(QLatin1String("configure")),
                    tr("Show Desktop"),
                    this, SLOT(showDesktop())
                    );

    m_menu->addSeparator();

    if(QFileInfo::exists(QString("/usr/bin/ukui-system-monitor"))) {
        m_menu->addAction(XdgIcon::fromTheme(QLatin1String("configure")),
                        tr("Show System Monitor"),
                        this, SLOT(systeMonitor())
                        );
    }

    m_menu->addSeparator();

    adjustPanel();

    /*
    UKUIPanelApplication *a = reinterpret_cast<UKUIPanelApplication*>(qApp);
    menu->addAction(XdgIcon::fromTheme(QLatin1String("list-add")),
                   tr("Add New Panel"),
                   a, SLOT(addNewPanel())
                  );


    if (a->count() > 1)
    {
        menu->addAction(XdgIcon::fromTheme(QLatin1String("list-remove")),
                       tr("Remove Panel", "Menu Item"),
                       this, SLOT(userRequestForDeletion())
                      )->setDisabled(mLockPanel);
    }
*/
    m_lockAction = m_menu->addAction(tr("Lock This Panel"));
    m_lockAction->setCheckable(true);
    m_lockAction->setChecked(m_lockPanel);
    connect(m_lockAction, &QAction::triggered, [this] { m_lockPanel = !m_lockPanel; saveSettings(false); });

    //Hidden features, lock the panel
    /*
    menu->addAction(XdgIcon::fromTheme(QLatin1String("configure")),
                   tr("Reset Panel"),
                   this, SLOT(panelReset())
                  )->setDisabled(mLockPanel);
*/

        QAction *about;
        about=new QAction(this);
        about->setText(tr("About Kylin"));
        m_menu->addAction(about);
        connect(about,&QAction::triggered, [this] {
            QProcess *process =new QProcess(this);
            process->start(
                    "bash",
                    QStringList() << "-c"
                                  << "dpkg -l | grep ukui-control-center");
                process->waitForFinished();
                QString strResult = process->readAllStandardOutput() + process->readAllStandardError();
                if (-1 != strResult.indexOf("3.0")) {
                    QProcess::startDetached(QString("ukui-control-center -a"));
                } else {
                    QProcess::startDetached(QString("ukui-control-center -m About"));
                }
        });

#ifdef DEBUG
    menu->addSeparator();
    menu->addAction("Exit (debug only)", qApp, SLOT(quit()));
#endif

    /* Note: in multihead & multipanel setup the QMenu::popup/exec places the window
     * sometimes wrongly (it seems that this bug is somehow connected to misinterpretation
     * of QDesktopWidget::availableGeometry)
     */
    m_menu->setGeometry(calculatePopupWindowPos(QCursor::pos(), m_menu->sizeHint()));
    willShowWindow(m_menu);
    m_menu->show();
}

Plugin* UKUIPanel::findPlugin(const IUKUIPanelPlugin* iPlugin) const
{
    const auto plugins = m_plugins->plugins();
    for (auto const & plug : plugins)
        if (plug->iPlugin() == iPlugin)
            return plug;
    return nullptr;
}

/************************************************

 ************************************************/
QRect UKUIPanel::calculatePopupWindowPos(QPoint const & absolutePos, QSize const & windowSize) const
{
    int x = absolutePos.x(), y = absolutePos.y();

    switch (position())
    {
    case IUKUIPanel::PositionTop:
        y = globalGeometry().bottom();
        break;

    case IUKUIPanel::PositionBottom:
        y = globalGeometry().top() - windowSize.height();
        break;

    case IUKUIPanel::PositionLeft:
        x = globalGeometry().right();
        break;

    case IUKUIPanel::PositionRight:
        x = globalGeometry().left() - windowSize.width();
        break;
    }

    QRect res(QPoint(x, y), windowSize);

    QRect screen = QApplication::desktop()->screenGeometry(this);
    // NOTE: We cannot use AvailableGeometry() which returns the work area here because when in a
    // multihead setup with different resolutions. In this case, the size of the work area is limited
    // by the smallest monitor and may be much smaller than the current screen and we will place the
    // menu at the wrong place. This is very bad for UX. So let's use the full size of the screen.
    if (res.right() > screen.right())
        res.moveRight(screen.right());

    if (res.bottom() > screen.bottom())
        res.moveBottom(screen.bottom());

    if (res.left() < screen.left())
        res.moveLeft(screen.left());

    if (res.top() < screen.top())
        res.moveTop(screen.top());

    return res;
}

QRect UKUIPanel::calculatePopupWindowPos(const IUKUIPanelPlugin *plugin, const QSize &windowSize) const
{
    Plugin *panel_plugin = findPlugin(plugin);
    if (nullptr == panel_plugin) {
        qWarning() << Q_FUNC_INFO << "Wrong logic? Unable to find Plugin* for" << plugin << "known plugins follow...";
        const auto plugins = m_plugins->plugins();
        for (auto const & plug : plugins)
            qWarning() << plug->iPlugin() << plug;

        return QRect();
    }

    // Note: assuming there are not contentMargins around the "BackgroundWidget" (UKUIPanelWidget)
    return calculatePopupWindowPos(globalGeometry().topLeft() + panel_plugin->geometry().topLeft(), windowSize);
}


void UKUIPanel::willShowWindow(QWidget * w)
{
    m_standaloneWindows->observeWindow(w);
}

void UKUIPanel::pluginFlagsChanged(const IUKUIPanelPlugin * /*plugin*/)
{
    m_layout->rebuild();
}

void UKUIPanel::pluginMoved(Plugin * plug)
{
    //get new position of the moved plugin
    bool found{false};
    QString plug_is_before;
    for (int i=0; i<m_layout->count(); ++i) {
        Plugin *plugin = qobject_cast<Plugin*>(m_layout->itemAt(i)->widget());
        if (plugin) {
            if (found) {
                //we found our plugin in previous cycle -> is before this (or empty as last)
                plug_is_before = plugin->settingsGroup();
                break;
            } else {
                found = (plug == plugin);
            }
        }
    }
    m_plugins->movePlugin(plug, plug_is_before);
}


void UKUIPanel::showPanel(bool animate)
{
    if (m_hidable)  {
        m_hideTimer.stop();
        if (m_hidden) {
            m_hidden = false;
            setPanelGeometry(m_animationTime > 0 && animate);
        }
    }
}

void UKUIPanel::hidePanel()
{
    if (m_hidable && !m_hidden
            && !m_standaloneWindows->isAnyWindowShown()
            )
        m_hideTimer.start();
}

void UKUIPanel::hidePanelWork()
{
    if (!geometry().contains(QCursor::pos())) {
        if (!m_standaloneWindows->isAnyWindowShown()) {
            m_hidden = true;
            setPanelGeometry(m_animationTime > 0);
        } else {
            m_hideTimer.start();
        }
    }
    QDBusMessage message =QDBusMessage::createSignal("/panel", "org.ukui.panel.settings", "PanelHided");
    QDBusConnection::sessionBus().send(message);
}

void UKUIPanel::setHidable(bool hidable, bool save)
{
    if (m_hidable == hidable)
        return;

    m_hidable = hidable;

    if (save)
        saveSettings(true);

    realign();
}

void UKUIPanel::setVisibleMargin(bool visibleMargin, bool save)
{
    if (m_visibleMargin == visibleMargin)
        return;

    m_visibleMargin = visibleMargin;

    if (save)
        saveSettings(true);

    realign();
}

void UKUIPanel::setAnimationTime(int animationTime, bool save)
{
    if (m_animationTime == animationTime)
        return;

    m_animationTime = animationTime;

    if (save)
        saveSettings(true);
}

void UKUIPanel::setShowDelay(int showDelay, bool save)
{
    if (m_showDelayTimer.interval() == showDelay)
        return;

    m_showDelayTimer.setInterval(showDelay);

    if (save)
        saveSettings(true);
}

bool UKUIPanel::isPluginSingletonAndRunnig(QString const & pluginId) const
{
    Plugin const * plugin = m_plugins->pluginByID(pluginId);
    if (nullptr == plugin)
        return false;
    else
        return plugin->iPlugin()->flags().testFlag(IUKUIPanelPlugin::SingleInstance);
}

void UKUIPanel::connectToServer(){
    m_cloudInterface = new QDBusInterface("org.kylinssoclient.dbus",
                                          "/org/kylinssoclient/path",
                                          "org.freedesktop.kylinssoclient.interface",
                                          QDBusConnection::sessionBus());
    if (!m_cloudInterface->isValid()) {
        qDebug() << "fail to connect to service";
        qDebug() << qPrintable(QDBusConnection::systemBus().lastError().message());
        return;
    }
//    QDBusConnection::sessionBus().connect(cloudInterface, SIGNAL(shortcutChanged()), this, SLOT(shortcutChangedSlot()));
    QDBusConnection::sessionBus().connect(QString(), QString("/org/kylinssoclient/path"), QString("org.freedesktop.kylinssoclient.interface"), "keyChanged", this, SLOT(keyChangedSlot(QString)));
    // 将以后所有DBus调用的超时设置为 milliseconds
    m_cloudInterface->setTimeout(2147483647); // -1 为默认的25s超时
}

void UKUIPanel::keyChangedSlot(const QString &key) {
    if(key == "ukui-panel") {
        m_settings->beginGroup(m_configGroup);
        m_settings->sync();
        m_lockPanel = m_settings->value(CFG_KEY_LOCKPANEL).toBool();
        m_settings->endGroup();
//        if(m_lockAction!=nullptr)
//            m_lockAction->setChecked(mLockPanel);
    }
}

/////////////////////////////////////////////////////////////////////////////////
/// \brief UKUIPanel::areaDivid
/// \param globalpos
/// \return
/// 以下所有函数均为任务栏拖拽相关（位置、大小）
/////////////////////////////////////////////////////////////////////////////////

IUKUIPanel::Position UKUIPanel::areaDivid(QPoint globalpos) {
    int x = globalpos.rx();
    int y = globalpos.ry();
    float W = QApplication::screens().at(0)->size().width();
    float H = QApplication::screens().at(0)->size().height();
    float slope = H / W;
    if ((x < 100 || x > W - 100) && (y > H - 100 || y < 100)) return m_position;
    if (y > (int)(x * slope) && y > (int)(H - x * slope)) return PositionBottom;
    if (y > (int)(x * slope) && y < (int)(H - x * slope)) return PositionLeft;
    if (y < (int)(x * slope) && y < (int)(H - x * slope)) return PositionTop;
    if (y < (int)(x * slope) && y > (int)(H - x * slope)) return PositionRight;
}


void UKUIPanel::mousePressEvent(QMouseEvent *event) {
    setCursor(Qt::DragMoveCursor);
}

void UKUIPanel::mouseMoveEvent(QMouseEvent* event)
{
    if (m_lockPanel) return;
    if (m_moveLock == -1) {
        if (event->pos().ry() < 10) m_moveLock = 0;
        else m_moveLock = 1;
    }
    if (!m_moveLock) {
        int panel_h = QApplication::screens().at(0)->size().height() - event->globalPos().ry();
        int icon_size = panel_h*0.695652174;
        setCursor(Qt::SizeVerCursor);
        if (panel_h <= PANEL_SIZE_LARGE && panel_h >= PANEL_SIZE_SMALL) {
            setPanelSize(panel_h, true);
            setIconSize(icon_size, true);
            m_gsettings->set(PANEL_SIZE_KEY, panel_h);
            m_gsettings->set(ICON_SIZE_KEY, icon_size);
        }
        return;
    }
    setCursor(Qt::SizeAllCursor);
    IUKUIPanel::Position currentpos = areaDivid(event->globalPos());
    if (m_oldPos != currentpos) {
        setPosition(0,currentpos,true);
        m_oldPos = currentpos;
    }
}

void UKUIPanel::mouseReleaseEvent(QMouseEvent* event)
{
    setCursor(Qt::ArrowCursor);
    realign();
    emit realigned();
    m_moveLock = -1;
}
