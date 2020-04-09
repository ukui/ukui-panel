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

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
//    ejectInterface *ForEject = new ejectInterface(nullptr,"dgsgsdfg");
//    ForEject->show();


    ui->setupUi(this);
    //框架的样式设置
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


    iconSystray = QIcon::fromTheme("drive-removable-media");
    vboxlayout = new QVBoxLayout();
    ui->centralWidget->setLayout(vboxlayout);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Popup);
    this->setWindowOpacity(0.95);
    this->resize( QSize( 280, 192 ));
    m_systray = new QSystemTrayIcon;
    //w.show();
    //m_systray->setIcon(QIcon("/usr/share/icons/ukui-icon-theme-default/22x22/devices/drive-removable-media.png"));
    m_systray->setIcon(iconSystray);
    m_systray->setToolTip(tr("usb management tool"));
    connect(m_systray, &QSystemTrayIcon::activated, this, &MainWindow::iconActivated);
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
            qDebug()<<"wwj,wozhendeshi,wuyule";
            g_volume_mount(cacheVolume->getGVolume(),
                           G_MOUNT_MOUNT_NONE,
                           nullptr,
                           nullptr,
                           nullptr,
                           nullptr);
        }
    }

    //about the drives


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
        if(g_mount_can_eject(mount->getGMount()) || g_drive_can_stop(g_volume_get_drive(g_mount_get_volume(mount->getGMount()))) )
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
                findList()->removeOne(cachedMount);
                ejectInterface *ForEject = new ejectInterface(nullptr,cachedMount->name());
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
                ForEject->show();
                if(findList()->size() == 0)
                {
                    m_systray->hide();
                }
            }
        }
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
    qDebug()<<findList()->size()<<"rensaddedheng ru meng ";
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setsystemTray(QSystemTrayIcon *_systray)
{
    m_systray = _systray;
}

void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
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
            for (auto cacheMount : *findList())
            {
                QString UDiskPath = g_file_get_path(g_mount_get_root(cacheMount->getGMount()));
                if (UDiskPath.contains("media") && g_mount_can_eject(cacheMount->getGMount()) || g_drive_can_stop(g_volume_get_drive(g_mount_get_volume(cacheMount->getGMount()))))
                {
                    num++;
                    this->setFixedSize(250, 98*num);

                    QByteArray date = UDiskPath.toLocal8Bit();
                    char *p_Change = date.data();
                    GFile *file = g_file_new_for_path(p_Change);
                    GFileInfo *info = g_file_query_filesystem_info(file,"*",nullptr,nullptr);
                    quint64 total = g_file_info_get_attribute_uint64(info,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE);
                    if(num == 1)
                    {
                        qDebug() << "name: " << cacheMount->name();
                        newarea(cacheMount->name(), total, UDiskPath, 1);
                    }
                    else
                    {
                        newarea(cacheMount->name(), total, UDiskPath, 2);
                    }
                    connect(eject_image_button, &QPushButton::clicked,[=]()
                    {
                        g_mount_eject_with_operation(cacheMount->getGMount(),
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

                if(num != 0)
                {
                    this->showNormal();
                    moveBottomRight();
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

void MainWindow::newarea(QString name, qlonglong capacity, QString path, int linestatus)
{
    QString str_capacity = size_human(capacity);
    QClickWidget *open_widget;
    open_widget = new QClickWidget (NULL,false,name,capacity,path);
    //打开设备的界面布局 open the device's interface layout
    QLabel *open_label;
    //QString str_capacity = size_human(capacity);
    QVBoxLayout *open_vboxlayout = new QVBoxLayout;
    QHBoxLayout *open_hboxlayout = new QHBoxLayout;
    QHBoxLayout *open_showcapacity_hboxlayout = new QHBoxLayout;
    //add eject image
    eject_image_button= new QPushButton();
    QIcon eject_icon(":picture/media-eject-symbolic.svg");
    eject_image_button->setIcon(eject_icon);
    eject_image_button->setFlat(true);
    // open_image_button->setDisabled(true);
    QLabel *image_show_label = new QLabel();
    QPixmap pixmap(":picture/drive-removable-media-usb.png");
    image_show_label->setPixmap(pixmap);
    QLabel *placeLabel = new QLabel();

    if(name.isEmpty())
    {
        open_label = new QLabel("打开 "+str_capacity+"卷");
    }
    else
    {
        open_label = new QLabel(name);
    }
    open_label->setDisabled(true);
    QVBoxLayout *imagepartionshow_vboxlayout = new QVBoxLayout();

    QLabel *open_capacity = new QLabel(this);
    qDebug()<<capacity<<"-----------------";
    open_capacity->setText(str_capacity+" (磁盘容量)");
    open_capacity->setAlignment(Qt::AlignCenter);
    open_capacity->setDisabled(true);
    //add layout
    imagepartionshow_vboxlayout->addSpacing(8);
    imagepartionshow_vboxlayout->addWidget(image_show_label);
    open_hboxlayout->addLayout(imagepartionshow_vboxlayout);
    open_hboxlayout->addStretch();
    open_hboxlayout->addWidget(open_label);
    open_hboxlayout->addStretch();
    open_hboxlayout->addWidget(eject_image_button);
    open_vboxlayout->addLayout(open_hboxlayout);
    open_showcapacity_hboxlayout->addWidget(open_capacity);
    open_vboxlayout->addLayout(open_showcapacity_hboxlayout);

    //dividing line
    QWidget *line = new QWidget;
    line->setFixedHeight(1);
    line->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    if (linestatus == 2)
    {
        this->vboxlayout->addWidget(line);
    }

    open_widget->setLayout(open_vboxlayout);
    this->vboxlayout->addWidget(open_widget);

    line->setStyleSheet(
                "QWidget{"
                "background-color: rgba(255,255,255,0.2);"
                "width:280px;"
                "height:1px;"
                "}"
                );
    open_label->setStyleSheet(
                //正常状态样式
                "QLabel{"
                "width:167px;"
                "height:15px;"
                "font-size:16px;"
                "font-family:Microsoft YaHei;"
                "font-weight:400;"
                "color:rgba(255,255,255,1);"
                "line-height:28px;"
                "opacity:0.91;"
                "}");
    open_capacity->setStyleSheet(
                "QLabel{"
                "width:111px;"
                "height:14px;"
                "font-size:14px;"
                "font-family:Microsoft YaHei;"
                "font-weight:400;"
                "color:rgba(255,255,255,0.35);"
                "line-height:28px;"
                "opacity:0.35;"
                "}"
                );
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
////////////////////////////////////////get the loacation of the mouse
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

