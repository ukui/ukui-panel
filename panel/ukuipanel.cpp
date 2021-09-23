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
#include "ukuipanellimits.h"
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

// Config keys and groups
#define CFG_KEY_SCREENNUM          "desktop"
#define CFG_KEY_POSITION           "position"
#define CFG_KEY_LINECNT            "lineCount"
#define CFG_KEY_LENGTH             "width"
#define CFG_KEY_PERCENT            "width-percent"
#define CFG_KEY_ALIGNMENT          "alignment"
#define CFG_KEY_RESERVESPACE       "reserve-space"
#define CFG_KEY_PLUGINS            "plugins"
#define CFG_KEY_HIDABLE            "hidable"
#define CFG_KEY_VISIBLE_MARGIN     "visible-margin"
#define CFG_KEY_ANIMATION          "animation-duration"
#define CFG_KEY_SHOW_DELAY         "show-delay"
#define CFG_KEY_LOCKPANEL          "lockPanel"
#define CFG_KEY_PLUGINS_PC         "plugins-pc"
#define CFG_KEY_PLUGINS_PAD         "plugins-pad"

#define GSETTINGS_SCHEMA_SCREENSAVER "org.mate.interface"
#define KEY_MODE "gtk-theme"

#define PANEL_SIZE_LARGE  92
#define PANEL_SIZE_MEDIUM 70
#define PANEL_SIZE_SMALL  46
#define ICON_SIZE_LARGE   64
#define ICON_SIZE_MEDIUM  48
#define ICON_SIZE_SMALL   32
#define PANEL_SIZE_LARGE_V 70
#define PANEL_SIZE_MEDIUM_V 62
#define PANEL_SIZE_SMALL_V 47
#define POPUP_BORDER_SPACING 4

#define PANEL_SETTINGS      "org.ukui.panel.settings"
#define PANEL_SIZE_KEY      "panelsize"
#define ICON_SIZE_KEY       "iconsize"
#define PANEL_POSITION_KEY  "panelposition"
#define SHOW_TASKVIEW       "showtaskview"
#define SHOW_NIGHTMODE      "shownightmode"

#define TRANSPARENCY_SETTINGS       "org.ukui.control-center.personalise"
#define TRANSPARENCY_KEY            "transparency"

#define UKUI_SERVICE        "org.gnome.SessionManager"
#define UKUI_PATH           "/org/gnome/SessionManager"
#define UKUI_INTERFACE      "org.gnome.SessionManager"

#define DBUS_NAME            "org.ukui.SettingsDaemon"
#define DBUS_PATH            "/org/ukui/SettingsDaemon/wayland"
#define DBUS_INTERFACE       "org.ukui.SettingsDaemon.wayland"

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
    mSettings(settings),
    mConfigGroup(configGroup),
    mPlugins{nullptr},
    mStandaloneWindows{new WindowNotifier},
    mPanelSize(0),
    mIconSize(0),
    mLineCount(0),
    mLength(0),
    mAlignment(AlignmentLeft),
    mPosition(IUKUIPanel::PositionBottom),
    mScreenNum(0), //whatever (avoid conditional on uninitialized value)
    mActualScreenNum(0),
    mHidable(false),
    mVisibleMargin(true),
    mHidden(false),
    mAnimationTime(0),
    mReserveSpace(true),
    mAnimation(nullptr),
    mLockPanel(false)
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
    if(qgetenv("XDG_SESSION_TYPE")=="wayland") flag_hw990="hw_990";
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

    mLayout = new UKUIPanelLayout(UKUIPanelWidget);
    connect(mLayout, &UKUIPanelLayout::pluginMoved, this, &UKUIPanel::pluginMoved);
    UKUIPanelWidget->setLayout(mLayout);
    mLayout->setLineCount(mLineCount);

    mDelaySave.setSingleShot(true);
    mDelaySave.setInterval(SETTINGS_SAVE_DELAY);
    connect(&mDelaySave, SIGNAL(timeout()), this, SLOT(saveSettings()));

    mHideTimer.setSingleShot(true);
    mHideTimer.setInterval(PANEL_HIDE_DELAY);
    connect(&mHideTimer, SIGNAL(timeout()), this, SLOT(hidePanelWork()));

    connectToServer();
    mShowDelayTimer.setSingleShot(true);
    mShowDelayTimer.setInterval(PANEL_SHOW_DELAY);
    connect(&mShowDelayTimer, &QTimer::timeout, [this] { showPanel(mAnimationTime > 0); });

/*
    QDBusConnection::sessionBus().connect(QString(),
                                          QString("/backend"),
                                          "org.kde.kscreen.Backend",
                                          "configChanged",
                                          this,
                                          SLOT(getSize()));
*/
    if(flag_hw990=="hw_990"){
        caculateScreenGeometry();
        mDbusXrandInter = new QDBusInterface(DBUS_NAME,
                                             DBUS_PATH,
                                             DBUS_INTERFACE,
                                             QDBusConnection::sessionBus());

        connect(mDbusXrandInter, SIGNAL(screenPrimaryChanged(int,int,int,int)),this, SLOT(priScreenChanged(int,int,int,int)));

    }else{
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

    }

    connect(UKUi::Settings::globalSettings(), SIGNAL(settingsChanged()), this, SLOT(update()));
    connect(ukuiApp, SIGNAL(themeChanged()), this, SLOT(realign()));

    connect(mStandaloneWindows.data(), &WindowNotifier::firstShown, [this] { showPanel(true); });
    connect(mStandaloneWindows.data(), &WindowNotifier::lastHidden, this, &UKUIPanel::hidePanel);

    const QByteArray id(PANEL_SETTINGS);
    gsettings = new QGSettings(id);
    setPosition(0,intToPosition(gsettings->get(PANEL_POSITION_KEY).toInt(),PositionBottom),true);

    connect(gsettings, &QGSettings::changed, this, [=] (const QString &key){
        if(key==ICON_SIZE_KEY){
            setIconSize(gsettings->get(ICON_SIZE_KEY).toInt(),true);
        }
        if(key==PANEL_SIZE_KEY){
            setPanelSize(gsettings->get(PANEL_SIZE_KEY).toInt(),true);
        }
        if(key == PANEL_POSITION_KEY){
            setPosition(0,intToPosition(gsettings->get(PANEL_POSITION_KEY).toInt(),PositionBottom),true);
        }
    });

    time = new QTimer(this);
    connect(time, &QTimer::timeout, this,[=] (){
        mShowDelayTimer.stop();
        hidePanel();
        time->stop();
    });
    qDebug()<<"Panel :: setGeometry finished";
