#include "watchermanager.h"
#include <QtDBus>
#include "convert-desktop-windowid/dbus-server/server.h"
#include "convert-desktop-windowid/dbus-server/dbus-adaptor.h"

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
    new PanelAdaptor(dbus);
    QDBusConnection con=QDBusConnection::sessionBus();

    if(!con.registerService("org.ukui.panel-daemon"))
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
