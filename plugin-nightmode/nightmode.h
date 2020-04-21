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
#include <QGSettings>

#include "../panel/plugin.h"
#include "../panel/ukuipanel.h"
#include "../panel/ukuicontrolstyle.h"

class NightModeButton:public QToolButton
{
    Q_OBJECT
public:
    NightModeButton(IUKUIPanelPlugin *plugin, QWidget* parent = 0);
    ~NightModeButton();

protected:
    void mousePressEvent(QMouseEvent* event);
    void contextMenuEvent(QContextMenuEvent *event);

private:
    void setNightMode(const bool nightMode);
    void setUkuiStyle(QString );
    IUKUIPanelPlugin * mPlugin;
    QMenu *nightModeMenu;
    QGSettings *gsettings;
    QGSettings *mqtstyleGsettings;
    QGSettings *mgtkstyleGsettings;
    QSettings *mqsettings;
    bool mode;

private slots:
    void turnNightMode();
    void setUpNightMode();
};

class NightMode : public QObject, public IUKUIPanelPlugin
{
    Q_OBJECT
public:
    NightMode(const IUKUIPanelPluginStartupInfo &startupInfo);
    ~NightMode();

    virtual QWidget *widget() { return mButton; }
    virtual QString themeId() const { return QStringLiteral("startmenu"); }
    void realign();
    virtual IUKUIPanelPlugin::Flags flags() const { return PreferRightAlignment | HaveConfigDialog ; }

private:
    NightModeButton *mButton;
};

class NightModeLibrary: public QObject, public IUKUIPanelPluginLibrary
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "lxqt.org/Panel/PluginInterface/3.0")
    Q_INTERFACES(IUKUIPanelPluginLibrary)
public:
    IUKUIPanelPlugin *instance(const IUKUIPanelPluginStartupInfo &startupInfo) const
    {
        return new NightMode(startupInfo);
    }
};

#endif
