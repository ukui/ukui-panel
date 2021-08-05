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

#include "repair-dialog-box.h"

#include <QDebug>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QButtonGroup>
#include <QButtonGroup>
#include <QMessageBox>
#include <QApplication>
#include <QDBusReply>
#include <QStyleFactory>
#include <QProgressBar>
#include <QComboBox>
#include <QCheckBox>
#include <QScrollArea>
#include <QThread>
#include <QStyle>
#include <QGSettings>

#include <gio/gio.h>

#include "device-manager.h"


#define UDISK_DBUS_NAME                 "org.freedesktop.UDisks2"
#define UDISK_BLOCK_DBUS_PATH           "/org/freedesktop/UDisks2/block_devices/"


RepairDialogBox::RepairDialogBox(GDrive* drive, QWidget* parent) : BaseDialog(parent), mDrive(G_DRIVE(drive))
{
    initUI();

    if (mDrive) {
        g_object_ref(mDrive);
        mDeviceName = g_drive_get_identifier (drive, G_DRIVE_IDENTIFIER_KIND_UNIX_DEVICE);
    }

    connect(mRepairBtn, &QPushButton::clicked, this, [=] (bool) {
        RepairProgressBar dlg(mDrive);
        isRunning(true);
        int ret = dlg.exec();
        isRunning(false);
        if (QDialog::Accepted == ret) {
            accept();
        }
    });

    connect(mFormatBtn, &QPushButton::clicked, this, [=] (bool) {
        FormateDialog dlg(mDrive);
        isRunning(true);
        int ret = dlg.exec();
        isRunning(false);
        if (QDialog::Accepted == ret) {
            accept();
        }
    });
}

RepairDialogBox::RepairDialogBox(GVolume* volume, QWidget* parent) : BaseDialog(parent), mVolume(G_VOLUME(volume))
{
    initUI();

    if (mVolume) {
        g_object_ref(mVolume);
        mDeviceName = g_volume_get_identifier (mVolume, G_DRIVE_IDENTIFIER_KIND_UNIX_DEVICE);
    }

    connect(mRepairBtn, &QPushButton::clicked, this, [=] (bool) {
        RepairProgressBar dlg(mVolume, this);
        connect(&dlg, &RepairProgressBar::remountDevice, this, &RepairDialogBox::onRemountDevice);
        isRunning(true);
        int ret = dlg.exec();
        isRunning(false);
        if (QDialog::Accepted == ret) {
            accept();
        }
    });

    connect(mFormatBtn, &QPushButton::clicked, this, [=] (bool) {
        FormateDialog dlg(mVolume, this);
        isRunning(true);
        int ret = dlg.exec();
        isRunning(false);
        if (QDialog::Accepted == ret) {
            accept();
        }
    });
}

void RepairDialogBox::onRemountDevice()
{
    FDVolumeInfo volumeInfo;
    volumeInfo.strId = mDeviceName.toStdString();
    qInfo()<<"volumeId:"<<volumeInfo.strId.c_str();
    Q_EMIT remountDevice(volumeInfo);
}

QString RepairDialogBox::getDeviceName()
{
    return mDeviceName;
}

RepairDialogBox::~RepairDialogBox()
{
    if (mDrive)             g_object_unref(mDrive);
    if (mVolume)            g_object_unref(mVolume);
}

