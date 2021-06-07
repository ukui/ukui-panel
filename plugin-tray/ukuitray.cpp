/*
 * Copyright (C) 2019 Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU  Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/&gt;.
 *
 */

/********************************************************************
  Inspired by freedesktops tint2 ;)

*********************************************************************/

/*
 * Some code in this file is commented but not deleted
 * because these commented codes are the old loading trayApp
 * I want to wait until the new version loading trayApp is stable before deleting
*/

#include <QApplication>
#include <QDebug>
#include <QTimer>
#include <QtX11Extras/QX11Info>
#include <QPainter>
#include <QtDBus/QtDBus>
#include "trayicon.h"
#include "../panel/iukuipanel.h"
#include "../panel/common/ukuigridlayout.h"
#include "ukuitray.h"
#include "xfitman.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/extensions/Xrender.h>
#include <X11/extensions/Xdamage.h>
#include <xcb/xcb.h>
#include <xcb/damage.h>
#include "ukuitrayplugin.h"
#undef Bool // defined as int in X11/Xlib.h

#include "../panel/iukuipanelplugin.h"

#include <QPushButton>
#include <QToolButton>
#include <QLabel>
#include <QMouseEvent>

#include "traydynamicgsetting.h"

#define _NET_SYSTEM_TRAY_ORIENTATION_HORZ 0
#define _NET_SYSTEM_TRAY_ORIENTATION_VERT 1

#define SYSTEM_TRAY_REQUEST_DOCK    0
#define SYSTEM_TRAY_BEGIN_MESSAGE   1
#define SYSTEM_TRAY_CANCEL_MESSAGE  2

#define XEMBED_EMBEDDED_NOTIFY  0
#define XEMBED_MAPPED          (1 << 0)

#define mWinWidth 46
#define mWinHeight 46

/* qt会将glib里的signals成员识别为宏，所以取消该宏
 * 后面如果用到signals时，使用Q_SIGNALS代替即可
 **/
#ifdef signals
#undef signals
#endif
extern "C" {
#include <glib.h>
#include <gio/gio.h>
#include <dconf/dconf.h>
}

#define KEYBINDINGS_CUSTOM_SCHEMA "org.ukui.panel.tray"
#define KEYBINDINGS_CUSTOM_DIR "/org/ukui/tray/keybindings/"

#define ACTION_KEY "action"
#define RECORD_KEY "record"
#define BINDING_KEY "binding"
#define NAME_KEY "name"

#define TRAY_APP_COUNT 16

#define PANEL_SETTINGS "org.ukui.panel.settings"
#define PANEL_LINES    "panellines"
#define TRAY_LINE      "traylines"
#define PANEL_SIZE     "panelsize"
#define ICON_SIZE      "iconsize"

bool flag=false;
extern storageBarStatus status;

UKUITray::UKUITray(UKUITrayPlugin *plugin, QWidget *parent):
    QFrame(parent),
    mValid(false),
    mTrayId(0),
    mDamageEvent(0),
    mDamageError(0),
    mIconSize(TRAY_ICON_SIZE_DEFAULT, TRAY_ICON_SIZE_DEFAULT),
    mPlugin(plugin),
    mDisplay(QX11Info::display())
{
    m_pwidget = NULL;
    status=ST_HIDE;

    const QByteArray id(PANEL_SETTINGS);
    if(QGSettings::isSchemaInstalled(id))
    {
        settings=new QGSettings(id);
    }
    connect(settings, &QGSettings::changed, this, [=] (const QString &key){
        if(key==ICON_SIZE){
            trayIconSizeRefresh();
            handleStorageUi();
        }
    });

    mLayout = new UKUi::GridLayout(this);
    setLayout(mLayout);
    _NET_SYSTEM_TRAY_OPCODE = XfitMan::atom("_NET_SYSTEM_TRAY_OPCODE");
    // Init the selection later just to ensure that no signals are sent until
    // after construction is done and the creating object has a chance to connect.
    QTimer::singleShot(3, this, SLOT(startTray()));
    mBtn =new StorageArrow(this);
    mLayout->addWidget(mBtn);
    storageFrame=new UKUIStorageFrame;
    // fix 任务栏可隐藏时，鼠标放到traystorage时任务栏也会隐藏问题
    mPlugin->willShowWindow(storageFrame);
    mStorageLayout = new UKUi::GridLayout(storageFrame);
    storageFrame->setLayout(mStorageLayout);
    handleStorageUi();
    connect(mBtn,SIGNAL(clicked()),this,SLOT(storageBar()));
    mBtn->setVisible(false);
    realign();
    //进入桌面后可能存在的托盘区域未刷新问题
    QTimer::singleShot(1000,[this] { realign(); trayIconSizeRefresh(); });
    //针对ukui桌面环境特殊应用的处理，保证稳定性
    QTimer::singleShot(3000,[this] { panelStartupFcitx();});
    QTimer::singleShot(30000,[this] {
        //QProcess::execute("sh /usr/share/ukui/ukui-panel/plugin-tray/trayAppSetting.sh");
    });
    QDBusConnection::sessionBus().connect(QString(), QString("/taskbar/click"), "com.ukui.panel.plugins.taskbar", "sendToUkuiDEApp", this, SLOT(hideStorageWidget()));
}

