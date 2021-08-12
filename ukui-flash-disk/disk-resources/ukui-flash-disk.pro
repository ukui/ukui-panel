QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
TARGET = ukui-flash-disk
TEMPLATE = app
PKGCONFIG +=gio-2.0 peony
#LIBS +=-lgio-2.0 -lglib-2.0
CONFIG += c++11 link_pkgconfig no_keywords debug
DEFINES += QT_DEPRECATED_WARNINGS
SOURCES += \
         $$PWD/../main.cpp \
         $$PWD/../mainwindow.cpp \
         $$PWD/../device-manager.cpp \
         $$PWD/../device-operation.cpp \
         $$PWD/../repair-dialog-box.cpp \
         $$PWD/../UnionVariable.cpp \
         $$PWD/../qclickwidget.cpp \
         $$PWD/../fdapplication.cpp \
         $$PWD/../fdframe.cpp \
         $$PWD/../flashdiskdata.cpp \
         $$PWD/../fdclickwidget.cpp \
         $$PWD/../interactivedialog.cpp \
         $$PWD/../ejectInterface.cpp \
         $$PWD/../datacdrom.cpp 
#        Removablemount.cpp


HEADERS += \
         $$PWD/../device-manager.h \
         $$PWD/../device-operation.h \
         $$PWD/../repair-dialog-box.h \
         $$PWD/../UnionVariable.h \
         $$PWD/../mainwindow.h \
         $$PWD/../qclickwidget.h \
         $$PWD/../fdapplication.h \
         $$PWD/../fdframe.h \
         $$PWD/../flashdiskdata.h \
         $$PWD/../fdclickwidget.h \
         $$PWD/../interactivedialog.h \
         $$PWD/../ejectInterface.h \
         $$PWD/../datacdrom.h 
#        Removablemount.h

FORMS   += mainwindow.ui

TRANSLATIONS += \
    ukui-flash-disk_tr.ts \
    ukui-flash-disk_zh_CN.ts

