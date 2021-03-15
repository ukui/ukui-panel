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

#ifndef __FDCLICKWIDGET_H__
#define __FDCLICKWIDGET_H__
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
class FDClickWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FDClickWidget(QWidget *parent = nullptr,
                          unsigned diskNo = 0,
                          QString strDriveId = "",
                          QString strVolumeId = "",
                          QString strMountId = "",
                          QString driveName = "",
                          QString volumeName = "",
                          quint64 capacityDis = 0,
                          QString strMountUri = "");
    ~FDClickWidget();

protected:
    void mousePressEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *ev);

private:
    QIcon imgIcon;
    unsigned m_uDiskNo;
    QString m_driveName;
    QString m_volumeName;
    quint64 m_capacityDis;
    QString m_mountUri;
    QString m_driveId;
    QString m_volumeId;
    QString m_mountId;
    MainWindow *m_mainwindow;
    QPoint mousePos;
    QLabel *image_show_label;
    QLabel *m_driveName_label;
    ClickLabel *m_nameDis1_label;
    QLabel *m_capacityDis1_label;
    QWidget *disWidgetNumOne;

    QGSettings *fontSettings = nullptr;
    QGSettings *qtSettings = nullptr;

    int fontSize;
    QString currentThemeMode;

public:
    QPushButton *m_eject_button = nullptr;
    ejectInterface *m_eject = nullptr;
    interactiveDialog *chooseDialog = nullptr;
    gpartedInterface *gpartedface = nullptr;
    bool ifSucess = false;
Q_SIGNALS:
    void clicked();
    void clickedEjectItem(FDClickWidget* pThis,QString strDriveId, QString strVolumeId, QString strMountId);
    void noDeviceSig();

private Q_SLOTS:
    void on_volume_clicked();
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

#endif // __FDCLICKWIDGET_H__
