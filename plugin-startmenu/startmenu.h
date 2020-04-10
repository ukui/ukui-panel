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
#include <XdgIcon>
#include <QMainWindow>
#include <QHBoxLayout>
#include <QDebug>
#include <QProcess>
#include "../panel/plugin.h"
#include "../panel/ukuipanel.h"
#include "../panel/config/configpanelwidget.h"
#include <../panel/popupmenu.h>
#include "../panel/iukuipanelplugin.h"
#include <QMenu>
#include <QPointF>

class UKUIStartMenuButton;
class UKUIStartMenuPlugin: public QObject, public IUKUIPanelPlugin
{
    Q_OBJECT
public:
    explicit UKUIStartMenuPlugin(const IUKUIPanelPluginStartupInfo &startupInfo);
    ~UKUIStartMenuPlugin();

    virtual QWidget *widget();
    virtual QString themeId() const { return "StartMenu"; }
    virtual Flags flags() const { return NeedsHandle; }

    void realign();

    bool isSeparate() const { return true; }

private:
    UKUIStartMenuButton *mWidget;
};


class StartMenuLibrary: public QObject, public IUKUIPanelPluginLibrary
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "ukui.org/Panel/PluginInterface/3.0")
    Q_INTERFACES(IUKUIPanelPluginLibrary)
public:
    IUKUIPanelPlugin *mPlugin;
    IUKUIPanelPlugin *instance(const IUKUIPanelPluginStartupInfo &startupInfo) const
    {
        return new UKUIStartMenuPlugin(startupInfo);
    }
};

class UKUIStartMenuButton:public QToolButton
{
    Q_OBJECT
public:
    UKUIStartMenuButton(IUKUIPanelPlugin *plugin, QWidget* parent = 0);
    ~UKUIStartMenuButton();
    void realign();
protected:
    void contextMenuEvent(QContextMenuEvent *event);
    void mousePressEvent(QMouseEvent* event);

private:
    QMenu *rightPressMenu;
    IUKUIPanelPlugin * mPlugin;

private slots:
    void ScreenServer();
    void SessionSwitch();
    void SessionLogout();
    void SessionReboot();
    void SessionShutdown();
};
#endif
