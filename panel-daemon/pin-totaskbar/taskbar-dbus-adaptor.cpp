/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp org.ukui.panel.xml -i pintotaskbar.h -a taskbar-dbus-adaptor
 *
 * qdbusxml2cpp is Copyright (C) 2020 The Qt Company Ltd.
 *
 * This is an auto-generated file.
 * Do not edit! All changes made to it will be lost.
 */

#include "taskbar-dbus-adaptor.h"
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

bool PanelAdaptor::AddToTaskbar(const QString &desktop)
{
    // handle method call org.ukui.panel.AddToTaskbar
    bool out0;
    QMetaObject::invokeMethod(parent(), "AddToTaskbar", Q_RETURN_ARG(bool, out0), Q_ARG(QString, desktop));
    return out0;
}

bool PanelAdaptor::CheckIfExist(const QString &desktop)
{
    // handle method call org.ukui.panel.CheckIfExist
    bool out0;
    QMetaObject::invokeMethod(parent(), "CheckIfExist", Q_RETURN_ARG(bool, out0), Q_ARG(QString, desktop));
    return out0;
}

bool PanelAdaptor::RemoveFromTaskbar(const QString &desktop)
{
    // handle method call org.ukui.panel.RemoveFromTaskbar
    bool out0;
    QMetaObject::invokeMethod(parent(), "RemoveFromTaskbar", Q_RETURN_ARG(bool, out0), Q_ARG(QString, desktop));
    return out0;
}

