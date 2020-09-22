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

#ifndef UKUITRAYSTOTAGE_H
#define UKUITRAYSTOTAGE_H

#include <QPainter>
#include "trayicon.h"
#include "../panel/ukuipanellayout.h"
#include "../panel/common/ukuigridlayout.h"
#include "xfitman.h"

class UKUiStorageWidget;
enum storageBarStatus{ST_HIDE,ST_SHOW};
/**
 * @brief This makes our storage
 */
class UKUIStorageFrame:public QWidget
{
    Q_OBJECT
public:
    UKUIStorageFrame(QWidget* parent =0);
    ~UKUIStorageFrame();
protected:
    bool eventFilter(QObject *, QEvent *);
    void paintEvent(QPaintEvent *event)override;
private:
    Atom _NET_SYSTEM_TRAY_OPCODE;
};

class UKUiStorageWidget:public QWidget
{
public:
    UKUiStorageWidget();
    ~UKUiStorageWidget();
protected:
    void paintEvent(QPaintEvent*);
};

#endif
