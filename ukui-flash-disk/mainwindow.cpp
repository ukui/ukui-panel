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
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <PeonyVolumeManager>
#include <gio/gio.h>
#include <glib.h>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusMessage>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusObjectPath>
#include <QDBusReply>
#include <QSystemTrayIcon>

#include "clickLabel.h"
int MainWindow::oneVolumeDriveNum = 0;
int MainWindow::twoVolumeDriveNum = 0;
int MainWindow::threeVolumeDriveNum = 0;
int MainWindow::fourVolumeDriveNum = 0;
int MainWindow::hign;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);
    //框架的样式设置
    int hign = 0;
    QPainterPath path;
    auto rect = this->rect();
    rect.adjust(0,0,-0,-0);
    path.addRoundedRect(rect, 6, 6);
    setProperty("blurRegion", QRegion(path.toFillPolygon().toPolygon()));
    this->setStyleSheet("QWidget{border:none;border-radius:6px;}");
    this->setAttribute(Qt::WA_TranslucentBackground);//设置窗口背景透明
    ui->centralWidget->setStyleSheet(
                "#centralWidget{"
                "width:280px;"
                "height:192px;"
                "background:rgba(19,19,20,0.95);"
                "border:1px solid rgba(255, 255, 255, 0.05);"
                "opacity:0.75;"

                "border-radius:6px;"
                "box-shadow:0px 2px 6px 0px rgba(0, 0, 0, 0.2);"
                "}"
                );


    iconSystray = QIcon::fromTheme("/usr/share/icons/ukui-icon-theme-default/scalable/devices/drive-removable-media.svg");
    vboxlayout = new QVBoxLayout();

    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Popup);
    this->setWindowOpacity(0.95);
    this->resize( QSize( 280, 192 ));
    m_systray = new QSystemTrayIcon;
    //show();
    //m_systray->setIcon(QIcon("/usr/share/icons/ukui-icon-theme-default/22x22/devices/drive-removable-media.png"));
    m_systray->setIcon(iconSystray);
    m_systray->setToolTip(tr("usb management tool"));
    getDeviceInfo();
    connect(m_systray, &QSystemTrayIcon::activated, this, &MainWindow::iconActivated);
    ui->centralWidget->setLayout(vboxlayout);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::getDeviceInfo()
{

    auto g_volume_monitor = g_volume_monitor_get();
    // about the mounts
    GList *current_mount_list = g_volume_monitor_get_mounts(g_volume_monitor);
    GList *current_device = current_mount_list;
    while (current_device)
    {
        GMount *gmount = G_MOUNT(current_device->data);
        std::shared_ptr<Peony::Mount> mount = std::make_shared<Peony::Mount>(gmount);
        qDebug()<<mount->name()<<"append to list";
        if(g_mount_can_eject(mount->getGMount()))
        {
            *findList()<<mount;
        }
        current_device = current_device->next;
    }

    //about the volumes
    GList *current_volume_list = g_volume_monitor_get_volumes(g_volume_monitor);
    GList *current_volume_device = current_volume_list;
    while(current_volume_device)
    {
        GVolume *gvolume = G_VOLUME(current_volume_device->data);
        std::shared_ptr<Peony::Volume> volume = std::make_shared<Peony::Volume>(gvolume);
        qDebug()<<volume->name()<<"append to list";
        if(g_volume_can_eject(volume->getGVolume()) || g_drive_can_eject(g_volume_get_drive(volume->getGVolume())) || g_drive_can_stop(g_volume_get_drive(volume->getGVolume())))
        {
            *findVolumeList()<<volume;
        }
        current_volume_device = current_volume_device->next;
        qDebug()<<"current_volume_lists"<<findVolumeList()->size();
    }
    for (auto cacheVolume : *findVolumeList())
    {
        if(g_volume_can_eject(cacheVolume->getGVolume()) || g_drive_can_eject(g_volume_get_drive(cacheVolume->getGVolume())) || g_drive_can_stop(g_volume_get_drive(cacheVolume->getGVolume())))
        {
            g_volume_mount(cacheVolume->getGVolume(),
                           G_MOUNT_MOUNT_NONE,
                           nullptr,
                           nullptr,
                           nullptr,
                           nullptr);
        }
    }

    //about the drives
    GList *current_drive_list = g_volume_monitor_get_connected_drives(g_volume_monitor);
    GList *current_drive_device = current_drive_list;
    while(current_drive_device)
    {
        GDrive *gdrive = G_DRIVE(current_drive_device->data);
        std::shared_ptr<Peony::Drive> drive = std::make_shared<Peony::Drive>(gdrive);
        qDebug()<<drive->name()<<"append to list";
        if(g_drive_can_stop(drive->getGDrive()) || g_drive_can_eject(drive->getGDrive()))
        {
            *findDriveList()<<drive;
            if(g_list_length(g_drive_get_volumes(drive->getGDrive())) == 1)
            {
                MainWindow::oneVolumeDriveNum += 1;
            }

            if(g_list_length(g_drive_get_volumes(drive->getGDrive())) == 2)
            {
                MainWindow::twoVolumeDriveNum += 1;
            }

            if(g_list_length(g_drive_get_volumes(drive->getGDrive())) == 3)
            {
                MainWindow::threeVolumeDriveNum += 1;
            }

            if(g_list_length(g_drive_get_volumes(drive->getGDrive())) == 4)
            {
                MainWindow::fourVolumeDriveNum += 1;
            }
        }
        current_drive_device = current_drive_device->next;
        qDebug()<<"current_drive_lists"<<findDriveList()->size()<<"wwj,nizhendeshirangwowuyule";
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
        if(g_mount_can_eject(mount->getGMount()) || g_drive_can_stop(g_mount_get_drive(mount->getGMount())) )
        {
            *findList()<<mount;
        }
        qDebug()<<"*findList():"<<findList()->size();
        m_systray->show();


    });

    manager->connect(manager, &Peony::VolumeManager::mountRemoved, [&](const std::shared_ptr<Peony::Mount> &mount)
    {
        qDebug()<<"mount"<<mount->name()<<"removed";
        for (auto cachedMount : *findList())
        {
            if (cachedMount->name() == mount->name())
            {
                //g_drive_get_name(g_volume_get_drive(g_mount_get_volume(cachedMount->getGMount())))
                //cachedMount->name())
                findList()->removeOne(cachedMount);
                if(g_list_length(g_drive_get_volumes(g_mount_get_drive(cachedMount->getGMount()))) == 1)
                {
                    MainWindow::oneVolumeDriveNum -= 1;
                }
                if(g_list_length(g_drive_get_volumes(g_mount_get_drive(cachedMount->getGMount()))) == 2)
                {
                    MainWindow::twoVolumeDriveNum -= 1;
                }
                if(g_list_length(g_drive_get_volumes(g_mount_get_drive(cachedMount->getGMount()))) == 3)
                {
                    MainWindow::threeVolumeDriveNum -= 1;
                }
                if(g_list_length(g_drive_get_volumes(g_mount_get_drive(cachedMount->getGMount()))) == 4)
                {
                    MainWindow::fourVolumeDriveNum -= 1;
                }
                ejectInterface *ForEject = new ejectInterface(nullptr,g_drive_get_name(g_mount_get_drive(cachedMount->getGMount())));
                int screenNum = QGuiApplication::screens().count();
                int panelHeight = getPanelHeight("PanelHeight");
                int position =0;
                position = getPanelPosition("PanelPosion");
                int screen = 0;
                QRect rect;
                int localX ,availableWidth,totalWidth;
                int localY,availableHeight,totalHeight;

                qDebug() << "任务栏位置"<< position;
                if (screenNum > 1)
                {
                    if (position == 3)                                  //on the right
                    {
                        screen = screenNum - 1;

                        //Available screen width and height
                        availableWidth =QGuiApplication::screens().at(screen)->geometry().x() +  QGuiApplication::screens().at(screen)->size().width()-panelHeight;
                        availableHeight = QGuiApplication::screens().at(screen)->availableGeometry().height();

                        //total width
                        totalWidth =  QGuiApplication::screens().at(0)->size().width() + QGuiApplication::screens().at(screen)->size().width();
                        totalHeight = QGuiApplication::screens().at(screen)->size().height();
                    }
                    else if(position  ==0 || position ==1)                  //above or bellow
                    {
                        availableHeight = QGuiApplication::screens().at(0)->size().height() - panelHeight;
                        availableWidth = QGuiApplication::screens().at(0)->size().width();
                        totalHeight = QGuiApplication::screens().at(0)->size().height();
                        totalWidth = QGuiApplication::screens().at(0)->size().width();
                    }
                    else
                    {
                        availableHeight = QGuiApplication::screens().at(0)->availableGeometry().height();
                        availableWidth = QGuiApplication::screens().at(0)->availableGeometry().width();
                        totalHeight = QGuiApplication::screens().at(0)->size().height();
                        totalWidth = QGuiApplication::screens().at(0)->size().width();
                    }
                }

                else
                {
                    availableHeight = QGuiApplication::screens().at(0)->availableGeometry().height();
                    availableWidth = QGuiApplication::screens().at(0)->availableGeometry().width();
                    totalHeight = QGuiApplication::screens().at(0)->size().height();
                    totalWidth = QGuiApplication::screens().at(0)->size().width();
                }
                //show the location of the systemtray
                rect = m_systray->geometry();
                localX = rect.x() - (ForEject->width()/2 - rect.size().width()/2) ;
                localY = availableHeight - ForEject->height();
                //modify location
                if (position == 0)
                { //下
                    if (availableWidth - rect.x() - rect.width()/2 < ForEject->width() / 2)
                        ForEject->setGeometry(availableWidth-ForEject->width(),availableHeight-ForEject->height()-3,ForEject->width(),ForEject->height());
                    else
                        ForEject->setGeometry(localX-16,availableHeight-ForEject->height()-3,ForEject->width(),ForEject->height());
                }
                else if (position == 1)
                { //上
                    if (availableWidth - rect.x() - rect.width()/2 < ForEject->width() / 2)
                        ForEject->setGeometry(availableWidth-ForEject->width(),totalHeight-availableHeight+3,ForEject->width(),ForEject->height());
                    else
                        ForEject->setGeometry(localX-16,totalHeight-availableHeight+3,ForEject->width(),ForEject->height());
                }
                else if (position == 2)
                {
                    if (availableHeight - rect.y() - rect.height()/2 > ForEject->height() /2)
                        ForEject->setGeometry(panelHeight + 3,rect.y() + (rect.width() /2) -(ForEject->height()/2) ,ForEject->width(),ForEject->height());
                    else
                        ForEject->setGeometry(panelHeight+3,localY,ForEject->width(),ForEject->height());//左
                }
                else if (position == 3)
                {
                    localX = availableWidth - ForEject->width();
                    if (availableHeight - rect.y() - rect.height()/2 > ForEject->height() /2)
                    {
                        ForEject->setGeometry(availableWidth - ForEject->width() -3,rect.y() + (rect.height() /2) -(ForEject->height()/2),ForEject->width(),ForEject->height());
                    }
                    else
                        ForEject->setGeometry(localX-3,localY,ForEject->width(),ForEject->height());
                }
                qDebug()<<"wohaoxiangni,haoxiangni,quebunengziyi";
                qDebug()<<g_drive_get_name(g_mount_get_drive(cachedMount->getGMount()))<<"drive:name";
                ForEject->show();
                if(findList()->size() == 0)
                {
                    m_systray->hide();
                }
            }
        }
    });

    manager->connect(manager, &Peony::VolumeManager::driveConnected, [=](const std::shared_ptr<Peony::Drive> &drive)
    {
        qDebug() << "drive" << drive->name() << "added";
        g_drive_start(drive->getGDrive(),
                       G_DRIVE_START_NONE,
                       nullptr,
                       nullptr,
                       nullptr,
                       nullptr);
        if(g_drive_can_eject(drive->getGDrive()) || g_drive_can_stop(drive->getGDrive()))
        {
            *findDriveList()<<drive;
            if(g_list_length(g_drive_get_volumes(drive->getGDrive())) == 1)
            {
                MainWindow::oneVolumeDriveNum += 1;
            }

            if(g_list_length(g_drive_get_volumes(drive->getGDrive())) == 2)
            {
                MainWindow::twoVolumeDriveNum += 1;
            }

            if(g_list_length(g_drive_get_volumes(drive->getGDrive())) == 3)
            {
                MainWindow::threeVolumeDriveNum += 1;
            }

            if(g_list_length(g_drive_get_volumes(drive->getGDrive())) == 4)
            {
                MainWindow::fourVolumeDriveNum += 1;
            }
        }

        qDebug()<<"实时监控我的drive数"<<findDriveList()->size();
    });


    manager->connect(manager, &Peony::VolumeManager::driveDisconnected, [=](const std::shared_ptr<Peony::Drive> &drive)
    {
        qDebug()<<"drive"<<drive->name()<<"removed"<<"-------------------------------------------------------------------------------------";
        for (auto cachedDrive : *findDriveList())
        {
            if (cachedDrive->name() == drive->name())
            {
                //g_drive_get_name(g_volume_get_drive(g_mount_get_volume(cachedMount->getGMount())))
                //cachedMount->name())
                if(g_list_length(g_drive_get_volumes(cachedDrive->getGDrive())) == 1)
                {
                    MainWindow::oneVolumeDriveNum -= 1;
                }

                if(g_list_length(g_drive_get_volumes(cachedDrive->getGDrive())) == 2)
                {
                    MainWindow::twoVolumeDriveNum -= 1;
                }

                if(g_list_length(g_drive_get_volumes(cachedDrive->getGDrive())) == 3)
                {
                    MainWindow::threeVolumeDriveNum -= 1;
                }

                if(g_list_length(g_drive_get_volumes(cachedDrive->getGDrive())) == 4)
                {
                    MainWindow::fourVolumeDriveNum -= 1;
                }
                findDriveList()->removeOne(cachedDrive);
                ejectInterface *ForEject = new ejectInterface(nullptr,g_drive_get_name(drive->getGDrive()));
                int screenNum = QGuiApplication::screens().count();
                int panelHeight = getPanelHeight("PanelHeight");
                int position =0;
                position = getPanelPosition("PanelPosion");
                int screen = 0;
                QRect rect;
                int localX ,availableWidth,totalWidth;
                int localY,availableHeight,totalHeight;

                qDebug() << "任务栏位置"<< position;
                if (screenNum > 1)
                {
                    if (position == 3)                                  //on the right
                    {
                        screen = screenNum - 1;

                        //Available screen width and height
                        availableWidth =QGuiApplication::screens().at(screen)->geometry().x() +  QGuiApplication::screens().at(screen)->size().width()-panelHeight;
                        availableHeight = QGuiApplication::screens().at(screen)->availableGeometry().height();

                        //total width
                        totalWidth =  QGuiApplication::screens().at(0)->size().width() + QGuiApplication::screens().at(screen)->size().width();
                        totalHeight = QGuiApplication::screens().at(screen)->size().height();
                    }
                    else if(position  ==0 || position ==1)                  //above or bellow
                    {
                        availableHeight = QGuiApplication::screens().at(0)->size().height() - panelHeight;
                        availableWidth = QGuiApplication::screens().at(0)->size().width();
                        totalHeight = QGuiApplication::screens().at(0)->size().height();
                        totalWidth = QGuiApplication::screens().at(0)->size().width();
                    }
                    else
                    {
                        availableHeight = QGuiApplication::screens().at(0)->availableGeometry().height();
                        availableWidth = QGuiApplication::screens().at(0)->availableGeometry().width();
                        totalHeight = QGuiApplication::screens().at(0)->size().height();
                        totalWidth = QGuiApplication::screens().at(0)->size().width();
                    }
                }

                else
                {
                    availableHeight = QGuiApplication::screens().at(0)->availableGeometry().height();
                    availableWidth = QGuiApplication::screens().at(0)->availableGeometry().width();
                    totalHeight = QGuiApplication::screens().at(0)->size().height();
                    totalWidth = QGuiApplication::screens().at(0)->size().width();
                }
                //show the location of the systemtray
                rect = m_systray->geometry();
                localX = rect.x() - (ForEject->width()/2 - rect.size().width()/2) ;
                localY = availableHeight - ForEject->height();
                //modify location
                if (position == 0)
                { //下
                    if (availableWidth - rect.x() - rect.width()/2 < ForEject->width() / 2)
                        ForEject->setGeometry(availableWidth-ForEject->width(),availableHeight-ForEject->height()-3,ForEject->width(),ForEject->height());
                    else
                        ForEject->setGeometry(localX-16,availableHeight-ForEject->height()-3,ForEject->width(),ForEject->height());
                }
                else if (position == 1)
                { //上
                    if (availableWidth - rect.x() - rect.width()/2 < ForEject->width() / 2)
                        ForEject->setGeometry(availableWidth-ForEject->width(),totalHeight-availableHeight+3,ForEject->width(),ForEject->height());
                    else
                        ForEject->setGeometry(localX-16,totalHeight-availableHeight+3,ForEject->width(),ForEject->height());
                }
                else if (position == 2)
                {
                    if (availableHeight - rect.y() - rect.height()/2 > ForEject->height() /2)
                        ForEject->setGeometry(panelHeight + 3,rect.y() + (rect.width() /2) -(ForEject->height()/2) ,ForEject->width(),ForEject->height());
                    else
                        ForEject->setGeometry(panelHeight+3,localY,ForEject->width(),ForEject->height());//左
                }
                else if (position == 3)
                {
                    localX = availableWidth - ForEject->width();
                    if (availableHeight - rect.y() - rect.height()/2 > ForEject->height() /2)
                    {
                        ForEject->setGeometry(availableWidth - ForEject->width() -3,rect.y() + (rect.height() /2) -(ForEject->height()/2),ForEject->width(),ForEject->height());
                    }
                    else
                        ForEject->setGeometry(localX-3,localY,ForEject->width(),ForEject->height());
                }
                qDebug()<<"wohaoxiangni,haoxiangni,quebunengziyi";
                qDebug()<<g_drive_get_name(cachedDrive->getGDrive())<<"drive:name";
                ForEject->show();
                if(findDriveList()->size() == 0)
                {
                    m_systray->hide();
                }
            }
        }
        qDebug()<<findDriveList()->size()<<"实时监控我的drive";
    });
