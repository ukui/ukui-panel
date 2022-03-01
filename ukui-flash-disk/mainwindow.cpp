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
#include <QDateTime>
#include <stdio.h>
#include <string.h>
#include "clickLabel.h"
#include "MacroFile.h"
#include "datacdrom.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
  , ui(new Ui::MainWindow)
  , findPointMount(false)
  , telephoneNum(0)
  , driveVolumeNum(0)
  , m_strSysRootDev(QString(""))
{
    ui->setupUi(this);

    initThemeMode();
    initFlashDisk();
    initSlots();
    installEventFilter(this);

    // get system root device
    getSystemRootDev();
    // underlying code to get the information of the usb device
    getDeviceInfo();
}

MainWindow::~MainWindow()
{
    delete ui;
    if (m_transparency_gsettings) {
        delete m_transparency_gsettings;
        m_transparency_gsettings = nullptr;
    }

    if (ifsettings) {
        delete ifsettings;
        ifsettings = nullptr;
    }
}

void MainWindow::initFlashDisk()
{
    m_vtDeviveId.clear();

    // 框架的样式设置
    // set the style of the framework
    interfaceHideTime = new QTimer(this);

    initTransparentState();
    ui->centralWidget->setObjectName("centralWidget");
    vboxlayout = new QVBoxLayout(this);
    ui->centralWidget->setLayout(vboxlayout);

#if (QT_VERSION < QT_VERSION_CHECK(5,7,0))
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::X11BypassWindowManagerHint);
#else
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Popup);
#endif
    this->setAttribute(Qt::WA_TranslucentBackground);

    m_systray = new QSystemTrayIcon(this);
    m_systray->setIcon(QIcon::fromTheme("media-removable-symbolic"));
    //m_systray->setVisible(true);
    m_systray->setToolTip(tr("usb management tool"));

    // init the screen
    screen = qApp->primaryScreen();

    m_nAppStartTimestamp = QDateTime::currentDateTime().toMSecsSinceEpoch();
    m_dataFlashDisk = FlashDiskData::getInstance();
}

void MainWindow::initSlots()
{
    connect(m_systray, &QSystemTrayIcon::activated, this, &MainWindow::iconActivated);
    connect(this, &MainWindow::convertShowWindow, this, &MainWindow::onConvertShowWindow);
    connect(this, &MainWindow::convertUpdateWindow, this, &MainWindow::onConvertUpdateWindow);
    connect(this, &MainWindow::remountVolume, this, &MainWindow::onRemountVolume);
    connect(this, &MainWindow::checkDriveValid, this, &MainWindow::onCheckDriveValid);
    connect(this, &MainWindow::notifyDeviceRemoved, this, &MainWindow::onNotifyDeviceRemoved);
    connect(m_dataFlashDisk, &FlashDiskData::notifyDeviceRemoved, this, &MainWindow::onNotifyDeviceRemoved);

    QDBusConnection::sessionBus().connect(QString(), QString("/taskbar/click"), \
                                                  "com.ukui.panel.plugins.taskbar", "sendToUkuiDEApp", this, SLOT(on_clickPanelToHideInterface()));

    connect(this, SIGNAL(deviceError(GDrive*)), this, SLOT(onDeviceErrored(GDrive*)), (Qt::UniqueConnection));
    connect(this, SIGNAL(mountVolume(GVolume*)), this, SLOT(onMountVolume(GVolume*)));
    connect(this, SIGNAL(ejectVolumeForce(GVolume*)), this, SLOT(onEjectVolumeForce(GVolume*)));
    connect(interfaceHideTime, SIGNAL(timeout()), this, SLOT(onMaininterfacehide()));
}

void MainWindow::onRequestSendDesktopNotify(QString message, QString strIcon)
{
    QDBusInterface iface("org.freedesktop.Notifications",
                         "/org/freedesktop/Notifications",
                         "org.freedesktop.Notifications",
                         QDBusConnection::sessionBus());
    QList<QVariant> args;
    args << (tr("ukui flash disk"))
         << ((unsigned int) 0)
         << strIcon
         << tr("kindly reminder") //显示的是什么类型的信息
         << message //显示的具体信息
         << QStringList()
         << QVariantMap()
         << (int)-1;
    iface.callWithArgumentList(QDBus::NoBlock,"Notify",args);
}

void MainWindow::onInsertAbnormalDiskNotify(QString message)
{
    QDBusInterface iface("org.freedesktop.Notifications",
                         "/org/freedesktop/Notifications",
                         "org.freedesktop.Notifications",
                         QDBusConnection::sessionBus());
    QList<QVariant> args;
    args << (tr("ukui flash disk"))
         << ((unsigned int) 0)
         << QString("media-removable-symbolic")
         << tr("wrong reminder") //显示的是什么类型的信息
         << message //显示的具体信息
         << QStringList()
         << QVariantMap()
         << (int)-1;
    iface.callWithArgumentList(QDBus::NoBlock,"Notify",args);
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
    const QByteArray idd(THEME_QT_SCHEMA);

    if(QGSettings::isSchemaInstalled(idd)) {
        QGSettings *qtSettings = new QGSettings(idd, QByteArray(), this);

        connect(qtSettings,&QGSettings::changed,this,[=](const QString &key) {
            currentThemeMode = qtSettings->get(MODE_QT_KEY).toString();
        });
        currentThemeMode = qtSettings->get(MODE_QT_KEY).toString();
    }

    const QByteArray id(AUTOLOAD);
    if(QGSettings::isSchemaInstalled(idd)) {
        ifsettings = new QGSettings(id);
    }
}

void MainWindow::on_clickPanelToHideInterface()
{
    if(!ui->centralWidget->isHidden()) {
        ui->centralWidget->hide();
    }
}

void MainWindow::getSystemRootDev()
{
    QString cmd = "df -l";
    QProcess *p = new QProcess();
    p->start(cmd);
    p->waitForFinished();
    while(p->canReadLine()) {
        QString str = p->readLine();
        QStringList infoList = str.split(QRegExp("\\s+"));
        if (infoList.size() >= 6) {
            if (infoList[5] == "/") {
                m_strSysRootDev = infoList[0];
                break;
            }
        }
    }
    delete p;
    p = nullptr;
}

bool MainWindow::isSystemRootDev(QString strDev)
{
    return m_strSysRootDev.startsWith(strDev);
}

bool MainWindow::getDevInterface(QString strDev)
{

    QString cmd = "udevadm info -n ";
    cmd.append(strDev);
    QProcess *p = new QProcess();
    p->start(cmd);
    p->waitForFinished();
    while(p->canReadLine()) {
        QString str = p->readLine();
        QStringList infoList = str.split('\n');
        QStringList::Iterator it = infoList.begin();
        for (; it != infoList.end(); it++){
            if(*it==("E: ID_BUS=ata")){
            return false;
            }

        }
    }
    delete p;
    p = nullptr;
    return true;
}

