#include "watchermanager.h"
#include <QtDBus>
#include "convert-desktop-windowid/dbus-server/server.h"
#include "convert-desktop-windowid/dbus-server/dbus-adaptor.h"

WatcherManager::WatcherManager(QObject *parent) : QObject(parent)
{
    Server* dbus=new Server;
    new PanelAdaptor(dbus);
    QDBusConnection con=QDBusConnection::sessionBus();
    if(!con.registerService("org.ukui.panel") ||
            !con.registerObject("/convert/desktopwid/",dbus,
                                QDBusConnection::ExportAllSlots|
                                QDBusConnection::ExportAllSignals))
    {
        qDebug()<<"dbus  fail";
    }

}