//    manager->connect(manager, &Peony::VolumeManager::volumeRemoved, [&](const std::shared_ptr<Peony::Volume> &volume)
//    {
//        qDebug()<<"volume"<<volume->name()<<"removed";
//        for (auto cacheVolume : *findVolumeList())
//        {
//            if(cacheVolume->name() == volume->name())
//            {
//                findVolumeList()->removeOne(cacheVolume);
//                if(findVolumeList() == 0)
//                {
//                    m_systray->hide();
//                }

//            }
//        }
//    });
    if(findList()->size() >= 1)
    {
        m_systray->show();
    }
    qDebug()<<findList()->size()<<"rensaddedheng ru meng +++++++++++++++++++++++++ ";
    qDebug()<<findVolumeList()->size()<<"wohaoxiangni,haoxiangni______________________________";
    qDebug()<<findDriveList()->size()<<"kailedengyanqiandemuyahg==============================";
    qDebug()<<"hign"<<MainWindow::hign<<"----------------------------------------------------------------------";
}

void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    //int hign = 200;
    int num = 0;
    if ( this->vboxlayout != NULL )
    {
        QLayoutItem* item;
        while ((item = this->vboxlayout->takeAt(0)) != NULL)
        {
            delete item->widget();
            delete item;
        }
    }
    
    switch (reason)
    {
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::Context:
      if (this->isHidden())
      {
          MainWindow::hign = MainWindow::oneVolumeDriveNum*98+MainWindow::twoVolumeDriveNum*110+MainWindow::threeVolumeDriveNum*130+MainWindow::fourVolumeDriveNum*160;
          for(auto cacheDrive : *findDriveList())
          {

              this->resize(250,MainWindow::hign);
              qDebug()<<"drive name:"<<g_drive_get_name(cacheDrive->getGDrive());
              g_drive_get_volumes(cacheDrive->getGDrive());
              int DisNum = g_list_length(g_drive_get_volumes(cacheDrive->getGDrive()));
              if (DisNum >0 )
              {
                qDebug()<<"开始判断这个drive有几个volume";
                if (g_drive_can_eject(cacheDrive->getGDrive()) || g_drive_can_stop(cacheDrive->getGDrive()))
                {
                    qDebug()<<"hign"<<"------------------------------"<<MainWindow::hign;
                    if(DisNum == 1)
                    {
                       qDebug()<<"这个drive有一个volume";
                       //this->resize(250, 98);
                       UDiskPathDis1 = g_file_get_path(g_mount_get_root(g_volume_get_mount((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive->getGDrive()),0))));
                       QByteArray date = UDiskPathDis1.toLocal8Bit();
                       char *p_Change = date.data();
                       GFile *file = g_file_new_for_path(p_Change);
                       GFileInfo *info = g_file_query_filesystem_info(file,"*",nullptr,nullptr);
                       totalDis1 = g_file_info_get_attribute_uint64(info,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE);
                       if(findDriveList()->size() == 1)
                       {
                           qDebug() << "name: " << g_drive_get_name(cacheDrive->getGDrive());
                           newarea(DisNum,g_drive_get_name(cacheDrive->getGDrive()),
                                   g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive->getGDrive()),0)),
                                   NULL,NULL,NULL, totalDis1,NULL,NULL,NULL, UDiskPathDis1,NULL,NULL,NULL,1);
                       }

                       if(findDriveList()->size() >= 2)
                       {
                           newarea(DisNum,g_drive_get_name(cacheDrive->getGDrive()),
                                   g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive->getGDrive()),0)),
                                   NULL,NULL,NULL,totalDis1,NULL,NULL,NULL, UDiskPathDis1,NULL,NULL,NULL, 2);
                       }
                    }
                    if(DisNum == 2)
                    {
                        qDebug()<<"这个drive有两个volume";
                        //this->resize(250,160);
                        UDiskPathDis1 = g_file_get_path(g_mount_get_root(g_volume_get_mount((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive->getGDrive()),0))));
                        QByteArray dateDis1 = UDiskPathDis1.toLocal8Bit();
                        char *p_ChangeDis1 = dateDis1.data();
                        GFile *fileDis1 = g_file_new_for_path(p_ChangeDis1);
                        GFileInfo *infoDis1 = g_file_query_filesystem_info(fileDis1,"*",nullptr,nullptr);
                        totalDis1 = g_file_info_get_attribute_uint64(infoDis1,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE);

                        UDiskPathDis2 = g_file_get_path(g_mount_get_root(g_volume_get_mount((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive->getGDrive()),1))));
                        QByteArray dateDis2 = UDiskPathDis2.toLocal8Bit();
                        char *p_ChangeDis2 = dateDis2.data();
                        GFile *fileDis2 = g_file_new_for_path(p_ChangeDis2);
                        GFileInfo *infoDis2 = g_file_query_filesystem_info(fileDis2,"*",nullptr,nullptr);
                        totalDis2 = g_file_info_get_attribute_uint64(infoDis2,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE);

                        if(findDriveList()->size() == 1)
                        {
                            qDebug() << "name: " << g_drive_get_name(cacheDrive->getGDrive());
                            qDebug()<<"只有一个drive这个drive有两个volume";
                            newarea(DisNum,g_drive_get_name(cacheDrive->getGDrive()),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive->getGDrive()),0)),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive->getGDrive()),1)),
                                    NULL,NULL, totalDis1,totalDis2,NULL,NULL, UDiskPathDis1,UDiskPathDis2,NULL,NULL,1);
                        }

                        if(findDriveList()->size() >= 2)
                        {
                            qDebug()<<"有多个drive,这个drive有两个volume";
                            newarea(DisNum,g_drive_get_name(cacheDrive->getGDrive()),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive->getGDrive()),0)),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive->getGDrive()),1)),
                                    NULL,NULL, totalDis1,totalDis2,NULL,NULL, UDiskPathDis1,UDiskPathDis2,NULL,NULL,2);
                        }

                    }
                    if(DisNum == 3)
                    {
                        qDebug()<<"这个drive有三个volume";
                        //this->resize(250,222);
                        UDiskPathDis1 = g_file_get_path(g_mount_get_root(g_volume_get_mount((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive->getGDrive()),0))));
                        QByteArray dateDis1 = UDiskPathDis1.toLocal8Bit();
                        char *p_ChangeDis1 = dateDis1.data();
                        GFile *fileDis1 = g_file_new_for_path(p_ChangeDis1);
                        GFileInfo *infoDis1 = g_file_query_filesystem_info(fileDis1,"*",nullptr,nullptr);
                        totalDis1 = g_file_info_get_attribute_uint64(infoDis1,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE);

                        UDiskPathDis2 = g_file_get_path(g_mount_get_root(g_volume_get_mount((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive->getGDrive()),1))));
                        QByteArray dateDis2 = UDiskPathDis2.toLocal8Bit();
                        char *p_ChangeDis2 = dateDis2.data();
                        GFile *fileDis2 = g_file_new_for_path(p_ChangeDis2);
                        GFileInfo *infoDis2 = g_file_query_filesystem_info(fileDis2,"*",nullptr,nullptr);
                        totalDis2 = g_file_info_get_attribute_uint64(infoDis2,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE);

                        UDiskPathDis3 = g_file_get_path(g_mount_get_root(g_volume_get_mount((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive->getGDrive()),2))));
                        QByteArray dateDis3 = UDiskPathDis3.toLocal8Bit();
                        char *p_ChangeDis3 = dateDis3.data();
                        GFile *fileDis3 = g_file_new_for_path(p_ChangeDis3);
                        GFileInfo *infoDis3 = g_file_query_filesystem_info(fileDis3,"*",nullptr,nullptr);
                        totalDis3 = g_file_info_get_attribute_uint64(infoDis3,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE);
                        if(findDriveList()->size() == 1)
                        {
                            qDebug() << "name: " << g_drive_get_name(cacheDrive->getGDrive());
                            newarea(DisNum,g_drive_get_name(cacheDrive->getGDrive()),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive->getGDrive()),0)),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive->getGDrive()),1)),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive->getGDrive()),2)),
                                    NULL, totalDis1,totalDis2,totalDis3,NULL, UDiskPathDis1,UDiskPathDis2,UDiskPathDis3,NULL,1);
                        }

                        if(findDriveList()->size() >= 2)
                        {
                            newarea(DisNum,g_drive_get_name(cacheDrive->getGDrive()),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive->getGDrive()),0)),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive->getGDrive()),1)),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive->getGDrive()),2)),
                                    NULL, totalDis1,totalDis2,totalDis3,NULL, UDiskPathDis1,UDiskPathDis2,UDiskPathDis3,NULL,2);
                        }

                    }
                    if(DisNum == 4)
                    {
                        qDebug()<<"这个drive有四个volume";
                        //this->resize(250,134);
                        UDiskPathDis1 = g_file_get_path(g_mount_get_root(g_volume_get_mount((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive->getGDrive()),0))));
                        QByteArray dateDis1 = UDiskPathDis1.toLocal8Bit();
                        char *p_ChangeDis1 = dateDis1.data();
                        GFile *fileDis1 = g_file_new_for_path(p_ChangeDis1);
                        GFileInfo *infoDis1 = g_file_query_filesystem_info(fileDis1,"*",nullptr,nullptr);
                        totalDis1 = g_file_info_get_attribute_uint64(infoDis1,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE);

                        UDiskPathDis2 = g_file_get_path(g_mount_get_root(g_volume_get_mount((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive->getGDrive()),1))));
                        QByteArray dateDis2 = UDiskPathDis2.toLocal8Bit();
                        char *p_ChangeDis2 = dateDis2.data();
                        GFile *fileDis2 = g_file_new_for_path(p_ChangeDis2);
                        GFileInfo *infoDis2 = g_file_query_filesystem_info(fileDis2,"*",nullptr,nullptr);
                        totalDis2 = g_file_info_get_attribute_uint64(infoDis2,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE);

                        UDiskPathDis3 = g_file_get_path(g_mount_get_root(g_volume_get_mount((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive->getGDrive()),2))));
                        QByteArray dateDis3 = UDiskPathDis3.toLocal8Bit();
                        char *p_ChangeDis3 = dateDis3.data();
                        GFile *fileDis3 = g_file_new_for_path(p_ChangeDis3);
                        GFileInfo *infoDis3 = g_file_query_filesystem_info(fileDis3,"*",nullptr,nullptr);
                        totalDis3 = g_file_info_get_attribute_uint64(infoDis3,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE);

                        UDiskPathDis4 = g_file_get_path(g_mount_get_root(g_volume_get_mount((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive->getGDrive()),3))));
                        QByteArray dateDis4 = UDiskPathDis4.toLocal8Bit();
                        char *p_ChangeDis4 = dateDis4.data();
                        GFile *fileDis4 = g_file_new_for_path(p_ChangeDis4);
                        GFileInfo *infoDis4 = g_file_query_filesystem_info(fileDis4,"*",nullptr,nullptr);
                        totalDis4 = g_file_info_get_attribute_uint64(infoDis4,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE);

                        if(findDriveList()->size() == 1)
                        {
                            qDebug() << "name: " << g_drive_get_name(cacheDrive->getGDrive());
                            qDebug() << "wowowowowowowowowoowowow";
                            newarea(DisNum,g_drive_get_name(cacheDrive->getGDrive()),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive->getGDrive()),0)),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive->getGDrive()),1)),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive->getGDrive()),2)),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive->getGDrive()),2)),
                                    totalDis1,totalDis2,totalDis3,totalDis4, UDiskPathDis1,UDiskPathDis2,UDiskPathDis3,UDiskPathDis4,1);
                        }

                        if(findDriveList()->size() >= 2)
                        {
                            newarea(DisNum,g_drive_get_name(cacheDrive->getGDrive()),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive->getGDrive()),0)),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive->getGDrive()),1)),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive->getGDrive()),2)),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive->getGDrive()),3)),
                                    totalDis1,totalDis2,totalDis3,totalDis4, UDiskPathDis1,UDiskPathDis2,UDiskPathDis3,UDiskPathDis4,2);
                        }

                    }
                    qDebug()<<"-----------------------------------------------------------";
                    connect(open_widget, &QClickWidget::clickedConvert,[=]()
                    {
                        qDebug()<<"相应信号";
                        g_drive_eject_with_operation(cacheDrive->getGDrive(),
                                                     G_MOUNT_UNMOUNT_NONE,
                                                     nullptr,
                                                     nullptr,
                                                     nullptr,
                                                     nullptr);
                        this->hide();
                        QLayoutItem* item;
                        while ((item = this->vboxlayout->takeAt(0)) != NULL)
                        {
                            delete item->widget();
                            delete item;
                        }



                    });
                }

                if(findDriveList()->size() != 0)
                {
                    this->showNormal();
                    moveBottomRight();
                }
            }


      }
    }
        else
        {
            this->hide();
        }
        break;
    default:
        break;
    }
}

