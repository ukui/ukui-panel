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

#ifndef PINTOTASKBAR_H
#define PINTOTASKBAR_H

#include <QObject>

class PinToTaskbar : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface","com.ukui.panel.desktop")
public:
    PinToTaskbar(QObject *parent = 0);

public Q_SLOTS:
    bool AddToTaskbar(const QString &desktop);
    bool RemoveFromTaskbar(const QString &desktop);
    bool CheckIfExist(const QString &desktop);

private:
    QList<QMap<QString, QVariant> > getTaskbarFixedList();
};

#endif // PINTOTASKBAR_H
