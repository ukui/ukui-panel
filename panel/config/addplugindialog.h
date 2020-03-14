/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.com>
 *
 * Copyright: 2019-2020 LXQt team
 * Modified by  hepuyao <hepuyao@kylinos.cn>
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


#ifndef UKUI_ADDPLUGINDIALOG_H
#define UKUI_ADDPLUGINDIALOG_H

//#include <UKUi/PluginInfo>
#include "../panel/common/ukuiplugininfo.h"
#include <QDialog>
#include <QTimer>

#define SEARCH_DELAY 125

namespace Ui {
    class AddPluginDialog;
}

class AddPluginDialog : public QDialog
{
    Q_OBJECT

public:
    AddPluginDialog(QWidget *parent = 0);
    ~AddPluginDialog();

signals:
    void pluginSelected(const UKUi::PluginInfo &plugin);

private:
    Ui::AddPluginDialog *ui;
    UKUi::PluginInfoList mPlugins;
    QTimer mSearchTimer;

private slots:
    void filter();
    void emitPluginSelected();
};

#endif // UKUI_ADDPLUGINDIALOG_H
