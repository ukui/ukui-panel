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
#include "ukuitrayplugin.h"
#undef Bool // defined as int in X11/Xlib.h

#include "../panel/iukuipanelplugin.h"
#include "traystorage.h"

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

#define KEYBINDINGS_CUSTOM_SCHEMA "org.ukui.panel.tray"
#define KEYBINDINGS_CUSTOM_DIR "/org/ukui/tray/keybindings/"
#define MAX_CUSTOM_SHORTCUTS 30
#define ACTION_KEY "action"
#define RECORD_KEY "record"
#define BINDING_KEY "binding"
#define NAME_KEY "name"

#define TRAY_APP_COUNT 16

/************************************************

 ************************************************/

/*
 * @Note 托盘应用共分为三个区域 Tray Storage Hide
 * Tray区域 ：UKUITray ； Storage区域：UKUIStorageFrame ； Hide区域没有界面
 * 图标可存放在三个区域中的任何一个区域，如果应用退出则被销毁
 *
 * @bug:
 * 1.图标移动过程中可能存在重影（一个图标的轮廓显示在另一个图标上方）
 * 2.图标模糊现象，目前存在的情况是安装系统起来的时候可能会有托盘图标模糊
 *   注销后模糊现象消失（已经修改改变图标大小的方式）
 *
 *
*/
bool flag=false;
//extern TrayStorageStatus storagestatus;
storageBarStatus status;

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
    mMapIcon.clear();
    m_pwidget = NULL;
    status=ST_HIDE;
    setLayout(new UKUi::GridLayout(this));
    _NET_SYSTEM_TRAY_OPCODE = XfitMan::atom("_NET_SYSTEM_TRAY_OPCODE");
    // Init the selection later just to ensure that no signals are sent until
    // after construction is done and the creating object has a chance to connect.
    QTimer::singleShot(0, this, SLOT(startTray()));
    mBtn =new QToolButton;
    mBtn->setStyle(new CustomStyle());
    mBtn->setIcon(QIcon("/usr/share/ukui-panel/panel/img/up.svg"));
    mBtn->setVisible(false);
    layout()->addWidget(mBtn);
    if(mPlugin)
    {
        mCurPosition = mPlugin->panel()->position();
    }

    storageFrame=new UKUIStorageFrame;
    storageFrame->setLayout(new UKUi::GridLayout);
    connect(mBtn,SIGNAL(clicked()),this,SLOT(storageBar()));
    connect(this,SIGNAL(positionChanged()),this,SLOT(changeIcon()));
    createIconMap();
    realign();
    changeIcon();
}


/************************************************
 ************************************************/
UKUITray::~UKUITray()
{
    for(int i  = 0; i < mTrayIcons.size(); i++)
    {
        if(mTrayIcons[i])
        {
            mTrayIcons[i]->deleteLater();
            mTrayIcons[i] = NULL;
        }
    }
    for(int i  = 0; i < mStorageIcons.size(); i++)
    {
        if(mStorageIcons[i])
        {
            mStorageIcons[i]->deleteLater();
            mStorageIcons[i] = NULL;
        }
    }
    for(int i  = 0; i < mHideIcons.size(); i++)
    {
        if(mHideIcons[i])
        {
            mHideIcons[i]->deleteLater();
            mHideIcons[i] = NULL;
        }
    }
    for(int i  = 0; i < mIcons.size(); i++)
    {
        if(mIcons[i])
        {
            mIcons[i]->deleteLater();
            mIcons[i] = NULL;
        }
    }
    mIcons.clear();
    mStorageIcons.clear();
    mHideIcons.clear();
    mTrayIcons.clear();
    mMapIcon.clear();
    freezeApp();
    stopTray();
}
void UKUITray::storageBar()
{
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
}

