QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
TARGET = ukui-flash-disk
TEMPLATE = app
PKGCONFIG +=gio-2.0 peony
#LIBS +=-lgio-2.0 -lglib-2.0
CONFIG += c++11 link_pkgconfig no_keywords debug
DEFINES += QT_DEPRECATED_WARNINGS
SOURCES += \
         main.cpp \
         mainwindow.cpp \
         UnionVariable.cpp \
         qclickwidget.cpp \
         fdapplication.cpp \
         fdframe.cpp \
         flashdiskdata.cpp \
         fdclickwidget.cpp 
#        Removablemount.cpp


HEADERS += \
         UnionVariable.h \
         mainwindow.h \
         qclickwidget.h \
         fdapplication.h \
         fdframe.h \
         flashdiskdata.h \
         fdclickwidget.h 
#        Removablemount.h

FORMS   += mainwindow.ui

TRANSLATIONS += \
    ukui-flash-disk_tr.ts \
    ukui-flash-disk_zh_CN.ts

