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
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusMessage>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusObjectPath>
#include <QDBusReply>
#include <QSystemTrayIcon>
#include <QTimer>
#include <QColor>
#include <peony-qt/file-utils.h>

#include "clickLabel.h"
#include "MacroFile.h"

//typedef void(*GAsyncReadyCallback) (GDrive *source_object,GAsyncResult *res,gpointer user_data);

//void frobnitz_result_func_mount(GMount *source_object,GAsyncResult *res,MainWindow *p_this)
//{
//    gboolean success = FALSE;
//    GError *err = nullptr;

//    success = g_mount_unmount_with_operation_finish(source_object,res,&err);
//    if(success)
//    {
//        if(g_mount_can_unmount(source_object))
//        {
//            findGMountList()->removeOne(source_object);
//            p_this->m_eject = new ejectInterface(p_this,g_volume_get_name(g_mount_get_volume(source_object)),DATADEVICE);
//            p_this->m_eject->show();
//        }
//    }
//    else
//    {
//        p_this->m_eject = new ejectInterface(p_this,g_drive_get_name(g_volume_get_drive(g_mount_get_volume(source_object))),OCCUPYDEVICE);
//        p_this->m_eject->show();
//    }

//}

void frobnitz_result_func(GDrive *source_object,GAsyncResult *res,MainWindow *p_this)
{
    gboolean success =  FALSE;
    GError *err = nullptr;
    success = g_drive_eject_with_operation_finish (source_object, res, &err);

    if (!err)
    {
      findGDriveList()->removeOne(source_object);
      p_this->m_eject = new ejectInterface(p_this,g_drive_get_name(source_object),NORMALDEVICE);
      p_this->m_eject->show();
    }

    else /*if(g_drive_can_stop(source_object) == true)*/
    {
        int volumeNum = g_list_length(g_drive_get_volumes(source_object));

        for(int eachVolume = 0 ; eachVolume < volumeNum ;eachVolume++)
        {
            p_this->flagType = 0;

            if(g_mount_can_unmount(g_volume_get_mount((GVolume *)g_list_nth_data(g_drive_get_volumes(source_object),eachVolume))))
            {
                char *dataPath = g_file_get_path(g_mount_get_root(g_volume_get_mount((GVolume *)g_list_nth_data(g_drive_get_volumes(source_object),eachVolume))));
                QProcess p;
                p.setProgram("pkexec");
                p.setArguments(QStringList()<<"umount"<<QString(dataPath));
                p.start();
                p_this->ifSucess = p.waitForFinished();
                p_this->m_eject = new ejectInterface(p_this,g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(source_object),eachVolume)),DATADEVICE);
                p_this->m_eject->show();
                findGMountList()->removeOne(g_volume_get_mount((GVolume *)g_list_nth_data(g_drive_get_volumes(source_object),eachVolume)));
                findGDriveList()->removeOne(source_object);
            }
        }
    }
//    else
//    {    ()<<"oh no"<<err->message<<err->code;
//        qDebug()<<"howwohohwohow";
//        p_this->m_eject = new ejectInterface(p_this,g_drive_get_name(source_object),OCCUPYDEVICE);
//        p_this->m_eject->show();
//    }

}



void frobnitz_result_func_drive(GDrive *source_object,GAsyncResult *res,MainWindow *p_this)
{
    gboolean success =  FALSE;
    GError *err = nullptr;
    success = g_drive_start_finish (source_object, res, &err);
//    if(!err)
//    {
//        qDebug()<<"drive start sucess";
//    }
//    else
//    {
//        qDebug()<<"drive start failed"<<"-------------------"<<err->message<<err->code;
//    }
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    driveNoGparted = 0;
    ifGpartedHasStarted = 0;
//    const QByteArray idtrans(THEME_QT_TRANS);

//    if(QGSettings::isSchemaInstalled(idtrans))
//    {
//        m_transparency_gsettings = new QGSettings(idtrans);
//    }
    const QByteArray idd(THEME_QT_SCHEMA);

    if(QGSettings::isSchemaInstalled(idd))
    {
        qtSettings = new QGSettings(idd);
    }

    initThemeMode();

    installEventFilter(this);
    ui->setupUi(this);
    //框架的样式设置
    //set the style of the framework
    interfaceHideTime = new QTimer(this);
    initTransparentState();

    QPainterPath path;
    auto rect = this->rect();
    rect.adjust(0,0,-0,-0);
    path.addRoundedRect(rect, 6, 6);
    setProperty("blurRegion", QRegion(path.toFillPolygon().toPolygon()));
    this->setAttribute(Qt::WA_TranslucentBackground);//设置窗口背景透明
    ui->centralWidget->setObjectName("centralWidget");
    iconSystray = QIcon::fromTheme("media-removable-symbolic");
    vboxlayout = new QVBoxLayout();
    //hboxlayout = new QHBoxLayout();
#if (QT_VERSION < QT_VERSION_CHECK(5,7,0))
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::X11BypassWindowManagerHint);
#else
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Popup);
#endif
    m_systray = new QSystemTrayIcon ;
    m_systray->setIcon(iconSystray);
    m_systray->setVisible(true);
    m_systray->setToolTip(tr("usb management tool"));
    //init the srceen
    screen = qApp->primaryScreen();
    //underlying code to get the information of the usb device
    getDeviceInfo();
    connect(m_systray, &QSystemTrayIcon::activated, this, &MainWindow::iconActivated);
    connect(this,&MainWindow::convertShowWindow,this,&MainWindow::onConvertShowWindow);
    ui->centralWidget->setLayout(vboxlayout);

    //    QDBusConnection::sessionBus().connect(QString(),QString("/taskbar/click"), \
    //                                          "com.ukui.panel.plugins.taskbar","sendToUkuiDEApp",this, \
    //                                          SLOT(on_clickPanelToHideInterface));

    //    QDBusConnection::sessionBus().connect(QString(), QString("/taskbar/click"),
    //                                          "com.ukui.panel.plugins.taskbar", "sendToUkuiDEApp", this,
    //                                          SLOT(on_clickPanelToHideInterface));
    QDBusConnection::sessionBus().connect(QString(), QString("/taskbar/click"), \
                                                  "com.ukui.panel.plugins.taskbar", "sendToUkuiDEApp", this, SLOT(on_clickPanelToHideInterface()));

}

MainWindow::~MainWindow()
{
    delete ui;
//    delete open_widget;
}

void MainWindow::initThemeMode()
{
//    if(!qtSettings)
//    {
//        currentThemeMode = "ukui-white";
//    }
//    QStringList keys = qtSettings->keys();
//    if(keys.contains("styleName"))
//    {
//        currentThemeMode = qtSettings->get("style-name").toString();
//    }
    connect(qtSettings,&QGSettings::changed,this,[=](const QString &key)
    {
        auto style = qtSettings->get(key).toString();
        currentThemeMode = qtSettings->get(MODE_QT_KEY).toString();
    });
    currentThemeMode = qtSettings->get(MODE_QT_KEY).toString();
}

void MainWindow::on_clickPanelToHideInterface()
{
//    if(MainWindow::isShow == true)
//    {
//        this->hide();
//        MainWindow::isShow = false;
//    }
    if(!this->isHidden())
        this->hide();
}

void MainWindow::getDeviceInfo()
{
//callback function that to monitor the insertion and removal of the underlying equipment
    GVolumeMonitor *g_volume_monitor = g_volume_monitor_get();
    g_signal_connect (g_volume_monitor, "drive-connected", G_CALLBACK (drive_connected_callback), this);
    g_signal_connect (g_volume_monitor, "drive-disconnected", G_CALLBACK (drive_disconnected_callback), this);
    g_signal_connect (g_volume_monitor, "volume-added", G_CALLBACK (volume_added_callback), this);
    g_signal_connect (g_volume_monitor, "volume-removed", G_CALLBACK (volume_removed_callback), this);
    g_signal_connect (g_volume_monitor, "mount-added", G_CALLBACK (mount_added_callback), this);
    g_signal_connect (g_volume_monitor, "mount-removed", G_CALLBACK (mount_removed_callback), this);
//about drive
    GList *current_drive_list = g_volume_monitor_get_connected_drives(g_volume_monitor);
    while(current_drive_list)
    {
        GDrive *gdrive = (GDrive *)current_drive_list->data;
        GVolume *g_volume = (GVolume *)g_list_nth(g_drive_get_volumes(gdrive),0);
        char *devPath = g_volume_get_identifier(g_volume,G_VOLUME_IDENTIFIER_KIND_UNIX_DEVICE);
        if(g_drive_can_eject(gdrive) || g_drive_can_stop(gdrive))
        {
            if(g_volume_can_eject((GVolume *)g_list_nth_data(g_drive_get_volumes(gdrive),0)))
            {
                *findGDriveList()<<gdrive;
            }
        }
        current_drive_list = current_drive_list->next;
        //if(g_drive_can_eject)
    }
//about volume
    GList *current_volume_list = g_volume_monitor_get_volumes(g_volume_monitor);
    while(current_volume_list)
    {
        GVolume *gvolume = (GVolume *)current_volume_list->data;
        if(g_volume_can_eject(gvolume) || g_drive_can_eject(g_volume_get_drive(gvolume)))
        {
            *findGVolumeList()<<gvolume;
            g_volume_mount(gvolume,
                           G_MOUNT_MOUNT_NONE,
                           nullptr,
                           nullptr,
                           nullptr,
                           nullptr);
        }
        current_volume_list = current_volume_list->next;
    }
//about mount
    GList *current_mount_list = g_volume_monitor_get_mounts(g_volume_monitor);
    while(current_mount_list)
    {
        GMount *gmount = (GMount *)current_mount_list->data;
        if(g_mount_can_eject(gmount) || g_drive_can_eject(g_mount_get_drive(gmount)))
        {
            *findGMountList()<<gmount;
        }
        current_mount_list = current_mount_list->next;
    }

 //determine the systray icon should be showed  or be hieded
    if(findGMountList()->size() >= 1 || findGDriveList()->size() >= 1)
    {
        m_systray->show();
    }
    if(findGDriveList()->size() == 0 || findGMountList()->size() == 0)
    {
        m_systray->hide();
    }
}

