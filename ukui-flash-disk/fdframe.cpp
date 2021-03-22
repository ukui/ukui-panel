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

#include <QPainter>
#include <QStyleOption>
#include <QDebug>
#include <QApplication>
#include <QScreen>

#include "fdframe.h"

FDFrame::FDFrame(QWidget* parent) : QFrame(parent)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::ToolTip);
    setAttribute(Qt::WA_AlwaysShowToolTips);
    setAttribute(Qt::WA_TranslucentBackground);
    //setWindowOpacity(0.8);
}

FDFrame::~FDFrame()
{

}

void FDFrame::paintEvent(QPaintEvent * event)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    QPainterPath rectPath;
    rectPath.addRoundedRect(this->rect(),4,4);
    
    // 绘制底色
    p.save();
    QStyleOption opt;
    opt.init(this);
    p.fillPath(rectPath, opt.palette.color(QPalette::Base));
    p.restore();
}