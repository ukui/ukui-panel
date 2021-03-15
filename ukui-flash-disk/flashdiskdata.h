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

#ifndef __FLASHDISKDATA_H__
#define __FLASHDISKDATA_H__

#include <QList>
#include <gio/gio.h>
#include <QString>
#include <QFont>
#include <QFontMetrics>
#include <QTextCodec>
#include <QMutex>
#include <map>
#include <string>

using namespace std;

typedef struct FDMountInfo_s {
    string strId = "";
    string strName = "";
    bool   isCanUnmount = false;
    bool   isCanEject = false;
    string strTooltip = "";
    string strUri = "";
    bool   isNativeDev = false;
    bool   isNewInsert = false;
    quint64 lluTotalSize = 0;
}FDMountInfo;

typedef struct FDVolumeInfo_s {
    string strId = "";
    string strName = "";
    string strDevName = "";
    bool   isCanMount = false;
    bool   isShouldAutoMount = false;
    bool   isCanEject = false;
    bool   isNewInsert = false;
    FDMountInfo mountInfo;
}FDVolumeInfo;

typedef struct FDDriveInfo_s {
    string strId = "";
    string strName = "";
    bool    isCanEject = false;
    bool    isRemovable = false;
    bool    isCanStart = false;
    bool    isCanStop = false;
    map<string, FDVolumeInfo> listVolumes;
}FDDriveInfo;

class FlashDiskData : public QObject
{
    Q_OBJECT
public:
    virtual ~FlashDiskData();
    static FlashDiskData* getInstance();
    map<string, FDDriveInfo>& getDevInfoWithDrive();
    map<string, FDVolumeInfo>& getDevInfoWithVolume();
    map<string, FDMountInfo>& getDevInfoWithMount();
    int addDriveInfo(FDDriveInfo driveInfo);
    int addVolumeInfoWithDrive(FDDriveInfo driveInfo, FDVolumeInfo volumeInfo);
    int addMountInfoWithDrive(FDDriveInfo driveInfo, FDVolumeInfo volumeInfo, FDMountInfo mountInfo);
    int addVolumeInfo(FDVolumeInfo volumeInfo);
    int addMountInfo(FDMountInfo mountInfo);
    int removeDriveInfo(FDDriveInfo driveInfo);
    int removeVolumeInfo(FDVolumeInfo volumeInfo);
    int removeMountInfo(FDMountInfo mountInfo);
    unsigned getValidInfoCount();
    void clearAllData();
    bool isMountInfoExist(FDMountInfo mountInfo);

    void OutputInfos();

private:
    FlashDiskData();

    static FlashDiskData *m_instance;
    static QMutex   m_mutex;

    map<string, FDDriveInfo> m_devInfoWithDrive; //
    map<string, FDVolumeInfo> m_devInfoWithVolume; // except with drive
    map<string, FDMountInfo> m_devInfoWithMount; // except with volume
};

#endif // __FLASHDISKDATA_H__