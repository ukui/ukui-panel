/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Copyright: 2011 Razor team
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
#undef Bool // defined as int in X11/Xlib.h

#include "../panel/iukuipanelplugin.h"
#include "traystorage.h"
//#include "../panel/customstyle.h"

#include <QPushButton>
#include <QToolButton>
#include <QLabel>

#define _NET_SYSTEM_TRAY_ORIENTATION_HORZ 0
#define _NET_SYSTEM_TRAY_ORIENTATION_VERT 1

#define SYSTEM_TRAY_REQUEST_DOCK    0
#define SYSTEM_TRAY_BEGIN_MESSAGE   1
#define SYSTEM_TRAY_CANCEL_MESSAGE  2

#define XEMBED_EMBEDDED_NOTIFY  0
#define XEMBED_MAPPED          (1 << 0)

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

#define KEYBINDINGS_CUSTOM_SCHEMA "org.ukui.control-center.keybinding"
#define KEYBINDINGS_CUSTOM_DIR "/org/ukui/desktop/keybindings/"

#define MAX_CUSTOM_SHORTCUTS 30

#define ACTION_KEY "action"
#define RECORD_KEY "record"
#define BINDING_KEY "binding"
#define NAME_KEY "name"

/************************************************

 ************************************************/

extern TrayStorageStatus storagestatus;

UKUITray::UKUITray(IUKUIPanelPlugin *plugin, QWidget *parent):
    QFrame(parent),
    mValid(false),
    mTrayId(0),
    mDamageEvent(0),
    mDamageError(0),
    mIconSize(TRAY_ICON_SIZE_DEFAULT, TRAY_ICON_SIZE_DEFAULT),
    mPlugin(plugin),
    mDisplay(QX11Info::display())
{
    mLayout = new UKUi::GridLayout(this);
    realign();
    _NET_SYSTEM_TRAY_OPCODE = XfitMan::atom("_NET_SYSTEM_TRAY_OPCODE");
    // Init the selection later just to ensure that no signals are sent until
    // after construction is done and the creating object has a chance to connect.
    QTimer::singleShot(0, this, SLOT(startTray()));
    QToolButton *bt=new QToolButton;
    bt->setStyle(new CustomStyle());
    bt->setIcon(QIcon("/usr/share/ukui-panel/panel/img/up.svg"));
    mLayout->addWidget(bt);

    tys= new TrayStorage();
    connect(bt,SIGNAL(clicked()),this,SLOT(storageBar()));
}


/************************************************
 ************************************************/
UKUITray::~UKUITray()
{
    freezeApp();
    stopTray();
}
void UKUITray::storageBar()
{
    int availableHeight = QGuiApplication::screens().at(0)->availableGeometry().height();
    int avaliableWidth=QGuiApplication::screens().at(0)->availableVirtualGeometry().width();

    switch (mPlugin->panel()->position()){
    case 0:
        tys->setGeometry(avaliableWidth-300,availableHeight-90,150,90);
        break;
    case 1:
        tys->setGeometry(avaliableWidth-300,0,130,90);
        break;
    case 2:
        tys->setGeometry(0,availableHeight-390,130,90);
        break;
    case 3:
        tys->setGeometry(avaliableWidth-130,availableHeight-290,130,90);
        break;
    default:
        break;
    }

    switch (storagestatus) {
    case HIDE:
        tys->show();
        storagestatus=HOVER;
        break;

    case HOVER:
        tys->hide();
        storagestatus=HIDE;
        break;
    default:
        break;
    }
}

/************************************************

 ************************************************/
