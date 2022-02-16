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

#pragma execution_character_set("utf-8")

#include "frmlunarcalendarwidget.h"
#include <QApplication>
#include <QTextCodec>
#include <QTranslator>
#include <QDebug>
#include <QTimer>
#include <KWindowEffects>

#include "xatom-helper.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setFont(QFont("Microsoft Yahei", 9));

#if (QT_VERSION <= QT_VERSION_CHECK(5,0,0))
#if _MSC_VER
    QTextCodec *codec = QTextCodec::codecForName("gbk");
#else
    QTextCodec *codec = QTextCodec::codecForName("utf-8");
#endif
    QTextCodec::setCodecForLocale(codec);
    QTextCodec::setCodecForCStrings(codec);
    QTextCodec::setCodecForTr(codec);
#else
    QTextCodec *codec = QTextCodec::codecForName("utf-8");
    QTextCodec::setCodecForLocale(codec);
#endif

    frmLunarCalendarWidget w;
    w.setWindowTitle("自定义农历控件");



    KWindowEffects::enableBlurBehind(w.winId(),true);

    // 添加窗管协议
    MotifWmHints hints;
    hints.flags = MWM_HINTS_FUNCTIONS|MWM_HINTS_DECORATIONS;
    hints.functions = MWM_FUNC_ALL;
    hints.decorations = MWM_DECOR_BORDER;
    XAtomHelper::getInstance()->setWindowMotifHint(w.winId(), hints);


    QApplication app(argc, argv);
    QString locale = QLocale::system().name();
        QTranslator translator;
        if (locale == "zh_CN"){
            if (translator.load("ukui-calendar_zh_CN.qm", "/usr/share/ukui-panel/panel/resources/"))
                app.installTranslator(&translator);
            else
                qDebug() << "Load translations file" << locale << "failed!";
        }
        if (locale == "tr_TR"){
            if (translator.load("ukui-panel_tr.qm", "/usr/share/ukui-panel/panel/resources/"))
                app.installTranslator(&translator);
            else
                qDebug() << "Load translations file" << locale << "failed!";
        }
        if (locale == "bo_CN"){
            if (translator.load("ukui-panel_bo_CN.qm", "/usr/share/ukui-panel/panel/resources/"))
                app.installTranslator(&translator);
            else
                qDebug() << "Load translations file" << locale << "failed!";
        }


    return a.exec();
}
