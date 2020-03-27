/*
 * Copyright (C) 2019 Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public as published by
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


#ifndef UKUIWEBVIEWDIALOG_H
#define UKUIWEBVIEWDIALOG_H

#include <QDialog>
#include <QtWebKitWidgets/QWebView>
#include <QtWebKitWidgets/QWebFrame>
#include "../panel/iukuipanelplugin.h"
namespace Ui {
class UkuiWebviewDialog;
}

enum CalendarShowMode
{
    lunarSunday        = 0,//show lunar and first day a week is sunday
    lunarMonday        = 1,//show lunar and first day a week is monday
    solarSunday        = 2,//show solar and first day a week is sunday
    solarMonday        = 3,//show solar and first day a week is monday
    Unknown            = 0xff
};

class UkuiWebviewDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UkuiWebviewDialog(QWidget *parent = nullptr);
    ~UkuiWebviewDialog();
    void creatwebview(int _mode);
    void showinfo(QString string);
    QWebView *webview(){return mWebView;}
Q_SIGNALS:
    void deactivated();

private:
    Ui::UkuiWebviewDialog *ui;
    QWebView   *mWebView;
    QSize      mQsize;
protected:
    virtual bool event(QEvent *event);
};

#endif // UKUIWEBVIEWDIALOG_H