void RepairDialogBox::initUI()
{
    setAutoFillBackground(true);
    setWindowTitle(tr("Disk test"));
    setBackgroundRole(QPalette::Base);
    setContentsMargins(24, 24, 24, 24);
    setFixedSize(mFixWidth, mFixHeight);

    setWindowIcon(QIcon::fromTheme("system-file-manager"));
    setWindowFlags(windowFlags() & ~Qt::WindowCloseButtonHint);

    QGridLayout* mainLayout = new QGridLayout(this);
    mainLayout->setContentsMargins(0,0,0,0);
    QHBoxLayout* btnGroup = new QHBoxLayout;

    QLabel* label = new QLabel;
    label->setBackgroundRole(QPalette::Base);
    label->setWordWrap(true);
    label->setTextFormat(Qt::RichText);
    label->setText(tr(""
                      "<h4>The system could not recognize the disk contents</h4>"
                      "<p>Check that the disk and drive are properly connected, "
                      "make sure the disk is not a read-only disk, and try again. "
                      "For more information, search for help on read-only files and"
                      " how to change read-only files.</p>"));
//    QScrollArea* scrollArea = new QScrollArea;
//    scrollArea->setContentsMargins(0, 0, 0, 0);
//    scrollArea->setBackgroundRole(QPalette::Base);
//    scrollArea->setAutoFillBackground(true);
//    scrollArea->setFrameStyle(0);
//    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//    scrollArea->setWidget(label);
    mainLayout->addWidget(label, 0, 0, 1, 2, Qt::AlignTop);

    mFormatBtn = new QPushButton(tr("Format disk"));
    mRepairBtn = new QPushButton(tr("Repair"));

    btnGroup->addWidget(mRepairBtn);
    btnGroup->addWidget(mFormatBtn);
    mainLayout->addLayout(btnGroup, 1, 1, 1, 1);

    connect(this, &QDialog::finished, this, [=] () {
        Q_EMIT repairOK(this);
        deleteLater();
    });

    connect(this, &QDialog::accepted, this, [=] () {
        Q_EMIT repairOK(this);
        deleteLater();
    });

    const DeviceManager* dm = DeviceManager::getInstance();
    connect(dm, &DeviceManager::driveDisconnected, this, [=] (QString device) {
        g_return_if_fail(false);
        g_return_if_fail(mDrive || mVolume);
        QString devName;
        if (mDrive) {
            g_autofree gchar* dev = g_drive_get_identifier(mDrive, G_DRIVE_IDENTIFIER_KIND_UNIX_DEVICE);
            devName = dev;
        } else if (mVolume) {
            GDrive* driv = g_volume_get_drive(mVolume);
            if (driv) {
                g_autofree gchar* dev = g_drive_get_identifier(driv, G_DRIVE_IDENTIFIER_KIND_UNIX_DEVICE);
                devName = dev;
                g_object_unref(driv);
            }
        }

        if (devName == device || (!device.isEmpty() && devName.isEmpty())) {
            accept();
        }
    });
}

void RepairDialogBox::isRunning(bool running)
{
    if (running) {
        mRepairBtn->setDisabled(true);
        mFormatBtn->setDisabled(true);
    } else {
        mRepairBtn->setEnabled(true);
        mFormatBtn->setEnabled(true);
    }
}

void RepairDialogBox::drive_disconnected_callback(GVolumeMonitor* monitor, GDrive* drive, RepairDialogBox* pThis)
{
    g_return_if_fail(drive);
    g_return_if_fail(!pThis->isHidden());

    g_autofree gchar*       tDev = nullptr;

    g_autofree char* devPath = g_drive_get_identifier(drive, G_DRIVE_IDENTIFIER_KIND_UNIX_DEVICE);
    if (devPath != NULL) {
        if (pThis->mVolume) {
            GDrive* driv = g_volume_get_drive(pThis->mVolume);
            if (driv) {
                tDev = g_drive_get_identifier(driv, G_DRIVE_IDENTIFIER_KIND_UNIX_DEVICE);
                g_object_unref(driv);
            }
        } else if (pThis->mDrive) {
            tDev = g_drive_get_identifier(pThis->mDrive, G_DRIVE_IDENTIFIER_KIND_UNIX_DEVICE);
        }

        if (tDev && 0 == g_strcmp0(tDev, devPath)) {
            pThis->accept();
        }
    }

    Q_UNUSED(monitor)
}

RepairProgressBar::RepairProgressBar(GDrive* drive, QWidget *parent) : BaseDialog(parent), mDrive(G_DRIVE(drive))
{
    mThread = new QThread(this);
    mDeviceOperation = new DeviceOperation(mDrive);
    mDeviceOperation->moveToThread(mThread);

    initUI();
}

