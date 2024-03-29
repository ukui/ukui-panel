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
#include "convertdesktoptowinid.h"
#include <KWindowSystem>
#include <QFile>
#include <QDebug>
#include <QDir>


ConvertDesktopToWinId::ConvertDesktopToWinId()
{
    //connect(KWindowSystem::self(), &KWindowSystem::windowAdded, this, &ConvertDesktopToWinId::tranIdToDesktop);
}

QString ConvertDesktopToWinId::tranIdToDesktop(WId id)
{
    QString desktopName = confirmDesktopFile(id);
    qDebug() << "desktopName is :" << desktopName;
    return desktopName;
}

QString ConvertDesktopToWinId::confirmDesktopFile(WId id)
{
    KWindowInfo info(id, 0, NET::WM2AllProperties);
    m_desktopfilePath = "";
    QDir infoDir(DEKSTOP_FILE_PATH);
    m_list = infoDir.entryInfoList();
    //跳过 ./ 和 ../ 目录
    m_list.removeAll(QFile(USR_SHARE_APP_CURRENT));
    m_list.removeAll(QFile(USR_SHARE_APP_UPER));

    //加入自启动目录下的desktop文件,优先使用/usr/share/applications/下的desktop
    QDir dir(AUTOSTART_DEKSTOP_FILE_PATH);
    QFileInfoList autostartList = dir.entryInfoList();
    m_list.append(autostartList);


    //第一种方法：比较名字一致性
    if (m_desktopfilePath.isEmpty()) {
        m_classClass = info.windowClassClass().toLower();
        m_className = info.windowClassName();

        //匹配安卓兼容
        if (m_className == "kylin-kmre-window") {
            searchAndroidApp(info);
            return m_desktopfilePath;
        }

        //匹配Inter端腾讯教育
        if (m_className == "txeduplatform") {
            searchTXeduApp(id);
            return m_desktopfilePath;
        }

        QFile file(QString("/proc/%1/status").arg(info.pid()));
        if (file.open(QIODevice::ReadOnly)) {
            char buf[1024];
            qint64 len=file.readLine(buf,sizeof(buf));
            if (len!=-1) {
                m_statusName = QString::fromLocal8Bit(buf).remove("Name:").remove("\t").remove("\n");
            }
        }
        compareClassName();
    }

    //第二种方法：获取点击应用时大部分desktop文件名
    if (m_desktopfilePath.isEmpty()) {
        searchFromEnviron(info);
    }

    //第三种方法：比较cmd命令行操作一致性
    if (m_desktopfilePath.isEmpty()) {
        QFile file(QString("/proc/%1/cmdline").arg(info.pid()));
        if (file.open(QIODevice::ReadOnly)) {
            char buf[1024];
            qint64 len=file.readLine(buf,sizeof(buf));
            if (len!=-1) {
                m_cmdLine = QString::fromLocal8Bit(buf).remove("\n");
            }
        }
        compareCmdExec();
    }

    //第四种方法：匹配部分字段
    if (m_desktopfilePath.isEmpty()) {
        compareLastStrategy();
    }
    return m_desktopfilePath;
}

void ConvertDesktopToWinId::searchAndroidApp(KWindowInfo info)
{
    QDir androidDir(QString(QDir::homePath() + ANDROID_FILE_PATH));
    m_androidList = androidDir.entryInfoList();
    m_androidList.removeAll(QDir::homePath() + ANDROID_APP_CURRENT);
    m_androidList.removeAll(QDir::homePath() + ANDROID_APP_UPER);

    QFile file(QString("/proc/%1/cmdline").arg(info.pid()));
    file.open(QIODevice::ReadOnly);
    QByteArray cmd = file.readAll();
    file.close();
    QList<QByteArray> cmdList = cmd.split('\0');
    for(int i = 0; i < m_androidList.size(); i++){
        QFileInfo fileInfo = m_androidList.at(i);
        QString desktopName = fileInfo.filePath();
        if(!fileInfo.filePath().endsWith(".desktop")){
            continue;
        }
        desktopName = desktopName.mid(desktopName.lastIndexOf("/") + 1);
        desktopName = desktopName.left(desktopName.lastIndexOf("."));
        if(desktopName == cmdList.at(10)){
            m_desktopfilePath = fileInfo.filePath();
            break;
        }
    }
}

