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

#ifndef FRMLUNARCALENDARWIDGET_H
#define FRMLUNARCALENDARWIDGET_H

#include <QWidget>
#include <QGSettings>
#include "calendardbus.h"
namespace Ui {
class frmLunarCalendarWidget;
}

class frmLunarCalendarWidget : public QWidget
{
    Q_OBJECT

public:
    explicit frmLunarCalendarWidget(QWidget *parent = 0);
    ~frmLunarCalendarWidget();

    void set_window_position();

    bool status;

protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *event);

private:
    Ui::frmLunarCalendarWidget *ui;
    QGSettings *transparency_gsettings;
    QGSettings *calendar_gsettings;
    CalendarDBus *mCalendarDBus;
    bool eventFilter(QObject *, QEvent *);

private Q_SLOTS:
    void initForm();
    void cboxCalendarStyle_currentIndexChanged(int index);
    void cboxSelectType_currentIndexChanged(int index);
    void cboxWeekNameFormat_currentIndexChanged(bool FirstDayisSun);
    void ckShowLunar_stateChanged(bool arg1);
    void changeUpSize();
    void changeDownSize();

Q_SIGNALS:
    void yijiChangeUp();
    void yijiChangeDown();
};

#endif // FRMLUNARCALENDARWIDGET_H
