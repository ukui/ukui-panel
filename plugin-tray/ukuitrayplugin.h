/*
 * Copyright (C) 2019 Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU  Lesser General Public License as published by
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

#ifndef UKUITRAYPLUGIN_H
#define UKUITRAYPLUGIN_H

#include "../panel/iukuipanelplugin.h"
#include <QDebug>
#include <QObject>
#include <QtX11Extras/QX11Info>

class UKUITray;
class UKUITrayPlugin : public QObject, public IUKUIPanelPlugin
{
    Q_OBJECT
public:
    explicit UKUITrayPlugin(const IUKUIPanelPluginStartupInfo &startupInfo);
    ~UKUITrayPlugin();

    virtual QWidget *widget();
    virtual QString themeId() const { return "Tray"; }
    virtual Flags flags() const { return  PreferRightAlignment | SingleInstance | NeedsHandle; }
    void realign();

    bool isSeparate() const { return true; }

private:
    UKUITray *mWidget;

};

class UKUITrayPluginLibrary: public QObject, public IUKUIPanelPluginLibrary
{
    Q_OBJECT
    // Q_PLUGIN_METADATA(IID "ukui.org/Panel/PluginInterface/3.0")
    Q_INTERFACES(IUKUIPanelPluginLibrary)
public:
    IUKUIPanelPlugin *instance(const IUKUIPanelPluginStartupInfo &startupInfo) const
    {
        // Currently only X11 supported
        if (!QX11Info::connection()) {
            qWarning() << "Currently tray plugin supports X11 only. Skipping.";
            return nullptr;
        }
        return new UKUITrayPlugin(startupInfo);
    }
};

#endif // UKUITRAYPLUGIN_H
