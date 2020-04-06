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
#include <QApplication>
#include <QDebug>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <syslog.h>
#include <QDebug>
#include <QtDBus/QDBusConnection>
#include <QTranslator>
#include "UnionVariable.h"
#include "mainwindow.h"
#include "MainController.h"



int main(int argc, char *argv[])
{
    //file lock to complete the single process
//    QStringList homePath = QStandardPaths::standardLocations(QStandardPaths::HomeLocation);
//    QString lockPath = homePath.at(0) + "/.config/ukui-flash-disk-lock";
//    int fd = open(lockPath.toUtf8().data(), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
//    if (fd < 0)
//    {
//        exit(1);
//    }

//    if (lockf(fd, F_TLOCK, 0))
//    {
//        syslog(LOG_ERR, "Can't lock single file, ukui-flash-disk is already running!");
//        qDebug()<<"Can't lock single file, ukui-flash-disk is already running!";
//        exit(0);
//    }

    QIcon::setThemeName("ukui-icon-theme-default");

    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(false);        //进程不隐式退出

    //load translation file
    QString locale = QLocale::system().name();
    QTranslator translator;
    if (locale == "zh_CN")
    {
        if (translator.load("/usr/share/ukui/ukui-panel/ukui-flash-disk_zh_CN.qm"))
        {
            a.installTranslator(&translator);
        }
        else
        {
            qDebug() << "Load translations file" << locale << "failed!";
        }

    }

    //load qss
    QFile qss(":ukui-flash-disk.qss");
    bool ok = qss.open(QFile::ReadOnly);
    if (!ok)
    qDebug() << "加载失败";
    qApp->setStyleSheet(qss.readAll());
    qss.close();
    MainController *ctrl = MainController::self();

    a.exec();
    delete ctrl;
    return 0;
}
