/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Marat "Morion" Talipov <morion.self@gmail.com>
 *
 * Copyright: 2019-2020 UKUI team
 * modified by  hepuyao <hepuyao@kylinos.cn>
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



//this class to show the page of config panel

#include "configpaneldialog.h"
#include <QDebug>

ConfigPanelDialog::ConfigPanelDialog(UKUIPanel *panel, QWidget *parent):
    UKUi::ConfigDialog(tr("Configure Panel"), panel->settings(), parent),
    mPanelPage(nullptr),
    mPluginsPage(nullptr)
{
    setAttribute(Qt::WA_DeleteOnClose);

    mPanelPage = new ConfigPanelWidget(panel, this);
    addPage(mPanelPage, tr("Panel"), QLatin1String("configure"));
    //connect(this, &ConfigPanelDialog::reset, mPanelPage, &ConfigPanelWidget::reset);

    mPluginsPage = new ConfigPluginsWidget(panel, this);
    addPage(mPluginsPage, tr("Widgets"), QLatin1String("preferences-plugin"));
    //connect(this, &ConfigPanelDialog::reset, mPluginsPage, &ConfigPluginsWidget::reset);

//    connect(this, &ConfigPanelDialog::accepted, [panel] {
//        panel->saveSettings();

//   });

    FileSetPanel *f;
    f=new FileSetPanel(mPanelPage);
    QDBusConnection conp=QDBusConnection::sessionBus();
    if(!conp.registerService("com.ukui.panel.settings") ||
            !conp.registerObject("/set",mPanelPage))
    {
        qDebug()<<"fail";
    }

}

void ConfigPanelDialog::showConfigPanelPage()
{
    showPage(mPanelPage);

}

void ConfigPanelDialog::showConfigPluginsPage()
{
    showPage(mPluginsPage);
}

void ConfigPanelDialog::updateIconThemeSettings()
{
    mPanelPage->updateIconThemeSettings();
}

void ConfigPanelDialog::configPosition_top()
{
    //qDebug()<<"mPanelPage ptr is:"<<mPanelPage;
    mPanelPage->positionChanged_top();

}

void ConfigPanelDialog::configPosition_bottom()
{
    //qDebug()<<"mPanelPage ptr is:"<<mPanelPage;
    mPanelPage->positionChanged_bottom();
}

void ConfigPanelDialog::configPosition_left()
{
    mPanelPage->positionChanged_left();
}

void ConfigPanelDialog::configPosition_right()
{
    mPanelPage->positionChanged_right();
}

void ConfigPanelDialog::configPanelSize_S()
{
    mPanelPage->changeToSmallSize();
}

void ConfigPanelDialog::configPanelSize_m()
{
    mPanelPage->changeToMidSize();
}

void ConfigPanelDialog::configPanelSize_l()
{
    mPanelPage->changeToLargeSize();
}
void ConfigPanelDialog::backgroundChange()
{
    mPanelPage->changeBackground();
}


