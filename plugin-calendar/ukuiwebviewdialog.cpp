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

#define CALENDAR_MAX_HEIGHT 704
#define CALENDAR_MIN_HEIGHT 600

#define CALENDAR_MAX_WIDTH 454

UkuiWebviewDialog::UkuiWebviewDialog(QWidget *parent) :
    QDialog(parent, Qt::Popup),
    mWebView(NULL),
    ui(new Ui::UkuiWebviewDialog)
{
    ui->setupUi(this);
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

void UkuiWebviewDialog::creatwebview(int _mode)
{
    int iViewWidth = CALENDAR_MAX_WIDTH;
    int iViewHeight = CALENDAR_MAX_HEIGHT;
    int iScreenHeight = QApplication::screens().at(0)->size().height();
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
        /*set window size*/
        mWebView->resize(iViewWidth,iViewHeight);
        mWebView->settings()->setAttribute(QWebSettings::JavascriptEnabled,true);
        mWebView->settings()->setAttribute(QWebSettings::WebAttribute::LocalStorageEnabled, true);
        mWebView->setContextMenuPolicy(Qt::NoContextMenu);
        mWebView->load(QUrl(htmlFilePath));
    }
}

bool UkuiWebviewDialog::event(QEvent *event)
{
    if (event->type() == QEvent::Close)
    {
        Q_EMIT deactivated();
    }

    return QDialog::event(event);
}
