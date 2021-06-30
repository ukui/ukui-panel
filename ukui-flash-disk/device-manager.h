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

#ifndef DEVICEMANAGER_H
#define DEVICEMANAGER_H

#include <gio/gio.h>

#include <QMap>
#include <QObject>

class DeviceManager : public QObject
{
    Q_OBJECT
public:
    static const DeviceManager* getInstance();

private:
    explicit DeviceManager(QObject* parent = nullptr);
    ~DeviceManager();
    static void drive_connected_callback(GVolumeMonitor* monitor, GDrive* drive, gpointer pThis);
    static void drive_disconnected_callback(GVolumeMonitor* monitor, GDrive* drive, gpointer pThis);

Q_SIGNALS:
    void driveDisconnected(QString drive);


private:
    static DeviceManager*               gInstance;
    GVolumeMonitor*                     mGvolumeMonitor = nullptr;
    QMap<gpointer, QString>             mDevice;
};

#endif // DEVICEMANAGER_H
