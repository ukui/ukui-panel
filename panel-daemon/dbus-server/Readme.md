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

使用工具qdbuscpp2xml从dbus.h生成XML文件；
qdbuscpp2xml -M server.h -o org.ukui.panel.daemon.xml

B.使用工具qdbusxml2cpp从XML文件生成继承自QDBusAbstractAdaptor的类,供服务端使用
qdbusxml2cpp org.ukui.panel.daemon.xml -i server.h -a dbus-adaptor

https://blog.51cto.com/9291927/21184
