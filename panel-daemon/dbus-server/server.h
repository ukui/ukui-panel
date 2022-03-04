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
#ifndef _SERVER_H
#define _SERVER_H
//Qt
#include <QObject>
#include <QGSettings>
#include <QVariant>
#include <QtDBus>

//file
#include "filewatcher/filewatcher.h"
#include "convert-desktop-windowid/convertdesktoptowinid.h"

class Server : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface","org.ukui.panel.daemon")
public:
    explicit Server(QObject *parent = 0);

public Q_SLOTS:
	//desktop 文件路径转换为wid
	int DesktopToWID(QString desktop);
	//window Id 转化为desktop文件
	QString WIDToDesktop(int id);
    void DesktopFileDeleteSlot(QString path,QStringList deleteFile);

Q_SIGNALS:
    //desktop文件被删除
    QString DesktopFileDelete(QString);

//	//时间改变
//    QString TimeChanged();

private:
    FileWatcher *m_fileWatcher;
    ConvertDesktopToWinId *m_desktop;

};

#endif // UKUIPANEL_INFORMATION_H
