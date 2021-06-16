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

#include "fdapplication.h"

FDApplication::FDApplication(int &argc, char **argv) 
    : QtSingleApplication(argc, argv) 
{
    
}

FDApplication::FDApplication(const QString &id, int &argc, char **argv)
    : QtSingleApplication(id, argc, argv)
{

}

FDApplication::~FDApplication() 
{

}

bool FDApplication::notify(QObject* obj, QEvent *event)
{
    Q_EMIT notifyWnd(obj, event);
    return QtSingleApplication::notify(obj, event);
}
