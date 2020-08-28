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
#include "config/configpaneldialog.h"
#include "popupmenu.h"
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

#include <KWindowSystem/KWindowSystem>
#include <KWindowSystem/NETWM>
//#include <glib.h>
//#include <gio/gio.h>
#include <QGSettings>
// Turn on this to show the time required to load each plugin during startup
// #define DEBUG_PLUGIN_LOADTIME
#ifdef DEBUG_PLUGIN_LOADTIME
#include <QElapsedTimer>
#endif

// Config keys and groups
#define CFG_KEY_SCREENNUM          "desktop"
#define CFG_KEY_POSITION           "position"
#define CFG_KEY_PANELSIZE          "panelSize"
#define CFG_KEY_ICONSIZE           "iconSize"
#define CFG_KEY_LINECNT            "lineCount"
#define CFG_KEY_LENGTH             "width"
#define CFG_KEY_PERCENT            "width-percent"
#define CFG_KEY_ALIGNMENT          "alignment"
#define CFG_KEY_FONTCOLOR          "font-color"
#define CFG_KEY_BACKGROUNDCOLOR    "background-color"
#define CFG_KEY_OPACITY            "opacity"
#define CFG_KEY_RESERVESPACE       "reserve-space"
#define CFG_KEY_PLUGINS            "plugins"
#define CFG_KEY_PLUGINS1            "plugins-pc"
#define CFG_KEY_PLUGINS2           "plugins-pad"
#define CFG_KEY_HIDABLE            "hidable"
#define CFG_KEY_VISIBLE_MARGIN     "visible-margin"
#define CFG_KEY_ANIMATION          "animation-duration"
#define CFG_KEY_SHOW_DELAY         "show-delay"
#define CFG_KEY_LOCKPANEL          "lockPanel"

#define GSETTINGS_SCHEMA_SCREENSAVER "org.mate.interface"
#define KEY_MODE "gtk-theme"

#define PANEL_SIZE_LARGE  92
#define PANEL_SIZE_MEDIUM 70
#define PANEL_SIZE_SMALL  46
#define ICON_SIZE_LARGE   64
#define ICON_SIZE_MEDIUM  48
#define ICON_SIZE_SMALL   32

#define PANEL_SETTINGS      "org.ukui.panel.settings"
#define PANEL_MODEL         "panelmodel"
#define PANEL_HIDE          "panelhide"
#define PANEL_SIZE_KEY      "panelsize"
#define ICON_SIZE_KEY       "iconsize"
#define PANEL_POSITION_KEY  "panelposition"
#define SHOW_TASKVIEW       "showtaskview"
#define SHOW_NIGHTMODE      "shownightmode"

#define TRANSPARENCY_SETTINGS       "org.ukui.control-center.personalise"
#define TRANSPARENCY_KEY            "transparency"

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