void MainWindow::onConvertShowWindow()
{
    num = 0;
    MainWindowShow();
}

//the drive-connected callback function the is triggered when the usb device is inseted

void MainWindow::drive_connected_callback(GVolumeMonitor *monitor, GDrive *drive, MainWindow *p_this)
{
    qDebug()<<"drive add";
    *findGDriveList()<<drive;
    if(findGDriveList()->size() >= 1)
    {
        p_this->m_systray->show();
    }

    p_this->triggerType = 0;
}

//the drive-disconnected callback function the is triggered when the usb device is pull out
void MainWindow::drive_disconnected_callback (GVolumeMonitor *monitor, GDrive *drive, MainWindow *p_this)
{
    qDebug()<<"drive disconnect";
    p_this->ifGpartedHasStarted = 0;
    p_this->driveNoGparted = 1;
    char *drive_name = g_drive_get_name(drive);
    findGDriveList()->removeOne(drive);
    p_this->hide();
    if(findGDriveList()->size() == 0)
    {
        p_this->m_systray->hide();
    }
}

//when the usb device is identified,we should mount every partition

void MainWindow::volume_added_callback(GVolumeMonitor *monitor, GVolume *volume, MainWindow *p_this)
{
    qDebug()<<"volume add";
    g_volume_mount(volume,
                   G_MOUNT_MOUNT_NONE,
                   nullptr,
                   nullptr,
                   GAsyncReadyCallback(frobnitz_result_func_volume),
                   p_this);
    //if the deveice is CD
    p_this->root = g_mount_get_default_location(g_volume_get_mount(volume));
    p_this->mount_uri = g_file_get_uri(p_this->root);
    qDebug()<<"mount uri"<<p_this->mount_uri;
    if(p_this->mount_uri)
    {
        qDebug()<<"mount uir is not null";
        if(strcmp(p_this->mount_uri,"burn:///") == 0 || strcmp(p_this->mount_uri,"cdda://sr0/") == 0 || strcmp(p_this->mount_uri,"file:///"))
        {
            if(!findGDriveList()->contains(g_volume_get_drive(volume)))
            {
                *findGDriveList()<<g_volume_get_drive(volume);
                qDebug()<<"drive name"<<g_drive_get_name(g_volume_get_drive(volume));
            }
            qDebug()<<"mount_uri"<<p_this->mount_uri;
        }
    }
    else
    {
        char *devPath = g_volume_get_identifier(volume,G_VOLUME_IDENTIFIER_KIND_UNIX_DEVICE);   //detective the kind of movable device
        qDebug()<<"devPath"<<devPath;
        if(g_str_has_prefix(devPath,"/dev/sr"))
        {
//            if(findGDriveList()->contains(g_volume_get_drive(volume)))
//            {
                *findGDriveList()<<g_volume_get_drive(volume);
//            }
        }

//        if(g_str_has_prefix(devPath,"/dev/sd"))
//        {
//            *findGMountList()<<g_volume_get_mount(volume);
//        }

    }
    g_object_unref(p_this->root);
    g_free(p_this->mount_uri);
//    g_object_unref(volume);
    p_this->ifGpartedHasStarted = 0;
    qDebug()<<"volume add final";
}


//when the U disk is pull out we should reduce all its partitions
void MainWindow::volume_removed_callback(GVolumeMonitor *monitor, GVolume *volume, MainWindow *p_this)
{
    qDebug()<<"volume removed";
    p_this->ifGpartedHasStarted = 0;
    qDebug()<<"ifGpartedStarted"<<p_this->ifGpartedHasStarted;
    qDebug()<<"g_drive_get_volumes"<<g_list_length(g_drive_get_volumes(g_volume_get_drive(volume)));
    if(g_list_length(g_drive_get_volumes(g_volume_get_drive(volume))) == 0)
    {
        findGDriveList()->removeOne(g_volume_get_drive(volume));
    }
    findGVolumeList()->removeOne(volume);
    connect(p_this,&MainWindow::ejectDriveSignal,p_this,[=](){
        p_this->driveNoGparted = 1;
    });
    if(p_this->driveNoGparted != 1)
    {
        if(findGVolumeList()->size() < findGMountList()->size() )
        {
            p_this->ifGpartedHasStarted = 1;
            qDebug()<<"findGMontList"<<findGMountList()->size()<<"findGVolumeList"<<findGVolumeList()->size();
        }
    }

//    if(findGVolumeList()->size() >= 0)
//    {
//        p_this->m_systray->show();
//    }
    p_this->root = g_mount_get_default_location(g_volume_get_mount(volume));
    p_this->mount_uri = g_file_get_uri(p_this->root);
    if(p_this->mount_uri)
    {
        if(strcmp(p_this->mount_uri,"burn:///") == 0 || strcmp(p_this->mount_uri,"cdda://sr0/")==0)
        {
//            *findGDriveList()<<g_volume_get_drive(volume);
            findGDriveList()->removeOne(g_volume_get_drive(volume));
        }
    }
    else
    {
        char *devPath = g_volume_get_identifier(volume,G_VOLUME_IDENTIFIER_KIND_UNIX_DEVICE);   //detective the kind of movable device
        if(g_str_has_prefix(devPath,"/dev/sr"))
        {
            findGDriveList()->removeOne(g_volume_get_drive(volume));
        }
    }
    g_object_unref(p_this->root);
    g_free(p_this->mount_uri);
}

//when the volumes were mounted we add its mounts number
void MainWindow::mount_added_callback(GVolumeMonitor *monitor, GMount *mount, MainWindow *p_this)
{
    qDebug()<<"mount add";
    char *devPath = g_volume_get_identifier(g_mount_get_volume(mount),G_VOLUME_IDENTIFIER_KIND_UNIX_DEVICE);
    qDebug()<<"dev_path"<<devPath;
    if(g_mount_can_eject(mount) || g_volume_can_eject(g_mount_get_volume(mount))|| g_drive_can_eject(g_mount_get_drive(mount))
            || g_str_has_prefix(devPath,"/dev/sdb")||g_str_has_prefix(devPath,"/dev/sdc") || g_str_has_prefix(devPath,"/dev/sdd")
            ||g_str_has_prefix(devPath,"/dev/sde") && !g_str_has_prefix(devPath,"/dev/sda") || g_str_has_prefix(devPath,"/dev/sr"))
    {
        *findGMountList()<<mount;
    }
    else
    {
        qDebug()<<"不符合过滤条件的设备已被挂载";
    }
//    if(findGMountList())
//    *findGMountList()<<mount;
    if(findGMountList()->size() >= 1)
    {
        p_this->m_systray->show();
    }
}

//when the mountes were uninstalled we reduce mounts number
void MainWindow::mount_removed_callback(GVolumeMonitor *monitor, GMount *mount, MainWindow *p_this)
{
    qDebug()<<mount<<"mount remove";
    findGMountList()->removeOne(mount);
    p_this->driveMountNum = 0;

    for(int i = 0; i<g_list_length(g_drive_get_volumes(g_mount_get_drive(mount)));i++)
    {
        if(g_volume_get_mount((GVolume *)g_list_nth(g_drive_get_volumes(g_mount_get_drive(mount)),i)) == NULL)
        {
            p_this->driveMountNum += 1;
        }
    }
    if(findGMountList()->size() == 0)
    {
        p_this->m_systray->hide();
    }

//     p_this->unloadMount();
}

//it stands that when you insert a usb device when all the  U disk partitions
void MainWindow::frobnitz_result_func_volume(GVolume *source_object,GAsyncResult *res,MainWindow *p_this)
{

    gboolean success =  FALSE;
    GError *err = nullptr;
    success = g_volume_mount_finish (source_object, res, &err);
    if(!err)
    {
        *findGVolumeList()<<source_object;
        p_this->num++;
        if (p_this->num >= g_list_length(g_drive_get_volumes(g_volume_get_drive(source_object))))
        {
            Q_EMIT p_this->convertShowWindow();     //emit a signal to trigger the MainMainShow slot
        }
    }
    else
    {
//        g_volume_mount(source_object,
//                       G_MOUNT_MOUNT_NONE,
//                       nullptr,
//                       nullptr,
//                       GAsyncReadyCallback(frobnitz_result_func_volume),
//                       nullptr);
        qDebug()<<"sorry mount failed";
    }
    p_this->ifGpartedHasStarted = 0;
}


