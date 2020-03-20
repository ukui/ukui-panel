/*
 * Copyright (C) 2019 Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
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
#ifndef EJECTINTERFACE_H
#define EJECTINTERFACE_H
#include <QWidget>
#include <QLabel>
#include <QBoxLayout>
#include <QIcon>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QStyle>
#include <QStyleOption>
#include <QPainter>
QT_BEGIN_NAMESPACE
namespace Ui { class ejectInterface; }
QT_END_NAMESPACE
class ejectInterface : public QWidget
{
    Q_OBJECT
public:
    ejectInterface(QWidget *parent,QString name);
    ~ejectInterface();

private:
    QLabel *eject_image_label;
    QLabel *show_text_label;
    QLabel *mount_name_label;
    QHBoxLayout *ejectinterface_h_BoxLayout;
    QHBoxLayout *mountname_h_BoxLayout;
    QVBoxLayout *main_V_BoxLayput;

protected:
    void paintEvent(QPaintEvent *event);

};

#endif
