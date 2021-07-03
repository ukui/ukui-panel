/*
 * Copyright (C) 2019 Tianjin KYLIN Information Technology Co., Ltd.
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



#ifndef STARTMENU_H
#define STARTMENU_H

#include <QApplication>
#include <QDesktopWidget>
#include <QFrame>
#include <QFontMetrics>
#include <QLineEdit>
#include <QToolButton>

#include <QMainWindow>
#include <QHBoxLayout>
#include <QDebug>
#include <QProcess>
#include <QStyleOption>
#include <QPainter>
#include <QGSettings>

#include "../panel/iukuipanelplugin.h"
#include "../panel/ukuipanel.h"
#include "../panel/ukuicontrolstyle.h"


class TaskViewButton:public UkuiToolButton
{
    Q_OBJECT
public:
    TaskViewButton();
    ~TaskViewButton();
protected:
    void mousePressEvent(QMouseEvent* event);

};

class TaskView: public QObject, public IUKUIPanelPlugin
{
    Q_OBJECT
public:
    TaskView(const IUKUIPanelPluginStartupInfo &startupInfo);
    ~TaskView();

    virtual QWidget *widget() { return mButton; }
    virtual QString themeId() const { return QStringLiteral("taskview"); }
    void realign();

private:
    TaskViewButton *mButton;
    QGSettings *gsettings;
    QTranslator *m_translator;
private:
    void translator();
};

class TaskViewLibrary: public QObject, public IUKUIPanelPluginLibrary
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "ukui.org/Panel/PluginInterface/3.0")
    Q_INTERFACES(IUKUIPanelPluginLibrary)
public:
    IUKUIPanelPlugin *instance(const IUKUIPanelPluginStartupInfo &startupInfo) const
    {
        return new TaskView(startupInfo);
    }
};

#endif
