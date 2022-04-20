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

#ifndef STATUSNOTIFIERSTORAGEARROW_H
#define STATUSNOTIFIERSTORAGEARROW_H

#include <QToolButton>
#include <QGSettings>
#include <QDrag>
#include "statusnotifierwidget.h"

#define UKUI_PANEL_SETTINGS              "org.ukui.panel.settings"
#define SHOW_STATUSNOTIFIER_BUTTON       "statusnotifierbutton"
#define PANEL_POSITION_KEY  "panelposition"

class StatusNotifierWidget;

class StatusNotifierStorageArrow : public QToolButton
{
    Q_OBJECT
public:
    StatusNotifierStorageArrow(StatusNotifierWidget *parent = nullptr);
    ~StatusNotifierStorageArrow();
protected:
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void dropEvent(QDropEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void resizeEvent(QResizeEvent *event);

private:
    void setArrowIcon();

private:
    QGSettings *mGsettings;
    StatusNotifierWidget *mParent;
    int mPanelPosition;

signals:
    void addButton(QString button);
};

#endif // STATUSNOTIFIERSTORAGEARROW_H
