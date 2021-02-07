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
#ifndef STORAGEARROW_H
#define STORAGEARROW_H

//Qt
#include <QToolButton>
#include <QPushButton>
#include <QGSettings>
enum storageArrowStatus{NORMAL,HOVER,PRESS};
class StorageArrow : public QPushButton {
public :
    StorageArrow(QWidget* parent );
    ~StorageArrow();
protected :
//    void paintEvent(QEvent *);
    /**
     * @brief enterEvent leaveEvent
     * @param event
     *  enterEvent leaveEvent 只是为了解决鼠标离开按钮后依然出现的悬浮现象
     */
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);
private:
    void setArrowIcon();
    int GetTaskbarInfo();
    QGSettings *gsetting;
    int panelPosition;
    int iconsize;
};
#endif
