/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Copyright: 2015 LXQt team
 * Authors:
 *   Paulo Lieuthier <paulolieuthier@gmail.com>
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
 *
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */

#include "configpluginswidget.h"
#include "ui_configpluginswidget.h"
#include "addplugindialog.h"
#include "panelpluginsmodel.h"
#include "../plugin.h"
#include "../iukuipanelplugin.h"

#include "../common/ukuihtmldelegate.h"
#include <QPushButton>
#include <QItemSelectionModel>

ConfigPluginsWidget::ConfigPluginsWidget(UKUIPanel *panel, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::ConfigPluginsWidget),
    mPanel(panel)
{
    ui->setupUi(this);

    PanelPluginsModel * plugins = mPanel->mPlugins.data();
    {
        QScopedPointer<QItemSelectionModel> m(ui->listView_plugins->selectionModel());
        ui->listView_plugins->setModel(plugins);
    }
    {
        QScopedPointer<QAbstractItemDelegate> d(ui->listView_plugins->itemDelegate());
        ui->listView_plugins->setItemDelegate(new UKUi::HtmlDelegate(QSize(16, 16), ui->listView_plugins));
    }

    resetButtons();

    connect(ui->listView_plugins->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &ConfigPluginsWidget::resetButtons);

    connect(ui->pushButton_moveUp,      &QToolButton::clicked, [this, plugins] { plugins->onMovePluginUp(ui->listView_plugins->currentIndex()); });
    connect(ui->pushButton_moveDown,    &QToolButton::clicked, [this, plugins] { plugins->onMovePluginDown(ui->listView_plugins->currentIndex()); });

    connect(ui->pushButton_addPlugin, &QPushButton::clicked, this, &ConfigPluginsWidget::showAddPluginDialog);
   // connect(ui->pushButton_removePlugin, &QToolButton::clicked, [this, plugins] { plugins->onRemovePlugin(ui->listView_plugins->currentIndex()); });

    connect(ui->pushButton_pluginConfig, &QToolButton::clicked, [this, plugins] { plugins->onConfigurePlugin(ui->listView_plugins->currentIndex()); });

    connect(plugins, &PanelPluginsModel::pluginAdded, this, &ConfigPluginsWidget::resetButtons);
    connect(plugins, &PanelPluginsModel::pluginRemoved, this, &ConfigPluginsWidget::resetButtons);
    connect(plugins, &PanelPluginsModel::pluginMoved, this, &ConfigPluginsWidget::resetButtons);
}

ConfigPluginsWidget::~ConfigPluginsWidget()
{
    delete ui;
}

void ConfigPluginsWidget::reset()
{

    qDebug()<<"ConfigPluginsWidget  reset is here";
}

void ConfigPluginsWidget::showAddPluginDialog()
{
    if (mAddPluginDialog.isNull())
    {
        mAddPluginDialog.reset(new AddPluginDialog);
        connect(mAddPluginDialog.data(), &AddPluginDialog::pluginSelected,
                mPanel->mPlugins.data(), &PanelPluginsModel::addPlugin);
    }
    mAddPluginDialog->show();
    mAddPluginDialog->raise();
    mAddPluginDialog->activateWindow();
    qDebug()<<"showAddPluginDialog  here"<<endl;
    qDebug()<<ui->listView_plugins->currentIndex()<<endl;

}

void ConfigPluginsWidget::resetButtons()
{
    PanelPluginsModel *model = mPanel->mPlugins.data();
    QItemSelectionModel *selectionModel = ui->listView_plugins->selectionModel();
    bool hasSelection = selectionModel->hasSelection();
    bool isFirstSelected = selectionModel->isSelected(model->index(0));
    bool isLastSelected = selectionModel->isSelected(model->index(model->rowCount() - 1));

    bool hasConfigDialog = false;
    if (hasSelection)
    {
        Plugin const * plugin
            = ui->listView_plugins->model()->data(selectionModel->currentIndex(), Qt::UserRole).value<Plugin const *>();
        if (nullptr != plugin)
            hasConfigDialog = plugin->iPlugin()->flags().testFlag(IUKUIPanelPlugin::HaveConfigDialog);
    }

    ui->pushButton_removePlugin->setEnabled(hasSelection);
    ui->pushButton_moveUp->setEnabled(hasSelection && !isFirstSelected);
    ui->pushButton_moveDown->setEnabled(hasSelection && !isLastSelected);
    ui->pushButton_pluginConfig->setEnabled(hasSelection && hasConfigDialog);
}