void ConvertDesktopToWinId::searchTXeduApp(WId id)
{
    KWindowInfo info(id, NET::WMAllProperties);
    QString name = info.name();
    for (int i = 0; i < m_list.size(); i++) {
        QString cmd;
        QFileInfo fileInfo = m_list.at(i);
        if (!fileInfo.filePath().endsWith(".desktop")) {
            continue;
        }
        cmd.sprintf(GET_DESKTOP_NAME_MAIN, fileInfo.filePath().toStdString().data());
        QString desktopName = getDesktopFileName(cmd).remove("\n");

        if (desktopName == name) {
            m_desktopfilePath = fileInfo.filePath();
            break;
        }
    }
}

void ConvertDesktopToWinId::searchFromEnviron(KWindowInfo info)
{
    QFile file("/proc/" + QString::number(info.pid()) + "/environ");
    file.open(QIODevice::ReadOnly);
    QByteArray BA = file.readAll();
    file.close();
    QList<QByteArray> list_BA = BA.split('\0');
    for (int i = 0; i < list_BA.length(); i++) {
        if (list_BA.at(i).startsWith("GIO_LAUNCHED_DESKTOP_FILE=")) {
            m_desktopfilePath = list_BA.at(i);
            m_desktopfilePath = m_desktopfilePath.mid(m_desktopfilePath.indexOf("=") + 1);
            //desktop文件地址需要重写
            m_desktopfilePath = m_desktopfilePath.mid(m_desktopfilePath.lastIndexOf("/") + 1);
            break;
        }
    }
    //desktop文件地址重写
    if (!m_desktopfilePath.isEmpty()) {
        for (int i = 0; i < m_list.size(); i++) {
            QFileInfo fileInfo = m_list.at(i);;
            if (fileInfo.filePath() == DEKSTOP_FILE_PATH + m_desktopfilePath) {
                m_desktopfilePath = fileInfo.filePath();
                break;
            }
        }
    }

}

void ConvertDesktopToWinId::compareClassName()
{
    for (int i = 0; i < m_list.size(); i++) {
        QFileInfo fileInfo = m_list.at(i);;
        QString path_desktop_name = fileInfo.filePath();
        if (!fileInfo.filePath().endsWith(".desktop")) {
            continue;
        }
        path_desktop_name = path_desktop_name.mid(path_desktop_name.lastIndexOf("/") + 1);
        path_desktop_name = path_desktop_name.left(path_desktop_name.lastIndexOf("."));
        if (path_desktop_name == m_classClass || path_desktop_name == m_className || path_desktop_name == m_statusName)  {
            m_desktopfilePath = fileInfo.filePath();
            break;
        }
    }
}

void ConvertDesktopToWinId::compareCmdExec()
{
    for (int i = 0; i < m_list.size(); i++) {
        QString cmd;
        QFileInfo fileInfo = m_list.at(i);
        if (!fileInfo.filePath().endsWith(".desktop")) {
            continue;
        }
        cmd.sprintf(GET_DESKTOP_EXEC_NAME_MAIN, fileInfo.filePath().toStdString().data());
        QString desktopFileExeName = getDesktopFileName(cmd).remove("\n");

        if (desktopFileExeName.isEmpty()) {
            continue;
        }

        if (desktopFileExeName == m_cmdLine || desktopFileExeName.startsWith(m_cmdLine) || m_cmdLine.startsWith(desktopFileExeName)) {
            m_desktopfilePath = fileInfo.filePath();
            break;
        }

        //仅仅是为了适配微信
        if (m_desktopfilePath.isEmpty()) {
            desktopFileExeName = "/usr/lib/" + desktopFileExeName;
            if (desktopFileExeName == m_cmdLine || desktopFileExeName.startsWith(m_cmdLine) || m_cmdLine.startsWith(desktopFileExeName)) {
                m_desktopfilePath = fileInfo.filePath();
            }
        }
    }
}

