/*
 * Copyright (C) 2021 KylinSoft Co., Ltd.
 * 
 * Authors:
 *  Yang Min yangmin@kylinos.cn
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

#ifndef __FDFRAME_H__
#define __FDFRAME_H__

#include <QFrame>
#include <QPainterPath>
#include <QGSettings>

class FDFrame : public QFrame 
{
    Q_OBJECT
public:
    explicit FDFrame(QWidget* parent);
    virtual ~FDFrame();

    void initOpacityGSettings();

protected:
    void paintEvent(QPaintEvent * event);

private:
    // QGSettings
    QGSettings *m_gsTransOpacity = nullptr;
    qreal m_curTransOpacity = 1;
};

#endif