QStringList pluginDesktopDirs();
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
    mModel(false),
    stModel(true),
    mHide(false),
    stHide(true),
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

    QString names_key(mConfigGroup);
    names_key += '/';
    names_key += QLatin1String(CFG_KEY_PLUGINS1);

    QString names_key1(mConfigGroup);
    names_key1 += '/';
    names_key1 += QLatin1String(CFG_KEY_PLUGINS2);

    pcmodel= new PanelPluginsModel(this,names_key , pluginDesktopDirs());
    padmodel = new PanelPluginsModel(this,names_key1 , pluginDesktopDirs());
    padmodel->onRemovePlugin();
    pcmodel->onRemovePlugin();

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

    mShowDelayTimer.setSingleShot(true);
    mShowDelayTimer.setInterval(PANEL_SHOW_DELAY);
    connect(&mShowDelayTimer, &QTimer::timeout, [this] { showPanel(mAnimationTime > 0); });

    /* 监听屏幕分辨路改变resized　和屏幕数量改变screenCountChanged
     * 或许存在无法监听到分辨率改变的情况（qt5.6），若出现则可换成
     * connect(QApplication::primaryScreen(),&QScreen::geometryChanged, this,&UKUIPanel::ensureVisible);
　　　*/
    connect(QApplication::desktop(), &QDesktopWidget::resized, this, &UKUIPanel::ensureVisible);
    connect(QApplication::desktop(), &QDesktopWidget::screenCountChanged, this, &UKUIPanel::ensureVisible);

    // connecting to QDesktopWidget::workAreaResized shouldn't be necessary,
    // as we've already connceted to QDesktopWidget::resized, but it actually
    connect(QApplication::desktop(), &QDesktopWidget::workAreaResized,
            this, &UKUIPanel::ensureVisible);


    connect(UKUi::Settings::globalSettings(), SIGNAL(settingsChanged()), this, SLOT(update()));
    connect(ukuiApp, SIGNAL(themeChanged()), this, SLOT(realign()));

    connect(mStandaloneWindows.data(), &WindowNotifier::firstShown, [this] { showPanel(true); });
    connect(mStandaloneWindows.data(), &WindowNotifier::lastHidden, this, &UKUIPanel::hidePanel);

    const QByteArray panelhide_id(PANEL_SETTINGS);
    if(QGSettings::isSchemaInstalled(panelhide_id)){
        panelhide_gsettings = new QGSettings(panelhide_id);
    }
    connect(panelhide_gsettings, &QGSettings::changed, this, [=] (const QString &key){
        if(key==PANEL_HIDE)
            mHide=gsettings->get(PANEL_HIDE).toBool();
        qDebug()<<"mHide"<<mHide;
        if(!stHide==mHide){
            if(mHide){
                mHidable=mHide;
            }
            else{
                mHidable=mHide;
            }
            stHide=mHide;
            mHidden = mHidable;
        }
    });


    const QByteArray panelmodel_id(PANEL_SETTINGS);
    if(QGSettings::isSchemaInstalled(panelmodel_id)){
        panelmodel_gsettings = new QGSettings(panelmodel_id);
        }
    connect(panelmodel_gsettings, &QGSettings::changed, this, [=] (const QString &key){
        if(key==PANEL_MODEL)
            mModel=gsettings->get(PANEL_MODEL).toBool();
        qDebug()<<"model"<<mModel;
//        readSettings(mModel);
//        ensureVisible();
        realign();
        if(!stModel==mModel){
            if(mModel){
                mHidable=true;
                resetloadPluginspad(padmodel,pcmodel);
            }
            else{
                mHidable=false;
                resetloadPluginspc(pcmodel,padmodel);
            }
            stModel=mModel;
            mHidden = mHidable;
        }
    });

    readSettings(mModel);

    ensureVisible();

    //loadPlugins();

    show();
    // show it the first time, despite setting

    if (mHidable){
        showPanel(false);
        QTimer::singleShot(PANEL_HIDE_FIRST_TIME, this, SLOT(hidePanel()));
    }
    UKUIPanelApplication *a = reinterpret_cast<UKUIPanelApplication*>(qApp);
    connect(a, &UKUIPanelApplication::primaryScreenChanged, this, &UKUIPanel::setPanelGeometry);

    const QByteArray id(PANEL_SETTINGS);
    gsettings = new QGSettings(id);

    updateStyleSheet();
    const QByteArray transparency_id(TRANSPARENCY_SETTINGS);
    if(QGSettings::isSchemaInstalled(transparency_id)){
        transparency_gsettings = new QGSettings(transparency_id);
        //setPanelBackground(true);
        }
    connect(transparency_gsettings, &QGSettings::changed, this, [=] (const QString &key){
        if(key==TRANSPARENCY_KEY)
        {
            //setPanelBackground(true);
            this->update();
        }
    });
    setPanelsize(PANEL_SIZE_MEDIUM);
    setIconsize(ICON_SIZE_MEDIUM);
    setPanelsize(PANEL_SIZE_SMALL);
    setIconsize(ICON_SIZE_SMALL);
}

/************************************************

 ************************************************/
