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
#include <QProxyStyle>

#include <gio/gio.h>
#include "device-operation.h"
#include "flashdiskdata.h"
#include "MacroFile.h"

class QGSettings;
class QDBusConnection;
class RepairProgressBar;

class BaseDialogStyle : public QProxyStyle
{
    Q_OBJECT
public:
    static BaseDialogStyle* getStyle();

public:
    void polish (QWidget* w) override;

    void drawControl(QStyle::ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = nullptr) const override;
    void drawComplexControl(ComplexControl control, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget) const override;

    void drawPrimitive(PrimitiveElement elem, const QStyleOption *option, QPainter *painter, const QWidget *widget) const override;
    void drawItemText(QPainter *painter, const QRect &rect, int flags, const QPalette &pal, bool enabled, const QString &text, QPalette::ColorRole textRole = QPalette::NoRole) const override;

private:
    BaseDialogStyle ();
    ~BaseDialogStyle ();

private:
    QPalette mPalette;
    static BaseDialogStyle* gInstance;
};

class BaseDialog : public QDialog
{
    Q_OBJECT
public:
    explicit BaseDialog(QWidget *parent = nullptr);
    QPalette getPalette();
    QPalette getBlackPalette();
    QPalette getWhitePalette();

    void setTheme();

Q_SIGNALS:
    void themeChanged ();

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
    static bool mshowflag;
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

Q_SIGNALS:
    void format();
};
#endif // REPAIRDIALOGBOX_H
