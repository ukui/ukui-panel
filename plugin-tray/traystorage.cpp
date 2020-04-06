/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Copyright: 2011 Razor team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.com>
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

/********************************************************************
  Inspired by freedesktops tint2 ;)

*********************************************************************/

#include <QApplication>
#include <QDebug>
#include <QTimer>
#include <QtX11Extras/QX11Info>
#include "trayicon.h"
#include "../panel/iukuipanel.h"
#include "../panel/common/ukuigridlayout.h"
#include "traystorage.h"
#include "xfitman.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/extensions/Xrender.h>
#include <X11/extensions/Xdamage.h>
#include <xcb/xcb.h>
#include <xcb/damage.h>

#undef Bool // defined as int in X11/Xlib.h

#include "../panel/iukuipanelplugin.h"
#include <QPushButton>
#include <QLabel>
#define _NET_SYSTEM_TRAY_ORIENTATION_HORZ 0
#define _NET_SYSTEM_TRAY_ORIENTATION_VERT 1

#define SYSTEM_TRAY_REQUEST_DOCK    0
#define SYSTEM_TRAY_BEGIN_MESSAGE   1
#define SYSTEM_TRAY_CANCEL_MESSAGE  2

#define XEMBED_EMBEDDED_NOTIFY  0
#define XEMBED_MAPPED          (1 << 0)

TrayStorageStatus storagestatus=HIDE;

/************************************************

 ************************************************/
TrayStorage::TrayStorage( QWidget *parent):
    QFrame(parent),
    mValid(false),
    mTrayId(0),
    mDamageEvent(0),
    mDamageError(0),
    mIconSize(TRAY_ICON_SIZE_DEFAULT, TRAY_ICON_SIZE_DEFAULT),
    mDisplay(QX11Info::display())
{
    mLayout = new UKUi::GridLayout(this);
    mLayout->setColumnCount(3);
    mLayout->setRowCount(3);
    setAttribute(Qt::WA_TranslucentBackground);//设置窗口背景透明
    setWindowFlags(/*Qt::FramelessWindowHint |*/ /*Qt::WindowStaysOnTopHint |*/ Qt::ToolTip);
    _NET_SYSTEM_TRAY_OPCODE = XfitMan::atom("_NET_SYSTEM_TRAY_OPCODE");
}


/************************************************
 ************************************************/
TrayStorage::~TrayStorage()
{
    stopTrayStorage();
}

void TrayStorage::stopTrayStorage()
{
    for (auto & icon : mIcons)
        disconnect(icon, &QObject::destroyed, this, &TrayStorage::onIconDestroyed);
    qDeleteAll(mIcons);
    if (mTrayId)
    {
        XDestroyWindow(mDisplay, mTrayId);
        mTrayId = 0;
    }
    mValid = false;
}
bool TrayStorage::nativeEventFilter(const QByteArray &eventType, void *message, long *)
{
    if (eventType != "xcb_generic_event_t")
        return false;

    xcb_generic_event_t* event = static_cast<xcb_generic_event_t *>(message);

    TrayIcon* icon;
    int event_type = event->response_type & ~0x80;

    return false;
}

void TrayStorage::setIconSize(QSize iconSize)
{
    mIconSize = iconSize;
    unsigned long size = qMin(mIconSize.width(), mIconSize.height());
    XChangeProperty(mDisplay,
                    mTrayId,
                    XfitMan::atom("_NET_SYSTEM_TRAY_ICON_SIZE"),
                    XA_CARDINAL,
                    32,
                    PropModeReplace,
                    (unsigned char*)&size,
                    1);
}


/************************************************

************************************************/
VisualID TrayStorage::getVisual()
{
    VisualID visualId = 0;
    Display* dsp = mDisplay;

    XVisualInfo templ;
    templ.screen=QX11Info::appScreen();
    templ.depth=32;
    templ.c_class=TrueColor;

    int nvi;
    XVisualInfo* xvi = XGetVisualInfo(dsp, VisualScreenMask|VisualDepthMask|VisualClassMask, &templ, &nvi);

    if (xvi)
    {
        int i;
        XRenderPictFormat* format;
        for (i = 0; i < nvi; i++)
        {
            format = XRenderFindVisualFormat(dsp, xvi[i].visual);
            if (format &&
                format->type == PictTypeDirect &&
                format->direct.alphaMask)
            {
                visualId = xvi[i].visualid;
                break;
            }
        }
        XFree(xvi);
    }

    return visualId;
}


void TrayStorage::onIconDestroyed(QObject * icon)
{
    //in the time QOjbect::destroyed is emitted, the child destructor
    //is already finished, so the qobject_cast to child will return nullptr in all cases
    mIcons.removeAll(static_cast<TrayIcon *>(icon));
}

/************************************************

 ************************************************/

void TrayStorage::mouseReleaseEvent(QMouseEvent *event)
{
    storagestatus = HIDE;
    this->hide();
    update();
    QWidget::mouseReleaseEvent(event);
}

void TrayStorage::mousePressEvent(QMouseEvent *event)
{
    update();
    QWidget::mousePressEvent(event);
}

void TrayStorage::leaveEvent(QEvent *)
{
//    tra= HIDE;
}

void TrayStorage::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    p.setBrush(QBrush(QColor(0x13,0x14,0x14,0xb2)));
    p.setPen(Qt::NoPen);
    p.setRenderHint(QPainter::Antialiasing);  // 反锯齿;
    p.drawRoundedRect(opt.rect,6,6);
    style()->drawPrimitive(QStyle::PE_Frame, &opt, &p, this);
}


/*
    点击窗口之外的部分隐藏
*/
bool TrayStorage:: event(QEvent *event)
{
    if (event->type() == QEvent::ActivationChange) {
        if (QApplication::activeWindow() != this) {
            hide();
            storagestatus=HIDE;
        }
    }
    return QWidget::event(event);
}
