/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp org.ukui.panel-daemon.desktopwid.xml -i server.h -a dbus-adaptor
 *
 * qdbusxml2cpp is Copyright (C) 2020 The Qt Company Ltd.
 *
 * This is an auto-generated file.
 * Do not edit! All changes made to it will be lost.
 */

#include "dbus-adaptor.h"
#include <QtCore/QMetaObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>

/*
 * Implementation of adaptor class PanelAdaptor
 */

PanelAdaptor::PanelAdaptor(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
    // constructor
    setAutoRelaySignals(true);
}

PanelAdaptor::~PanelAdaptor()
{
    // destructor
}

int PanelAdaptor::DesktopToWID(const QString &desktop)
{
    // handle method call org.ukui.panel.DesktopToWID
    int out0;
    QMetaObject::invokeMethod(parent(), "DesktopToWID", Q_RETURN_ARG(int, out0), Q_ARG(QString, desktop));
    return out0;
}

QString PanelAdaptor::WIDToDesktop(int id)
{
    // handle method call org.ukui.panel.WIDToDesktop
    QString out0;
    QMetaObject::invokeMethod(parent(), "WIDToDesktop", Q_RETURN_ARG(QString, out0), Q_ARG(int, id));
    return out0;
}

