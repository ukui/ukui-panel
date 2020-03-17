/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Copyright: 2012 Razor team
 *            2014 LXQt team
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


#ifndef UKUITASKBARPLUGIN_H
#define UKUITASKBARPLUGIN_H

#include "../panel/iukuipanel.h"
#include "../panel/iukuipanelplugin.h"
#include "ukuitaskbar.h"
#include <QDebug>
class UKUITaskBar;

class UKUITaskBarPlugin : public QObject, public IUKUIPanelPlugin
{
    Q_OBJECT
public:
    UKUITaskBarPlugin(const IUKUIPanelPluginStartupInfo &startupInfo);
    ~UKUITaskBarPlugin();

    QString themeId() const { return "TaskBar"; }
    virtual Flags flags() const { return HaveConfigDialog | NeedsHandle; }

    QWidget *widget() { return mTaskBar; }
    QDialog *configureDialog();

    void settingsChanged() { mTaskBar->settingsChanged(); }
    void realign();

    bool isSeparate() const { return true; }
    bool isExpandable() const { return true; }
private:
    UKUITaskBar *mTaskBar;
};

class UKUITaskBarPluginLibrary: public QObject, public IUKUIPanelPluginLibrary
{
    Q_OBJECT
    // Q_PLUGIN_METADATA(IID "ukui.org/Panel/PluginInterface/3.0")
    Q_INTERFACES(IUKUIPanelPluginLibrary)
public:
    IUKUIPanelPlugin *instance(const IUKUIPanelPluginStartupInfo &startupInfo) const { return new UKUITaskBarPlugin(startupInfo);}
};

#endif // UKUITASKBARPLUGIN_H
