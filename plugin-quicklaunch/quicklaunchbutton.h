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
#include <QPainter>
#include "popupmenu.h"

class IUKUIPanelPlugin;
//class CustomStyle;
#include "../panel/ukuicontrolstyle.h"
class QuicklaunchMenu:public QMenu
{
public:
    QuicklaunchMenu();
    ~QuicklaunchMenu();
protected:
    void contextMenuEvent(QContextMenuEvent*);

};
class QuickLaunchButton : public QToolButton
{
    Q_OBJECT

public:
    QuickLaunchButton(QuickLaunchAction * act, IUKUIPanelPlugin * plugin, QWidget* parent = 0);
    ~QuickLaunchButton();

    QHash<QString,QString> settingsMap();
    QString file_name;

signals:
    void buttonDeleted();
    void switchButtons(QuickLaunchButton *from, QuickLaunchButton *to);
    void movedLeft();
    void movedRight();

protected:
    //! Disable that annoying small arrow when there is a menu
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void dragEnterEvent(QDragEnterEvent *e);
    void dragMoveEvent(QDragMoveEvent * e);
    void contextMenuEvent(QContextMenuEvent*);
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);

private:
    QuickLaunchAction *mAct;
    IUKUIPanelPlugin * mPlugin;
    QAction *mDeleteAct;
    QAction *mMoveLeftAct;
    QAction *mMoveRightAct;
    QuicklaunchMenu *mMenu;
    QPoint mDragStart;
    enum QuickLaunchStatus{NORMAL, HOVER, PRESS};
    QuickLaunchStatus quicklanuchstatus;
    CustomStyle toolbuttonstyle;

private slots:
    void this_customContextMenuRequested(const QPoint & pos);
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
