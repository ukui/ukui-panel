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

#include "ukuitaskclosebutton.h"
#include <QDebug>
UKUITaskCloseButton::UKUITaskCloseButton(const WId window, QWidget *parent):
    QToolButton(parent),
    mWindow(window)
{
    //connect(parent, &UKUITaskBar::buttonRotationRefreshed, this, &UKUITaskGroup::setAutoRotation);
}


/************************************************

 ************************************************/
void UKUITaskCloseButton::mousePressEvent(QMouseEvent* event)
{
//    const Qt::MouseButton b = event->button();

//    if (Qt::LeftButton == b)
//        mDragStartPosition = event->pos();
//    else if (Qt::MidButton == b && parentTaskBar()->closeOnMiddleClick())
//        closeApplication();

    QToolButton::mousePressEvent(event);
}

/************************************************

 ************************************************/
void UKUITaskCloseButton::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button()== Qt::LeftButton)
    {
        emit sigClicked();
    }
    QToolButton::mouseReleaseEvent(event);

}