//最后的匹配策略汇总
void ConvertDesktopToWinId::compareLastStrategy()
{
    compareCmdName();

    if (m_desktopfilePath.isEmpty()) {
        compareDesktopClass();
    }

    if (m_desktopfilePath.isEmpty()) {
        containsName();
    }
}

void ConvertDesktopToWinId::compareCmdName()
{
    for (int i = 0; i < m_list.size(); i++) {
        QString cmd;
        QFileInfo fileInfo = m_list.at(i);
        if (!fileInfo.filePath().endsWith(".desktop")) {
            continue;
        }
        cmd.sprintf(GET_DESKTOP_EXEC_NAME_MAIN, fileInfo.filePath().toStdString().data());
        QString desktopFileExeName = getDesktopFileName(cmd).remove("\n");

        if (desktopFileExeName.isEmpty()) {
            continue;
        }

        if (desktopFileExeName.startsWith(m_className) || desktopFileExeName.endsWith(m_className)) {
            m_desktopfilePath = fileInfo.filePath();
            break;
        }
    }
}

void ConvertDesktopToWinId::compareDesktopClass()
{
    for (int i = 0; i < m_list.size(); i++) {
        QFileInfo fileInfo = m_list.at(i);
        QString path_desktop_name = fileInfo.filePath();
        if (!fileInfo.filePath().endsWith(".desktop")) {
            continue;
        }
        path_desktop_name = path_desktop_name.mid(path_desktop_name.lastIndexOf("/") + 1);
        path_desktop_name = path_desktop_name.left(path_desktop_name.lastIndexOf("."));

        if (path_desktop_name.startsWith(m_className) || path_desktop_name.endsWith(m_className)) {
            m_desktopfilePath = fileInfo.filePath();
            break;
        }
        else if (m_className.startsWith(path_desktop_name) || m_className.endsWith(path_desktop_name)) {
            m_desktopfilePath = fileInfo.filePath();
            break;
        }
    }
}

void ConvertDesktopToWinId::containsName()
{
    for (int i = 0; i < m_list.size(); i++) {
        QString cmd;
        QFileInfo fileInfo = m_list.at(i);
        QString path_desktop_name = fileInfo.filePath();

        if (!fileInfo.filePath().endsWith(".desktop")) {
            continue;
        }

        cmd.sprintf(GET_DESKTOP_EXEC_NAME_MAIN, fileInfo.filePath().toStdString().data());
        QString desktopFileExeName = getDesktopFileName(cmd).remove("\n");

        path_desktop_name = path_desktop_name.mid(path_desktop_name.lastIndexOf("/") + 1);
        path_desktop_name = path_desktop_name.left(path_desktop_name.lastIndexOf("."));

        if (path_desktop_name.contains(m_className) || desktopFileExeName.contains(m_className)) {
            m_desktopfilePath = fileInfo.filePath();
            break;
        }
    }
}

//执行头文件中宏定义写好的终端指令获取对应的Exec字段
QString ConvertDesktopToWinId::getDesktopFileName(QString cmd)
{
    char name[200];
    FILE *fp1 = NULL;
    if ((fp1 = popen(cmd.toStdString().data(), "r")) == NULL)
        return QString();
    memset(name, 0, sizeof(name));
    fgets(name, sizeof(name), fp1);
    pclose(fp1);
    return QString(name);
}


ConvertDesktopToWinId::~ConvertDesktopToWinId()
{
}
