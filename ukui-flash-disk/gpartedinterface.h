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
#ifndef GPARTEDINTERFACE_H
#define GPARTEDINTERFACE_H

#include "MacroFile.h"

#include <QWidget>
#include <QtWidgets>
#include <QLabel>
#include <QBoxLayout>
#include <QIcon>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QStyle>
#include <QStyleOption>
#include <QPainter>
#include <QPainterPath>
#include <QTimer>
#include <QApplication>
#include <QScreen>
#include <QDebug>
#include <QPushButton>
#include <QLabel>
#include <qgsettings.h>

QT_BEGIN_NAMESPACE
namespace Ui { class gpartedInterface; }
QT_END_NAMESPACE

class gpartedInterface: public QWidget
{
    Q_OBJECT
public:
    gpartedInterface(QWidget *parent);
    ~gpartedInterface();
private:
    double m_transparency;
    QGSettings *m_transparency_gsettings = nullptr;
    QGSettings *gsetting = nullptr;

    QPushButton *okButton;
    QLabel *noticeLabel;
    QHBoxLayout *notice_H_BoxLayout = nullptr;
    QHBoxLayout *button_H_BoxLayout = nullptr;
    QVBoxLayout *main_V_BoxLayout = nullptr;

private:
    void initWidgets();
    void moveChooseDialogRight();
    void initTransparentState();
    void getTransparentData();
protected:
    void paintEvent(QPaintEvent *event);
};

#endif //GPARTEDINTERFAC_H