void UKUITray::showAndHideStorage(bool storageStatus)
{
    qDebug()<<"showAndHideStorage"<<storageStatus;
    if(storageStatus)
    {
        storageFrame->hide();
    }
    else
    {
        storageFrame->show();
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
    /* 监听到托盘应用的启动事件
     * tr:Listen to the launch event of the tray application
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
         * tr:Listen to the exit event of the tray application
         */
    case DestroyNotify: {
        /*
         * 不清楚托盘栏区域UKUITray 是如何检测到应用崩溃之后刷新布局的
         * 在这里本应该刷新收纳栏 handleStorageUi()
         * 由于收纳栏不刷新可能会导致任务栏的崩溃
         * 在freeze信号接受的地方freezeTrayApp（）同样进行刷新收纳栏的操作
         * 但是如果在这里刷新收纳栏目会导致收纳栏的应用异常退出
         *
         * tr:
         * If there is a tray application exit, refresh the storage bar in time,
         * The reason for entering this step is because it is not clear
         * how the tray bar area UKUITray detected the app crash and refreshed the layout
         * The panel may crash because the storage bar is not refreshed
         * So refresh the storage bar in freezeTrayApp()
         */

        unsigned long event_window;
        event_window = reinterpret_cast<xcb_destroy_notify_event_t*>(event)->window;
        icon = findIcon(event_window);
        if (icon)
        {
            icon->windowDestroyed(event_window);
            mHideIcons.removeAll(icon);
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


/*
 * @brief Adjust the size of the tray in real time
 * @Note
 * 关于设置托盘栏图标大小的方法
 * ukui采用与lxqt-panel不同的方式
 * 直接在realign函数中进行设置每个托盘应用所在的位置的大小和位置
 *
 * tr:
 * About the method of setting the icon size of the tray bar
 * ukui takes a different approach from lxqt-panel
 * Set the size and position of each tray application location directly
 * in the realign function
 */
void UKUITray::realign()
{
    layout()->setEnabled(false);
    IUKUIPanel *panel = mPlugin->panel();



    /*　刷新托盘收纳栏界面
     * 不要在这里对收纳栏的图标执setFixedSize和setIconSize的操作
     * 这些应该在handleStorageUi()函数中执行
     * handleStorageUi();
    */
    //    for(int i=0;i<mStorageIcons.size();i++)
    //    {
    //        if(mStorageIcons.at(i))
    //        {
    //            mStorageIcons.at(i)->setFixedSize(mPlugin->panel()->iconSize(),mPlugin->panel()->panelSize());
    //            mStorageIcons.at(i)->setIconSize(QSize(mPlugin->panel()->iconSize()/2,mPlugin->panel()->iconSize()/2));
    //        }
    //        else
    //        {
    //            qDebug()<<"mTrayIcons add error   :  "<<mTrayIcons.at(i);
    //        }
    //    }
    if (panel->isHorizontal())
    {
        if(mTrayIcons.size()<TRAY_APP_COUNT)
        {
            dynamic_cast<UKUi::GridLayout*>(layout())->setRowCount(panel->lineCount());
            dynamic_cast<UKUi::GridLayout*>(layout())->setColumnCount(0);
            for(int i=0;i<mTrayIcons.size();i++)
            {
                if(mTrayIcons.at(i))
                {
                    mTrayIcons.at(i)->setFixedSize(mPlugin->panel()->iconSize(),mPlugin->panel()->panelSize());
                    mTrayIcons.at(i)->setIconSize(QSize(mPlugin->panel()->iconSize()/2,mPlugin->panel()->iconSize()/2));
                }
                else
                {
                    qDebug()<<"mTrayIcons add error   :  "<<mTrayIcons.at(i);
                }
            }
            mBtn->setFixedSize(mPlugin->panel()->iconSize(),mPlugin->panel()->panelSize());
        }
        else
        {
            dynamic_cast<UKUi::GridLayout*>(layout())->setRowCount(2);
            dynamic_cast<UKUi::GridLayout*>(layout())->setColumnCount(0);
            for(int i=0;i<mTrayIcons.size();i++)
            {
                if(mTrayIcons.at(i))
                {
                    mTrayIcons.at(i)->setFixedSize(mPlugin->panel()->iconSize()/2,mPlugin->panel()->panelSize()/2);
                    mTrayIcons.at(i)->setIconSize(QSize(mPlugin->panel()->iconSize()/2,mPlugin->panel()->iconSize()/2));
                }
                else
                {
                    qDebug()<<"mTrayIcons add error   :  "<<mTrayIcons.at(i);
                }
            }
        }


    }
    else
    {
        if(mTrayIcons.size()<TRAY_APP_COUNT/2)
        {
            dynamic_cast<UKUi::GridLayout*>(layout())->setColumnCount(panel->lineCount());
            dynamic_cast<UKUi::GridLayout*>(layout())->setRowCount(0);

            for(int i=0;i<mTrayIcons.size();i++)
            {
                if(mTrayIcons.at(i))
                {
                    mTrayIcons.at(i)->setFixedSize(mPlugin->panel()->panelSize(),mPlugin->panel()->iconSize());
                    mTrayIcons.at(i)->setIconSize(QSize(mPlugin->panel()->iconSize()/2,mPlugin->panel()->iconSize()/2));
                }
                else
                {
                    qDebug()<<"mTrayIcons add error   :  "<<mTrayIcons.at(i);
                }
            }
        }
        else
        {
            dynamic_cast<UKUi::GridLayout*>(layout())->setColumnCount(2);
            dynamic_cast<UKUi::GridLayout*>(layout())->setRowCount(0);
            for(int i=0;i<mTrayIcons.size();i++)
            {
                if(mTrayIcons.at(i))
                {
                    mTrayIcons.at(i)->setFixedSize(mPlugin->panel()->panelSize()/2,mPlugin->panel()->iconSize()/2);
                    mTrayIcons.at(i)->setIconSize(QSize(mPlugin->panel()->iconSize()/2,mPlugin->panel()->iconSize()/2));
                }
                else
                {
                    qDebug()<<"mTrayIcons add error   :  "<<mTrayIcons.at(i);
                }
            }
        }
        mBtn->setFixedSize(mPlugin->panel()->panelSize(),mPlugin->panel()->iconSize());

    }

    if(storageFrame)
    {
        storageFrame->setGeometry(mPlugin->panel()->calculatePopupWindowPos(mapToGlobal(QPoint(-30,0)), storageFrame->size()));
    }

    if(mCurPosition != panel->position())
    {
        mCurPosition = panel->position();
        emit positionChanged();
    }
    layout()->setEnabled(true);

}

void UKUITray::createIconMap()
{
    mMapIcon[IUKUIPanel::PositionBottom] = QIcon("/usr/share/ukui-panel/panel/img/tray-up.svg");
    mMapIcon[IUKUIPanel::PositionLeft] = QIcon("/usr/share/ukui-panel/panel/img/tray-right.svg");
    mMapIcon[IUKUIPanel::PositionTop] = QIcon("/usr/share/ukui-panel/panel/img/tray-down.svg");
    mMapIcon[IUKUIPanel::PositionRight] = QIcon("/usr/share/ukui-panel/panel/img/tray-left.svg");
}

void UKUITray::changeIcon()
{
    qDebug()<<"change icon now";
    QIcon icon;
    QMap<IUKUIPanel::Position, QIcon>::const_iterator it = mMapIcon.find(mCurPosition);
    if(it != mMapIcon.end())
    {
        icon = it.value();
    }
    mBtn->setIcon(icon);
    mBtn->setFixedSize(QSize(30,mPlugin->panel()->panelSize()));
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
            //            Window winId=44040845;
            //            QSize iconSize(32,32);
            //            TrayIcon *icon = new TrayIcon(winId,iconSize,this);
            //            //            icon->setFixedSize(40,40);
            //            icon->setIconSize(QSize(32,32));
        }


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
    for(TrayIcon* icon : qAsConst(mIcons) )
    {
        if (icon->iconId() == id || icon->windowId() == id)
            return icon;
    }
    return 0;
}

TrayIcon* UKUITray::findTrayIcon(Window id)
{
    for(TrayIcon* trayicon : qAsConst(mTrayIcons) )
    {
        if (trayicon->iconId() == id || trayicon->windowId() == id)
            return trayicon;
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

TrayIcon* UKUITray::findHideIcon(Window id)
{
    for(TrayIcon* hideicon :qAsConst(mHideIcons))
    {
        if (hideicon->iconId() == id || hideicon->windowId() == id)
            return hideicon;
    }
    return 0;
}
/************************************************

************************************************/
void UKUITray::setIconSize()
{
    int iconSize=16;
    mIconSize=QSize(iconSize,iconSize);
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
    if (XGetSelectionOwner(dsp, _NET_SYSTEM_TRAY_S) != None)
    {
        qWarning() << "Another systray is running";
        mValid = false;
        return;
    }

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
    mIcons.removeAll(static_cast<TrayIcon *>(icon));
    mStorageIcons.removeAll(static_cast<TrayIcon *>(icon));
    if(0 == mStorageIcons.size())
    {
        mBtn->setVisible(false);
    }
}


void UKUITray::freezeTrayApp(Window winId)
{
    QList<char *> existsPath = listExistsPath();
    int bingdingStr;

    for (char * path : existsPath)
    {
        QString p =KEYBINDINGS_CUSTOM_DIR;
        std::string str = p.toStdString();
        const int len = str.length();
        char * prepath = new char[len+1];
        strcpy(prepath,str.c_str());
        char * allpath = strcat(prepath, path);

        const QByteArray ba(KEYBINDINGS_CUSTOM_SCHEMA);
        const QByteArray bba(allpath);

        QGSettings *settings = NULL;
        const QByteArray id(KEYBINDINGS_CUSTOM_SCHEMA);
        if(QGSettings::isSchemaInstalled(id))
        {
            if(bba.isEmpty())
            {
                continue;
            }
            settings= new QGSettings(ba, bba,this);
            if(settings)
            {
                if(settings->keys().contains(BINDING_KEY))
                {
                    bingdingStr=settings->get(BINDING_KEY).toInt();
                }
            }

            if(winId==bingdingStr)
            {
                settings->set(ACTION_KEY,"freeze");
            }
        }

        if(settings)
        {
            settings->deleteLater();
        }
    }
    /*
     * 在任何一个托盘应用异常退出的时候都需要刷新收纳栏的界面
     * 这可能会导致panel的崩溃
     *
     * 将handleStorageUi　放到freezeTrayApp　函数最后以避免崩溃问题
    */
    TrayIcon *storageicon = findStorageIcon(winId);
    if(storageicon)
    {
        mStorageIcons.removeOne(storageicon);
        storageicon->deleteLater();
        storageicon=NULL;
    }
    TrayIcon *trayicon = findTrayIcon(winId);
    if(trayicon)
    {
        mTrayIcons.removeOne(trayicon);
        trayicon->deleteLater();
        trayicon=NULL;
    }
    handleStorageUi();
}


/*
*/
void UKUITray::addTrayIcon(Window winId)
{
    // decline to add an icon for a window we already manage
    TrayIcon *icon = findTrayIcon(winId);
    if(icon)
        return;
    else
    {
        icon = new TrayIcon(winId, mIconSize, this);
        mIcons.append(icon);
        mTrayIcons.append(icon);
        layout()->addWidget(icon);

        connect(icon,&QObject::destroyed,icon,&TrayIcon::notifyAppFreeze);
        connect(icon,&TrayIcon::notifyTray,this,&UKUITray::freezeTrayApp);
        connect(icon, &QObject::destroyed, this, &UKUITray::onIconDestroyed);
    }
}

void UKUITray::addStorageIcon(Window winId)
{
    TrayIcon *storageicon = findStorageIcon(winId);
    if(storageicon)
        return;
    else
    {
        storageicon = new TrayIcon(winId, mIconSize, this);
        mIcons.append(storageicon);
        mStorageIcons.append(storageicon);
        //storageLayout->addWidget(storageicon);

        connect(storageicon,&QObject::destroyed,storageicon,&TrayIcon::notifyAppFreeze);
        connect(storageicon,&TrayIcon::notifyTray,this,&UKUITray::freezeTrayApp);
        connect(storageicon, &QObject::destroyed, this, &UKUITray::onIconDestroyed);
        if(mStorageIcons.size() > 0)
        {
            if(!mBtn->isVisible())
            {
                mBtn->setVisible(true);
            }
            handleStorageUi();
        }
    }
}

/*
 * @brief It is not yet clear whether the feature will be released
 * Note:
 * 关于完全隐藏托盘图标（不在任务栏中也不在收纳栏中）
 * 目前依然采用“隐藏托盘图标即放置到收纳栏”的方案
 * hide区域是一个在tray和storage之外的区域，没有界面
 *
 * 在moveIconToTray ，moveIconToStorage 中均判断是否为一个hide图标
 *
*/
void UKUITray::addHideIcon(Window winId)
{
    TrayIcon *hideicon = findHideIcon(winId);
    if(!hideicon)
        return;
    else
    {
        mIcons.append(hideicon);
        mHideIcons.append(hideicon);
    }
}

void UKUITray::moveIconToTray(Window winId)
{
    qDebug()<<"inter moveIconToTray";
    TrayIcon *storageicon = findStorageIcon(winId);
    TrayIcon *hideicon = findHideIcon(winId);

    if(!storageicon && !hideicon)
        return;
    else if(storageicon)
    {
        mStorageIcons.removeOne(storageicon);
        //storageLayout->removeWidget(storageicon);
        disconnect(storageicon, &QObject::destroyed, this, &UKUITray::onIconDestroyed);
        mTrayIcons.append(storageicon);
        layout()->addWidget(storageicon);

        if(mStorageIcons.size() > 0)
        {
            handleStorageUi();
        }
        else
        {
            mBtn->setVisible(false);
        }

        connect(storageicon,&QObject::destroyed,storageicon,&TrayIcon::notifyAppFreeze);
        connect(storageicon,&TrayIcon::notifyTray,this,&UKUITray::freezeTrayApp);
        connect(storageicon, &QObject::destroyed, this, &UKUITray::onIconDestroyed);

    }

    else if(hideicon)
    {
        qDebug()<<"inter   move Icon to tray   findHideIcon";
        mHideIcons.removeOne(hideicon);
        //storageLayout->removeWidget(storageicon);
        disconnect(hideicon, &QObject::destroyed, this, &UKUITray::onIconDestroyed);
        mTrayIcons.append(hideicon);
        layout()->addWidget(hideicon);

        //      connect(storageicon,&QObject::destroyed,storageicon,&TrayIcon::notifyAppFreeze);
        //      connect(storageicon,&TrayIcon::notifyTray,this,&UKUITray::freezeTrayApp);
        connect(hideicon, &QObject::destroyed, this, &UKUITray::onIconDestroyed);
    }
}

void UKUITray::moveIconToStorage(Window winId)
{
    qDebug()<<"inter moveIconToStorage";
    TrayIcon *icon = findTrayIcon(winId);
    TrayIcon *hideicon = findHideIcon(winId);
    if(!icon && !hideicon)
        return;
    else if(icon)
    {
        layout()->removeWidget(icon);
        mTrayIcons.removeOne(icon);
        disconnect(icon, &QObject::destroyed, this, &UKUITray::onIconDestroyed);
        mStorageIcons.append(icon);

        if(mStorageIcons.size() > 0)
        {
            if(!mBtn->isVisible())
            {
                mBtn->setVisible(true);
            }
            handleStorageUi();
        }
        else
        {
            mBtn->setVisible(false);
        }

        connect(icon,&QObject::destroyed,icon,&TrayIcon::notifyAppFreeze);
        connect(icon,&TrayIcon::notifyTray,this,&UKUITray::freezeTrayApp);
        connect(icon, &QObject::destroyed, this, &UKUITray::onIconDestroyed);
    }

    else if(hideicon)
    {
        mHideIcons.removeOne(hideicon);
        //storageLayout->removeWidget(storageicon);
        disconnect(hideicon, &QObject::destroyed, this, &UKUITray::onIconDestroyed);
        mStorageIcons.append(hideicon);

        if(mStorageIcons.size() > 0)
        {
            if(!mBtn->isVisible())
            {
                mBtn->setVisible(true);
            }
            handleStorageUi();
        }
        else
        {
            mBtn->setVisible(false);
        }

        //      connect(storageicon,&QObject::destroyed,storageicon,&TrayIcon::notifyAppFreeze);
        //      connect(storageicon,&TrayIcon::notifyTray,this,&UKUITray::freezeTrayApp);
        connect(hideicon, &QObject::destroyed, this, &UKUITray::onIconDestroyed);
    }
}

void UKUITray::moveIconToHide(Window winId)
{
    TrayIcon *trayicon = findTrayIcon(winId);
    TrayIcon *storageicon = findStorageIcon(winId);
    if(!trayicon && !storageicon)
        return;
    else if(trayicon)
    {
        layout()->removeWidget(trayicon);
        mTrayIcons.removeOne(trayicon);
        disconnect(trayicon, &QObject::destroyed, this, &UKUITray::onIconDestroyed);
        mHideIcons.append(trayicon);
        connect(trayicon, &QObject::destroyed, this, &UKUITray::onIconDestroyed);
    }

    else if(storageicon)
    {
        mStorageIcons.removeOne(storageicon);
        disconnect(storageicon, &QObject::destroyed, this, &UKUITray::onIconDestroyed);
        mHideIcons.append(storageicon);

        if(mStorageIcons.size() > 0)
        {
            if(!mBtn->isVisible())
            {
                mBtn->setVisible(true);
            }
            handleStorageUi();
        }
        else
        {
            mBtn->setVisible(false);
        }
        connect(storageicon, &QObject::destroyed, this, &UKUITray::onIconDestroyed);
    }
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
            if(bba.isEmpty())
            {
                continue;
            }
            settings= new QGSettings(id, bba);
            if(settings)
            {
                if(settings->keys().contains(RECORD_KEY))
                {
                    settings->set(ACTION_KEY,settings->get(RECORD_KEY).toString());
                }

                if(settings->keys().contains(ACTION_KEY))
                {
                    actionStr = settings->get(ACTION_KEY).toString();
                }

                if(settings->keys().contains(NAME_KEY))
                {
                    nameStr = settings->get(NAME_KEY).toString();
                }
            }

            if(nameStr==xfitMan().getApplicationName(wid))
            {
                settings->set(BINDING_KEY, wid);
                bingdingStr=wid;
                if(QString::compare(actionStr,"tray")==0)
                {
                    addTrayIcon(bingdingStr);
                }
                if(QString::compare(actionStr,"storage")==0)
                {
                    addStorageIcon(bingdingStr);
                }
                if(QString::compare(actionStr,"hide")==0)
                {
                    addHideIcon(bingdingStr);
                }
                connect(settings, &QGSettings::changed, this, [=] (const QString &key)
                {
                    if(key=="action"){
                        if(QString::compare(settings->get(ACTION_KEY).toString(),"tray")==0){
                            moveIconToTray(bingdingStr);
                        }
                        else if(QString::compare(settings->get(ACTION_KEY).toString(),"storage")==0)
                        {
                            moveIconToStorage(bingdingStr);
                        }
                        else if(QString::compare(settings->get(ACTION_KEY).toString(),"hide")==0)
                        {

                            moveIconToHide(bingdingStr);
                        }
                        else if(QString::compare(settings->get(ACTION_KEY).toString(),"freeze")==0)
                        {

                        }
                    }
                });
                break;
            }
        }
        settings->deleteLater();
        delete  prepath;
        count++;
    }

    if(count >= existsPath.count())
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
            trayIconNameList<<"ukui-volume-control-applet-qt"<<"kylin-nm"<<"ukui-sidebar"<<"indicator-china-weather"<<"ukui-flash-disk";
            if(trayIconNameList.contains(xfitMan().getApplicationName(wid)))
            {
                newsetting->set(ACTION_KEY,"tray");
                newsetting->set(RECORD_KEY,"tray");
                addTrayIcon(wid);
            }
            else
            {
                newsetting->set(ACTION_KEY,"storage");
                newsetting->set(RECORD_KEY,"storage");
                addStorageIcon(wid);
            }
        }
        delete newsetting;

        QGSettings* settings;
        const QByteArray keyId(KEYBINDINGS_CUSTOM_SCHEMA);
        if(QGSettings::isSchemaInstalled(keyId))
        {
            settings= new QGSettings(id, idd,this);
            connect(settings, &QGSettings::changed, this, [=] (const QString &key)
            {
                if(key=="action")
                {
                    if(QString::compare(settings->get(ACTION_KEY).toString(),"tray")==0)
                    {
                        moveIconToTray(wid);
                    }
                    else if(QString::compare(settings->get(ACTION_KEY).toString(),"storage")==0){
                        moveIconToStorage(wid);
                    }
                    else if(QString::compare(settings->get(ACTION_KEY).toString(),"hide")==0){
                        moveIconToHide(wid);
                    }
                    else{
                        qDebug()<<"get error action";
                        qWarning()<<"get error action";
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
        QString p =KEYBINDINGS_CUSTOM_DIR;
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

/*
 * @brief  收纳栏样式
 *
 * 根据panel的大小自动调节收纳栏
 * 未设置m_pwidget的透明属性会导致收纳栏异常（布局混乱）
 * 因此不使用QWidget，而是继承 QWidget 写一个 UKUiStorageWidget，并设置其基础样式
 *
*/
void UKUITray::handleStorageUi()
{
    //    qDebug()<<"void UKUITray::handleStorageUi():"<<mStorageIcons.size();
    int winWidth = 0;
    int winHeight = 0;
    if(m_pwidget)
    {
        if(storageFrame->layout()->count())
        {
            storageFrame->layout()->removeWidget(m_pwidget);
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
    m_pwidget->setLayout(new UKUi::GridLayout);

#define mWinWidth 46
#define mWinHeight 46
    switch(mStorageIcons.size())
    {
    case 1:
        winWidth  = mWinWidth;
        winHeight = mWinHeight;
        m_pwidget->setFixedSize(winWidth,winHeight);
        dynamic_cast<UKUi::GridLayout*>(m_pwidget->layout())->setColumnCount(1);
        dynamic_cast<UKUi::GridLayout*>(m_pwidget->layout())->setRowCount(1);
        break;
    case 2:
        winWidth  = mWinWidth*2;
        winHeight = mWinHeight;
        m_pwidget->setFixedSize(winWidth,winHeight);
        dynamic_cast<UKUi::GridLayout*>(m_pwidget->layout())->setColumnCount(2);
        dynamic_cast<UKUi::GridLayout*>(m_pwidget->layout())->setRowCount(1);
        break;
    case 3:
        winWidth  = mWinWidth*3;
        winHeight = mWinHeight;
        m_pwidget->setFixedSize(winWidth,winHeight);
        dynamic_cast<UKUi::GridLayout*>(m_pwidget->layout())->setColumnCount(3);
        dynamic_cast<UKUi::GridLayout*>(m_pwidget->layout())->setRowCount(1);
        break;
    case 4:
        winWidth  = mWinWidth*3;
        winHeight = mWinHeight*2;
        m_pwidget->setFixedSize(winWidth,winHeight);
        dynamic_cast<UKUi::GridLayout*>(m_pwidget->layout())->setColumnCount(3);
        dynamic_cast<UKUi::GridLayout*>(m_pwidget->layout())->setRowCount(2);
        break;
    case 5:
        winWidth  = mWinWidth*3;
        winHeight = mWinHeight*2;
        m_pwidget->setFixedSize(winWidth,winHeight);
        dynamic_cast<UKUi::GridLayout*>(m_pwidget->layout())->setColumnCount(3);
        dynamic_cast<UKUi::GridLayout*>(m_pwidget->layout())->setRowCount(2);
        break;
    case 6:
        winWidth  = mWinWidth*3;
        winHeight = mWinHeight*2;
        m_pwidget->setFixedSize(winWidth,winHeight);
        dynamic_cast<UKUi::GridLayout*>(m_pwidget->layout())->setColumnCount(3);
        dynamic_cast<UKUi::GridLayout*>(m_pwidget->layout())->setRowCount(2);
        break;
    case 7:
        winWidth  = mWinWidth*3;
        winHeight = mWinHeight*3;
        m_pwidget->setFixedSize(winWidth,winHeight);
        dynamic_cast<UKUi::GridLayout*>(m_pwidget->layout())->setColumnCount(3);
        dynamic_cast<UKUi::GridLayout*>(m_pwidget->layout())->setRowCount(3);
        break;
    case 8:
        winWidth  = mWinWidth*3;
        winHeight = mWinHeight*3;
        m_pwidget->setFixedSize(winWidth,winHeight);
        dynamic_cast<UKUi::GridLayout*>(m_pwidget->layout())->setColumnCount(3);
        dynamic_cast<UKUi::GridLayout*>(m_pwidget->layout())->setRowCount(3);
        break;
    case 9:
        winWidth  = mWinWidth*3;
        winHeight = mWinHeight*3;
        m_pwidget->setFixedSize(winWidth,winHeight);
        dynamic_cast<UKUi::GridLayout*>(m_pwidget->layout())->setColumnCount(3);
        dynamic_cast<UKUi::GridLayout*>(m_pwidget->layout())->setRowCount(3);
        break;
    case 10:
        winWidth  = mWinWidth*3;
        winHeight = mWinHeight*4;
        m_pwidget->setFixedSize(winWidth,winHeight);
        dynamic_cast<UKUi::GridLayout*>(m_pwidget->layout())->setColumnCount(3);
        dynamic_cast<UKUi::GridLayout*>(m_pwidget->layout())->setRowCount(4);
        break;
    case 11:
        winWidth  = mWinWidth*3;
        winHeight = mWinHeight*4;
        m_pwidget->setFixedSize(winWidth,winHeight);
        dynamic_cast<UKUi::GridLayout*>(m_pwidget->layout())->setColumnCount(3);
        dynamic_cast<UKUi::GridLayout*>(m_pwidget->layout())->setRowCount(4);
        break;
    case 12:
        winWidth  = mWinWidth*3;
        winHeight = mWinHeight*4;
        m_pwidget->setFixedSize(winWidth,winHeight);
        dynamic_cast<UKUi::GridLayout*>(m_pwidget->layout())->setColumnCount(3);
        dynamic_cast<UKUi::GridLayout*>(m_pwidget->layout())->setRowCount(4);
        break;
    default:
        break;
    }

    for(auto it = mStorageIcons.begin();it != mStorageIcons.end();++it)
    {
        m_pwidget->layout()->addWidget(*it);
        (*it)->setFixedSize(mWinWidth,mWinHeight);
    }
    storageFrame->layout()->addWidget(m_pwidget);
    //    qDebug()<<"m_pwidget:"<<m_pwidget->size();
    storageFrame->setFixedSize(winWidth,winHeight);
    //    qDebug()<<"tys size"<<storageFrame->width()<<","<<storageFrame->height();
}

UKUIStorageFrame::UKUIStorageFrame(QWidget *parent):
    QWidget(parent, Qt::Popup)
{
    installEventFilter(this);
    setLayout(new UKUi::GridLayout(this));
    dynamic_cast<UKUi::GridLayout*>(layout())->setRowCount(1);
    dynamic_cast<UKUi::GridLayout*>(layout())->setColumnCount(1);
    setMinimumHeight(0);
    setMinimumWidth(0);
    setAttribute(Qt::WA_TranslucentBackground);//设置窗口背景透明
    /*
     * @brief setWindowFlags
     *
     * 冲突的窗口属性 这里本应使用Popup窗口属性，但是popup的属性与托盘有冲突
     * 会使得点击事件无法生效
     *
     * 备选方案是使用QToolTip 这导致了无法进入事件过滤来检测活动窗口的变化
     *
     * Qt::WindowStaysOnTopHint | Qt::Tool | Qt::FramelessWindowHint
     * 这三个参数分别代表 设置窗体一直置顶，并且不会抢焦点 | 工具窗口 |设置窗体无边框，不可拖动拖拽拉伸
     *
     * 但是在某些情况下会出现在任务啦上依然会显示窗口，因此加入新的属性 X11BypassWindowManagerHint
     */
    setWindowFlags(Qt::WindowStaysOnTopHint | Qt::Tool | Qt::FramelessWindowHint| Qt::X11BypassWindowManagerHint);
    _NET_SYSTEM_TRAY_OPCODE = XfitMan::atom("_NET_SYSTEM_TRAY_OPCODE");

}

UKUIStorageFrame::~UKUIStorageFrame(){
}

//bool UKUIStorageFrame::event(QEvent *event)
//{
//    if (event->type() == QEvent::WindowDeactivate) {
//        qDebug()<<"UKUIStorageFrame  enter";
//        if (QApplication::activeWindow() != this && flag==true) {
//            this->hide();
//            qDebug()<<"UKUIStorageFrame  hide";
//        }
//    }
//    return QWidget::event(event);
//}

/*
 * 事件过滤，检测鼠标点击外部活动区域则收回收纳栏
*/
bool UKUIStorageFrame::eventFilter(QObject *obj, QEvent *event)
{
    //    Q_UNUSED(obj);
    //    Q_UNUSED(event);

    if (obj == this)
    {
        if (event->type() == QEvent::WindowDeactivate &&status==ST_SHOW )
        {
            this->hide();
            status=ST_HIDE;
            return true;
        } else if (event->type() == QEvent::StyleChange) {
        }
    }

    if (!isActiveWindow())
    {
        activateWindow();
    }
    return false;
}

void UKUIStorageFrame::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    p.setBrush(QBrush(QColor(0x13,0x14,0x14,0x4d)));
    p.setPen(Qt::NoPen);
    p.setRenderHint(QPainter::Antialiasing);  // 反锯齿;
    QPainterPath path;
    p.drawRoundedRect(opt.rect,6,6);
    path.addRoundedRect(opt.rect,6,6);
    setProperty("blurRegion",QRegion(path.toFillPolygon().toPolygon()));
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

UKUiStorageWidget::UKUiStorageWidget(){
    setAttribute(Qt::WA_TranslucentBackground);//设置窗口背景透明
}

UKUiStorageWidget::~UKUiStorageWidget(){
}

void UKUiStorageWidget::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    p.setBrush(QBrush(QColor(0x13,0x14,0x14,0x4d)));
    p.setPen(Qt::NoPen);
    p.setRenderHint(QPainter::Antialiasing);
    p.drawRoundedRect(opt.rect,6,6);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
