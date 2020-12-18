/*
 * Copyright (C) 2019 Tianjin KYLIN Information Technology Co., Ltd.
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
#include "UnionVariable.h"
#include <QDir>
#include <QFileInfoList>
#include <QDebug>
#include <syslog.h>
#include <wait.h>

static QList<GMount *> gmountList;
QList<GMount *> *findGMountList()
{
    return &gmountList;
}

static QList<GVolume *> gtelevolumeList;
QList<GVolume *> *findTeleGVolumeList()
{
    return &gtelevolumeList;
}

static QList<GVolume *> gvolumeList;
QList<GVolume *> *findGVolumeList()
{
    return &gvolumeList;
}

static QList<GDrive *> gdriveList;
QList<GDrive *> *findGDriveList()
{
    return &gdriveList;
}

static QList<GMount *> gtelemountList;
QList<GMount *> *findTeleGMountList()
{
    return &gtelemountList;
}

QString getElidedText(QFont font, QString str, int MaxWidth)
{
    if (str.isEmpty())
    {
        return "";
    }

    QFontMetrics fontWidth(font);

    //计算字符串宽度
    //calculat the width of the string
    int width = fontWidth.width(str);

    //当字符串宽度大于最大宽度时进行转换
    //Convert when string width is greater than maximum width
    if (width >= MaxWidth)
    {
        //右部显示省略号
        //show by ellipsis in right
        str = fontWidth.elidedText(str, Qt::ElideRight, MaxWidth);
    }
    //返回处理后的字符串
    //return the string that is been handled
    return str;
}

void handleVolumeLabelForFat32Me(QString &volumeName,const QString &unixDeviceName){
    QFileInfoList diskList;
    QFileInfo diskLabel;
    QDir diskDir;
    QString partitionName,linkTarget;
    QString tmpName,finalName;
    int i;

    diskDir.setPath("/dev/disk/by-label");
    if(!diskDir.exists())               //this means: volume has no name.
        return;                         //            or there no mobile devices.

    diskList = diskDir.entryInfoList(); //all file from dir.
    /* eg: unixDeviceName == "/dev/sdb4"
     *     partitionName == "sdb4"
     */
    partitionName = unixDeviceName.mid(unixDeviceName.lastIndexOf('/')+1);

    for(i = 0; i < diskList.size(); ++i){
        diskLabel = diskList.at(i);
        linkTarget = diskLabel.symLinkTarget();
        if(linkTarget.contains(partitionName))
            break;
        linkTarget.clear();
    }

    if(!linkTarget.isEmpty())
        tmpName = diskLabel.fileName();//可能带有乱码的名字

    if(!tmpName.isEmpty()){
        if(tmpName == volumeName)      //ntfs、exfat格式或者非纯中文名的fat32设备,这个设备的名字不需要转码
            return;
        else{
            finalName = transcodeForGbkCode(tmpName.toLocal8Bit(), volumeName);
            if(!finalName.isEmpty())
                volumeName = finalName;
        }
    }
}

QString transcodeForGbkCode(QByteArray gbkName, QString &volumeName)
{
    int i;
    QByteArray dest,tmp;
    QString name;
    int len = gbkName.size();

    for(i = 0x0; i < len; ++i){
        if(92 == gbkName.at(i)){
            if(4 == tmp.size())
                dest.append(QByteArray::fromHex(tmp));
            else{
                if(tmp.size() > 4){
                    dest.append(QByteArray::fromHex(tmp.left(4)));
                    dest.append(tmp.mid(4));
                }else
                    dest.append(tmp);
            }
            tmp.clear();
            tmp.append(gbkName.at(i));
            continue;
        }else if(tmp.size() > 0){
            tmp.append(gbkName.at(i));
            continue;
        }else
            dest.append(gbkName.at(i));
    }

    if(4 == tmp.size())
        dest.append(QByteArray::fromHex(tmp));
    else{
        if(tmp.size() > 4){
            dest.append(QByteArray::fromHex(tmp.left(4)));
            dest.append(tmp.mid(4));
        }else
            dest.append(tmp);
    }

    /*
    * gio的api获取的卷名和/dev/disk/by-label先的名字不一致，有可能是卷名
    * 中含有特殊字符，导致/dev/disk/label下的卷名含有转义字符，导致二者的名字不一致
    * 而不是编码格式的不一致导致的，比如卷名：“数据光盘(2020-08-22)”，在/dev/disk/by-label
    * 写的名字:"数据光盘\x282020-08-22\x29",经过上述处理之后可以去除转义字符，在判断一次
    * 是否相等。比较完美的解决方案是找到能够判断字符串的编码格式，目前还没有找到实现方式，需要进一步完善
    */
    name = QString(dest);
    if (name == volumeName){
        return name;
    }

    name = QTextCodec::codecForName("GBK")->toUnicode(dest);
    return name;
}