void MainWindow::getDeviceInfo()
{
    // setting
    FILE *fp = NULL;
    int a = 0;
    char buf[128] = {0};

    fp = fopen("/proc/cmdline","r");
    if(fp) {
        while(fscanf(fp,"%127s",buf) >0 ) {
            if(strcmp(buf,"live") == 0) {
                a++;
            }
        }
        fclose(fp);
    }
    if(a > 0) {
        QProcess::startDetached("gsettings set org.ukui.flash-disk.autoload ifautoload false");
    }

    // callback function that to monitor the insertion and removal of the underlying equipment
    GVolumeMonitor *g_volume_monitor = g_volume_monitor_get();
    g_signal_connect (g_volume_monitor, "drive-connected", G_CALLBACK (drive_connected_callback), this);
    g_signal_connect (g_volume_monitor, "drive-disconnected", G_CALLBACK (drive_disconnected_callback), this);
    g_signal_connect (g_volume_monitor, "volume-added", G_CALLBACK (volume_added_callback), this);
    g_signal_connect (g_volume_monitor, "volume-removed", G_CALLBACK (volume_removed_callback), this);
    g_signal_connect (g_volume_monitor, "mount-added", G_CALLBACK (mount_added_callback), this);
    g_signal_connect (g_volume_monitor, "mount-removed", G_CALLBACK (mount_removed_callback), this);

    GList *lDrive = NULL, *lVolume = NULL, *lMount = NULL;
    // about drive
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
            getDriveIconsInfo(gdrive, driveInfo);
            driveInfo.isCanEject = g_drive_can_eject(gdrive);
            driveInfo.isCanStop = g_drive_can_stop(gdrive);
            driveInfo.isCanStart = g_drive_can_start(gdrive);
            driveInfo.isRemovable = g_drive_is_removable(gdrive);
            if(!isSystemRootDev(driveInfo.strId.c_str()) && getDevInterface(driveInfo.strId.c_str()) && (driveInfo.isCanEject || driveInfo.isCanStop || driveInfo.isRemovable)) {
                if(g_str_has_prefix(devPath,"/dev/sr") || g_str_has_prefix(devPath,"/dev/bus") || g_str_has_prefix(devPath,"/dev/sd")
                    || g_str_has_prefix(devPath,"/dev/mmcblk")) {
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
                            getVolumeIconsInfo(volume, volumeInfo);
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
                                    if (fileRoot) {
                                        GFileInfo *info = g_file_query_filesystem_info(fileRoot,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE,nullptr,nullptr);
                                        if (info) {
                                            volumeInfo.mountInfo.lluTotalSize = g_file_info_get_attribute_uint64(info,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE);
                                            g_object_unref(info);
                                        }
                                        g_object_unref(fileRoot);
                                    }
                                    getDataCDRomCapacity(volumeInfo.strId.c_str(),volumeInfo.mountInfo.lluTotalSize);
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
                                    getMountIconsInfo(mount, volumeInfo.mountInfo);
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
            g_free(devPath);
        }
    }
    if (current_drive_list) {
        g_list_free(current_drive_list);
    }
    // about volume not associated with a drive
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
                    if (!(g_str_has_prefix(devPath,"/dev/sr") || g_str_has_prefix(devPath,"/dev/bus") || g_str_has_prefix(devPath,"/dev/sd")
                        || g_str_has_prefix(devPath,"/dev/mmcblk"))) {
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
                getVolumeIconsInfo(volume, volumeInfo);
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
                        if (fileRoot) {
                            GFileInfo *info = g_file_query_filesystem_info(fileRoot,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE,nullptr,nullptr);
                            if (info) {
                                volumeInfo.mountInfo.lluTotalSize = g_file_info_get_attribute_uint64(info,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE);
                                g_object_unref(info);
                            }
                            g_object_unref(fileRoot);
                        }
                        getDataCDRomCapacity(volumeInfo.strId.c_str(),volumeInfo.mountInfo.lluTotalSize);
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
                        getMountIconsInfo(mount, volumeInfo.mountInfo);
                    }
                    g_object_unref(mount);
                } else {
                    if (volumeInfo.isCanMount) {
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

    // about mount not associated with a volume
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
                        getDriveIconsInfo(gdrive, driveInfo);
                        driveInfo.isCanEject = g_drive_can_eject(gdrive);
                        driveInfo.isCanStop = g_drive_can_stop(gdrive);
                        driveInfo.isCanStart = g_drive_can_start(gdrive);
                        driveInfo.isRemovable = g_drive_is_removable(gdrive);
                        if(!isSystemRootDev(driveInfo.strId.c_str()) && (driveInfo.isCanEject || driveInfo.isCanStop || driveInfo.isRemovable)) {
                            if(g_str_has_prefix(devPath,"/dev/sr") || g_str_has_prefix(devPath,"/dev/bus") || g_str_has_prefix(devPath,"/dev/sd")
                                || g_str_has_prefix(devPath,"/dev/mmcblk")) {
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
                                    if (fileRoot) {
                                        GFileInfo *info = g_file_query_filesystem_info(fileRoot,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE,nullptr,nullptr);
                                        if (info) {
                                            mountInfo.lluTotalSize = g_file_info_get_attribute_uint64(info,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE);
                                            g_object_unref(info);
                                        }
                                        g_object_unref(fileRoot);
                                    }
                                    getDataCDRomCapacity(driveInfo.strId.c_str(),mountInfo.lluTotalSize);
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
                                    getMountIconsInfo(gmount, mountInfo);
                                    if (isValidMount) {
                                        FDVolumeInfo volumeInfo;
                                        volumeInfo.mountInfo = mountInfo;
                                        m_dataFlashDisk->addMountInfoWithDrive(driveInfo, volumeInfo, mountInfo);
                                    }
                                }
                            }
                        }
                        g_free(devPath);
                    }
                    g_object_unref(gdrive);
                } else {
                    # if 0
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
                        if (fileRoot) {
                            GFileInfo *info = g_file_query_filesystem_info(fileRoot,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE,nullptr,nullptr);
                            if (info) {
                                mountInfo.lluTotalSize = g_file_info_get_attribute_uint64(info,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE);
                                g_object_unref(info);
                            }
                            g_object_unref(fileRoot);
                        }
                        getDataCDRomCapacity(mountInfo.strId.c_str(),mountInfo.lluTotalSize);
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
                    #endif
                }
            } else {
                g_object_unref(gvolume);
            }
        }
        g_list_free(current_mount_list);
    }

    // determine the systray icon should be showed  or be hieded
    if(m_dataFlashDisk->getValidInfoCount() >= 1) {
        m_systray->show();
    } else {
        m_systray->hide();
    }
    m_dataFlashDisk->OutputInfos();
}

void MainWindow::onConvertShowWindow(QString strDriveId, QString strMountUri)
{
    // 进程启动时一定时间内不弹窗提示
    if (QDateTime::currentDateTime().toMSecsSinceEpoch() - m_nAppStartTimestamp < NEWINFO_DELAYSHOW_TIME) {
        m_dataFlashDisk->resetAllNewState();
        return;
    }
    insertorclick = true;

    MainWindowShow();
    string strDeviceId = strDriveId.toStdString();
    if (std::find(m_vtDeviveId.begin(), m_vtDeviveId.end(), strDeviceId) == m_vtDeviveId.end()) {
        #if IFDISTINCT_DEVICON
        QString strIcon = m_dataFlashDisk->getVolumeIcon(strDriveId);
        onRequestSendDesktopNotify(tr("Please do not pull out the storage device when reading or writing"),
                                        strIcon);
        #else
        if (strDriveId.startsWith("/dev/sr")) {
            onRequestSendDesktopNotify(tr("Please do not pull out the CDROM when reading or writing"),
                                        QString("media-removable-symbolic"));
        } else if (strDriveId.startsWith("/dev/mmcblk")) {
            onRequestSendDesktopNotify(tr("Please do not pull out the SD Card when reading or writing"),
                                        QString("media-removable-symbolic"));
        } else {
            onRequestSendDesktopNotify(tr("Please do not pull out the USB flash disk when reading or writing"),
                                        QString("drive-removable-media-usb"));
        }
        #endif
        m_vtDeviveId.push_back(strDeviceId);
    }
}

void MainWindow::onNotifyDeviceRemoved(QString strDevId)
{
    if (strDevId.isEmpty()) {
        return;
    }
    #if IFDISTINCT_DEVICON
    QString strIcon = m_dataFlashDisk->getVolumeIcon(strDevId);
    onRequestSendDesktopNotify(tr("Storage device removed"), strIcon);
    #else
    if (strDevId.startsWith("/dev/sr")) {
        onRequestSendDesktopNotify(tr("Storage device removed"),
                                    QString("media-removable-symbolic"));
    } else if (strDevId.startsWith("/dev/mmcblk")) {
        onRequestSendDesktopNotify(tr("Storage device removed"),
                                    QString("media-removable-symbolic"));
    } else {
        onRequestSendDesktopNotify(tr("Storage device removed"),
                                    QString("drive-removable-media-usb"));
    }
    #endif
}

void MainWindow::onConvertUpdateWindow(QString strDevName, unsigned uDevType)
{
    // uDevType: 0 drive , 1 volume, 2 mount
    if (uDevType != 0) {    // not drive detached
        insertorclick = true;
        MainWindowShow(true);
    }
}