UKUITray::~UKUITray()
{
//    for(int i  = 0; i < mIcons.size(); i++)
//    {
//        if(mIcons[i])
//        {
//            mIcons[i]->deleteLater();
//            mIcons[i] = NULL;
//        }
//    }
//    mIcons.clear();
//    mStorageIcons.clear();
//    mTrayIcons.clear();
    freezeApp();
    stopTray();
}

void UKUITray::storageBar()
{
    if(mStorageIcons.size()<1) mBtn->setVisible(false);
    if(status==ST_HIDE)
    {
        status = ST_SHOW;
        showAndHideStorage(false);
    }
    else
    {
        status = ST_HIDE;
        showAndHideStorage(true);
    }
    realign();
}

void UKUITray::hideStorageWidget()
{
    status = ST_HIDE;
    showAndHideStorage(true);
}

void UKUITray::showAndHideStorage(bool storageStatus)
{
    if(storageStatus){
        storageFrame->hide();
    }else{
        storageFrame->show();
        handleStorageUi();
    }
}

bool UKUITray::nativeEventFilter(const QByteArray &eventType, void *message, long *)
{
    if (eventType != "xcb_generic_event_t")
        return false;

    xcb_generic_event_t* event = static_cast<xcb_generic_event_t *>(message);

    TrayIcon* icon;
    int event_type = event->response_type & ~0x80;

    switch (event_type)
    {
    /* 监听到托盘应用的启动事件
     */
    case ClientMessage:
        clientMessageEvent(event);
        repaint();
        break;

        //        case ConfigureNotify:
        //            icon = findIcon(event->xconfigure.window);
        //            if (icon)
        //                icon->configureEvent(&(event->xconfigure));
        //            break;


        /*
         * 监听到托盘应用的退出事件
         */
    case DestroyNotify: {
        /*
         * 在这里本应该刷新收纳栏 handleStorageUi()
         * 由于收纳栏不刷新可能会导致任务栏的崩溃
         * 在freeze信号接受的地方freezeTrayApp（）同样进行刷新收纳栏的操作
         * 但是如果在这里刷新收纳栏目会导致收纳栏的应用异常退出
         */

        unsigned long event_window;
        event_window = reinterpret_cast<xcb_destroy_notify_event_t*>(event)->window;
        icon = findIcon(event_window);
        if (icon)
        {
            icon->windowDestroyed(event_window);
            mTrayIcons.removeAll(icon);
            mStorageIcons.removeAll(icon);
            delete icon;
        }
        break;
    }
    default:
        if (event_type == mDamageEvent + XDamageNotify)
        {
            xcb_damage_notify_event_t* dmg = reinterpret_cast<xcb_damage_notify_event_t*>(event);
            icon = findIcon(dmg->drawable);
            if (icon)
                icon->update();
        }
        break;
    }

    return false;
}

