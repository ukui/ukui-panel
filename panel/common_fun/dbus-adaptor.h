/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp org.ukui.panel.information.xml -i ukuipanel_infomation.h -a dbus-adaptor
 *
 * qdbusxml2cpp is Copyright (C) 2020 The Qt Company Ltd.
 *
 * This is an auto-generated file.
 * This file may have been hand-edited. Look for HAND-EDIT comments
 * before re-generating it.
 */

#ifndef DBUS-ADAPTOR_H
#define DBUS-ADAPTOR_H

#include <QtCore/QObject>
#include <QtDBus/QtDBus>
#include "ukuipanel_infomation.h"
QT_BEGIN_NAMESPACE
class QByteArray;
template<class T> class QList;
template<class Key, class Value> class QMap;
class QString;
class QStringList;
class QVariant;
QT_END_NAMESPACE

/*
 * Adaptor class for interface org.ukui.panel
 */
class PanelAdaptor: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.ukui.panel")
    Q_CLASSINFO("D-Bus Introspection", ""
"  <interface name=\"org.ukui.panel\">\n"
"    <method name=\"GetPrimaryScreenGeometry\">\n"
"      <arg direction=\"out\" type=\"av\"/>\n"
"    </method>\n"
"    <method name=\"GetPrimaryScreenAvailableGeometry\">\n"
"      <arg direction=\"out\" type=\"av\"/>\n"
"    </method>\n"
"    <method name=\"GetPrimaryScreenPhysicalGeometry\">\n"
"      <arg direction=\"out\" type=\"av\"/>\n"
"    </method>\n"
"    <method name=\"GetPanelPosition\">\n"
"      <arg direction=\"out\" type=\"s\"/>\n"
"    </method>\n"
"    <method name=\"setPanelInformation\">\n"
"      <arg direction=\"in\" type=\"i\"/>\n"
"      <arg direction=\"in\" type=\"i\"/>\n"
"      <arg direction=\"in\" type=\"i\"/>\n"
"      <arg direction=\"in\" type=\"i\"/>\n"
"      <arg direction=\"in\" type=\"i\"/>\n"
"      <arg direction=\"in\" type=\"i\"/>\n"
"    </method>\n"
"  </interface>\n"
        "")
public:
    PanelAdaptor(QObject *parent);
    virtual ~PanelAdaptor();

public: // PROPERTIES
public Q_SLOTS: // METHODS
    QString GetPanelPosition();
    QVariantList GetPrimaryScreenAvailableGeometry();
    QVariantList GetPrimaryScreenGeometry();
    QVariantList GetPrimaryScreenPhysicalGeometry();
    void setPanelInformation(int in0, int in1, int in2, int in3, int in4, int in5);
Q_SIGNALS: // SIGNALS
};

#endif
