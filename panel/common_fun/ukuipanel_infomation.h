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
#ifndef UKUIPANEL_INFORMATION_H
#define UKUIPANEL_INFORMATION_H
//Qt
#include <QObject>
#include <QGSettings>
#include <QVariant>
#include <QtDBus>
class UKuiPanelInformation : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface","org.ukui.panel")
public:
    explicit UKuiPanelInformation(QObject *parent = 0);

private:
    int screen_x;
    int screen_y;
    int screen_width;
    int screen_height;
    int panelposition;
    int panelsize;

public Q_SLOTS:
    QVariantList GetPrimaryScreenGeometry();
    QVariantList GetPrimaryScreenAvailableGeometry();
    QVariantList GetPrimaryScreenPhysicalGeometry();
    QString GetPanelPosition();

    void setPanelInformation(int ,int ,int, int, int, int);
};

#endif // UKUIPANEL_INFORMATION_H