RepairProgressBar::RepairProgressBar(GVolume *volume, QWidget *parent) : BaseDialog(parent), mVolume(G_VOLUME(volume))
{
    mThread = new QThread(this);
    mDeviceOperation = new DeviceOperation(mVolume);
    mDeviceOperation->moveToThread(mThread);

    initUI();
}

RepairProgressBar::~RepairProgressBar()
{
    if (mTimer)             mTimer->deleteLater();
    if (mProgress)          mProgress->deleteLater();
    if (mCancelBtn)         mCancelBtn->deleteLater();
    if (mDeviceOperation)   mDeviceOperation->deleteLater();
}

int RepairProgressBar::exec()
{
    Q_EMIT startRepair();

    return QDialog::exec();
}

void RepairProgressBar::onStartRepair()
{
    mThread->start();
    mCancelBtn->setDisabled(true);
    mProgress->setValue(mProgress->minimum());
    mTimer->start();
}

void RepairProgressBar::initUI()
{
    setAutoFillBackground(true);
    setWindowTitle(tr("Disk repair"));
    setBackgroundRole(QPalette::Base);
    setContentsMargins(24, 24, 24, 24);
    setFixedSize(mFixWidth, mFixHeight);
    setWindowFlags(windowFlags() & ~Qt::WindowCloseButtonHint);

    mTimer = new QTimer;
    mTimer->setInterval(1000);

    QGridLayout* mainLayout = new QGridLayout(this);

    QLabel* label = new QLabel;
    label->setWordWrap(true);
    label->setTextFormat(Qt::RichText);
    label->setBackgroundRole(QPalette::Base);
    label->setText(tr("<h3>%1</h3>").arg(tr("Attempting a disk repair...")));
    mainLayout->addWidget(label, 1, 1, 1, 4);

    mProgress = new QProgressBar;
    mProgress->setMinimum(0);
    mProgress->setMaximum(1000);
    mainLayout->addWidget(mProgress, 2, 1, 1, 4, Qt::AlignTop);

    mCancelBtn = new QPushButton;
    mCancelBtn->setText(tr("Cancel"));
    mainLayout->addWidget(mCancelBtn, 3, 4, 1, 1);

    connect(this, &RepairProgressBar::startRepair, this, &RepairProgressBar::onStartRepair);
    connect(this, &RepairProgressBar::startRepair, mDeviceOperation, &DeviceOperation::udiskRepair);
    connect(this, &RepairProgressBar::cancel, mDeviceOperation, &DeviceOperation::udiskFormatCancel);
    connect(mDeviceOperation, &DeviceOperation::repairFinished, this, &RepairProgressBar::onStopRepair);
    connect(this, &QDialog::finished, [=] () {
        Q_EMIT cancel();
        mTimer->stop();
        mThread->exit();
    });

    connect(mTimer, &QTimer::timeout, this, [=] () {
        int val = mProgress->value();
        if (val <= mProgress->maximum()) {
            mProgress->setValue(++val);
        }
    });
}

void RepairProgressBar::onStopRepair(bool success)
{
    mCancelBtn->setEnabled(true);
    mProgress->setValue(mProgress->maximum());
    mTimer->stop();
    mThread->exit();

    if (success) {
        MessageBox msg(tr("Disk repair"), tr("Repair successfully!"), QMessageBox::Ok, this);
        msg.setPalette(getPalette());
        msg.exec();
        // remount device
        Q_EMIT remountDevice();
        accept();
    } else {
        MessageBox msg(tr("Disk repair"), tr("The repair completed. If the USB flash disk is not mounted, please try formatting the device!"), QMessageBox::Ok, this);
        msg.setPalette(getPalette());
        msg.exec();
        reject();
    }
}

FormateDialog::FormateDialog(GVolume* volume, QWidget *parent) : BaseDialog(parent), mVolume(volume)
{
    mThread = new QThread;
    mDeviceOperation = new DeviceOperation(mVolume);
    mDeviceOperation->moveToThread(mThread);

    initUI();
}

