/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Copyright: 2010-2012 Razor team
 * Authors:
 *   Petr Vanek <petr@scribus.info>
 *   Kuzma Shapran <kuzma.shapran@gmail.com>
 *
 * Copyright: 2019 Tianjin KYLIN Information Technology Co., Ltd. *
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

 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */


#ifndef UKUIQUICKLAUNCHBUTTON_H
#define UKUIQUICKLAUNCHBUTTON_H

#include "quicklaunchaction.h"
#include <QMimeData>
#include <QToolButton>
#include "../panel/customstyle.h"
#include <QStyleOption>
#include <QGSettings>
#include <QPainter>
#include "popupmenu.h"

class IUKUIPanelPlugin;
//class CustomStyle;
#include "../panel/ukuicontrolstyle.h"

class QuickLaunchButton : public QToolButton
{
    Q_OBJECT

public:
    QuickLaunchButton(QuickLaunchAction * act, IUKUIPanelPlugin * plugin, QWidget* parent = 0);
    ~QuickLaunchButton();

    QHash<QString,QString> settingsMap();
    QString file_name;
    QString file;
    QString name;
    QString exec;
    static QString mimeDataFormat() { return QLatin1String("x-ukui/quicklaunch-button"); }

signals:
    void buttonDeleted();
    void switchButtons(QuickLaunchButton *from, QuickLaunchButton *to);
    void movedLeft();
    void movedRight();

protected:
    //! Disable that annoying small arrow when there is a menu
    /**
     * @brief contextMenuEvent
     * 右键菜单选项，从customContextMenuRequested的方式
     * 改为用contextMenuEvent函数处理
     */
    void contextMenuEvent(QContextMenuEvent*);
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);
    /**
     *  以下是拖拽相关函数
     */
    void dropEvent(QDropEvent *e);
    virtual QMimeData * mimeData();
    void dragLeaveEvent(QDragLeaveEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent* e);
    void dragEnterEvent(QDragEnterEvent *e);
    void dragMoveEvent(QDragMoveEvent * e);

private:
    QuickLaunchAction *mAct;
    IUKUIPanelPlugin * mPlugin;
    QAction *mDeleteAct;
    QAction *mMoveLeftAct;
    QAction *mMoveRightAct;
    QMenu *mMenu;
    QPoint mDragStart;
    enum QuickLaunchStatus{NORMAL, HOVER, PRESS};
    QuickLaunchStatus quicklanuchstatus;
    CustomStyle toolbuttonstyle;
    QGSettings *mgsettings;
    QString isComputerOrTrash(QString urlName);
public slots:
    void selfRemove();
};


class ButtonMimeData: public QMimeData
{
    Q_OBJECT
public:
    ButtonMimeData():
        QMimeData(),
        mButton(0)
    {
    }

    QuickLaunchButton *button() const { return mButton; }
    void setButton(QuickLaunchButton *button) { mButton = button; }

private:
    QuickLaunchButton *mButton;
};

#endif
