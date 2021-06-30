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

#ifndef DEVICEOPERATION_H
#define DEVICEOPERATION_H

#include <QObject>
#include <gio/gio.h>
#include <udisks/udisks.h>

class DeviceOperation : public QObject
{
    Q_OBJECT
public:
    explicit DeviceOperation(GDrive* drive, QObject *parent = nullptr);
    explicit DeviceOperation(GVolume* volume, QObject *parent = nullptr);
    ~DeviceOperation();

public Q_SLOTS:
    void udiskFormat(QString type, QString labelName);
    void udiskRepair();
    void udiskFormatCancel();
    void udiskRepairCancel();

    QString udiskSize();
    QString udiskUUID();
    QString udiskLabel();

private:
    UDisksObject* getObjectFromBlockDevice(UDisksClient *client, const gchar *bdevice);

Q_SIGNALS:
    void repairFinished(bool);
    void formatFinished(bool);

private:
    UDisksBlock*            mDiskBlock = NULL;
    UDisksManager*          mDiskManager = NULL;
    UDisksFilesystem*       mDiskFilesystem = NULL;

    GCancellable            mRepairCancel;
    GCancellable            mFormatCancel;
};

#endif // DEVICEOPERATION_H
