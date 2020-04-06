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

#ifndef UKUICONTROLSTYLE_H
#define UKUICONTROLSTYLE_H
#include <QMenu>
#include <QToolButton>
#include <QFrame>
#include <QWidget>
class UKUiMenu:public QMenu
{
public:
    UKUiMenu();
    ~UKUiMenu();

protected:
    void paintEvent(QPaintEvent*);
};

class UkuiToolButton:public QToolButton
{
public:
    UkuiToolButton();
    ~UkuiToolButton();
    void paintTooltipStyle();
};

class UKUiFrame:public QFrame
{
public:
    UKUiFrame();
    ~UKUiFrame();
protected:
    void paintEvent(QPaintEvent*);
};

class UKUiWidget:public QWidget
{
public:
    UKUiWidget();
    ~UKUiWidget();
protected:
    void paintEvent(QPaintEvent*);
};
#endif
