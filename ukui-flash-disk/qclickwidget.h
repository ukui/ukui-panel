/*
 * Copyright: 2019 Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This program or library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
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
#include <qgsettings.h>

#include "ejectInterface.h"
#include "clickLabel.h"
#include "UnionVariable.h"
#include "interactivedialog.h"
#include "gpartedinterface.h"
class MainWindow;
class QClickWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QClickWidget(QWidget *parent = nullptr,
                          int num = 0,
                          GDrive *Drive=NULL,
                          GVolume *Volume=NULL,
                          QString driveName=NULL,
                          QString nameDis1=NULL,
                          QString nameDis2 =NULL,
                          QString nameDis3 = NULL,
                          QString nameDis4 = NULL,
                          qlonglong capacityDis1=0,
                          qlonglong capacityDis2=0,
                          qlonglong capacityDis3=0,
                          qlonglong capacityDis4=0,
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
    //void paintEvent(QPaintEvent *);

private:
    QIcon imgIcon;
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
    GDrive *m_Drive;
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
    QWidget *disWidgetNumOne;
    QWidget *disWidgetNumTwo;
    QWidget *disWidgetNumThree;
    QWidget *disWidgetNumFour;

    QGSettings *fontSettings = nullptr;
    QGSettings *qtSettings = nullptr;

    int fontSize;
    QString currentThemeMode;

public:
    QPushButton *m_eject_button = nullptr;
    ejectInterface *m_eject = nullptr;
    interactiveDialog *chooseDialog = nullptr;
    gpartedInterface *gpartedface = nullptr;
    bool ifSucess;
    int flagType;
Q_SIGNALS:
    void clicked();
    void clickedConvert();
    void noDeviceSig();

private Q_SLOTS:
    void on_volume1_clicked();
    void on_volume2_clicked();
    void on_volume3_clicked();
    void on_volume4_clicked();
    void switchWidgetClicked();
private:
    QString size_human(qlonglong capacity);
    QPixmap drawSymbolicColoredPixmap(const QPixmap &source);
protected:
    bool eventFilter(QObject *obj, QEvent *event);
    void resizeEvent(QResizeEvent *event);
public:
    void initFontSize();
    void initThemeMode();

};

#endif
