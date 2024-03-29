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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QtWidgets>
#include <QLabel>
#include <QPushButton>
#include <QSystemTrayIcon>
#include <QIcon>
#include <gio/gio.h>
#include <glib.h>
#include <QApplication>
#include <QEvent>
#include <qgsettings.h>
#include <QMap>
#include <vector>

using namespace std;

#include "qclickwidget.h"
#include "UnionVariable.h"
#include "ejectInterface.h"
#include "mainwindow.h"
#include "MacroFile.h"
#include "flashdiskdata.h"
#include "fdclickwidget.h"
#include "repair-dialog-box.h"

#define UDFAUTOMOUNT 0

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public:
    QSystemTrayIcon *m_systray;
    ejectInterface *m_eject = nullptr;
    interactiveDialog *chooseDialog = nullptr;
    bool m_ismountadd = false;
    bool ifSucess;
    int flagType;
    int driveMountNum;
    vector<string> m_vtDeviveId;
    EjectDeviceInfo m_curEjectDeviceInfo;

    void MainWindowShow(bool isUpdate = false);
    void initTransparentState();
    void initThemeMode();
    void getTransparentData();
    int getPanelPosition(QString str);
    int getPanelHeight(QString str);

protected:
    void hideEvent(QHideEvent event);
    void resizeEvent(QResizeEvent *event);
    bool eventFilter(QObject *obj, QEvent *event);
    void paintEvent(QPaintEvent *event);

private:
    Ui::MainWindow *ui;
    QVBoxLayout *vboxlayout;
    QLabel *no_device_label;
    QPushButton *eject_image_button;

    char *UDiskPathDis1;
    char *UDiskPathDis2;
    char *UDiskPathDis3;
    char *UDiskPathDis4;
    quint64 totalDis1;
    quint64 totalDis2;
    quint64 totalDis3;
    quint64 totalDis4;
    QClickWidget *open_widget;
    FDClickWidget *m_fdClickWidget;
    int hign;
    int VolumeNum;
    QTimer *interfaceHideTime;
    int num = 0;
    QScreen *screen;
    int triggerType = 0; //detective the type of MainWinow(insert USB disk or click systemtray icon)

    double m_transparency;
    QString currentThemeMode;

    QGSettings *m_transparency_gsettings = nullptr;

    QWidget *line = nullptr;
    bool ifautoload;
    bool insertorclick;

    QGSettings * ifsettings = nullptr;
    int telephoneNum;
    QString tmpPath;
    bool findPointMount;
    int driveVolumeNum;
    FlashDiskData* m_dataFlashDisk = nullptr;
    bool m_bIsMouseInTraIcon = false;
    bool m_bIsMouseInCentral = false;
    qint64 m_nAppStartTimestamp = 0; // 进程启动时的时间戳
    QString m_strSysRootDev;

    bool                                mIsrunning = false;
    QMap<QString, RepairDialogBox*>     mRepairDialog;

    void initFlashDisk();
    void initSlots();

    void newarea(int No,
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
                 int linestatus);
    void newarea(unsigned uDiskNo,
                 QString strDriveId,
                 QString strVolumeId,
                 QString strMountId,
                 QString driveName,
                 QString volumeName,
                 quint64 capacityDis,
                 QString strMountUri,
                 int linestatus);
    void moveBottomRight();
    void moveBottomDirect(GDrive *drive);
    void moveBottomNoBase();

    QString size_human(qlonglong capacity);
    void getDeviceInfo();
    static void frobnitz_result_func_volume(GVolume *source_object,GAsyncResult *res,MainWindow *p_this);
    static void frobnitz_result_func_mount(GMount *source_object,GAsyncResult *res,MainWindow *p_this);
    //static void frobnitz_result_func_volume(GVolume *source_object,GAsyncResult *res,gpointer);
    static void drive_connected_callback (GVolumeMonitor *monitor, GDrive *drive, MainWindow *p_this);
    static void drive_disconnected_callback (GVolumeMonitor *monitor, GDrive *drive, MainWindow *p_this);
    static void volume_added_callback (GVolumeMonitor *monitor, GVolume *volume, MainWindow *p_this);
    static void volume_removed_callback (GVolumeMonitor *monitor, GVolume *volume, MainWindow *p_this);
    static void mount_added_callback (GVolumeMonitor *monitor, GMount *mount, MainWindow *p_this);
    static void mount_removed_callback (GVolumeMonitor *monitor, GMount *mount, MainWindow *p_this);
    void ifgetPinitMount();
    static void frobnitz_force_result_func(GDrive *source_object,GAsyncResult *res,MainWindow *p_this);
    static void frobnitz_result_func(GDrive *source_object,GAsyncResult *res,MainWindow *p_this);
    static void frobnitz_normal_result_volume_eject(GVolume *source_object,GAsyncResult *res,MainWindow *p_this);
    static void frobnitz_force_result_unmount(GMount *source_object,GAsyncResult *res,MainWindow *p_this);
    static void AsyncUnmount(QString strMountRoot,MainWindow *p_this);

    void getSystemRootDev();
    bool isSystemRootDev(QString strDev);

    bool doRealEject(EjectDeviceInfo* peDeviceInfo, GMountUnmountFlags flag);
    static GAsyncReadyCallback fileEjectMountableCB(GFile *file, GAsyncResult *res, EjectDeviceInfo *peDeviceInfo);
    static void driveStopCb(GObject* object, GAsyncResult* res, EjectDeviceInfo *peDeviceInfo);

    bool getDataCDRomCapacity(QString strDevId, quint64 &totalCapacity);
    void getDriveIconsInfo(GDrive* drive, FDDriveInfo& driveInfo);
    void getVolumeIconsInfo(GVolume* volume, FDVolumeInfo& volumeInfo);
    void getMountIconsInfo(GMount* mount, FDMountInfo& mountInfo);
    bool getDevInterface(QString strDev);

public Q_SLOTS:
    void iconActivated(QSystemTrayIcon::ActivationReason reason);    
    void onConvertShowWindow(QString strDriveId, QString strMountUri);
    void onConvertUpdateWindow(QString strDevName, unsigned uDevType);
    void onMaininterfacehide();
    void on_clickPanelToHideInterface();
    void onRequestSendDesktopNotify(QString message, QString strIcon);
    void onInsertAbnormalDiskNotify(QString message);
    void onNotifyWnd(QObject* obj, QEvent *event);
    void onClickedEject(EjectDeviceInfo eDeviceInfo);
    void onRemountVolume(FDVolumeInfo volumeInfo);
    void onCheckDriveValid(FDDriveInfo driveInfo);

    bool onDeviceErrored(GDrive* drive);
    void onMountVolume(GVolume*);
    void onEjectVolumeForce(GVolume* v);             // A fix pops up if the mount fails

    void onNotifyDeviceRemoved(QString strDevId);

Q_SIGNALS:
    void clicked();
    void convertShowWindow(QString strDriveId, QString strMountUri);
    void convertUpdateWindow(QString, unsigned);
    void unloadMount();
    void telephoneMount();

    bool deviceError(GDrive*);
    void mountVolume(GVolume*);
    void ejectVolumeForce(GVolume*);
    void remountVolume(FDVolumeInfo volumeInfo);
    void checkDriveValid(FDDriveInfo driveInfo);
    void notifyDeviceRemoved(QString strDevId);
};

#endif
