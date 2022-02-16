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
#include "server.h"
//Qt
#include <QDebug>
#include <QDBusMessage>
#include "convert-desktop-windowid/convertdesktoptowinid.h"


Server::Server(QObject *parent) :
    QObject(parent)
{
    mFileWatcher = new FileWatcher();
    connect(mFileWatcher,&FileWatcher::DesktopDeleteFile,this,&Server::DesktopFileDeleteSlot);
    mDesktop = new ConvertDesktopToWinId();
}


QString Server::WIDToDesktop(int id)
{
    return mDesktop->tranIdToDesktop(id);
}

int Server::DesktopToWID(QString desktop)
{

}

void Server::DesktopFileDeleteSlot(QString path,QStringList deleteFile){
//    emit DesktopFileDelete(path,deleteFile);
}

//QString Server::TimeChanged()
//{

//}
