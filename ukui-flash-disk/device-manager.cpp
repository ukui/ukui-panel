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

#include "device-manager.h"

DeviceManager* DeviceManager::gInstance = new DeviceManager;

const DeviceManager* DeviceManager::getInstance()
{
    return gInstance;
}

DeviceManager::DeviceManager(QObject *parent) : QObject(parent)
{
    mGvolumeMonitor = g_volume_monitor_get();

    g_signal_connect(mGvolumeMonitor, "drive-connected", G_CALLBACK(drive_connected_callback), this);
    g_signal_connect(mGvolumeMonitor, "drive-disconnected", G_CALLBACK(drive_disconnected_callback), this);
}

DeviceManager::~DeviceManager()
{
    if (mGvolumeMonitor)        g_object_unref(mGvolumeMonitor);
}

void DeviceManager::drive_connected_callback(GVolumeMonitor *monitor, GDrive *drive, gpointer pThis)
{
    g_return_if_fail(drive);
    g_return_if_fail(pThis);

    DeviceManager*      dm = static_cast<DeviceManager*>(pThis);


    Q_UNUSED(dm);
    Q_UNUSED(monitor);
}

void DeviceManager::drive_disconnected_callback(GVolumeMonitor* monitor, GDrive* drive, gpointer pThis)
{
    g_return_if_fail(drive);
    g_return_if_fail(pThis);
#if 0
    DeviceManager*      dm = static_cast<DeviceManager*>(pThis);

    g_autofree gchar*   devName = g_drive_get_identifier(drive, G_DRIVE_IDENTIFIER_KIND_UNIX_DEVICE);
    if (nullptr != devName) {
        Q_EMIT dm->driveDisconnected(devName);
    }
#endif
    Q_UNUSED(monitor);
}