//here we begin painting the main interface
void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    driveNoGparted = 0;
    if(ifGpartedHasStarted == 0)
    {
//    qDebug()<<m_transparency;
//    QPalette palette = this->palette();
//    QColor color = QColor(Qt::red);
//    color.setAlphaF(m_transparency/100);
//    palette.setBrush(QPalette::Window, color);
//    this->setPalette(palette);
//    this->setAutoFillBackground(true);
//    this->setWindowOpacity(m_transparency/100);
    this->getTransparentData();
    qDebug()<<"findGMountList.size"<<findGMountList()->size();
    qDebug()<<"findGVolumeList.size"<<findGVolumeList()->size();

    qDebug()<<"findGDriveList.size"<<findGDriveList()->size();
    qDebug()<<"m_transparency"<<m_transparency;
    QString strTrans;
    strTrans =  QString::number(m_transparency, 10, 2);
#if (QT_VERSION < QT_VERSION_CHECK(5,7,0))
    QString convertStyle = "#centralWidget{background:rgba(19,19,20,0.95);}";
#else
//    QString convertStyle = "#centralWidget{background:rgba(19,19,20," + strTrans + ");}";
#endif
//    this->setStyleSheet(convertStyle);
//    if(this->interfaceHideTime != NULL)
//    {
//        delete this->interfaceHideTime;
//    }
    //int hign = 200;
    triggerType = 1;  //It represents how we open the interface
    if(ui->centralWidget != NULL)
    {
        //cancel the connection between the timeout signal and main interface hiding
        disconnect(interfaceHideTime, SIGNAL(timeout()), this, SLOT(on_Maininterface_hide()));
    }
    num = 0;
    if ( this->vboxlayout != NULL )
    {
        QLayoutItem* item;
        while ((item = this->vboxlayout->takeAt(0)) != NULL)
        {
            delete item->widget();
            delete item;
        }
    }
    if(this->open_widget != NULL)
    {
    }
    switch (reason)
    {
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::Context:
      if (this->isHidden())
      {
          //Convenient interface layout for all drives

          for(auto cacheDrive : *findGDriveList())
          {
              int singleSignal = 0;
              int cdSignal = 0;
              listVolumes = g_drive_get_volumes(cacheDrive);
              for(vList = listVolumes; vList != NULL; vList = vList->next)
              {
                  volume = (GVolume *)vList->data;
                  if(g_volume_get_mount(volume) != NULL)
                  {
                      root = g_mount_get_default_location(g_volume_get_mount(volume));
                      mount_uri = g_file_get_uri(root);
                      qDebug()<<"mount_uri"<<mount_uri;
                      if(g_str_has_prefix(mount_uri,"file:///"))
                      {
                          singleSignal += 1;
                          qDebug()<<"normal device is has been loaded";
                      }


                      if(g_str_has_prefix(mount_uri,"burn:///") || g_str_has_prefix(mount_uri,"cdda://"))
                      cdSignal += 1;
                      qDebug()<<"Important:mount uri"<<mount_uri;
                      g_object_unref(volume);
                      g_object_unref(root);
                      g_free(mount_uri);
                  }
              }
//          for(auto cacheDrive : *findGDriveList())
//          {

//              qDebug()<<"findGDriveList().num"<<findGDriveList()->size();
//              qDebug()<<"findGMountList().num"<<findGMountList()->size();

//              int singleSignal = 0;
//              int cdSignal = 0;
//              listVolumes = g_drive_get_volumes(cacheDrive);
//              for(vList = listVolumes; vList != NULL; vList = vList->next)
//              {
//                  volume = (GVolume *)vList->data;
//                  if(g_volume_get_mount(volume) != NULL)
//                  {
//                      root = g_mount_get_default_location(g_volume_get_mount(volume));
//                      mount_uri = g_file_get_uri(root);
//                      if(g_str_has_prefix(mount_uri,"file:///"))
//                      singleSignal += 1;

//                      if(g_str_has_prefix(mount_uri,"burn:///")||g_str_has_prefix(mount_uri,"cdda://"))
//                      cdSignal += 1;
//                      qDebug()<<"Important:mount uri"<<mount_uri;
//                      g_object_unref(volume);
//                      g_object_unref(root);
//                      g_free(mount_uri);
//                  }
//              }
              g_list_free(listVolumes);
              hign = findGMountList()->size()*40 + findGDriveList()->size()*55;
              this->setFixedSize(280,hign);


              if(cdSignal)
              {
                  switch(g_list_length(g_drive_get_volumes(cacheDrive)))
                  {
                      case 1:
                      num++;
                      //when the drive's volume number is 1
                      /*determine whether the drive is only one and whether if the drive is the fisrst one,
                       *if the answer is yes,we set the last parameter is 1.*/
                      /*if(findGDriveList()->size() == 1)
                      {
                          newarea(1,cacheDrive,g_drive_get_name(cacheDrive),
                                  g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0)),
                                  NULL,NULL,NULL, 1,NULL,NULL,NULL, "burn:///",NULL,NULL,NULL,1);
                      }
                      else*/ if(num == 1)
                      {
                          newarea(1,cacheDrive,g_drive_get_name(cacheDrive),
                                  g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0)),
                                  NULL,NULL,NULL, 1,NULL,NULL,NULL, "burn:///",NULL,NULL,NULL,1);
                      }
                      else
                      {
                          newarea(1,cacheDrive,g_drive_get_name(cacheDrive),
                                  g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0)),
                                  NULL,NULL,NULL,1,NULL,NULL,NULL, "burn:///",NULL,NULL,NULL, 2);
                      }
                      break;
                      case 2:
                      num++;
                      /*if(findGDriveList()->size() == 1)
                      {
                          newarea(2,cacheDrive,g_drive_get_name(cacheDrive),
                                  g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0)),
                                  g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),1)),
                                  NULL,NULL, 1,1,NULL,NULL, "burn:///","burn:///",NULL,NULL,1);
                      }
                      else */if(num == 1)
                      {
                          newarea(2,cacheDrive,g_drive_get_name(cacheDrive),
                                  g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0)),
                                  g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),1)),
                                  NULL,NULL, 1,1,NULL,NULL, "burn:///","burn:///",NULL,NULL,1);
                      }
                      else
                      {
                          newarea(2,cacheDrive,g_drive_get_name(cacheDrive),
                                  g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0)),
                                  g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),1)),
                                  NULL,NULL,1,1,NULL,NULL, "burn:///","burn:///",NULL,NULL, 2);
                      }
                      break;
                      default:
                      num++;
                      /*if(findGDriveList()->size() == 1)
                      {
                          newarea(1,cacheDrive,g_drive_get_name(cacheDrive),
                                  g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0)),
                                  NULL,NULL,NULL, 1,NULL,NULL,NULL, "burn:///",NULL,NULL,NULL,1);
                      }
                      else */if(num == 1)
                      {
                          newarea(1,cacheDrive,g_drive_get_name(cacheDrive),
                                  g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0)),
                                  NULL,NULL,NULL, 1,NULL,NULL,NULL, "burn:///",NULL,NULL,NULL,1);
                      }
                      else
                      {
                          newarea(1,cacheDrive,g_drive_get_name(cacheDrive),
                                  g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0)),
                                  NULL,NULL,NULL,1,NULL,NULL,NULL, "burn:///",NULL,NULL,NULL, 2);
                      }
                  }
              }
              else
              {
              GList *volumeNumber = g_drive_get_volumes(cacheDrive);
              int DisNum = g_list_length(volumeNumber);
              driveMountNum = 0;

              if(singleSignal !=0 )
              {
              if (DisNum >0)
              {
                if (g_drive_can_eject(cacheDrive) || g_drive_can_stop(cacheDrive))
                {
                    /*
                     * to get the U disk partition's path,name,capacity and U disk's name,then we layout by the
                     * number of its volume
                     * */

                    //when the drive's volume number is 1
                    hign = findGMountList()->size()*40 + findGDriveList()->size()*55;
                    if(DisNum == 1)
                    {
                       num++; 
                       char *driveName = g_drive_get_name(cacheDrive);
                       GVolume *element = (GVolume *)g_list_nth_data(volumeNumber,0);
                       char *volumeName = g_volume_get_name(element);
                       QString apiName = QString(volumeName);
                       char *deviceName = g_volume_get_identifier(element,G_VOLUME_IDENTIFIER_KIND_UNIX_DEVICE);
                       if(deviceName){
                           QString unixDeviceName = QString(deviceName);
                           Peony::FileUtils::handleVolumeLabelForFat32(apiName,unixDeviceName);
                           g_free(deviceName);
                       }
                       GFile *fileRoot = g_mount_get_root(g_volume_get_mount(element));
                       UDiskPathDis1 = g_file_get_path(fileRoot);
                       GFile *file = g_file_new_for_path(UDiskPathDis1);
                       GFileInfo *info = g_file_query_filesystem_info(file,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE,nullptr,nullptr);
                       totalDis1 = g_file_info_get_attribute_uint64(info,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE);
                       //when the drive's volume number is 1
                       /*determine whether the drive is only one and whether if the drive is the fisrst one,
                        *if the answer is yes,we set the last parameter is 1.*/
                       if(num == 1)
                       {
                           newarea(DisNum,cacheDrive,driveName,
                                   apiName,
                                   NULL,NULL,NULL, totalDis1,NULL,NULL,NULL, QString(UDiskPathDis1),NULL,NULL,NULL,1);
                       }
                       else
                       {
                           newarea(DisNum,cacheDrive,driveName,
                                   volumeName,
                                   NULL,NULL,NULL, totalDis1,NULL,NULL,NULL, QString(UDiskPathDis1),NULL,NULL,NULL,2);
                       }

                       g_free(driveName);
                       g_free(volumeName);
                       g_object_unref(element);
                       g_free(UDiskPathDis1);
                       g_object_unref(file);
                    }
                    //when the drive's volume number is 2
                    if(DisNum == 2)
                    {
                        num++;
                        char *driveName = g_drive_get_name(cacheDrive);
                        GVolume *element1 = (GVolume *)g_list_nth_data(volumeNumber,0);
                        char *volumeName1 = g_volume_get_name(element1);
                        QString apiName1 = QString(volumeName1);
                        char *deviceName1 = g_volume_get_identifier(element1,G_VOLUME_IDENTIFIER_KIND_UNIX_DEVICE);
                        if(deviceName1){
                            QString unixDeviceName = QString(deviceName1);
                            Peony::FileUtils::handleVolumeLabelForFat32(apiName1,unixDeviceName);
                            g_free(deviceName1);
                        }
                        GFile *fileRoot1 = g_mount_get_root(g_volume_get_mount(element1));
                        UDiskPathDis1 = g_file_get_path(fileRoot1);
                        GFile *file1 = g_file_new_for_path(UDiskPathDis1);
                        GFileInfo *info1 = g_file_query_filesystem_info(file1,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE,nullptr,nullptr);
                        totalDis1 = g_file_info_get_attribute_uint64(info1,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE);

                        GVolume *element2 = (GVolume *)g_list_nth_data(volumeNumber,1);
                        char *volumeName2 = g_volume_get_name(element2);
                        QString apiName2 = QString(volumeName2);
                        char *deviceName2 = g_volume_get_identifier(element2,G_VOLUME_IDENTIFIER_KIND_UNIX_DEVICE);
                        if(deviceName2){
                            QString unixDeviceName = QString(deviceName2);
                            Peony::FileUtils::handleVolumeLabelForFat32(apiName2,unixDeviceName);
                            g_free(deviceName2);
                        }
                        GFile *fileRoot2 = g_mount_get_root(g_volume_get_mount(element2));
                        UDiskPathDis2 = g_file_get_path(fileRoot2);
                        GFile *file2 = g_file_new_for_path(UDiskPathDis2);
                        GFileInfo *info2 = g_file_query_filesystem_info(file2,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE,nullptr,nullptr);
                        totalDis2 = g_file_info_get_attribute_uint64(info2,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE);
                        //when the drive's volume number is 1
                        /*determine whether the drive is only one and whether if the drive is the fisrst one,
                         *if the answer is yes,we set the last parameter is 1.*/
                        if(num == 1)
                        {

                            newarea(DisNum,cacheDrive,driveName,
                                    apiName1,
                                    apiName2,
                                    NULL,NULL, totalDis1,totalDis2,NULL,NULL, QString(UDiskPathDis1),QString(UDiskPathDis2),NULL,NULL,1);
                        }
                        else
                        {
                            newarea(DisNum,cacheDrive,driveName,
                                    apiName1,
                                    apiName2,
                                    NULL,NULL, totalDis1,totalDis2,NULL,NULL, QString(UDiskPathDis1),QString(UDiskPathDis2),NULL,NULL,2);
                        }

                        g_free(driveName);
                        g_free(volumeName1);
                        g_free(volumeName2);
                        g_object_unref(element1);
                        g_object_unref(element2);
                        g_free(UDiskPathDis1);
                        g_free(UDiskPathDis2);
                        g_object_unref(file1);
                        g_object_unref(file2);
                    }
                    //when the drive's volume number is 3
                    if(DisNum == 3)
                    {
                        num++;
                        UDiskPathDis1 = g_file_get_path(g_mount_get_root(g_volume_get_mount((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0))));
//                        QByteArray dateDis1 = UDiskPathDis1.toLocal8Bit();
//                        char *p_ChangeDis1 = dateDis1.data();
                        GFile *fileDis1 = g_file_new_for_path(UDiskPathDis1);
                        GFileInfo *infoDis1 = g_file_query_filesystem_info(fileDis1,"*",nullptr,nullptr);
                        totalDis1 = g_file_info_get_attribute_uint64(infoDis1,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE);

                        UDiskPathDis2 = g_file_get_path(g_mount_get_root(g_volume_get_mount((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),1))));
//                        QByteArray dateDis2 = UDiskPathDis2.toLocal8Bit();
//                        char *p_ChangeDis2 = dateDis2.data();
                        GFile *fileDis2 = g_file_new_for_path(UDiskPathDis2);
                        GFileInfo *infoDis2 = g_file_query_filesystem_info(fileDis2,"*",nullptr,nullptr);
                        totalDis2 = g_file_info_get_attribute_uint64(infoDis2,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE);

                        UDiskPathDis3 = g_file_get_path(g_mount_get_root(g_volume_get_mount((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),2))));
//                        QByteArray dateDis3 = UDiskPathDis3.toLocal8Bit();
//                        char *p_ChangeDis3 = dateDis3.data();
                        GFile *fileDis3 = g_file_new_for_path(UDiskPathDis3);
                        GFileInfo *infoDis3 = g_file_query_filesystem_info(fileDis3,"*",nullptr,nullptr);
                        totalDis3 = g_file_info_get_attribute_uint64(infoDis3,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE);
                        if(findGDriveList()->size() == 1)
                        {
                            newarea(DisNum,cacheDrive,g_drive_get_name(cacheDrive),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0)),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),1)),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),2)),
                                    NULL, totalDis1,totalDis2,totalDis3,NULL, QString(UDiskPathDis1),QString(UDiskPathDis2),QString(UDiskPathDis3),NULL,1);
                        }

                        else if(num == 1)
                        {
                            newarea(DisNum,cacheDrive,g_drive_get_name(cacheDrive),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0)),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),1)),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),2)),
                                    NULL, totalDis1,totalDis2,totalDis3,NULL, QString(UDiskPathDis1),QString(UDiskPathDis2),QString(UDiskPathDis3),NULL,1);
                        }

                        else
                        {
                            newarea(DisNum,cacheDrive,g_drive_get_name(cacheDrive),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0)),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),1)),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),2)),
                                    NULL, totalDis1,totalDis2,totalDis3,NULL, QString(UDiskPathDis1),QString(UDiskPathDis2),QString(UDiskPathDis3),NULL,2);
                        }

                    }
                    //when the drive's volume number is 4
                    if(DisNum == 4)
                    {
                        num++;
                        UDiskPathDis1 = g_file_get_path(g_mount_get_root(g_volume_get_mount((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0))));
                        GFile *fileDis1 = g_file_new_for_path(UDiskPathDis1);
                        GFileInfo *infoDis1 = g_file_query_filesystem_info(fileDis1,"*",nullptr,nullptr);
                        totalDis1 = g_file_info_get_attribute_uint64(infoDis1,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE);

                        UDiskPathDis2 = g_file_get_path(g_mount_get_root(g_volume_get_mount((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),1))));
                        GFile *fileDis2 = g_file_new_for_path(UDiskPathDis2);
                        GFileInfo *infoDis2 = g_file_query_filesystem_info(fileDis2,"*",nullptr,nullptr);
                        totalDis2 = g_file_info_get_attribute_uint64(infoDis2,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE);

                        UDiskPathDis3 = g_file_get_path(g_mount_get_root(g_volume_get_mount((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),2))));
                        GFile *fileDis3 = g_file_new_for_path(UDiskPathDis3);
                        GFileInfo *infoDis3 = g_file_query_filesystem_info(fileDis3,"*",nullptr,nullptr);
                        totalDis3 = g_file_info_get_attribute_uint64(infoDis3,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE);

                        UDiskPathDis4 = g_file_get_path(g_mount_get_root(g_volume_get_mount((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),3))));
                        GFile *fileDis4 = g_file_new_for_path(UDiskPathDis4);
                        GFileInfo *infoDis4 = g_file_query_filesystem_info(fileDis4,"*",nullptr,nullptr);
                        totalDis4 = g_file_info_get_attribute_uint64(infoDis4,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE);

                        if(findGDriveList()->size() == 1)
                        {
                            newarea(DisNum,cacheDrive,g_drive_get_name(cacheDrive),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0)),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),1)),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),2)),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),3)),
                                    totalDis1,totalDis2,totalDis3,totalDis4, QString(UDiskPathDis1),QString(UDiskPathDis2),QString(UDiskPathDis3),QString(UDiskPathDis4),1);
                        }

                        else if(num == 1)
                        {
                            newarea(DisNum,cacheDrive,g_drive_get_name(cacheDrive),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0)),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),1)),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),2)),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),3)),
                                    totalDis1,totalDis2,totalDis3,totalDis4, UDiskPathDis1,UDiskPathDis2,UDiskPathDis3,UDiskPathDis4,1);
                        }

                        else
                        {
                            newarea(DisNum,cacheDrive,g_drive_get_name(cacheDrive),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0)),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),1)),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),2)),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),3)),
                                    totalDis1,totalDis2,totalDis3,totalDis4, UDiskPathDis1,UDiskPathDis2,UDiskPathDis3,UDiskPathDis4,2);
                        }

                    }
                }
              }
              }
              }



                if(findGDriveList()->size() != 0)
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
    ui->centralWidget->show();
    }
    else
    {
        qDebug()<<"ifGparted---"<<ifGpartedHasStarted;
        m_eject = new ejectInterface(this,NULL,GPARTEDINTERFACE);
        m_eject->show();
    }

}

