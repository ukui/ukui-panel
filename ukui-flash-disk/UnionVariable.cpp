/*
 * Copyright (C) 2019 Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
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
#include "UnionVariable.h"
#include <QString>
#include <QFont>
#include <QFontMetrics>

static QList<GMount *> gmountList;
QList<GMount *> *findGMountList()
{
    return &gmountList;
}

static QList<GVolume *> gvolumeList;
QList<GVolume *> *findGVolumeList()
{
    return &gvolumeList;
}

static QList<GDrive *> gdriveList;
QList<GDrive *> *findGDriveList()
{
    return &gdriveList;
}

QString getElidedText(QFont font, QString str, int MaxWidth)
{
    if (str.isEmpty())
    {
        return "";
    }

    QFontMetrics fontWidth(font);

    //计算字符串宽度
    //calculat the width of the string
    int width = fontWidth.width(str);

    //当字符串宽度大于最大宽度时进行转换
    //Convert when string width is greater than maximum width
    if (width >= MaxWidth)
    {
        //右部显示省略号
        //show by ellipsis in right
        str = fontWidth.elidedText(str, Qt::ElideRight, MaxWidth);
    }
    //返回处理后的字符串
    //return the string that is been handled
    return str;
}


