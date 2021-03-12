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
#include <KWindowEffects>
#include <QtConcurrent/QtConcurrent>
#include <stdio.h>
#include <string.h>
#include "clickLabel.h"
#include "MacroFile.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    findPointMount = false;
    telephoneNum = 0;
    driveVolumeNum = 0;

    const QByteArray idd(THEME_QT_SCHEMA);

    if(QGSettings::isSchemaInstalled(idd))
    {
        qtSettings = new QGSettings(idd);
    }

    const QByteArray id(AUTOLOAD);
    if(QGSettings::isSchemaInstalled(idd))
    {
        ifsettings = new QGSettings(id);
    }

    initThemeMode();

    installEventFilter(this);
    m_dataFlashDisk = FlashDiskData::getInstance();
    ui->setupUi(this);
    //框架的样式设置
    //set the style of the framework
    interfaceHideTime = new QTimer(this);
    connect(interfaceHideTime, SIGNAL(timeout()), this, SLOT(onMaininterfacehide()));
    initTransparentState();
    ui->centralWidget->setObjectName("centralWidget");
    iconSystray = QIcon::fromTheme("media-removable-symbolic");
    vboxlayout = new QVBoxLayout();
    //hboxlayout = new QHBoxLayout();
#if (QT_VERSION < QT_VERSION_CHECK(5,7,0))
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::X11BypassWindowManagerHint);
#else
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Popup);
#endif
    this->setAttribute(Qt::WA_TranslucentBackground);
    m_systray = new QSystemTrayIcon;
    m_systray->setIcon(iconSystray);
    m_systray->setVisible(true);
    m_systray->setToolTip(tr("usb management tool"));
    //init the screen
    screen = qApp->primaryScreen();
    //underlying code to get the information of the usb device
    getDeviceInfo();
    connect(m_systray, &QSystemTrayIcon::activated, this, &MainWindow::iconActivated);
    connect(this,&MainWindow::convertShowWindow,this,&MainWindow::onConvertShowWindow);
    connect(this,&MainWindow::convertUpdateWindow,this,&MainWindow::onConvertUpdateWindow);
    ui->centralWidget->setLayout(vboxlayout);
    QDBusConnection::sessionBus().connect(QString(), QString("/taskbar/click"), \
                                                  "com.ukui.panel.plugins.taskbar", "sendToUkuiDEApp", this, SLOT(on_clickPanelToHideInterface()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onRequestSendDesktopNotify(QString message)
{
    QDBusInterface iface("org.freedesktop.Notifications",
                         "/org/freedesktop/Notifications",
                         "org.freedesktop.Notifications",
                         QDBusConnection::sessionBus());
    QList<QVariant> args;
    args<<(tr("ukui-flash-disk"))
       <<((unsigned int) 0)
      <<QString("media-removable-symbolic")
     <<tr("kindly reminder") //显示的是什么类型的信息
    <<message //显示的具体信息
    <<QStringList()
    <<QVariantMap()
    <<(int)-1;
    iface.callWithArgumentList(QDBus::AutoDetect,"Notify",args);
}

void MainWindow::onInsertAbnormalDiskNotify(QString message)
{
    QDBusInterface iface("org.freedesktop.Notifications",
                         "/org/freedesktop/Notifications",
                         "org.freedesktop.Notifications",
                         QDBusConnection::sessionBus());
    QList<QVariant> args;
    args<<(tr("ukui-flash-disk"))
       <<((unsigned int) 0)
      <<QString("media-removable-symbolic")
     <<tr("wrong reminder") //显示的是什么类型的信息
    <<message //显示的具体信息
    <<QStringList()
    <<QVariantMap()
    <<(int)-1;
    iface.callWithArgumentList(QDBus::AutoDetect,"Notify",args);
}

void MainWindow::onNotifyWnd(QObject* obj, QEvent *event)
{
    QString strObjName(obj->metaObject()->className());
    if (strObjName == "QSystemTrayIconSys") {
        if (!m_bIsMouseInTraIcon && event->type() == QEvent::Enter) {
            if (interfaceHideTime->isActive()) {
                interfaceHideTime->stop();
            }
            m_bIsMouseInTraIcon = true;
        } else if (m_bIsMouseInTraIcon && event->type() == QEvent::Leave) {
            if (!m_bIsMouseInCentral) {
                interfaceHideTime->start(2000);
            }
            m_bIsMouseInTraIcon = false;
        }
    } else if (obj == ui->centralWidget) {
        if (!m_bIsMouseInCentral && event->type() == QEvent::Enter) {
            interfaceHideTime->stop();
            ui->centralWidget->show();
            m_bIsMouseInCentral = true;
        } else if (m_bIsMouseInCentral && event->type() == QEvent::Leave) {
            if (!m_bIsMouseInTraIcon) {
                interfaceHideTime->start(2000);
            }
            m_bIsMouseInCentral = false;
        }
    }
}

void MainWindow::initThemeMode()
{
    connect(qtSettings,&QGSettings::changed,this,[=](const QString &key)
    {
        auto style = qtSettings->get(key).toString();
        currentThemeMode = qtSettings->get(MODE_QT_KEY).toString();
    });
    currentThemeMode = qtSettings->get(MODE_QT_KEY).toString();
}

void MainWindow::on_clickPanelToHideInterface()
{
    if(!ui->centralWidget->isHidden())
        ui->centralWidget->hide();
}

void MainWindow::getDeviceInfo()
{
    // setting
    FILE *fp = NULL;
    int a = 0;
    char buf[128] = {0};

    fp = fopen("/proc/cmdline","r");
    if (fp) {
        while(fscanf(fp,"%127s",buf) >0 )
        {
            if(strcmp(buf,"live") == 0)
            {
                a++;
            }
        }
        fclose(fp);
    }
    if(a > 0)
    {
        QProcess::startDetached("gsettings set org.ukui.flash-disk.autoload ifautoload false");
    }

//callback function that to monitor the insertion and removal of the underlying equipment
    GVolumeMonitor *g_volume_monitor = g_volume_monitor_get();
    g_signal_connect (g_volume_monitor, "drive-connected", G_CALLBACK (drive_connected_callback), this);
    g_signal_connect (g_volume_monitor, "drive-disconnected", G_CALLBACK (drive_disconnected_callback), this);
    g_signal_connect (g_volume_monitor, "volume-added", G_CALLBACK (volume_added_callback), this);
    g_signal_connect (g_volume_monitor, "volume-removed", G_CALLBACK (volume_removed_callback), this);
    g_signal_connect (g_volume_monitor, "mount-added", G_CALLBACK (mount_added_callback), this);
    g_signal_connect (g_volume_monitor, "mount-removed", G_CALLBACK (mount_removed_callback), this);

    GList *lDrive = NULL, *lVolume = NULL, *lMount = NULL;
//about drive
    GList *current_drive_list = g_volume_monitor_get_connected_drives(g_volume_monitor);
    for (lDrive = current_drive_list; lDrive != NULL; lDrive = lDrive->next) {
        GDrive *gdrive = (GDrive *)lDrive->data;
        char *devPath = g_drive_get_identifier(gdrive,G_DRIVE_IDENTIFIER_KIND_UNIX_DEVICE);
        if (devPath != NULL) {
            FDDriveInfo driveInfo;
            driveInfo.strId = devPath;
            char *strName = g_drive_get_name(gdrive);
            if (strName) {
                driveInfo.strName = strName;
                g_free(strName);
            }
            driveInfo.isCanEject = g_drive_can_eject(gdrive);
            driveInfo.isCanStop = g_drive_can_stop(gdrive);
            driveInfo.isCanStart = g_drive_can_start(gdrive);
            driveInfo.isRemovable = g_drive_is_removable(gdrive);
            if(driveInfo.isCanEject || driveInfo.isCanStop) {
                if(!g_str_has_prefix(devPath,"/dev/sda")) {
                    if(g_str_has_prefix(devPath,"/dev/sr") || g_str_has_prefix(devPath,"/dev/sd")) {
                        GList* gdriveVolumes = g_drive_get_volumes(gdrive);
                        if (gdriveVolumes) {
                            for(lVolume = gdriveVolumes; lVolume != NULL; lVolume = lVolume->next){ //遍历驱动器上的所有卷设备
                                GVolume* volume = (GVolume *)lVolume->data;
                                FDVolumeInfo volumeInfo;
                                bool isValidMount = true;
                                char *volumeId = g_volume_get_identifier(volume,G_VOLUME_IDENTIFIER_KIND_UNIX_DEVICE);
                                if (volumeId) {
                                    volumeInfo.strId = volumeId;
                                    g_free(volumeId);
                                } else {
                                    continue ;
                                }
                                char *volumeName = g_volume_get_name(volume);
                                if (volumeName) {
                                    volumeInfo.strName = volumeName;
                                    g_free(volumeName);
                                }
                                char *strDevName = g_volume_get_identifier(volume,G_VOLUME_IDENTIFIER_KIND_UNIX_DEVICE);
                                if (strDevName) {
                                    volumeInfo.strDevName = strDevName;
                                    g_free(strDevName);
                                }
                                volumeInfo.isCanMount = g_volume_can_mount(volume);
                                volumeInfo.isCanEject = g_volume_can_eject(volume);
                                volumeInfo.isShouldAutoMount = g_volume_should_automount(volume);
                                GMount* mount = g_volume_get_mount(volume); //get当前卷设备的挂载信息
                                if (mount) {                  //该卷设备已挂载
                                    volumeInfo.mountInfo.isCanEject = g_mount_can_eject(mount);
                                    volumeInfo.mountInfo.isCanUnmount = g_mount_can_unmount(mount);
                                    if (volumeInfo.mountInfo.isCanEject || volumeInfo.mountInfo.isCanUnmount) {
                                        char *mountId = g_mount_get_uuid(mount);
                                        if (mountId) {
                                            volumeInfo.mountInfo.strId = mountId;
                                            g_free(mountId);
                                        }
                                        char *mountName = g_mount_get_name(mount);
                                        if (mountName) {
                                            volumeInfo.mountInfo.strName = mountName;
                                            g_free(mountName);
                                        }
                                        // get mount total size
                                        GFile *fileRoot = g_mount_get_root(mount);
                                        GFileInfo *info = g_file_query_filesystem_info(fileRoot,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE,nullptr,nullptr);
                                        volumeInfo.mountInfo.lluTotalSize = g_file_info_get_attribute_uint64(info,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE);
                                        g_object_unref(info);
                                        g_object_unref(fileRoot);
                                        // get mount uri
                                        GFile *root = g_mount_get_default_location(mount);
                                        if (root) {
                                            volumeInfo.mountInfo.isNativeDev = g_file_is_native(root);     //判断设备是本地设备or网络设备
                                            char *mountUri = g_file_get_uri(root);           //get挂载点的uri路径
                                            if (mountUri) {
                                                volumeInfo.mountInfo.strUri = mountUri;
                                                if (g_str_has_prefix(mountUri,"file:///data")) {
                                                    isValidMount = false;
                                                } else {
                                                    if (volumeInfo.mountInfo.strId.empty()) {
                                                        volumeInfo.mountInfo.strId = volumeInfo.mountInfo.strUri;
                                                    }
                                                }
                                                g_free(mountUri);
                                            }
                                            char *tooltip = g_file_get_parse_name(root);      //提示，即文件的解释
                                            if (tooltip) {
                                                volumeInfo.mountInfo.strTooltip =tooltip;
                                                g_free(tooltip);
                                            }
                                            g_object_unref(root);
                                        }
                                    }
                                    g_object_unref(mount);
                                } else {
                                    if(ifsettings->get(IFAUTOLOAD).toBool()) {
                                        g_volume_mount(volume,
                                                G_MOUNT_MOUNT_NONE,
                                                nullptr,
                                                nullptr,
                                                nullptr,
                                                nullptr);
                                    }
                                }
                                if (isValidMount) {
                                    driveInfo.listVolumes[volumeInfo.strId] = volumeInfo;
                                }
                            }
                            g_list_free(gdriveVolumes);
                        }
                        m_dataFlashDisk->addDriveInfo(driveInfo);
                    }
                }
            }
            g_free(devPath);
        }
    }
    if (current_drive_list) {
        g_list_free(current_drive_list);
    }
//about volume not associated with a drive
    GList *current_volume_list = g_volume_monitor_get_volumes(g_volume_monitor);
    if (current_volume_list) {
        for (lVolume = current_volume_list; lVolume != NULL; lVolume = lVolume->next) {
            GVolume *volume = (GVolume *)lVolume->data;
            GDrive *gdrive = g_volume_get_drive(volume);
            if (!gdrive) {
                FDVolumeInfo volumeInfo;
                bool isValidMount = true;
                char *devPath = g_volume_get_identifier(volume,G_VOLUME_IDENTIFIER_KIND_UNIX_DEVICE);
                if (devPath) {
                    if (!(g_str_has_prefix(devPath,"/dev/sr") || (g_str_has_prefix(devPath,"/dev/sd") 
                            && !g_str_has_prefix(devPath,"/dev/sda")))) {
                        g_free(devPath);
                        continue;
                    }
                    g_free(devPath);
                }
                char *volumeId = g_volume_get_identifier(volume,G_VOLUME_IDENTIFIER_KIND_UNIX_DEVICE);
                if (volumeId) {
                    volumeInfo.strId = volumeId;
                    g_free(volumeId);
                } else {
                    continue ;
                }
                char *volumeName = g_volume_get_name(volume);
                if (volumeName) {
                    volumeInfo.strName = volumeName;
                    g_free(volumeName);
                }
                char *strDevName = g_volume_get_identifier(volume,G_VOLUME_IDENTIFIER_KIND_UNIX_DEVICE);
                if (strDevName) {
                    volumeInfo.strDevName = strDevName;
                    g_free(strDevName);
                }
                volumeInfo.isCanMount = g_volume_can_mount(volume);
                volumeInfo.isCanEject = g_volume_can_eject(volume);
                volumeInfo.isShouldAutoMount = g_volume_should_automount(volume);
                GMount* mount = g_volume_get_mount(volume); //get当前卷设备的挂载信息
                if (mount) {                  //该卷设备已挂载
                    volumeInfo.mountInfo.isCanEject = g_mount_can_eject(mount);
                    volumeInfo.mountInfo.isCanUnmount = g_mount_can_unmount(mount);
                    if (volumeInfo.mountInfo.isCanEject || volumeInfo.mountInfo.isCanUnmount) {
                        char *mountId = g_mount_get_uuid(mount);
                        if (mountId) {
                            volumeInfo.mountInfo.strId = mountId;
                            g_free(mountId);
                        }
                        char *mountName = g_mount_get_name(mount);
                        if (mountName) {
                            volumeInfo.mountInfo.strName = mountName;
                            g_free(mountName);
                        }
                        // get mount total size
                        GFile *fileRoot = g_mount_get_root(mount);
                        GFileInfo *info = g_file_query_filesystem_info(fileRoot,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE,nullptr,nullptr);
                        volumeInfo.mountInfo.lluTotalSize = g_file_info_get_attribute_uint64(info,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE);
                        g_object_unref(info);
                        g_object_unref(fileRoot);
                        // get mount uri
                        GFile *root = g_mount_get_default_location(mount);
                        if (root) {
                            volumeInfo.mountInfo.isNativeDev = g_file_is_native(root);     //判断设备是本地设备or网络设备
                            char *mountUri = g_file_get_uri(root);           //get挂载点的uri路径
                            if (mountUri) {
                                volumeInfo.mountInfo.strUri = mountUri;
                                if (g_str_has_prefix(mountUri,"file:///data")) {
                                    isValidMount = false;
                                } else {
                                    if (volumeInfo.mountInfo.strId.empty()) {
                                        volumeInfo.mountInfo.strId = volumeInfo.mountInfo.strUri;
                                    }
                                }
                                g_free(mountUri);
                            }
                            char *tooltip = g_file_get_parse_name(root);      //提示，即文件的解释
                            if (tooltip) {
                                volumeInfo.mountInfo.strTooltip =tooltip;
                                g_free(tooltip);
                            }
                            g_object_unref(root);
                        }
                    }
                    g_object_unref(mount);
                } else {
                    if (volumeInfo.isCanEject) {
                        if(ifsettings->get(IFAUTOLOAD).toBool()) {
                            g_volume_mount(volume,
                                    G_MOUNT_MOUNT_NONE,
                                    nullptr,
                                    nullptr,
                                    nullptr,
                                    nullptr);
                        }
                    }
                }
                if (isValidMount) {
                    m_dataFlashDisk->addVolumeInfo(volumeInfo);
                }
            } else {
                g_object_unref(gdrive);
            }
        }
        g_list_free(current_volume_list);
    }
//about mount not associated with a volume
    GList *current_mount_list = g_volume_monitor_get_mounts(g_volume_monitor);
    if (current_mount_list) {
        for (lMount = current_mount_list; lMount != NULL; lMount = lMount->next) {
            GMount *gmount = (GMount *)lMount->data;
            GVolume *gvolume = g_mount_get_volume(gmount);
            if (!gvolume) {
                GDrive* gdrive = g_mount_get_drive(gmount);
                if (gdrive) {
                    char *devPath = g_drive_get_identifier(gdrive,G_DRIVE_IDENTIFIER_KIND_UNIX_DEVICE);
                    if (devPath != NULL) {
                        FDDriveInfo driveInfo;
                        driveInfo.strId = devPath;
                        char *strName = g_drive_get_name(gdrive);
                        if (strName) {
                            driveInfo.strName = strName;
                            g_free(strName);
                        }
                        driveInfo.isCanEject = g_drive_can_eject(gdrive);
                        driveInfo.isCanStop = g_drive_can_stop(gdrive);
                        driveInfo.isCanStart = g_drive_can_start(gdrive);
                        driveInfo.isRemovable = g_drive_is_removable(gdrive);
                        if(driveInfo.isCanEject || driveInfo.isCanStop) {
                            if(!g_str_has_prefix(devPath,"/dev/sda")) {
                                if(g_str_has_prefix(devPath,"/dev/sr") || g_str_has_prefix(devPath,"/dev/sd")) {
                                    FDMountInfo mountInfo;
                                    bool isValidMount = true;
                                    mountInfo.isCanEject = g_mount_can_eject(gmount);
                                    mountInfo.isCanUnmount = g_mount_can_unmount(gmount);
                                    if (mountInfo.isCanEject || mountInfo.isCanUnmount) {
                                        char *mountId = g_mount_get_uuid(gmount);
                                        if (mountId) {
                                            mountInfo.strId = mountId;
                                            g_free(mountId);
                                        }
                                        char *mountName = g_mount_get_name(gmount);
                                        if (mountName) {
                                            mountInfo.strName = mountName;
                                            g_free(mountName);
                                        }
                                        // get mount total size
                                        GFile *fileRoot = g_mount_get_root(gmount);
                                        GFileInfo *info = g_file_query_filesystem_info(fileRoot,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE,nullptr,nullptr);
                                        mountInfo.lluTotalSize = g_file_info_get_attribute_uint64(info,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE);
                                        g_object_unref(info);
                                        g_object_unref(fileRoot);
                                        // get mount uri
                                        GFile *root = g_mount_get_default_location(gmount);
                                        if (root) {
                                            mountInfo.isNativeDev = g_file_is_native(root);     //判断设备是本地设备or网络设备
                                            char *mountUri = g_file_get_uri(root);           //get挂载点的uri路径
                                            if (mountUri) {
                                                mountInfo.strUri = mountUri;
                                                if (g_str_has_prefix(mountUri,"file:///data")) {
                                                    isValidMount = false;
                                                } else {
                                                    if (mountInfo.strId.empty()) {
                                                        mountInfo.strId = mountInfo.strUri;
                                                    }
                                                }
                                                g_free(mountUri);
                                            }
                                            char *tooltip = g_file_get_parse_name(root);      //提示，即文件的解释
                                            if (tooltip) {
                                                mountInfo.strTooltip =tooltip;
                                                g_free(tooltip);
                                            }
                                            g_object_unref(root);
                                        }
                                        if (isValidMount) {
                                            FDVolumeInfo volumeInfo;
                                            volumeInfo.mountInfo = mountInfo;
                                            m_dataFlashDisk->addMountInfoWithDrive(driveInfo, volumeInfo, mountInfo);
                                        }
                                    }
                                }
                            }
                        }
                        g_free(devPath);
                    }
                    g_object_unref(gdrive);
                } else {
                    FDMountInfo mountInfo;
                    bool isValidMount = true;
                    mountInfo.isCanEject = g_mount_can_eject(gmount);
                    mountInfo.isCanUnmount = g_mount_can_unmount(gmount);
                    if (mountInfo.isCanEject || mountInfo.isCanUnmount) {
                        char *mountId = g_mount_get_uuid(gmount);
                        if (mountId) {
                            mountInfo.strId = mountId;
                            g_free(mountId);
                        }
                        char *mountName = g_mount_get_name(gmount);
                        if (mountName) {
                            mountInfo.strName = mountName;
                            g_free(mountName);
                        }
                        // get mount total size
                        GFile *fileRoot = g_mount_get_root(gmount);
                        GFileInfo *info = g_file_query_filesystem_info(fileRoot,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE,nullptr,nullptr);
                        mountInfo.lluTotalSize = g_file_info_get_attribute_uint64(info,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE);
                        g_object_unref(info);
                        g_object_unref(fileRoot);
                        // get mount uri
                        GFile *root = g_mount_get_default_location(gmount);
                        if (root) {
                            mountInfo.isNativeDev = g_file_is_native(root);     //判断设备是本地设备or网络设备
                            char *mountUri = g_file_get_uri(root);           //get挂载点的uri路径
                            if (mountUri) {
                                mountInfo.strUri = mountUri;
                                if (g_str_has_prefix(mountUri,"file:///data")) {
                                    isValidMount = false;
                                } else {
                                    if (mountInfo.strId.empty()) {
                                        mountInfo.strId = mountInfo.strUri;
                                    }
                                }
                                g_free(mountUri);
                            }
                            char *tooltip = g_file_get_parse_name(root);      //提示，即文件的解释
                            if (tooltip) {
                                mountInfo.strTooltip =tooltip;
                                g_free(tooltip);
                            }
                            g_object_unref(root);
                        }
                        if (isValidMount) {
                            m_dataFlashDisk->addMountInfo(mountInfo);
                        }
                    }
                }
            } else {
                g_object_unref(gvolume);
            }
        }
        g_list_free(current_mount_list);
    }
 //determine the systray icon should be showed  or be hieded
    if(m_dataFlashDisk->getValidInfoCount() >= 1) {
        m_systray->show();
    } else {
        m_systray->hide();
    }
}

void MainWindow::onConvertShowWindow()
{
    insertorclick = true;
    MainWindowShow();
    onRequestSendDesktopNotify(tr("Please do not pull out the USB flash disk when reading or writing"));
}

void MainWindow::onConvertUpdateWindow(QString strDevName, unsigned uDevType)
{
    // uDevType: 0 drive , 1 volume, 2 mount
    if (uDevType != 0) {    // not drive detached
        insertorclick = true;
        MainWindowShow(true);
    }
}

//the drive-connected callback function the is triggered when the usb device is inseted
void MainWindow::drive_connected_callback(GVolumeMonitor *monitor, GDrive *drive, MainWindow *p_this)
{
    qDebug()<<"drive add";
    if(p_this->ifsettings->get(IFAUTOLOAD).toBool())
    {
        GList *lVolume = NULL;
        FDDriveInfo driveInfo;
        GDrive *gdrive = (GDrive *)drive;
        unsigned uSubVolumeSize = 0;
        char *devPath = g_drive_get_identifier(gdrive,G_DRIVE_IDENTIFIER_KIND_UNIX_DEVICE);
        if (devPath != NULL) {
            driveInfo.strId = devPath;
            char *strName = g_drive_get_name(gdrive);
            if (strName) {
                driveInfo.strName = strName;
                g_free(strName);
            }
            driveInfo.isCanEject = g_drive_can_eject(gdrive);
            driveInfo.isCanStop = g_drive_can_stop(gdrive);
            driveInfo.isCanStart = g_drive_can_start(gdrive);
            driveInfo.isRemovable = g_drive_is_removable(gdrive);
            g_free(devPath);
        }
        lVolume = g_drive_get_volumes(gdrive);
        if (lVolume) {
            uSubVolumeSize = g_list_length(lVolume);
            g_list_free(lVolume);
        }

        if (!driveInfo.strId.empty()) {
            p_this->m_dataFlashDisk->addDriveInfo(driveInfo);
        }
        // perhaps uSubVolumeSize is 0 and is ok ?
        // else {
        //     qDebug()<<"wrong disk has intered";
        //     p_this->onInsertAbnormalDiskNotify(tr("There is a problem with this device"));
        // }
    }

    if(p_this->m_dataFlashDisk->getValidInfoCount() >= 1) {
        p_this->m_systray->show();
    }

    p_this->triggerType = 0;
    p_this->m_dataFlashDisk->OutputInfos();
}

//the drive-disconnected callback function the is triggered when the usb device is pull out
void MainWindow::drive_disconnected_callback (GVolumeMonitor *monitor, GDrive *drive, MainWindow *p_this)
{
    qDebug()<<"drive disconnect";

    FDDriveInfo driveInfo;
    char *devPath = g_drive_get_identifier(drive,G_DRIVE_IDENTIFIER_KIND_UNIX_DEVICE);
    if (devPath != NULL) {
        driveInfo.strId = devPath;
        g_free(devPath);
    }
    p_this->m_dataFlashDisk->removeDriveInfo(driveInfo);
    if(p_this->m_dataFlashDisk->getValidInfoCount() == 0) {
        p_this->ui->centralWidget->hide();
        p_this->m_systray->hide();
    }
    p_this->m_dataFlashDisk->OutputInfos();
}

//when the usb device is identified,we should mount every partition
void MainWindow::volume_added_callback(GVolumeMonitor *monitor, GVolume *volume, MainWindow *p_this)
{
    qDebug()<<"volume add";

    FILE *fp = NULL;
    int a = 0;
    char buf[128] = {0};

    fp = fopen("/proc/cmdline","r");
    if (fp) {
        while(fscanf(fp,"%127s",buf) > 0)
        {
            if(strcmp(buf,"live") == 0)
            {
                a++;
            }
        }
        fclose(fp);
    }
    p_this->ifautoload = p_this->ifsettings->get(IFAUTOLOAD).toBool();
    if(a > 0)
    {
        QProcess::startDetached("gsettings set org.ukui.flash-disk.autoload ifautoload false");
    }
    else
    {
        QProcess::startDetached("gsettings set org.ukui.flash-disk.autoload ifautoload true");
    }
    bool isNewMount = false;
    GDrive* gdrive = g_volume_get_drive(volume);
    if(!gdrive) {
        FDVolumeInfo volumeInfo;
        bool isValidMount = true;
        char *devPath = g_volume_get_identifier(volume,G_VOLUME_IDENTIFIER_KIND_UNIX_DEVICE);
        if (devPath) {
            if (!(g_str_has_prefix(devPath,"/dev/sr") || (g_str_has_prefix(devPath,"/dev/sd")
                    && !g_str_has_prefix(devPath,"/dev/sda")))) {
                g_free(devPath);
                return;
            }
            g_free(devPath);
        }
        char *volumeId = g_volume_get_identifier(volume,G_VOLUME_IDENTIFIER_KIND_UNIX_DEVICE);
        if (volumeId) {
            volumeInfo.strId = volumeId;
            g_free(volumeId);
        } else {
            return ;
        }
        char *volumeName = g_volume_get_name(volume);
        if (volumeName) {
            volumeInfo.strName = volumeName;
            g_free(volumeName);
        }
        char *strDevName = g_volume_get_identifier(volume,G_VOLUME_IDENTIFIER_KIND_UNIX_DEVICE);
        if (strDevName) {
            volumeInfo.strDevName = strDevName;
            g_free(strDevName);
        }
        volumeInfo.isCanMount = g_volume_can_mount(volume);
        volumeInfo.isCanEject = g_volume_can_eject(volume);
        volumeInfo.isShouldAutoMount = g_volume_should_automount(volume);
        GMount* mount = g_volume_get_mount(volume); //get当前卷设备的挂载信息
        if (mount) {                  //该卷设备已挂载
            volumeInfo.mountInfo.isCanEject = g_mount_can_eject(mount);
            volumeInfo.mountInfo.isCanUnmount = g_mount_can_unmount(mount);
            if (volumeInfo.mountInfo.isCanEject || volumeInfo.mountInfo.isCanUnmount) {
                char *mountId = g_mount_get_uuid(mount);
                if (mountId) {
                    volumeInfo.mountInfo.strId = mountId;
                    g_free(mountId);
                }
                char *mountName = g_mount_get_name(mount);
                if (mountName) {
                    volumeInfo.mountInfo.strName = mountName;
                    g_free(mountName);
                }
                isNewMount = !(p_this->m_dataFlashDisk->isMountInfoExist(volumeInfo.mountInfo));
                // get mount total size
                GFile *fileRoot = g_mount_get_root(mount);
                GFileInfo *info = g_file_query_filesystem_info(fileRoot,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE,nullptr,nullptr);
                volumeInfo.mountInfo.lluTotalSize = g_file_info_get_attribute_uint64(info,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE);
                g_object_unref(info);
                g_object_unref(fileRoot);
                // get mount uri
                GFile *root = g_mount_get_default_location(mount);
                if (root) {
                    volumeInfo.mountInfo.isNativeDev = g_file_is_native(root);     //判断设备是本地设备or网络设备
                    char *mountUri = g_file_get_uri(root);           //get挂载点的uri路径
                    if (mountUri) {
                        volumeInfo.mountInfo.strUri = mountUri;
                        if (g_str_has_prefix(mountUri,"file:///data")) {
                            isValidMount = false;
                        } else {
                            if (volumeInfo.mountInfo.strId.empty()) {
                                volumeInfo.mountInfo.strId = volumeInfo.mountInfo.strUri;
                            }
                        }
                        g_free(mountUri);
                    }
                    char *tooltip = g_file_get_parse_name(root);      //提示，即文件的解释
                    if (tooltip) {
                        volumeInfo.mountInfo.strTooltip =tooltip;
                        g_free(tooltip);
                    }
                    g_object_unref(root);
                }
            }
            g_object_unref(mount);
        } else {
            if (volumeInfo.isCanEject) {
                if(p_this->ifsettings->get(IFAUTOLOAD).toBool()) {
                    g_volume_mount(volume,
                                G_MOUNT_MOUNT_NONE,
                                nullptr,
                                nullptr,
                                GAsyncReadyCallback(frobnitz_result_func_volume),
                                p_this);
                }
            }
        }
        if (isValidMount) {
            p_this->m_dataFlashDisk->addVolumeInfo(volumeInfo);
        }
    } else {
        char *devPath = g_drive_get_identifier(gdrive,G_DRIVE_IDENTIFIER_KIND_UNIX_DEVICE);
        if (devPath != NULL) {
            FDDriveInfo driveInfo;
            FDVolumeInfo volumeInfo;
            driveInfo.strId = devPath;
            char *strName = g_drive_get_name(gdrive);
            if (strName) {
                driveInfo.strName = strName;
                g_free(strName);
            }
            driveInfo.isCanEject = g_drive_can_eject(gdrive);
            driveInfo.isCanStop = g_drive_can_stop(gdrive);
            driveInfo.isCanStart = g_drive_can_start(gdrive);
            driveInfo.isRemovable = g_drive_is_removable(gdrive);

            if(driveInfo.isCanEject || driveInfo.isCanStop) {
                if(!g_str_has_prefix(devPath,"/dev/sda")) {
                    if(g_str_has_prefix(devPath,"/dev/sr") || g_str_has_prefix(devPath,"/dev/sd")) {
                        char *volumeId = g_volume_get_identifier(volume,G_VOLUME_IDENTIFIER_KIND_UNIX_DEVICE);
                        if (volumeId) {
                            volumeInfo.strId = volumeId;
                            g_free(volumeId);
                            char *volumeName = g_volume_get_name(volume);
                            if (volumeName) {
                                volumeInfo.strName = volumeName;
                                g_free(volumeName);
                            }
                            char *strDevName = g_volume_get_identifier(volume,G_VOLUME_IDENTIFIER_KIND_UNIX_DEVICE);
                            if (strDevName) {
                                volumeInfo.strDevName = strDevName;
                                g_free(strDevName);
                            }
                            volumeInfo.isCanMount = g_volume_can_mount(volume);
                            volumeInfo.isCanEject = g_volume_can_eject(volume);
                            volumeInfo.isShouldAutoMount = g_volume_should_automount(volume);
                            GMount* mount = g_volume_get_mount(volume); //get当前卷设备的挂载信息
                            if (mount) {                  //该卷设备已挂载
                                volumeInfo.mountInfo.isCanEject = g_mount_can_eject(mount);
                                volumeInfo.mountInfo.isCanUnmount = g_mount_can_unmount(mount);
                                if (volumeInfo.mountInfo.isCanEject || volumeInfo.mountInfo.isCanUnmount) {
                                    char *mountId = g_mount_get_uuid(mount);
                                    if (mountId) {
                                        volumeInfo.mountInfo.strId = mountId;
                                        g_free(mountId);
                                    }
                                    char *mountName = g_mount_get_name(mount);
                                    if (mountName) {
                                        volumeInfo.mountInfo.strName = mountName;
                                        g_free(mountName);
                                    }
                                    isNewMount = !(p_this->m_dataFlashDisk->isMountInfoExist(volumeInfo.mountInfo));
                                    // get mount total size
                                    GFile *fileRoot = g_mount_get_root(mount);
                                    GFileInfo *info = g_file_query_filesystem_info(fileRoot,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE,nullptr,nullptr);
                                    volumeInfo.mountInfo.lluTotalSize = g_file_info_get_attribute_uint64(info,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE);
                                    g_object_unref(info);
                                    g_object_unref(fileRoot);
                                    // get mount uri
                                    GFile *root = g_mount_get_default_location(mount);
                                    if (root) {
                                        volumeInfo.mountInfo.isNativeDev = g_file_is_native(root);     //判断设备是本地设备or网络设备
                                        char *mountUri = g_file_get_uri(root);           //get挂载点的uri路径
                                        if (mountUri) {
                                            volumeInfo.mountInfo.strUri = mountUri;
                                            if (!g_str_has_prefix(mountUri,"file:///data")) {
                                                if (volumeInfo.mountInfo.strId.empty()) {
                                                    volumeInfo.mountInfo.strId = volumeInfo.mountInfo.strUri;
                                                }
                                            }
                                            g_free(mountUri);
                                        }
                                        char *tooltip = g_file_get_parse_name(root);      //提示，即文件的解释
                                        if (tooltip) {
                                            volumeInfo.mountInfo.strTooltip =tooltip;
                                            g_free(tooltip);
                                        }
                                        g_object_unref(root);
                                    }
                                }
                                g_object_unref(mount);
                            } else {
                                if(p_this->ifsettings->get(IFAUTOLOAD).toBool()) {
                                    g_volume_mount(volume,
                                                G_MOUNT_MOUNT_NONE,
                                                nullptr,
                                                nullptr,
                                                GAsyncReadyCallback(frobnitz_result_func_volume),
                                                p_this);
                                }
                            }
                        }
                    }
                }
            }
            g_free(devPath);
            p_this->m_dataFlashDisk->addVolumeInfoWithDrive(driveInfo, volumeInfo);
            if (g_str_has_prefix(volumeInfo.strDevName.c_str(),"/dev/sr") && isNewMount) {
                //qDebug()<<"cd data disk has mounted!";
                Q_EMIT p_this->convertShowWindow();
            }
        }
        g_object_unref(gdrive);
    }
    if(p_this->m_dataFlashDisk->getValidInfoCount() > 0)
    {
        p_this->m_systray->show();
    }
    p_this->m_dataFlashDisk->OutputInfos();
}

//when the U disk is pull out we should reduce all its partitions
void MainWindow::volume_removed_callback(GVolumeMonitor *monitor, GVolume *volume, MainWindow *p_this)
{
    qDebug()<<"volume removed";
    FDVolumeInfo volumeInfo;
    char *volumeId = g_volume_get_identifier(volume,G_VOLUME_IDENTIFIER_KIND_UNIX_DEVICE);
    if (volumeId) {
        volumeInfo.strId = volumeId;
        g_free(volumeId);
    }
    char *strName = g_volume_get_name(volume);
    if (strName) {
        volumeInfo.strName = strName;
        g_free(strName);
    }
    p_this->m_dataFlashDisk->removeVolumeInfo(volumeInfo);
    if(p_this->m_dataFlashDisk->getValidInfoCount() == 0) {
        p_this->m_systray->hide();
    }
    p_this->m_dataFlashDisk->OutputInfos();
    Q_EMIT p_this->convertUpdateWindow(QString::fromStdString(volumeInfo.strName), 1);     //emit a signal to update the MainMainShow slot
}

//when the volumes were mounted we add its mounts number
void MainWindow::mount_added_callback(GVolumeMonitor *monitor, GMount *mount, MainWindow *p_this)
{
    qDebug()<<"mount add";

    GDrive* gdrive = g_mount_get_drive(mount);
    GVolume* gvolume = g_mount_get_volume(mount);
    string strVolumePath = "";
    FDDriveInfo driveInfo;
    FDVolumeInfo volumeInfo;
    FDMountInfo mountInfo;
    bool isValidMount = true;
    if (gdrive) {
        char *devPath = g_drive_get_identifier(gdrive,G_DRIVE_IDENTIFIER_KIND_UNIX_DEVICE);
        if (devPath != NULL) {
            driveInfo.strId = devPath;
            char *strName = g_drive_get_name(gdrive);
            if (strName) {
                driveInfo.strName = strName;
                g_free(strName);
            }
            driveInfo.isCanEject = g_drive_can_eject(gdrive);
            driveInfo.isCanStop = g_drive_can_stop(gdrive);
            driveInfo.isCanStart = g_drive_can_start(gdrive);
            driveInfo.isRemovable = g_drive_is_removable(gdrive);
            g_free(devPath);
        }
        g_object_unref(gdrive);
    }
    if (gvolume) {
        char *volumeId = g_volume_get_identifier(gvolume,G_VOLUME_IDENTIFIER_KIND_UNIX_DEVICE);
        char *devVolumePath = g_volume_get_identifier(gvolume,G_VOLUME_IDENTIFIER_KIND_UNIX_DEVICE);
        if (devVolumePath) {
            strVolumePath = devVolumePath;
            g_free(devVolumePath);
        }
        if (volumeId) {
            volumeInfo.strId = volumeId;
            g_free(volumeId);
            char *volumeName = g_volume_get_name(gvolume);
            if (volumeName) {
                volumeInfo.strName = volumeName;
                g_free(volumeName);
            }
            char *strDevName = g_volume_get_identifier(gvolume,G_VOLUME_IDENTIFIER_KIND_UNIX_DEVICE);
            if (strDevName) {
                volumeInfo.strDevName = strDevName;
                g_free(strDevName);
            }

            volumeInfo.isCanMount = g_volume_can_mount(gvolume);
            volumeInfo.isCanEject = g_volume_can_eject(gvolume);
            volumeInfo.isShouldAutoMount = g_volume_should_automount(gvolume);
        }
        g_object_unref(gvolume);
    }
    mountInfo.isCanEject = g_mount_can_eject(mount);
    char *mountId = g_mount_get_uuid(mount);
    if (mountId) {
        mountInfo.strId = mountId;
        g_free(mountId);
    }
    char *mountName = g_mount_get_name(mount);
    if (mountName) {
        mountInfo.strName = mountName;
        g_free(mountName);
    }
    mountInfo.isCanUnmount = g_mount_can_unmount(mount);
    GFile *root = g_mount_get_default_location(mount);
    // get mount total size
    GFile *fileRoot = g_mount_get_root(mount);
    GFileInfo *info = g_file_query_filesystem_info(fileRoot,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE,nullptr,nullptr);
    mountInfo.lluTotalSize = g_file_info_get_attribute_uint64(info,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE);
    g_object_unref(info);
    g_object_unref(fileRoot);
    // get mount uri
    if (root) {
        mountInfo.isNativeDev = g_file_is_native(root);     //判断设备是本地设备or网络设备
        char *mountUri = g_file_get_uri(root);           //get挂载点的uri路径
        if (mountUri) {
            mountInfo.strUri = mountUri;
            if (g_str_has_prefix(mountUri,"file:///data")) {
                isValidMount = false;
            } else {
                if (mountInfo.strId.empty()) {
                    mountInfo.strId = mountInfo.strUri;
                }
            }
            g_free(mountUri);
        }
        char *tooltip = g_file_get_parse_name(root);      //提示，即文件的解释
        if (tooltip) {
            mountInfo.strTooltip = tooltip;
            g_free(tooltip);
        }
        g_object_unref(root);
    }

    if (driveInfo.strId.empty()) {
       Q_EMIT p_this->telephoneMount();
    }
    bool isNewMount = !(p_this->m_dataFlashDisk->isMountInfoExist(mountInfo));
    if(isValidMount && !g_str_has_prefix(strVolumePath.c_str(),"/dev/sda") && (mountInfo.isCanUnmount || g_str_has_prefix(strVolumePath.c_str(),"/dev/bus")
            || g_str_has_prefix(strVolumePath.c_str(),"/dev/sr"))) {
        qDebug() << "real mount loaded";
        if (!driveInfo.strId.empty()) {
            if (!volumeInfo.strId.empty()) {
                volumeInfo.mountInfo = mountInfo;
                p_this->m_dataFlashDisk->addVolumeInfoWithDrive(driveInfo, volumeInfo);
            } else {
                p_this->m_dataFlashDisk->addMountInfo(mountInfo);
            }
        } else if (!volumeInfo.strId.empty()) {
            volumeInfo.mountInfo = mountInfo;
            p_this->m_dataFlashDisk->addVolumeInfo(volumeInfo);
        } else {
            p_this->m_dataFlashDisk->addMountInfo(mountInfo);
        }
    }
    else
    {
        qDebug()<<"不符合过滤条件的设备已被挂载";
    }

    if(p_this->m_dataFlashDisk->getValidInfoCount() >= 1)
    {
        if (isValidMount && g_str_has_prefix(strVolumePath.c_str(),"/dev/sr") && isNewMount) {
            //qDebug()<<"cd data disk has mounted!";
            Q_EMIT p_this->convertShowWindow();
        }
        p_this->m_systray->show();
    }
    p_this->m_dataFlashDisk->OutputInfos();
}

//when the mountes were uninstalled we reduce mounts number
void MainWindow::mount_removed_callback(GVolumeMonitor *monitor, GMount *mount, MainWindow *p_this)
{
    qDebug()<<mount<<"mount remove";
    FDMountInfo mountInfo;
    mountInfo.isCanEject = g_mount_can_eject(mount);
    char *mountId = g_mount_get_uuid(mount);
    if (mountId) {
        mountInfo.strId = mountId;
        g_free(mountId);
    }
    char *mountName = g_mount_get_name(mount);
    if (mountName) {
        mountInfo.strName = mountName;
        g_free(mountName);
    }
    mountInfo.isCanUnmount = g_mount_can_unmount(mount);
    GFile *root = g_mount_get_default_location(mount);
    if (root) {
        mountInfo.isNativeDev = g_file_is_native(root);     //判断设备是本地设备or网络设备
        char *mountUri = g_file_get_uri(root);           //get挂载点的uri路径
        if (mountUri) {
            mountInfo.strUri = mountUri;
            g_free(mountUri);
            if (mountInfo.strId.empty()) {
                mountInfo.strId = mountInfo.strUri;
            }
        }
        char *tooltip = g_file_get_parse_name(root);      //提示，即文件的解释
        if (tooltip) {
            mountInfo.strTooltip = tooltip;
            g_free(tooltip);
        }
        g_object_unref(root);
    }
    p_this->m_dataFlashDisk->removeMountInfo(mountInfo);

    if(p_this->m_dataFlashDisk->getValidInfoCount() == 0)
    {
        p_this->m_systray->hide();
    }
    p_this->m_dataFlashDisk->OutputInfos();
    Q_EMIT p_this->convertUpdateWindow(QString::fromStdString(mountInfo.strName), 2);     //emit a signal to update the MainMainShow slot
}

//it stands that when you insert a usb device when all the  U disk partitions
void MainWindow::frobnitz_result_func_volume(GVolume *source_object,GAsyncResult *res,MainWindow *p_this)
{
    gboolean success =  FALSE;
    GError *err = nullptr;
    bool bMountSuccess = false;
    success = g_volume_mount_finish (source_object, res, &err);
    if(!err)
    {
        GMount* gmount = g_volume_get_mount(source_object);
        GDrive* gdrive = g_volume_get_drive(source_object);
        FDDriveInfo driveInfo;
        FDVolumeInfo volumeInfo;
        FDMountInfo mountInfo;
        if (gdrive) {
            char *devPath = g_drive_get_identifier(gdrive,G_DRIVE_IDENTIFIER_KIND_UNIX_DEVICE);
            if (devPath != NULL) {
                driveInfo.strId = devPath;
                char *strName = g_drive_get_name(gdrive);
                if (strName) {
                    driveInfo.strName = strName;
                    g_free(strName);
                }
                driveInfo.isCanEject = g_drive_can_eject(gdrive);
                driveInfo.isCanStop = g_drive_can_stop(gdrive);
                driveInfo.isCanStart = g_drive_can_start(gdrive);
                driveInfo.isRemovable = g_drive_is_removable(gdrive);
                g_free(devPath);
            }
            g_object_unref(gdrive);
        }
        char *volumeId = g_volume_get_identifier(source_object,G_VOLUME_IDENTIFIER_KIND_UNIX_DEVICE);
        if (volumeId) {
            volumeInfo.strId = volumeId;
            g_free(volumeId);
            char *volumeName = g_volume_get_name(source_object);
            if (volumeName) {
                volumeInfo.strName = volumeName;
                g_free(volumeName);
            }
            char *strDevName = g_volume_get_identifier(source_object,G_VOLUME_IDENTIFIER_KIND_UNIX_DEVICE);
            if (strDevName) {
                volumeInfo.strDevName = strDevName;
                g_free(strDevName);
            }

            volumeInfo.isCanMount = g_volume_can_mount(source_object);
            volumeInfo.isCanEject = g_volume_can_eject(source_object);
            volumeInfo.isShouldAutoMount = g_volume_should_automount(source_object);
        }
        if (gmount) {
            bMountSuccess = true;
            mountInfo.isCanEject = g_mount_can_eject(gmount);
            char *mountId = g_mount_get_uuid(gmount);
            if (mountId) {
                mountInfo.strId = mountId;
                g_free(mountId);
            }
            char *mountName = g_mount_get_name(gmount);
            if (mountName) {
                mountInfo.strName = mountName;
                g_free(mountName);
            }
            mountInfo.isCanUnmount = g_mount_can_unmount(gmount);
            // get mount total size
            GFile *fileRoot = g_mount_get_root(gmount);
            GFileInfo *info = g_file_query_filesystem_info(fileRoot,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE,nullptr,nullptr);
            mountInfo.lluTotalSize = g_file_info_get_attribute_uint64(info,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE);
            g_object_unref(info);
            g_object_unref(fileRoot);
            // get mount uri
            GFile *root = g_mount_get_default_location(gmount);
            if (root) {
                mountInfo.isNativeDev = g_file_is_native(root);     //判断设备是本地设备or网络设备
                char *mountUri = g_file_get_uri(root);           //get挂载点的uri路径
                if (mountUri) {
                    mountInfo.strUri = mountUri;
                    if (g_str_has_prefix(mountUri,"file:///data")) {
                        bMountSuccess = false;
                    }else {
                        if (mountInfo.strId.empty()) {
                            mountInfo.strId = mountInfo.strUri;
                        }
                    }
                    g_free(mountUri);
                }
                char *tooltip = g_file_get_parse_name(root);      //提示，即文件的解释
                if (tooltip) {
                    mountInfo.strTooltip = tooltip;
                    g_free(tooltip);
                }
                g_object_unref(root);
            }
            g_object_unref(gmount);
        }        
        if (bMountSuccess) {
            if (!driveInfo.strId.empty()) {
                if (!volumeInfo.strId.empty()) {
                    volumeInfo.mountInfo = mountInfo;
                    p_this->m_dataFlashDisk->addVolumeInfoWithDrive(driveInfo, volumeInfo);
                } else {
                    p_this->m_dataFlashDisk->addMountInfo(mountInfo);
                }
            } else if (!volumeInfo.strId.empty()) {
                volumeInfo.mountInfo = mountInfo;
                p_this->m_dataFlashDisk->addVolumeInfo(volumeInfo);
            } else {
                p_this->m_dataFlashDisk->addMountInfo(mountInfo);
            }
            qDebug()<<"sig has emited";
            Q_EMIT p_this->convertShowWindow();     //emit a signal to trigger the MainMainShow slot
        } 
    }
    else
    {
        qDebug()<<"sorry mount failed";
    }
}

//here we begin painting the main interface
void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    insertorclick = false;
    triggerType = 1;  //It represents how we open the interface

    if (ui->centralWidget && !ui->centralWidget->isHidden()) {
        ui->centralWidget->hide();
        return ;
    }

    MainWindowShow();
}

void MainWindow::hideEvent(QHideEvent event)
{
    // delete open_widget;
}


/*
 * newarea use all the information of the U disk to paint the main interface and add line
*/
void MainWindow::newarea(int No,
                         GDrive *Drive,
                         GVolume *Volume,
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
    open_widget = new QClickWidget(ui->centralWidget,No,Drive,Volume,Drivename,nameDis1,nameDis2,nameDis3,nameDis4,
                                   capacityDis1,capacityDis2,capacityDis3,capacityDis4,
                                   pathDis1,pathDis2,pathDis3,pathDis4);
    connect(open_widget,&QClickWidget::clickedConvert,this,[=]()
    {
        ui->centralWidget->hide();
    });
    connect(open_widget,&QClickWidget::noDeviceSig,this,[=]()
    {
        m_systray->hide();
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
}

void MainWindow::newarea(unsigned uDiskNo,
                 QString strDriveId,
                 QString strVolumeId,
                 QString strMountId,
                 QString driveName,
                 QString volumeName,
                 quint64 capacityDis,
                 QString strMountUri,
                 int linestatus)
{
    m_fdClickWidget = new FDClickWidget(ui->centralWidget,uDiskNo,strDriveId,strVolumeId,strMountId,
                                                driveName,volumeName,capacityDis,strMountUri);
    connect(m_fdClickWidget, &FDClickWidget::clickedEjectItem,this,&MainWindow::onClickedEjectItem);

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

    this->vboxlayout->addWidget(m_fdClickWidget);
    vboxlayout->setContentsMargins(2,4,2,4);

    if (linestatus == 0)
    {
        this->vboxlayout->addWidget(line);
    }
}

void MainWindow::moveBottomRight()
{
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
}

void MainWindow::MainWindowShow(bool isUpdate)
{
    this->getTransparentData();
    m_dataFlashDisk->OutputInfos();
    QString strTrans;
    strTrans =  QString::number(m_transparency, 10, 2);
#if (QT_VERSION < QT_VERSION_CHECK(5,7,0))
    QString convertStyle = "#centralWidget{background:rgba(19,19,20,0.95);}";
#else
//    QString convertStyle = "#centralWidget{background:rgba(19,19,20," + strTrans + ");}";
#endif

    if(ui->centralWidget != NULL)
    {
        //cancel the connection between the timeout signal and main interface hiding
        if (insertorclick == false) {
            if (interfaceHideTime->isActive()) {
                interfaceHideTime->stop();
            }
        } else if (!m_bIsMouseInTraIcon && !m_bIsMouseInCentral) {
            if (isUpdate) {
                if (ui->centralWidget->isHidden()) {
                    return ;
                } else {
                    if (m_dataFlashDisk->getValidInfoCount() < 1) {
                        ui->centralWidget->hide();
                        return ;
                    }
                }
            }
            interfaceHideTime->start(5000);
        }
    }
    num = 0;
    unsigned uDiskCount = 0;
    unsigned uVolumeCount = 0;
    QList<FDClickWidget *> listMainWindow = ui->centralWidget->findChildren<FDClickWidget *>();
    for(FDClickWidget *listItem:listMainWindow)
    {
        listItem->deleteLater();
    }

    QList<QWidget *> listLine = ui->centralWidget->findChildren<QWidget *>();
    for(QWidget *listItem:listLine)
    {
        if(listItem->objectName() == "lineWidget") {
            listItem->deleteLater();
        }
    }

    //Convenient interface layout for all drives
    map<string, FDDriveInfo>& listDriveInfo = m_dataFlashDisk->getDevInfoWithDrive();
    if (!listDriveInfo.empty()) {
        map<string, FDDriveInfo>::iterator itDriveInfo = listDriveInfo.begin();
        for ( ;itDriveInfo != listDriveInfo.end(); itDriveInfo++) {
            unsigned uVolumeNum = 0;
            bool isCanShow = (itDriveInfo->second.isCanEject || itDriveInfo->second.isCanStop);
            QString strDriveName = QString::fromStdString(itDriveInfo->second.strName);
            if (itDriveInfo->second.listVolumes.size() > 0) {
                uDiskCount++;
            }
            map<string, FDVolumeInfo>::iterator itVolumeInfo = itDriveInfo->second.listVolumes.begin();
            for ( ;itVolumeInfo != itDriveInfo->second.listVolumes.end(); itVolumeInfo++) {
                QString strApiName;
                quint64 lluTotalSize = itVolumeInfo->second.mountInfo.lluTotalSize;
                QString strMountUri = QString::fromStdString(itVolumeInfo->second.mountInfo.strUri);
                QString strDriveId = QString::fromStdString(itDriveInfo->second.strId);
                QString strVolumeId = QString::fromStdString(itVolumeInfo->second.strId);
                QString strMountId = QString::fromStdString(itVolumeInfo->second.mountInfo.strId);
                unsigned uVolumeType = 0;  // 0:normal file volume, 1: cddata, 2:tele dev
                if (!itVolumeInfo->second.mountInfo.strId.empty()) {
                    string strMountUri = itVolumeInfo->second.mountInfo.strUri;
                    if(g_str_has_prefix(strMountUri.c_str(),"file:///")) {
                        uVolumeType = 0;
                    } else if (g_str_has_prefix(strMountUri.c_str(),"mtp://") || g_str_has_prefix(strMountUri.c_str(),"gphoto2://")){
                        uVolumeType = 2;
                    } else if (g_str_has_prefix(strMountUri.c_str(),"burn:///") || g_str_has_prefix(strMountUri.c_str(),"cdda://")) {
                        uVolumeType = 1;
                    }
                }
                if (uVolumeType == 1 || uVolumeType == 2 || isCanShow) {  // cd module or drive can show
                    if (!itVolumeInfo->second.strId.empty()) {
                        uVolumeNum++;
                        uVolumeCount++;
                        strApiName = QString::fromStdString(itVolumeInfo->second.strName);
                    } else if (!itVolumeInfo->second.mountInfo.strId.empty()) {
                        uVolumeNum++;
                        uVolumeCount++;
                        strApiName = QString::fromStdString(itVolumeInfo->second.mountInfo.strName);
                    } else {
                        continue;
                    }
                } else {
                    continue;
                }
                if (uDiskCount > 0) {
                    if (uVolumeType == 1) {   // deal with cd info
                        if(uDiskCount == 1 || uVolumeNum != 1) {
                            newarea(uVolumeNum, strDriveId,strVolumeId,strMountId,strDriveName,
                                    strApiName, 1, "burn:///", 1);
                        } else {
                            newarea(uVolumeNum, strDriveId,strVolumeId,strMountId,strDriveName,
                                    strApiName, 1, "burn:///", 2);
                        }
                    } else if (uVolumeType == 2) {
                        QString telephoneName = tr("telephone device");
                        QByteArray strTelePhone = telephoneName.toLocal8Bit();
                        char *realTele = strTelePhone.data();
                        if(uDiskCount == 1 || uVolumeNum != 1) {
                            newarea(uVolumeNum, strDriveId,strVolumeId,strMountId,realTele,
                                    strApiName, lluTotalSize, strMountUri,1);
                        } else {
                            newarea(uVolumeNum, strDriveId,strVolumeId,strMountId,realTele,
                                    strApiName, lluTotalSize, strMountUri,2);
                        }
                    } else {
                        if(uDiskCount == 1 || uVolumeNum != 1) {
                            newarea(uVolumeNum, strDriveId,strVolumeId,strMountId,strDriveName,
                                    strApiName, lluTotalSize,strMountUri,1);
                        } else {
                            newarea(uVolumeNum, strDriveId,strVolumeId,strMountId,strDriveName,
                                    strApiName,lluTotalSize,strMountUri,2);
                        }
                    }
                }
            }
        }
    }
    // show volume info without drive
    map<string, FDVolumeInfo>& listVolumeInfo = m_dataFlashDisk->getDevInfoWithVolume();
    if (!listVolumeInfo.empty()) {
        map<string, FDVolumeInfo>::iterator itVolumeInfo = listVolumeInfo.begin();
        for (; itVolumeInfo != listVolumeInfo.end(); itVolumeInfo++) {
            QString strApiName;
            QString strMainName;
            bool isCanShow = itVolumeInfo->second.isCanEject;
            quint64 lluTotalSize = itVolumeInfo->second.mountInfo.lluTotalSize;
            QString strMountUri = QString::fromStdString(itVolumeInfo->second.mountInfo.strUri);
            QString strVolumeId = QString::fromStdString(itVolumeInfo->second.strId);
            QString strMountId = QString::fromStdString(itVolumeInfo->second.mountInfo.strId);
            unsigned uVolumeType = 0;  // 0:normal file volume, 1: cddata, 2:tele dev
            if (!itVolumeInfo->second.mountInfo.strId.empty()) {
                string strMountUri = itVolumeInfo->second.mountInfo.strUri;
                if(g_str_has_prefix(strMountUri.c_str(),"file:///")) {
                    uVolumeType = 0;
                } else if (g_str_has_prefix(strMountUri.c_str(),"mtp://") || g_str_has_prefix(strMountUri.c_str(),"gphoto2://")){
                    uVolumeType = 2;
                } else if (g_str_has_prefix(strMountUri.c_str(),"burn:///") || g_str_has_prefix(strMountUri.c_str(),"cdda://")) {
                    uVolumeType = 1;
                }
            }
            if (uVolumeType == 1 || uVolumeType == 2 || isCanShow) {  // cd module or drive can show
                if (!itVolumeInfo->second.strId.empty()) {
                    uDiskCount++;
                    uVolumeCount++;
                    strMainName = strApiName = QString::fromStdString(itVolumeInfo->second.strName);
                } else if (!itVolumeInfo->second.mountInfo.strId.empty()) {
                    uDiskCount++;
                    uVolumeCount++;
                    strMainName = strApiName = QString::fromStdString(itVolumeInfo->second.mountInfo.strName);
                } else {
                    continue;
                }
            } else {
                continue;
            }
            if (uDiskCount > 0) {
                if (uVolumeType == 1) {   // deal with cd info
                    if(uDiskCount == 1) {
                        newarea(1, "",strVolumeId,strMountId,strMainName,
                                strApiName,1,"burn:///",1);
                    } else {
                        newarea(1, "",strVolumeId,strMountId,strMainName,
                                strApiName,1,"burn:///",2);
                    }
                } else if (uVolumeType == 2) {
                    QString telephoneName = tr("telephone device");
                    QByteArray strTelePhone = telephoneName.toLocal8Bit();
                    char *realTele = strTelePhone.data();
                    if(uDiskCount == 1) {
                        newarea(1, "",strVolumeId,strMountId,realTele,
                                strApiName,lluTotalSize,strMountUri,1);
                    } else {
                        newarea(1, "",strVolumeId,strMountId,realTele,
                                strApiName,lluTotalSize,strMountUri,2);
                    }
                } else {
                    if(uDiskCount == 1) {
                        newarea(1, "",strVolumeId,strMountId,strMainName,
                                strApiName,lluTotalSize,strMountUri,1);
                    } else {
                        newarea(1, "",strVolumeId,strMountId,strMainName,
                                strApiName,lluTotalSize,strMountUri,2);
                    }
                }
            }
        }
    }
    // show mount info without drive & volume
    map<string, FDMountInfo>& listMountInfo = m_dataFlashDisk->getDevInfoWithMount();
    if (!listMountInfo.empty()) {
        map<string, FDMountInfo>::iterator itMountInfo = listMountInfo.begin();
        for (; itMountInfo != listMountInfo.end(); itMountInfo++) {
            QString strApiName;
            QString strMainName;
            bool isCanShow = (itMountInfo->second.isCanUnmount || itMountInfo->second.isCanEject);
            quint64 lluTotalSize = itMountInfo->second.lluTotalSize;
            QString strMountUri = QString::fromStdString(itMountInfo->second.strUri);
            QString strMountId = QString::fromStdString(itMountInfo->second.strId);
            unsigned uMountType = 0;  // 0:normal file, 1: cddata, 2:tele dev
            if (!itMountInfo->second.strId.empty()) {
                string strMountUri = itMountInfo->second.strUri;
                if(g_str_has_prefix(strMountUri.c_str(),"file:///")) {
                    uMountType = 0;
                } else if (g_str_has_prefix(strMountUri.c_str(),"mtp://") || g_str_has_prefix(strMountUri.c_str(),"gphoto2://")){
                    uMountType = 2;
                } else if (g_str_has_prefix(strMountUri.c_str(),"burn:///") || g_str_has_prefix(strMountUri.c_str(),"cdda://")) {
                    uMountType = 1;
                }
            } else {
                continue;
            }
            if (uMountType == 1 || uMountType == 2 || isCanShow) {  // cd module or drive can show
                uDiskCount++;
                uVolumeCount++;
                strMainName = strApiName = QString::fromStdString(itMountInfo->second.strName);
            } else {
                continue;
            }
            if (uDiskCount > 0) {
                if (uMountType == 1) {   // deal with cd info
                    if(uDiskCount == 1) {
                        newarea(1, "","",strMountId,strMainName,
                                strApiName,1, "burn:///", 1);
                    } else {
                        newarea(1, "","",strMountId,strMainName,
                                strApiName,1, "burn:///", 2);
                    }
                } else if (uMountType == 2) {
                    QString telephoneName = tr("telephone device");
                    QByteArray strTelePhone = telephoneName.toLocal8Bit();
                    char *realTele = strTelePhone.data();
                    if(uDiskCount == 1) {
                        newarea(1, "","",strMountId,realTele,
                                strApiName,lluTotalSize,strMountUri,1);
                    } else {
                        newarea(1, "","",strMountId,realTele,
                                strApiName,lluTotalSize,strMountUri,2);
                    }
                } else {
                    if(uDiskCount == 1) {
                        newarea(1, "","",strMountId,strMainName,
                                strApiName,lluTotalSize,strMountUri,1);
                    } else {
                        newarea(1, "","",strMountId,strMainName,
                                strApiName,lluTotalSize,strMountUri,2);
                    }
                }
            }
        }
    }

    if (uDiskCount > 0) {
        uVolumeCount = uVolumeCount < uDiskCount ? uDiskCount : uVolumeCount;
        hign = uDiskCount*FLASHDISKITEM_TITLE_HEIGHT+uVolumeCount*FLASHDISKITEM_CONTENT_HEIGHT;
        ui->centralWidget->setFixedSize(280, hign);
    } else {
        hign = 0;
        ui->centralWidget->setFixedSize(0, 0);
    }

    moveBottomNoBase();
    ui->centralWidget->show();
}

void MainWindow::ifgetPinitMount()
{
    int pointMountNum = 0;
    QFile file(tmpPath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QString content = file.readLine().trimmed();
        while (!file.atEnd())
        {
            if (content.contains(".mount"))
                pointMountNum += 1;
                content = file.readLine().trimmed();
                if(pointMountNum >= 1)
                    findPointMount = true;
                else
                    findPointMount = false;
        }
    }
    file.close();
}

void MainWindow::onMaininterfacehide()
{
    ui->centralWidget->hide();
    this->driveVolumeNum = 0;
    interfaceHideTime->stop();
}

void MainWindow::moveBottomNoBase()
{
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
        ui->centralWidget->setGeometry(QRect(x + QApplication::primaryScreen()->geometry().width()-ui->centralWidget->width() - DISTANCEMEND - DISTANCEPADDING,y+ QApplication::primaryScreen()->geometry().height()-panelSize-ui->centralWidget->height() - DISTANCEPADDING,ui->centralWidget->width(),ui->centralWidget->height()));
    else if(position==1)
        ui->centralWidget->setGeometry(QRect(x + QApplication::primaryScreen()->geometry().width()-ui->centralWidget->width() - DISTANCEMEND - DISTANCEPADDING,y+ panelSize + DISTANCEPADDING,ui->centralWidget->width(),ui->centralWidget->height()));  // Style::minw,Style::minh the width and the height of the interface  which you want to show
    else if(position==2)
        ui->centralWidget->setGeometry(QRect(x+panelSize + DISTANCEPADDING,y + QApplication::primaryScreen()->geometry().height() - ui->centralWidget->height() - DISTANCEPADDING,ui->centralWidget->width(),ui->centralWidget->height()));
    else
        ui->centralWidget->setGeometry(QRect(x+QApplication::primaryScreen()->geometry().width()-panelSize-ui->centralWidget->width() - DISTANCEPADDING,y + QApplication::primaryScreen()->geometry().height() - ui->centralWidget->height() - DISTANCEPADDING,ui->centralWidget->width(),ui->centralWidget->height()));
    ui->centralWidget->repaint();
}

/*
 * determine how to open the maininterface,if trigger is 0,the main interface show when we inset USB
 * device directly,if trigger is 1,we show main interface by clicking the systray icon.
*/
bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    #if 0
    if(triggerType == 0)
    {
        if(event->type() == QEvent::Enter)
        {
            disconnect(interfaceHideTime, SIGNAL(timeout()), this, SLOT(onMaininterfacehide()));
            this->show();
        }

        if(event->type() == QEvent::Leave)
        {
            connect(interfaceHideTime, SIGNAL(timeout()), this, SLOT(onMaininterfacehide()));
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
    }
    if (!isActiveWindow())
    {
        activateWindow();
    }
    #endif
    return false;
}

//new a gsettings object to get the information of the opacity of the window
void MainWindow::initTransparentState()
{
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
    QPainterPath path;
    auto rect = this->rect();
    rect.adjust(1, 1, -1, -1);
    path.addRoundedRect(rect, 4, 4);
    setProperty("blurRegion", QRegion(path.toFillPolygon().toPolygon()));

    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    QRect rectReal = this->rect();
    p.setRenderHint(QPainter::Antialiasing);  // 反锯齿;
    p.setBrush(opt.palette.color(QPalette::Base));
    p.setOpacity(m_transparency);
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(rectReal, 4, 4);
    QWidget::paintEvent(event);

    KWindowEffects::enableBlurBehind(this->winId(), true, QRegion(path.toFillPolygon().toPolygon()));
}

void MainWindow::onClickedEjectItem(FDClickWidget* pThis, QString strDriveId, QString strVolumeId, QString strMountId)
{
    // find the device's drive & volume & mount
    GList *lDrive = NULL;
    GList *lVolume = NULL;
    GList *lMount = NULL;
    GList *current_drive_list = NULL;
    GList *current_volume_list = NULL;
    GList *current_mount_list = NULL;
    GDrive* devDrive = NULL;
    GVolume* devVolume = NULL;
    GMount* devMount = NULL;
    unsigned uVolumeSize = 0;
    bool isDone = false;
    //about drive
    GVolumeMonitor *g_volume_monitor = g_volume_monitor_get();
    if (!strDriveId.isEmpty()) {
        current_drive_list = g_volume_monitor_get_connected_drives(g_volume_monitor);
        for (lDrive = current_drive_list; lDrive != NULL; lDrive = lDrive->next) {
            GDrive *gdrive = (GDrive *)lDrive->data;
            char *devPath = g_drive_get_identifier(gdrive,G_DRIVE_IDENTIFIER_KIND_UNIX_DEVICE);
            if (devPath != NULL) {
                if(strDriveId == devPath) {
                    devDrive = gdrive;
                    current_volume_list = g_drive_get_volumes(gdrive);
                    if (current_volume_list) {
                        uVolumeSize = g_list_length(current_volume_list);
                        for(lVolume = current_volume_list; lVolume != NULL; lVolume = lVolume->next){ //遍历驱动器上的所有卷设备
                            GVolume* volume = (GVolume *)lVolume->data;
                            char *volumeId = g_volume_get_identifier(volume,G_VOLUME_IDENTIFIER_KIND_UNIX_DEVICE);
                            if (volumeId) {
                                if (strVolumeId != volumeId) {
                                    g_free(volumeId);
                                    continue;
                                }
                                g_free(volumeId);
                            } else {
                                continue ;
                            }
                            devVolume = volume;
                            break;
                        }
                    }
                }
                g_free(devPath);
            }
        }
        if (devDrive != NULL) {
            if (devDrive != NULL) {
                g_drive_eject_with_operation(devDrive,
                        G_MOUNT_UNMOUNT_NONE,
                        NULL,
                        NULL,
                        GAsyncReadyCallback(&MainWindow::frobnitz_result_func),
                        this);
                isDone = true;
            }
        }
        if (current_volume_list) {
            g_list_free(current_volume_list);
            current_volume_list = NULL;
        }
        if (current_drive_list) {
            g_list_free(current_drive_list);
            current_drive_list = NULL;
        }
    }
    //about volume not associated with a drive
    if (!isDone && !strVolumeId.isEmpty()) {
        current_volume_list = g_volume_monitor_get_volumes(g_volume_monitor);
        if (current_volume_list) {
            for (lVolume = current_volume_list; lVolume != NULL; lVolume = lVolume->next) {
                GVolume *volume = (GVolume *)lVolume->data;
                GDrive *gdrive = g_volume_get_drive(volume);
                if (!gdrive) {
                    char *volumeId = g_volume_get_identifier(volume,G_VOLUME_IDENTIFIER_KIND_UNIX_DEVICE);
                    if (volumeId) {
                        if (strVolumeId != volumeId) {
                            g_free(volumeId);
                            continue;
                        }
                        g_free(volumeId);
                    } else {
                        continue ;
                    }
                    devVolume = volume;
                    break;
                } else {
                    g_object_unref(gdrive);
                }
            }
        }
        if (devVolume != NULL) {
            GMount* pCurMount = g_volume_get_mount(devVolume);
            if (pCurMount) {
                g_mount_unmount_with_operation(pCurMount,
                        G_MOUNT_UNMOUNT_NONE,
                        NULL,
                        NULL,
                        GAsyncReadyCallback(&MainWindow::frobnitz_force_result_unmount),
                        this);
                g_object_unref(pCurMount);
                isDone = true;
            }
        }
        if (current_volume_list) {
            g_list_free(current_volume_list);
            current_volume_list = NULL;
        }
    }
    //about mount not associated with a volume
    if (!isDone && !strMountId.isEmpty()) {
        current_mount_list = g_volume_monitor_get_mounts(g_volume_monitor);
        if (current_mount_list) {
            for (lMount = current_mount_list; lMount != NULL; lMount = lMount->next) {
                GMount *gmount = (GMount *)lMount->data;
                GVolume *gvolume = g_mount_get_volume(gmount);
                if (!gvolume) {
                    QString strId = "";
                    char *mountId = g_mount_get_uuid(gmount);
                    if (mountId) {
                        strId = mountId;
                        g_free(mountId);
                    }
                    // get mount uri
                    GFile *root = g_mount_get_default_location(gmount);
                    if (root) {
                        char *mountUri = g_file_get_uri(root);           //get挂载点的uri路径
                        if (mountUri) {
                            if (strId.isEmpty()) {
                                strId = mountUri;
                            }
                            g_free(mountUri);
                        }
                        g_object_unref(root);
                    }
                    if (strMountId == strId) {
                        devMount = gmount;
                        break;
                    }
                } else {
                    g_object_unref(gvolume);
                }
            }
            if (devMount != NULL) {
                g_mount_unmount_with_operation(devMount,
                        G_MOUNT_UNMOUNT_NONE,
                        NULL,
                        NULL,
                        GAsyncReadyCallback(&MainWindow::frobnitz_force_result_unmount),
                        this);
                isDone = true;
            }
            g_list_free(current_mount_list);
            current_mount_list = NULL;
        }
    }
    if (!isDone) {
        FDMountInfo mountInfo;
        FDVolumeInfo volumeInfo;
        FDDriveInfo driveInfo;
        mountInfo.strId = strMountId.toStdString();
        m_dataFlashDisk->removeMountInfo(mountInfo);
        volumeInfo.strId = strVolumeId.toStdString();
        m_dataFlashDisk->removeVolumeInfo(volumeInfo);
        driveInfo.strId = strDriveId.toStdString();
        m_dataFlashDisk->removeDriveInfo(driveInfo);
        if(m_dataFlashDisk->getValidInfoCount() == 0)
        {
            m_systray->hide();
        }
    }
    ui->centralWidget->hide();
}

void MainWindow::AsyncUnmount(QString strMountRoot,MainWindow *p_this) 
{
    qDebug()<<"dataPath:"<<strMountRoot;
    QProcess p;
    p.setProgram("pkexec");
    p.setArguments(QStringList()<<"eject"<<strMountRoot);
    p.start();
    bool bSuccess = p.waitForFinished();
    if (p_this)
        p_this->ifSucess = bSuccess;
}

void MainWindow::frobnitz_force_result_func(GDrive *source_object,GAsyncResult *res,MainWindow *p_this)
{
    auto env = qgetenv("QT_QPA_PLATFORMTHEME");
    gboolean success =  FALSE;
    GError *err = nullptr;
    success = g_drive_eject_with_operation_finish (source_object, res, &err);
    if (!err) {
        FDDriveInfo driveInfo;
        char *devPath = g_drive_get_identifier(source_object,G_DRIVE_IDENTIFIER_KIND_UNIX_DEVICE);
        if (devPath != NULL) {
            driveInfo.strId = devPath;
            g_free(devPath);
        }
        char *strName = g_drive_get_name(source_object);
        if (strName) {
            driveInfo.strName = strName;
            g_free(strName);
        }
        FlashDiskData::getInstance()->removeDriveInfo(driveInfo);
        if(FlashDiskData::getInstance()->getValidInfoCount() == 0)
        {
            p_this->m_systray->hide();
        }
        p_this->m_eject = new ejectInterface(p_this->ui->centralWidget,QString::fromStdString(driveInfo.strName),NORMALDEVICE);
        p_this->m_eject->show();
    } else {
        GList* listVolume = g_drive_get_volumes(source_object);
        if (listVolume) {
            GList* lVolume = NULL;
            for (lVolume = listVolume; lVolume != NULL; lVolume= lVolume->next) {
                GVolume* gvolume = (GVolume *)lVolume->data;
                GMount* gmount = g_volume_get_mount(gvolume);
                if (gmount) {
                    GFile* root = g_mount_get_root(gmount);
                    if (root) {
                        char* strMountRoot = g_file_get_path(root);
                        if (strMountRoot) {
                            QString strMountPath = strMountRoot;
                            QtConcurrent::run(&MainWindow::AsyncUnmount, strMountPath, p_this);
                            g_free(strMountRoot);
                        }
                        g_object_unref(root);
                    }
                    g_object_unref(gmount);
                }
            }
            g_list_free(listVolume);
            listVolume = NULL;
        }
    }
}

void MainWindow::frobnitz_result_func(GDrive *source_object,GAsyncResult *res,MainWindow *p_this)
{
    gboolean success =  FALSE;
    GError *err = nullptr;
    success = g_drive_eject_with_operation_finish (source_object, res, &err);

    if (!err) {
        FDDriveInfo driveInfo;
        char *devPath = g_drive_get_identifier(source_object,G_DRIVE_IDENTIFIER_KIND_UNIX_DEVICE);
        if (devPath != NULL) {
            driveInfo.strId = devPath;
            g_free(devPath);
        }
        char *strName = g_drive_get_name(source_object);
        if (strName) {
            driveInfo.strName = strName;
            g_free(strName);
        }
        FlashDiskData::getInstance()->removeDriveInfo(driveInfo);
        if(FlashDiskData::getInstance()->getValidInfoCount() == 0)
        {
            p_this->m_systray->hide();
        }
        p_this->m_eject = new ejectInterface(p_this->ui->centralWidget,QString::fromStdString(driveInfo.strName),NORMALDEVICE);
        p_this->m_eject->show();
    } else /*if(g_drive_can_stop(source_object) == true)*/ {
        if (p_this->chooseDialog == nullptr)
        {
            p_this->chooseDialog = new interactiveDialog(p_this->ui->centralWidget);
        }
        p_this->chooseDialog->show();
        p_this->chooseDialog->setFocus();
        p_this->connect(p_this->chooseDialog,&interactiveDialog::FORCESIG,p_this,[=]()
        {
            g_drive_eject_with_operation(source_object,
                                         G_MOUNT_UNMOUNT_FORCE,
                                         NULL,
                                         NULL,
                                         GAsyncReadyCallback(&MainWindow::frobnitz_force_result_func),
                                         p_this
                                         );
            p_this->chooseDialog->close();
        });
    }
}

void MainWindow::frobnitz_normal_result_volume_eject(GVolume *source_object,GAsyncResult *res,MainWindow *p_this)
{
    gboolean success =  FALSE;
    GError *err = nullptr;
    success = g_volume_eject_with_operation_finish(source_object, res, &err);
    if(!err)
    {
        FDVolumeInfo volumeInfo;
        char *volumeId = g_volume_get_identifier(source_object,G_VOLUME_IDENTIFIER_KIND_UNIX_DEVICE);
        if (volumeId) {
            volumeInfo.strId = volumeId;
            g_free(volumeId);
        }
        FlashDiskData::getInstance()->removeVolumeInfo(volumeInfo);
        if(FlashDiskData::getInstance()->getValidInfoCount() == 0)
        {
            p_this->m_systray->hide();
        }
    }
}

void MainWindow::frobnitz_force_result_unmount(GMount *source_object,GAsyncResult *res,MainWindow *p_this)
{
    gboolean success =  FALSE;
    GError *err = nullptr;
    success = g_mount_unmount_with_operation_finish(source_object,res, &err);
    if(!err)
    {
        FDMountInfo mountInfo;
        mountInfo.isCanEject = g_mount_can_eject(source_object);
        char *mountId = g_mount_get_uuid(source_object);
        if (mountId) {
            mountInfo.strId = mountId;
            g_free(mountId);
        }
        GFile *root = g_mount_get_default_location(source_object);
        if (root) {
            char *mountUri = g_file_get_uri(root);           //get挂载点的uri路径
            if (mountUri) {
                mountInfo.strUri = mountUri;
                g_free(mountUri);
                if (mountInfo.strId.empty()) {
                    mountInfo.strId = mountInfo.strUri;
                }
            }
            g_object_unref(root);
        }
        FlashDiskData::getInstance()->removeMountInfo(mountInfo);
        if(FlashDiskData::getInstance()->getValidInfoCount() == 0)
        {
            p_this->m_systray->hide();
        }
    }
}