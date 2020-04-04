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

#include "ukuicontrolstyle.h"
#include <QStyleOption>
#include <QPainter>
#include <QToolTip>
#include <QPalette>
UKUiMenu::UKUiMenu(){
}

UKUiMenu::~UKUiMenu(){
}

void UKUiMenu::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    p.setBrush(QBrush(Qt::red));
    p.setPen(Qt::black);
    p.drawRoundedRect(opt.rect,6,6);
//    p.drawText(rect(), opt. Qt::AlignCenter,);
    style()->drawPrimitive(QStyle::PE_PanelMenu, &opt, &p, this);
}

UkuiToolButton::UkuiToolButton(){}
UkuiToolButton::~UkuiToolButton(){}
void UkuiToolButton::paintTooltipStyle()
{
    //设置QToolTip颜色
    QPalette palette = QToolTip::palette();
    palette.setColor(QPalette::Inactive,QPalette::ToolTipBase,Qt::black);   //设置ToolTip背景色
    palette.setColor(QPalette::Inactive,QPalette::ToolTipText, Qt::white); 	//设置ToolTip字体色
    QToolTip::setPalette(palette);
//    QFont font("Segoe UI", -1, 50);
//    font.setPixelSize(12);
//    QToolTip::setFont(font);  //设置ToolTip字体
}

UKUiFrame::UKUiFrame(){
}

UKUiFrame::~UKUiFrame(){
}

void UKUiFrame::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    p.setBrush(QBrush(QColor(0x13,0x14,0x14,0xb2)));
    p.setPen(Qt::NoPen);
    p.setRenderHint(QPainter::Antialiasing);
    p.drawRoundedRect(opt.rect,6,6);
    style()->drawPrimitive(QStyle::PE_Frame, &opt, &p, this);
}

UKUiWidget::UKUiWidget(){
}

UKUiWidget::~UKUiWidget(){
}

void UKUiWidget::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    p.setBrush(QBrush(QColor(0x13,0x14,0x14,0xb2)));
    p.setPen(Qt::NoPen);
    p.setRenderHint(QPainter::Antialiasing);
    p.drawRoundedRect(opt.rect,6,6);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