// the drive-connected callback function the is triggered when the usb device is inseted
void MainWindow::drive_connected_callback(GVolumeMonitor *monitor, GDrive *drive, MainWindow *p_this)
{
    qInfo() << "drive add";
    if(p_this->ifsettings->get(IFAUTOLOAD).toBool()) {
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

        if (!g_drive_has_volumes(gdrive)) {
            QTimer::singleShot(1000, p_this, [&,driveInfo,p_this]() { p_this->onCheckDriveValid(driveInfo); });
        }

        lVolume = g_drive_get_volumes(gdrive);
        if (lVolume) {
            uSubVolumeSize = g_list_length(lVolume);
            g_list_free(lVolume);
        }

        p_this->getDriveIconsInfo(gdrive, driveInfo);

        if (!driveInfo.strId.empty()) {
            p_this->m_dataFlashDisk->addDriveInfo(driveInfo);
        }
        // perhaps uSubVolumeSize is 0 and is ok ?
        // else {
        //     qInfo()<<"wrong disk has intered";
        //     p_this->onInsertAbnormalDiskNotify(tr("There is a problem with this device"));
        // }
    }

    if(p_this->m_dataFlashDisk->getValidInfoCount() >= 1) {
        p_this->m_systray->show();
    }

    p_this->triggerType = 0;
    p_this->m_dataFlashDisk->OutputInfos();
}

// the drive-disconnected callback function the is triggered when the usb device is pull out
void MainWindow::drive_disconnected_callback (GVolumeMonitor *monitor, GDrive *drive, MainWindow *p_this)
{
    qInfo() << "drive disconnect";

    FDDriveInfo driveInfo;
    char *devPath = g_drive_get_identifier(drive,G_DRIVE_IDENTIFIER_KIND_UNIX_DEVICE);
    if (devPath != NULL) {
        driveInfo.strId = devPath;
        g_free(devPath);
    }

    vector<string>::iterator itDeviceId = p_this->m_vtDeviveId.begin();
    for (; itDeviceId != p_this->m_vtDeviveId.end();) {
        if (driveInfo.strId == *itDeviceId) {
            itDeviceId = p_this->m_vtDeviveId.erase(itDeviceId);
        } else {
            itDeviceId++;
        }
    }
    p_this->m_dataFlashDisk->removeDriveInfo(driveInfo);
    Q_EMIT p_this->notifyDeviceRemoved(QString::fromStdString(driveInfo.strId));
    if(p_this->m_dataFlashDisk->getValidInfoCount() == 0) {
        p_this->ui->centralWidget->hide();
        p_this->m_systray->hide();
    }
    p_this->m_dataFlashDisk->OutputInfos();
}

// when the usb device is identified, we should mount every partition
void MainWindow::volume_added_callback(GVolumeMonitor *monitor, GVolume *volume, MainWindow *p_this)
{
    qDebug() << "volume add";
    GDrive* gdrive = g_volume_get_drive(volume);

    FILE *fp = NULL;
    int a = 0;
    char buf[128] = {0};

    fp = fopen("/proc/cmdline","r");
    if (fp) {
        while(fscanf(fp,"%127s",buf) > 0) {
            if(strcmp(buf,"live") == 0) {
                a++;
            }
        }
        fclose(fp);
    }
    p_this->ifautoload = p_this->ifsettings->get(IFAUTOLOAD).toBool();

    if(a > 0) {
        QProcess::startDetached("gsettings set org.ukui.flash-disk.autoload ifautoload false");
    } else {
        //QProcess::startDetached("gsettings set org.ukui.flash-disk.autoload ifautoload true");
    }

    bool isNewMount = false;
    if(!gdrive) {
        FDVolumeInfo volumeInfo;
        bool isValidMount = true;
        char *devPath = g_volume_get_identifier(volume,G_VOLUME_IDENTIFIER_KIND_UNIX_DEVICE);
        if (devPath) {
            if (!(g_str_has_prefix(devPath,"/dev/sr") || g_str_has_prefix(devPath,"/dev/bus") || g_str_has_prefix(devPath,"/dev/sd")
                  || g_str_has_prefix(devPath,"/dev/mmcblk"))) {
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
        p_this->getVolumeIconsInfo(volume, volumeInfo);
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
                if (fileRoot) {
                    GFileInfo *info = g_file_query_filesystem_info(fileRoot,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE,nullptr,nullptr);
                    if (info) {
                        volumeInfo.mountInfo.lluTotalSize = g_file_info_get_attribute_uint64(info,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE);
                        g_object_unref(info);
                    }
                    g_object_unref(fileRoot);
                }
                p_this->getDataCDRomCapacity(volumeInfo.strId.c_str(), volumeInfo.mountInfo.lluTotalSize);
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
                        volumeInfo.mountInfo.strTooltip = tooltip;
                        g_free(tooltip);
                    }
                    g_object_unref(root);
                }
                p_this->getMountIconsInfo(mount, volumeInfo.mountInfo);
            }
            g_object_unref(mount);
        } else {
            if (volumeInfo.isCanMount) {
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
            p_this->getDriveIconsInfo(gdrive, driveInfo);
            driveInfo.isCanEject = g_drive_can_eject(gdrive);
            driveInfo.isCanStop = g_drive_can_stop(gdrive);
            driveInfo.isCanStart = g_drive_can_start(gdrive);
            driveInfo.isRemovable = g_drive_is_removable(gdrive);

            if(!p_this->isSystemRootDev(driveInfo.strId.c_str()) && p_this->getDevInterface(driveInfo.strId.c_str()) &&
                (driveInfo.isCanEject || driveInfo.isCanStop || driveInfo.isRemovable)) {
                if(g_str_has_prefix(devPath,"/dev/sr") || g_str_has_prefix(devPath,"/dev/bus") || g_str_has_prefix(devPath,"/dev/sd")
                    || g_str_has_prefix(devPath,"/dev/mmcblk")) {
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
                        p_this->getVolumeIconsInfo(volume, volumeInfo);
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
                                if (fileRoot) {
                                    GFileInfo *info = g_file_query_filesystem_info(fileRoot,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE,nullptr,nullptr);
                                    if (info) {
                                        volumeInfo.mountInfo.lluTotalSize = g_file_info_get_attribute_uint64(info,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE);
                                        g_object_unref(info);
                                    }
                                    g_object_unref(fileRoot);
                                }
                                p_this->getDataCDRomCapacity(volumeInfo.strId.c_str(), volumeInfo.mountInfo.lluTotalSize);
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
                                p_this->getMountIconsInfo(mount, volumeInfo.mountInfo);
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
                if (isNewMount) {
                    //qInfo()<<"cd data disk has mounted!";
                    volumeInfo.isNewInsert = true;
                    p_this->m_dataFlashDisk->addVolumeInfoWithDrive(driveInfo, volumeInfo);
                    string strDevId = driveInfo.strId.empty()?volumeInfo.strId:driveInfo.strId;
                    Q_EMIT p_this->convertShowWindow(strDevId.c_str(), volumeInfo.mountInfo.strUri.c_str());
                } else {
                    p_this->m_dataFlashDisk->addVolumeInfoWithDrive(driveInfo, volumeInfo);
                }
            }
            g_free(devPath);
        }
        g_object_unref(gdrive);
    }
    if(p_this->m_dataFlashDisk->getValidInfoCount() > 0)
    {
        p_this->m_systray->show();
    }
    p_this->m_dataFlashDisk->OutputInfos();
}

// when the U disk is pull out we should reduce all its partitions
void MainWindow::volume_removed_callback(GVolumeMonitor *monitor, GVolume *volume, MainWindow *p_this)
{
    qInfo() << "volume removed";
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
    vector<string>::iterator itDeviceId = p_this->m_vtDeviveId.begin();
    for (; itDeviceId != p_this->m_vtDeviveId.end();) {
        if (volumeInfo.strId == *itDeviceId) {
            itDeviceId = p_this->m_vtDeviveId.erase(itDeviceId);
        } else {
            itDeviceId++;
        }
    }
    p_this->m_dataFlashDisk->removeVolumeInfo(volumeInfo);
    if(p_this->m_dataFlashDisk->getValidInfoCount() == 0) {
        p_this->m_systray->hide();
    }
    p_this->m_dataFlashDisk->OutputInfos();
    Q_EMIT p_this->convertUpdateWindow(QString::fromStdString(volumeInfo.strName), 1);     //emit a signal to update the MainMainShow slot
}

