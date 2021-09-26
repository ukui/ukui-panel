/*
 * Copyright (C) 2021 KylinSoft Co., Ltd.
 * 
 * Authors:
 *  Yang Min yangmin@kylinos.cn
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

#include <QDebug>
#include <iostream>
#include <QDateTime>

using namespace std;

#include "flashdiskdata.h"

FlashDiskData* FlashDiskData::m_instance = nullptr;
QMutex FlashDiskData::m_mutex;

FlashDiskData::FlashDiskData()
{
    m_devInfoWithDrive.clear();
    m_devInfoWithVolume.clear();
    m_devInfoWithMount.clear();
}

FlashDiskData::~FlashDiskData()
{

}

FlashDiskData *FlashDiskData::getInstance()
{
    if (m_instance == nullptr)
    {
        QMutexLocker locker(&m_mutex);
        if (m_instance == nullptr)
        {
            m_instance = new FlashDiskData();
        }
    }
    return m_instance;
}

map<string, FDDriveInfo>& FlashDiskData::getDevInfoWithDrive()
{
    return m_devInfoWithDrive;
}

map<string, FDVolumeInfo>& FlashDiskData::getDevInfoWithVolume()
{
    return m_devInfoWithVolume;
}

map<string, FDMountInfo>& FlashDiskData::getDevInfoWithMount()
{
    return m_devInfoWithMount;
}

int FlashDiskData::addDriveInfo(FDDriveInfo driveInfo)
{
    if (driveInfo.strId.empty()) {
        return -1;
    }
    map<string, FDVolumeInfo>::iterator itVolumeInfoNew = driveInfo.listVolumes.begin();
    for (; itVolumeInfoNew != driveInfo.listVolumes.end(); itVolumeInfoNew++) {
        if (!itVolumeInfoNew->second.strId.empty()) {
            map<string, FDVolumeInfo>::iterator itVolumeInfo = m_devInfoWithVolume.find(itVolumeInfoNew->second.strId);
            if (itVolumeInfo != m_devInfoWithVolume.end()) {
                m_devInfoWithVolume.erase(itVolumeInfo);
            }
        }
    }
    m_devInfoWithDrive[driveInfo.strId] = driveInfo;
    return 0;
}

int FlashDiskData::addVolumeInfoWithDrive(FDDriveInfo driveInfo, FDVolumeInfo volumeInfo)
{
    if (driveInfo.strId.empty() || volumeInfo.strId.empty()) {
        return -1;
    }
    volumeInfo.mountInfo.lluMountTick = QDateTime::currentDateTime().toMSecsSinceEpoch();
    map<string, FDVolumeInfo>::iterator itVolumeInfo = m_devInfoWithVolume.find(volumeInfo.strId);
    if (itVolumeInfo != m_devInfoWithVolume.end()) {
        m_devInfoWithVolume.erase(itVolumeInfo);
    }
    map<string, FDDriveInfo>::iterator itDriveInfo = m_devInfoWithDrive.find(driveInfo.strId);
    if (itDriveInfo != m_devInfoWithDrive.end()) {
        itDriveInfo->second.listVolumes[volumeInfo.strId] = volumeInfo;
    } else {
        driveInfo.listVolumes.clear();
        driveInfo.listVolumes[volumeInfo.strId] = volumeInfo;
        m_devInfoWithDrive[driveInfo.strId] = driveInfo;
    }
    return 0;
}

int FlashDiskData::addMountInfoWithDrive(FDDriveInfo driveInfo, FDVolumeInfo volumeInfo, FDMountInfo mountInfo)
{
    if (driveInfo.strId.empty() || mountInfo.strId.empty()) {
        return -1;
    }
    mountInfo.lluMountTick = QDateTime::currentDateTime().toMSecsSinceEpoch();
    map<string, FDDriveInfo>::iterator itDriveInfo = m_devInfoWithDrive.find(driveInfo.strId);
    if (itDriveInfo != m_devInfoWithDrive.end()) {
        volumeInfo.mountInfo = mountInfo;
        if (volumeInfo.strId.empty()) {
            itDriveInfo->second.listVolumes[mountInfo.strId] = volumeInfo;
        } else {
            itDriveInfo->second.listVolumes[volumeInfo.strId] = volumeInfo;
        }
    } else {
        driveInfo.listVolumes.clear();
        volumeInfo.mountInfo = mountInfo;
        if (volumeInfo.strId.empty()) {
            driveInfo.listVolumes[mountInfo.strId] = volumeInfo;
        } else {
            driveInfo.listVolumes[volumeInfo.strId] = volumeInfo;
        }
        m_devInfoWithDrive[driveInfo.strId] = driveInfo;
    }
    return 0;
}

int FlashDiskData::addVolumeInfo(FDVolumeInfo volumeInfo)
{
    if (volumeInfo.strId.empty()) {
        return -1;
    }
    volumeInfo.mountInfo.lluMountTick = QDateTime::currentDateTime().toMSecsSinceEpoch();
    map<string, FDDriveInfo>::iterator itDriveInfo = m_devInfoWithDrive.begin();
    for (; itDriveInfo != m_devInfoWithDrive.end(); itDriveInfo++) {
        if (itDriveInfo->second.listVolumes.find(volumeInfo.strId) != itDriveInfo->second.listVolumes.end()) {
            itDriveInfo->second.listVolumes[volumeInfo.strId] = volumeInfo;
            return 1;
        }
    }
    if (!volumeInfo.mountInfo.strId.empty()) {
        map<string, FDMountInfo>::iterator itMountInfo = m_devInfoWithMount.find(volumeInfo.mountInfo.strId);
        if (itMountInfo != m_devInfoWithMount.end()) {
            m_devInfoWithMount.erase(itMountInfo);
        }
    }
    m_devInfoWithVolume[volumeInfo.strId] = volumeInfo;
    return 0;
}

int FlashDiskData::addMountInfo(FDMountInfo mountInfo)
{
    if (mountInfo.strId.empty()) {
        return -1;
    }
    mountInfo.lluMountTick = QDateTime::currentDateTime().toMSecsSinceEpoch();
    map<string, FDVolumeInfo>::iterator itVolumeInfo = m_devInfoWithVolume.begin();
    for (; itVolumeInfo != m_devInfoWithVolume.end(); itVolumeInfo++) {
        if (itVolumeInfo->second.mountInfo.strId == mountInfo.strId) {
            return 1;
        }
    }
    map<string, FDDriveInfo>::iterator itDriveInfo = m_devInfoWithDrive.begin();
    for (; itDriveInfo != m_devInfoWithDrive.end(); itDriveInfo++) {
        itVolumeInfo = itDriveInfo->second.listVolumes.begin();
        for (; itVolumeInfo != itDriveInfo->second.listVolumes.end(); itVolumeInfo++) {
            if (itVolumeInfo->second.mountInfo.strId == mountInfo.strId) {
                return 1;
            }
        }
    }
    m_devInfoWithMount[mountInfo.strId] = mountInfo;
    return 0;
}

int FlashDiskData::removeDriveInfo(FDDriveInfo driveInfo)
{
    if (driveInfo.strId.empty()) {
        return -1;
    }
    if (m_devInfoWithDrive.find(driveInfo.strId) != m_devInfoWithDrive.end()) {
        if (getAttachedVolumeCount(driveInfo.strId) > 0) {
            Q_EMIT notifyDeviceRemoved(QString::fromStdString(driveInfo.strId));
        }
        m_devInfoWithDrive.erase(driveInfo.strId);
        return 0;
    }
    return 1;
}

int FlashDiskData::removeVolumeInfo(FDVolumeInfo volumeInfo)
{
    if (volumeInfo.strId.empty()) {
        return -1;
    }
    if (m_devInfoWithVolume.find(volumeInfo.strId) != m_devInfoWithVolume.end()) {
        if (getAttachedVolumeCount(volumeInfo.strId) == 1) {
            Q_EMIT notifyDeviceRemoved(QString::fromStdString(volumeInfo.strId));
        }
        m_devInfoWithVolume.erase(volumeInfo.strId);
        return 0;
    } else {
        map<string, FDDriveInfo>::iterator itDriveInfo = m_devInfoWithDrive.begin();
        for (; itDriveInfo != m_devInfoWithDrive.end(); itDriveInfo++) {
            map<string, FDVolumeInfo>::iterator itVolumeInfo = itDriveInfo->second.listVolumes.find(volumeInfo.strId);
            if (itVolumeInfo != itDriveInfo->second.listVolumes.end()) {
                if (getAttachedVolumeCount(volumeInfo.strId) == 1) {
                    Q_EMIT notifyDeviceRemoved(QString::fromStdString(volumeInfo.strId));
                }
                if (itVolumeInfo->second.mountInfo.strId.empty()) {
                    itDriveInfo->second.listVolumes.erase(volumeInfo.strId);
                } else {
                    itVolumeInfo->second.strId = "";
                }
                return 0;
            }
        }
    }
    return 1;
}

int FlashDiskData::removeMountInfo(FDMountInfo mountInfo)
{
    if (mountInfo.strId.empty()) {
        return -1;
    }
    if (m_devInfoWithMount.find(mountInfo.strId) != m_devInfoWithMount.end()) {
        m_devInfoWithMount.erase(mountInfo.strId);
        return 0;
    } else {
        map<string, FDVolumeInfo>::iterator itVolumeInfo = m_devInfoWithVolume.begin();
        for (; itVolumeInfo != m_devInfoWithVolume.end(); itVolumeInfo++) {
            if (itVolumeInfo->second.mountInfo.strId == mountInfo.strId) {
                #if 0
                FDMountInfo mountTemp;
                itVolumeInfo->second.mountInfo = mountTemp;
                if (itVolumeInfo->second.strId.empty()) {
                    m_devInfoWithVolume.erase(itVolumeInfo);
                }
                #else 
                // if (getAttachedVolumeCount(itVolumeInfo->second.strId) == 1) {
                //     Q_EMIT notifyDeviceRemoved(QString::fromStdString(itVolumeInfo->second.strId));
                // }
                // m_devInfoWithVolume.erase(itVolumeInfo);
                itVolumeInfo->second.mountInfo.strId = "";
                #endif
                return 0;
            }
        }
        map<string, FDDriveInfo>::iterator itDriveInfo = m_devInfoWithDrive.begin();
        for (; itDriveInfo != m_devInfoWithDrive.end(); itDriveInfo++) {
            itVolumeInfo = itDriveInfo->second.listVolumes.begin();
            for (; itVolumeInfo != itDriveInfo->second.listVolumes.end(); itVolumeInfo++) {
                if (itVolumeInfo->second.mountInfo.strId == mountInfo.strId) {
                    #if 0
                    FDMountInfo mountTemp;
                    itVolumeInfo->second.mountInfo = mountTemp;
                    if (itVolumeInfo->second.strId.empty()) {
                        itDriveInfo->second.listVolumes.erase(itVolumeInfo);
                    }
                    #else
                    // if (getAttachedVolumeCount(itVolumeInfo->second.strId) == 1) {
                    //     Q_EMIT notifyDeviceRemoved(QString::fromStdString(itVolumeInfo->second.strId));
                    // }
                    // itDriveInfo->second.listVolumes.erase(itVolumeInfo);
                    itVolumeInfo->second.mountInfo.strId = "";
                    #endif
                    return 0;
                }
            }
        }
    }
    return 1;
}

bool FlashDiskData::isMountInfoExist(FDMountInfo mountInfo)
{
    if (mountInfo.strId.empty()) {
        return false;
    }
    if (m_devInfoWithMount.find(mountInfo.strId) != m_devInfoWithMount.end()) {
        return true;
    }
    map<string, FDDriveInfo>::iterator itDriveInfo;
    map<string, FDVolumeInfo>::iterator itVolumeInfo;
    itVolumeInfo = m_devInfoWithVolume.begin();
    for (; itVolumeInfo != m_devInfoWithVolume.end(); itVolumeInfo++) {
        if (itVolumeInfo->second.mountInfo.strId == mountInfo.strId) {
            return true;
        }
    }
    itDriveInfo = m_devInfoWithDrive.begin();
    for (; itDriveInfo != m_devInfoWithDrive.end(); itDriveInfo++) {
        itVolumeInfo = itDriveInfo->second.listVolumes.begin();
        for (; itVolumeInfo != itDriveInfo->second.listVolumes.end(); itVolumeInfo++) {
            if (itVolumeInfo->second.mountInfo.strId == mountInfo.strId) {
                return true;
            }
        }
    }
    return false;
}

quint64 FlashDiskData::getMountTickDiff(FDMountInfo mountInfo)
{
    quint64 curTick = QDateTime::currentDateTime().toMSecsSinceEpoch();
    if (mountInfo.strId.empty()) {
        return 0;
    }
    if (m_devInfoWithMount.find(mountInfo.strId) != m_devInfoWithMount.end()) {
        return (curTick-m_devInfoWithMount[mountInfo.strId].lluMountTick);
    }
    map<string, FDDriveInfo>::iterator itDriveInfo;
    map<string, FDVolumeInfo>::iterator itVolumeInfo;
    itVolumeInfo = m_devInfoWithVolume.begin();
    for (; itVolumeInfo != m_devInfoWithVolume.end(); itVolumeInfo++) {
        if (itVolumeInfo->second.mountInfo.strId == mountInfo.strId) {
            return (curTick-itVolumeInfo->second.mountInfo.lluMountTick);
        }
    }
    itDriveInfo = m_devInfoWithDrive.begin();
    for (; itDriveInfo != m_devInfoWithDrive.end(); itDriveInfo++) {
        itVolumeInfo = itDriveInfo->second.listVolumes.begin();
        for (; itVolumeInfo != itDriveInfo->second.listVolumes.end(); itVolumeInfo++) {
            if (itVolumeInfo->second.mountInfo.strId == mountInfo.strId) {
                return (curTick-itVolumeInfo->second.mountInfo.lluMountTick);
            }
        }
    }
    return 0;
}

bool FlashDiskData::getVolumeInfoByMount(FDMountInfo mountInfo, FDVolumeInfo& volumeInfo)
{
    quint64 curTick = QDateTime::currentDateTime().toMSecsSinceEpoch();
    if (mountInfo.strId.empty()) {
        return false;
    }
    if (m_devInfoWithMount.find(mountInfo.strId) != m_devInfoWithMount.end()) {
        return false;
    }
    map<string, FDDriveInfo>::iterator itDriveInfo;
    map<string, FDVolumeInfo>::iterator itVolumeInfo;
    itVolumeInfo = m_devInfoWithVolume.begin();
    for (; itVolumeInfo != m_devInfoWithVolume.end(); itVolumeInfo++) {
        if (itVolumeInfo->second.mountInfo.strId == mountInfo.strId) {
            volumeInfo = itVolumeInfo->second;
            return true;
        }
    }
    itDriveInfo = m_devInfoWithDrive.begin();
    for (; itDriveInfo != m_devInfoWithDrive.end(); itDriveInfo++) {
        itVolumeInfo = itDriveInfo->second.listVolumes.begin();
        for (; itVolumeInfo != itDriveInfo->second.listVolumes.end(); itVolumeInfo++) {
            if (itVolumeInfo->second.mountInfo.strId == mountInfo.strId) {
                volumeInfo = itVolumeInfo->second;
                return true;
            }
        }
    }
    return false;
}

void FlashDiskData::resetAllNewState()
{
    map<string, FDMountInfo>::iterator itMountInfo = m_devInfoWithMount.begin();
    for (; itMountInfo != m_devInfoWithMount.end(); itMountInfo++) {
        if (!itMountInfo->second.strId.empty()) {
            itMountInfo->second.isNewInsert = false;
        }
    }
    map<string, FDVolumeInfo>::iterator itVolumeInfo = m_devInfoWithVolume.begin();
    for (; itVolumeInfo != m_devInfoWithVolume.end(); itVolumeInfo++) {
        itVolumeInfo->second.isNewInsert = false;
        if (!itVolumeInfo->second.mountInfo.strId.empty()) {
            itVolumeInfo->second.mountInfo.isNewInsert = false;
        }
    }
    map<string, FDDriveInfo>::iterator itDriveInfo = m_devInfoWithDrive.begin();
    for (; itDriveInfo != m_devInfoWithDrive.end(); itDriveInfo++) {
        itVolumeInfo = itDriveInfo->second.listVolumes.begin();
        for (; itVolumeInfo != itDriveInfo->second.listVolumes.end(); itVolumeInfo++) {
            itVolumeInfo->second.isNewInsert = false;
            if (!itVolumeInfo->second.mountInfo.strId.empty()) {
                itVolumeInfo->second.mountInfo.isNewInsert = false;
            }
        }
    }
}

unsigned FlashDiskData::getValidInfoCount()
{
    unsigned uDriveCount = 0;
    unsigned uVolumeCount = 0;
    unsigned uMountCount = 0;
    uMountCount = m_devInfoWithMount.size();
    map<string, FDVolumeInfo>::iterator itVolumeInfo = m_devInfoWithVolume.begin();
    for (; itVolumeInfo != m_devInfoWithVolume.end(); itVolumeInfo++) {
        if (!itVolumeInfo->second.mountInfo.strId.empty()) {
            uVolumeCount ++;
        }
    }
    map<string, FDDriveInfo>::iterator itDriveInfo = m_devInfoWithDrive.begin();
    for (; itDriveInfo != m_devInfoWithDrive.end(); itDriveInfo++) {
        itVolumeInfo = itDriveInfo->second.listVolumes.begin();
        for (; itVolumeInfo != itDriveInfo->second.listVolumes.end(); itVolumeInfo++) {
            if (!itVolumeInfo->second.mountInfo.strId.empty()) {
                uDriveCount ++;
            }
        }
    }
    return uDriveCount + uVolumeCount + uMountCount;
}

unsigned FlashDiskData::getAttachedVolumeCount(string strId)
{
    if (strId.empty()) {
        return 0;
    }
    map<string, FDVolumeInfo>::iterator itVolumeInfo = m_devInfoWithVolume.begin();
    for (; itVolumeInfo != m_devInfoWithVolume.end(); itVolumeInfo++) {
        if (itVolumeInfo->second.strId == strId) {
            return 1;
        }
    }
    map<string, FDDriveInfo>::iterator itDriveInfo = m_devInfoWithDrive.begin();
    for (; itDriveInfo != m_devInfoWithDrive.end(); itDriveInfo++) {
        if (itDriveInfo->second.strId == strId) {
            return itDriveInfo->second.listVolumes.size();
        } else {
            itVolumeInfo = itDriveInfo->second.listVolumes.begin();
            for (; itVolumeInfo != itDriveInfo->second.listVolumes.end(); itVolumeInfo++) {
                if (itVolumeInfo->second.strId == strId) {
                    return itDriveInfo->second.listVolumes.size();
                }
            }
        }
    }
    return 0;
}

void FlashDiskData::clearAllData()
{
    
}

void FlashDiskData::OutputInfos()
{
    #if 0
    unsigned uDriveCount = 0;
    unsigned uVolumeCount = 0;
    unsigned uMountCount = 0;
    uMountCount = m_devInfoWithMount.size();
    uVolumeCount = m_devInfoWithVolume.size();
    map<string, FDDriveInfo>::iterator itDriveInfo = m_devInfoWithDrive.begin();
    for (; itDriveInfo != m_devInfoWithDrive.end(); itDriveInfo++) {
        uDriveCount += itDriveInfo->second.listVolumes.size();
    }
    qDebug()<<"info count:"<<uDriveCount<<"|"<<uVolumeCount<<"|"<<uMountCount;
    qDebug()<<"+++++++++++++++++++++++++++++++++++++++++++++++++++++++++";
    map<string, FDMountInfo>::iterator itMountInfo = m_devInfoWithMount.begin();
    for (; itMountInfo != m_devInfoWithMount.end(); itMountInfo++) {
        if (!itMountInfo->second.strId.empty()) {
            qDebug()<<"Mount:"<<QString::fromStdString(itMountInfo->second.strId)<<"|"
                <<QString::fromStdString(itMountInfo->second.strName)<<"|"<<itMountInfo->second.isCanUnmount
                <<"|"<<itMountInfo->second.isCanEject<<"|"<<QString::fromStdString(itMountInfo->second.strTooltip)<<"|"
                <<QString::fromStdString(itMountInfo->second.strUri)<<"|"<<itMountInfo->second.isNativeDev
                <<"|"<<itMountInfo->second.lluTotalSize<<"|"<<itMountInfo->second.isNewInsert<<"|"<<itMountInfo->second.lluMountTick;
        }
    }
    qDebug()<<"--------------------------------------------------------";
    map<string, FDVolumeInfo>::iterator itVolumeInfo = m_devInfoWithVolume.begin();
    for (; itVolumeInfo != m_devInfoWithVolume.end(); itVolumeInfo++) {
        qDebug()<<"Volume:"<<QString::fromStdString(itVolumeInfo->second.strId)<<"|"
            <<QString::fromStdString(itVolumeInfo->second.strName)<<"|"<<itVolumeInfo->second.isCanMount
            <<"|"<<itVolumeInfo->second.isShouldAutoMount<<"|"<<itVolumeInfo->second.isCanEject
            <<"|"<<QString::fromStdString(itVolumeInfo->second.strDevName)<<"|"<<itVolumeInfo->second.isNewInsert;
        if (!itVolumeInfo->second.mountInfo.strId.empty()) {
            qDebug()<<"Mount:"<<QString::fromStdString(itVolumeInfo->second.mountInfo.strId)<<"|"
                <<QString::fromStdString(itVolumeInfo->second.mountInfo.strName)<<"|"<<itVolumeInfo->second.mountInfo.isCanUnmount
                <<"|"<<itVolumeInfo->second.mountInfo.isCanEject<<"|"<<QString::fromStdString(itVolumeInfo->second.mountInfo.strTooltip)<<"|"
                <<QString::fromStdString(itVolumeInfo->second.mountInfo.strUri)<<"|"<<itVolumeInfo->second.mountInfo.isNativeDev
                <<"|"<<itVolumeInfo->second.mountInfo.lluTotalSize<<"|"<<itVolumeInfo->second.mountInfo.isNewInsert<<"|"
                <<itVolumeInfo->second.mountInfo.lluMountTick;
        }
    }
    qDebug()<<"--------------------------------------------------------";
    itDriveInfo = m_devInfoWithDrive.begin();
    for (; itDriveInfo != m_devInfoWithDrive.end(); itDriveInfo++) {
        qDebug()<<"Drive:"<<QString::fromStdString(itDriveInfo->second.strId)<<"|"
            <<QString::fromStdString(itDriveInfo->second.strName)<<"|"<<itDriveInfo->second.isCanEject
            <<"|"<<itDriveInfo->second.isRemovable<<"|"<<itDriveInfo->second.isCanStart<<"|"<<itDriveInfo->second.isCanStop;
        itVolumeInfo = itDriveInfo->second.listVolumes.begin();
        for (; itVolumeInfo != itDriveInfo->second.listVolumes.end(); itVolumeInfo++) {
            qDebug()<<"Volume:"<<QString::fromStdString(itVolumeInfo->second.strId)<<"|"
                <<QString::fromStdString(itVolumeInfo->second.strName)<<"|"<<itVolumeInfo->second.isCanMount
                <<"|"<<itVolumeInfo->second.isShouldAutoMount<<"|"<<itVolumeInfo->second.isCanEject
                <<"|"<<QString::fromStdString(itVolumeInfo->second.strDevName)<<"|"<<itVolumeInfo->second.isNewInsert;
            if (!itVolumeInfo->second.mountInfo.strId.empty()) {
                qDebug()<<"Mount:"<<QString::fromStdString(itVolumeInfo->second.mountInfo.strId)<<"|"
                    <<QString::fromStdString(itVolumeInfo->second.mountInfo.strName)<<"|"<<itVolumeInfo->second.mountInfo.isCanUnmount
                    <<"|"<<itVolumeInfo->second.mountInfo.isCanEject<<"|"<<QString::fromStdString(itVolumeInfo->second.mountInfo.strTooltip)<<"|"
                    <<QString::fromStdString(itVolumeInfo->second.mountInfo.strUri)<<"|"<<itVolumeInfo->second.mountInfo.isNativeDev
                    <<"|"<<itVolumeInfo->second.mountInfo.lluTotalSize<<"|"<<itVolumeInfo->second.mountInfo.isNewInsert<<"|"
                    <<itVolumeInfo->second.mountInfo.lluMountTick;
            }
        }
    }
    qDebug()<<"+++++++++++++++++++++++++++++++++++++++++++++++++++++++++";
    #endif
}