void MainWindow::hideEvent(QHideEvent event)
{
    delete open_widget;
}


/*
 * newarea use all the information of the U disk to paint the main interface and add line
*/
void MainWindow::newarea(int No,
                         GDrive *Drive,
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
    open_widget = new QClickWidget(this,No,Drive,Drivename,nameDis1,nameDis2,nameDis3,nameDis4,
                                   capacityDis1,capacityDis2,capacityDis3,capacityDis4,
                                   pathDis1,pathDis2,pathDis3,pathDis4);
    connect(open_widget,&QClickWidget::clickedConvert,this,[=]()
    {
        this->hide();
        Q_EMIT this->ejectDriveSignal();
        ifGpartedHasStarted = 0;
        driveNoGparted = 1;
        qDebug()<<"drive has been disconnected";
    });


    line = new QWidget;
    line->setFixedHeight(1);
    line->setObjectName("lineWidget");
    if(currentThemeMode == "ukui-dark" || currentThemeMode == "ukui-black" || currentThemeMode == "ukui-default")
    {
        line->setStyleSheet("background-color:rgba(255,255,255,0.2);");
    }
    else
    {
        line->setStyleSheet("background-color:rgba(0,0,0,0.2);");
    }

    //when the drive is only or the drive is the first one,we make linestatus become  1
    line->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    line->setFixedSize(276,1);
    if (linestatus == 2)
    {
        this->vboxlayout->addWidget(line);
    }

    this->vboxlayout->addWidget(open_widget);
    vboxlayout->setContentsMargins(2,8,2,8);

    if (linestatus == 0)
    {
        this->vboxlayout->addWidget(line);
    }

//    open_widget->setStyleSheet(
//                //正常状态样式
//                "QClickWidget{"
//                "height:79px;"
//                "}"
//                "QClickWidget:hover{"
//                "background-color:rgba(255,255,255,30);"
//                "}"
//                );

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
//    int screenNum = QGuiApplication::screens().count();
//    qDebug()<<"ScreenNum:"<<"-------------"<<screenNum;
//    int panelHeight = getPanelHeight("PanelHeight");
//    int position =0;
//    position = getPanelPosition("PanelPosion");
//    int screen = 0;
//    QRect rect;
//    int localX ,availableWidth,totalWidth;
//    int localY,availableHeight,totalHeight;

//    qDebug() << "任务栏位置"<< position;
//    if (screenNum > 1)
//    {
//        if (position == rightPosition)                                  //on the right
//        {
//            screen = screenNum - 1;

//            //Available screen width and height
//            availableWidth =QGuiApplication::screens().at(screen)->geometry().x() +  QGuiApplication::screens().at(screen)->size().width()-panelHeight;
//            qDebug()<<QGuiApplication::screens().at(screen)->geometry().x()<<"QGuiApplication::screens().at(screen)->geometry().x()";
//            qDebug()<<QGuiApplication::screens().at(screen)->size().width()<<"QGuiApplication::screens().at(screen)->size().width()";
//            availableHeight = QGuiApplication::screens().at(screen)->availableGeometry().height();

//            //total width
//            totalWidth =  QGuiApplication::screens().at(0)->size().width() + QGuiApplication::screens().at(screen)->size().width();
//            totalHeight = QGuiApplication::screens().at(screen)->size().height();
//            this->move(totalWidth,totalHeight);
//        }
//        else if(position  ==downPosition || upPosition ==1)                  //above or bellow
//        {
//            availableHeight = QGuiApplication::screens().at(0)->size().height() - panelHeight;
//            availableWidth = QGuiApplication::screens().at(0)->size().width();
//            totalHeight = QGuiApplication::screens().at(0)->size().height();
//            totalWidth = QGuiApplication::screens().at(0)->size().width();
//        }
//        else
//        {
//            availableHeight = QGuiApplication::screens().at(0)->availableGeometry().height();
//            availableWidth = QGuiApplication::screens().at(0)->availableGeometry().width();
//            totalHeight = QGuiApplication::screens().at(0)->size().height();
//            totalWidth = QGuiApplication::screens().at(0)->size().width();
//        }
//    }

//    else
//    {
//        availableHeight = QGuiApplication::screens().at(0)->availableGeometry().height();
//        availableWidth = QGuiApplication::screens().at(0)->availableGeometry().width();
//        totalHeight = QGuiApplication::screens().at(0)->size().height();
//        totalWidth = QGuiApplication::screens().at(0)->size().width();

//        //show the location of the systemtray
//        rect = m_systray->geometry();
//        localX = rect.x() - (this->width()/2 - rect.size().width()/2) ;
//        localY = availableHeight - this->height();

//        //modify location
//        if (position == downPosition)
//        { //下
//            if (availableWidth - rect.x() - rect.width()/2 < this->width() / 2)
//                this->setGeometry(availableWidth-this->width(),availableHeight-this->height()-DistanceToPanel,this->width(),this->height());
//            else
//                this->setGeometry(localX,availableHeight-this->height()-DistanceToPanel,this->width(),this->height());
//        }
//        else if (position == upPosition)
//        { //上
//            if (availableWidth - rect.x() - rect.width()/2 < this->width() / 2)
//                this->setGeometry(availableWidth-this->width(),totalHeight-availableHeight+DistanceToPanel,this->width(),this->height());
//            else
//                this->setGeometry(localX,totalHeight-availableHeight+DistanceToPanel,this->width(),this->height());
//        }
//        else if (position == leftPosition)
//        {
//            if (availableHeight - rect.y() - rect.height()/2 > this->height() /2)
//                this->setGeometry(panelHeight + DistanceToPanel,rect.y() + (rect.width() /2) -(this->height()/2) ,this->width(),this->height());
//            else
//                this->setGeometry(panelHeight+DistanceToPanel,localY,this->width(),this->height());//左
//        }
//        else if (position == rightPosition)
//        {
//            localX = availableWidth - this->width();
//            if (availableHeight - rect.y() - rect.height()/2 > this->height() /2)
//                this->setGeometry(availableWidth - this->width() -DistanceToPanel,rect.y() + (rect.width() /2) -(this->height()/2),this->width(),this->height());
//            else
//                this->setGeometry(localX-DistanceToPanel,localY,this->width(),this->height());
//        }
//    }
    QRect availableGeometry = qApp->primaryScreen()->availableGeometry();
    QRect screenGeometry = qApp->primaryScreen()->geometry();

    QDesktopWidget* desktopWidget = QApplication::desktop();
    QRect deskMainRect = desktopWidget->availableGeometry(0);//获取可用桌面大小
    QRect screenMainRect = desktopWidget->screenGeometry(0);//获取设备屏幕大小
    QRect deskDupRect = desktopWidget->availableGeometry(1);//获取可用桌面大小
    QRect screenDupRect = desktopWidget->screenGeometry(1);//获取设备屏幕大小

    int PanelPosition = getPanelPosition("position");     //n
    int PanelHeight = getPanelHeight("height");                   //m
//    int d = 2; //窗口边沿到任务栏距离

    if (screenGeometry.width() == availableGeometry.width() && screenGeometry.height() == availableGeometry.height()) {
        if (PanelPosition == 0) {
            //任务栏在下侧
            this->move(availableGeometry.x() + availableGeometry.width() - this->width(), screenMainRect.y() + availableGeometry.height() - this->height() - PanelHeight - DistanceToPanel);
        } else if(PanelPosition == 1) {
            //任务栏在上侧
            this->move(availableGeometry.x() + availableGeometry.width() - this->width(), screenMainRect.y() + screenGeometry.height() - availableGeometry.height() + PanelHeight + DistanceToPanel);
        } else if (PanelPosition == 2) {
            //任务栏在左侧
            if (screenGeometry.x() == 0) {//主屏在左侧
                this->move(PanelHeight + DistanceToPanel, screenMainRect.y() + screenMainRect.height() - this->height());
            } else {//主屏在右侧
                this->move(screenMainRect.x() + PanelHeight + DistanceToPanel, screenMainRect.y() + screenMainRect.height() - this->height());
            }
        } else if (PanelPosition == 3) {
            //任务栏在右侧
            if (screenGeometry.x() == 0) {//主屏在左侧
                this->move(screenMainRect.width() - this->width() - PanelHeight - DistanceToPanel, screenMainRect.y() + screenMainRect.height() - this->height());
            } else {//主屏在右侧
                this->move(screenMainRect.x() + screenMainRect.width() - this->width() - PanelHeight - DistanceToPanel, screenMainRect.y() + screenMainRect.height() - this->height());
            }
        }
    } else if(screenGeometry.width() == availableGeometry.width() ) {
        if (m_systray->geometry().y() > availableGeometry.height()/2) {
            //任务栏在下侧
            this->move(availableGeometry.x() + availableGeometry.width() - this->width(), screenMainRect.y() + availableGeometry.height() - this->height() - DistanceToPanel);
        } else {
            //任务栏在上侧
            this->move(availableGeometry.x() + availableGeometry.width() - this->width(), screenMainRect.y() + screenGeometry.height() - availableGeometry.height() + DistanceToPanel);
        }
    } else if (screenGeometry.height() == availableGeometry.height()) {
        if (m_systray->geometry().x() > availableGeometry.width()/2) {
            //任务栏在右侧
            this->move(availableGeometry.x() + availableGeometry.width() - this->width() - DistanceToPanel, screenMainRect.y() + screenGeometry.height() - this->height());
        } else {
            //任务栏在左侧
            this->move(screenGeometry.width() - availableGeometry.width() + DistanceToPanel, screenMainRect.y() + screenGeometry.height() - this->height());
        }
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


void MainWindow::resizeEvent(QResizeEvent *event)
{
}

void MainWindow::moveBottomDirect(GDrive *drive)
{
//    ejectInterface *ForEject = new ejectInterface(nullptr,g_drive_get_name(drive));
//    int screenNum = QGuiApplication::screens().count();
//    int panelHeight = getPanelHeight("PanelHeight");
//    int position =0;
//    position = getPanelPosition("PanelPosion");
//    int screen = 0;
//    QRect rect;
//    int localX ,availableWidth,totalWidth;
//    int localY,availableHeight,totalHeight;

//    qDebug() << "任务栏位置"<< position;
//    if (screenNum > 1)
//    {
//        if (position == rightPosition)                                  //on the right
//        {
//            screen = screenNum - 1;

//            //Available screen width and height
//            availableWidth =QGuiApplication::screens().at(screen)->geometry().x() +  QGuiApplication::screens().at(screen)->size().width()-panelHeight;
//            availableHeight = QGuiApplication::screens().at(screen)->availableGeometry().height();

//            //total width
//            totalWidth =  QGuiApplication::screens().at(0)->size().width() + QGuiApplication::screens().at(screen)->size().width();
//            totalHeight = QGuiApplication::screens().at(screen)->size().height();
//        }
//        else if(position  ==downPosition || position ==upPosition)                  //above or bellow
//        {
//            availableHeight = QGuiApplication::screens().at(0)->size().height() - panelHeight;
//            availableWidth = QGuiApplication::screens().at(0)->size().width();
//            totalHeight = QGuiApplication::screens().at(0)->size().height();
//            totalWidth = QGuiApplication::screens().at(0)->size().width();
//        }
//        else
//        {
//            availableHeight = QGuiApplication::screens().at(0)->availableGeometry().height();
//            availableWidth = QGuiApplication::screens().at(0)->availableGeometry().width();
//            totalHeight = QGuiApplication::screens().at(0)->size().height();
//            totalWidth = QGuiApplication::screens().at(0)->size().width();
//        }
//    }

//    else
//    {
//        availableHeight = QGuiApplication::screens().at(0)->availableGeometry().height();
//        availableWidth = QGuiApplication::screens().at(0)->availableGeometry().width();
//        totalHeight = QGuiApplication::screens().at(0)->size().height();
//        totalWidth = QGuiApplication::screens().at(0)->size().width();
//    }
//    //show the location of the systemtray
//    rect = m_systray->geometry();
//    localX = rect.x() - (ForEject->width()/2 - rect.size().width()/2) ;
//    localY = availableHeight - ForEject->height();
//    //modify location
//    if (position == downPosition)
//    { //下
//        if (availableWidth - rect.x() - rect.width()/2 < ForEject->width() / 2)
//            ForEject->setGeometry(availableWidth-ForEject->width(),availableHeight-ForEject->height()-DistanceToPanel,ForEject->width(),ForEject->height());
//        else
//            ForEject->setGeometry(localX-16,availableHeight-ForEject->height()-DistanceToPanel,ForEject->width(),ForEject->height());
//    }
//    else if (position == upPosition)
//    { //上
//        if (availableWidth - rect.x() - rect.width()/2 < ForEject->width() / 2)
//            ForEject->setGeometry(availableWidth-ForEject->width(),totalHeight-availableHeight+DistanceToPanel,ForEject->width(),ForEject->height());
//        else
//            ForEject->setGeometry(localX-16,totalHeight-availableHeight+DistanceToPanel,ForEject->width(),ForEject->height());
//    }
//    else if (position == leftPosition)
//    {
//        if (availableHeight - rect.y() - rect.height()/2 > ForEject->height() /2)
//            ForEject->setGeometry(panelHeight + DistanceToPanel,rect.y() + (rect.width() /2) -(ForEject->height()/2) ,ForEject->width(),ForEject->height());
//        else
//            ForEject->setGeometry(panelHeight+DistanceToPanel,localY,ForEject->width(),ForEject->height());//左
//    }
//    else if (position == rightPosition)
//    {
//        localX = availableWidth - ForEject->width();
//        if (availableHeight - rect.y() - rect.height()/2 > ForEject->height() /2)
//        {
//            ForEject->setGeometry(availableWidth - ForEject->width() -DistanceToPanel,rect.y() + (rect.height() /2) -(ForEject->height()/2),ForEject->width(),ForEject->height());
//        }
//        else
//            ForEject->setGeometry(localX-DistanceToPanel,localY,ForEject->width(),ForEject->height());
//    }
//    ForEject->show();
}

void MainWindow::MainWindowShow()
{
    qDebug()<<"findGDriveList.size"<<findGDriveList()->size();
    qDebug()<<"findGMountList.size"<<findGMountList()->size();
    ui->centralWidget->setObjectName("centralWidget");
//    ui->centralWidget->setStyleSheet(
//                "#centralWidget{"
//                "width:280px;"
//                "height:192px;"
//                "border:1px solid rgba(255, 255, 255, 0.05);"
//                "opacity:0.75;"

//                "border-radius:6px;"
//                "box-shadow:0px 2px 6px 0px rgba(0, 0, 0, 0.2);"
////                "margin:0px;"
////                "border-width:0px;"
////                "padding:0px;"
//                "}"
//                );
//    m_transparency = this->getTransparentData();

//    QString strTrans;
//    strTrans =  QString::number(m_transparency, 10, 2);
//    qDebug()<<"strTrans---"<<strTrans;
    QString convertStyle = "#centralWidget{background:rgba(19,19,20,0.70);}";
//    this->setStyleSheet(convertStyle);

    num = 0;
    if  ( this->vboxlayout != NULL )
    {
        QLayoutItem* item;
        while ((item = this->vboxlayout->takeAt(0)) != NULL)
        {
            delete item->widget();
            delete item;
        }
    }
//      if (this->isHidden())
//      {

          //MainWindow::hign = MainWindow::oneVolumeDriveNum*98+MainWindow::twoVolumeDriveNum*110+MainWindow::threeVolumeDriveNum*130+MainWindow::fourVolumeDriveNum*160;
          for(auto cacheDrive : *findGDriveList())
          {

              qDebug()<<"findGDriveList().num"<<findGDriveList()->size();
              qDebug()<<"findGMountList().num"<<findGMountList()->size();

              int singleSignal = 0;
              int cdSignal = 0;
              listVolumes = g_drive_get_volumes(cacheDrive);
              for(vList = listVolumes; vList != NULL; vList = vList->next)
              {
                  volume = (GVolume *)vList->data;
                  if(g_volume_get_mount(volume) != NULL)
                  {
                      root = g_mount_get_default_location(g_volume_get_mount(volume));
                      mount_uri = g_file_get_uri(root);
                      if(g_str_has_prefix(mount_uri,"file:///"))
                      singleSignal += 1;

                      if(g_str_has_prefix(mount_uri,"burn:///")||g_str_has_prefix(mount_uri,"cdda://"))
                      cdSignal += 1;
                      qDebug()<<"Important:mount uri"<<mount_uri;
                      g_object_unref(volume);
                      g_object_unref(root);
                      g_free(mount_uri);
                  }
              }
              g_list_free(listVolumes);

              hign = findGVolumeList()->size()*40 + findGDriveList()->size()*55;
              this->setFixedSize(280,hign);

              if(cdSignal)
              {
                  switch(g_list_length(g_drive_get_volumes(cacheDrive)))
                  {
                      case 1:
                      num++;
                      //when the drive's volume number is 1
                      /*determine whether the drive is only one and whether if the drive is the fisrst one,
                       *if the answer is yes,we set the last parameter is 1.*/
                      /*if(findGDriveList()->size() == 1)
                      {
                          newarea(1,cacheDrive,g_drive_get_name(cacheDrive),
                                  g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0)),
                                  NULL,NULL,NULL, 1,NULL,NULL,NULL, "burn:///",NULL,NULL,NULL,1);
                      }
                      else*/ if(num == 1)
                      {
                          newarea(1,cacheDrive,g_drive_get_name(cacheDrive),
                                  g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0)),
                                  NULL,NULL,NULL, 1,NULL,NULL,NULL, "burn:///",NULL,NULL,NULL,1);
                      }
                      else
                      {
                          newarea(1,cacheDrive,g_drive_get_name(cacheDrive),
                                  g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0)),
                                  NULL,NULL,NULL,1,NULL,NULL,NULL, "burn:///",NULL,NULL,NULL, 2);
                      }
                      break;
                      case 2:
                      num++;
                      /*if(findGDriveList()->size() == 1)
                      {
                          newarea(2,cacheDrive,g_drive_get_name(cacheDrive),
                                  g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0)),
                                  g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),1)),
                                  NULL,NULL, 1,1,NULL,NULL, "burn:///","burn:///",NULL,NULL,1);
                      }
                      else */if(num == 1)
                      {
                          newarea(2,cacheDrive,g_drive_get_name(cacheDrive),
                                  g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0)),
                                  g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),1)),
                                  NULL,NULL, 1,1,NULL,NULL, "burn:///","burn:///",NULL,NULL,1);
                      }
                      else
                      {
                          newarea(2,cacheDrive,g_drive_get_name(cacheDrive),
                                  g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0)),
                                  g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),1)),
                                  NULL,NULL,1,1,NULL,NULL, "burn:///","burn:///",NULL,NULL, 2);
                      }
                      break;
                      default:
                      num++;
                      /*if(findGDriveList()->size() == 1)
                      {
                          newarea(1,cacheDrive,g_drive_get_name(cacheDrive),
                                  g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0)),
                                  NULL,NULL,NULL, 1,NULL,NULL,NULL, "burn:///",NULL,NULL,NULL,1);
                      }
                      else */if(num == 1)
                      {
                          newarea(1,cacheDrive,g_drive_get_name(cacheDrive),
                                  g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0)),
                                  NULL,NULL,NULL, 1,NULL,NULL,NULL, "burn:///",NULL,NULL,NULL,1);
                      }
                      else
                      {
                          newarea(1,cacheDrive,g_drive_get_name(cacheDrive),
                                  g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0)),
                                  NULL,NULL,NULL,1,NULL,NULL,NULL, "burn:///",NULL,NULL,NULL, 2);
                      }
                  }
              }
              else
              {
              GList *volumeNumber = g_drive_get_volumes(cacheDrive);
              int DisNum = g_list_length(volumeNumber);
              driveMountNum = 0;
              if(singleSignal !=0 )
              {
              if (DisNum >0)
              {
                if (g_drive_can_eject(cacheDrive) || g_drive_can_stop(cacheDrive))
                {
                    /*
                     * to get the U disk partition's path,name,capacity and U disk's name,then we layout by the
                     * number of its volume
                     * */

                    //when the drive's volume number is 1
                    hign = findGMountList()->size()*40 + findGDriveList()->size()*55;
                    if(DisNum == 1)
                    {
                       num++;
                       char *driveName = g_drive_get_name(cacheDrive);
                       GVolume *element = (GVolume *)g_list_nth_data(volumeNumber,0);
                       char *volumeName = g_volume_get_name(element);
                       QString apiName = QString(volumeName);
                       char *deviceName = g_volume_get_identifier(element,G_VOLUME_IDENTIFIER_KIND_UNIX_DEVICE);
                       if(deviceName){
                           QString unixDeviceName = QString(deviceName);
                           Peony::FileUtils::handleVolumeLabelForFat32(apiName,unixDeviceName);
                           g_free(deviceName);
                       }
                       GFile *fileRoot = g_mount_get_root(g_volume_get_mount(element));
                       UDiskPathDis1 = g_file_get_path(fileRoot);
                       GFile *file = g_file_new_for_path(UDiskPathDis1);
                       GFileInfo *info = g_file_query_filesystem_info(file,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE,nullptr,nullptr);
                       totalDis1 = g_file_info_get_attribute_uint64(info,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE);
                       //when the drive's volume number is 1
                       /*determine whether the drive is only one and whether if the drive is the fisrst one,
                        *if the answer is yes,we set the last parameter is 1.*/
                       if(num == 1)
                       {
                           newarea(DisNum,cacheDrive,driveName,
                                   apiName,
                                   NULL,NULL,NULL, totalDis1,NULL,NULL,NULL, QString(UDiskPathDis1),NULL,NULL,NULL,1);
                       }
//                       else if(findGDriveList()->size() == 1)
//                       {
//                           newarea(DisNum,driveName,
//                                   volumeName,
//                                   NULL,NULL,NULL, totalDis1,NULL,NULL,NULL, QString(UDiskPathDis1),NULL,NULL,NULL,1);
//                       }
                       else
                       {
                           newarea(DisNum,cacheDrive,driveName,
                                   apiName,
                                   NULL,NULL,NULL, totalDis1,NULL,NULL,NULL, QString(UDiskPathDis1),NULL,NULL,NULL,2);
                       }

                       g_free(driveName);
                       g_free(volumeName);
                       g_object_unref(element);
                       g_free(UDiskPathDis1);
                       g_object_unref(file);
                    }
                    //when the drive's volume number is 2
                    if(DisNum == 2)
                    {
                        num++;
                        char *driveName = g_drive_get_name(cacheDrive);
                        GVolume *element1 = (GVolume *)g_list_nth_data(volumeNumber,0);
                        char *volumeName1 = g_volume_get_name(element1);
                        QString apiName1 = QString(volumeName1);
                        char *deviceName1 = g_volume_get_identifier(element1,G_VOLUME_IDENTIFIER_KIND_UNIX_DEVICE);
                        if(deviceName1){
                            QString unixDeviceName1 = QString(deviceName1);
                            Peony::FileUtils::handleVolumeLabelForFat32(apiName1,unixDeviceName1);
                            g_free(deviceName1);
                        }
                        GFile *fileRoot1 = g_mount_get_root(g_volume_get_mount(element1));
                        UDiskPathDis1 = g_file_get_path(fileRoot1);
                        GFile *file1 = g_file_new_for_path(UDiskPathDis1);
                        GFileInfo *info1 = g_file_query_filesystem_info(file1,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE,nullptr,nullptr);
                        totalDis1 = g_file_info_get_attribute_uint64(info1,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE);

                        GVolume *element2 = (GVolume *)g_list_nth_data(volumeNumber,1);
                        char *volumeName2 = g_volume_get_name(element2);
                        QString apiName2 = QString(volumeName2);
                        char *deviceName2 = g_volume_get_identifier(element2,G_VOLUME_IDENTIFIER_KIND_UNIX_DEVICE);
                        if(deviceName2){
                            QString unixDeviceName = QString(deviceName2);
                            Peony::FileUtils::handleVolumeLabelForFat32(apiName2,unixDeviceName);
                            g_free(deviceName2);
                        }
                        GFile *fileRoot2 = g_mount_get_root(g_volume_get_mount(element2));
                        UDiskPathDis2 = g_file_get_path(fileRoot2);
                        GFile *file2 = g_file_new_for_path(UDiskPathDis2);
                        GFileInfo *info2 = g_file_query_filesystem_info(file2,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE,nullptr,nullptr);
                        totalDis2 = g_file_info_get_attribute_uint64(info2,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE);
                        //when the drive's volume number is 1
                        /*determine whether the drive is only one and whether if the drive is the fisrst one,
                         *if the answer is yes,we set the last parameter is 1.*/
                        if(num == 1)
                        {

                            newarea(DisNum,cacheDrive,driveName,
                                    apiName1,
                                    apiName2,
                                    NULL,NULL, totalDis1,totalDis2,NULL,NULL, QString(UDiskPathDis1),QString(UDiskPathDis2),NULL,NULL,1);
                        }
                        else
                        {
                            newarea(DisNum,cacheDrive,driveName,
                                    apiName1,
                                    apiName2,
                                    NULL,NULL, totalDis1,totalDis2,NULL,NULL, QString(UDiskPathDis1),QString(UDiskPathDis2),NULL,NULL,2);
                        }

                        g_free(driveName);
                        g_free(volumeName1);
                        g_free(volumeName2);
                        g_object_unref(element1);
                        g_object_unref(element2);
                        g_free(UDiskPathDis1);
                        g_free(UDiskPathDis2);
                        g_object_unref(file1);
                        g_object_unref(file2);
                    }
                    //when the drive's volume number is 3
                    if(DisNum == 3)
                    {
                        num++;
                        UDiskPathDis1 = g_file_get_path(g_mount_get_root(g_volume_get_mount((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0))));
//                        QByteArray dateDis1 = UDiskPathDis1.toLocal8Bit();
//                        char *p_ChangeDis1 = dateDis1.data();
                        GFile *fileDis1 = g_file_new_for_path(UDiskPathDis1);
                        GFileInfo *infoDis1 = g_file_query_filesystem_info(fileDis1,"*",nullptr,nullptr);
                        totalDis1 = g_file_info_get_attribute_uint64(infoDis1,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE);

                        UDiskPathDis2 = g_file_get_path(g_mount_get_root(g_volume_get_mount((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),1))));
//                        QByteArray dateDis2 = UDiskPathDis2.toLocal8Bit();
//                        char *p_ChangeDis2 = dateDis2.data();
                        GFile *fileDis2 = g_file_new_for_path(UDiskPathDis2);
                        GFileInfo *infoDis2 = g_file_query_filesystem_info(fileDis2,"*",nullptr,nullptr);
                        totalDis2 = g_file_info_get_attribute_uint64(infoDis2,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE);

                        UDiskPathDis3 = g_file_get_path(g_mount_get_root(g_volume_get_mount((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),2))));
//                        QByteArray dateDis3 = UDiskPathDis3.toLocal8Bit();
//                        char *p_ChangeDis3 = dateDis3.data();
                        GFile *fileDis3 = g_file_new_for_path(UDiskPathDis3);
                        GFileInfo *infoDis3 = g_file_query_filesystem_info(fileDis3,"*",nullptr,nullptr);
                        totalDis3 = g_file_info_get_attribute_uint64(infoDis3,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE);
                        if(findGDriveList()->size() == 1)
                        {
                            newarea(DisNum,cacheDrive,g_drive_get_name(cacheDrive),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0)),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),1)),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),2)),
                                    NULL, totalDis1,totalDis2,totalDis3,NULL, QString(UDiskPathDis1),QString(UDiskPathDis2),QString(UDiskPathDis3),NULL,1);
                        }

                        else if(num == 1)
                        {
                            newarea(DisNum,cacheDrive,g_drive_get_name(cacheDrive),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0)),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),1)),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),2)),
                                    NULL, totalDis1,totalDis2,totalDis3,NULL, QString(UDiskPathDis1),QString(UDiskPathDis2),QString(UDiskPathDis3),NULL,1);
                        }

                        else
                        {
                            newarea(DisNum,cacheDrive,g_drive_get_name(cacheDrive),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0)),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),1)),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),2)),
                                    NULL, totalDis1,totalDis2,totalDis3,NULL, QString(UDiskPathDis1),QString(UDiskPathDis2),QString(UDiskPathDis3),NULL,2);
                        }

                    }
                    //when the drive's volume number is 4
                    if(DisNum == 4)
                    {
                        num++;
                        UDiskPathDis1 = g_file_get_path(g_mount_get_root(g_volume_get_mount((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0))));
//                        QByteArray dateDis1 = UDiskPathDis1.toLocal8Bit();
//                        char *p_ChangeDis1 = dateDis1.data();
                        GFile *fileDis1 = g_file_new_for_path(UDiskPathDis1);
                        GFileInfo *infoDis1 = g_file_query_filesystem_info(fileDis1,"*",nullptr,nullptr);
                        totalDis1 = g_file_info_get_attribute_uint64(infoDis1,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE);

                        UDiskPathDis2 = g_file_get_path(g_mount_get_root(g_volume_get_mount((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),1))));
