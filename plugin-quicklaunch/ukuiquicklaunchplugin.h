/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Copyright: 2013 Razor team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.com>
 *
 * Copyright: 2019 Tianjin KYLIN Information Technology Co., Ltd. *
 *
 * This program or library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */

#ifndef UKUIQUICKLAUNCHPLUGIN_H
#define UKUIQUICKLAUNCHPLUGIN_H

#include "../panel/iukuipanelplugin.h"
#include <QObject>


class UKUIQuickLaunch;

class UKUIQuickLaunchPlugin: public QObject, public IUKUIPanelPlugin
{
    Q_OBJECT
public:
    explicit UKUIQuickLaunchPlugin(const IUKUIPanelPluginStartupInfo &startupInfo);
    ~UKUIQuickLaunchPlugin();

    virtual QWidget *widget();
    virtual QString themeId() const { return "QuickLaunch"; }
    virtual Flags flags() const { return NeedsHandle; }

    void realign();

    bool isSeparate() const { return true; }

private:
    UKUIQuickLaunch *mWidget;
};


class UKUIQuickLaunchPluginLibrary: public QObject, public IUKUIPanelPluginLibrary
{
    Q_OBJECT
    // Q_PLUGIN_METADATA(IID "ukui.org/Panel/PluginInterface/3.0")
    Q_INTERFACES(IUKUIPanelPluginLibrary)
public:
    IUKUIPanelPlugin *instance(const IUKUIPanelPluginStartupInfo &startupInfo) const
    {
        return new UKUIQuickLaunchPlugin(startupInfo);
    }
};
#endif // UKUIQUICKLAUNCHPLUGIN_H
