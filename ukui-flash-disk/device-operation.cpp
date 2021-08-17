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

#include "device-operation.h"

#include <sys/stat.h>


static void formatCB (GObject* sourceObject, GAsyncResult* res, gpointer udata);
static void repairCB (GObject* sourceObject, GAsyncResult* res, gpointer udata);


DeviceOperation::DeviceOperation(GDrive *drive, QObject *parent) : QObject(parent)
{
    g_return_if_fail(drive);

    g_autofree char* devName = g_drive_get_identifier(drive, G_DRIVE_IDENTIFIER_KIND_UNIX_DEVICE);
    g_return_if_fail(devName);

    UDisksClient* client = udisks_client_new_sync(NULL, NULL);
    g_return_if_fail(client);

    UDisksObject* udiskObj = getObjectFromBlockDevice(client, devName);
    g_return_if_fail(udiskObj);

    mDiskBlock = udisks_object_get_block(udiskObj);
    mDiskManager = udisks_object_get_manager(udiskObj);
    mDiskFilesystem = udisks_object_get_filesystem(udiskObj);

    g_clear_object(&client);
    g_clear_object(&udiskObj);
}

DeviceOperation::DeviceOperation(GVolume* volume, QObject *parent) : QObject(parent)
{
    g_return_if_fail(volume);

    g_autofree char* devName = g_volume_get_identifier(volume, G_DRIVE_IDENTIFIER_KIND_UNIX_DEVICE);
    g_return_if_fail(devName);

    UDisksClient* client = udisks_client_new_sync(NULL, NULL);
    g_return_if_fail(client);

    UDisksObject* udiskObj = getObjectFromBlockDevice(client, devName);
    g_return_if_fail(udiskObj);

    mDiskBlock = udisks_object_get_block(udiskObj);
    mDiskManager = udisks_object_get_manager(udiskObj);
    mDiskFilesystem = udisks_object_get_filesystem(udiskObj);

    g_clear_object(&client);
    g_clear_object(&udiskObj);
}

DeviceOperation::~DeviceOperation()
{
    g_clear_object(&mDiskBlock);
    g_clear_object(&mDiskManager);
    g_clear_object(&mDiskFilesystem);
}

void DeviceOperation::udiskFormat(QString type, QString labelName)
{
    if (!mDiskBlock) {
        Q_EMIT formatFinished(false);
        return;
    }

    GVariantBuilder optionsBuilder;
    g_variant_builder_init(&optionsBuilder, G_VARIANT_TYPE_VARDICT);

    g_variant_builder_add (&optionsBuilder, "{sv}", "label", g_variant_new_string (labelName.toUtf8().constData()));
    g_variant_builder_add (&optionsBuilder, "{sv}", "take-ownership", g_variant_new_boolean (TRUE));
    g_variant_builder_add (&optionsBuilder, "{sv}", "update-partition-type", g_variant_new_boolean (TRUE));

//    g_cancellable_reset(&mFormatCancel);

    udisks_block_call_format(mDiskBlock, type.toLower().toUtf8().constData(), g_variant_builder_end(&optionsBuilder), nullptr /*&mFormatCancel*/, GAsyncReadyCallback(formatCB), this);
}

void DeviceOperation::udiskRepair()
{
    if (!mDiskFilesystem) {
        Q_EMIT repairFinished(false);
        return;
    }

    GVariantBuilder optionsBuilder;
    g_variant_builder_init(&optionsBuilder, G_VARIANT_TYPE_VARDICT);

//    g_cancellable_reset(&mRepairCancel);

    udisks_filesystem_call_repair(mDiskFilesystem, g_variant_builder_end(&optionsBuilder), nullptr /*&mRepairCancel*/, GAsyncReadyCallback(repairCB), this);
}

void DeviceOperation::udiskFormatCancel()
{
//    if (g_cancellable_is_cancelled(&mFormatCancel)) {
//        g_cancellable_cancel(&mFormatCancel);
//    }
}

void DeviceOperation::udiskRepairCancel()
{
//    if (g_cancellable_is_cancelled(&mRepairCancel)) {
//        g_cancellable_cancel(&mRepairCancel);
//    }
}

QString DeviceOperation::udiskSize()
{
    g_return_val_if_fail(mDiskBlock, tr("unknown"));

    guint64 size = udisks_block_get_size(mDiskBlock);
    g_autofree char* str = g_format_size_full(size, G_FORMAT_SIZE_IEC_UNITS);

    return str;
}

QString DeviceOperation::udiskUUID()
{
    g_return_val_if_fail(mDiskBlock, tr("unknown"));

    const char* str = udisks_block_get_id_uuid(mDiskBlock);

    return str;
}

QString DeviceOperation::udiskLabel()
{
    g_return_val_if_fail(mDiskBlock, tr("unknown"));

    const char* str = udisks_block_get_id_label(mDiskBlock);

    return str;
}

UDisksObject* DeviceOperation::getObjectFromBlockDevice(UDisksClient* client, const gchar* bdevice)
{
    struct stat statbuf;
    UDisksBlock* block = NULL;
    UDisksObject* object = NULL;
    UDisksObject* cryptoBackingObject = NULL;
    g_autofree const gchar* cryptoBackingDevice = NULL;

    g_return_val_if_fail(stat(bdevice, &statbuf) == 0, object);

    block = udisks_client_get_block_for_dev (client, statbuf.st_rdev);
    g_return_val_if_fail(block != NULL, object);

    object = UDISKS_OBJECT (g_dbus_interface_dup_object (G_DBUS_INTERFACE (block)));

    cryptoBackingDevice = udisks_block_get_crypto_backing_device ((udisks_object_peek_block (object)));
    cryptoBackingObject = udisks_client_get_object (client, cryptoBackingDevice);
    if (cryptoBackingObject != NULL) {
        g_object_unref (object);
        object = cryptoBackingObject;
    }

    g_object_unref (block);

    return object;
}

static void formatCB (GObject* sourceObject, GAsyncResult* res, gpointer udata)
{
    bool ret = true;
    GError* error = NULL;
    DeviceOperation* pThis = static_cast<DeviceOperation*>(udata);

    if (!udisks_block_call_format_finish (UDISKS_BLOCK(sourceObject), res, &error)) {
        if (NULL != error && NULL != strstr(error->message, "wipefs:")) {
            g_clear_error(&error);
        }
        ret = false;
    }

    Q_EMIT pThis->formatFinished(ret);
}
static void repairCB (GObject* sourceObject, GAsyncResult* res, gpointer udata)
{
    GError* error = NULL;
    gboolean outRet = FALSE;
    DeviceOperation* pThis = static_cast<DeviceOperation*>(udata);

    if (!udisks_filesystem_call_repair_finish(UDISKS_FILESYSTEM(sourceObject), &outRet, res, &error)) {
        g_clear_error(&error);
    }

    Q_EMIT pThis->repairFinished(outRet);
}
