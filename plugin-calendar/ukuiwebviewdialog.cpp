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
    mWebView(NULL),
    ui(new Ui::UkuiWebviewDialog)
{
    ui->setupUi(this);
    installEventFilter(this);
}

UkuiWebviewDialog::~UkuiWebviewDialog()
{
    delete ui;
    if(mWebView != NULL)
    {
        mWebView->deleteLater();
        mWebView = NULL;
    }
}

void UkuiWebviewDialog::creatwebview(int _mode, int _panelSize)
{
    int iViewWidth = CALENDAR_MAX_WIDTH;
    int iViewHeight = CALENDAR_MAX_HEIGHT;
    int iScreenHeight = QApplication::screens().at(0)->size().height() - _panelSize;
    if(!mWebView)
    {
         mWebView = new QWebView(this);
    }
    else
    {
        //deletePopup();
    }
    if(mWebView != NULL)
    {
        QString  htmlFilePath = QLatin1String(PACKAGE_DATA_DIR);
        if (QLocale::system().name() == "zh_CN")
        {
            if(_mode == lunarMonday)
            {
                //first day a week is monday in lunar mode
                if(CALENDAR_MAX_HEIGHT < iScreenHeight)
                {
                    htmlFilePath = QLatin1String("file://") + htmlFilePath + QLatin1String("/plugin-calendar/html/ukui-mon.html");
                }
                else
                {
                    iViewHeight = CALENDAR_MIN_HEIGHT;
                    htmlFilePath = QLatin1String("file://") + htmlFilePath + QLatin1String("/plugin-calendar/html/ukui-mon-min.html");
                }
            }
            else if(_mode == solarSunday)
            {
                //first day a week is sunday in solar mode
                iViewHeight = 600;
                htmlFilePath = QLatin1String("file://") + htmlFilePath + QLatin1String("/plugin-calendar/html/ukui-solar-cn.html");
            }
            else if(_mode == solarMonday)
            {
                //first day a week is monday in solar mode
                iViewHeight = 600;
                htmlFilePath = QLatin1String("file://") + htmlFilePath + QLatin1String("/plugin-calendar/html/ukui-solar-cn-mon.html");
            }
            else
            {
                //first day a week is sunday in lunar mode
                if(CALENDAR_MAX_HEIGHT < iScreenHeight)
                {
                    htmlFilePath = QLatin1String("file://") + htmlFilePath + QLatin1String("/plugin-calendar/html/ukui.html");
                }
                else
                {
                    iViewHeight = 600;
                    htmlFilePath = QLatin1String("file://") + htmlFilePath + QLatin1String("/plugin-calendar/html/ukui-min.html");
                }
            }
        }
        else
        {
            if(_mode == solarSunday)
            {
                htmlFilePath = QLatin1String("file://") + htmlFilePath + QLatin1String("/plugin-calendar/html/ukui-solar-en.html");
            }
            else
            {
                htmlFilePath = QLatin1String("file://") + htmlFilePath + QLatin1String("/plugin-calendar/html/ukui-solar-en-mon.html");
            }
            iViewHeight = 600;
        }
        /*set window no margins*/
        mWebView->setWindowFlags(Qt::FramelessWindowHint);

        /*set rounded corner ,including radius*/
        QBitmap bmp(iViewWidth,iViewHeight);
        bmp.fill();
        QPainter p(&bmp);
        p.setPen(Qt::NoPen);
        p.setBrush(Qt::black);
        p.drawRoundedRect(bmp.rect(),6,6);
        setMask(bmp);

        /*set window size*/
        mWebView->resize(iViewWidth,iViewHeight);
        mWebView->settings()->setAttribute(QWebSettings::JavascriptEnabled,true);
        mWebView->settings()->setAttribute(QWebSettings::WebAttribute::LocalStorageEnabled, true);
        mWebView->setContextMenuPolicy(Qt::NoContextMenu);
        mWebView->load(QUrl(htmlFilePath));
    }
}

#if 0
//event 与 nativeEvent  是用来处理界面的隐藏与显示相关的问题
bool UkuiWebviewDialog::event(QEvent *event)
{
    if (event->type() == QEvent::Close)
    {
        Q_EMIT deactivated();
    }

    return QDialog::event(event);
}

bool UkuiWebviewDialog::nativeEvent(const QByteArray &eventType, void *message, long *result)

{
    Q_UNUSED(result);
    if (eventType != "xcb_generic_event_t") {
        return false;
    }
    xcb_generic_event_t *event = (xcb_generic_event_t*)message;
    switch (event->response_type & ~0x80) {
    qDebug()<<"YYF - event->response_type : "<<event->response_type;//YYF 20200922
    case XCB_FOCUS_OUT:
        this->hide();
        break;
    }
    return false;
}
#endif
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