#if 0
    //给session发信号，告知任务栏已经启动完成，可以启动下一个组件
    QDBusInterface interface(UKUI_SERVICE,
                             UKUI_PATH,
                             UKUI_INTERFACE,
                             QDBusConnection::sessionBus());
    interface.call("startupfinished","ukui-panel","finish");
#endif

//    int height = QApplication::screens().at(0)->size().height();
//    int width = QApplication::screens().at(0)->size().width();
    MAX_SIZE_PANEL_IN_CALC = PANEL_SIZE_LARGE;//0.0851852 * height;
    MID_SIZE_PANEL_IN_CALC = PANEL_SIZE_MEDIUM;//0.0648148 * height;
    SML_SIZE_PANEL_IN_CALC = PANEL_SIZE_SMALL;//0.0425926 * height;
    if (!isHorizontal()) {
        MAX_SIZE_PANEL_IN_CALC = PANEL_SIZE_LARGE_V;//*= ((float)height / (float)width) * 1.25;
        MID_SIZE_PANEL_IN_CALC = PANEL_SIZE_MEDIUM_V;//*= ((float)height / (float)width) * 1.5;
        SML_SIZE_PANEL_IN_CALC = PANEL_SIZE_SMALL_V;//*= ((float)height / (float)width) * 1.75;
    }
    MAX_ICON_SIZE_IN_CLAC = 0.695652174 * MAX_SIZE_PANEL_IN_CALC;//ICON_SIZE_LARGE;
    MID_ICON_SIZE_IN_CLAC =  0.695652174 * MID_SIZE_PANEL_IN_CALC;//ICON_SIZE_MEDIUM;
    SML_ICON_SIZE_IN_CLAC =  0.695652174 * SML_SIZE_PANEL_IN_CALC;//ICON_SIZE_SMALL;

    readSettings();

    ensureVisible();

    qDebug()<<"Panel :: loadPlugins start";
    loadPlugins();
    qDebug()<<"Panel :: loadPlugins finished";

    show();
    qDebug()<<"Panel :: show UKuiPanel finished";	
#if 1
    //给session发信号，告知任务栏已经启动完成，可以启动下一个组件
    QDBusInterface interface(UKUI_SERVICE,
                             UKUI_PATH,
                             UKUI_INTERFACE,
                             QDBusConnection::sessionBus());
    interface.call("startupfinished","ukui-panel","finish");
#endif
    // show it the first time, despite setting
    if (mHidable)
    {
        showPanel(false);
        QTimer::singleShot(PANEL_HIDE_FIRST_TIME, this, SLOT(hidePanel()));
    }

    styleAdjust();
    qDebug()<<"Panel :: UKuiPanel  finished";

    gsettings->set(PANEL_SIZE_KEY, gsettings->get(PANEL_SIZE_KEY).toInt());
    gsettings->set(ICON_SIZE_KEY, gsettings->get(ICON_SIZE_KEY).toInt());

    UKuiPanelInformation* dbus=new UKuiPanelInformation;
    new PanelAdaptor(dbus);
    QDBusConnection con=QDBusConnection::sessionBus();
    if(!con.registerService("org.ukui.panel") ||
            !con.registerObject("/panel/position",dbus))
    {
        qDebug()<<"fail";
    }
}

void UKUIPanel::getSize() {
    int flg = 0;
    int size = gsettings->get(PANEL_SIZE_KEY).toInt();
    if (size == MAX_SIZE_PANEL_IN_CALC) {
        flg = 2;
    } else if (size == MID_SIZE_PANEL_IN_CALC) {
        flg = 1;
    }
    //int height = QApplication::screens().at(0)->size().height();
    //int width = QApplication::screens().at(0)->size().width();
    MAX_SIZE_PANEL_IN_CALC = PANEL_SIZE_LARGE;//0.0851852 * height;
    MID_SIZE_PANEL_IN_CALC = PANEL_SIZE_MEDIUM;//0.0648148 * height;
    SML_SIZE_PANEL_IN_CALC = PANEL_SIZE_SMALL;//0.0425926 * height;
    if (!isHorizontal()) {
        MAX_SIZE_PANEL_IN_CALC = PANEL_SIZE_LARGE_V;//*= ((float)height / (float)width) * 1.25;
        MID_SIZE_PANEL_IN_CALC = PANEL_SIZE_MEDIUM_V;//*= ((float)height / (float)width) * 1.5;
        SML_SIZE_PANEL_IN_CALC = PANEL_SIZE_SMALL_V;//*= ((float)height / (float)width) * 1.75;
    }
    MAX_ICON_SIZE_IN_CLAC = 0.695652174 * MAX_SIZE_PANEL_IN_CALC;//ICON_SIZE_LARGE;
    MID_ICON_SIZE_IN_CLAC =  0.695652174 * MID_SIZE_PANEL_IN_CALC;//ICON_SIZE_MEDIUM;
    SML_ICON_SIZE_IN_CLAC =  0.695652174 * SML_SIZE_PANEL_IN_CALC;//ICON_SIZE_SMALL;
    switch (flg) {
    case 0:
        gsettings->set(PANEL_SIZE_KEY, SML_SIZE_PANEL_IN_CALC);
        gsettings->set(ICON_SIZE_KEY, SML_ICON_SIZE_IN_CLAC);
        break;
    case 1:
        gsettings->set(PANEL_SIZE_KEY, MID_SIZE_PANEL_IN_CALC);
        gsettings->set(ICON_SIZE_KEY, MID_ICON_SIZE_IN_CLAC);
        break;
    case 2:
        gsettings->set(PANEL_SIZE_KEY, MAX_SIZE_PANEL_IN_CALC);
        gsettings->set(ICON_SIZE_KEY, MAX_ICON_SIZE_IN_CLAC);
        break;
    }
}