void UKUITray::realign()
{
    mLayout->setEnabled(false);
    IUKUIPanel *panel = mPlugin->panel();

    /*　刷新托盘收纳栏界面
     * 不要在这里对收纳栏的图标执setFixedSize和setIconSize的操作
     * 这些应该在handleStorageUi()函数中执行
     * handleStorageUi();
    */
    if (panel->isHorizontal()){

        dynamic_cast<UKUi::GridLayout*>(mLayout)->setRowCount(panel->lineCount());
        dynamic_cast<UKUi::GridLayout*>(mLayout)->setColumnCount(0);
    }
    else{
        dynamic_cast<UKUi::GridLayout*>(mLayout)->setColumnCount(panel->lineCount());
        dynamic_cast<UKUi::GridLayout*>(mLayout)->setRowCount(0);
    }

    if(storageFrame){
        int storageFramePosition;
        if(mPlugin->panel()->isHorizontal()){
            storageFramePosition=(storageFrame->size().width()-mBtn->size().width())/2;
            storageFrame->setGeometry(mPlugin->panel()->calculatePopupWindowPos(mapToGlobal(QPoint(-storageFramePosition,0)), storageFrame->size()));
        }
        else{
            storageFramePosition=(storageFrame->size().height()-mBtn->size().height())/2;
            storageFrame->setGeometry(mPlugin->panel()->calculatePopupWindowPos(mapToGlobal(QPoint(0,-storageFramePosition)), storageFrame->size()));
        }
    }

    if(mStorageIcons.size()<1) mBtn->setVisible(false);
    mLayout->setEnabled(true);
#if 0
    //设置任务栏
    for(int i=0;i<mTrayIcons.size();i++){
    TrayIcon *trayicon = mTrayIcons[i];
    connect(trayicon,&TrayIcon::iconIsMoving,[this](Window id){moveIconToStorage(id);});
    }
    for(int i=0;i<mStorageIcons.size();i++){
    TrayIcon *trayicon = mStorageIcons[i];
    connect(trayicon,&TrayIcon::iconIsMoving,[this](Window id){moveIconToTray(id);});
    }
#endif
}

void UKUITray::trayIconSizeRefresh()
{
    for(int i=0;i<mIcons.size();i++){
        if(mIcons.at(i)){
            if(mPlugin->panel()->isHorizontal()){
                mIcons.at(i)->setFixedSize(mPlugin->panel()->iconSize(),mPlugin->panel()->panelSize());
            }else{
                mIcons.at(i)->setFixedSize(mPlugin->panel()->panelSize(),mPlugin->panel()->iconSize());
            }
            mIcons.at(i)->setIconSize(QSize(mPlugin->panel()->iconSize()/2,mPlugin->panel()->iconSize()/2));
        }else{
            qDebug()<<"错误的托盘图标";
        }
    }
}

/*监听托盘事件*/
void UKUITray::clientMessageEvent(xcb_generic_event_t *e)
{
    unsigned long opcode;
    unsigned long message_type;
    Window id;
    xcb_client_message_event_t* event = reinterpret_cast<xcb_client_message_event_t*>(e);
    uint32_t* data32 = event->data.data32;
    message_type = event->type;
    opcode = data32[1];
    if(message_type != _NET_SYSTEM_TRAY_OPCODE)
        return;

    switch (opcode)
    {
    case SYSTEM_TRAY_REQUEST_DOCK:
        id = data32[2];
        if(id){
            regulateIcon(&id);
            trayIconSizeRefresh();
        }


    case SYSTEM_TRAY_BEGIN_MESSAGE:
    case SYSTEM_TRAY_CANCEL_MESSAGE:
//        qDebug() << "we don't show balloon messages.";
        break;

    default:
        //            if (opcode == xfitMan().atom("_NET_SYSTEM_TRAY_MESSAGE_DATA"))
        //                qDebug() << "message from dockapp:" << e->data.b;
        //            else
        //                qDebug() << "SYSTEM_TRAY : unknown message type" << opcode;
        break;
    }
}

