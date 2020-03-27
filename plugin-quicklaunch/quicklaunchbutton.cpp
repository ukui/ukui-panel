/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Petr Vanek <petr@scribus.info>
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

#include "quicklaunchbutton.h"
#include "ukuiquicklaunch.h"
#include "../panel/iukuipanelplugin.h"
#include <QAction>
#include <QDrag>
#include <QMenu>
#include <QMouseEvent>
#include <QStylePainter>
#include <QStyleOptionToolButton>
#include <QApplication>
#include <XdgIcon>
#include <string>

#define MIMETYPE "x-ukui/quicklaunch-button"


QuickLaunchButton::QuickLaunchButton(QuickLaunchAction * act, IUKUIPanelPlugin * plugin, QWidget * parent)
    : QToolButton(parent),
      mAct(act),
      mPlugin(plugin)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setAcceptDrops(true);
    setAutoRaise(true);
    quicklanuchstatus = NORMAL;

    setDefaultAction(mAct);
    mAct->setParent(this);

    mMoveLeftAct = new QAction(XdgIcon::fromTheme("go-previous"), tr("move to left"), this);
    connect(mMoveLeftAct, SIGNAL(triggered()), this, SIGNAL(movedLeft()));

    mMoveRightAct = new QAction(XdgIcon::fromTheme("go-next"), tr("move to right"), this);
    connect(mMoveRightAct, SIGNAL(triggered()), this, SIGNAL(movedRight()));


    mDeleteAct = new QAction(XdgIcon::fromTheme("dialog-close"), tr("delete from quicklaunch"), this);
    connect(mDeleteAct, SIGNAL(triggered()), this, SLOT(selfRemove()));
    addAction(mDeleteAct);
    mMenu = new QuicklaunchMenu();
    mMenu->addAction(mAct);
    mMenu->addActions(mAct->addtitionalActions());
    mMenu->addSeparator();
    mMenu->addAction(mMoveLeftAct);
    mMenu->addAction(mMoveRightAct);
    mMenu->addSeparator();
    mMenu->addAction(mDeleteAct);


    setContextMenuPolicy(Qt::CustomContextMenu);
//    mMenu.exec(QCursor::pos());
    connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
            this, SLOT(this_customContextMenuRequested(const QPoint&)));
    //file_name=act->m_settingsMap["name"];
    file_name=act->m_settingsMap["desktop"];
    this->setStyle(new CustomStyle());
    repaint();

}


QuickLaunchButton::~QuickLaunchButton()
{
    //m_act->deleteLater();
}


QHash<QString,QString> QuickLaunchButton::settingsMap()
{
    Q_ASSERT(mAct);
    return mAct->settingsMap();
}


void QuickLaunchButton::this_customContextMenuRequested(const QPoint & pos)
{
   UKUIQuickLaunch *panel = qobject_cast<UKUIQuickLaunch*>(parent());

    mMoveLeftAct->setEnabled( panel && panel->indexOfButton(this) > 0);
    mMoveRightAct->setEnabled(panel && panel->indexOfButton(this) < panel->countOfButtons() - 1);
    mPlugin->willShowWindow(mMenu);
    mMenu->popup(mPlugin->panel()->calculatePopupWindowPos(mapToGlobal({0, 0}), mMenu->sizeHint()).topLeft());
}


void QuickLaunchButton::selfRemove()
{
    emit buttonDeleted();
}

void QuickLaunchButton::enterEvent(QEvent *)
{
    quicklanuchstatus =HOVER;
    repaint();
}

void QuickLaunchButton::leaveEvent(QEvent *)
{
    quicklanuchstatus=NORMAL;
    repaint();
}

void QuickLaunchButton::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton && e->modifiers() == Qt::ControlModifier)
    {
        mDragStart = e->pos();
        return;
    }

    QToolButton::mousePressEvent(e);
}


void QuickLaunchButton::mouseMoveEvent(QMouseEvent *e)
{
    if (!(e->buttons() & Qt::LeftButton))
    {
        return;
    }

    if ((e->pos() - mDragStart).manhattanLength() < QApplication::startDragDistance())
    {
        return;
    }

    if (e->modifiers() != Qt::ControlModifier)
    {
        return;
    }

    QDrag *drag = new QDrag(this);
    ButtonMimeData *mimeData = new ButtonMimeData();
    mimeData->setButton(this);
    drag->setMimeData(mimeData);

    drag->exec(Qt::MoveAction);

    // Icon was droped outside the panel, remove button
    if (!drag->target())
    {
        selfRemove();
    }
}


void QuickLaunchButton::dragMoveEvent(QDragMoveEvent * e)
{
    if (e->mimeData()->hasFormat(MIMETYPE))
        e->acceptProposedAction();
    else
        e->ignore();
}


void QuickLaunchButton::dragEnterEvent(QDragEnterEvent *e)
{
    const ButtonMimeData *mimeData = qobject_cast<const ButtonMimeData*>(e->mimeData());
    if (mimeData && mimeData->button())
    {
        emit switchButtons(mimeData->button(), this);
    }
}

void QuickLaunchButton::contextMenuEvent(QContextMenuEvent *){
}
QuicklaunchMenu::QuicklaunchMenu(){
}

QuicklaunchMenu::~QuicklaunchMenu(){
}

void QuicklaunchMenu::contextMenuEvent(QContextMenuEvent *)
{
}
