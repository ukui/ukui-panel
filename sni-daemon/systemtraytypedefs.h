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

#ifndef SYSTEMTRAYTYPEDEFS_H
#define SYSTEMTRAYTYPEDEFS_H

#include <QByteArray>
#include <QString>
#include <QVector>

struct KDbusImageStruct {
    int width;
    int height;
    QByteArray data;
};

typedef QVector<KDbusImageStruct> KDbusImageVector;

struct KDbusToolTipStruct {
    QString icon;
    KDbusImageVector image;
    QString title;
    QString subTitle;
};

Q_DECLARE_METATYPE(KDbusImageStruct)
Q_DECLARE_METATYPE(KDbusImageVector)
Q_DECLARE_METATYPE(KDbusToolTipStruct)

#endif

