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
//#include "../panel/plugin.h"
#include "../panel/ukuipanel.h"
//#include "../panel/config/configpanelwidget.h"
//#include <../panel/popupmenu.h>
#include <QMenu>
#include <QPointF>
#include "../panel/highlight-effect.h"
#include "searchwindow.h"
#include <QMainWindow>
class UKUIAssistantButton;
class UKUIAssistantPlugin: public QObject, public IUKUIPanelPlugin
{
    Q_OBJECT
public:
    explicit UKUIAssistantPlugin(const IUKUIPanelPluginStartupInfo &startupInfo);
    ~UKUIAssistantPlugin();

    SearchWindow *win;

    virtual QWidget *widget();
    virtual QString themeId() const { return "Assistant"; }
    virtual Flags flags() const { return NeedsHandle; }

    void realign();

    bool isSeparate() const { return true; }


private:
    UKUIAssistantButton *mWidget;

};


class ASSISTANTLibrary: public QObject, public IUKUIPanelPluginLibrary
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "ukui.org/Panel/PluginInterface/3.0")
    Q_INTERFACES(IUKUIPanelPluginLibrary)
public:
    IUKUIPanelPlugin *mPlugin;
    IUKUIPanelPlugin *instance(const IUKUIPanelPluginStartupInfo &startupInfo) const
    {
        return new UKUIAssistantPlugin(startupInfo);
    }
};

class UKUIAssistantButton:public QToolButton
{
    Q_OBJECT
public:
    UKUIAssistantButton(IUKUIPanelPlugin *plugin, QWidget* parent = 0);
    ~UKUIAssistantButton();
    SearchWindow *win;
    void realign();
protected:
    void contextMenuEvent(QContextMenuEvent *event);
    void mousePressEvent(QMouseEvent* event);

private:
    QMenu *rightPressMenu;
    IUKUIPanelPlugin * mPlugin;
    QPushButton *pb1;
    QPushButton *pb2;
    QPushButton *pb3;
    QLineEdit *le;

};
#endif