FormateDialog::~FormateDialog()
{
    if (mTimer)             mTimer->deleteLater();
    if (mThread)            mThread->deleteLater();
    if (mNameEdit)          mNameEdit->deleteLater();
    if (mFSCombox)          mFSCombox->deleteLater();
    if (mProgress)          mProgress->deleteLater();
    if (mCancelBtn)         mCancelBtn->deleteLater();
    if (mFormatBtn)         mFormatBtn->deleteLater();
    if (mEraseCkbox)        mEraseCkbox->deleteLater();
    if (mRomSizeCombox)     mRomSizeCombox->deleteLater();
    if (mDeviceOperation)   mDeviceOperation->deleteLater();
}

FormateDialog::FormateDialog(GDrive* drive, QWidget *parent) : BaseDialog(parent), mDrive(drive)
{
    mThread = new QThread(this);
    mDeviceOperation = new DeviceOperation(mDrive);
    mDeviceOperation->moveToThread(mThread);

    initUI();
}

int FormateDialog::exec()
{
    if (mDrive || mVolume) {
        mRomSizeCombox->clear();
        mRomSizeCombox->addItem(mDeviceOperation->udiskSize());
    }

    return QDialog::exec();
}

void FormateDialog::onStopFormat(bool success)
{
    mFormatBtn->setEnabled(true);
    mCancelBtn->setEnabled(true);

    mProgress->setValue(mProgress->maximum());
    mTimer->stop();
    mThread->exit();

    if (success) {
        MessageBox msg(tr("Disk format"), tr("Formatted successfully!"), QMessageBox::Ok, this);
        msg.exec();
        accept();
    } else {
        MessageBox msg(tr("Disk format"), tr("Formatting failed, please unplug the U disk and try again!"), QMessageBox::Ok, this);
        msg.exec();
        reject();
    }
}

void FormateDialog::onStartFormat()
{
    mProgress->setValue(mProgress->minimum());
    mTimer->start();

    mFormatBtn->setDisabled(true);
    mCancelBtn->setDisabled(true);
}

