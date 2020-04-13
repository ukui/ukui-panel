/*
 * Copyright (C) 2019 Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
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
#include "MainController.h"

MainController* MainController::mSelf = 0;  //static variable
MainController* MainController::self()      //static function    //complete the singleton object
{
    if (!mSelf)
    {
        mSelf = new MainController;
    }
    return mSelf;
}

MainController::MainController()
{
    init();
    m_DiskWindow = new MainWindow;         //main process singleton object
}

MainController::~MainController()
{
}

void MainController::init()                   //init select
{
    if(IsNotRunning())
    {
        creatDBusService();                 //create connect
        qDebug()<<"--------------creatDBusService";
    }
    else
    {
        qDebug()<<"ukui-flash-disk is running";  //or finish the process
        exit(0);
    }
}

int MainController::IsNotRunning()
{
    char service_name[SERVICE_NAME_SIZE];
    memset(service_name, 0, SERVICE_NAME_SIZE);
    snprintf(service_name, SERVICE_NAME_SIZE, "%s_%d",UKUI_FLASH_DISK_SERVICE,getuid());
    QDBusConnection conn = QDBusConnection::sessionBus();
    if (!conn.isConnected())
        return 0;

    QDBusReply<QString> reply = conn.interface()->call("GetNameOwner", service_name);
    return reply.value() == "";
}

void MainController::creatDBusService()
{
    // 用于建立到session bus的连接
    QDBusConnection bus = QDBusConnection::sessionBus();
    // 在session bus上注册名为"com.kylin_user_guide.hotel"的service

    char service_name[SERVICE_NAME_SIZE];
    memset(service_name, 0, SERVICE_NAME_SIZE);
    snprintf(service_name, SERVICE_NAME_SIZE, "%s_%d",UKUI_FLASH_DISK_SERVICE,getuid());

    if (!bus.registerService(service_name))
    {  //注意命名规则-和_
            qDebug() << bus.lastError().message();
            exit(1);
    }
    // "QDBusConnection::ExportAllSlots"表示把类Hotel的所有Slot都导出为这个Object的method
    bus.registerObject("/", this ,QDBusConnection::ExportAllSlots);
}
