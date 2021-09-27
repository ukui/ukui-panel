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

//Qt
#include <QStyleOption>
#include <QPainter>
#include <QDesktopWidget>
#include <QApplication>
#include <QTimer>

#include "storagearrow.h"
#include "../panel/customstyle.h"
#include "../panel/iukuipanel.h"
#define PANEL_SETTINGS      "org.ukui.panel.settings"
#define PANEL_POSITION_KEY  "panelposition"
#define ICON_SIZE_KEY       "iconsize"

storageArrowStatus state=NORMAL;
StorageArrow::StorageArrow(QWidget* parent):
    QPushButton(parent)
{
    setStyle(new CustomStyle());
//    setVisible(false);

    QTimer::singleShot(10,[this]{setArrowIcon();});
    const QByteArray id(PANEL_SETTINGS);
    gsetting = new QGSettings(id);
    panelPosition = gsetting->get(PANEL_POSITION_KEY).toInt();
    iconsize=gsetting->get(ICON_SIZE_KEY).toInt();
    connect(gsetting, &QGSettings::changed, this, [=] (const QString &key){
        if(key == PANEL_POSITION_KEY){
            panelPosition=gsetting->get(PANEL_POSITION_KEY).toInt();
            setArrowIcon();
        }
        if(key == ICON_SIZE_KEY){
            iconsize=gsetting->get(ICON_SIZE_KEY).toInt();
            setArrowIcon();
        }
    });

}

StorageArrow::~StorageArrow() { }

//void StorageArrow::paintEvent(QEvent *e)
//{
//    QStyleOption opt;
//    opt.initFrom(this);
//    QPainter p(this);

//    switch(state)
//    {
//    case NORMAL:
//        p.setBrush(QColor(0xff,0xff,0xff,0x0f));
//        p.setPen(Qt::NoPen);
//        break;
//    case HOVER:
//        p.setBrush(QColor(0xff,0xff,0xff,0x1f));
//        p.setPen(Qt::NoPen);
//        break;
//    case PRESS:
////        p.setBrush(Qt::green);
//        p.setPen(Qt::NoPen);
//        break;
//    }
//    p.setRenderHint(QPainter::Antialiasing);  // 反锯齿;
//    p.drawRoundedRect(opt.rect,4,4);
//    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
//}

int StorageArrow::GetTaskbarInfo()
{


}

void StorageArrow::setArrowIcon()
{
    switch (panelPosition) {
    case 1:
        setIcon(QIcon::fromTheme("pan-down-symbolic"));
        break;
    case 2:
        setIcon(QIcon::fromTheme("pan-end-symbolic"));
        break;
    case 3:
        setIcon(QIcon::fromTheme("pan-start-symbolic"));
        break;
    default:
        setIcon(QIcon::fromTheme("pan-up-symbolic"));
        break;
    }
    setProperty("useIconHighlightEffect", 0x2);
    setIconSize(QSize(iconsize/2,iconsize/2));
    setFixedSize(iconsize,iconsize * 1.3);
}

void StorageArrow::enterEvent(QEvent *) {
    repaint();
    return;
}

void StorageArrow::leaveEvent(QEvent *) {
    repaint();
    return;
}
