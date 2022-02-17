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

#ifndef SNIXEMBEDPROXY_H
#define SNIXEMBEDPROXY_H

#include <QObject>
#include <QHash>
#include <QAbstractNativeEventFilter>

#include <xcb/xcb.h>

class KSelectionOwner;
class SNIProxy;

class SniXembedProxy : public QObject,public QAbstractNativeEventFilter
{
    Q_OBJECT

public:
    SniXembedProxy();
    ~SniXembedProxy();

protected:
    bool nativeEventFilter(const QByteArray &eventType, void *message, long *result) override;

private:
    void init();
    bool addDamageWatch(xcb_window_t client);
    void dock(xcb_window_t embed_win);
    void undock(xcb_window_t client);
    void setSystemTrayVisual();

private Q_SLOTS:
    void onClaimedOwnership();
    void onFailedToClaimOwnership();
    void onLostOwnership();

private:
    uint8_t m_damageEventBase;

    QHash<xcb_window_t, u_int32_t> m_damageWatches;
    QHash<xcb_window_t, SNIProxy *> m_proxies;
    KSelectionOwner *m_selectionOwner;

};
#endif // SNIXEMBEDPROXY_H