//                        QByteArray dateDis2 = UDiskPathDis2.toLocal8Bit();
//                        char *p_ChangeDis2 = dateDis2.data();
                        GFile *fileDis2 = g_file_new_for_path(UDiskPathDis2);
                        GFileInfo *infoDis2 = g_file_query_filesystem_info(fileDis2,"*",nullptr,nullptr);
                        totalDis2 = g_file_info_get_attribute_uint64(infoDis2,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE);

                        UDiskPathDis3 = g_file_get_path(g_mount_get_root(g_volume_get_mount((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),2))));
//                        QByteArray dateDis3 = UDiskPathDis3.toLocal8Bit();
//                        char *p_ChangeDis3 = dateDis3.data();
                        GFile *fileDis3 = g_file_new_for_path(UDiskPathDis3);
                        GFileInfo *infoDis3 = g_file_query_filesystem_info(fileDis3,"*",nullptr,nullptr);
                        totalDis3 = g_file_info_get_attribute_uint64(infoDis3,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE);

                        UDiskPathDis4 = g_file_get_path(g_mount_get_root(g_volume_get_mount((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),3))));
//                        QByteArray dateDis4 = UDiskPathDis4.toLocal8Bit();
//                        char *p_ChangeDis4 = dateDis4.data();
                        GFile *fileDis4 = g_file_new_for_path(UDiskPathDis4);
                        GFileInfo *infoDis4 = g_file_query_filesystem_info(fileDis4,"*",nullptr,nullptr);
                        totalDis4 = g_file_info_get_attribute_uint64(infoDis4,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE);

                        if(findGDriveList()->size() == 1)
                        {
                            newarea(DisNum,cacheDrive,g_drive_get_name(cacheDrive),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0)),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),1)),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),2)),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),3)),
                                    totalDis1,totalDis2,totalDis3,totalDis4, QString(UDiskPathDis1),QString(UDiskPathDis2),QString(UDiskPathDis3),QString(UDiskPathDis4),1);
                        }

                        else if(num == 1)
                        {
                            newarea(DisNum,cacheDrive,g_drive_get_name(cacheDrive),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0)),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),1)),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),2)),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),3)),
                                    totalDis1,totalDis2,totalDis3,totalDis4, UDiskPathDis1,UDiskPathDis2,UDiskPathDis3,UDiskPathDis4,1);
                        }

                        else
                        {
                            newarea(DisNum,cacheDrive,g_drive_get_name(cacheDrive),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0)),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),1)),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),2)),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),3)),
                                    totalDis1,totalDis2,totalDis3,totalDis4, UDiskPathDis1,UDiskPathDis2,UDiskPathDis3,UDiskPathDis4,2);
                        }

                    }
                }
              }
              }
              }


                if(findGDriveList()->size() != 0)
                {
                    this->showNormal();
                    moveBottomNoBase();
                }

            }
          ui->centralWidget->show();
          interfaceHideTime->setTimerType(Qt::PreciseTimer);
          if(ui->centralWidget != NULL)
          {
              connect(interfaceHideTime, SIGNAL(timeout()), this, SLOT(on_Maininterface_hide()));
          }
          interfaceHideTime->start(2000);

      }


