/*
 * Copyright (C) 2019 Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
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
#ifndef QCLICKWIDGET_H
#define QCLICKWIDGET_H
#include <QWidget>
#include <QProcess>
#include <QDebug>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QStyle>
#include <QStyleOption>
#include <QPainter>
#include <QLabel>
#include <QBoxLayout>
#include <QPushButton>
#include <QToolButton>

#include "clickLabel.h"
class MainWindow;
class QClickWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QClickWidget(QWidget *parent = nullptr,
                          int num = 0,
                          QString driveName=NULL,
                          QString nameDis1=NULL,
                          QString nameDis2 =NULL,
                          QString nameDis3 = NULL,
                          QString nameDis4 = NULL,
                          qlonglong capacityDis1=NULL,
                          qlonglong capacityDis2=NULL,
                          qlonglong capacityDis3=NULL,
                          qlonglong capacityDis4=NULL,
                          QString pathDis1=NULL,
                          QString pathDis2=NULL,
                          QString pathDis3=NULL,
                          QString pathDis4=NULL);
    ~QClickWidget();
public Q_SLOTS:
    void mouseClicked();
protected:
    void mousePressEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *ev);
    void paintEvent(QPaintEvent *);

private:
    QString m_driveName;
    QString m_nameDis1;
    QString m_nameDis2;
    QString m_nameDis3;
    QString m_nameDis4;
    qlonglong m_capacityDis1;
    qlonglong m_capacityDis2;
    qlonglong m_capacityDis3;
    qlonglong m_capacityDis4;
    QString m_pathDis1;
    QString m_pathDis2;
    QString m_pathDis3;
    QString m_pathDis4;
    MainWindow *m_mainwindow;
    QPoint mousePos;
    int m_Num;
    QLabel *image_show_label;
    QLabel *m_driveName_label;
    ClickLabel *m_nameDis1_label;
    ClickLabel *m_nameDis2_label;
    ClickLabel *m_nameDis3_label;
    ClickLabel *m_nameDis4_label;
    QLabel *m_capacityDis1_label;
    QLabel *m_capacityDis2_label;
    QLabel *m_capacityDis3_label;
    QLabel *m_capacityDis4_label;

public:
    QPushButton *m_eject_button;
Q_SIGNALS:
    void clicked();
    void clickedConvert();

private Q_SLOTS:
    void on_volume1_clicked();
    void on_volume2_clicked();
    void on_volume3_clicked();
    void on_volume4_clicked();
    void switchWidgetClicked();
private:
    QString size_human(qlonglong capacity);
protected:
    bool eventFilter(QObject *obj, QEvent *event);
    void resizeEvent(QResizeEvent *event);
public:
    QString getElidedText(QFont font, QString str, int MaxWidth);

};

#endif
