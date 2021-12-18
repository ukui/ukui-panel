/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp org.ukui.panel.xml -i pintotaskbar.h -a taskbar-dbus-adaptor
 *
 * qdbusxml2cpp is Copyright (C) 2020 The Qt Company Ltd.
 *
 * This is an auto-generated file.
 * This file may have been hand-edited. Look for HAND-EDIT comments
 * before re-generating it.
 */

#ifndef TASKBAR-DBUS-ADAPTOR_H
#define TASKBAR-DBUS-ADAPTOR_H

#include <QtCore/QObject>
#include <QtDBus/QtDBus>
#include "pintotaskbar.h"
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
"    <method name=\"AddToTaskbar\">\n"
"      <arg direction=\"out\" type=\"b\"/>\n"
"      <arg direction=\"in\" type=\"s\" name=\"desktop\"/>\n"
"    </method>\n"
"    <method name=\"RemoveFromTaskbar\">\n"
"      <arg direction=\"out\" type=\"b\"/>\n"
"      <arg direction=\"in\" type=\"s\" name=\"desktop\"/>\n"
"    </method>\n"
"    <method name=\"CheckIfExist\">\n"
"      <arg direction=\"out\" type=\"b\"/>\n"
"      <arg direction=\"in\" type=\"s\" name=\"desktop\"/>\n"
"    </method>\n"
"  </interface>\n"
        "")
public:
    PanelAdaptor(QObject *parent);
    virtual ~PanelAdaptor();

public: // PROPERTIES
public Q_SLOTS: // METHODS
    bool AddToTaskbar(const QString &desktop);
    bool CheckIfExist(const QString &desktop);
    bool RemoveFromTaskbar(const QString &desktop);
Q_SIGNALS: // SIGNALS
};

#endif
