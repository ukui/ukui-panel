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

/*
 * 用来动态绑定gsetting
*/

#ifndef TRAYDYNAMICGSETTING_H
#define TRAYDYNAMICGSETTING_H
//Qt
#include <QString>
#include <QList>
#include <QDebug>
#include <QGSettings>
//x11
#include <X11/X.h>

/**
 * 通过设置gsetting来调节应用在托盘或者收纳
 * 以下listExistsPath findFreePath
 */
/**
 * @brief listExistsPath
 * @return
 *
 * 列出存在的可供gsettings使用的路径
 */
QList<char *> listExistsPath();
QString findFreePath();
/**
 * @brief freezeTrayApp
 * @param winId
 * 将托盘应用置为freeze的状态
 */
void freezeTrayApp(int winId);

/**
 * @brief freezeApp
 * 将所有的托盘应用的状态至为freeze
 * 一般存在与在任务栏退出的时候
 */
void freezeApp();
#endif // TRAYDYNAMICGSETTING_H