void UKUIPanel::readSettings(bool cut)
{
    if(cut){
    // Read settings ......................................
    mSettings->beginGroup(mConfigGroup);
    mHidable=true;
    // Let Hidability be the first thing we readcalendarcalendarcalendar
    // so that every call to realign() is without side-effect
   // mHidable = mSettings->value(CFG_KEY_HIDABLE, mHidable).toBool();
    mHidden = mHidable;
    mVisibleMargin = mSettings->value(CFG_KEY_VISIBLE_MARGIN, mVisibleMargin).toBool();
    mAnimationTime = mSettings->value(CFG_KEY_ANIMATION, mAnimationTime).toInt();
    mShowDelayTimer.setInterval(mSettings->value(CFG_KEY_SHOW_DELAY, mShowDelayTimer.interval()).toInt());

    // By default we are using size & count from theme.
    setPanelSize(mSettings->value(CFG_KEY_PANELSIZE, PANEL_DEFAULT_SIZE).toInt(), false);
    setIconSize(mSettings->value(CFG_KEY_ICONSIZE, PANEL_DEFAULT_ICON_SIZE).toInt(), false);
    setLineCount(mSettings->value(CFG_KEY_LINECNT, PANEL_DEFAULT_LINE_COUNT).toInt(), false);

    setLength(mSettings->value(CFG_KEY_LENGTH, 100).toInt(),
              mSettings->value(CFG_KEY_PERCENT, true).toBool(),
              false);

    mScreenNum = mSettings->value(CFG_KEY_SCREENNUM, QApplication::desktop()->primaryScreen()).toInt();
    setPosition(mScreenNum,
                strToPosition(mSettings->value(CFG_KEY_POSITION).toString(), PositionBottom),
                false);

    setAlignment(Alignment(mSettings->value(CFG_KEY_ALIGNMENT, mAlignment).toInt()), false);

    QColor color = mSettings->value(CFG_KEY_FONTCOLOR, "").value<QColor>();
    if (color.isValid())
        setFontColor(color, true);

    setOpacity(mSettings->value(CFG_KEY_OPACITY, 100).toInt(), true);
    mReserveSpace = mSettings->value(CFG_KEY_RESERVESPACE, true).toBool();
    color = mSettings->value(CFG_KEY_BACKGROUNDCOLOR, "").value<QColor>();
    if (color.isValid())
        setBackgroundColor(color, true);

    mLockPanel = mSettings->value(CFG_KEY_LOCKPANEL, false).toBool();

    mSettings->endGroup();
    }
    else{
        mSettings->beginGroup(mConfigGroup);
          mHidable=false;
         //mHidable = mSettings->value(CFG_KEY_HIDABLE, mHidable).toBool();
         mHidden = mHidable;

        mVisibleMargin = mSettings->value(CFG_KEY_VISIBLE_MARGIN, mVisibleMargin).toBool();

        mAnimationTime = mSettings->value(CFG_KEY_ANIMATION, mAnimationTime).toInt();
        mShowDelayTimer.setInterval(mSettings->value(CFG_KEY_SHOW_DELAY, mShowDelayTimer.interval()).toInt());

        // By default we are using size & count from theme.
        setPanelSize(mSettings->value(CFG_KEY_PANELSIZE, PANEL_DEFAULT_SIZE).toInt(), false);
        setIconSize(mSettings->value(CFG_KEY_ICONSIZE, PANEL_DEFAULT_ICON_SIZE).toInt(), false);
        setLineCount(mSettings->value(CFG_KEY_LINECNT, PANEL_DEFAULT_LINE_COUNT).toInt(), false);

        setLength(mSettings->value(CFG_KEY_LENGTH, 100).toInt(),
                  mSettings->value(CFG_KEY_PERCENT, true).toBool(),
                  false);

        mScreenNum = mSettings->value(CFG_KEY_SCREENNUM, QApplication::desktop()->primaryScreen()).toInt();
        setPosition(mScreenNum,
                    strToPosition(mSettings->value(CFG_KEY_POSITION).toString(), PositionBottom),
                    false);

        setAlignment(Alignment(mSettings->value(CFG_KEY_ALIGNMENT, mAlignment).toInt()), false);

        QColor color = mSettings->value(CFG_KEY_FONTCOLOR, "").value<QColor>();
        if (color.isValid())
            setFontColor(color, true);

        setOpacity(mSettings->value(CFG_KEY_OPACITY, 100).toInt(), true);
        mReserveSpace = mSettings->value(CFG_KEY_RESERVESPACE, true).toBool();
        color = mSettings->value(CFG_KEY_BACKGROUNDCOLOR, "").value<QColor>();
        if (color.isValid())
            setBackgroundColor(color, true);

        mLockPanel = mSettings->value(CFG_KEY_LOCKPANEL, false).toBool();

        mSettings->endGroup();
    }

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

    mSettings->setValue(CFG_KEY_PANELSIZE, mPanelSize);
    mSettings->setValue(CFG_KEY_ICONSIZE, mIconSize);
    mSettings->setValue(CFG_KEY_LINECNT, mLineCount);

    mSettings->setValue(CFG_KEY_LENGTH, mLength);
    mSettings->setValue(CFG_KEY_PERCENT, mLengthInPercents);

    mSettings->setValue(CFG_KEY_SCREENNUM, mScreenNum);
    mSettings->setValue(CFG_KEY_POSITION, positionToStr(mPosition));

    mSettings->setValue(CFG_KEY_ALIGNMENT, mAlignment);

    mSettings->setValue(CFG_KEY_FONTCOLOR, mFontColor.isValid() ? mFontColor : QColor());
    mSettings->setValue(CFG_KEY_BACKGROUNDCOLOR, mBackgroundColor.isValid() ? mBackgroundColor : QColor());
    mSettings->setValue(CFG_KEY_OPACITY, mOpacity);
    mSettings->setValue(CFG_KEY_RESERVESPACE, mReserveSpace);

    mSettings->setValue(CFG_KEY_HIDABLE, mHidable);
    mSettings->setValue(PANEL_MODEL, mModel);
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
}