bool UKUITray::nativeEventFilter(const QByteArray &eventType, void *message, long *)
{
    if (eventType != "xcb_generic_event_t")
        return false;

    xcb_generic_event_t* event = static_cast<xcb_generic_event_t *>(message);

    TrayIcon* icon;
    int event_type = event->response_type & ~0x80;

    switch (event_type)
    {
        case ClientMessage:
            clientMessageEvent(event);
            break;

//        case ConfigureNotify:
//            icon = findIcon(event->xconfigure.window);
//            if (icon)
//                icon->configureEvent(&(event->xconfigure));
//            break;

        case DestroyNotify: {
            unsigned long event_window;
            event_window = reinterpret_cast<xcb_destroy_notify_event_t*>(event)->window;
            icon = findIcon(event_window);
            if (icon)
            {
                icon->windowDestroyed(event_window);
                mIcons.removeAll(icon);
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


/************************************************

 ************************************************/
void UKUITray::realign()
{
    mLayout->setEnabled(false);
    IUKUIPanel *panel = mPlugin->panel();

    if (panel->isHorizontal())
    {
        mLayout->setRowCount(panel->lineCount());
        mLayout->setColumnCount(0);
    }
    else
    {
        mLayout->setColumnCount(panel->lineCount());
        mLayout->setRowCount(0);
    }
    mLayout->setEnabled(true);
}


/************************************************

 ************************************************/
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
            }
            /*
            //old  way to load trayApp  ,don't delete these code Until the new version is stable.
            if(id)
            {
                if(xfitMan().getApplicationName(id)=="kylin-nm")
                {
                    addIcon(id);
                }
                else
                {
                    storageAddIcon(id);
                }
            }
            */


        case SYSTEM_TRAY_BEGIN_MESSAGE:
        case SYSTEM_TRAY_CANCEL_MESSAGE:
            qDebug() << "we don't show balloon messages.";
            break;


        default:
//            if (opcode == xfitMan().atom("_NET_SYSTEM_TRAY_MESSAGE_DATA"))
//                qDebug() << "message from dockapp:" << e->data.b;
//            else
//                qDebug() << "SYSTEM_TRAY : unknown message type" << opcode;
            break;
    }
}

/************************************************

 ************************************************/
TrayIcon* UKUITray::findIcon(Window id)
{
    for(TrayIcon* icon : qAsConst(mIcons))
    {
        if (icon->iconId() == id || icon->windowId() == id)
            return icon;
    }
    return 0;
}

TrayIcon* UKUITray::findStorageIcon(Window id)
{
    for(TrayIcon* storageicon :qAsConst(mStorageIcons))
    {
        if (storageicon->iconId() == id || storageicon->windowId() == id)
            return storageicon;
    }
    return 0;
}

/************************************************

************************************************/
void UKUITray::setIconSize(QSize iconSize)
{
    mIconSize = iconSize;
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


/************************************************

************************************************/
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
        for (i = 0; i < nvi; i++)
        {
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


/************************************************
   freedesktop systray specification
 ************************************************/
void UKUITray::startTray()
{
    Display* dsp = mDisplay;
    Window root = QX11Info::appRootWindow();

    QString s = QString("_NET_SYSTEM_TRAY_S%1").arg(DefaultScreen(dsp));
    Atom _NET_SYSTEM_TRAY_S = XfitMan::atom(s.toLatin1());
    //this limit the tray apps  | will not run more Same apps
//    if (XGetSelectionOwner(dsp, _NET_SYSTEM_TRAY_S) != None)
//    {
//        qWarning() << "Another systray is running";
//        mValid = false;
//        return;
//    }

    // init systray protocol
    mTrayId = XCreateSimpleWindow(dsp, root, -1, -1, 1, 1, 0, 0, 0);

    XSetSelectionOwner(dsp, _NET_SYSTEM_TRAY_S, mTrayId, CurrentTime);
    if (XGetSelectionOwner(dsp, _NET_SYSTEM_TRAY_S) != mTrayId)
    {
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
    if (visualId)
    {
        XChangeProperty(mDisplay,
                        mTrayId,
                        XfitMan::atom("_NET_SYSTEM_TRAY_VISUAL"),
                        XA_VISUALID,
                        32,
                        PropModeReplace,
                        (unsigned char*)&visualId,
                        1);
    }
    // ******************************************

    setIconSize(mIconSize);

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


/************************************************

 ************************************************/
void UKUITray::stopTray()
{
    for (auto & icon : mIcons)
        disconnect(icon, &QObject::destroyed, this, &UKUITray::onIconDestroyed);
    qDeleteAll(mIcons);
    if (mTrayId)
    {
        XDestroyWindow(mDisplay, mTrayId);
        mTrayId = 0;
    }
    mValid = false;
}

void UKUITray::stopStorageTray()
{
    for (auto & icon : mStorageIcons)
        disconnect(icon, &QObject::destroyed, this, &UKUITray::onIconDestroyed);
    qDeleteAll(mStorageIcons);
    if (mTrayId)
    {
        XDestroyWindow(mDisplay, mTrayId);
        mTrayId = 0;
    }
}
/************************************************

 ************************************************/
void UKUITray::onIconDestroyed(QObject * icon)
{
    //in the time QOjbect::destroyed is emitted, the child destructor
    //is already finished, so the qobject_cast to child will return nullptr in all cases
//    qDebug()<<"tray icon destory";
    mIcons.removeAll(static_cast<TrayIcon *>(icon));
    mStorageIcons.removeAll(static_cast<TrayIcon *>(icon));
}

void UKUITray::freezeIconSlot(TrayIcon *icon,Window winid)
{
    connect(icon,&QFrame::destroyed,[=](){
        qDebug() << "destory wind" << winid;
    });
}

void UKUITray::freezeTrayApp(Window winId)
{
    qDebug()<<"freezeTrayApp     id:" <<winId<<"name is:" <<xfitMan().getApplicationName(winId);
    int wid=(int)winId;
    QList<char *> existsPath = listExistsPath();
    QString actionStr;
    int bingdingStr;
    QString nameStr;
    QString recordStr;

    nameStr = xfitMan().getApplicationName(wid);
    for (char * path : existsPath){
        QString p ="/org/ukui/desktop/keybindings/";
        std::string str = p.toStdString();
        const int len = str.length();
        char * prepath = new char[len+1];
        strcpy(prepath,str.c_str());
        char * allpath = strcat(prepath, path);

        const QByteArray ba(KEYBINDINGS_CUSTOM_SCHEMA);
        const QByteArray bba(allpath);

        QGSettings *settings;
        const QByteArray id(KEYBINDINGS_CUSTOM_SCHEMA);
        if(QGSettings::isSchemaInstalled(id)) {
        settings= new QGSettings(ba, bba,this);

        actionStr = settings->get(ACTION_KEY).toString();
        bingdingStr = settings->get(BINDING_KEY).toInt();

        recordStr=settings->get(RECORD_KEY).toString();
        settings->set(ACTION_KEY,recordStr);
        nameStr = settings->get(NAME_KEY).toString();


        settings->setObjectName(nameStr);
        if(winId==bingdingStr)
        {
          qDebug()<<"freezze the app     ********** "<<xfitMan().getApplicationName(wid)<<"and change it winid ;   path is :"<<bba;
          settings->set(ACTION_KEY,"freeze");
        }
        }
        delete settings;
    }

}
/************************************************

 ************************************************/
void UKUITray::addIcon(Window winId)
{
    // decline to add an icon for a window we already manage
    TrayIcon *icon = findIcon(winId);
    if(icon)
        return;
    else
    icon = new TrayIcon(winId, mIconSize, this);
    mIcons.append(icon);
    mLayout->addWidget(icon);
//    connect(icon,&QObject::destroyed,icon,&TrayIcon::notifyAppFreeze);
//    connect(icon,&TrayIcon::notifyTray,this,&UKUITray::freezeTrayApp);
    connect(icon, &QObject::destroyed, this, &UKUITray::onIconDestroyed);

}

void UKUITray::moveIcon(Window winId)
{
    TrayIcon *icon = findIcon(winId);
    if(icon)
    {
        mLayout->removeWidget(icon);
        mIcons.removeOne(icon);
        disconnect(icon, &QObject::destroyed, this, &UKUITray::onIconDestroyed);
        mStorageIcons.append(icon);
        tys->mLayout->addWidget(icon);
        qDebug() << "从托盘移动到收纳";
    }
//    connect(icon,&QObject::destroyed,icon,&TrayIcon::notifyAppFreeze);
//    connect(icon,&TrayIcon::notifyTray,this,&UKUITray::freezeTrayApp);
    connect(icon, &QObject::destroyed, this, &UKUITray::onIconDestroyed);
}

void UKUITray::storageAddIcon(Window winId)
{
    TrayIcon *storageicon = findStorageIcon(winId);
    if(storageicon)
        return;
    else
    storageicon = new TrayIcon(winId, mIconSize, this);
    mStorageIcons.append(storageicon);
    tys->mLayout->addWidget(storageicon);
//    connect(storageicon,&QObject::destroyed,storageicon,&TrayIcon::notifyAppFreeze);
//    connect(storageicon,&TrayIcon::notifyTray,this,&UKUITray::freezeTrayApp);
    connect(storageicon, &QObject::destroyed, this, &UKUITray::onIconDestroyed);
}

void UKUITray::storageMoveIcon(Window winId)
{
    TrayIcon *storageicon = findStorageIcon(winId);
    if(storageicon)
    {
        mStorageIcons.removeOne(storageicon);
        tys->mLayout->removeWidget(storageicon);
        disconnect(storageicon, &QObject::destroyed, this, &UKUITray::onIconDestroyed);
        mIcons.append(storageicon);
        mLayout->addWidget(storageicon);
        qDebug() << "从收纳栏移除到托盘";
    }
//    connect(storageicon,&QObject::destroyed,storageicon,&TrayIcon::notifyAppFreeze);
//    connect(storageicon,&TrayIcon::notifyTray,this,&UKUITray::freezeTrayApp);
    connect(storageicon, &QObject::destroyed, this, &UKUITray::onIconDestroyed);
}

QList<char *> UKUITray::listExistsPath(){
    char ** childs;
    int len;

    DConfClient * client = dconf_client_new();
    childs = dconf_client_list (client, KEYBINDINGS_CUSTOM_DIR, &len);
    g_object_unref (client);

    QList<char *> vals;

    for (int i = 0; childs[i] != NULL; i++){
        if (dconf_is_rel_dir (childs[i], NULL)){
            char * val = g_strdup (childs[i]);

            vals.append(val);
        }
    }
    g_strfreev (childs);
    return vals;
}


void UKUITray::regulateIcon(Window *mid)
{
    int wid=(int)*mid;
    int count=0;
    QList<char *> existsPath = listExistsPath();
    QString actionStr;
    int bingdingStr;
    QString nameStr;
    QString recordStr;

//    char * prepath = QString(p).toLatin1().data();
//    char * prepath = QString(KEYBINDINGS_CUSTOM_DIR).toLatin1().data();

    nameStr = xfitMan().getApplicationName(wid);
    //匹配表中存在的name与该wid的name，若相等则用新的wid覆盖旧的wid，否则在表中添加新的路径，写上新的wid，name，以及状态。
    for (char * path : existsPath){
        QString p ="/org/ukui/desktop/keybindings/";
        std::string str = p.toStdString();
        const int len = str.length();
        char * prepath = new char[len+1];
        strcpy(prepath,str.c_str());
        char * allpath = strcat(prepath, path);

        const QByteArray ba(KEYBINDINGS_CUSTOM_SCHEMA);
        const QByteArray bba(allpath);

//        connectGsetting(ba,bba,wid);
//        qDebug()<<"all path :"<<allpath << "prepath:" << prepath << "path:" << path <<"appname:" <<xfitMan().getApplicationName(wid) << "存在path大小" << existsPath.count();
        QGSettings *settings;
        const QByteArray id(KEYBINDINGS_CUSTOM_SCHEMA);
        if(QGSettings::isSchemaInstalled(id)) {
        settings= new QGSettings(ba, bba,this);

        actionStr = settings->get(ACTION_KEY).toString();
        bingdingStr = settings->get(BINDING_KEY).toInt();

        recordStr=settings->get(RECORD_KEY).toString();
        settings->set(ACTION_KEY,recordStr);
        nameStr = settings->get(NAME_KEY).toString();


        settings->setObjectName(nameStr);
        if(nameStr==xfitMan().getApplicationName(wid))
        {
            qDebug()<<"find the app "<<xfitMan().getApplicationName(wid)<<"and change it winid ;   path is :"<<bba;
            settings->set(BINDING_KEY, wid);
//            actionStr=recordStr;
            bingdingStr=wid;

            if(QString::compare(actionStr,"tray")){
                qDebug()<<"tray add Icon ";
                addIcon(bingdingStr);
            }
            else if(QString::compare(actionStr,"storage")){
                qDebug()<<"storege add Icon ";
                storageAddIcon(bingdingStr);
            }
            connect(settings, &QGSettings::changed, this, [=] (const QString &key) {
                qDebug()<<"status changed ------------>" <<"name:" <<nameStr <<endl;
                if(key=="action")
                {
                    if(QString::compare(settings->get(ACTION_KEY).toString(),"tray"))
                    {
//                        qDebug()<<"***** action change to false; storage ->tray ******** window id is:"<<bingdingStr<<"  name is "<<xfitMan().getApplicationName(bingdingStr);
                        moveIcon(bingdingStr);
                    }
                    else if(QString::compare(settings->get(ACTION_KEY).toString(),"storage")){
//                        qDebug()<<"****action change to true; tray ->storage  ********  window id is:"<<bingdingStr<<"  name is "<<xfitMan() .getApplicationName(bingdingStr);
                        storageMoveIcon(bingdingStr);
                    }
                }
            });
            break;
        }
        }
//        delete settings;
                count++;
//        g_warning("full path: %s", allpath);
//        qDebug() << ACTION_KEY << actionStr << BINDING_KEY << bingdingStr <<NAME_KEY << nameStr;
    }

    if(count>=existsPath.count())
    {
        QString availablepath = findFreePath();

        qDebug() << "new app will be add" << availablepath;
//        qDebug()<<"xfitMan().getApplicationName(wid):"<<xfitMan().getApplicationName(wid);

        const QByteArray id(KEYBINDINGS_CUSTOM_SCHEMA);
        const QByteArray idd(availablepath.toUtf8().data());
        QGSettings *newsetting;
        const QByteArray keyid(KEYBINDINGS_CUSTOM_SCHEMA);
        if(QGSettings::isSchemaInstalled(keyid)) {
        newsetting=new QGSettings(id,idd);
        newsetting->set(BINDING_KEY,wid);
        newsetting->set(NAME_KEY,xfitMan().getApplicationName(wid));
        newsetting->set(ACTION_KEY,"tray");
        newsetting->set(RECORD_KEY,"tray");
        }
        delete newsetting;
        addIcon(wid);

        QGSettings* settings;
        const QByteArray keyId(KEYBINDINGS_CUSTOM_SCHEMA);
        if(QGSettings::isSchemaInstalled(keyId)) {
        settings= new QGSettings(id, idd,this);
        connect(settings, &QGSettings::changed, this, [=] (const QString &key) {
            qDebug()<<"status changed ------------>" <<"name:" <<nameStr <<endl;
            if(key=="action")
            {
                if(settings->get(ACTION_KEY).toString()=="tray")
                {
//                        qDebug()<<"***** action change to false; storage ->tray ******** window id is:"<<bingdingStr<<"  name is "<<xfitMan().getApplicationName(bingdingStr);
                    moveIcon(bingdingStr);
                }
                else if(settings->get(ACTION_KEY).toString()=="storage"){
//                        qDebug()<<"****action change to true; tray ->storage  ********  window id is:"<<bingdingStr<<"  name is "<<xfitMan() .getApplicationName(bingdingStr);
                    storageMoveIcon(bingdingStr);
                }
            }
        });
        }
            count++;
    }


}

void UKUITray::freezeApp()
{
    QList<char *> existsPath = listExistsPath();
    for (char * path : existsPath){
        QString p ="/org/ukui/desktop/keybindings/";
        std::string str = p.toStdString();
        const int len = str.length();
        char * prepath = new char[len+1];
        strcpy(prepath,str.c_str());
        char * allpath = strcat(prepath, path);

        const QByteArray ba(KEYBINDINGS_CUSTOM_SCHEMA);
        const QByteArray bba(allpath);

//        connectGsetting(ba,bba,wid);
//        qDebug()<<"all path :"<<allpath << "prepath:" << prepath << "path:" << path <<"appname:" <<xfitMan().getApplicationName(wid) << "存在path大小" << existsPath.count();
        QGSettings *settings;
        const QByteArray id(KEYBINDINGS_CUSTOM_SCHEMA);
        if(QGSettings::isSchemaInstalled(id)) {
        settings= new QGSettings(ba, bba,this);

        settings->set(ACTION_KEY,"freeze");
        }
    }
}


QString UKUITray::findFreePath(){
    int i = 0;
    char * dir;
    bool found;
    QList<char *> existsdirs;

    existsdirs = listExistsPath();

    for (; i < MAX_CUSTOM_SHORTCUTS; i++){
        found = true;
        dir = QString("custom%1/").arg(i).toLatin1().data();
        for (int j = 0; j < existsdirs.count(); j++)
            if (!g_strcmp0(dir, existsdirs.at(j))){
                found = false;
                break;
            }
        if (found)
            break;
    }

    if (i == MAX_CUSTOM_SHORTCUTS){
        qDebug() << "Keyboard Shortcuts" << "Too many custom shortcuts";
        return "";
    }
    return QString("%1%2").arg(KEYBINDINGS_CUSTOM_DIR).arg(QString(dir));
}
