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

#ifndef LISTENGSETTINGS_H
#define LISTENGSETTINGS_H
//Qt
#include <QObject>
#include <QGSettings>
#include <QVariant>
class ListenGsettings : public QObject
{
    Q_OBJECT
public:
    ListenGsettings();
    QGSettings *m_panelGsettings;
Q_SIGNALS:
    void panelsizechanged(int panelsize);
    void iconsizechanged(int iconsize);
    void panelpositionchanged(int panelposition);
};

#endif // LISTENGSETTINGS_H
