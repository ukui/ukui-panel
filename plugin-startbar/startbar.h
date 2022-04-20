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

#include "../panel/iukuipanelplugin.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QFrame>
#include <QFontMetrics>
#include <QLineEdit>
#include <QToolButton>
#include <QPushButton>
#include <QMainWindow>
#include <QHBoxLayout>
#include <QDebug>
#include <QProcess>
#include "../panel/ukuipanel.h"
#include <QMenu>
#include <QPointF>
#include <QMainWindow>

#include "startmenu_button.h"
#include "taskview_button.h"
#include "../panel/common/ukuigridlayout.h"
class UKUIStartBarWidget;
class UKUIStartbarPlugin: public QObject, public IUKUIPanelPlugin
{
    Q_OBJECT
public:
    explicit UKUIStartbarPlugin(const IUKUIPanelPluginStartupInfo &startupInfo);
    ~UKUIStartbarPlugin();

    virtual QWidget *widget();
    virtual QString themeId() const { return "startbar"; }
    virtual Flags flags() const { return NeedsHandle; }

    void realign();

    bool isSeparate() const { return true; }


private:
    UKUIStartBarWidget *m_widget;

};


class StartBarLibrary: public QObject, public IUKUIPanelPluginLibrary
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "ukui.org/Panel/PluginInterface/3.0")
    Q_INTERFACES(IUKUIPanelPluginLibrary)
public:
    IUKUIPanelPlugin *m_plugin;
    IUKUIPanelPlugin *instance(const IUKUIPanelPluginStartupInfo &startupInfo) const
    {
        return new UKUIStartbarPlugin(startupInfo);
    }
};

class UKUIStartBarWidget:public QFrame
{
    Q_OBJECT
public:
    UKUIStartBarWidget(IUKUIPanelPlugin *plugin, QWidget* parent = 0);
    ~UKUIStartBarWidget();
    void realign();
protected:


private:
    IUKUIPanelPlugin *m_plugin;
    StartMenuButton *m_startMenuButton;
    TaskViewButton *m_taskViewButton;
    UKUi::GridLayout *m_layout;
    QTranslator *m_translator;
    QGSettings *m_gsettings;
private:
    void translator();

};
#endif
