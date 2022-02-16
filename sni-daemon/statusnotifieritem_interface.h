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

#ifndef STATUSNOTIFIERITEM_INTERFACE_H
#define STATUSNOTIFIERITEM_INTERFACE_H

#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtDBus/QtDBus>
#include "systemtraytypedefs.h"

/*
 * Proxy class for interface org.kde.StatusNotifierItem
 */
class OrgKdeStatusNotifierItemInterface: public QDBusAbstractInterface
{
    Q_OBJECT
public:
    static inline const char *staticInterfaceName()
    { return "org.kde.StatusNotifierItem"; }

public:
    OrgKdeStatusNotifierItemInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent = nullptr);

    ~OrgKdeStatusNotifierItemInterface();

    Q_PROPERTY(QString AttentionIconName READ attentionIconName)
    inline QString attentionIconName() const
    { return qvariant_cast< QString >(property("AttentionIconName")); }

    Q_PROPERTY(KDbusImageVector AttentionIconPixmap READ attentionIconPixmap)
    inline KDbusImageVector attentionIconPixmap() const
    { return qvariant_cast< KDbusImageVector >(property("AttentionIconPixmap")); }

    Q_PROPERTY(QString AttentionMovieName READ attentionMovieName)
    inline QString attentionMovieName() const
    { return qvariant_cast< QString >(property("AttentionMovieName")); }

    Q_PROPERTY(QString Category READ category)
    inline QString category() const
    { return qvariant_cast< QString >(property("Category")); }

    Q_PROPERTY(QString IconName READ iconName)
    inline QString iconName() const
    { return qvariant_cast< QString >(property("IconName")); }

    Q_PROPERTY(KDbusImageVector IconPixmap READ iconPixmap)
    inline KDbusImageVector iconPixmap() const
    { return qvariant_cast< KDbusImageVector >(property("IconPixmap")); }

    Q_PROPERTY(QString IconThemePath READ iconThemePath)
    inline QString iconThemePath() const
    { return qvariant_cast< QString >(property("IconThemePath")); }

    Q_PROPERTY(QString Id READ id)
    inline QString id() const
    { return qvariant_cast< QString >(property("Id")); }

    Q_PROPERTY(bool ItemIsMenu READ itemIsMenu)
    inline bool itemIsMenu() const
    { return qvariant_cast< bool >(property("ItemIsMenu")); }

    Q_PROPERTY(QDBusObjectPath Menu READ menu)
    inline QDBusObjectPath menu() const
    { return qvariant_cast< QDBusObjectPath >(property("Menu")); }

    Q_PROPERTY(QString OverlayIconName READ overlayIconName)
    inline QString overlayIconName() const
    { return qvariant_cast< QString >(property("OverlayIconName")); }

    Q_PROPERTY(KDbusImageVector OverlayIconPixmap READ overlayIconPixmap)
    inline KDbusImageVector overlayIconPixmap() const
    { return qvariant_cast< KDbusImageVector >(property("OverlayIconPixmap")); }

    Q_PROPERTY(QString Status READ status)
    inline QString status() const
    { return qvariant_cast< QString >(property("Status")); }

    Q_PROPERTY(QString Title READ title)
    inline QString title() const
    { return qvariant_cast< QString >(property("Title")); }

    Q_PROPERTY(KDbusToolTipStruct ToolTip READ toolTip)
    inline KDbusToolTipStruct toolTip() const
    { return qvariant_cast< KDbusToolTipStruct >(property("ToolTip")); }

    Q_PROPERTY(int WindowId READ windowId)
    inline int windowId() const
    { return qvariant_cast< int >(property("WindowId")); }

public Q_SLOTS: // METHODS
    inline QDBusPendingReply<> Activate(int x, int y)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(x) << QVariant::fromValue(y);
        return asyncCallWithArgumentList(QStringLiteral("Activate"), argumentList);
    }

    inline QDBusPendingReply<> ContextMenu(int x, int y)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(x) << QVariant::fromValue(y);
        return asyncCallWithArgumentList(QStringLiteral("ContextMenu"), argumentList);
    }

    inline QDBusPendingReply<> Scroll(int delta, const QString &orientation)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(delta) << QVariant::fromValue(orientation);
        return asyncCallWithArgumentList(QStringLiteral("Scroll"), argumentList);
    }

    inline QDBusPendingReply<> SecondaryActivate(int x, int y)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(x) << QVariant::fromValue(y);
        return asyncCallWithArgumentList(QStringLiteral("SecondaryActivate"), argumentList);
    }

Q_SIGNALS: // SIGNALS
    void NewAttentionIcon();
    void NewIcon();
    void NewOverlayIcon();
    void NewStatus(const QString &status);
    void NewTitle();
    void NewToolTip();
};

namespace org {
  namespace kde {
    typedef ::OrgKdeStatusNotifierItemInterface StatusNotifierItem;
  }
}
#endif