void MainWindow::eject_drive()
{
    qDebug()<<"相应信号"<<"---------------------";
}

void MainWindow::newarea(int No,
                         QString Drivename,
                         QString nameDis1,
                         QString nameDis2,
                         QString nameDis3,
                         QString nameDis4,
                         qlonglong capacityDis1,
                         qlonglong capacityDis2,
                         qlonglong capacityDis3,
                         qlonglong capacityDis4,
                         QString pathDis1,
                         QString pathDis2,
                         QString pathDis3,
                         QString pathDis4,
                         int linestatus)
{
    open_widget = new QClickWidget(NULL,No,Drivename,nameDis1,nameDis2,nameDis3,nameDis4,
                                   capacityDis1,capacityDis2,capacityDis3,capacityDis4,
                                   pathDis1,pathDis2,pathDis3,pathDis4);
    connect(open_widget, &QClickWidget::clickedConvert, [=]{
        qDebug() << "==============debug";
    });

    QWidget *line = new QWidget;
    line->setFixedHeight(1);
    line->setObjectName("lineWidget");
    line->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    if (linestatus == 0)
    {
        this->vboxlayout->addWidget(line);
    }
    if (linestatus == 2)
    {
        this->vboxlayout->addWidget(line);
    }

    this->vboxlayout->addWidget(open_widget);
    //this->setGeometry(1,1,22,33);

//    QString str_capacity = size_human(capacity);
//    QClickWidget *open_widget;
//    open_widget = new QClickWidget (NULL,false,name,capacity,path);
//    //打开设备的界面布局 open the device's interface layout
//    QLabel *open_label;
//    //QString str_capacity = size_human(capacity);
//    QVBoxLayout *open_vboxlayout = new QVBoxLayout;
//    QHBoxLayout *open_hboxlayout = new QHBoxLayout;
//    QHBoxLayout *open_showcapacity_hboxlayout = new QHBoxLayout;
//    //add eject image
//    eject_image_button= new QPushButton();
//    QIcon eject_icon(":picture/media-eject-symbolic.svg");
//    eject_image_button->setIcon(eject_icon);
//    eject_image_button->setFlat(true);
//    // open_image_button->setDisabled(true);
//    QLabel *image_show_label = new QLabel();
//    QPixmap pixmap(":picture/drive-removable-media-usb.png");
//    image_show_label->setPixmap(pixmap);
//    QLabel *placeLabel = new QLabel();

//    if(name.isEmpty())
//    {
//        open_label = new QLabel("打开 "+str_capacity+"卷");
//    }
//    else
//    {
//        open_label = new QLabel(name);
//    }
//    open_label->setDisabled(true);
//    QVBoxLayout *imagepartionshow_vboxlayout = new QVBoxLayout();

//    QLabel *open_capacity = new QLabel(this);
//    qDebug()<<capacity<<"-----------------";
//    open_capacity->setText(str_capacity+" (磁盘容量)");
//    open_capacity->setAlignment(Qt::AlignCenter);
//    open_capacity->setDisabled(true);
//    //add layout
//    imagepartionshow_vboxlayout->addSpacing(8);
//    imagepartionshow_vboxlayout->addWidget(image_show_label);
//    open_hboxlayout->addLayout(imagepartionshow_vboxlayout);
//    open_hboxlayout->addStretch();
//    open_hboxlayout->addWidget(open_label);
//    open_hboxlayout->addStretch();
//    open_hboxlayout->addWidget(eject_image_button);
//    open_vboxlayout->addLayout(open_hboxlayout);
//    open_showcapacity_hboxlayout->addWidget(open_capacity);
//    open_vboxlayout->addLayout(open_showcapacity_hboxlayout);

//    //dividing line
//    QWidget *line = new QWidget;
//    line->setFixedHeight(1);
//    line->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

//    if (linestatus == 2)
//    {
//        this->vboxlayout->addWidget(line);
//    }

//    open_widget->setLayout(open_vboxlayout);
//    this->vboxlayout->addWidget(open_widget);

//    line->setStyleSheet(
//                "QWidget{"
//                "background-color: rgba(255,255,255,0.2);"
//                "width:276px;"
//                "height:1px;"
//                "}"
//                );
//    open_label->setStyleSheet(
//                //正常状态样式
//                "QLabel{"
//                "width:167px;"
//                "height:15px;"
//                "font-size:16px;"
//                "font-family:Microsoft YaHei;"
//                "font-weight:400;"
//                "color:rgba(255,255,255,1);"
//                "line-height:28px;"
//                "opacity:0.91;"
//                "}");
//    open_capacity->setStyleSheet(
//                "QLabel{"
//                "width:111px;"
//                "height:14px;"
//                "font-size:14px;"
//                "font-family:Microsoft YaHei;"
//                "font-weight:400;"
//                "color:rgba(255,255,255,0.35);"
//                "line-height:28px;"
//                "opacity:0.35;"
//                "}"
//                );
    open_widget->setStyleSheet(
                //正常状态样式
                "QClickWidget{"
                "height:79px;"
                "}"
                "QClickWidget:hover{"
                "background-color:rgba(255,255,255,30);"
                "}"
                );

}

