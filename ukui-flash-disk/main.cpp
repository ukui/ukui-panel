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

#include <QApplication>

#include <PeonyVolumeManager>

#include <QSystemTrayIcon>

#include <QDebug>

#include <gio/gio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <syslog.h>
#include <QDebug>
#include "UnionVariable.h"
#include "mainwindow.h"



int main(int argc, char *argv[])
{
    int fd = open("/tmp/usb-flash-disk", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fd < 0)
    {
        exit(1);
    }
    if (lockf(fd, F_TLOCK, 0))
    {
        syslog(LOG_ERR, "Can't lock single file, ukui-flash-disk is already running!");
        qDebug()<<"Can't lock single file, ukui-flash-disk is already running!";
        exit(0);
    }
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(false);        //进程不隐式退出

    QSystemTrayIcon m_systray;
    MainWindow w;
    //w.show();
    m_systray.setIcon(QIcon("/usr/share/icons/ukui-icon-theme-default/22x22/devices/drive-removable-media.png"));
    QObject::connect(&m_systray, &QSystemTrayIcon::activated, &w, &MainWindow::iconActivated);
    auto g_volume_monitor = g_volume_monitor_get();
    GList *current_mount_list = g_volume_monitor_get_mounts(g_volume_monitor);
    GList *current_device = current_mount_list;
    while (current_device)
    {
        GMount *gmount = G_MOUNT(current_device->data);
        std::shared_ptr<Peony::Mount> mount = std::make_shared<Peony::Mount>(gmount);
        qDebug()<<mount->name()<<"append to list";
        *findList()<<mount;
        current_device = current_device->next;
    }

    auto manager = Peony::VolumeManager::getInstance();

    //volumeAdded一般在设备插入时触发
    manager->connect(manager, &Peony::VolumeManager::volumeAdded, [](const std::shared_ptr<Peony::Volume> &volume)
    {
        qDebug() << "volume" << volume->name() << "added";
        g_volume_mount(volume->getGVolume(),
                       G_MOUNT_MOUNT_NONE,
                       nullptr,
                       nullptr,
                       nullptr,
                       nullptr);
    });

    manager->connect(manager, &Peony::VolumeManager::volumeRemoved, [](const std::shared_ptr<Peony::Volume> &volume)
    {
        qDebug()<< "volume" << volume->name() << "removed";
    });

    //注意mountAdded必须要挂载之后才会触发
    manager->connect(manager, &Peony::VolumeManager::mountAdded, [&](const std::shared_ptr<Peony::Mount> &mount)
    {
        qDebug()<<"mount"<<mount->name()<<"added";
        if(g_mount_can_eject(mount->getGMount()))
        {
            *findList()<<mount;
        }
        qDebug()<<"*findList():"<<findList()->size();
//        m_systray.setIcon(QIcon("/usr/share/icons/ukui-icon-theme-default/22x22/devices/drive-removable-media.png"));
//        m_systray.show();
        if(findList()->size() != 0)
        {
            m_systray.show();
        }

    });

    manager->connect(manager, &Peony::VolumeManager::mountRemoved, [&](const std::shared_ptr<Peony::Mount> &mount)
    {
        qDebug()<<"mount"<<mount->name()<<"removed";
        for (auto cachedMount : *findList())
        {
            if (cachedMount->name() == mount->name())
            {
                findList()->removeOne(cachedMount);
                if(findList()->size() == 0)
                {
                    m_systray.hide();
                }
            }
        }
    });


    return a.exec();
}