/************************************************

 ************************************************/
UKUIPanel::~UKUIPanel()
{
    mLayout->setEnabled(false);
    delete mAnimation;
    delete mConfigDialog.data();
    // do not save settings because of "user deleted panel" functionality saveSettings();
}


/************************************************

 ************************************************/
void UKUIPanel::show()
{
    QWidget::show();
    KWindowSystem::setOnDesktop(effectiveWinId(), NET::OnAllDesktops);
}


/************************************************

 ************************************************/
QStringList pluginDesktopDirs()
{
    QStringList dirs;
    dirs << QString(getenv("UKUI_PANEL_PLUGINS_DIR")).split(':', QString::SkipEmptyParts);
    dirs << QString("%1/%2").arg(XdgDirs::dataHome(), "/ukui/ukui-panel");
    dirs << PLUGIN_DESKTOPS_DIR;
    return dirs;
}


/************************************************

 ************************************************/
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
        connect(plugin, &Plugin::dragLeft, [this] { mShowDelayTimer.stop(); hidePanel(); });
    }
}

void UKUIPanel::resetloadPluginspc(PanelPluginsModel *pcmodel,PanelPluginsModel *padmodel){
    padmodel->onRemovePlugin();
    QString names_key1(mConfigGroup);
    names_key1 += '/';
    names_key1 += QLatin1String(CFG_KEY_PLUGINS1);
    pcmodel = new PanelPluginsModel(this,names_key1 , pluginDesktopDirs());
    mPlugins.reset(pcmodel);
    const auto plugins = mPlugins->plugins();
    for (auto const & plugin : plugins)
    {
        mLayout->addPlugin(plugin);
        connect(plugin, &Plugin::dragLeft, [this] { mShowDelayTimer.stop(); hidePanel(); });
    }

}

void UKUIPanel::resetloadPluginspad(PanelPluginsModel *padmodel,PanelPluginsModel *pcmodel){
    pcmodel->onRemovePlugin();
    QString names_key1(mConfigGroup);
    names_key1 += '/';
    names_key1 += QLatin1String(CFG_KEY_PLUGINS2);
    padmodel = new PanelPluginsModel(this,names_key1 , pluginDesktopDirs());

    mPlugins.reset(padmodel);
    const auto plugins = mPlugins->plugins();
    for (auto const & plugin : plugins)
    {
        mLayout->addPlugin(plugin);
        connect(plugin, &Plugin::dragLeft, [this] { mShowDelayTimer.stop(); hidePanel(); });
    }

}