// when the volumes were mounted we add its mounts number
void MainWindow::mount_added_callback(GVolumeMonitor *monitor, GMount *mount, MainWindow *p_this)
{
    qInfo() << "mount add";

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
            p_this->getDriveIconsInfo(gdrive, driveInfo);
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
            p_this->getVolumeIconsInfo(gvolume, volumeInfo);
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
    if (fileRoot) {
        unsigned uRetryTime = 5;
        unsigned uDelayTime = 200000;
        while (uRetryTime > 0) {
            GFileInfo *info = g_file_query_filesystem_info(fileRoot,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE,nullptr,nullptr);
            if (info) {
                mountInfo.lluTotalSize = g_file_info_get_attribute_uint64(info,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE);
                g_object_unref(info);
                break;
            } else {
                usleep(uDelayTime);
            }
            uRetryTime --;
        }
        g_object_unref(fileRoot);
    }
    p_this->getDataCDRomCapacity(driveInfo.strId.empty()?volumeInfo.strId.c_str():driveInfo.strId.c_str(),
            mountInfo.lluTotalSize);
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
    p_this->getMountIconsInfo(mount, mountInfo);

    if (driveInfo.strId.empty()) {
       Q_EMIT p_this->telephoneMount();
    }
    bool isNewMount = !(p_this->m_dataFlashDisk->isMountInfoExist(mountInfo));
    if (!driveInfo.strId.empty()) {
        if (p_this->isSystemRootDev(driveInfo.strId.c_str()) ||
            (!driveInfo.isCanEject && !driveInfo.isCanStop && !driveInfo.isRemovable)) {
            isValidMount = false;
        }
    }
    if (volumeInfo.strId.empty()) {
        // 没有卷信息的挂载不处理（ftp等）
        isValidMount = false;
    }
    if(isValidMount && (mountInfo.isCanUnmount || g_str_has_prefix(strVolumePath.c_str(),"/dev/bus")
            || g_str_has_prefix(strVolumePath.c_str(),"/dev/sr") || g_str_has_prefix(strVolumePath.c_str(),"/dev/mmcblk"))) {
        qInfo() << "real mount loaded";
        mountInfo.isNewInsert = true;
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
    else {
        qInfo()<<"不符合过滤条件的设备已被挂载";
    }

    if(p_this->m_dataFlashDisk->getValidInfoCount() >= 1) {
        if (isValidMount && isNewMount) {
            //qInfo()<<"cd data disk has mounted!";
            string strDevId = driveInfo.strId.empty()?volumeInfo.strId:driveInfo.strId;
            Q_EMIT p_this->convertShowWindow(strDevId.c_str(), mountInfo.strUri.c_str());
        }
        p_this->m_systray->show();
    }
    p_this->m_dataFlashDisk->OutputInfos();
}

// when the mountes were uninstalled we reduce mounts number
void MainWindow::mount_removed_callback(GVolumeMonitor *monitor, GMount *mount, MainWindow *p_this)
{
    qInfo() << mount << "mount remove";
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
    // check mount's volume had removed?
    FDVolumeInfo volumeInfo;
    p_this->m_dataFlashDisk->getVolumeInfoByMount(mountInfo, volumeInfo);
    quint64 mountTickDiff = p_this->m_dataFlashDisk->getMountTickDiff(mountInfo);

    p_this->m_dataFlashDisk->removeMountInfo(mountInfo);

    if(p_this->m_dataFlashDisk->getValidInfoCount() == 0) {
        p_this->m_systray->hide();
    }
    p_this->m_dataFlashDisk->OutputInfos();
    Q_EMIT p_this->convertUpdateWindow(QString::fromStdString(mountInfo.strName), 2);     //emit a signal to update the MainMainShow slot
    qInfo()<<"ID:"<<mountInfo.strId.c_str()<<",volume is:"<<volumeInfo.strId.c_str()<<",mount tickdiff:"<<mountTickDiff;
    if (mountTickDiff > 0 && mountTickDiff < 500 && !volumeInfo.strId.empty()) {
        QTimer::singleShot(1000, p_this, [&,volumeInfo,p_this]() { p_this->onRemountVolume(volumeInfo); });
    }
}

// it stands that when you insert a usb device when all the  U disk partitions
void MainWindow::frobnitz_result_func_volume(GVolume *source_object,GAsyncResult *res,MainWindow *p_this)
{
    gboolean success =  FALSE;
    GError *err = nullptr;
    bool bMountSuccess = false;
    success = g_volume_mount_finish (source_object, res, &err);
    if(!err) {
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
                p_this->getDriveIconsInfo(gdrive, driveInfo);
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
            p_this->getVolumeIconsInfo(source_object, volumeInfo);
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
            if (fileRoot) {
                unsigned uRetryTime = 5;
                unsigned uDelayTime = 200000;
                while (uRetryTime > 0) {
                    GFileInfo *info = g_file_query_filesystem_info(fileRoot,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE,nullptr,nullptr);
                    if (info) {
                        mountInfo.lluTotalSize = g_file_info_get_attribute_uint64(info,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE);
                        g_object_unref(info);
                        break;
                    } else {
                        usleep(uDelayTime);
                    }
                    uRetryTime --;
                }
                g_object_unref(fileRoot);
            }
            p_this->getDataCDRomCapacity(driveInfo.strId.empty()?volumeInfo.strId.c_str():driveInfo.strId.c_str(),
                mountInfo.lluTotalSize);
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
            p_this->getMountIconsInfo(gmount, mountInfo);
            g_object_unref(gmount);
        }
        if (bMountSuccess) {
            mountInfo.isNewInsert = true;
            bool isNewMount = !(p_this->m_dataFlashDisk->isMountInfoExist(mountInfo));
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
            if (isNewMount) {
                qInfo()<<"sig has emitted";
                string strDevId = driveInfo.strId.empty()?volumeInfo.strId:driveInfo.strId;
                Q_EMIT p_this->convertShowWindow(strDevId.c_str(), mountInfo.strUri.c_str());     //emit a signal to trigger the MainMainShow slot
            }
        }
    } else if (G_IO_ERROR_ALREADY_MOUNTED == err->code) {
    } else if (G_IO_ERROR_UNKNOWN == err->code) {
    } else {
        qInfo()<<"sorry mount failed"<<err->code<<","<<err->message;
        GDrive* gdrive = g_volume_get_drive(source_object);
        Q_EMIT p_this->deviceError(gdrive);
        if (nullptr != gdrive) {
            g_object_unref(gdrive);
        }
    }
}

// here we begin painting the main interface
void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    triggerType = 1;  //It represents how we open the interface

    if (ui->centralWidget && !ui->centralWidget->isHidden() && !insertorclick) {
        ui->centralWidget->hide();
        return ;
    }
    insertorclick = false;

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
    if(currentThemeMode == "ukui-dark" || currentThemeMode == "ukui-black" || currentThemeMode == "ukui-default"
        || currentThemeMode == "ukui")
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

    vboxlayout->setContentsMargins(2,4,2,4);
    vboxlayout->setSpacing(0);
    vboxlayout->setMargin(0);
    this->vboxlayout->addWidget(open_widget);

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
    connect(m_fdClickWidget, &FDClickWidget::clickedEject,this,&MainWindow::onClickedEject);

    line = new QWidget;
    line->setFixedHeight(1);
    line->setObjectName("lineWidget");
    if(currentThemeMode == "ukui-dark" || currentThemeMode == "ukui-black" || currentThemeMode == "ukui-default"
        || currentThemeMode == "ukui") {
        line->setStyleSheet("background-color:rgba(255,255,255,0.2);");
    } else {
        line->setStyleSheet("background-color:rgba(0,0,0,0.2);");
    }

    // when the drive is only or the drive is the first one,we make linestatus become  1
    line->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    line->setFixedSize(276,1);
    if (linestatus == 2) {
        this->vboxlayout->addWidget(line);
    }

    vboxlayout->setContentsMargins(2,4,2,4);
    //vboxlayout->setSpacing(0);
    //vboxlayout->setMargin(0);
    this->vboxlayout->addWidget(m_fdClickWidget);

    if (linestatus == 0) {
        this->vboxlayout->addWidget(line);
    }
}

