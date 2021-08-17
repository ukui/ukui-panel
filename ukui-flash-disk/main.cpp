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
#include <QTextCodec>
//#include <ukui-log4qt.h>
#include "UnionVariable.h"
#include "mainwindow.h"
#include "MainController.h"
#include "fdapplication.h"

int main(int argc, char *argv[])
{
//    initUkuiLog4qt("ukui-flash-disk");
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
#endif

    QTextCodec *codec = QTextCodec::codecForName("utf8"); //Linux
    QTextCodec::setCodecForLocale(codec);

    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QString id = QString("ukui-flash-disk_%1_%2").arg(getuid()).arg(QLatin1String(getenv("DISPLAY")));
    FDApplication a(id, argc, argv);
    a.setQuitOnLastWindowClosed(false);        //Process does not exit implicitly
    if (a.isRunning()) {
        qInfo()<<"ukui-flash-disk is runnning, now exit!";
        return 1;
    }

    //load translation file
    QString locale = QLocale::system().name();
    QTranslator translator;
    if (locale == "zh_CN")
    {
        if (translator.load("/usr/share/ukui/ukui-panel/ukui-flash-disk_zh_CN.qm"))
        {
            qDebug() << "load success";
            a.installTranslator(&translator);
        }
        else
        {
            qDebug() << "Load translations file" << locale << "failed!";
        }
    }

    if (QApplication::desktop()->width() >= 2560)
    {
        #if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
                QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
                QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
        #endif
    }

    MainController *ctrl = MainController::self();
    if (ctrl->init() != 0)  // single instance is running
        return 0;
    QObject::connect(&a, &FDApplication::notifyWnd, ctrl, &MainController::notifyWnd);
    a.exec();
    delete ctrl;
    return 0;
}