/************************************************

 ************************************************/
void UKUIPanel::readSettings()
{
    // Read settings ......................................
    mSettings->beginGroup(mConfigGroup);

    // Let Hidability be the first thing we read
    // so that every call to realign() is without side-effect
    mHidable = mSettings->value(CFG_KEY_HIDABLE, mHidable).toBool();
    mHidden = mHidable;
    mVisibleMargin = mSettings->value(CFG_KEY_VISIBLE_MARGIN, mVisibleMargin).toBool();
    mAnimationTime = mSettings->value(CFG_KEY_ANIMATION, mAnimationTime).toInt();
    mShowDelayTimer.setInterval(mSettings->value(CFG_KEY_SHOW_DELAY, mShowDelayTimer.interval()).toInt());

    // By default we are using size & count from theme.
    setLineCount(mSettings->value(CFG_KEY_LINECNT, PANEL_DEFAULT_LINE_COUNT).toInt(), false);

    setLength(mSettings->value(CFG_KEY_LENGTH, 100).toInt(),
              mSettings->value(CFG_KEY_PERCENT, true).toBool(),
              false);
    mScreenNum = mSettings->value(CFG_KEY_SCREENNUM, QApplication::desktop()->primaryScreen()).toInt();
//    setPosition(mScreenNum,
//                strToPosition(mSettings->value(CFG_KEY_POSITION).toString(), PositionBottom),
//                false);
    setAlignment(Alignment(mSettings->value(CFG_KEY_ALIGNMENT, mAlignment).toInt()), false);
    mReserveSpace = mSettings->value(CFG_KEY_RESERVESPACE, true).toBool();
    mLockPanel = mSettings->value(CFG_KEY_LOCKPANEL, false).toBool();

    mSettings->endGroup();

}


/************************************************

 ************************************************/
void UKUIPanel::saveSettings(bool later)
{
    mDelaySave.stop();
    if (later)
    {
        mDelaySave.start();
        return;
    }

    mSettings->beginGroup(mConfigGroup);

    //Note: save/load of plugin names is completely handled by mPlugins object
    //mSettings->setValue(CFG_KEY_PLUGINS, mPlugins->pluginNames());

    mSettings->setValue(CFG_KEY_LINECNT, mLineCount);

    mSettings->setValue(CFG_KEY_LENGTH, mLength);
    mSettings->setValue(CFG_KEY_PERCENT, mLengthInPercents);

    mSettings->setValue(CFG_KEY_SCREENNUM, mScreenNum);
//    mSettings->setValue(CFG_KEY_POSITION, positionToStr(mPosition));

    mSettings->setValue(CFG_KEY_ALIGNMENT, mAlignment);

    mSettings->setValue(CFG_KEY_RESERVESPACE, mReserveSpace);

    mSettings->setValue(CFG_KEY_HIDABLE, mHidable);
    mSettings->setValue(CFG_KEY_VISIBLE_MARGIN, mVisibleMargin);
    mSettings->setValue(CFG_KEY_ANIMATION, mAnimationTime);
    mSettings->setValue(CFG_KEY_SHOW_DELAY, mShowDelayTimer.interval());

    mSettings->setValue(CFG_KEY_LOCKPANEL, mLockPanel);

    mSettings->endGroup();
}


/*确保任务栏在调整分辨率和增加·屏幕之后能保持显示正常*/
void UKUIPanel::ensureVisible()
{
    if (!canPlacedOn(mScreenNum, mPosition))
        setPosition(findAvailableScreen(mPosition), mPosition, false);
    else
        mActualScreenNum = mScreenNum;

    // the screen size might be changed
    realign();
    getSize();
}


UKUIPanel::~UKUIPanel()
{
    mLayout->setEnabled(false);
    delete mAnimation;
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
    QString names_key(mConfigGroup);
    names_key += '/';
    names_key += QLatin1String(CFG_KEY_PLUGINS);
    mPlugins.reset(new PanelPluginsModel(this, names_key, pluginDesktopDirs()));

    connect(mPlugins.data(), &PanelPluginsModel::pluginAdded, mLayout, &UKUIPanelLayout::addPlugin);
    connect(mPlugins.data(), &PanelPluginsModel::pluginMovedUp, mLayout, &UKUIPanelLayout::moveUpPlugin);
    //reemit signals
    connect(mPlugins.data(), &PanelPluginsModel::pluginAdded, this, &UKUIPanel::pluginAdded);
    connect(mPlugins.data(), &PanelPluginsModel::pluginRemoved, this, &UKUIPanel::pluginRemoved);

    const auto plugins = mPlugins->plugins();
    for (auto const & plugin : plugins)
    {
        mLayout->addPlugin(plugin);
        connect(plugin, &Plugin::dragLeft, [this] { mShowDelayTimer.stop(); hidePanel();});
    }
}

void UKUIPanel::reloadPlugins(QString model){

    QStringList list=readConfig(model);
    checkPlugins(list);
    movePlugins(list);
}

QStringList UKUIPanel::readConfig(QString model){

    QStringList list;
    mSettings->beginGroup(mConfigGroup);
    if(model=="pc"){
        list = mSettings->value(CFG_KEY_PLUGINS_PC).toStringList();
    }else{
        list = mSettings->value(CFG_KEY_PLUGINS_PAD).toStringList();
    }
    mSettings->endGroup();
    return list;
}

void UKUIPanel::checkPlugins(QStringList list){

    const auto plugins = mPlugins->plugins();
    for (auto const & plugin : plugins)
    {
        plugin->hide();
    }

    for(int i=0;i<list.size();i++){
        if(mPlugins->pluginNames().contains(list[i])){
            if(mPlugins->pluginByName(list[i])){
                mPlugins->pluginByName(list[i])->show();
            }
        }
    }
}

