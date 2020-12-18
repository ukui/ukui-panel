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
#ifndef UNIONVARIABLE_H
#define UNIONVARIABLE_H
#include <QList>
#include <gio/gio.h>
#include <QString>
#include <QFont>
#include <QFontMetrics>
#include <QTextCodec>
#include <syslog.h>
QList<GMount *> *findGMountList();
QList<GMount *> *findTeleGMountList();
QList<GVolume *> *findGVolumeList();
QList<GVolume *> *findTeleGVolumeList();
QList<GDrive *> *findGDriveList();
QString getElidedText(QFont font, QString str, int MaxWidth);
QString transcodeForGbkCode(QByteArray gbkName, QString &volumeName);
void handleVolumeLabelForFat32Me(QString &volumeName,const QString &unixDevcieName);
//int m_system(char *cmd);
#endif
