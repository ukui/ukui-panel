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
