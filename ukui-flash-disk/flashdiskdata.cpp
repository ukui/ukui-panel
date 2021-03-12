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

    m_devInfoWithDrive[driveInfo.strId] = driveInfo;
    return 0;
}

int FlashDiskData::addVolumeInfoWithDrive(FDDriveInfo driveInfo, FDVolumeInfo volumeInfo)
{
    if (driveInfo.strId.empty() || volumeInfo.strId.empty()) {
        return -1;
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
    map<string, FDDriveInfo>::iterator itDriveInfo = m_devInfoWithDrive.begin();
    for (; itDriveInfo != m_devInfoWithDrive.end(); itDriveInfo++) {
        if (itDriveInfo->second.listVolumes.find(volumeInfo.strId) != itDriveInfo->second.listVolumes.end()) {
            return 1;
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
        m_devInfoWithVolume.erase(volumeInfo.strId);
        return 0;
    } else {
        map<string, FDDriveInfo>::iterator itDriveInfo = m_devInfoWithDrive.begin();
        for (; itDriveInfo != m_devInfoWithDrive.end(); itDriveInfo++) {
            map<string, FDVolumeInfo>::iterator itVolumeInfo = itDriveInfo->second.listVolumes.find(volumeInfo.strId);
            if (itVolumeInfo != itDriveInfo->second.listVolumes.end()) {
                itVolumeInfo->second.strId = "";
                if (itVolumeInfo->second.mountInfo.strId.empty()) {
                    itDriveInfo->second.listVolumes.erase(volumeInfo.strId);
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
                m_devInfoWithVolume.erase(itVolumeInfo);
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
                    itDriveInfo->second.listVolumes.erase(itVolumeInfo);
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

unsigned FlashDiskData::getValidInfoCount()
{
    unsigned uDriveCount = 0;
    unsigned uVolumeCount = 0;
    unsigned uMountCount = 0;
    uMountCount = m_devInfoWithMount.size();
    uVolumeCount = m_devInfoWithVolume.size();
    map<string, FDDriveInfo>::iterator itDriveInfo = m_devInfoWithDrive.begin();
    for (; itDriveInfo != m_devInfoWithDrive.end(); itDriveInfo++) {
        uDriveCount += itDriveInfo->second.listVolumes.size();
    }
    return uDriveCount + uVolumeCount + uMountCount;
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
                <<"|"<<itMountInfo->second.lluTotalSize;
        }
    }
    qDebug()<<"--------------------------------------------------------";
    map<string, FDVolumeInfo>::iterator itVolumeInfo = m_devInfoWithVolume.begin();
    for (; itVolumeInfo != m_devInfoWithVolume.end(); itVolumeInfo++) {
        qDebug()<<"Volume:"<<QString::fromStdString(itVolumeInfo->second.strId)<<"|"
            <<QString::fromStdString(itVolumeInfo->second.strName)<<"|"<<itVolumeInfo->second.isCanMount
            <<"|"<<itVolumeInfo->second.isShouldAutoMount<<"|"<<itVolumeInfo->second.isCanEject
            <<"|"<<QString::fromStdString(itVolumeInfo->second.strDevName);
        if (!itVolumeInfo->second.mountInfo.strId.empty()) {
            qDebug()<<"Mount:"<<QString::fromStdString(itVolumeInfo->second.mountInfo.strId)<<"|"
                <<QString::fromStdString(itVolumeInfo->second.mountInfo.strName)<<"|"<<itVolumeInfo->second.mountInfo.isCanUnmount
                <<"|"<<itVolumeInfo->second.mountInfo.isCanEject<<"|"<<QString::fromStdString(itVolumeInfo->second.mountInfo.strTooltip)<<"|"
                <<QString::fromStdString(itVolumeInfo->second.mountInfo.strUri)<<"|"<<itVolumeInfo->second.mountInfo.isNativeDev
                <<"|"<<itVolumeInfo->second.mountInfo.lluTotalSize;
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
                <<"|"<<QString::fromStdString(itVolumeInfo->second.strDevName);
            if (!itVolumeInfo->second.mountInfo.strId.empty()) {
                qDebug()<<"Mount:"<<QString::fromStdString(itVolumeInfo->second.mountInfo.strId)<<"|"
                    <<QString::fromStdString(itVolumeInfo->second.mountInfo.strName)<<"|"<<itVolumeInfo->second.mountInfo.isCanUnmount
                    <<"|"<<itVolumeInfo->second.mountInfo.isCanEject<<"|"<<QString::fromStdString(itVolumeInfo->second.mountInfo.strTooltip)<<"|"
                    <<QString::fromStdString(itVolumeInfo->second.mountInfo.strUri)<<"|"<<itVolumeInfo->second.mountInfo.isNativeDev
                    <<"|"<<itVolumeInfo->second.mountInfo.lluTotalSize;
            }
        }
    }
    qDebug()<<"+++++++++++++++++++++++++++++++++++++++++++++++++++++++++";
    #endif
}