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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QtWidgets>
#include <QLabel>
#include <QPushButton>
#include "qclickwidget.h"
#include<QSystemTrayIcon>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QVBoxLayout *vboxlayout;
    QLabel *no_device_label;
    QPushButton *eject_image_button;
    void newarea(QString name, qlonglong capacity, QString path,int linestatus);
    void moveBottomRight();
    QString size_human(qlonglong capacity);
    //QSystemTrayIcon m_systray;
    //void initUi();

public Q_SLOTS:
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    //void ejectDevice();
Q_SIGNALS:
    void clicked();
};

#endif
