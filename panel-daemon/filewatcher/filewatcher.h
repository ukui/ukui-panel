/*
 * Copyright (C) 2019 Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU  Lesser General Public License as published by
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

#ifndef FILEWATCHER_H
#define FILEWATCHER_H

#include <QObject>
#include <QFileSystemWatcher>
#include <QWidget>
#include <QStringList>
#include <QMap>


class FileWatcher : public QObject
{
    Q_OBJECT
public:
    FileWatcher();

    QFileSystemWatcher *fsWatcher;
    void initDirMonitor(QString path);
    QMap<QString, QStringList> m_currentContentsMap; // 当前每个监控的内容目录列表
    void directoryUpdated(const QString &path);

signals:
    void DesktopDeleteFile(const QString &path, QStringList deleteFile);
};

#endif // FILEWACTHER_H
