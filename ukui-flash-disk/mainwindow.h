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

#include "qclickwidget.h"
#include "UnionVariable.h"
#include "ejectInterface.h"
#include "mainwindow.h"
#include "MacroFile.h"
#include "flashdiskdata.h"
#include "fdclickwidget.h"


namespace Ui {
class MainWindow;
}

//struct UMount
//{
//    QString id;
//    int count;
//};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void MainWindowShow(bool isUpdate = false);

protected:
    void hideEvent(QHideEvent event);

private:
    Ui::MainWindow *ui;
    QVBoxLayout *vboxlayout;
    //QHBoxLayout *hboxlayout;
    QLabel *no_device_label;
    QPushButton *eject_image_button;
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

private:
    QIcon iconSystray;
//    QString UDiskPathDis1;
//    QString UDiskPathDis2;
//    QString UDiskPathDis3;
//    QString UDiskPathDis4;
    char *UDiskPathDis1;
    char *UDiskPathDis2;
    char *UDiskPathDis3;
    char *UDiskPathDis4;
    quint64 totalDis1;
    quint64 totalDis2;
    quint64 totalDis3;
    quint64 totalDis4;
//    QClickWidget *open_widget;
    QClickWidget *open_widget;
    FDClickWidget *m_fdClickWidget;
    int hign;
    int VolumeNum;
    QTimer *interfaceHideTime;
    int num = 0;
    QScreen *screen;
    int triggerType = 0; //detective the type of MainWinow(insert USB disk or click systemtray icon)

    char *mount_uri;
    GFile *root;
    char *mount_uriSecond;
    GFile *rootSecond;
    GList *listVolumes;
    GList *vList;
    GVolume *volume;

    double m_transparency;
    QString currentThemeMode;

    QGSettings *m_transparency_gsettings = nullptr;
    QGSettings *qtSettings = nullptr;

    QWidget *line = nullptr;
    bool ifautoload;
    bool insertorclick;

    QGSettings * ifsettings;
    int telephoneNum;
    QString tmpPath;
    bool findPointMount;
    QList<GMount *> volumeDevice;
    QMap<GDrive *,QList<GMount *>> deviceMap;
    QMap<GDrive *,QList<GMount *>>::Iterator it;
    int driveVolumeNum;
    FlashDiskData* m_dataFlashDisk;
    bool m_bIsMouseInTraIcon = false;
    bool m_bIsMouseInCentral = false;
    //authority
    //QDBusInterface *systemIface;
public:
    QSystemTrayIcon *m_systray;
    ejectInterface *m_eject;
    interactiveDialog *chooseDialog = nullptr;

    void initTransparentState();
    void initThemeMode();
//    double getTransparentData();
    void getTransparentData();
    int getPanelPosition(QString str);
    int getPanelHeight(QString str);
    bool ifSucess;
    int flagType;
//    static bool isShow;
    int driveMountNum;

public Q_SLOTS:
    void iconActivated(QSystemTrayIcon::ActivationReason reason);    
    void onConvertShowWindow();
    void onConvertUpdateWindow(QString strDevName, unsigned uDevType);
    void onMaininterfacehide();
    void on_clickPanelToHideInterface();
    void onRequestSendDesktopNotify(QString message);
    void onInsertAbnormalDiskNotify(QString message);
    void onNotifyWnd(QObject* obj, QEvent *event);
    void onClickedEjectItem(FDClickWidget* pThis, QString strDriveId, QString strVolumeId, QString strMountId);
Q_SIGNALS:
    void clicked();
    void convertShowWindow();
    void convertUpdateWindow(QString, unsigned);
    void unloadMount();
    void telephoneMount();
protected:
    void resizeEvent(QResizeEvent *event);
    bool eventFilter(QObject *obj, QEvent *event);
    void paintEvent(QPaintEvent *event);
};

#endif