void FormateDialog::initUI()
{
    setAutoFillBackground(true);
    setWindowTitle(tr("Format"));
    setBackgroundRole(QPalette::Base);
    setContentsMargins(24, 24, 24, 24);
    setFixedSize(mFixWidth, mFixHeight);
    setWindowIcon(QIcon::fromTheme("system-file-manager"));
    setWindowFlags(windowFlags() & ~Qt::WindowCloseButtonHint);

    QGridLayout* mainLayout = new QGridLayout(this);

    QLabel* romSizeLabel = new QLabel;
    romSizeLabel->setText(tr("Rom size:"));
    mRomSizeCombox = new QComboBox;
    mainLayout->addWidget(romSizeLabel, 1, 1, 1, 2);
    mainLayout->addWidget(mRomSizeCombox, 1, 3, 1, 6);

    QLabel* fsLabel = new QLabel;
    fsLabel->setText(tr("Filesystem:"));
    mFSCombox = new QComboBox;
    mFSCombox->addItem("ntfs");
    mFSCombox->addItem("vfat");
    mFSCombox->addItem("ext4");
    mainLayout->addWidget(fsLabel, 2, 1, 1, 2);
    mainLayout->addWidget(mFSCombox, 2, 3, 1, 6);

    QLabel* uNameLabel = new QLabel;
    uNameLabel->setText(tr("Disk name:"));
    mNameEdit = new QLineEdit;
    mainLayout->addWidget(uNameLabel, 3, 1, 1, 2);
    mainLayout->addWidget(mNameEdit, 3, 3, 1, 6);

    QLabel* eraseLabel = new QLabel;
    eraseLabel->setWordWrap(true);
    eraseLabel->setText(tr("Completely erase(Time is longer, please confirm!)"));
    mEraseCkbox = new QCheckBox;
    mainLayout->addWidget(mEraseCkbox, 4, 1, 1, 1, Qt::AlignRight);
    mainLayout->addWidget(eraseLabel, 4, 2, 1, 6, Qt::AlignLeft);

    mProgress = new QProgressBar;
    mProgress->setMinimum(0);
    mProgress->setMaximum(1000);
    mainLayout->addWidget(mProgress, 5, 1, 1, 8);

    mCancelBtn = new QPushButton(tr("Cancel"));
    mFormatBtn = new QPushButton(tr("Format disk"));

    mainLayout->addWidget(mCancelBtn, 6, 5, 1, 2, Qt::AlignRight);
    mainLayout->addWidget(mFormatBtn, 6, 7, 1, 2, Qt::AlignRight);

    mTimer = new QTimer;
    mTimer->setInterval(1000);
    connect(mTimer, &QTimer::timeout, this, [=] () {
        int val = mProgress->value();
        if (val <= mProgress->maximum()) {
            mProgress->setValue(++val);
        }
    });

    connect(this, &FormateDialog::startFormat, this, &FormateDialog::onStartFormat);
    connect(this, &FormateDialog::startFormat, mDeviceOperation, &DeviceOperation::udiskFormat);
    connect(this, &FormateDialog::cancel, mDeviceOperation, &DeviceOperation::udiskFormatCancel);
    connect(mDeviceOperation, &DeviceOperation::formatFinished, this, &FormateDialog::onStopFormat);

    connect(mCancelBtn, &QPushButton::clicked, this, [=] (bool) {
        reject();
    });

    connect(this, &QDialog::finished, this, [=] () {
        Q_EMIT cancel();
        mTimer->stop();
        mThread->exit();
    });

    connect(mFormatBtn, &QPushButton::clicked, this, [=] (bool) {
        MessageBox msg(tr("Disk format"), tr("Formatting this volume will erase all data on it. Please back up all retained data before formatting. Do you want to continue?"), QMessageBox::Ok | QMessageBox::Cancel, this);
        if (QMessageBox::Cancel == msg.exec()) {
            return ;
        }

        mThread->start();
        QString ctype = mFSCombox->currentText();
        QString type = ctype.isEmpty() ? "vfat" : ctype;
        QString dlabel = mNameEdit->text();
        QString label = dlabel.isEmpty() ? mDeviceOperation->udiskLabel() : dlabel;

        Q_EMIT startFormat(type, label);
    });
}

BaseDialog::BaseDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle(tr("Disk test"));

    if (QGSettings::isSchemaInstalled("org.ukui.style")) {
        mGSettings = new QGSettings ("org.ukui.style", "/org/ukui/style/");

        connect(mGSettings, &QGSettings::changed, this, [=] (const QString &key) {
            if ("styleName" == key) {
                setPalette(getPalette());
                update();
            }
        });
    }

    setTheme();
}

QPalette BaseDialog::getPalette()
{
    if (mGSettings && QGSettings::isSchemaInstalled("org.ukui.style")) {
        QString value = mGSettings->get("styleName").toString();
        if ("ukui-default" == value) {
            return getWhitePalette();
        } else {
            return getBlackPalette();
        }
    }

    return getWhitePalette();
}

