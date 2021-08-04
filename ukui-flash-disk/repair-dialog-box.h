/*
 * Copyright (C) 2021 KylinSoft Co., Ltd.
 *
 * Authors:
 *  Ding Jing dingjing@kylinos.cn
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

#ifndef REPAIRDIALOGBOX_H
#define REPAIRDIALOGBOX_H

#include <QDialog>
#include <QCheckBox>
#include <QComboBox>
#include <QProgressBar>
#include <QTimer>
#include <QThread>
#include <QLabel>
#include <QLineEdit>
#include <QWidget>
#include <QMessageBox>

#include <gio/gio.h>
#include "device-operation.h"
#include "flashdiskdata.h"

class QGSettings;
class QDBusConnection;
class RepairProgressBar;

class BaseDialog : public QDialog
{
    Q_OBJECT
public:
    explicit BaseDialog(QWidget *parent = nullptr);
    QPalette getPalette();
    QPalette getBlackPalette();
    QPalette getWhitePalette();

    void setTheme();

private:
    QGSettings*             mGSettings = nullptr;
};

class RepairDialogBox : public BaseDialog
{
    Q_OBJECT
public:
    explicit RepairDialogBox(GDrive* drive, QWidget *parent = nullptr);
    explicit RepairDialogBox(GVolume* volume, QWidget *parent = nullptr);
    QString getDeviceName ();
    ~RepairDialogBox();

public Q_SLOTS:
    void onRemountDevice();

private:
    void initUI();
    void isRunning(bool running = false);
    static void drive_disconnected_callback(GVolumeMonitor* monitor, GDrive* drive, RepairDialogBox* pThis);

Q_SIGNALS:
    void repairOK (RepairDialogBox*);
    void remountDevice(FDVolumeInfo volumeInfo);

private:
    bool                mRepair = false;
    bool                mFormat = false;

    const int           mFixWidth = 480;
    const int           mFixHeight = 240;

    gulong              mVMConnect = 0;

    QPushButton*        mRepairBtn = nullptr;
    QPushButton*        mFormatBtn = nullptr;

    QString             mDeviceName = nullptr;
    GDrive*             mDrive = nullptr;
    GVolume*            mVolume = nullptr;
};

class RepairProgressBar : public BaseDialog
{
    Q_OBJECT
public:
    explicit RepairProgressBar(GDrive* drive, QWidget* parent = nullptr);
    explicit RepairProgressBar(GVolume* volume, QWidget* parent = nullptr);
    ~RepairProgressBar();

    int exec() override;

    void onStopRepair(bool);
    void onStartRepair();

private:
    void initUI ();

Q_SIGNALS:
    void cancel();
    void startRepair();
    void remountDevice();

private:
    int                 mProcess = 0;
    const int           mFixWidth = 480;
    const int           mFixHeight = 240;

    QTimer*             mTimer = nullptr;
    QProgressBar*       mProgress = nullptr;
    QPushButton*        mCancelBtn = nullptr;

    QThread*            mThread = nullptr;
    DeviceOperation*    mDeviceOperation = nullptr;

    GDrive*             mDrive = nullptr;
    GVolume*            mVolume = nullptr;
};

class FormateDialog : public BaseDialog
{
    Q_OBJECT
public:
    explicit FormateDialog(GDrive* drive, QWidget *parent = nullptr);
    explicit FormateDialog(GVolume* drive, QWidget *parent = nullptr);
    ~FormateDialog();
    int exec() override;

    void onStopFormat(bool);
    void onStartFormat();

private:
    void initUI();

Q_SIGNALS:
    void cancel();
    void startFormat(QString type, QString labelName);

private:
    const int           mFixWidth = 480;
    const int           mFixHeight = 440;

    QTimer*             mTimer = nullptr;

    QComboBox*          mFSCombox = nullptr;
    QProgressBar*       mProgress = nullptr;
    QPushButton*        mCancelBtn = nullptr;
    QPushButton*        mFormatBtn = nullptr;
    QCheckBox*          mEraseCkbox = nullptr;
    QLineEdit*          mNameEdit = nullptr;
    QComboBox*          mRomSizeCombox = nullptr;

    QThread*            mThread = nullptr;
    DeviceOperation*    mDeviceOperation = nullptr;

    GDrive*             mDrive = nullptr;
    GVolume*            mVolume = nullptr;

};

// tip dialog
class MessageBox : public BaseDialog
{
    Q_OBJECT
public:
    MessageBox(QString title, QString text, QMessageBox::StandardButtons bt, QWidget* parent = nullptr);

private:
};
#endif // REPAIRDIALOGBOX_H
