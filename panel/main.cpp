/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.com>
 *
 * Copyright: 2019 Tianjin KYLIN Information Technology Co., Ltd. *
 *
 * This program or library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */


#include "ukuipanelapplication.h"
#include <QTranslator>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <syslog.h>
#include <QStandardPaths>
#include <QDateTime>
#include <QMutex>
#include <QFile>
#include <QDir>
#include <X11/Xlib.h>
#include <ukui-log4qt.h>
/*! The ukui-panel is the panel of UKUI.
  Usage: ukui-panel [CONFIG_ID]
    CONFIG_ID      Section name in config file ~/.config/ukui/panel.conf
                   (default main)
 */
void messageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    QByteArray currentTime = QTime::currentTime().toString().toLocal8Bit();

    QString logFilePath = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/ukui-panel.log";

    bool showDebug = true;
    if (!QFile::exists(logFilePath)) {
        showDebug = false;
    }
    FILE *log_file = nullptr;

    if (showDebug) {
        log_file = fopen(logFilePath.toLocal8Bit().constData(), "a+");
    }

    const char *file = context.file ? context.file : "";
    const char *function = context.function ? context.function : "";
    switch (type) {
    case QtDebugMsg:
        if (!log_file) {
            break;
        }
        fprintf(log_file, "Debug: %s: %s (%s:%u, %s)\n", currentTime.constData(), localMsg.constData(), file, context.line, function);
        break;
    case QtInfoMsg:
        fprintf(log_file? log_file: stdout, "Info: %s: %s (%s:%u, %s)\n", currentTime.constData(), localMsg.constData(), file, context.line, function);
        break;
    case QtWarningMsg:
        fprintf(log_file? log_file: stderr, "Warning: %s: %s (%s:%u, %s)\n", currentTime.constData(), localMsg.constData(), file, context.line, function);
        break;
    case QtCriticalMsg:
        fprintf(log_file? log_file: stderr, "Critical: %s: %s (%s:%u, %s)\n", currentTime.constData(), localMsg.constData(), file, context.line, function);
        break;
    case QtFatalMsg:
        fprintf(log_file? log_file: stderr, "Fatal: %s: %s (%s:%u, %s)\n", currentTime.constData(), localMsg.constData(), file, context.line, function);
        break;
    }

    if (log_file)
        fclose(log_file);
}


int main(int argc, char *argv[])
{
    initUkuiLog4qt("ukui-panel");
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
#endif

    UKUIPanelApplication app(argc, argv);

//    qInstallMessageHandler(messageOutput);

    //Singleton
    QStringList homePath = QStandardPaths::standardLocations(QStandardPaths::HomeLocation);
    int fd = open(QString(homePath.at(0) + "/.config/ukui-panel%1.lock").arg(getenv("DISPLAY")).toUtf8().data(), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fd < 0) { exit(1); }
    if (lockf(fd, F_TLOCK, 0)) {
        syslog(LOG_ERR, "Can't lock single file, ukui-panel is already running!");
        exit(0);
    }

    //tanslate
//    QString locale = QLocale::system().name();
//    QTranslator translator;
//    if (locale == "zh_CN"){
//        if (translator.load("ukui-panel_zh_CN.qm", "/usr/share/ukui-panel/panel/resources/")){
//            app.installTranslator(&translator);
//        }
//        else{
//            qDebug() << "Load translations file" << locale << "failed!";
//        }
//    }
//    if (locale == "tr_TR"){
//        if (translator.load("ukui-panel_tr.qm", "/usr/share/ukui-panel/panel/resources/"))
//            app.installTranslator(&translator);
//        else
//            qDebug() << "Load translations file" << locale << "failed!";
//    }

    return app.exec();
}