void MainWindow::on_Maininterface_hide()
{
    this->hide();
    interfaceHideTime->stop();
}

void MainWindow::moveBottomNoBase()
{
//    screen->availableGeometry();
//    screen->availableSize();
//    if(screen->availableGeometry().x() == screen->availableGeometry().y() && screen->availableSize().height() < screen->size().height())
//    {
//        qDebug()<<"the positon of panel is down";
//        this->move(screen->availableGeometry().x() + screen->size().width() -
//                   this->width() - DistanceToPanel,screen->availableGeometry().y() +
//                   screen->availableSize().height() - this->height() - DistanceToPanel);
//    }

//    if(screen->availableGeometry().x() < screen->availableGeometry().y() && screen->availableSize().height() < screen->size().height())
//    {
//        qDebug()<<"this position of panel is up";
//        this->move(screen->availableGeometry().x() + screen->size().width() -
//                   this->width() - DistanceToPanel,screen->availableGeometry().y());
//    }

//    if(screen->availableGeometry().x() > screen->availableGeometry().y() && screen->availableSize().width() < screen->size().width())
//    {
//        qDebug()<<"this position of panel is left";
//        this->move(screen->availableGeometry().x() + DistanceToPanel,screen->availableGeometry().y()
//                   + screen->availableSize().height() - this->height());
//    }

//    if(screen->availableGeometry().x() == screen->availableGeometry().y() && screen->availableSize().width() < screen->size().width())
//    {
//        qDebug()<<"this position of panel is right";
//        this->move(screen->availableGeometry().x() + screen->availableSize().width() -
//                   DistanceToPanel - this->width(),screen->availableGeometry().y() +
//                   screen->availableSize().height() - (this->height())*(DistanceToPanel - 1));
//    }
    int position=0;
    int panelSize=0;
    if(QGSettings::isSchemaInstalled(QString("org.ukui.panel.settings").toLocal8Bit()))
    {
        QGSettings* gsetting=new QGSettings(QString("org.ukui.panel.settings").toLocal8Bit());
        if(gsetting->keys().contains(QString("panelposition")))
            position=gsetting->get("panelposition").toInt();
        else
            position=0;
        if(gsetting->keys().contains(QString("panelsize")))
            panelSize=gsetting->get("panelsize").toInt();
        else
            panelSize=SmallPanelSize;
    }
    else
    {
        position=0;
        panelSize=SmallPanelSize;
    }

    int x=QApplication::primaryScreen()->geometry().x();
    int y=QApplication::primaryScreen()->geometry().y();

    if(position==0)
        this->setGeometry(QRect(x + QApplication::primaryScreen()->geometry().width()-this->width(),y+QApplication::primaryScreen()->geometry().height()-panelSize-this->height(),this->width(),this->height()));
    else if(position==1)
        this->setGeometry(QRect(x + QApplication::primaryScreen()->geometry().width()-this->width(),y+panelSize,this->width(),this->height()));  // Style::minw,Style::minh the width and the height of the interface  which you want to show
    else if(position==2)
        this->setGeometry(QRect(x+panelSize,y + QApplication::primaryScreen()->geometry().height() - this->height(),this->width(),this->height()));
    else
        this->setGeometry(QRect(x+QApplication::primaryScreen()->geometry().width()-panelSize-this->width(),y + QApplication::primaryScreen()->geometry().height() - this->height(),this->width(),this->height()));
}

