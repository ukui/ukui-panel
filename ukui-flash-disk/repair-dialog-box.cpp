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
#include <QPainter>
#include <QStyleOption>
#include <QPalette>
#include <QPainterPath>
#include <QAction>
#include <QAbstractItemView>

#include <gio/gio.h>

#include "device-manager.h"


#define UDISK_DBUS_NAME                 "org.freedesktop.UDisks2"
#define UDISK_BLOCK_DBUS_PATH           "/org/freedesktop/UDisks2/block_devices/"

BaseDialogStyle* BaseDialogStyle::gInstance = NULL;

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
bool RepairDialogBox::mshowflag = false;

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

    setWindowIcon(QIcon::fromTheme("system-file-manager"));
    setWindowFlags(windowFlags() & ~Qt::WindowCloseButtonHint);

    QGSettings* fontSettings = nullptr;
    const QByteArray id1(THEME_QT_SCHEMA);
    static int fontSize;

    if(QGSettings::isSchemaInstalled(id1))
    {
        fontSettings = new QGSettings(id1);
    }

    fontSize = fontSettings->get(FONT_SIZE).toString().toFloat();
    if((fontSize>=14)&&(qgetenv ("GDM_LANG") == "en"))
    {
        setFixedSize(mFixWidth, mFixHeight+55);
    }else{
        setFixedSize(mFixWidth, mFixHeight);
    }
    QObject::connect(fontSettings,&QGSettings::changed,[=](QString key)
    {
       if(mshowflag){
           fontSize = fontSettings->get(FONT_SIZE).toString().toFloat();
           if((fontSize>=14)&&(qgetenv ("GDM_LANG") == "en"))
           {
               setFixedSize(mFixWidth, mFixHeight+55);
           }
           else{
               setFixedSize(mFixWidth, mFixHeight);
           }
       }
    });

    QGridLayout* mainLayout = new QGridLayout(this);
    mainLayout->setContentsMargins(0,0,0,0);
    QHBoxLayout* btnGroup = new QHBoxLayout;

    g_autofree gchar* devName = mDrive ? DeviceOperation::getDriveLabel (mDrive) : DeviceOperation::getDriveLabel (mVolume);

    QLabel* label = new QLabel;
    label->setBackgroundRole(QPalette::Base);
    label->setWordWrap(true);
    label->setTextFormat(Qt::RichText);
    if (devName) {
        label->setText(tr(
                          "<h4>The system could not recognize the disk contents</h4>"
                          "<p>Check that the disk/drive '%1' is properly connected,"
                          "make sure the disk is not a read-only disk, and try again."
                          "For more information, search for help on read-only files and"
                          "how to change read-only files.</p>").arg (devName));
    } else {
        label->setText(tr(
                          "<h4>The system could not recognize the disk contents</h4>"
                          "<p>Check that the disk/drive is properly connected,"
                          "make sure the disk is not a read-only disk, and try again."
                          "For more information, search for help on read-only files and"
                          "how to change read-only files.</p>"));
    }
//    QScrollArea* scrollArea = new QScrollArea;
//    scrollArea->setContentsMargins(0, 0, 0, 0);
//    scrollArea->setBackgroundRole(QPalette::Base);
//    scrollArea->setAutoFillBackground(true);
//    scrollArea->setFrameStyle(0);
//    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//    scrollArea->setWidget(label);
    mainLayout->addWidget(label, 0, 0, 6, 60, Qt::AlignTop);

    mFormatBtn = new QPushButton(tr("Format disk"));
    mRepairBtn = new QPushButton(tr("Repair"));

    btnGroup->addWidget(mRepairBtn);
    btnGroup->addWidget(mFormatBtn);
    mainLayout->addLayout(btnGroup, 5, 30, 1, 30);

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
        MessageBox msg(tr("Disk repair"), tr("Repair failed. If the USB flash disk is not mounted, please try formatting the device!"), QMessageBox::Ok | QMessageBox::Cancel, this);

        connect(&msg, &MessageBox::format, this, [=](){
            FormateDialog dlg(mDrive);
            int ret = dlg.exec();
            if (QDialog::Accepted == ret) {
                accept();
            }
        });
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
    mFSCombox->addItem("ext4");
    mFSCombox->addItem("exfat");
    mFSCombox->addItem("vfat/fat32");
    mainLayout->addWidget(fsLabel, 2, 1, 1, 2);
    mainLayout->addWidget(mFSCombox, 2, 3, 1, 6);
    if (BaseDialog* wi = qobject_cast<BaseDialog*>(this)) {
        QAbstractItemView * popuView = mFSCombox->view();
        popuView->setPalette(wi->palette());
        popuView = mRomSizeCombox->view();
        popuView->setPalette(wi->palette());
    }

    QLabel* uNameLabel = new QLabel;
    uNameLabel->setText(tr("Disk name:"));
    mNameEdit = new QLineEdit;
    mainLayout->addWidget(uNameLabel, 3, 1, 1, 2);
    mainLayout->addWidget(mNameEdit, 3, 3, 1, 6);

    QLabel* eraseLabel = new QLabel;
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
        type = ("vfat/fat32" == type) ? "vfat" : type;
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
                //setStyleSheet ("QCheckBox{margin:3px;}");
                QPalette p = getPalette();
                for (auto obj : children ()) {
                    if (QWidget* w = qobject_cast<QWidget*>(obj)) {
                        w->setPalette (p);
                        w->update ();
                        if (QComboBox * combo = qobject_cast<QComboBox*>(obj)) {
                            QAbstractItemView * popuView = combo->view();
                            popuView->setPalette(p);
                            popuView->update();
                        }
                    }
                }
                setPalette(p);
                update();
            }
        });
    }

    setTheme();

    if ((qgetenv ("DESKTOP_SESSION") == "ukui" && qgetenv ("XDG_SESSION_TYPE") == "x11")||
		(qgetenv ("DESKTOP_SESSION") == "ukui-wayland" && qgetenv ("XDG_SESSION_TYPE") == "wayland")) {
        setStyle (BaseDialogStyle::getStyle ());
        //setStyleSheet ("QCheckBox{margin:3px;}");
    }
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

    QColor window_bg(231,231,231),
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

    palette.setBrush(QPalette::WindowText,font_bg);

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
        ok->setText(tr("Format"));
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

    if (ok) {
        connect(ok, &QPushButton::clicked, this, [=] (bool) {
            Q_EMIT format();
            done(QMessageBox::Ok);
        });
    }
    if (cancel) {
        connect(cancel, &QPushButton::clicked, this, [=] (bool) {
            done(QMessageBox::Cancel);
        });
    }
}