void MainWindow::moveBottomRight()
{
    //MARGIN 为到任务栏或屏幕边缘的间隔
    #define MARGIN 4
    QDBusInterface iface("org.ukui.panel",
                         "/panel/position",
                         "org.ukui.panel",
                         QDBusConnection::sessionBus());
    QDBusReply<QVariantList> reply=iface.call("GetPrimaryScreenGeometry");
    //reply获取的参数共5个，分别是 主屏可用区域的起点x坐标，主屏可用区域的起点y坐标，主屏可用区域的宽度，主屏可用区域高度，任务栏位置
    //    reply.value();
    qInfo()<<reply.value().at(4).toInt();
    QVariantList position_list=reply.value();

    switch(reply.value().at(4).toInt()){
    case 1:
        //任务栏位于上方
        this->setGeometry(position_list.at(0).toInt()+position_list.at(2).toInt()-this->width()-MARGIN,
                          position_list.at(1).toInt()+MARGIN,
                          this->width(),this->height());
        break;
        //任务栏位于左边
    case 2:
        this->setGeometry(position_list.at(0).toInt()+MARGIN,
                          position_list.at(1).toInt()+reply.value().at(3).toInt()-this->height()-MARGIN,
                          this->width(),this->height());
        break;
        //任务栏位于右边
    case 3:
        this->setGeometry(position_list.at(0).toInt()+position_list.at(2).toInt()-this->width()-MARGIN,
                          position_list.at(1).toInt()+reply.value().at(3).toInt()-this->height()-MARGIN,
                          this->width(),this->height());
        break;
        //任务栏位于下方
    default:
        this->setGeometry(position_list.at(0).toInt()+position_list.at(2).toInt()-this->width()-MARGIN,
                          position_list.at(1).toInt()+reply.value().at(3).toInt()-this->height()-MARGIN,
                          this->width(),this->height());
        break;
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
        listItem->hide();
        listItem->deleteLater();
    }

    QList<QWidget *> listLine = ui->centralWidget->findChildren<QWidget *>();
    for(QWidget *listItem:listLine)
    {
        if(listItem->objectName() == "lineWidget") {
            listItem->hide();
            listItem->deleteLater();
        }
    }
    // only show new insert device
    if (insertorclick) {
        //Convenient interface layout for all drives
        map<string, FDDriveInfo>& listDriveInfo = m_dataFlashDisk->getDevInfoWithDrive();
        if (!listDriveInfo.empty()) {
            map<string, FDDriveInfo>::iterator itDriveInfo = listDriveInfo.begin();
            for ( ;itDriveInfo != listDriveInfo.end(); itDriveInfo++) {
                unsigned uVolumeNum = 0;
                bool isCanShow = (itDriveInfo->second.isCanEject || itDriveInfo->second.isCanStop || itDriveInfo->second.isRemovable);
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
                    if ((itVolumeInfo->second.strId.empty() && itVolumeInfo->second.mountInfo.strId.empty())
                        || (!itVolumeInfo->second.isNewInsert && !itVolumeInfo->second.mountInfo.isNewInsert)) {
                        // is not new insert device
                        continue;
                    }
                    if (!itVolumeInfo->second.mountInfo.strId.empty()) {
                        string strMountUri = itVolumeInfo->second.mountInfo.strUri;
                        if(g_str_has_prefix(strMountUri.c_str(),"file:///")) {
                            uVolumeType = 0;
                        } else if (g_str_has_prefix(strMountUri.c_str(),"mtp://") || g_str_has_prefix(strMountUri.c_str(),"gphoto2://")){
                            uVolumeType = 2;
                        } else if (g_str_has_prefix(strMountUri.c_str(),"burn:///")/* || g_str_has_prefix(strMountUri.c_str(),"cdda://")*/) {
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
                if (itDriveInfo->second.listVolumes.size() > 0 && uVolumeNum == 0 && uDiskCount > 0) {
                    uDiskCount --;
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
                if ((itVolumeInfo->second.strId.empty() && itVolumeInfo->second.mountInfo.strId.empty())
                    || (!itVolumeInfo->second.isNewInsert && !itVolumeInfo->second.mountInfo.isNewInsert)) {
                    // is not new insert device
                    continue;
                }
                if (!itVolumeInfo->second.mountInfo.strId.empty()) {
                    string strMountUri = itVolumeInfo->second.mountInfo.strUri;
                    if(g_str_has_prefix(strMountUri.c_str(),"file:///")) {
                        uVolumeType = 0;
                    } else if (g_str_has_prefix(strMountUri.c_str(),"mtp://") || g_str_has_prefix(strMountUri.c_str(),"gphoto2://")){
                        uVolumeType = 2;
                    } else if (g_str_has_prefix(strMountUri.c_str(),"burn:///")/* || g_str_has_prefix(strMountUri.c_str(),"cdda://")*/) {
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
                if (itMountInfo->second.strId.empty() || !itMountInfo->second.isNewInsert) {
                    // is not new insert device
                    continue;
                }
                if (!itMountInfo->second.strId.empty()) {
                    string strMountUri = itMountInfo->second.strUri;
                    if(g_str_has_prefix(strMountUri.c_str(),"file:///")) {
                        uMountType = 0;
                    } else if (g_str_has_prefix(strMountUri.c_str(),"mtp://") || g_str_has_prefix(strMountUri.c_str(),"gphoto2://")){
                        uMountType = 2;
                    } else if (g_str_has_prefix(strMountUri.c_str(),"burn:///")/* || g_str_has_prefix(strMountUri.c_str(),"cdda://")*/) {
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
    } else {  // show all device
        //Convenient interface layout for all drives
        map<string, FDDriveInfo>& listDriveInfo = m_dataFlashDisk->getDevInfoWithDrive();
        if (!listDriveInfo.empty()) {
            map<string, FDDriveInfo>::iterator itDriveInfo = listDriveInfo.begin();
            for ( ;itDriveInfo != listDriveInfo.end(); itDriveInfo++) {
                unsigned uVolumeNum = 0;
                bool isCanShow = (itDriveInfo->second.isCanEject || itDriveInfo->second.isCanStop || itDriveInfo->second.isRemovable);
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
                        } else if (g_str_has_prefix(strMountUri.c_str(),"burn:///")/* || g_str_has_prefix(strMountUri.c_str(),"cdda://")*/) {
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
                    } else if (g_str_has_prefix(strMountUri.c_str(),"burn:///")/* || g_str_has_prefix(strMountUri.c_str(),"cdda://")*/) {
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
                    } else if (g_str_has_prefix(strMountUri.c_str(),"burn:///")/* || g_str_has_prefix(strMountUri.c_str(),"cdda://")*/) {
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
            if (content.contains(".mount")) {
                pointMountNum += 1;
            }

            content = file.readLine().trimmed();
            if(pointMountNum >= 1) {
                findPointMount = true;
            } else {
                findPointMount = false;
            }
        }
    }
    file.close();
}

void MainWindow::onMaininterfacehide()
{
    ui->centralWidget->hide();
    this->driveVolumeNum = 0;
    interfaceHideTime->stop();
    m_dataFlashDisk->resetAllNewState();
}

void MainWindow::moveBottomNoBase()
{
#if 0
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

#endif
    // MARGIN 为到任务栏或屏幕边缘的间隔
#define MARGIN 4
    QDBusInterface iface("org.ukui.panel",
                         "/panel/position",
                         "org.ukui.panel",
                         QDBusConnection::sessionBus());
    QDBusReply<QVariantList> reply=iface.call("GetPrimaryScreenGeometry");

    if (!iface.isValid() || !iface.isValid() || reply.value().size()<5) {
        qCritical() << QDBusConnection::sessionBus().lastError().message();
        ui->centralWidget->setGeometry(0,0,ui->centralWidget->width(),ui->centralWidget->height());
    } else {
        // reply获取的参数共5个，分别是 主屏可用区域的起点x坐标，主屏可用区域的起点y坐标，主屏可用区域的宽度，主屏可用区域高度，任务栏位置
        QVariantList position_list=reply.value();

        switch(reply.value().at(4).toInt()){
        case 1:
            // 任务栏位于上方
            ui->centralWidget->setGeometry(position_list.at(0).toInt()+position_list.at(2).toInt()-ui->centralWidget->width()-MARGIN,
                                           position_list.at(1).toInt()+MARGIN,
                                           ui->centralWidget->width(),ui->centralWidget->height());
            break;
            // 任务栏位于左边
        case 2:
            ui->centralWidget->setGeometry(position_list.at(0).toInt()+MARGIN,
                                           position_list.at(1).toInt()+reply.value().at(3).toInt()-ui->centralWidget->height()-MARGIN,
                                           ui->centralWidget->width(),this->height());
            break;
            // 任务栏位于右边
        case 3:
            ui->centralWidget->setGeometry(position_list.at(0).toInt()+position_list.at(2).toInt()-ui->centralWidget->width()-MARGIN,
                                           position_list.at(1).toInt()+reply.value().at(3).toInt()-ui->centralWidget->height()-MARGIN,
                                           ui->centralWidget->width(),ui->centralWidget->height());
            break;
            // 任务栏位于下方
        default:
            ui->centralWidget->setGeometry(position_list.at(0).toInt()+position_list.at(2).toInt()-ui->centralWidget->width()-MARGIN,
                                           position_list.at(1).toInt()+reply.value().at(3).toInt()-ui->centralWidget->height()-MARGIN,
                                           ui->centralWidget->width(),ui->centralWidget->height());
            break;
        }
    }
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

// new a gsettings object to get the information of the opacity of the window
void MainWindow::initTransparentState()
{
    const QByteArray idtrans(THEME_QT_TRANS);

    if(QGSettings::isSchemaInstalled(idtrans))
    {
        m_transparency_gsettings = new QGSettings(idtrans);
    }
}

// use gsettings to get the opacity
void MainWindow::getTransparentData()
{
    if (!m_transparency_gsettings) {
       m_transparency = 0.95;
       return;
    }

    QStringList keys = m_transparency_gsettings->keys();
    if (keys.contains("transparency")) {
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

void MainWindow::onEjectVolumeForce(GVolume *v)
{
    g_volume_eject_with_operation(v, G_MOUNT_UNMOUNT_FORCE, nullptr, nullptr, GAsyncReadyCallback(&MainWindow::frobnitz_force_result_func), this);
}

void MainWindow::AsyncUnmount(QString strMountRoot,MainWindow *p_this)
{
    qInfo() << "dataPath:" << strMountRoot;
    QProcess p;
    p.setProgram("pkexec");
    p.setArguments(QStringList() << "eject" << strMountRoot);
    p.start();

    bool bSuccess = p.waitForFinished();

    if (p_this) {
        p_this->ifSucess = bSuccess;
    }
}

void MainWindow::frobnitz_force_result_func(GDrive *source_object,GAsyncResult *res, MainWindow *p_this)
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

        p_this->m_eject = new ejectInterface(p_this->ui->centralWidget,QString::fromStdString(driveInfo.strName),
                                            NORMALDEVICE,QString::fromStdString(driveInfo.strId));
        p_this->m_eject->show();
        FlashDiskData::getInstance()->removeDriveInfo(driveInfo);
        if(FlashDiskData::getInstance()->getValidInfoCount() == 0) {
            p_this->m_systray->hide();
        }
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

        p_this->m_eject = new ejectInterface(p_this->ui->centralWidget,QString::fromStdString(driveInfo.strName),
                                            NORMALDEVICE,QString::fromStdString(driveInfo.strId));
        p_this->m_eject->show();
        FlashDiskData::getInstance()->removeDriveInfo(driveInfo);
        if(FlashDiskData::getInstance()->getValidInfoCount() == 0) {
            p_this->m_systray->hide();
        }
    } else /*if(g_drive_can_stop(source_object) == true)*/ {
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

        if (p_this->chooseDialog == nullptr) {
            p_this->chooseDialog = new interactiveDialog(QString::fromStdString(driveInfo.strId), p_this->ui->centralWidget);
        }
        p_this->chooseDialog->show();
        p_this->chooseDialog->setFocus();
        p_this->connect(p_this->chooseDialog,&interactiveDialog::FORCESIG,p_this,[=]() {
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
    if(!err) {
        FDVolumeInfo volumeInfo;
        char *volumeId = g_volume_get_identifier(source_object,G_VOLUME_IDENTIFIER_KIND_UNIX_DEVICE);
        if (volumeId) {
            volumeInfo.strId = volumeId;
            g_free(volumeId);
        }

        FlashDiskData::getInstance()->removeVolumeInfo(volumeInfo);
        if(FlashDiskData::getInstance()->getValidInfoCount() == 0) {
            p_this->m_systray->hide();
        }
    }
}

void MainWindow::frobnitz_force_result_unmount(GMount *source_object,GAsyncResult *res,MainWindow *p_this)
{
    gboolean success =  FALSE;
    GError *err = nullptr;
    success = g_mount_unmount_with_operation_finish(source_object,res, &err);
    if(!err) {
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

void MainWindow::onClickedEject(EjectDeviceInfo eDeviceInfo)
{
    m_curEjectDeviceInfo = eDeviceInfo;
    m_curEjectDeviceInfo.pVoid = this;
    m_curEjectDeviceInfo.uFlag = G_MOUNT_UNMOUNT_NONE;
    doRealEject(&m_curEjectDeviceInfo, G_MOUNT_UNMOUNT_NONE);
}

bool MainWindow::onDeviceErrored(GDrive* drive)
{
    g_return_val_if_fail(G_IS_DRIVE(drive), false);

    g_autofree char* device = g_drive_get_identifier (drive, G_DRIVE_IDENTIFIER_KIND_UNIX_DEVICE);

    g_return_val_if_fail(device && strstr(device, "/dev/sd"), false);

    g_return_val_if_fail (DeviceOperation::getDriveSize(drive) > 0, false);

    GList* volumes = g_drive_get_volumes(drive);
    if (!volumes && !mRepairDialog.contains(device)) {
        RepairDialogBox* b = new RepairDialogBox(drive);
        b->connect(b, &RepairDialogBox::repairOK, this, [=] (RepairDialogBox* d) {
            if (mRepairDialog.contains(d->getDeviceName())) {
                mRepairDialog.remove(d->getDeviceName());
            }
        });
        mRepairDialog[device] = b;
        b->show();
    } else {
        for (auto v = volumes; nullptr != v; v = v->next) {
            GVolume* vv = G_VOLUME(v->data);
            g_autofree char* volumeName = g_volume_get_identifier (vv, G_DRIVE_IDENTIFIER_KIND_UNIX_DEVICE);
            g_autoptr(GMount) m = g_volume_get_mount(vv);
            if (!m) {
                if (volumeName && !mRepairDialog.contains(volumeName)) {
                    RepairDialogBox* b = new RepairDialogBox(vv);
                    b->connect(b, &RepairDialogBox::remountDevice, this, &MainWindow::remountVolume);
                    b->connect(b, &RepairDialogBox::repairOK, this, [=] (RepairDialogBox* d) {
                        if (mRepairDialog.contains(d->getDeviceName())) {
                            mRepairDialog.remove(d->getDeviceName());
                        }
                    });
                    mRepairDialog[volumeName] = b;
                    b->show();
                }
            }
        }
        g_list_free_full(volumes, g_object_unref);
    }

    return true;
}

void MainWindow::onMountVolume(GVolume* v)
{
    g_volume_mount(v, G_MOUNT_MOUNT_NONE, nullptr, nullptr, GAsyncReadyCallback(frobnitz_result_func_volume), this);
}

bool MainWindow::doRealEject(EjectDeviceInfo* peDeviceInfo, GMountUnmountFlags flag)
{
    // find the device's drive & volume & mount
    if (!peDeviceInfo || !peDeviceInfo->pVoid) {
        return false;
    }

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
    if (!peDeviceInfo->strDriveId.isEmpty()) {
        current_drive_list = g_volume_monitor_get_connected_drives(g_volume_monitor);
        for (lDrive = current_drive_list; lDrive != NULL; lDrive = lDrive->next) {
            GDrive *gdrive = (GDrive *)lDrive->data;
            char *devPath = g_drive_get_identifier(gdrive,G_DRIVE_IDENTIFIER_KIND_UNIX_DEVICE);
            if (devPath != NULL) {
                if(peDeviceInfo->strDriveId == devPath) {
                    devDrive = gdrive;
                    current_volume_list = g_drive_get_volumes(gdrive);
                    if (current_volume_list) {
                        uVolumeSize = g_list_length(current_volume_list);
                        for(lVolume = current_volume_list; lVolume != NULL; lVolume = lVolume->next){ //遍历驱动器上的所有卷设备
                            GVolume* volume = (GVolume *)lVolume->data;
                            char *volumeId = g_volume_get_identifier(volume,G_VOLUME_IDENTIFIER_KIND_UNIX_DEVICE);
                            if (volumeId) {
                                if (peDeviceInfo->strVolumeId != volumeId) {
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
                if (peDeviceInfo->strMountUri.isEmpty()) {
                    g_drive_eject_with_operation(devDrive,
                            flag,
                            NULL,
                            NULL,
                            GAsyncReadyCallback(&MainWindow::frobnitz_result_func),
                            peDeviceInfo->pVoid);
                    isDone = true;
                } else {
                    if (g_drive_can_eject(devDrive)){//for udisk or DVD.
                        g_drive_eject_with_operation(devDrive,
                                flag,
                                NULL,
                                NULL,
                                GAsyncReadyCallback(&MainWindow::frobnitz_result_func),
                                peDeviceInfo->pVoid);
                        isDone = true;
                        #if 0
                        GFile *file = g_file_new_for_uri(peDeviceInfo->strMountUri.toUtf8().constData());
                        if (file) {
                            g_file_eject_mountable_with_operation(file,
                                                            flag,
                                                            nullptr,
                                                            nullptr,
                                                            GAsyncReadyCallback(fileEjectMountableCB),
                                                            peDeviceInfo);
                            g_object_unref(file);
                            isDone = true;
                        }
                        #endif
                    } else if(g_drive_can_stop(devDrive) || g_drive_is_removable(devDrive)){//for mobile harddisk.
                        g_drive_stop(devDrive, flag, NULL, NULL,
                                    GAsyncReadyCallback(driveStopCb),
                                    peDeviceInfo);
                        isDone = true;
                    }
                }
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
    if (!isDone && !peDeviceInfo->strVolumeId.isEmpty()) {
        current_volume_list = g_volume_monitor_get_volumes(g_volume_monitor);
        if (current_volume_list) {
            for (lVolume = current_volume_list; lVolume != NULL; lVolume = lVolume->next) {
                GVolume *volume = (GVolume *)lVolume->data;
                GDrive *gdrive = g_volume_get_drive(volume);
                if (!gdrive) {
                    char *volumeId = g_volume_get_identifier(volume,G_VOLUME_IDENTIFIER_KIND_UNIX_DEVICE);
                    if (volumeId) {
                        if (peDeviceInfo->strVolumeId != volumeId) {
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
                        flag,
                        NULL,
                        NULL,
                        GAsyncReadyCallback(&MainWindow::frobnitz_force_result_unmount),
                        peDeviceInfo->pVoid);
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
    if (!isDone && !peDeviceInfo->strMountId.isEmpty()) {
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
                    if (peDeviceInfo->strMountId == strId) {
                        devMount = gmount;
                        break;
                    }
                } else {
                    g_object_unref(gvolume);
                }
            }
            if (devMount != NULL) {
                g_mount_unmount_with_operation(devMount,
                        flag,
                        NULL,
                        NULL,
                        GAsyncReadyCallback(&MainWindow::frobnitz_force_result_unmount),
                        peDeviceInfo->pVoid);
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
        mountInfo.strId = peDeviceInfo->strMountId.toStdString();
        m_dataFlashDisk->removeMountInfo(mountInfo);
        volumeInfo.strId = peDeviceInfo->strVolumeId.toStdString();
        m_dataFlashDisk->removeVolumeInfo(volumeInfo);
        driveInfo.strId = peDeviceInfo->strDriveId.toStdString();
        m_dataFlashDisk->removeDriveInfo(driveInfo);
        if(m_dataFlashDisk->getValidInfoCount() == 0) {
            m_systray->hide();
        }
    }
    ui->centralWidget->hide();
    return true;
}

GAsyncReadyCallback MainWindow::fileEjectMountableCB(GFile *file, GAsyncResult *res, EjectDeviceInfo *peDeviceInfo)
{
    gboolean success =  FALSE;
    GError *err = nullptr;
    success = g_file_eject_mountable_with_operation_finish(file, res, &err);
    if (!err) {
        FDDriveInfo driveInfo;
        driveInfo.strId = peDeviceInfo->strDriveId.toStdString();
        driveInfo.strName = peDeviceInfo->strDriveName.toStdString();
        MainWindow* pThis = (MainWindow*)(peDeviceInfo->pVoid);
        pThis->m_eject = new ejectInterface(pThis->ui->centralWidget,QString::fromStdString(driveInfo.strName),
                                            NORMALDEVICE,QString::fromStdString(driveInfo.strId));
        pThis->m_eject->show();
        FlashDiskData::getInstance()->removeDriveInfo(driveInfo);
        if(FlashDiskData::getInstance()->getValidInfoCount() == 0) {
            pThis->m_systray->hide();
        }
    } else /*if(g_drive_can_stop(source_object) == true)*/ {
        if (peDeviceInfo->uFlag != G_MOUNT_UNMOUNT_FORCE) {
            FDDriveInfo driveInfo;
            driveInfo.strId = peDeviceInfo->strDriveId.toStdString();
            driveInfo.strName = peDeviceInfo->strDriveName.toStdString();
            MainWindow* pThis = (MainWindow*)(peDeviceInfo->pVoid);
            if (pThis->chooseDialog == nullptr) {
                pThis->chooseDialog = new interactiveDialog(QString::fromStdString(driveInfo.strId), pThis->ui->centralWidget);
            }
            pThis->chooseDialog->show();
            pThis->chooseDialog->setFocus();
            pThis->connect(pThis->chooseDialog,&interactiveDialog::FORCESIG,pThis,[=]() {
                pThis->chooseDialog->close();
                peDeviceInfo->uFlag = G_MOUNT_UNMOUNT_FORCE;
                pThis->doRealEject(peDeviceInfo, G_MOUNT_UNMOUNT_FORCE);
            });
        }
    }
    return nullptr;
}

void MainWindow::driveStopCb(GObject* object, GAsyncResult* res, EjectDeviceInfo *peDeviceInfo)
{
    gboolean success =  FALSE;
    GError *err = nullptr;
    success = g_drive_stop_finish(G_DRIVE(object), res, &err);
    qInfo() << "driveStopCb:" << success;

    if (success || !err || (G_IO_ERROR_FAILED_HANDLED == err->code)) {
        FDDriveInfo driveInfo;
        driveInfo.strId = peDeviceInfo->strDriveId.toStdString();
        driveInfo.strName = peDeviceInfo->strDriveName.toStdString();
        MainWindow* pThis = (MainWindow*)(peDeviceInfo->pVoid);
        pThis->m_eject = new ejectInterface(pThis->ui->centralWidget,QString::fromStdString(driveInfo.strName),
                                            NORMALDEVICE,QString::fromStdString(driveInfo.strId));
        pThis->m_eject->show();
        FlashDiskData::getInstance()->removeDriveInfo(driveInfo);
        if(FlashDiskData::getInstance()->getValidInfoCount() == 0) {
            pThis->m_systray->hide();
        }
    } else {
        if (peDeviceInfo->uFlag != G_MOUNT_UNMOUNT_FORCE) {
            FDDriveInfo driveInfo;
            driveInfo.strId = peDeviceInfo->strDriveId.toStdString();
            driveInfo.strName = peDeviceInfo->strDriveName.toStdString();
            MainWindow* pThis = (MainWindow*)(peDeviceInfo->pVoid);
            if (pThis->chooseDialog == nullptr) {
                pThis->chooseDialog = new interactiveDialog(QString::fromStdString(driveInfo.strId), pThis->ui->centralWidget);
            }
            pThis->chooseDialog->show();
            pThis->chooseDialog->setFocus();
            pThis->connect(pThis->chooseDialog,&interactiveDialog::FORCESIG,pThis,[=]() {
                pThis->chooseDialog->close();
                peDeviceInfo->uFlag = G_MOUNT_UNMOUNT_FORCE;
                pThis->doRealEject(peDeviceInfo, G_MOUNT_UNMOUNT_FORCE);
            });
        }
    }
}

void MainWindow::onRemountVolume(FDVolumeInfo volumeInfo)
{
    if (volumeInfo.strId.empty()) {
        return;
    }

    if (!(g_str_has_prefix(volumeInfo.strId.c_str(),"/dev/bus") || g_str_has_prefix(volumeInfo.strId.c_str(),"/dev/sd"))) {
        return;
    }

    qInfo() << "volumeInfo.strId:" << volumeInfo.strId.c_str();
    GList *lDrive = NULL;
    GList *lVolume = NULL;
    GList *current_drive_list = NULL;
    GList *current_volume_list = NULL;
    GDrive* devDrive = NULL;
    GVolume* devVolume = NULL;
    bool isDone = false;

    //about drive
    GVolumeMonitor *g_volume_monitor = g_volume_monitor_get();
    current_drive_list = g_volume_monitor_get_connected_drives(g_volume_monitor);
    for (lDrive = current_drive_list; lDrive != NULL; lDrive = lDrive->next) {
        GDrive *gdrive = (GDrive *)lDrive->data;
        char *devPath = g_drive_get_identifier(gdrive,G_DRIVE_IDENTIFIER_KIND_UNIX_DEVICE);
        if (devPath != NULL) {
            devDrive = gdrive;
            current_volume_list = g_drive_get_volumes(gdrive);
            if (current_volume_list) {
                for(lVolume = current_volume_list; lVolume != NULL; lVolume = lVolume->next){ //遍历驱动器上的所有卷设备
                    GVolume* volume = (GVolume *)lVolume->data;
                    char *volumeId = g_volume_get_identifier(volume,G_VOLUME_IDENTIFIER_KIND_UNIX_DEVICE);
                    if (volumeId) {
                        if (volumeInfo.strId != volumeId) {
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
            g_free(devPath);
        }
    }
    if (devVolume != NULL) {
        isDone = true;
        GMount* pCurMount = g_volume_get_mount(devVolume);
        if (pCurMount) {
            g_object_unref(pCurMount);
        } else {
            volumeInfo.isCanMount = g_volume_can_mount(devVolume);
            qInfo()<<"Volume canmount:"<<volumeInfo.isCanMount<<"|"<<ifsettings->get(IFAUTOLOAD).toBool();
            if(ifsettings->get(IFAUTOLOAD).toBool()) {
                g_volume_mount(devVolume,
                            G_MOUNT_MOUNT_NONE,
                            nullptr,
                            nullptr,
                            GAsyncReadyCallback(frobnitz_result_func_volume),
                            this);
            }
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

    //about volume not associated with a drive
    if (!isDone) {
        current_volume_list = g_volume_monitor_get_volumes(g_volume_monitor);
        if (current_volume_list) {
            for (lVolume = current_volume_list; lVolume != NULL; lVolume = lVolume->next) {
                GVolume *volume = (GVolume *)lVolume->data;
                GDrive *gdrive = g_volume_get_drive(volume);
                if (!gdrive) {
                    char *volumeId = g_volume_get_identifier(volume,G_VOLUME_IDENTIFIER_KIND_UNIX_DEVICE);
                    if (volumeId) {
                        if (volumeInfo.strId != volumeId) {
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
            isDone = true;
            GMount* pCurMount = g_volume_get_mount(devVolume);
            if (pCurMount) {
                g_object_unref(pCurMount);
            } else {
                volumeInfo.isCanMount = g_volume_can_mount(devVolume);
                qInfo()<<"Volume canmount:"<<volumeInfo.isCanMount<<"|"<<ifsettings->get(IFAUTOLOAD).toBool();
                if(ifsettings->get(IFAUTOLOAD).toBool()) {
                    g_volume_mount(devVolume,
                                G_MOUNT_MOUNT_NONE,
                                nullptr,
                                nullptr,
                                GAsyncReadyCallback(frobnitz_result_func_volume),
                                this);
                }
            }
        }
        if (current_volume_list) {
            g_list_free(current_volume_list);
            current_volume_list = NULL;
        }
    }
}

void MainWindow::onCheckDriveValid(FDDriveInfo driveInfo)
{
    if (driveInfo.strId.empty()) {
        return;
    }

    qInfo() << "driveInfo.strId:" << driveInfo.strId.c_str();
    if(!this->isSystemRootDev(driveInfo.strId.c_str()) &&
        (driveInfo.isCanEject || driveInfo.isCanStop || driveInfo.isRemovable)) {
        if(g_str_has_prefix(driveInfo.strId.c_str(),"/dev/sr") || g_str_has_prefix(driveInfo.strId.c_str(),"/dev/bus")
            || g_str_has_prefix(driveInfo.strId.c_str(),"/dev/sd") || g_str_has_prefix(driveInfo.strId.c_str(),"/dev/mmcblk")) {

            GList *lDrive = NULL;
            GList *current_drive_list = NULL;
            GDrive* devDrive = NULL;

            //about drive
            GVolumeMonitor *g_volume_monitor = g_volume_monitor_get();
            current_drive_list = g_volume_monitor_get_connected_drives(g_volume_monitor);
            for (lDrive = current_drive_list; lDrive != NULL; lDrive = lDrive->next) {
                GDrive *gdrive = (GDrive *)lDrive->data;
                char *devPath = g_drive_get_identifier(gdrive,G_DRIVE_IDENTIFIER_KIND_UNIX_DEVICE);
                if (devPath != NULL) {
                    string strId = devPath;
                    if (strId == driveInfo.strId) {
                        devDrive = gdrive;
                        g_free(devPath);
                        break;
                    }
                    g_free(devPath);
                }
            }
            if (devDrive != NULL) {
                if (!g_drive_has_volumes(devDrive)) {
                    Q_EMIT this->deviceError(devDrive);
                }
            }
            if (current_drive_list) {
                g_list_free(current_drive_list);
                current_drive_list = NULL;
            }
        }
    }
}

bool MainWindow::getDataCDRomCapacity(QString strDevId, quint64 &totalCapacity)
{
    if (!strDevId.startsWith("/dev/sr")) {
        return false;
    }

    quint64 uTotalCapacity = 0;
    DataCDROM *cdrom = new DataCDROM(strDevId);
    if (cdrom) {
        cdrom->getCDROMInfo();
        uTotalCapacity = cdrom->getCDROMCapacity();
        delete cdrom;
        cdrom = nullptr;
    }
    if (uTotalCapacity > 0) {
        totalCapacity = uTotalCapacity;
        return true;
    } else {
        return false;
    }
}

void MainWindow::getDriveIconsInfo(GDrive* drive, FDDriveInfo& driveInfo)
{
    GIcon *g_icon = g_drive_get_icon(drive);
    if (g_icon) {
        if (G_IS_THEMED_ICON(g_icon)) {
            const gchar* const* icon_names = g_themed_icon_get_names(G_THEMED_ICON (g_icon));
            if (icon_names) {
                driveInfo.strIconPath = *icon_names;
            }
        }
        g_object_unref(g_icon);
    }
}

void MainWindow::getVolumeIconsInfo(GVolume* volume, FDVolumeInfo& volumeInfo)
{
    GIcon *g_icon = g_volume_get_icon(volume);
    if (g_icon) {
        if (G_IS_THEMED_ICON(g_icon)) {
            const gchar* const* icon_names = g_themed_icon_get_names(G_THEMED_ICON (g_icon));
            if (icon_names) {
                volumeInfo.strIconPath = *icon_names;
            }
        }
        g_object_unref(g_icon);
    }
}

void MainWindow::getMountIconsInfo(GMount* mount, FDMountInfo& mountInfo)
{
    GIcon *g_icon = g_mount_get_icon(mount);
    if (g_icon) {
        if (G_IS_ICON(g_icon)) {
            if (G_IS_THEMED_ICON(g_icon)) {
                const gchar* const* icon_names = g_themed_icon_get_names(G_THEMED_ICON (g_icon));
                if (icon_names) {
                    mountInfo.strIconPath = *icon_names;
                }
            } else {
                // if it's a bootable-media,maybe we can get the icon from the mount directory.
                char *bootableIcon = g_icon_to_string(g_icon);
                if(bootableIcon) {
                    mountInfo.strIconPath = bootableIcon;
                    g_free(bootableIcon);
                }
            }
        }
        g_object_unref(g_icon);
    }
}