/*
 * determine how to open the maininterface,if trigger is 0,the main interface show when we inset USB
 * device directly,if trigger is 1,we show main interface by clicking the systray icon.
*/
bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if(triggerType == 0)
    {
        if(event->type() == QEvent::Enter)
        {
            disconnect(interfaceHideTime, SIGNAL(timeout()), this, SLOT(on_Maininterface_hide()));
            this->show();
        }

        if(event->type() == QEvent::Leave)
        {
            connect(interfaceHideTime, SIGNAL(timeout()), this, SLOT(on_Maininterface_hide()));
            interfaceHideTime->start(2000);
        }
    }

    if(triggerType == 1){}

    if (obj == this)
    {
        if (event->type() == QEvent::WindowDeactivate && !(this->isHidden()))
        {
            this->hide();
            return true;
        }
//        else if (event->type() == QEvent::ActionChanged)
//        {
//            if (this->isHidden())
//            {
//                MainWindow::isShow = false;
//            }
//            else
//            {
//                MainWindow::isShow = true;
//            }
//        }
    }
    if (!isActiveWindow())
    {
        activateWindow();
    }
    return false;
}



//new a gsettings object to get the information of the opacity of the window
void MainWindow::initTransparentState()
{
//    if (QGSettings::isSchemaInstalled("org.ukui.control-center.personalise")) {
//        m_transparency_gsettings = new QGSettings("org.ukui.control-center.personalise");
//    }
    const QByteArray idtrans(THEME_QT_TRANS);

    if(QGSettings::isSchemaInstalled(idtrans))
    {
        m_transparency_gsettings = new QGSettings(idtrans);
    }
}

//use gsettings to get the opacity
void MainWindow::getTransparentData()
{
    if (!m_transparency_gsettings)
    {
       m_transparency = 0.95;
       return;
    }

    QStringList keys = m_transparency_gsettings->keys();
    if (keys.contains("transparency"))
    { 
        m_transparency = m_transparency_gsettings->get("transparency").toDouble();
    }
}


void MainWindow::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    QRect rect = this->rect();
    p.setRenderHint(QPainter::Antialiasing);  // 反锯齿;
    p.setBrush(opt.palette.color(QPalette::Base));
    p.setOpacity(m_transparency);
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(rect, 6, 6);
    QWidget::paintEvent(event);
}
