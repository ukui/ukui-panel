/*
 * Copyright (C) 2019 Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public as published by
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


#include "ukuiwebviewdialog.h"
#include "ui_ukuiwebviewdialog.h"
#include <QPalette>
#include <QBrush>
#include <QMessageBox>
#include <QColor>
#include <QTime>
#include <QApplication>
#include <QSize>
#include <QScreen>
#include <QBitmap>
#include <QPainter>
#include <xcb/xcb.h>

#define CALENDAR_MAX_HEIGHT 704
#define CALENDAR_MIN_HEIGHT 600

#define CALENDAR_MAX_WIDTH 454
UkuiWebviewDialogStatus status;

UkuiWebviewDialog::UkuiWebviewDialog(QWidget *parent) :
    QDialog(parent, Qt::FramelessWindowHint | Qt::Tool | Qt::X11BypassWindowManagerHint),
    ui(new Ui::UkuiWebviewDialog)
{
    ui->setupUi(this);
    installEventFilter(this);
}

UkuiWebviewDialog::~UkuiWebviewDialog()
{
    delete ui;
}

void UkuiWebviewDialog::creatwebview(int _mode, int _panelSize)
{
}

/*
 * 事件过滤，检测鼠标点击外部活动区域则收回收纳栏
*/
bool UkuiWebviewDialog::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == this)
    {
        if (event->type() == QEvent::MouseButtonPress)
           {
               QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
               if (mouseEvent->button() == Qt::LeftButton)
               {
                   this->hide();
                   status=ST_HIDE;
                   return true;
               }
               else if(mouseEvent->button() == Qt::RightButton)
               {
                   return true;
               }
           }
        else if(event->type() == QEvent::ContextMenu)
        {
            return false;
        }
        else if (event->type() == QEvent::WindowDeactivate &&status==ST_SHOW)
        {
//            qDebug()<<"激活外部窗口";
            this->hide();
            status=ST_HIDE;
            return true;
        } else if (event->type() == QEvent::StyleChange) {
        }
    }

    if (!isActiveWindow())
    {
        activateWindow();
    }
    return false;
}