QPalette BaseDialog::getWhitePalette()
{
    auto palette = qApp->palette();

    QColor  window_bg(231,231,231),
        window_no_bg(233,233,233),
        base_bg(255,255,255),
        base_no_bg(248, 248, 248),
        font_bg(0,0,0),
        font_br_bg(255,255,255),
        font_di_bg(191,191,191),
        button_bg(217,217,217),
        button_ac_bg(107,142,235),
        button_di_bg(233,233,233),
        highlight_bg(61,107,229),
        tip_bg(248,248,248),
        tip_font(22,22,22),
        alternateBase(248,248,248);

    palette.setBrush(QPalette::Window,window_bg);
    palette.setBrush(QPalette::Active,QPalette::Window,window_bg);
    palette.setBrush(QPalette::Inactive,QPalette::Window,window_no_bg);
    palette.setBrush(QPalette::Disabled,QPalette::Window,window_no_bg);
    palette.setBrush(QPalette::WindowText,font_bg);
    palette.setBrush(QPalette::Active,QPalette::WindowText,font_bg);
    palette.setBrush(QPalette::Inactive,QPalette::WindowText,font_bg);
    palette.setBrush(QPalette::Disabled,QPalette::WindowText,font_di_bg);

    palette.setBrush(QPalette::Base,base_bg);
    palette.setBrush(QPalette::Active,QPalette::Base,base_bg);
    palette.setBrush(QPalette::Inactive,QPalette::Base,base_no_bg);
    palette.setBrush(QPalette::Disabled,QPalette::Base,base_no_bg);
    palette.setBrush(QPalette::Text,font_bg);
    palette.setBrush(QPalette::Active,QPalette::Text,font_bg);
    palette.setBrush(QPalette::Disabled,QPalette::Text,font_di_bg);

    //Cursor placeholder
#if (QT_VERSION >= QT_VERSION_CHECK(5,12,0))
    palette.setBrush(QPalette::PlaceholderText,font_di_bg);
#endif

    palette.setBrush(QPalette::ToolTipBase,tip_bg);
    palette.setBrush(QPalette::ToolTipText,tip_font);

    palette.setBrush(QPalette::Highlight,highlight_bg);
    palette.setBrush(QPalette::Active,QPalette::Highlight,highlight_bg);
    palette.setBrush(QPalette::HighlightedText,font_br_bg);

    palette.setBrush(QPalette::BrightText,font_br_bg);
    palette.setBrush(QPalette::Active,QPalette::BrightText,font_br_bg);
    palette.setBrush(QPalette::Inactive,QPalette::BrightText,font_br_bg);
    palette.setBrush(QPalette::Disabled,QPalette::BrightText,font_di_bg);

    palette.setBrush(QPalette::Button,button_bg);
    palette.setBrush(QPalette::Active,QPalette::Button,button_bg);
    palette.setBrush(QPalette::Inactive,QPalette::Button,button_bg);
    palette.setBrush(QPalette::Disabled,QPalette::Button,button_di_bg);
    palette.setBrush(QPalette::ButtonText,font_bg);
    palette.setBrush(QPalette::Inactive,QPalette::ButtonText,font_bg);
    palette.setBrush(QPalette::Disabled,QPalette::ButtonText,font_di_bg);

    palette.setBrush(QPalette::AlternateBase,alternateBase);
    palette.setBrush(QPalette::Inactive,QPalette::AlternateBase,alternateBase);
    palette.setBrush(QPalette::Disabled,QPalette::AlternateBase,button_di_bg);

    return palette;
}