BaseDialogStyle::BaseDialogStyle()
{

}

BaseDialogStyle::~BaseDialogStyle()
{

}

BaseDialogStyle *BaseDialogStyle::getStyle()
{
    if (!gInstance) {
        gInstance = new BaseDialogStyle;
    }

    return gInstance;
}

void BaseDialogStyle::polish(QWidget *w)
{
    if (BaseDialog* wi = qobject_cast<BaseDialog*>(w)) {
        mPalette = wi->getPalette ();
    }

    if (w) w->setPalette (mPalette);
}

void BaseDialogStyle::drawControl(QStyle::ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    switch (element) {
    case CE_ComboBoxLabel: {
        if (const QStyleOptionComboBox *comboBox = qstyleoption_cast<const QStyleOptionComboBox *>(option)) {
            QRect arrowRect = QProxyStyle::subControlRect(CC_ComboBox, comboBox, SC_ComboBoxArrow, widget);
            QRect editRect = QProxyStyle::subControlRect(CC_ComboBox, comboBox, SC_ComboBoxEditField, widget);

            QStyleOption arrow = *option;
            arrow.state = option->state & State_Enabled ? State_Enabled : State_None;
            arrow.rect = arrowRect;
            arrow.palette = mPalette;
            QProxyStyle::drawPrimitive(PE_IndicatorArrowDown, &arrow, painter, widget);

            painter->save();

            if (!comboBox->currentText.isEmpty() && !comboBox->editable) {
                drawItemText(painter, editRect, visualAlignment(option->direction, Qt::AlignLeft | Qt::AlignVCenter),
                                      mPalette, option->state & State_Enabled, comboBox->currentText);
            }
            painter->restore();
            return;
        }
    }
    break;

    case CE_PushButton: {
        if (const QStyleOptionButton *button = qstyleoption_cast<const QStyleOptionButton *>(option)) {
            drawControl(CE_PushButtonBevel, option, painter, widget);
            QStyleOptionButton subopt = *button;
            subopt.rect = QProxyStyle::subElementRect(SE_PushButtonContents, option, widget);
            subopt.palette = mPalette;
            drawControl(CE_PushButtonLabel, &subopt, painter, widget);
            return;
        }
    }
    break;

    case CE_PushButtonBevel: {
        drawPrimitive(PE_PanelButtonCommand, option, painter, widget);
    }
    break;

    case CE_PushButtonLabel: {
        if (const QStyleOptionButton *button = qstyleoption_cast<const QStyleOptionButton *>(option)) {
            const bool enable = button->state & State_Enabled;
            const bool text = !button->text.isEmpty();
            const bool icon = !button->icon.isNull();

            bool isWindowButton = false;
            bool isWindowColoseButton = false;
            bool isImportant = false;
            bool useButtonPalette = false;
            if (widget) {
                if (widget->property("isWindowButton").isValid()) {
                    if (widget->property("isWindowButton").toInt() == 0x01)
                        isWindowButton = true;
                    if (widget->property("isWindowButton").toInt() == 0x02)
                        isWindowColoseButton = true;
                }
                if (widget->property("isImportant").isValid())
                    isImportant = widget->property("isImportant").toBool();

                if (widget->property("useButtonPalette").isValid())
                    useButtonPalette = widget->property("useButtonPalette").toBool();
            }

            QRect drawRect = button->rect;
            int spacing = 8;
            QStyleOption sub = *option;
            if (isImportant && !(button->features & QStyleOptionButton::Flat)) {
                sub.state = option->state | State_On;
            } else if (isWindowButton || useButtonPalette) {
                sub.state = enable ? State_Enabled : State_None;
            } else {
                sub.state = option->state;
            }

            if (button->features & QStyleOptionButton::HasMenu) {
                QRect arrowRect;
                int indicator = proxy()->pixelMetric(PM_MenuButtonIndicator, option, widget);
                arrowRect.setRect(drawRect.right() - indicator, drawRect.top() + (drawRect.height() - indicator) / 2, indicator, indicator);
                arrowRect = visualRect(option->direction, option->rect, arrowRect);
                if (!text && !icon)
                    spacing = 0;
                drawRect.setWidth(drawRect.width() - indicator - spacing);
                drawRect = visualRect(button->direction, button->rect, drawRect);
                sub.rect = arrowRect;
                QProxyStyle::drawPrimitive(PE_IndicatorArrowDown, &sub, painter, widget);
            }

            int tf = Qt::AlignCenter;
            if (QProxyStyle::styleHint(SH_UnderlineShortcut, button, widget)) {
                tf |= Qt::TextShowMnemonic;
            }
            QPixmap pixmap;
            if (icon) {
                QIcon::Mode mode = button->state & State_Enabled ? QIcon::Normal : QIcon::Disabled;
                if (mode == QIcon::Normal && button->state & State_HasFocus)
                    mode = QIcon::Active;
                QIcon::State state = QIcon::Off;
                if (button->state & State_On)
                    state = QIcon::On;
                pixmap = button->icon.pixmap(button->iconSize, mode, state);
            }

            QFontMetrics fm = button->fontMetrics;
            int textWidth = fm.boundingRect(option->rect, tf, button->text).width() + 2;
            int iconWidth = icon ? button->iconSize.width() : 0;
            QRect iconRect, textRect;
            if (icon && text) {
                int width = textWidth + spacing + iconWidth;
                if (width > drawRect.width()) {
                    width = drawRect.width();
                    textWidth = width - spacing - iconWidth;
                }
                textRect.setRect(drawRect.x(), drawRect.y(), width, drawRect.height());
                textRect.moveCenter(drawRect.center());
                iconRect.setRect(textRect.left(), textRect.top(), iconWidth, textRect.height());
                textRect.setRect(iconRect.right() + spacing + 1, textRect.y(), textWidth, textRect.height());
                iconRect = visualRect(option->direction, drawRect, iconRect);
                textRect = visualRect(option->direction, drawRect, textRect);
            } else if (icon) {
                iconRect = drawRect;
            } else if (text) {
                textRect = drawRect;
            }

            if (textRect.isValid()) {
                if (enable) {
                    if (isWindowButton || useButtonPalette) {
                        drawItemText(painter, textRect, tf, button->palette, true, button->text, mPalette.ButtonText);
                    } else {
                        if (isImportant) {
                            if (button->features & QStyleOptionButton::Flat) {
                                drawItemText(painter, textRect, tf, mPalette, true, button->text, mPalette.ButtonText);
                            } else {
                                drawItemText(painter, textRect, tf, mPalette, true, button->text, mPalette.HighlightedText);
                            }
                            if (button->state & (State_MouseOver | State_Sunken | State_On)) {
                                drawItemText(painter, textRect, tf, mPalette, true, button->text, mPalette.HighlightedText);
                            }
                        } else {
                            if (button->state & (State_MouseOver | State_Sunken | State_On)) {
                                drawItemText(painter, textRect, tf, mPalette, true, button->text, mPalette.HighlightedText);
                            } else {
                                drawItemText(painter, textRect, tf, mPalette, true, button->text, mPalette.ButtonText);
                            }
                        }
                    }
                } else {
                    drawItemText(painter, textRect, tf, mPalette, false, button->text, mPalette.ButtonText);
                }
            }
            return;
        }
    }
    break;

    case CE_CheckBox: {
        if (const QStyleOptionButton *button = qstyleoption_cast<const QStyleOptionButton *>(option)) {
            QStyleOptionButton subopt = *button;
            subopt.palette = mPalette;
            subopt.rect = QProxyStyle::subElementRect(SE_CheckBoxIndicator, option, widget);
            drawPrimitive(PE_IndicatorCheckBox, &subopt, painter, widget);
            subopt.rect = QProxyStyle::subElementRect(SE_CheckBoxContents, option, widget);
            drawControl(CE_CheckBoxLabel, &subopt, painter, widget);
            return;
        }
    }
    break;

    case CE_RadioButtonLabel:
    case CE_CheckBoxLabel: {
        if (const QStyleOptionButton *button = qstyleoption_cast<const QStyleOptionButton *>(option)) {
            uint alignment = visualAlignment(button->direction, Qt::AlignLeft | Qt::AlignVCenter);
            const bool enable = button->state & State_Enabled;

            if (!proxy()->styleHint(SH_UnderlineShortcut, button, widget))
                alignment |= Qt::TextHideMnemonic;
            QPixmap pixmap;
            QRect textRect = button->rect;
            if (!button->icon.isNull()) {
                pixmap = button->icon.pixmap(button->iconSize, enable ? QIcon::Normal : QIcon::Disabled);
                drawItemPixmap(painter, button->rect, alignment, pixmap);
                int spacing = 8;
                if (button->direction == Qt::RightToLeft)
                    textRect.setRight(textRect.right() - button->iconSize.width() - spacing);
                else
                    textRect.setLeft(textRect.left() + button->iconSize.width() + spacing);
            }
            if (!button->text.isEmpty()){
                drawItemText(painter, textRect, alignment | Qt::TextShowMnemonic, mPalette, button->state & State_Enabled, button->text, mPalette.WindowText);
            }
            return;
        }
    }
    break;

    case CE_MenuItem:
        break;

        /* 进度条 */
    case CE_ProgressBar: {
        if (const QStyleOptionProgressBar *pb = qstyleoption_cast<const QStyleOptionProgressBar *>(option)) {
            QStyleOptionProgressBar subOption = *pb;
            subOption.palette = mPalette;
            subOption.rect = proxy()->subElementRect(SE_ProgressBarGroove, pb, widget);
            proxy()->drawControl(CE_ProgressBarGroove, &subOption, painter, widget);
            subOption.rect = proxy()->subElementRect(SE_ProgressBarContents, pb, widget);
            proxy()->drawControl(CE_ProgressBarContents, &subOption, painter, widget);
            if (pb->textVisible) {
                subOption.rect = proxy()->subElementRect(SE_ProgressBarLabel, pb, widget);
                proxy()->drawControl(CE_ProgressBarLabel, &subOption, painter, widget);
            }
            return;
        }
        break;
    }

    case CE_ProgressBarGroove: {
        const bool enable = option->state & State_Enabled;
        painter->save();
        painter->setPen(Qt::NoPen);
        painter->setBrush(mPalette.brush(enable ? mPalette.Active : mPalette.Disabled, mPalette.Button));
        painter->drawRoundedRect(option->rect, 4, 4);
        painter->restore();
        return;
    }

    case CE_ProgressBarContents: {
        if (const QStyleOptionProgressBar *pb = qstyleoption_cast<const QStyleOptionProgressBar *>(option)) {
            const auto progress = qMax(pb->progress, pb->minimum);
            if (progress == pb->minimum)
                return;

            const bool vertical = pb->orientation == Qt::Vertical;
            const bool inverted = pb->invertedAppearance;
            const bool indeterminate = (pb->minimum == 0 && pb->maximum == 0);

            QRect rect = pb->rect;
            int maxWidth = vertical ? pb->rect.height() : pb->rect.width();
            const auto totalSteps = qMax(Q_INT64_C(1), qint64(pb->maximum) - pb->minimum);
            const auto progressSteps = qint64(progress) - pb->minimum;
            const auto progressBarWidth = progressSteps * maxWidth / totalSteps;
            int len = indeterminate ? maxWidth : progressBarWidth;

            bool reverse = (!vertical && (pb->direction == Qt::RightToLeft)) || vertical;
            if (inverted)
                reverse = !reverse;

            QColor startColor = mPalette.color(mPalette.Active, mPalette.Highlight);
            QColor endColor = mPalette.color(mPalette.Active, mPalette.Highlight);
            QLinearGradient linearGradient;
            linearGradient.setColorAt(0, startColor);
            linearGradient.setColorAt(1, endColor);
            QRect progressRect;
            if (indeterminate) {

            } else {
                if (vertical) {
                    if (reverse) {
                        progressRect.setRect(rect.left(), rect.bottom() + 1 - len, rect.width(), len);
                        linearGradient.setStart(progressRect.bottomLeft());
                        linearGradient.setFinalStop(progressRect.topLeft());
                    } else {
                        progressRect.setRect(rect.x(), rect.top(), rect.width(), len);
                        linearGradient.setStart(progressRect.topLeft());
                        linearGradient.setFinalStop(progressRect.bottomLeft());
                    }
                } else {
                    if (reverse) {
                        progressRect.setRect(rect.right() + 1 - len, rect.top(), len, rect.height());
                        linearGradient.setStart(progressRect.topRight());
                        linearGradient.setFinalStop(progressRect.topLeft());
                    } else {
                        progressRect.setRect(rect.x(), rect.y(), len, rect.height());
                        linearGradient.setStart(progressRect.topLeft());
                        linearGradient.setFinalStop(progressRect.topRight());
                    }
                }
            }
            painter->save();
            painter->setPen(Qt::NoPen);
            painter->setBrush(linearGradient);
            painter->setRenderHint(QPainter::Antialiasing, true);
            painter->drawRoundedRect(progressRect, 4, 4);
            painter->restore();
            return;
        }
    }
    break;

    case CE_ProgressBarLabel: {
        if (const QStyleOptionProgressBar *pb = qstyleoption_cast<const QStyleOptionProgressBar *>(option)) {
            if (pb->textVisible) {
                const auto progress = qMax(pb->progress, pb->minimum);
                const bool vertical = pb->orientation == Qt::Vertical;
                const bool inverted = pb->invertedAppearance;
                const bool indeterminate = (pb->minimum == 0 && pb->maximum == 0);

                int maxWidth = vertical ? pb->rect.height() : pb->rect.width();
                const auto totalSteps = qMax(Q_INT64_C(1), qint64(pb->maximum) - pb->minimum);
                const auto progressSteps = qint64(progress) - pb->minimum;
                const auto progressBarWidth = progressSteps * maxWidth / totalSteps;
                int len = indeterminate ? maxWidth : progressBarWidth;

                bool reverse = (!vertical && (pb->direction == Qt::RightToLeft)) || vertical;
                if (inverted)
                    reverse = !reverse;

                painter->save();
                painter->setBrush(Qt::NoBrush);
                QRect rect = pb->rect;
                if (pb->orientation == Qt::Vertical) {
                    rect.setRect(rect.y(), rect.x(), rect.height(), rect.width());
                    QTransform m;
                    m.rotate(90);
                    m.translate(0, -rect.height());
                    painter->setTransform(m, true);
                }
                QRect textRect(rect.x(), rect.y(), pb->fontMetrics.horizontalAdvance(pb->text), rect.height());
                textRect.moveCenter(rect.center());
                if (len <= textRect.left()) {
                    painter->setPen(mPalette.color(mPalette.Active, mPalette.WindowText));
                    painter->drawText(textRect, pb->text, QTextOption(Qt::AlignAbsolute | Qt::AlignHCenter | Qt::AlignVCenter));
                } else if (len >= textRect.right()) {
                    painter->setPen(mPalette.color(mPalette.Active, mPalette.HighlightedText));
                    painter->drawText(textRect, pb->text, QTextOption(Qt::AlignAbsolute | Qt::AlignHCenter | Qt::AlignVCenter));
                } else {
                    QRect leftRect(textRect.x(), textRect.y(), len - textRect.left(), textRect.height());
                    QRect rightRect(leftRect.right() + 1, textRect.y(), textRect.right() + 1 - len, textRect.height());
                    if (reverse) {
                        leftRect.setRect(textRect.left(), textRect.top(), maxWidth - len - textRect.left(), textRect.height());
                        rightRect.setRect(leftRect.right() + 1, textRect.top(), textRect.width() - leftRect.width(), textRect.height());
                        painter->setPen(mPalette.color(mPalette.Active, mPalette.HighlightedText));
                        painter->setClipRect(rightRect);
                        painter->drawText(textRect, pb->text, QTextOption(Qt::AlignAbsolute | Qt::AlignHCenter | Qt::AlignVCenter));
                        painter->setPen(mPalette.color(mPalette.Active, mPalette.WindowText));
                        painter->setClipRect(leftRect);
                        painter->drawText(textRect, pb->text, QTextOption(Qt::AlignAbsolute | Qt::AlignHCenter | Qt::AlignVCenter));
                    } else {
                        painter->setPen(mPalette.color(mPalette.Active, mPalette.WindowText));
                        painter->setClipRect(rightRect);
                        painter->drawText(textRect, pb->text, QTextOption(Qt::AlignAbsolute | Qt::AlignHCenter | Qt::AlignVCenter));
                        painter->setPen(mPalette.color(mPalette.Active, mPalette.HighlightedText));
                        painter->setClipRect(leftRect);
                        painter->drawText(textRect, pb->text, QTextOption(Qt::AlignAbsolute | Qt::AlignHCenter | Qt::AlignVCenter));
                    }
                }
                painter->resetTransform();
                painter->restore();
            }
            return;
        }
    }
    break;

    default:
        QStyleOption opt = *option;
        opt.palette = mPalette;
        QProxyStyle::drawControl (element, &opt, painter, widget);
        break;
    }
    QProxyStyle::drawControl (element, option, painter, widget);
}

