/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp org.ukui.panel.information.xml -i ukuipanel_infomation.h -a dbus-adaptor
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

QString PanelAdaptor::GetPanelPosition()
{
    // handle method call org.ukui.panel.GetPanelPosition
    QString out0;
    QMetaObject::invokeMethod(parent(), "GetPanelPosition", Q_RETURN_ARG(QString, out0));
    return out0;
}

QVariantList PanelAdaptor::GetPrimaryScreenAvailableGeometry()
{
    // handle method call org.ukui.panel.GetPrimaryScreenAvailableGeometry
    QVariantList out0;
    QMetaObject::invokeMethod(parent(), "GetPrimaryScreenAvailableGeometry", Q_RETURN_ARG(QVariantList, out0));
    return out0;
}

QVariantList PanelAdaptor::GetPrimaryScreenGeometry()
{
    // handle method call org.ukui.panel.GetPrimaryScreenGeometry
    QVariantList out0;
    QMetaObject::invokeMethod(parent(), "GetPrimaryScreenGeometry", Q_RETURN_ARG(QVariantList, out0));
    return out0;
}

QVariantList PanelAdaptor::GetPrimaryScreenPhysicalGeometry()
{
    // handle method call org.ukui.panel.GetPrimaryScreenPhysicalGeometry
    QVariantList out0;
    QMetaObject::invokeMethod(parent(), "GetPrimaryScreenPhysicalGeometry", Q_RETURN_ARG(QVariantList, out0));
    return out0;
}

void PanelAdaptor::setPanelInformation(int in0, int in1, int in2, int in3, int in4, int in5)
{
    // handle method call org.ukui.panel.setPanelInformation
    QMetaObject::invokeMethod(parent(), "setPanelInformation", Q_ARG(int, in0), Q_ARG(int, in1), Q_ARG(int, in2), Q_ARG(int, in3), Q_ARG(int, in4), Q_ARG(int, in5));
}

