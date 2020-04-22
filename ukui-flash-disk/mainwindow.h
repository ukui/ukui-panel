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

#include "qclickwidget.h"
#include "UnionVariable.h"
#include "ejectInterface.h"

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
    void MainWindowShow();
private:
    Ui::MainWindow *ui;
    QVBoxLayout *vboxlayout;
    //QHBoxLayout *hboxlayout;
    QLabel *no_device_label;
    QPushButton *eject_image_button;
    void newarea(int No,
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
    void moveBottomRight();
    void moveBottomDirect(GDrive *drive);
    void moveBottomNoBase();

    QString size_human(qlonglong capacity);
    void getDeviceInfo();
    static void frobnitz_result_func_volume(GVolume *source_object,GAsyncResult *res,MainWindow *p_this);
    //static void frobnitz_result_func_volume(GVolume *source_object,GAsyncResult *res,gpointer);
    static void drive_connected_callback (GVolumeMonitor *monitor, GDrive *drive, MainWindow *p_this);
    static void drive_disconnected_callback (GVolumeMonitor *monitor, GDrive *drive, MainWindow *p_this);
    static void volume_added_callback (GVolumeMonitor *monitor, GVolume *volume, MainWindow *p_this);
    static void volume_removed_callback (GVolumeMonitor *monitor, GVolume *volume, gpointer);
    static void mount_added_callback (GVolumeMonitor *monitor, GMount *mount, gpointer);
    static void mount_removed_callback (GVolumeMonitor *, GMount *mount, gpointer);

private:
    QIcon iconSystray;
    QString UDiskPathDis1;
    QString UDiskPathDis2;
    QString UDiskPathDis3;
    QString UDiskPathDis4;
    quint64 totalDis1;
    quint64 totalDis2;
    quint64 totalDis3;
    quint64 totalDis4;
    QClickWidget *open_widget;
    int hign;
    int VolumeNum;
    QTimer *interfaceHideTime;
    int num = 0;
    QScreen *screen;
public:
    QSystemTrayIcon *m_systray;
    ejectInterface *m_eject;

public Q_SLOTS:
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    int getPanelPosition(QString str);
    int getPanelHeight(QString str);
    void onConvertShowWindow();
    void on_Maininterface_hide();
Q_SIGNALS:
    void clicked();
    void convertShowWindow();

protected:
    void resizeEvent(QResizeEvent *event);
};

#endif