void UKUIPanel::movePlugins(QStringList list){
    for(int i=0;i<mLayout->count();i++){
        mLayout->removeItem(mLayout->itemAt(i));
    }
    const auto plugins = mPlugins->plugins();
    for (auto const & plugin : plugins)
    {
        if(!plugin->isHidden()){
            mLayout->addWidget(plugin);
        }
    }
}


int UKUIPanel::getReserveDimension()
{
    return mHidable ? PANEL_HIDE_SIZE : qMax(PANEL_MINIMUM_SIZE, mPanelSize);
}

/* get primary screen changed in  990*/
void UKUIPanel::priScreenChanged(int x, int y, int width, int height)
{

    mcurrentScreenRect.setRect(x, y, width, height);
    setPanelGeometry();
    realign();
}



void UKUIPanel::caculateScreenGeometry()
{
    int priX, priY, priWid, priHei;
    priX = getScreenGeometry("x");
    priY = getScreenGeometry("y");
    priWid = getScreenGeometry("width");
    priHei = getScreenGeometry("height");

    mcurrentScreenRect.setRect(priX, priY, priWid, priHei);
    if(priWid==0){
    qDebug("初始化获取到的dbus信号错误，获取的宽度为0");
        mcurrentScreenRect = QApplication::desktop()->screenGeometry(0);
    }
}

