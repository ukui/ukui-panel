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

#ifndef __FDAPPLICATION_H__
#define __FDAPPLICATION_H__

#include <QApplication>
#include <QDebug>

class FDApplication : public QApplication
{
    Q_OBJECT
public:
    FDApplication(int &argc, char **argv);
    virtual ~FDApplication();

    bool notify(QObject* obj, QEvent *event);

Q_SIGNALS:
    void notifyWnd(QObject* obj, QEvent *event);
};

#endif // __FDAPPLICATION_H__