void BaseDialogStyle::drawPrimitive(PrimitiveElement elem, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    switch (elem) {
    case PE_PanelButtonTool: {
        QProxyStyle::drawPrimitive (elem, option, painter, widget);
        return;
        bool isWindowColoseButton = false;
        bool isWindowButton = false;
        bool useButtonPalette = false;
        if (widget) {
            if (widget->property("isWindowButton").isValid()) {
                if (widget->property("isWindowButton").toInt() == 0x01)
                    isWindowButton = true;
                if (widget->property("isWindowButton").toInt() == 0x02)
                    isWindowColoseButton = true;
            }
            if (widget->property("useButtonPalette").isValid())
                useButtonPalette = widget->property("useButtonPalette").toBool();
        }

        const bool enable = option->state & State_Enabled;
        const bool raise = option->state & State_AutoRaise;
        const bool sunken = option->state & State_Sunken;
        const bool hover = option->state & State_MouseOver;
        const bool on = option->state & State_On;

        if (!enable) {
            painter->save();
            painter->setPen(Qt::NoPen);
            if (on)
                painter->setBrush(mPalette.color(mPalette.Disabled, mPalette.Button));
            else if (raise)
                painter->setBrush(Qt::NoBrush);
            else
                painter->setBrush(mPalette.color(mPalette.Disabled, mPalette.Button));
            painter->setRenderHint(QPainter::Antialiasing, true);
            painter->drawRoundedRect(option->rect, 4, 4);
            painter->restore();
            return;
        }

        if (!raise) {
            painter->save();
            painter->setPen(Qt::NoPen);
            painter->setBrush(mPalette.color(mPalette.Active, mPalette.Button));
            painter->setRenderHint(QPainter::Antialiasing, true);
            painter->drawRoundedRect(option->rect, 4, 4);
            painter->restore();
        }

        painter->save();
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->setPen(Qt::NoPen);
        if (sunken || on) {
            if (isWindowButton) {
                QColor color = mPalette.color(mPalette.Active, mPalette.Base);
                color.setAlphaF(0.15);
                painter->setBrush(color);
            } else if (isWindowColoseButton) {
                painter->setBrush(QColor("#E44C50"));
            } else {
                painter->setBrush(mPalette.brush (mPalette.Active, mPalette.Highlight));
            }
        } else if (hover) {
            if (isWindowButton) {
                QColor color = mPalette.color(mPalette.Active, mPalette.Base);
                color.setAlphaF(0.1);
                painter->setBrush(color);
            } else if (isWindowColoseButton) {
                painter->setBrush(QColor("#F86458"));
            } else {
                painter->setBrush(mPalette.brush (mPalette.Active, mPalette.Highlight));
            }
        }
        painter->drawRoundedRect(option->rect, 4, 4);
        painter->restore();
        return;
        return;
    }
    break;
    case PE_PanelButtonCommand: {
        if (const QStyleOptionButton *button = qstyleoption_cast<const QStyleOptionButton *>(option)) {
            const bool enable = button->state & State_Enabled;
            const bool hover = button->state & State_MouseOver;
            const bool sunken = button->state & State_Sunken;
            const bool on = button->state & State_On;
            qreal x_Radius = 4;
            qreal y_Radius = 4;
            bool isWindowButton = false;
            bool isWindowColoseButton = false;
            bool isImportant = false;
            bool useButtonPalette = false;
            if (widget) {
                if (widget->property("isWindowButton").isValid()) {
                    if (widget->property("isWindowButton").toInt() == 0x01)
                        isWindowButton = true;
                    if (widget->property("isWindowButton").toInt() == 0x02)
                        isWindowColoseButton = true;
                }
                if (widget->property("isImportant").isValid())
                    isImportant = widget->property("isImportant").toBool();

                if (widget->property("useButtonPalette").isValid())
                    useButtonPalette = widget->property("useButtonPalette").toBool();

                if (qobject_cast<const QComboBox*>(widget) || qobject_cast<const QLineEdit*>(widget)
                    || qobject_cast<const QAbstractSpinBox*>(widget))
                    useButtonPalette = true;
            }

            if (!enable) {
                painter->save();
                painter->setPen(Qt::NoPen);
                if (on) {
                    painter->setBrush(Qt::NoBrush);
                } else if (button->features & QStyleOptionButton::Flat) {
                    painter->setBrush(Qt::NoBrush);
                } else {
                    painter->setBrush(mPalette.brush(mPalette.Disabled, mPalette.Button));
                }
                painter->setRenderHint(QPainter::Antialiasing, true);
                painter->drawRoundedRect(option->rect, x_Radius, y_Radius);
                painter->restore();
                return;
            }

            if (!(button->features & QStyleOptionButton::Flat)) {
                painter->save();
                painter->setPen(Qt::NoPen);
                if (isImportant)
                    painter->setBrush(mPalette.brush(mPalette.Active, mPalette.Highlight));
                else
                    painter->setBrush(mPalette.brush(mPalette.Active, mPalette.Button));
                painter->setRenderHint(QPainter::Antialiasing, true);
                painter->drawRoundedRect(option->rect, x_Radius, y_Radius);
                painter->restore();
            }

            painter->save();
            painter->setRenderHint(QPainter::Antialiasing,true);
            painter->setPen(Qt::NoPen);
            if (sunken || on) {
                if (isWindowColoseButton) {
                    painter->setBrush(QColor("#E44C50"));
                } else {
                    painter->setBrush(mPalette.brush (mPalette.Active, mPalette.Highlight));
                }
            } else if (hover) {
                if (isWindowColoseButton) {
                    painter->setBrush(QColor("#F86458"));
                } else {
                    painter->setBrush(mPalette.brush (mPalette.Active, mPalette.Highlight));
                }
            }
            painter->drawRoundedRect(button->rect, x_Radius, y_Radius);
            painter->restore();
            return;
        }
    }
    break;
    case PE_IndicatorCheckBox: {
        if (const QStyleOptionButton *checkbox = qstyleoption_cast<const QStyleOptionButton*>(option)) {
            const bool useDarkPalette = false;
            bool enable = checkbox->state & State_Enabled;
            bool mouseOver = checkbox->state & State_MouseOver;
            bool sunKen = checkbox->state & State_Sunken;
            bool on = checkbox->state & State_On;
            bool noChange = checkbox->state & State_NoChange;

            QRectF rect = checkbox->rect;
            int width = rect.width();
            int heigth = rect.height();
            int x_Radius = 4;
            int y_Radius = 4;

            QPainterPath path;
            if (on) {
                path.moveTo(width/4 + checkbox->rect.left(), heigth/2 + checkbox->rect.top());
                path.lineTo(width*0.45 + checkbox->rect.left(), heigth*3/4 + checkbox->rect.top());
                path.lineTo(width*3/4 + checkbox->rect.left(), heigth/4 + checkbox->rect.top());
            } else if (noChange){
                path.moveTo(rect.left() + width/4, rect.center().y());
                path.lineTo(rect.right() - width/4 , rect.center().y());
            }

            painter->save();
            painter->setClipRect(rect);
            painter->setRenderHint(QPainter::Antialiasing, true);
            if (enable) {
                if (on | noChange) {
                    if (sunKen) {
                        painter->setPen(QColor(25, 101, 207));
                        painter->setBrush(mPalette.brush(mPalette.Active, mPalette.Highlight));
                    } else if (mouseOver) {
                        painter->setPen(QColor(36, 109, 212));
                        painter->setBrush(mPalette.brush(mPalette.Active, mPalette.Highlight));
                    } else {
                        painter->setPen(QColor(36, 109, 212));
                        painter->setBrush(mPalette.brush(mPalette.Active, mPalette.Highlight));
                    }
                    painter->drawRoundedRect(rect, x_Radius, y_Radius);

                    painter->setPen(QPen(mPalette.brush(mPalette.Active, mPalette.HighlightedText), 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
                    painter->setBrush(Qt::NoBrush);
                    painter->drawPath(path);
                } else {
                    if (sunKen) {
                        if (useDarkPalette) {
                            painter->setPen(QColor(36, 109, 212));
                            painter->setBrush(QColor(6, 35, 97));
                        } else {
                            painter->setPen(QColor(36, 109, 212));
                            painter->setBrush(QColor(179, 221, 255));
                        }
                    } else if (mouseOver) {
                        if (useDarkPalette) {
                            painter->setPen(QColor(55, 144, 250));
                            painter->setBrush(QColor(9, 53, 153));
                        } else {
                            painter->setPen(QColor(97, 173, 255));
                            painter->setBrush(QColor(219, 240, 255));
                        }
                    } else {
                        if (useDarkPalette) {
                            painter->setPen(QColor(72, 72, 77));
                            painter->setBrush(QColor(48, 48, 51));
                        } else {
                            painter->setPen(QColor(191, 191, 191));
                            painter->setBrush(mPalette.color(mPalette.Active, mPalette.Window));
                        }
                    }
                    painter->drawRoundedRect(rect, x_Radius, y_Radius);
                }
            } else {
                if (useDarkPalette) {
                    painter->setPen(QColor(48, 48, 51));
                    painter->setBrush(QColor(28, 28, 30));
                } else {
                    painter->setPen(QColor(224, 224, 224));
                    painter->setBrush(QColor(233, 233, 233));
                }
                painter->drawRoundedRect(rect, x_Radius, y_Radius);
                if (on | noChange) {
                    painter->setPen(QPen(mPalette.brush(mPalette.Disabled, mPalette.ButtonText), 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
                    painter->setBrush(Qt::NoBrush);
                    painter->drawPath(path);
                }
            }
            painter->restore();
            return;
        }
    }
    break;
    case PE_PanelLineEdit: {
        if (widget) {
            if (QAction *clearAction = widget->findChild<QAction *>(QLatin1String("_q_qlineeditclearaction"))) {
                QStyleOption subOption = *option;
                subOption.palette = mPalette;
                QColor color = subOption.palette.color(QPalette::Text);
                color.setAlphaF(1.0);
                subOption.palette.setColor(QPalette::Text, color);
            }
        }

        if (widget) {
            if (widget->parentWidget())
                if (widget->parentWidget()->inherits("QDoubleSpinBox")|| widget->parentWidget()->inherits("QSpinBox")
                    || widget->parentWidget()->inherits("QComboBox") || widget->parentWidget()->inherits("QDateTimeEdit")) {
                    return;
                }
        }

        if (const QStyleOptionFrame *f = qstyleoption_cast<const QStyleOptionFrame *>(option)) {
            const bool enable = f->state & State_Enabled;
            const bool focus = f->state & State_HasFocus;

            if (!enable) {
                painter->save();
                painter->setPen(Qt::NoPen);
                painter->setBrush(mPalette.brush(mPalette.Disabled, mPalette.Button));
                painter->setRenderHint(QPainter::Antialiasing, true);
                painter->drawRoundedRect(option->rect, 4, 4);
                painter->restore();
                return;
            }

            if (f->state & State_ReadOnly) {
                painter->save();
                painter->setPen(Qt::NoPen);
                painter->setBrush(mPalette.brush(mPalette.Active, mPalette.Button));
                painter->setRenderHint(QPainter::Antialiasing, true);
                painter->drawRoundedRect(option->rect, 4, 4);
                painter->restore();
                return;
            }

            if (focus) {
                painter->save();
                painter->setPen(QPen(mPalette.brush(mPalette.Active, mPalette.Highlight), 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
                painter->setBrush(mPalette.brush(mPalette.Active, mPalette.Base));
                painter->setRenderHint(QPainter::Antialiasing, true);
                painter->drawRoundedRect(option->rect.adjusted(1, 1, -1, -1), 4, 4);
                painter->restore();
            } else {
                QStyleOptionButton button;
                button.state = option->state & ~(State_Sunken | State_On);
                button.palette = mPalette;
                button.rect = option->rect;
                drawPrimitive(PE_PanelButtonCommand, &button, painter, widget);

                if (f->state & State_MouseOver) {
                    QRectF rect = f->rect;
                    painter->save();
                    painter->setPen(QPen(mPalette.brush(mPalette.Active, mPalette.Highlight), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
                    painter->setBrush(Qt::NoBrush);
                    painter->setRenderHint(QPainter::Antialiasing, true);
                    painter->drawRoundedRect(rect.adjusted(0.5, 0.5, -0.5, -0.5), 4, 4);
                    painter->restore();
                }
            }
            return;
        }
    }
    break;
    default:
        QStyleOption opt = *option;
        opt.palette = mPalette;
        QProxyStyle::drawPrimitive (elem, &opt, painter, widget);
        break;
    }
}

void BaseDialogStyle::drawItemText(QPainter *painter, const QRect &rect, int flags, const QPalette &pal, bool enabled, const QString &text, QPalette::ColorRole textRole) const
{
    QProxyStyle::drawItemText (painter, rect, flags, mPalette, enabled, text, textRole);
}

void BaseDialogStyle::drawComplexControl(ComplexControl control, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget) const
{
    switch (control) {
    case CC_ComboBox: {
        if (const QStyleOptionComboBox *comboBox = qstyleoption_cast<const QStyleOptionComboBox *>(option)) {
            const bool enable = comboBox->state & State_Enabled;
            const bool on = comboBox->state & State_On;
            const bool hover = comboBox->state & State_MouseOver;

            if (!enable) {
                painter->save();
                painter->setPen(Qt::NoPen);
                painter->setBrush(mPalette.brush(mPalette.Disabled, mPalette.Button));
                painter->setRenderHint(QPainter::Antialiasing,true);
                painter->drawRoundedRect(option->rect, 4, 4);
                painter->restore();
                return;
            }

            if (comboBox->editable) {
                painter->save();
                if (comboBox->state & (State_HasFocus | State_On)) {
                    painter->setPen(QPen(mPalette.brush(mPalette.Active, mPalette.Highlight), 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
                    painter->setBrush(mPalette.brush(mPalette.Active, mPalette.Base));
                } else {
                    painter->setPen(Qt::NoPen);
                    painter->setBrush(mPalette.brush(mPalette.Active, mPalette.Button));
                }
                painter->setRenderHint(QPainter::Antialiasing,true);
                painter->drawRoundedRect(option->rect.adjusted(1, 1, -1, -1), 4, 4);
                painter->restore();
            } else {
                QStyleOptionButton button;
                button.state = option->state;
                button.rect = option->rect;
                button.palette = mPalette;
                drawPrimitive(PE_PanelButtonCommand, &button, painter, widget);
                if (on) {
                    painter->save();
                    painter->setPen(QPen(mPalette.brush(mPalette.Active, mPalette.Highlight), 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
                    painter->setBrush(Qt::NoBrush);
                    painter->setRenderHint(QPainter::Antialiasing,true);
                    painter->drawRoundedRect(option->rect.adjusted(1, 1, -1, -1), 4, 4);
                    painter->restore();
                }
            }

            if (hover) {
                QRectF rect = comboBox->rect;
                painter->save();
                painter->setPen(QPen(mPalette.brush(mPalette.Active, mPalette.Highlight), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
                painter->setBrush(Qt::NoBrush);
                painter->setRenderHint(QPainter::Antialiasing,true);
                painter->drawRoundedRect(rect.adjusted(0.5, 0.5, -0.5, -0.5), 4, 4);
                painter->restore();
            }

            return;
        }
        break;
    }
    default:
        QStyleOptionComplex opt = *option;
        opt.palette = mPalette;
        QProxyStyle::drawComplexControl (control, &opt, painter, widget);
        break;
    }
}