int UKUIPanel::getScreenGeometry(QString methodName)
{
    int res = 0;
    QDBusMessage message = QDBusMessage::createMethodCall(DBUS_NAME,
                               DBUS_PATH,
                               DBUS_INTERFACE,
                               methodName);
    QDBusMessage response = QDBusConnection::sessionBus().call(message);
    if (response.type() == QDBusMessage::ReplyMessage)
    {
        if(response.arguments().isEmpty() == false) {
            int value = response.arguments().takeFirst().toInt();
            res = value;
            qDebug() << value;
        }
    } else {
        qDebug()<<methodName<<"called failed";
    }
    return res;
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

    if(flag_hw990=="hw_990"){
        int priX, priY, priWid, priHei;
        priX = getScreenGeometry("x");
        priY = getScreenGeometry("y");
        priWid = getScreenGeometry("width");
        priHei = getScreenGeometry("height");

        if(priWid==0){
            //华为990上获取到settings-daemo发送的屏幕信息有误（目前认为priWid为0 则为有误）
            qWarning()<<"get ScreenGeometry Info From Settings-Daemo Error";
            currentScreen = QGuiApplication::screens().at(0)->geometry();
        }
        currentScreen.setRect(priX, priY, priWid, priHei);
    }else{
        currentScreen=QGuiApplication::screens().at(0)->geometry();
    }

    if (isHorizontal()){
        rect.setHeight(qMax(PANEL_MINIMUM_SIZE, mPanelSize));
        if (mLengthInPercents)
            rect.setWidth(currentScreen.width() * mLength / 100.0);
        else{
            if (mLength <= 0)
                rect.setWidth(currentScreen.width() + mLength);
            else
                rect.setWidth(mLength);
        }
        rect.setWidth(qMax(rect.size().width(), mLayout->minimumSize().width()));

        // Horiz ......................
        switch (mAlignment)
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
        if (mPosition == IUKUIPanel::PositionTop)
        {
            if (mHidden)
                rect.moveBottom(currentScreen.top() + PANEL_HIDE_SIZE - 1);
            else
                rect.moveTop(currentScreen.top());
        }
        else
        {
            if (mHidden)
                rect.moveTop(currentScreen.bottom() - PANEL_HIDE_SIZE + 1);
            else
                rect.moveBottom(currentScreen.bottom() +1 );
        }
        qDebug()<<"ukui-panel Rect is :"<<rect;
    }
    else
    {
        // Vert panel ***************************
        rect.setWidth(qMax(PANEL_MINIMUM_SIZE, mPanelSize));
        if (mLengthInPercents)
            rect.setHeight(currentScreen.height() * mLength / 100.0);
        else
        {
            if (mLength <= 0)
                rect.setHeight(currentScreen.height() + mLength);
            else
                rect.setHeight(mLength);
        }

        rect.setHeight(qMax(rect.size().height(), mLayout->minimumSize().height()));

        // Vert .......................
        switch (mAlignment)
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
        if (mPosition == IUKUIPanel::PositionLeft)
        {
            if (mHidden)
                rect.moveRight(currentScreen.left() + PANEL_HIDE_SIZE - 1);
            else
                rect.moveLeft(currentScreen.left());
        }
        else
        {
            if (mHidden)
                rect.moveLeft(currentScreen.right() - PANEL_HIDE_SIZE + 1);
            else
                rect.moveRight(currentScreen.right());
        }
    }
    if (rect != geometry())
    {
        setFixedSize(rect.size());
        if (animate)
        {
            if (mAnimation == nullptr)
            {
                mAnimation = new QPropertyAnimation(this, "geometry");
                mAnimation->setEasingCurve(QEasingCurve::Linear);
                //Note: for hiding, the margins are set after animation is finished
                connect(mAnimation, &QAbstractAnimation::finished, [this] { if (mHidden) setMargins(); });
            }
            mAnimation->setDuration(mAnimationTime);
            mAnimation->setStartValue(geometry());
            mAnimation->setEndValue(rect);
            //Note: for showing-up, the margins are removed instantly
            if (!mHidden)
                setMargins();
            mAnimation->start();
        }
        else
        {
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
    args.append(gsettings->get(PANEL_POSITION_KEY).toInt());
    message.setArguments(args);
    QDBusConnection::sessionBus().send(message);
}

/*设置边距*/
void UKUIPanel::setMargins()
{
    if (mHidden)
    {
        if (isHorizontal())
        {
            if (mPosition == IUKUIPanel::PositionTop)
                mLayout->setContentsMargins(0, 0, 0, PANEL_HIDE_SIZE);
            else
                mLayout->setContentsMargins(0, PANEL_HIDE_SIZE, 0, 0);
        }
        else
        {
            if (mPosition == IUKUIPanel::PositionLeft)
                mLayout->setContentsMargins(0, 0, PANEL_HIDE_SIZE, 0);
            else
                mLayout->setContentsMargins(PANEL_HIDE_SIZE, 0, 0, 0);
        }
        if (!mVisibleMargin)
            setWindowOpacity(0.0);
    }
    else {
        mLayout->setContentsMargins(0, 0, 0, 0);
        if (!mVisibleMargin)
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

    if (mReserveSpace)
    {
        const QRect wholeScreen = QApplication::desktop()->geometry();
        const QRect rect = geometry();
        // NOTE: https://standards.freedesktop.org/wm-spec/wm-spec-latest.html
        // Quote from the EWMH spec: " Note that the strut is relative to the screen edge, and not the edge of the xinerama monitor."
        // So, we use the geometry of the whole screen to calculate the strut rather than using the geometry of individual monitors.
        // Though the spec only mention Xinerama and did not mention XRandR, the rule should still be applied.
        // At least openbox is implemented like this.
        switch (mPosition)
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
    int current = mScreenNum;

    for (int i = current; i < QApplication::desktop()->screenCount(); ++i)
        if (canPlacedOn(i, position))
            return i;

    for (int i = 0; i < current; ++i)
        if (canPlacedOn(i, position))
            return i;

    return 0;
}


///************************************************

// ************************************************/
//void UKUIPanel::showConfigDialog()
//{
//        if (mConfigDialog.isNull())
//            mConfigDialog = new ConfigPanelDialog(this, nullptr /*make it top level window*/);

//        mConfigDialog->showConfigPanelPage();
//        mStandaloneWindows->observeWindow(mConfigDialog.data());
//        mConfigDialog->show();
//        mConfigDialog->raise();
//        mConfigDialog->activateWindow();
//        WId wid = mConfigDialog->windowHandle()->winId();

//        KWindowSystem::activateWindow(wid);
//        KWindowSystem::setOnDesktop(wid, KWindowSystem::currentDesktop());

//    mConfigDialog = new ConfigPanelDialog(this, nullptr);
//    mConfigDialog->show();
//    //mConfigWidget->positionChanged();

//}


/************************************************

 ************************************************/
//void UKUIPanel::showAddPluginDialog()
//{
//    if (mConfigDialog.isNull())
//        mConfigDialog = new ConfigPanelDialog(this, nullptr /*make it top level window*/);

//    mConfigDialog->showConfigPluginsPage();
//    mStandaloneWindows->observeWindow(mConfigDialog.data());
//    mConfigDialog->show();
//    mConfigDialog->raise();
//    mConfigDialog->activateWindow();
//    WId wid = mConfigDialog->windowHandle()->winId();

//    KWindowSystem::activateWindow(wid);
//    KWindowSystem::setOnDesktop(wid, KWindowSystem::currentDesktop());
//}


/*右键　设置任务栏选项*/
void UKUIPanel::setUpPanel()
{
    if(QFileInfo::exists(QString("/usr/bin/ukui-control-center")))
    {
        QProcess *process =new QProcess(this);
        process->startDetached("/usr/bin/ukui-control-center  -d");
        process->deleteLater();
    }
    else
        qDebug()<<"not find /usr/bin/ukui-control-center";
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
    menu->addMenu(pmenu_panelsize);

    pmenuaction_s->setCheckable(true);
    pmenuaction_m->setCheckable(true);
    pmenuaction_l->setCheckable(true);
    pmenuaction_s->setChecked(gsettings->get(PANEL_SIZE_KEY).toInt()==SML_SIZE_PANEL_IN_CALC);
    pmenuaction_m->setChecked(gsettings->get(PANEL_SIZE_KEY).toInt()==MID_SIZE_PANEL_IN_CALC);
    pmenuaction_l->setChecked(gsettings->get(PANEL_SIZE_KEY).toInt()==MAX_SIZE_PANEL_IN_CALC);

    connect(pmenuaction_s,&QAction::triggered,[this] {
        gsettings->set(PANEL_SIZE_KEY,SML_SIZE_PANEL_IN_CALC);
        gsettings->set(ICON_SIZE_KEY,SML_ICON_SIZE_IN_CLAC);
    });
    connect(pmenuaction_m,&QAction::triggered,[this] {
        gsettings->set(PANEL_SIZE_KEY,MID_SIZE_PANEL_IN_CALC);
        gsettings->set(ICON_SIZE_KEY,MID_ICON_SIZE_IN_CLAC);
    });
    connect(pmenuaction_l,&QAction::triggered,[this] {
        gsettings->set(PANEL_SIZE_KEY,MAX_SIZE_PANEL_IN_CALC);
        gsettings->set(ICON_SIZE_KEY,MAX_ICON_SIZE_IN_CLAC);
    });
    pmenu_panelsize->setDisabled(mLockPanel);

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
    menu->addMenu(pmenu_positon);

    pmenuaction_top->setCheckable(true);
    pmenuaction_bottom->setCheckable(true);
    pmenuaction_left->setCheckable(true);
    pmenuaction_right->setCheckable(true);
    pmenuaction_top->setChecked(gsettings->get(PANEL_POSITION_KEY).toInt()==1);
    pmenuaction_bottom->setChecked(gsettings->get(PANEL_POSITION_KEY).toInt()==0);
    pmenuaction_left->setChecked(gsettings->get(PANEL_POSITION_KEY).toInt()==2);
    pmenuaction_right->setChecked(gsettings->get(PANEL_POSITION_KEY).toInt()==3);


    connect(pmenuaction_top,&QAction::triggered, [this] { setPosition(0,PositionTop,true);});
    connect(pmenuaction_bottom,&QAction::triggered, [this] { setPosition(0,PositionBottom,true);});
    connect(pmenuaction_left,&QAction::triggered, [this] { setPosition(0,PositionLeft,true);});
    connect(pmenuaction_right,&QAction::triggered, [this] { setPosition(0,PositionRight,true);});
    pmenu_positon->setWindowOpacity(0.9);
    pmenu_positon->setDisabled(mLockPanel);


    mSettings->beginGroup(mConfigGroup);
    QAction * hidepanel = menu->addAction(tr("Hide Panel"));
    hidepanel->setDisabled(mLockPanel);
    hidepanel->setCheckable(true);
    hidepanel->setChecked(mHidable);
    connect(hidepanel, &QAction::triggered, [this] {
        mSettings->beginGroup(mConfigGroup);
        mHidable = mSettings->value(CFG_KEY_HIDABLE, mHidable).toBool();
        mSettings->endGroup();
        if(mHidable)
            mHideTimer.stop();
        setHidable(!mHidable,true);
        mHidden=mHidable;
        mShowDelayTimer.start();
        time->start(1000);
    });
    mSettings->endGroup();
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
    if(gsettings->keys().contains(SHOW_TASKVIEW))
    {
        if(gsettings->get(SHOW_TASKVIEW).toBool()){
            gsettings->set(SHOW_TASKVIEW,false);
        }
        else
            gsettings->set(SHOW_TASKVIEW,true);
    }
}

/*右键　显示夜间模式按钮　选项*/
void UKUIPanel::showNightModeButton()
{
//    system("ukui-window-switch --show-workspace");
    if(gsettings->keys().contains(SHOW_NIGHTMODE))
    {
        if(gsettings->get(SHOW_NIGHTMODE).toBool()){
            gsettings->set(SHOW_NIGHTMODE,false);
        }
        else
            gsettings->set(SHOW_NIGHTMODE,true);
    }
}

/*设置任务栏高度*/
void UKUIPanel::setPanelSize(int value, bool save)
{
    if (mPanelSize != value)
    {
        mPanelSize = value;
        realign();

        if (save)
            saveSettings(true);
    }
}

/*设置任务栏图标大小*/
void UKUIPanel::setIconSize(int value, bool save)
{
    if (mIconSize != value)
    {
        mIconSize = value;
        mLayout->setLineSize(mIconSize);

        if (save)
            saveSettings(true);

        realign();
    }
}

void UKUIPanel::setLineCount(int value, bool save)
{
    if (mLineCount != value)
    {
        mLineCount = value;
        mLayout->setEnabled(false);
        mLayout->setLineCount(mLineCount);
        mLayout->setEnabled(true);

        if (save)
            saveSettings(true);

        realign();
    }
}


/************************************************

 ************************************************/
void UKUIPanel::setLength(int length, bool inPercents, bool save)
{
    if (mLength == length &&
            mLengthInPercents == inPercents)
        return;

    mLength = length;
    mLengthInPercents = inPercents;

    if (save)
        saveSettings(true);

    realign();
}


/************************************************

 ************************************************/
void UKUIPanel::setPosition(int screen, IUKUIPanel::Position position, bool save)
{
    if (mScreenNum == screen &&
            mPosition == position)
        return;

    mActualScreenNum = screen;
    mPosition = position;
    mLayout->setPosition(mPosition);

    if (save)
    {
        mScreenNum = screen;
        saveSettings(true);
    }

    // Qt 5 adds a new class QScreen and add API for setting the screen of a QWindow.
    // so we had better use it. However, without this, our program should still work
    // as long as XRandR is used. Since XRandR combined all screens into a large virtual desktop
    // every screen and their virtual siblings are actually on the same virtual desktop.
    // So things still work if we don't set the screen correctly, but this is not the case
    // for other backends, such as the upcoming wayland support. Hence it's better to set it.
    if(windowHandle())
    {
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

    //dbus 发任务栏位置的信号，开始菜单等监听
    QDBusMessage message=QDBusMessage::createSignal("/panel/settings", "com.ukui.panel.settings", "SendPanelSetings");
    gsettings->set(PANEL_POSITION_KEY,position);
    message<<position;
    QDBusConnection::sessionBus().send(message);

    setPanelGeometry(true);
}

/************************************************
 *
 ************************************************/
void UKUIPanel::setAlignment(Alignment value, bool save)
{
    if (mAlignment == value)
        return;

    mAlignment = value;

    if (save)
        saveSettings(true);

    realign();
}

/************************************************
 *
 ************************************************/
void UKUIPanel::setFontColor(QColor color, bool save)
{
    mFontColor = color;

    if (save)
        saveSettings(true);
}

/************************************************

 ************************************************/
void UKUIPanel::setBackgroundColor(QColor color, bool save)
{
    mBackgroundColor = color;

    if (save)
        saveSettings(true);
}

void UKUIPanel::setReserveSpace(bool reserveSpace, bool save)
{
    if (mReserveSpace == reserveSpace)
        return;

    mReserveSpace = reserveSpace;

    if (save)
        saveSettings(true);

    updateWmStrut();
}


/************************************************

 ************************************************/
QRect UKUIPanel::globalGeometry() const
{
    // panel is the the top-most widget/window, no calculation needed
    return geometry();
}


/************************************************

 ************************************************/
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
//        reloadPlugins("pc");
        mShowDelayTimer.start();
        break;

    case QEvent::Leave:
//        reloadPlugins("pad");
    case QEvent::DragLeave:
        mShowDelayTimer.stop();
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
    if(QGSettings::isSchemaInstalled(transparency_id)){
        transparency_gsettings = new QGSettings(transparency_id);

//    if(transparency_gsettings->keys().contains(TRANSPARENCY_KEY)){
    transparency=transparency_gsettings->get(TRANSPARENCY_KEY).toDouble()*255;
    this->update();
    connect(transparency_gsettings, &QGSettings::changed, this, [=] (const QString &key){
        if(key==TRANSPARENCY_KEY && transparency_action=="open"){
            transparency=transparency_gsettings->get(TRANSPARENCY_KEY).toDouble()*255;
            this->update();
        }
     });
    }else{
        transparency=0.75;
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
    color.setAlpha(transparency);
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
    menu = new QMenu(this);
    menu->setAttribute(Qt::WA_DeleteOnClose);

    /* @new features
     * //Plugin Menu  负责显示插件的菜单项，ukui３.0的设计暂时不需要插件菜单项
     * 关于后续的插件右键菜单的详细调整
     * 如果需要在任务栏菜单项上面添加 插件的菜单选项就放开此功能
     * 放开此功能可以丰富插件右键菜单，windows是这样做的
     */
    if (plugin)
    {
        QMenu *m = plugin->popupMenu();

        if (m)
        {
            //menu->addTitle(plugin->windowTitle());
            const auto actions = m->actions();
            for (auto const & action : actions)
            {
                action->setParent(menu);
                action->setDisabled(mLockPanel);
                menu->addAction(action);
            }
            delete m;
        }
    }

/*
//    menu->addTitle(QIcon(), tr("Panel"));
    menu->addAction(XdgIcon::fromTheme(QLatin1String("configure")),
                   tr("Configure Panel"),
                   this, SLOT(showConfigDialog())
                  )->setDisabled(mLockPanel);
    menu->addAction(XdgIcon::fromTheme("preferences-plugin"),
                   tr("Manage Widgets"),
                   this, SLOT(showAddPluginDialog())
                  )->setDisabled(mLockPanel);
*/

    menu->addAction(QIcon::fromTheme("document-page-setup-symbolic"),
                    tr("Set up Panel"),
                    this, SLOT(setUpPanel())
                    );

    menu->addSeparator();

    QAction * showtaskview = menu->addAction(tr("Show Taskview"));
    showtaskview->setCheckable(true);
    showtaskview->setChecked(gsettings->get(SHOW_TASKVIEW).toBool());
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

    if(nightmode_action == "show"){
        QAction * shownightmode = menu->addAction(tr("Show Nightmode"));
        shownightmode->setCheckable(true);
        shownightmode->setChecked(gsettings->get(SHOW_NIGHTMODE).toBool());
        connect(shownightmode, &QAction::triggered, [this] { showNightModeButton(); });
    }

    menu->addAction(XdgIcon::fromTheme(QLatin1String("configure")),
                    tr("Show Desktop"),
                    this, SLOT(showDesktop())
                    );

    menu->addSeparator();

    if(QFileInfo::exists(QString("/usr/bin/ukui-system-monitor"))){
        menu->addAction(XdgIcon::fromTheme(QLatin1String("configure")),
                        tr("Show System Monitor"),
                        this, SLOT(systeMonitor())
                        );
    }

    menu->addSeparator();

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
    m_lockAction = menu->addAction(tr("Lock This Panel"));
    m_lockAction->setCheckable(true);
    m_lockAction->setChecked(mLockPanel);
    connect(m_lockAction, &QAction::triggered, [this] { mLockPanel = !mLockPanel; saveSettings(false); });

    //Hidden features, lock the panel
    /*
    menu->addAction(XdgIcon::fromTheme(QLatin1String("configure")),
                   tr("Reset Panel"),
                   this, SLOT(panelReset())
                  )->setDisabled(mLockPanel);
*/

    if(QFileInfo::exists(QString("/usr/bin/ukui-about")))
    {
        QAction *about;
        about=new QAction(this);
        about->setText(tr("About Kylin"));
        menu->addAction(about);
        connect(about,&QAction::triggered, [this] {
            QProcess *process =new QProcess(this);
            process->startDetached("ukui-control-center -a");
        });

    }
#ifdef DEBUG
    menu->addSeparator();
    menu->addAction("Exit (debug only)", qApp, SLOT(quit()));
#endif

    /* Note: in multihead & multipanel setup the QMenu::popup/exec places the window
     * sometimes wrongly (it seems that this bug is somehow connected to misinterpretation
     * of QDesktopWidget::availableGeometry)
     */
    menu->setGeometry(calculatePopupWindowPos(QCursor::pos(), menu->sizeHint()));
    willShowWindow(menu);
    menu->show();
}

Plugin* UKUIPanel::findPlugin(const IUKUIPanelPlugin* iPlugin) const
{
    const auto plugins = mPlugins->plugins();
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

/************************************************

 ************************************************/
QRect UKUIPanel::calculatePopupWindowPos(const IUKUIPanelPlugin *plugin, const QSize &windowSize) const
{
    Plugin *panel_plugin = findPlugin(plugin);
    if (nullptr == panel_plugin)
    {
        qWarning() << Q_FUNC_INFO << "Wrong logic? Unable to find Plugin* for" << plugin << "known plugins follow...";
        const auto plugins = mPlugins->plugins();
        for (auto const & plug : plugins)
            qWarning() << plug->iPlugin() << plug;

        return QRect();
    }

    // Note: assuming there are not contentMargins around the "BackgroundWidget" (UKUIPanelWidget)
    return calculatePopupWindowPos(globalGeometry().topLeft() + panel_plugin->geometry().topLeft(), windowSize);
}


/************************************************

 ************************************************/
void UKUIPanel::willShowWindow(QWidget * w)
{
    mStandaloneWindows->observeWindow(w);
}

/************************************************

 ************************************************/
void UKUIPanel::pluginFlagsChanged(const IUKUIPanelPlugin * /*plugin*/)
{
    mLayout->rebuild();
}

/************************************************

 ************************************************/
QString UKUIPanel::qssPosition() const
{
    return positionToStr(position());
}

/************************************************

 ************************************************/
void UKUIPanel::pluginMoved(Plugin * plug)
{
    //get new position of the moved plugin
    bool found{false};
    QString plug_is_before;
    for (int i=0; i<mLayout->count(); ++i)
    {
        Plugin *plugin = qobject_cast<Plugin*>(mLayout->itemAt(i)->widget());
        if (plugin)
        {
            if (found)
            {
                //we found our plugin in previous cycle -> is before this (or empty as last)
                plug_is_before = plugin->settingsGroup();
                break;
            } else
                found = (plug == plugin);
        }
    }
    mPlugins->movePlugin(plug, plug_is_before);
}


/************************************************

 ************************************************/
void UKUIPanel::userRequestForDeletion()
{
    const QMessageBox::StandardButton ret
            = QMessageBox::warning(this, tr("Remove Panel", "Dialog Title") ,
                                   tr("Removing a panel can not be undone.\nDo you want to remove this panel?"),
                                   QMessageBox::Yes | QMessageBox::No);

    if (ret != QMessageBox::Yes) {
        return;
    }

    mSettings->beginGroup(mConfigGroup);
    const QStringList plugins = mSettings->value("plugins").toStringList();
    mSettings->endGroup();

    for(const QString& i : plugins)
        if (!i.isEmpty())
            mSettings->remove(i);

    mSettings->remove(mConfigGroup);

    emit deletedByUser(this);
}

void UKUIPanel::showPanel(bool animate)
{
    if (mHidable)
    {
        mHideTimer.stop();
        if (mHidden)
        {
            mHidden = false;
            setPanelGeometry(mAnimationTime > 0 && animate);
        }
    }
}

void UKUIPanel::hidePanel()
{
    if (mHidable && !mHidden
            && !mStandaloneWindows->isAnyWindowShown()
            )
        mHideTimer.start();
}

void UKUIPanel::hidePanelWork()
{
    if (!geometry().contains(QCursor::pos()))
    {
        if (!mStandaloneWindows->isAnyWindowShown())
        {
            mHidden = true;
            setPanelGeometry(mAnimationTime > 0);
        } else
        {
            mHideTimer.start();
        }
    }
    QDBusMessage message =QDBusMessage::createSignal("/panel", "org.ukui.panel.settings", "PanelHided");
    QDBusConnection::sessionBus().send(message);
}

void UKUIPanel::setHidable(bool hidable, bool save)
{
    if (mHidable == hidable)
        return;

    mHidable = hidable;

    if (save)
        saveSettings(true);

    realign();
}

void UKUIPanel::setVisibleMargin(bool visibleMargin, bool save)
{
    if (mVisibleMargin == visibleMargin)
        return;

    mVisibleMargin = visibleMargin;

    if (save)
        saveSettings(true);

    realign();
}

void UKUIPanel::setAnimationTime(int animationTime, bool save)
{
    if (mAnimationTime == animationTime)
        return;

    mAnimationTime = animationTime;

    if (save)
        saveSettings(true);
}

void UKUIPanel::setShowDelay(int showDelay, bool save)
{
    if (mShowDelayTimer.interval() == showDelay)
        return;

    mShowDelayTimer.setInterval(showDelay);

    if (save)
        saveSettings(true);
}

QString UKUIPanel::iconTheme() const
{
    return mSettings->value("iconTheme").toString();
}

void UKUIPanel::setIconTheme(const QString& iconTheme)
{
    UKUIPanelApplication *a = reinterpret_cast<UKUIPanelApplication*>(qApp);
    a->setIconTheme(iconTheme);
}

//void UKUIPanel::updateConfigDialog() const
//{
//    if (!mConfigDialog.isNull() && mConfigDialog->isVisible())
//    {
//        mConfigDialog->updateIconThemeSettings();
//        const QList<QWidget*> widgets = mConfigDialog->findChildren<QWidget*>();
//        for (QWidget *widget : widgets)
//            widget->update();
//    }
//}

bool UKUIPanel::isPluginSingletonAndRunnig(QString const & pluginId) const
{
    Plugin const * plugin = mPlugins->pluginByID(pluginId);
    if (nullptr == plugin)
        return false;
    else
        return plugin->iPlugin()->flags().testFlag(IUKUIPanelPlugin::SingleInstance);
}

void UKUIPanel::panelReset()
{
    QFile::remove(QString(qgetenv("HOME"))+"/.config/ukui/panel.conf");
    QFile::copy("/usr/share/ukui/panel.conf",QString(qgetenv("HOME"))+"/.config/ukui/panel.conf");
}

void UKUIPanel::connectToServer(){
    m_cloudInterface = new QDBusInterface("org.kylinssoclient.dbus",
                                          "/org/kylinssoclient/path",
                                          "org.freedesktop.kylinssoclient.interface",
                                          QDBusConnection::sessionBus());
    if (!m_cloudInterface->isValid())
    {
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
        mSettings->beginGroup(mConfigGroup);
        mSettings->sync();
        mLockPanel = mSettings->value(CFG_KEY_LOCKPANEL).toBool();
        mSettings->endGroup();
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
    if ((x < 100 || x > W - 100) && (y > H - 100 || y < 100)) return mPosition;
    if (y > (int)(x * slope) && y > (int)(H - x * slope)) return PositionBottom;
    if (y > (int)(x * slope) && y < (int)(H - x * slope)) return PositionLeft;
    if (y < (int)(x * slope) && y < (int)(H - x * slope)) return PositionTop;
    if (y < (int)(x * slope) && y > (int)(H - x * slope)) return PositionRight;
}


void UKUIPanel::mousePressEvent(QMouseEvent *event) {
    setCursor(Qt::DragMoveCursor);
}

void UKUIPanel::enterEvent(QEvent *event) {
       // setCursor(Qt::SizeVerCursor);
}

void UKUIPanel::leaveEvent(QEvent *event) {
    setCursor(Qt::ArrowCursor);
}

void UKUIPanel::mouseMoveEvent(QMouseEvent* event)
{
    if (mLockPanel) return;
    if (movelock == -1) {
        if (event->pos().ry() < 10) movelock = 0;
        else movelock = 1;
    }
    if (!movelock) {
        int panel_h = QApplication::screens().at(0)->size().height() - event->globalPos().ry();
        int icon_size = panel_h*0.695652174;
        setCursor(Qt::SizeVerCursor);
        if (panel_h <= PANEL_SIZE_LARGE && panel_h >= PANEL_SIZE_SMALL) {
            setPanelSize(panel_h, true);
            setIconSize(icon_size, true);
            gsettings->set(PANEL_SIZE_KEY, panel_h);
            gsettings->set(ICON_SIZE_KEY, icon_size);
        }
        return;
    }
    setCursor(Qt::SizeAllCursor);
    IUKUIPanel::Position currentpos = areaDivid(event->globalPos());
    if (oldpos != currentpos)
    {
        setPosition(0,currentpos,true);
        oldpos = currentpos;
    }
}

void UKUIPanel::mouseReleaseEvent(QMouseEvent* event)
{
    setCursor(Qt::ArrowCursor);
    realign();
    emit realigned();
    movelock = -1;
}
