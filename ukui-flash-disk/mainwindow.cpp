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
#include "UnionVariable.h"
#include <PeonyVolumeManager>
#include <gio/gio.h>
#include <glib.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //框架的样式设置
    ui->centralWidget->setStyleSheet(
                "#centralWidget{"
                "width:280px;"
                "height:192px;"
                "background:rgba(19,19,20,0.95);"
                "border:1px rgba(255, 255, 255, 0.05);"
                "opacity:0.75;"

                "border-radius:1px;"
                "box-shadow:0px 2px 6px 0px rgba(0, 0, 0, 0.2);"
                "}"
                );
    vboxlayout = new QVBoxLayout();
    ui->centralWidget->setLayout(vboxlayout);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Popup);
    this->setWindowOpacity(0.95);
    this->resize( QSize( 280, 192 ));
    //initUi();
}

MainWindow::~MainWindow()
{
    delete ui;
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
            Q_FOREACH(auto cacheMount , *findList())
            {
                QString UDiskPath = g_file_get_path(g_mount_get_root(cacheMount->getGMount()));
                if (UDiskPath.contains("media") && g_mount_can_eject(cacheMount->getGMount()))
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
//                        QClickWidget *eject_widget;
//                        eject_widget = new QClickWidget(NULL,true,cacheMount->name(),total,UDiskPath);
                       //findList()->size()--;
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
    //打开设备的界面布局
    QLabel *open_label;
    //QString str_capacity = size_human(capacity);
    QVBoxLayout *open_vboxlayout = new QVBoxLayout;
    QHBoxLayout *open_hboxlayout = new QHBoxLayout;
    QHBoxLayout *open_showcapacity_hboxlayout = new QHBoxLayout;
    //添加弹出图标
    eject_image_button= new QPushButton();
    QIcon eject_icon("/usr/share/icons/ukui-icon-theme-default/22x22/emblems/emblem-unreadable.png");
    eject_image_button->setIcon(eject_icon);
    eject_image_button->setFlat(true);
    //    open_image_button->setDisabled(true);
    QLabel *image_show_label = new QLabel();
    QPixmap pixmap("/usr/share/icons/ukui-icon-theme-default/22x22/devices/drive-removable-media.png");
    image_show_label->setPixmap(pixmap);

    if(name.isEmpty())
    {
        open_label = new QLabel("打开 "+str_capacity+"卷");
    }
    else
    {
        open_label = new QLabel(name);
    }
    open_label->setDisabled(true);

    QLabel *open_capacity = new QLabel(this);
    qDebug()<<capacity<<"-----------------";
    open_capacity->setText(str_capacity+" (磁盘容量)");
    open_capacity->setAlignment(Qt::AlignCenter);
    open_capacity->setDisabled(true);
    //添加布局
    open_hboxlayout->addWidget(image_show_label);
    open_hboxlayout->addWidget(open_label);
    open_hboxlayout->addStretch();
    open_hboxlayout->addWidget(eject_image_button);
    open_vboxlayout->addLayout(open_hboxlayout);
    open_showcapacity_hboxlayout->addWidget(open_capacity);
    open_vboxlayout->addLayout(open_showcapacity_hboxlayout);

    //分割线
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
    //获取鼠标位置
    QPoint globalCursorPos = QCursor::pos();

    QRect availableGeometry = qApp->primaryScreen()->availableGeometry();
    QRect screenGeometry = qApp->primaryScreen()->geometry();

    QDesktopWidget* pDesktopWidget = QApplication::desktop();

    QRect screenRect = pDesktopWidget->screenGeometry();//屏幕区域

    if (screenRect.height() != availableGeometry.height())
    {
        this->move(globalCursorPos.x()-125, availableGeometry.height() - this->height());
    }
    else
    {
        this->move(globalCursorPos.x()-125, availableGeometry.height() - this->height() - 40);
    }
}

