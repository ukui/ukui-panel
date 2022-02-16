#include "watchermanager.h"
#include <QtDBus>



WatcherManager::WatcherManager(QObject *parent) : QObject(parent)
{
   register_dbus();
}

WatcherManager::~WatcherManager()
{
    delete taskbar_dbus;
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


    taskbar_dbus=new PinToTaskbar;
    new PanelAdaptor(taskbar_dbus);
    QDBusConnection taskbar_connection=QDBusConnection::sessionBus();

    if(!taskbar_connection.registerService("com.ukui.panel.desktop"))
    {
        qDebug()<<"error1:"<<con.lastError().message();
    }
    if(!taskbar_connection.registerObject("/",taskbar_dbus,
                           QDBusConnection::ExportAllSlots|
                           QDBusConnection::ExportAllSignals))
    {
        qDebug()<<"error2:"<<taskbar_connection.lastError().message();
    }
}
