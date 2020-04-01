/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Copyright: 2011 Razor team
 *            2014 LXQt team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.com>
 *   Maciej Płaza <plaza.maciej@gmail.com>
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

#include "ukuigrouppopup.h"
#include <QEnterEvent>
#include <QDrag>
#include <QMimeData>
#include <QLayout>
#include <QPainter>
#include <QStyleOption>
#include <QDebug>
#include <QApplication>
#include <QScreen>

/************************************************
    this class is just a container of window buttons
    the main purpose is showing window buttons in
    vertical layout and drag&drop feature inside
    group
 ************************************************/
UKUIGroupPopup::UKUIGroupPopup(UKUITaskGroup *group):
    QFrame(group),
    mGroup(group)
{
    Q_ASSERT(group);
    setAcceptDrops(true);
    setWindowFlags(Qt::FramelessWindowHint | Qt::ToolTip);
    setAttribute(Qt::WA_AlwaysShowToolTips);
    setAttribute(Qt::WA_TranslucentBackground);

    setLayout(new QHBoxLayout);
    layout()->setSpacing(3);
    layout()->setMargin(3);

    connect(&mCloseTimer, &QTimer::timeout, this, &UKUIGroupPopup::closeTimerSlot);
    mCloseTimer.setSingleShot(true);
    mCloseTimer.setInterval(400);
    setMaximumWidth(QApplication::screens().at(0)->size().width());
    setMaximumHeight(QApplication::screens().at(0)->size().height());
}

UKUIGroupPopup::~UKUIGroupPopup()
{
}

void UKUIGroupPopup::dropEvent(QDropEvent *event)
{
    qlonglong temp;
    QDataStream stream(event->mimeData()->data(UKUITaskButton::mimeDataFormat()));
    stream >> temp;
    WId window = (WId) temp;

    UKUITaskButton *button = nullptr;
    int oldIndex(0);
    // get current position of the button being dragged
    for (int i = 0; i < layout()->count(); i++)
    {
        UKUITaskButton *b = qobject_cast<UKUITaskButton*>(layout()->itemAt(i)->widget());
        if (b && b->windowId() == window)
        {
            button = b;
            oldIndex = i;
            break;
        }
    }

    if (button == nullptr)
        return;

    int newIndex = -1;
    // find the new position to place it in
    for (int i = 0; i < oldIndex && newIndex == -1; i++)
    {
        QWidget *w = layout()->itemAt(i)->widget();
        if (w && w->pos().y() + w->height() / 2 > event->pos().y())
            newIndex = i;
    }
    const int size = layout()->count();
    for (int i = size - 1; i > oldIndex && newIndex == -1; i--)
    {
        QWidget *w = layout()->itemAt(i)->widget();
        if (w && w->pos().y() + w->height() / 2 < event->pos().y())
            newIndex = i;
    }

    if (newIndex == -1 || newIndex == oldIndex)
        return;

    QVBoxLayout * l = qobject_cast<QVBoxLayout *>(layout());
    l->takeAt(oldIndex);
    l->insertWidget(newIndex, button);
    l->invalidate();

}

void UKUIGroupPopup::dragEnterEvent(QDragEnterEvent *event)
{
    event->accept();
    QWidget::dragEnterEvent(event);
}

void UKUIGroupPopup::dragLeaveEvent(QDragLeaveEvent *event)
{
    hide(false/*not fast*/);
    QFrame::dragLeaveEvent(event);
}

/************************************************
 *
 ************************************************/
void UKUIGroupPopup::leaveEvent(QEvent *event)
{
//    qDebug()<<"UKUIGroupPopup::leaveEvent:"<<mCloseTimer.isActive();
//    mCloseTimer.start();
//    isSetByLeaveEvent = true;
    QTimer::singleShot(300, this,SLOT(closeWindowDelay()));
}

/************************************************
 *
 ************************************************/
void UKUIGroupPopup::enterEvent(QEvent *event)
{
    QTimer::singleShot(300, this,SLOT(killTimerDelay()));
//    mCloseTimer.stop();
}

void UKUIGroupPopup::killTimerDelay()
{
      mCloseTimer.stop();
}

void UKUIGroupPopup::closeWindowDelay()
{
    if(mCloseTimer.isActive())
    {
        mCloseTimer.stop();
    }
    close();
}

void UKUIGroupPopup::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    QStyleOption opt;
    opt.initFrom(this);
    p.setBrush(QBrush(QColor(0xff,0x14,0x14,0xb2)));
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void UKUIGroupPopup::hide(bool fast)
{
    if (fast)
        close();
    else
        mCloseTimer.start();
}

void UKUIGroupPopup::show()
{
    mCloseTimer.stop();
    QFrame::show();
}

void UKUIGroupPopup::closeTimerSlot()
{
    bool button_has_dnd_hover = false;
    QLayout* l = layout();
    for (int i = 0; l->count() > i; ++i)
    {
        UKUITaskWidget const * const button = dynamic_cast<UKUITaskWidget const *>(l->itemAt(i)->widget());
        if (0 != button && button->hasDragAndDropHover())
        {
            button_has_dnd_hover = true;
            break;
        }
    }
    if (!button_has_dnd_hover)
        close();
}
