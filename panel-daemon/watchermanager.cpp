#include "watchermanager.h"
#include <QtDBus>
#include "dbus-server/server.h"
#include "dbus-server/dbus-adaptor.h"



WatcherManager::WatcherManager(QObject *parent) : QObject(parent)
{
   register_dbus();

}

WatcherManager::~WatcherManager()
{

}

void WatcherManager::register_dbus()
{
    Server* dbus=new Server;
    connect(dbus,&Server::DesktopFileDelete, this,[=](){
        qDebug()<<"signal send success!";
    });
    new DaemonAdaptor(dbus);
    QDBusConnection con=QDBusConnection::sessionBus();

    if(!con.registerService("org.ukui.panel.daemon"))
    {
        qDebug()<<"error1:"<<con.lastError().message();
    }
    if(!con.registerObject("/convert/desktopwid",dbus,
                           QDBusConnection::ExportAllSlots|
                           QDBusConnection::ExportAllSignals))
    {
        qDebug()<<"error2:"<<con.lastError().message();
    }
//    if(!con.registerService("org.ukui.panel") ||
//            !con.registerObject("/convert/desktopwid/",dbus,
//                                QDBusConnection::ExportAllSlots|
//                                QDBusConnection::ExportAllSignals))
//    {
//        qDebug()<<"dbus  fail";
//    }

}