QString MainWindow::size_human(qlonglong capacity)
{
//    float capacity = this->size();
    QStringList list;
    list << "KB" << "MB" << "GB" << "TB";

    QStringListIterator i(list);
    QString unit("bytes");

    while(capacity >= 1024.0 && i.hasNext())
    {
        unit = i.next();
        capacity /= 1024.0;
    }
    qDebug() << "capacity==============" << capacity << unit;
    QString str_capacity=QString(" %1%2").arg(capacity).arg(unit);
    return str_capacity;
 //   return QString().setNum(capacity,'f',2)+" "+unit;
}


void MainWindow::moveBottomRight()
{
////////////////////////////////////////get the loacation of the mouseQLabel *m_pathDis1_label;
    /*QPoint globalCursorPos = QCursor::pos();

    QRect availableGeometry = qApp->primaryScreen()->availableGeometry();
    QRect screenGeometry = qApp->primaryScreen()->geometry();

    QDesktopWidget* pDesktopWidget = QApplication::desktop();

    QRect screenRect = pDesktopWidget->screenGeometry();//area of the screen

    if (screenRect.height() != availableGeometry.height())
    {
        this->move(globalCursorPos.x()-125, availableGeometry.height() - this->height());
    }
    else
    {
        this->move(globalCursorPos.x()-125, availableGeometry.height() - this->height() - 40);
    }*/
    //////////////////////////////////////origin code base on the location of the mouse
    int screenNum = QGuiApplication::screens().count();
    int panelHeight = getPanelHeight("PanelHeight");
    int position =0;
    position = getPanelPosition("PanelPosion");
    int screen = 0;
    QRect rect;
    int localX ,availableWidth,totalWidth;
    int localY,availableHeight,totalHeight;

    qDebug() << "任务栏位置"<< position;
    if (screenNum > 1)
    {
        if (position == 3)                                  //on the right
        {
            screen = screenNum - 1;

            //Available screen width and height
            availableWidth =QGuiApplication::screens().at(screen)->geometry().x() +  QGuiApplication::screens().at(screen)->size().width()-panelHeight;
            availableHeight = QGuiApplication::screens().at(screen)->availableGeometry().height();

            //total width
            totalWidth =  QGuiApplication::screens().at(0)->size().width() + QGuiApplication::screens().at(screen)->size().width();
            totalHeight = QGuiApplication::screens().at(screen)->size().height();
        }
        else if(position  ==0 || position ==1)                  //above or bellow
        {
            availableHeight = QGuiApplication::screens().at(0)->size().height() - panelHeight;
            availableWidth = QGuiApplication::screens().at(0)->size().width();
            totalHeight = QGuiApplication::screens().at(0)->size().height();
            totalWidth = QGuiApplication::screens().at(0)->size().width();
        }
        else
        {
            availableHeight = QGuiApplication::screens().at(0)->availableGeometry().height();
            availableWidth = QGuiApplication::screens().at(0)->availableGeometry().width();
            totalHeight = QGuiApplication::screens().at(0)->size().height();
            totalWidth = QGuiApplication::screens().at(0)->size().width();
        }
    }

    else
    {
        availableHeight = QGuiApplication::screens().at(0)->availableGeometry().height();
        availableWidth = QGuiApplication::screens().at(0)->availableGeometry().width();
        totalHeight = QGuiApplication::screens().at(0)->size().height();
        totalWidth = QGuiApplication::screens().at(0)->size().width();
    }
    //show the location of the systemtray
    rect = m_systray->geometry();
    localX = rect.x() - (this->width()/2 - rect.size().width()/2) ;
    localY = availableHeight - this->height();

    //modify location
    if (position == 0)
    { //下
        if (availableWidth - rect.x() - rect.width()/2 < this->width() / 2)
            this->setGeometry(availableWidth-this->width(),availableHeight-this->height()-3,this->width(),this->height());
        else
            this->setGeometry(localX,availableHeight-this->height()-3,this->width(),this->height());
    }
    else if (position == 1)
    { //上
        if (availableWidth - rect.x() - rect.width()/2 < this->width() / 2)
            this->setGeometry(availableWidth-this->width(),totalHeight-availableHeight+3,this->width(),this->height());
        else
            this->setGeometry(localX,totalHeight-availableHeight+3,this->width(),this->height());
    }
    else if (position == 2)
    {
        if (availableHeight - rect.y() - rect.height()/2 > this->height() /2)
            this->setGeometry(panelHeight + 3,rect.y() + (rect.width() /2) -(this->height()/2) ,this->width(),this->height());
        else
            this->setGeometry(panelHeight+3,localY,this->width(),this->height());//左
    }
    else if (position == 3)
    {
        localX = availableWidth - this->width();
        if (availableHeight - rect.y() - rect.height()/2 > this->height() /2)
            this->setGeometry(availableWidth - this->width() -3,rect.y() + (rect.width() /2) -(this->height()/2),this->width(),this->height());
        else
            this->setGeometry(localX-3,localY,this->width(),this->height());
    }
}

/*
    use dbus to get the location of the panel
*/
int MainWindow::getPanelPosition(QString str)
{
    QDBusInterface interface( "com.ukui.panel.desktop",
                              "/",
                              "com.ukui.panel.desktop",
                              QDBusConnection::sessionBus() );
    QDBusReply<int> reply = interface.call("GetPanelPosition", str);

    return reply;
}

/*
    use the dbus to get the height of the panel
*/
int MainWindow::getPanelHeight(QString str)
{
    QDBusInterface interface( "com.ukui.panel.desktop",
                              "/",
                              "com.ukui.panel.desktop",
                              QDBusConnection::sessionBus() );
    QDBusReply<int> reply = interface.call("GetPanelSize", str);
    return reply;
}