/************************************************

 ************************************************/
int UKUIPanel::getReserveDimension()
{
    // return mHidable ? PANEL_HIDE_SIZE : qMax(PANEL_MINIMUM_SIZE, mPanelSize);
    return mHidable ? PANEL_HIDE_SIZE : qMax(PANEL_MINIMUM_SIZE, mPanelSize);
}

/*
 The setting frame of the old panel does not follow the main screen
 but can be displayed on any screen
 but the current desktop environment of ukui is set to follow the main screen
 All default parameters desktop()->screenGeometry are 0
 */
void UKUIPanel::setPanelGeometry(bool animate)
{
    const QRect currentScreen = QApplication::desktop()->screenGeometry(0);
    QRect rect;

    if (isHorizontal())
    {
        // Horiz panel ***************************
        rect.setHeight(qMax(PANEL_MINIMUM_SIZE, mPanelSize));
        if(mModel){
            if (mLengthInPercents){
                rect.setWidth(currentScreen.width() * mLength / 150.0);
            }
            else
            {
                if (mLength <= 0)
                    rect.setWidth(currentScreen.width() + mLength);
                else
                    rect.setWidth(mLength);
            }
        }

        else{

            if (mLengthInPercents){
                rect.setWidth(currentScreen.width() * mLength / 100.0);
            }
            else
            {
                if (mLength <= 0)
                    rect.setWidth(currentScreen.width() + mLength);
                else
                    rect.setWidth(mLength);
            }
        }

        rect.setWidth(qMax(rect.size().width(), mLayout->minimumSize().width()));

        // Horiz ......................
        //switch (mAlignment)
        switch(0)
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
                rect.moveBottom(currentScreen.bottom());
        }
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
}

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
    updateStyleSheet();
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
                                            /* Top    */  rect.top() + getReserveDimension(), rect.left(), rect.right(),
                                            /* Bottom */  0, 0, 0
                                            );
            break;

        case UKUIPanel::PositionBottom:
            KWindowSystem::setExtendedStrut(wid,
                                            /* Left   */  0, 0, 0,
                                            /* Right  */  0, 0, 0,
                                            /* Top    */  0, 0, 0,
                                            /* Bottom */  wholeScreen.bottom() - rect.bottom() + getReserveDimension(), rect.left(), rect.right()
                                            );
            break;

        case UKUIPanel::PositionLeft:
            KWindowSystem::setExtendedStrut(wid,
                                            /* Left   */  rect.left() + getReserveDimension(), rect.top(), rect.bottom(),
                                            /* Right  */  0, 0, 0,
                                            /* Top    */  0, 0, 0,
                                            /* Bottom */  0, 0, 0
                                            );

            break;

        case UKUIPanel::PositionRight:
            KWindowSystem::setExtendedStrut(wid,
                                            /* Left   */  0, 0, 0,
                                            /* Right  */  wholeScreen.right() - rect.right() + getReserveDimension(), rect.top(), rect.bottom(),
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


/************************************************

 ************************************************/
void UKUIPanel::showConfigDialog()
{
        if (mConfigDialog.isNull())
            mConfigDialog = new ConfigPanelDialog(this, nullptr /*make it top level window*/);

        mConfigDialog->showConfigPanelPage();
        mStandaloneWindows->observeWindow(mConfigDialog.data());
        mConfigDialog->show();
        mConfigDialog->raise();
        mConfigDialog->activateWindow();
        WId wid = mConfigDialog->windowHandle()->winId();

        KWindowSystem::activateWindow(wid);
        KWindowSystem::setOnDesktop(wid, KWindowSystem::currentDesktop());

    mConfigDialog = new ConfigPanelDialog(this, nullptr);
    mConfigDialog->show();
    //mConfigWidget->positionChanged();

}


/************************************************

 ************************************************/
void UKUIPanel::showAddPluginDialog()
{
    if (mConfigDialog.isNull())
        mConfigDialog = new ConfigPanelDialog(this, nullptr /*make it top level window*/);

    mConfigDialog->showConfigPluginsPage();
    mStandaloneWindows->observeWindow(mConfigDialog.data());
    mConfigDialog->show();
    mConfigDialog->raise();
    mConfigDialog->activateWindow();
    WId wid = mConfigDialog->windowHandle()->winId();

    KWindowSystem::activateWindow(wid);
    KWindowSystem::setOnDesktop(wid, KWindowSystem::currentDesktop());
}


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
    if(QFileInfo::exists(QString("/usr/bin/mate-system-monitor")) || QFileInfo::exists(QString("/usr/bin/ukui-system-monitor")))
    {
        QProcess *process =new QProcess(this);
        if(QFileInfo::exists(QString("/usr/bin/ukui-system-monitor")))
            process->startDetached("/usr/bin/ukui-system-monitor");
        else
            process->startDetached("/usr/bin/mate-system-monitor");
    }
    else
        qDebug()<<"not find /usr/bin/mate-system-monitor or /usr/bin/ukui-system-monitor";
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
    pmenuaction_s->setChecked(gsettings->get(PANEL_SIZE_KEY).toInt()==PANEL_SIZE_SMALL);
    pmenuaction_m->setChecked(gsettings->get(PANEL_SIZE_KEY).toInt()==PANEL_SIZE_MEDIUM);
    pmenuaction_l->setChecked(gsettings->get(PANEL_SIZE_KEY).toInt()==PANEL_SIZE_LARGE);

    connect(pmenuaction_s,&QAction::triggered,[this] {
        setPanelsize(PANEL_SIZE_SMALL);
        setIconsize(ICON_SIZE_SMALL);
    });
    connect(pmenuaction_m,&QAction::triggered,[this] {
        setPanelsize(PANEL_SIZE_MEDIUM);
        setIconsize(ICON_SIZE_MEDIUM);
    });
    connect(pmenuaction_l,&QAction::triggered,[this] {
        setPanelsize(PANEL_SIZE_LARGE);
        setIconsize(ICON_SIZE_LARGE);
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


    connect(pmenuaction_top,&QAction::triggered, [this] { setPanelPosition(PositionTop);});
    connect(pmenuaction_bottom,&QAction::triggered, [this] { setPanelPosition(PositionBottom);});
    connect(pmenuaction_left,&QAction::triggered, [this] { setPanelPosition(PositionLeft);});
    connect(pmenuaction_right,&QAction::triggered, [this] { setPanelPosition(PositionRight);});
    pmenu_positon->setWindowOpacity(0.9);
    pmenu_positon->setDisabled(mLockPanel);

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

void UKUIPanel::panelhide()
{
   // qDebug()<<"hide is :"<<hide;
if(gsettings->get(PANEL_HIDE).toBool())
            gsettings->set(PANEL_HIDE,false);
else
            gsettings->set(PANEL_HIDE,true);
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
void UKUIPanel::updateStyleSheet()
{
//    QStringList sheet;
//    sheet << QString("UKUIPanel #BackgroundWidget { background-color: rgba(19,22,28,0.9); }");
//    setStyleSheet(sheet.join("\n"));
}



/************************************************

 ************************************************/
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


void UKUIPanel::setPanelmodel(bool value, bool save)
{
    if (mModel != value)
    {
        mModel = value;
        realign();

        if (save)
            saveSettings(true);
    }
}

/************************************************

 ************************************************/

void UKUIPanel::setIconSize(int value, bool save)
{
    if (mIconSize != value)
    {
        mIconSize = value;
        updateStyleSheet();
        mLayout->setLineSize(mIconSize);

        if (save)
            saveSettings(true);

        realign();
    }
}


/************************************************

 ************************************************/
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
    updateStyleSheet();

    if (save)
        saveSettings(true);
}

/************************************************

 ************************************************/
void UKUIPanel::setBackgroundColor(QColor color, bool save)
{
    mBackgroundColor = color;
    updateStyleSheet();

    if (save)
        saveSettings(true);
}

/*设置任务栏的背景色，可通过gsetting修改*/
void UKUIPanel::setPanelBackground(bool effective)
{
    if(effective)
    {
        QStringList sheet;
        sheet << QString("UKUIPanel #BackgroundWidget { background-color: rgba(19,22,22,%1); }").arg(transparency_gsettings->get(TRANSPARENCY_KEY).toDouble());
        setStyleSheet(sheet.join("\n"));
    }
    else
        updateStyleSheet();
}


/************************************************
 *
 ************************************************/
void UKUIPanel::setOpacity(int opacity, bool save)
{
    mOpacity = opacity;
    updateStyleSheet();

    if (save)
        saveSettings(true);
}


/************************************************
 *
 ************************************************/
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
        mShowDelayTimer.start();
        break;

    case QEvent::Leave:
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
    double tran=transparency_gsettings->get(TRANSPARENCY_KEY).toDouble()*255;
    p.setBrush(QBrush(QColor(250,250,250,tran)));

    p.setRenderHint(QPainter::Antialiasing);
    p.drawRoundedRect(opt.rect,20,90);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}


/*Right-Clicked Menu of ukui-panel
 * it's a Popup Menu
*/
void UKUIPanel::showPopupMenu(Plugin *plugin)
{
    menu = new PopupMenu(tr("Panel"), this);
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
            menu->addTitle(plugin->windowTitle());
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
    menu->addTitle(QIcon(), tr("Panel"));
    menu->addAction(XdgIcon::fromTheme(QLatin1String("configure")),
                   tr("Configure Panel"),
                   this, SLOT(showConfigDialog())
                  )->setDisabled(mLockPanel);
    menu->addAction(XdgIcon::fromTheme("preferences-plugin"),
                   tr("Manage Widgets"),
                   this, SLOT(showAddPluginDialog())
                  )->setDisabled(mLockPanel);
    */

    menu->addAction(QIcon(HighLightEffect::drawSymbolicColoredPixmap(QPixmap::fromImage(QIcon::fromTheme("document-page-setup").pixmap(24,24).toImage()))),
                    tr("Set up Panel"),
                    this, SLOT(setUpPanel())
                    )->setDisabled(mLockPanel);

    menu->addSeparator();

    QAction * showtaskview = menu->addAction(tr("Show Taskview"));
    showtaskview->setDisabled(mLockPanel);
    showtaskview->setCheckable(true);
    showtaskview->setChecked(gsettings->get(SHOW_TASKVIEW).toBool());
    connect(showtaskview, &QAction::triggered, [this] { showTaskView(); });

//    QAction * sWitchToHide = menu->addAction(tr("hide panel"));
//    sWitchToHide->setDisabled(mLockPanel);
//    sWitchToHide->setCheckable(true);
//    sWitchToHide->setChecked(gsettings->get(PANEL_HIDE).toBool());
//    connect(sWitchToHide, &QAction::triggered, [this] { panelhide(); });

#if (QT_VERSION > QT_VERSION_CHECK(5,7,0))
    QAction * shownightmode = menu->addAction(tr("Show Nightmode"));
    shownightmode->setDisabled(mLockPanel);
    shownightmode->setCheckable(true);
    shownightmode->setChecked(gsettings->get(SHOW_NIGHTMODE).toBool());
    connect(shownightmode, &QAction::triggered, [this] { showNightModeButton(); });
#endif

    menu->addAction(XdgIcon::fromTheme(QLatin1String("configure")),
                    tr("Show Desktop"),
                    this, SLOT(showDesktop())
                    );

    menu->addSeparator();

    menu->addAction(XdgIcon::fromTheme(QLatin1String("configure")),
                    tr("Show System Monitor"),
                    this, SLOT(systeMonitor())
                    );

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
    QAction * act_lock = menu->addAction(tr("Lock This Panel"));
    act_lock->setCheckable(true);
    act_lock->setChecked(mLockPanel);
    connect(act_lock, &QAction::triggered, [this] { mLockPanel = !mLockPanel; saveSettings(false); });

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
        about->setDisabled(mLockPanel);
        connect(about,&QAction::triggered, [this] {
            QProcess *process =new QProcess(this);
            process->startDetached("/usr/bin/ukui-about");
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

void UKUIPanel::updateConfigDialog() const
{
    if (!mConfigDialog.isNull() && mConfigDialog->isVisible())
    {
        mConfigDialog->updateIconThemeSettings();
        const QList<QWidget*> widgets = mConfigDialog->findChildren<QWidget*>();
        for (QWidget *widget : widgets)
            widget->update();
    }
}

bool UKUIPanel::isPluginSingletonAndRunnig(QString const & pluginId) const
{
    Plugin const * plugin = mPlugins->pluginByID(pluginId);
    if (nullptr == plugin)
        return false;
    else
        return plugin->iPlugin()->flags().testFlag(IUKUIPanelPlugin::SingleInstance);
}

void UKUIPanel::setPanelPosition(Position position)
{
    QDBusMessage message=QDBusMessage::createSignal("/panel/settings", "com.ukui.panel.settings", "SendPanelSetings");
    if(position==PositionTop)
    {
        setPosition(0,PositionTop,true);
        gsettings->set(PANEL_POSITION_KEY,1);
        message<<1;
        QDBusConnection::sessionBus().send(message);
    }
    else if(position==PositionLeft)
    {
        setPosition(0,PositionLeft,true);
        gsettings->set(PANEL_POSITION_KEY,2);
        message<<2;
        QDBusConnection::sessionBus().send(message);
    }
    else if(position==PositionRight)
    {
        this->setPosition(0,PositionRight,true);
        gsettings->set(PANEL_POSITION_KEY,3);
        message<<3;
        QDBusConnection::sessionBus().send(message);
    }
    else
    {
        setPosition(0,PositionBottom,true);
        gsettings->set(PANEL_POSITION_KEY,0);
        message<<0;
        QDBusConnection::sessionBus().send(message);
    }
}

/*ukui-panel use gsettings to set panelsize & iconsize
 * it need to emit signals to other application
 * and for users to adjust through the command lines
*/
void UKUIPanel::setPanelsize(int panelsize)
{
    setPanelSize(panelsize,true);
    gsettings->set(PANEL_SIZE_KEY,panelsize);
}

void UKUIPanel::setIconsize(int iconsize)
{
    setIconSize(iconsize,true);
    gsettings->set(ICON_SIZE_KEY,iconsize);
}

/*Extra code will be deleted*/
void UKUIPanel::changeSizeToSmall()
{
    setPanelSize(PANEL_SIZE_SMALL,true);
    setIconSize(ICON_SIZE_SMALL,true);
    gsettings->set(PANEL_SIZE_KEY,PANEL_SIZE_SMALL);
    gsettings->set(ICON_SIZE_KEY,ICON_SIZE_SMALL);
}

void UKUIPanel::changeSizeToMedium()
{
    setPanelSize(PANEL_SIZE_MEDIUM,true);
    setIconSize(ICON_SIZE_MEDIUM,true);
    gsettings->set(PANEL_SIZE_KEY,PANEL_SIZE_MEDIUM);
    gsettings->set(ICON_SIZE_KEY,ICON_SIZE_MEDIUM);
}

void UKUIPanel::changeSizeToLarge()
{
    setPanelSize(PANEL_SIZE_LARGE,true);
    setIconSize(ICON_SIZE_MEDIUM,true);
    gsettings->set(PANEL_SIZE_KEY,PANEL_SIZE_LARGE);
    gsettings->set(ICON_SIZE_KEY,ICON_SIZE_LARGE);
}

void UKUIPanel::panelReset()
{
    QFile::remove(QString(qgetenv("HOME"))+"/.config/ukui/panel.conf");
    QFile::copy("/usr/share/ukui/panel.conf",QString(qgetenv("HOME"))+"/.config/ukui/panel.conf");
}
void UKUIPanel::panelBackgroundChange()
{
    qDebug()<<"panel background change ***";
    if(mConfigDialog.isNull())
    {
        mConfigDialog = new ConfigPanelDialog(this, nullptr);
    }
    mConfigDialog->backgroundChange();
}

void UKUIPanel::panelmodelpadChange(){
    setPanelmodel(true,true);
    gsettings->set(PANEL_MODEL,true);

}

void UKUIPanel::panelmodelpcChange(){
    setPanelmodel(false,true);
    gsettings->set(PANEL_MODEL,false);

}
