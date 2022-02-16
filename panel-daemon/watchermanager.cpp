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