/*遍历mIcons中的TrayIcon*/
TrayIcon* UKUITray::findIcon(Window id)
{
#if (QT_VERSION < QT_VERSION_CHECK(5,7,0))
    for(int i=0;i<mIcons.size();i++){
        TrayIcon *icon=mIcons[i];
#endif
#if (QT_VERSION >= QT_VERSION_CHECK(5,7,0))
    for(TrayIcon* icon : qAsConst(mIcons) ){
#endif
        if (icon->iconId() == id || icon->windowId() == id)
            return icon;
    }
    return 0;
}

TrayIcon* UKUITray::findTrayIcon(Window id)
{
#if (QT_VERSION < QT_VERSION_CHECK(5,7,0))
    for(int i=0;i<mTrayIcons.size();i++){
    TrayIcon *trayicon = mTrayIcons[i];
#endif
#if (QT_VERSION >= QT_VERSION_CHECK(5,7,0))
    for(TrayIcon* trayicon : qAsConst(mTrayIcons) ){
#endif
        if (trayicon->iconId() == id || trayicon->windowId() == id)
            return trayicon;
    }
    return 0;
}

TrayIcon* UKUITray::findStorageIcon(Window id)
{
#if (QT_VERSION < QT_VERSION_CHECK(5,7,0))
    for(int i=0;i<mStorageIcons.size();i++){
    TrayIcon *storageicon=mStorageIcons[i];
#endif
#if (QT_VERSION >= QT_VERSION_CHECK(5,7,0))
    for(TrayIcon* storageicon :qAsConst(mStorageIcons)){
#endif
        if (storageicon->iconId() == id || storageicon->windowId() == id)
            return storageicon;
    }
    return 0;
}

void UKUITray::setIconSize()
{
    mIconSize=QSize(mPlugin->panel()->iconSize()/2,mPlugin->panel()->iconSize()/2);
    unsigned long size = qMin(mIconSize.width(), mIconSize.height());
    XChangeProperty(mDisplay,
                    mTrayId,
                    XfitMan::atom("_NET_SYSTEM_TRAY_ICON_SIZE"),
                    XA_CARDINAL,
                    32,
                    PropModeReplace,
                    (unsigned char*)&size,
                    1);
}

VisualID UKUITray::getVisual()
{
    VisualID visualId = 0;
    Display* dsp = mDisplay;

    XVisualInfo templ;
    templ.screen=QX11Info::appScreen();
    templ.depth=32;
    templ.c_class=TrueColor;

    int nvi;
    XVisualInfo* xvi = XGetVisualInfo(dsp, VisualScreenMask|VisualDepthMask|VisualClassMask, &templ, &nvi);

    if (xvi)
    {
        int i;
        XRenderPictFormat* format;
        for (i = 0; i < nvi; i++){
            format = XRenderFindVisualFormat(dsp, xvi[i].visual);
            if (format &&
                    format->type == PictTypeDirect &&
                    format->direct.alphaMask)
            {
                visualId = xvi[i].visualid;
                break;
            }
        }
        XFree(xvi);
    }

    return visualId;
}

/**
 * @brief UKUITray::startTray
 * freedesktop系统托盘规范
 */
void UKUITray::startTray()
{
    Display* dsp = mDisplay;
    Window root = QX11Info::appRootWindow();

    QString s = QString("_NET_SYSTEM_TRAY_S%1").arg(DefaultScreen(dsp));
    Atom _NET_SYSTEM_TRAY_S = XfitMan::atom(s.toLatin1());
    //this limit the tray apps  | will not run more Same apps
    if (XGetSelectionOwner(dsp, _NET_SYSTEM_TRAY_S) != None){
        qWarning() << "Another systray is running";
        mValid = false;
        return;
    }

    // init systray protocol
    mTrayId = XCreateSimpleWindow(dsp, root, -1, -1, 1, 1, 0, 0, 0);

    XSetSelectionOwner(dsp, _NET_SYSTEM_TRAY_S, mTrayId, CurrentTime);
    if (XGetSelectionOwner(dsp, _NET_SYSTEM_TRAY_S) != mTrayId){
        qWarning() << "Can't get systray manager";
        stopTray();
        mValid = false;
        return;
    }

    int orientation = _NET_SYSTEM_TRAY_ORIENTATION_HORZ;
    XChangeProperty(dsp,
                    mTrayId,
                    XfitMan::atom("_NET_SYSTEM_TRAY_ORIENTATION"),
                    XA_CARDINAL,
                    32,
                    PropModeReplace,
                    (unsigned char *) &orientation,
                    1);

    // ** Visual ********************************
    VisualID visualId = getVisual();
    if (visualId){
        XChangeProperty(mDisplay,
                        mTrayId,
                        XfitMan::atom("_NET_SYSTEM_TRAY_VISUAL"),
                        XA_VISUALID,
                        32,
                        PropModeReplace,
                        (unsigned char*)&visualId,
                        1);
    }
    setIconSize();

    XClientMessageEvent ev;
    ev.type = ClientMessage;
    ev.window = root;
    ev.message_type = XfitMan::atom("MANAGER");
    ev.format = 32;
    ev.data.l[0] = CurrentTime;
    ev.data.l[1] = _NET_SYSTEM_TRAY_S;
    ev.data.l[2] = mTrayId;
    ev.data.l[3] = 0;
    ev.data.l[4] = 0;
    XSendEvent(dsp, root, False, StructureNotifyMask, (XEvent*)&ev);

    XDamageQueryExtension(mDisplay, &mDamageEvent, &mDamageError);
    mValid = true;

    qApp->installNativeEventFilter(this);
}

void UKUITray::stopTray()
{
    for (auto & icon : mIcons)
        disconnect(icon, &QObject::destroyed, this, &UKUITray::onIconDestroyed);
    qDeleteAll(mIcons);
    if (mTrayId){
        XDestroyWindow(mDisplay, mTrayId);
        mTrayId = 0;
    }
    mValid = false;
}

//void UKUITray::stopStorageTray()
//{
//    for (auto & icon : mStorageIcons)
//        disconnect(icon, &QObject::destroyed, this, &UKUITray::onIconDestroyed);
//    qDeleteAll(mStorageIcons);
//    if (mTrayId){
//        XDestroyWindow(mDisplay, mTrayId);
//        mTrayId = 0;
//    }
//}

void UKUITray::onIconDestroyed(QObject * icon)
{
    //in the time QOjbect::destroyed is emitted, the child destructor
    //is already finished, so the qobject_cast to child will return nullptr in all cases
    mIcons.removeAll(static_cast<TrayIcon *>(icon));
    mStorageIcons.removeAll(static_cast<TrayIcon *>(icon));
}

void UKUITray::addTrayIcon(Window winId)
{
    // decline to add an icon for a window we already manage
    TrayIcon *icon = findTrayIcon(winId);
    if(icon)
        return;
    else{
        icon = new TrayIcon(winId, mIconSize, this);
        mIcons.append(icon);
        mTrayIcons.append(icon);
        mLayout->addWidget(icon);
        connect(icon, SIGNAL(switchButtons(TrayIcon*,TrayIcon*)), this, SLOT(switchButtons(TrayIcon*,TrayIcon*)));

        connect(icon,&QObject::destroyed,icon,&TrayIcon::notifyAppFreeze);
        connect(icon,&TrayIcon::notifyTray,[=](Window id){ freezeTrayApp(id);});
        connect(icon, &QObject::destroyed, this, &UKUITray::onIconDestroyed);
    }
}

void UKUITray::addStorageIcon(Window winId)
{
    TrayIcon *storageicon = findStorageIcon(winId);
    if(storageicon)
        return;
    else{
        storageicon = new TrayIcon(winId, mIconSize, this);
        mIcons.append(storageicon);
        mStorageIcons.append(storageicon);
        //storageLayout->addWidget(storageicon);

        connect(storageicon,&QObject::destroyed,storageicon,&TrayIcon::notifyAppFreeze);
        connect(storageicon,&TrayIcon::notifyTray,[=](Window id){freezeTrayApp(id);});
        connect(storageicon, &QObject::destroyed, this, &UKUITray::onIconDestroyed);
        if(mStorageIcons.size() > 0){
            if(!mBtn->isVisible()){
                mBtn->setVisible(true);
            }
        }
    }
}

/*与控制面板交互，移动应用到托盘栏或者收纳栏*/
void UKUITray::moveIconToTray(Window winId)
{
    TrayIcon *storageicon = findStorageIcon(winId);

    if(!storageicon)
        return;
    else{
        mStorageIcons.removeOne(storageicon);
        //storageLayout->removeWidget(storageicon);
        disconnect(storageicon, &QObject::destroyed, this, &UKUITray::onIconDestroyed);
        mTrayIcons.append(storageicon);
        mLayout->addWidget(storageicon);

        if(mStorageIcons.size() > 0){
            handleStorageUi();
        }
        else{
            mBtn->setVisible(false);
        }

        connect(storageicon,&QObject::destroyed,storageicon,&TrayIcon::notifyAppFreeze);
        connect(storageicon,&TrayIcon::notifyTray,[this](Window id){freezeTrayApp(id);});
        connect(storageicon, &QObject::destroyed, this, &UKUITray::onIconDestroyed);
    }
    trayIconSizeRefresh();
    handleStorageUi();
}

void UKUITray::moveIconToStorage(Window winId)
{
    TrayIcon *icon = findTrayIcon(winId);
    if(!icon)
        return;
    else{
        mLayout->removeWidget(icon);
        mTrayIcons.removeOne(icon);
        disconnect(icon, &QObject::destroyed, this, &UKUITray::onIconDestroyed);
        mStorageIcons.append(icon);

        if(mStorageIcons.size() > 0){
            if(!mBtn->isVisible()){
                mBtn->setVisible(true);
            }
            handleStorageUi();
        }
        else{
            mBtn->setVisible(false);
        }

        connect(icon,&QObject::destroyed,icon,&TrayIcon::notifyAppFreeze);
        connect(icon,&TrayIcon::notifyTray,[this](Window id){freezeTrayApp(id);});
        connect(icon, &QObject::destroyed, this, &UKUITray::onIconDestroyed);
    }
    handleStorageUi();
}

void UKUITray::regulateIcon(Window *mid)
{
    int wid=(int)*mid;
    int count=0;
    QList<char *> existsPath = listExistsPath();
    QString actionStr;
    int bingdingStr;
    QString nameStr;

    //匹配表中存在的name与该wid的name，若相等则用新的wid覆盖旧的wid，否则在表中添加新的路径，写上新的wid，name，以及状态。
    for (char * path : existsPath)
    {
        QString p =KEYBINDINGS_CUSTOM_DIR;
        std::string str = p.toStdString();
        const int len = str.length();
        char * prepath = new char[len+1];
        strcpy(prepath,str.c_str());
        char * allpath = strcat(prepath, path);
        const QByteArray bba(allpath);

        QGSettings *settings = NULL;
        const QByteArray id(KEYBINDINGS_CUSTOM_SCHEMA);
        if(QGSettings::isSchemaInstalled(id))
        {
            if(bba.isEmpty()){
                free(allpath);
                free(prepath);
                continue;
            }
            settings= new QGSettings(id, bba);
            if(settings){
                if(settings->keys().contains(RECORD_KEY)){
                    settings->set(ACTION_KEY,settings->get(RECORD_KEY).toString());
                }

                if(settings->keys().contains(ACTION_KEY)){
                    actionStr = settings->get(ACTION_KEY).toString();
                }

                if(settings->keys().contains(NAME_KEY)){
                    nameStr = settings->get(NAME_KEY).toString();
                }
                if(nameStr=="" || nameStr=="ErrorApplication"){
                    settings->reset(NAME_KEY);
                    settings->reset(BINDING_KEY);
                    settings->reset(ACTION_KEY);
                    settings->reset(RECORD_KEY);
                }
            }
            if(nameStr==xfitMan().getApplicationName(wid))
            {
                if(nameStr=="fcitx") {
                    fcitx_flag=true;
                }
                settings->set(BINDING_KEY, wid);
                bingdingStr=wid;
                if(QString::compare(actionStr,"tray")==0){
                    addTrayIcon(bingdingStr);
//                    TrayIcon *icon = findTrayIcon(bingdingStr);
//                    connect(icon,&TrayIcon::iconIsMoving,[this](Window id){moveIconToStorage(id);});
                }
                if(QString::compare(actionStr,"storage")==0){
                    addStorageIcon(bingdingStr);
//                    TrayIcon *icon = findStorageIcon(bingdingStr);
//                    connect(icon,&TrayIcon::iconIsMoving,[this](Window id){moveIconToTray(id);});
                }
                connect(settings, &QGSettings::changed, this, [=] (const QString &key){
                    if(key=="action"){
                        if(QString::compare(settings->get(ACTION_KEY).toString(),"tray")==0){
                            moveIconToTray(bingdingStr);
//                            TrayIcon *icon = findStorageIcon(bingdingStr);
//                            connect(icon,&TrayIcon::iconIsMoving,[this](Window id){moveIconToTray(id);});
                        }
                        else if(QString::compare(settings->get(ACTION_KEY).toString(),"storage")==0){
                            moveIconToStorage(bingdingStr);
//                            TrayIcon *icon = findTrayIcon(bingdingStr);
//                            connect(icon,&TrayIcon::iconIsMoving,[this](Window id){moveIconToStorage(id);});
                        }
                        else if(QString::compare(settings->get(ACTION_KEY).toString(),"freeze")==0){

                        }
                    }
                });
                delete allpath;
                break;
            }
        }
        settings->deleteLater();
        delete  prepath;
        count++;
        g_free(path);
    }

    if(count >= existsPath.count())
    {
        newAppDetect(wid);
        count++;
    }
}

void UKUITray::newAppDetect(int wid)
{
    QString availablepath = findFreePath();

    const QByteArray id(KEYBINDINGS_CUSTOM_SCHEMA);
    const QByteArray idd(availablepath.toUtf8().data());
    QGSettings *newsetting;
    const QByteArray keyid(KEYBINDINGS_CUSTOM_SCHEMA);
    if(QGSettings::isSchemaInstalled(keyid))
    {
        newsetting=new QGSettings(id,idd);
        newsetting->set(BINDING_KEY,wid);
        newsetting->set(NAME_KEY,xfitMan().getApplicationName(wid));

        QStringList trayIconNameList;
        trayIconNameList<<"ukui-volume-control-applet-qt"<<"kylin-nm"<<"ukui-sidebar"<<"indicator-china-weather"<<"ukui-flash-disk"<<"fcitx"<<"sogouimebs-qimpanel"<<"fcitx-qimpanel"<<"explorer.exe"<<"ukui-power-manager-tray";
        if(trayIconNameList.contains(xfitMan().getApplicationName(wid))){
            newsetting->set(ACTION_KEY,"tray");
            newsetting->set(RECORD_KEY,"tray");
            addTrayIcon(wid);
        }
        else{
            newsetting->set(ACTION_KEY,"storage");
            newsetting->set(RECORD_KEY,"storage");
            addStorageIcon(wid);
        }
    }
    delete newsetting;

    QGSettings* settings;
    const QByteArray keyId(KEYBINDINGS_CUSTOM_SCHEMA);
    if(QGSettings::isSchemaInstalled(keyId)){
        settings= new QGSettings(id, idd,this);
        connect(settings, &QGSettings::changed, this, [=] (const QString &key){
            if(key=="action"){
                if(QString::compare(settings->get(ACTION_KEY).toString(),"tray")==0){
                    moveIconToTray(wid);
                }
                else if(QString::compare(settings->get(ACTION_KEY).toString(),"storage")==0){
                    moveIconToStorage(wid);
                }
                else{
                    qDebug()<<"get error action";
                    qWarning()<<"get error action";
                }
            }
        });
    }
}

void UKUITray::handleStorageUi()
{

    if(m_pwidget)
    {
        if(mStorageLayout->count()){
            mStorageLayout->removeWidget(m_pwidget);
        }
        UKUi::GridLayout *vLayout = dynamic_cast<UKUi::GridLayout*>(m_pwidget->layout());
        if(vLayout)
        {
            vLayout->deleteLater();
        }
        m_pwidget->deleteLater();
        m_pwidget = NULL;
    }
    m_pwidget =  new UKUiStorageWidget;
    m_pwidget->setStorageWidgetButtonLayout(mStorageIcons.size());

    for(auto it = mStorageIcons.begin();it != mStorageIcons.end();++it)
    {
        m_pwidget->layout()->addWidget(*it);
        (*it)->setFixedSize(mWinWidth,mWinHeight);
    }
    storageFrame->layout()->addWidget(m_pwidget);
    storageFrame->setStorageFrameSize(mStorageIcons.size());
}

void UKUITray::contextMenuEvent(QContextMenuEvent *event)
{

}
void UKUITray::switchButtons(TrayIcon *button1, TrayIcon *button2)
{
    if (button1 == button2)
        return;
    int n1 = mLayout->indexOf(button1);
    int n2 = mLayout->indexOf(button2);
    int l = qMin(n1, n2);
    int m = qMax(n1, n2);
    mLayout->moveItem(l, m);
    mLayout->moveItem(m-1, l);
}

//特殊处理
//任务栏异常退出可能导致输入法异常，此时任务栏再次起来不会重新拉起输入法,此处特殊处理
void UKUITray::panelStartupFcitx()
{
    if(!fcitx_flag){
        qDebug()<<"fcitx未启动";
        QProcess *process =new QProcess(this);
        process->startDetached("fcitx");
        process->deleteLater();
    }
}