QPalette BaseDialog::getBlackPalette()
{
    auto palette = qApp->palette();
    QColor
    window_bg(45,46,50),
    window_no_bg(48,46,50),
    base_bg(31,32,34),
    base_no_bg(28,28,30),
    font_bg(255,255,255),
    font_br_bg(255,255,255),
    font_di_bg(255,255,255),
    button_bg(61,61,65),
    button_ac_bg(48,48,51),
    button_di_bg(48,48,51),
    highlight_bg(61,107,229),
    tip_bg(61,61,65),
    tip_font(232,232,232),
    alternateBase(36,35,40);


    font_bg.setAlphaF(0.9);
    font_br_bg.setAlphaF(0.9);
    font_di_bg.setAlphaF(0.1);

    palette.setBrush(QPalette::Window,window_bg);
    palette.setBrush(QPalette::Active,QPalette::Window,window_bg);
    palette.setBrush(QPalette::Inactive,QPalette::Window,window_no_bg);
    palette.setBrush(QPalette::Disabled,QPalette::Window,window_no_bg);
    palette.setBrush(QPalette::WindowText,font_bg);
    palette.setBrush(QPalette::Active,QPalette::WindowText,font_bg);
    palette.setBrush(QPalette::Inactive,QPalette::WindowText,font_bg);
    palette.setBrush(QPalette::Disabled,QPalette::WindowText,font_di_bg);

    palette.setBrush(QPalette::Base,base_bg);
    palette.setBrush(QPalette::Active,QPalette::Base,base_bg);
    palette.setBrush(QPalette::Inactive,QPalette::Base,base_no_bg);
    palette.setBrush(QPalette::Disabled,QPalette::Base,base_no_bg);
    palette.setBrush(QPalette::Text,font_bg);
    palette.setBrush(QPalette::Active,QPalette::Text,font_bg);
    palette.setBrush(QPalette::Disabled,QPalette::Text,font_di_bg);

    //Cursor placeholder
#if (QT_VERSION >= QT_VERSION_CHECK(5,12,0))
    palette.setBrush(QPalette::PlaceholderText,font_di_bg);
#endif

    palette.setBrush(QPalette::ToolTipBase,tip_bg);
    palette.setBrush(QPalette::ToolTipText,tip_font);

    palette.setBrush(QPalette::Highlight,highlight_bg);
    palette.setBrush(QPalette::Active,QPalette::Highlight,highlight_bg);
    palette.setBrush(QPalette::HighlightedText,font_br_bg);

    palette.setBrush(QPalette::BrightText,font_br_bg);
    palette.setBrush(QPalette::Active,QPalette::BrightText,font_br_bg);
    palette.setBrush(QPalette::Inactive,QPalette::BrightText,font_br_bg);
    palette.setBrush(QPalette::Disabled,QPalette::BrightText,font_di_bg);

    palette.setBrush(QPalette::Button,button_bg);
    palette.setBrush(QPalette::Active,QPalette::Button,button_bg);
    palette.setBrush(QPalette::Inactive,QPalette::Button,button_bg);
    palette.setBrush(QPalette::Disabled,QPalette::Button,button_di_bg);
    palette.setBrush(QPalette::ButtonText,font_bg);
    palette.setBrush(QPalette::Inactive,QPalette::ButtonText,font_bg);
    palette.setBrush(QPalette::Disabled,QPalette::ButtonText,font_di_bg);

    palette.setBrush(QPalette::AlternateBase,alternateBase);
    palette.setBrush(QPalette::Inactive,QPalette::AlternateBase,alternateBase);
    palette.setBrush(QPalette::Disabled,QPalette::AlternateBase,button_di_bg);

    return palette;
}

void BaseDialog::setTheme()
{
    setPalette(getPalette());
}

MessageBox::MessageBox(QString title, QString text, QMessageBox::StandardButtons bt, QWidget *parent) : BaseDialog(parent)
{
    setFixedSize(420, 200);
    setContentsMargins(24, 24, 24, 24);
    setAutoFillBackground(true);
    setWindowTitle(title);
    setBackgroundRole(QPalette::Base);

    QGridLayout* mainLayout = new QGridLayout(this);

    QLabel* label = new QLabel;
    label->setText(text);
    label->setWordWrap(true);
    mainLayout->addWidget(label, 1, 1, 1, 4);

    QPushButton* ok = nullptr;
    QPushButton* cancel = nullptr;
    if (bt & QMessageBox::Ok && bt & QMessageBox::Cancel) {
        ok = new QPushButton;
        ok->setText(tr("OK"));
        mainLayout->addWidget(ok, 2, 3, 1, 1);
        cancel = new QPushButton;
        cancel->setText(tr("Cancel"));
        mainLayout->addWidget(cancel, 2, 4, 1, 1);
    } else if (bt & QMessageBox::Ok && !(bt & QMessageBox::Cancel)) {
        ok = new QPushButton;
        ok->setText(tr("OK"));
        mainLayout->addWidget(ok, 2, 4, 1, 1);
    } else if (!(bt & QMessageBox::Ok) && bt & QMessageBox::Cancel) {
        cancel = new QPushButton;
        cancel->setText(tr("Cancel"));
        mainLayout->addWidget(cancel, 2, 4, 1, 1);
    }

    if (ok)     connect(ok,     &QPushButton::clicked, this, [=] (bool) {done(QMessageBox::Ok);});
    if (cancel) connect(cancel, &QPushButton::clicked, this, [=] (bool) {done(QMessageBox::Cancel);});
}
