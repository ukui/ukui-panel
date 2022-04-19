/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Copyright: 2010-2011 Razor team
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

#include "panelpluginsmodel.h"
#include "plugin.h"
#include "iukuipanelplugin.h"
#include "ukuipanel.h"
#include "ukuipanelapplication.h"
#include <QPointer>
#include <XdgIcon>
#include "common/ukuisettings.h"
#include <QDebug>

#define CONFIG_FILE_BACKUP     "/usr/share/ukui/panel.conf"

PanelPluginsModel::PanelPluginsModel(UKUIPanel * panel,
                                     QString const & namesKey,
                                     QStringList const & desktopDirs,
                                     QObject * parent/* = nullptr*/)
    : QAbstractListModel{parent},
    m_namesKey(namesKey),
    m_panel(panel)
{
    loadPlugins(desktopDirs);
}

PanelPluginsModel::~PanelPluginsModel()
{
    qDeleteAll(plugins());
}

int PanelPluginsModel::rowCount(const QModelIndex & parent/* = QModelIndex()*/) const
{
    return QModelIndex() == parent ? m_plugins.size() : 0;
}


QVariant PanelPluginsModel::data(const QModelIndex & index, int role/* = Qt::DisplayRole*/) const
{
    Q_ASSERT(QModelIndex() == index.parent()
            && 0 == index.column()
            && m_plugins.size() > index.row()
            );

    pluginslist_t::const_reference plugin = m_plugins[index.row()];
    QVariant ret;
    switch (role)
    {
        case Qt::DisplayRole:
            if (plugin.second.isNull())
                ret = QString("<b>Unknown</b> (%1)").arg(plugin.first);
            else
                ret = QString("<b>%1</b> (%2)").arg(plugin.second->name(), plugin.first);
            break;
        case Qt::DecorationRole:
            if (plugin.second.isNull())
                ret = XdgIcon::fromTheme("preferences-plugin");
            else
                ret = plugin.second->desktopFile().icon(XdgIcon::fromTheme("preferences-plugin"));
            break;
        case Qt::UserRole:
            ret = QVariant::fromValue(const_cast<Plugin const *>(plugin.second.data()));
            break;
    }
    return ret;
}

Qt::ItemFlags PanelPluginsModel::flags(const QModelIndex & index) const
{
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemNeverHasChildren;
}

QStringList PanelPluginsModel::pluginNames() const
{
    QStringList names;
    for (auto const & p : m_plugins)
        names.append(p.first);
    return names;
}

QList<Plugin *> PanelPluginsModel::plugins() const
{
    QList<Plugin *> plugins;
    for (auto const & p : m_plugins)
        if (!p.second.isNull())
            plugins.append(p.second.data());
    return plugins;
}

Plugin* PanelPluginsModel::pluginByName(QString name) const
{
    for (auto const & p : m_plugins)
        if (p.first == name)
            return p.second.data();
    return nullptr;
}

Plugin const * PanelPluginsModel::pluginByID(QString id) const
{
    for (auto const & p : m_plugins)
    {
        Plugin *plugin = p.second.data();
        if (plugin && plugin->desktopFile().id() == id)
            return plugin;
    }
    return nullptr;
}

void PanelPluginsModel::addPlugin(const UKUi::PluginInfo &desktopFile)
{
    if (dynamic_cast<UKUIPanelApplication const *>(qApp)->isPluginSingletonAndRunnig(desktopFile.id()))
        return;
    QString name = findNewPluginSettingsGroup(desktopFile.id());
    QPointer<Plugin> plugin = loadPlugin(desktopFile, name);
    if (plugin.isNull())
        return;
    qDebug()<< plugin->name()<<endl;
    beginInsertRows(QModelIndex(), m_plugins.size(), m_plugins.size());
    m_plugins.append({name, plugin});
    endInsertRows();
    m_panel->settings()->setValue(m_namesKey, pluginNames());
    emit pluginAdded(plugin.data());
}

void PanelPluginsModel::removePlugin(pluginslist_t::iterator plugin)
{
    if (m_plugins.end() != plugin)
    {
        m_panel->settings()->remove(plugin->first);
        Plugin * p = plugin->second.data();
        const int row = plugin - m_plugins.begin();
        beginRemoveRows(QModelIndex(), row, row);
        m_plugins.erase(plugin);
        endRemoveRows();
        emit pluginRemoved(p); // p can be nullptr
        m_panel->settings()->setValue(m_namesKey, pluginNames());
        if (nullptr != p)
            p->deleteLater();
    }
}

void PanelPluginsModel::removePlugin()
{
    Plugin * p = qobject_cast<Plugin*>(sender());
    auto plugin = std::find_if(m_plugins.begin(), m_plugins.end(),
                               [p] (pluginslist_t::const_reference obj) { return p == obj.second; });
    removePlugin(std::move(plugin));
}

void PanelPluginsModel::movePlugin(Plugin * plugin, QString const & nameAfter)
{
    //merge list of plugins (try to preserve original position)
    //subtract mPlugin.begin() from the found Plugins to get the model index
    const int from =
        std::find_if(m_plugins.begin(), m_plugins.end(), [plugin] (pluginslist_t::const_reference obj) { return plugin == obj.second.data(); })
        - m_plugins.begin();
    const int to =
        std::find_if(m_plugins.begin(), m_plugins.end(), [nameAfter] (pluginslist_t::const_reference obj) { return nameAfter == obj.first; })
        - m_plugins.begin();
    /* 'from' is the current position of the Plugin to be moved ("moved Plugin"),
     * 'to' is the position of the Plugin behind the one that is being moved
     * ("behind Plugin"). There are several cases to distinguish:
     * 1. from > to: The moved Plugin had been behind the behind Plugin before
     * and is moved to the front of the behind Plugin. The moved Plugin will
     * be inserted at position 'to', the behind Plugin and all the following
     * Plugins (until the former position of the moved Plugin) will increment
     * their indexes.
     * 2. from < to: The moved Plugin had already been located before the
     * behind Plugin. In this case, the move operation only reorders the
     * Plugins before the behind Plugin. All the Plugins between the moved
     * Plugin and the behind Plugin will decrement their index. Therefore, the
     * movedPlugin will not be at position 'to' but rather on position 'to-1'.
     * 3. from == to: This does not make sense, we catch this case to prevent
     * errors.
     * 4. from == to-1: The moved Plugin has not moved because it had already
     * been located in front of the behind Plugin.
     */
    const int to_plugins = from < to ? to - 1 : to;

    if (from != to && from != to_plugins)
    {
        /* Although the new position of the moved Plugin will be 'to-1' if
         * from < to, we insert 'to' here. This is exactly how it is done
         * in the Qt documentation.
         */
        beginMoveRows(QModelIndex(), from, from, QModelIndex(), to);
        // For the QList::move method, use the right position
        m_plugins.move(from, to_plugins);
        endMoveRows();
        emit pluginMoved(plugin);
        m_panel->settings()->setValue(m_namesKey, pluginNames());
    }
}

void PanelPluginsModel::loadPlugins(QStringList const & desktopDirs)
{
    QSettings backup_qsettings(CONFIG_FILE_BACKUP,QSettings::IniFormat);

    QStringList plugin_names = backup_qsettings.value(m_namesKey).toStringList();

#ifdef DEBUG_PLUGIN_LOADTIME
    QElapsedTimer timer;
    timer.start();
    qint64 lastTime = 0;
#endif
    for (auto const & name : plugin_names)
    {
        pluginslist_t::iterator i = m_plugins.insert(m_plugins.end(), {name, nullptr});
        QString type = backup_qsettings.value(name + "/type").toString();
        if (type.isEmpty())
        {
            qWarning() << QString("Section \"%1\" not found in %2.").arg(name, backup_qsettings.fileName());
            continue;
        }
#ifdef WITH_SCREENSAVER_FALLBACK
        if (QStringLiteral("screensaver") == type)
        {
            //plugin-screensaver was dropped
            //convert settings to plugin-quicklaunch
            const QString & lock_desktop = QStringLiteral(UKUI_LOCK_DESKTOP);
            qWarning().noquote() << "Found deprecated plugin of type 'screensaver', migrating to 'quicklaunch' with '" << lock_desktop << '\'';
            type = QStringLiteral("quicklaunch");
            UKUi::Settings * settings = mPanel->settings();
            settings->beginGroup(name);
            settings->remove(QString{});//remove all existing keys
            settings->setValue(QStringLiteral("type"), type);
            settings->beginWriteArray(QStringLiteral("apps"), 1);
            settings->setArrayIndex(0);
            settings->setValue(QStringLiteral("desktop"), lock_desktop);
            settings->endArray();
            settings->endGroup();
        }
#endif

        UKUi::PluginInfoList list = UKUi::PluginInfo::search(desktopDirs, "UKUIPanel/Plugin", QString("%1.desktop").arg(type));
        if( !list.count())
        {
            qWarning() << QString("Plugin \"%1\" not found.").arg(type);
            continue;
        }

        i->second = loadPlugin(list.first(), name);
#ifdef DEBUG_PLUGIN_LOADTIME
        qDebug() << "load plugin" << type << "takes" << (timer.elapsed() - lastTime) << "ms";
        lastTime = timer.elapsed();
#endif
    }
}

QPointer<Plugin> PanelPluginsModel::loadPlugin(UKUi::PluginInfo const & desktopFile, QString const & settingsGroup)
{
    std::unique_ptr<Plugin> plugin(new Plugin(desktopFile, m_panel->settings(), settingsGroup, m_panel));
    if (plugin->isLoaded())
    {
        connect(m_panel, &UKUIPanel::realigned, plugin.get(), &Plugin::realign);
        connect(plugin.get(), &Plugin::remove,
                this, static_cast<void (PanelPluginsModel::*)()>(&PanelPluginsModel::removePlugin));
        return plugin.release();
    }

    return nullptr;
}

QString PanelPluginsModel::findNewPluginSettingsGroup(const QString &pluginType) const
{
    QStringList groups = m_panel->settings()->childGroups();
    groups.sort();

    // Generate new section name
    QString pluginName = QString("%1").arg(pluginType);

    if (!groups.contains(pluginName))
        return pluginName;
    else
    {
        for (int i = 2; true; ++i)
        {
            pluginName = QString("%1%2").arg(pluginType).arg(i);
            if (!groups.contains(pluginName))
                return pluginName;
        }
    }
}

bool PanelPluginsModel::isIndexValid(QModelIndex const & index) const
{
    return index.isValid() && QModelIndex() == index.parent()
        && 0 == index.column() && m_plugins.size() > index.row();
}

void PanelPluginsModel::onMovePluginUp(QModelIndex const & index)
{
    if (!isIndexValid(index))
        return;

    const int row = index.row();
    if (0 >= row)
        return; //can't move up

    beginMoveRows(QModelIndex(), row, row, QModelIndex(), row - 1);
    m_plugins.swap(row - 1, row);
    endMoveRows();
    pluginslist_t::const_reference moved_plugin = m_plugins[row - 1];
    pluginslist_t::const_reference prev_plugin = m_plugins[row];

    emit pluginMoved(moved_plugin.second.data());
    //emit signal for layout only in case both plugins are loaded/displayed
    if (!moved_plugin.second.isNull() && !prev_plugin.second.isNull())
        emit pluginMovedUp(moved_plugin.second.data());

    m_panel->settings()->setValue(m_namesKey, pluginNames());
}

void PanelPluginsModel::onMovePluginDown(QModelIndex const & index)
{
    if (!isIndexValid(index))
        return;

    const int row = index.row();
    if (m_plugins.size() <= row + 1)
        return; //can't move down

    beginMoveRows(QModelIndex(), row, row, QModelIndex(), row + 2);
    m_plugins.swap(row, row + 1);
    endMoveRows();
    pluginslist_t::const_reference moved_plugin = m_plugins[row + 1];
    pluginslist_t::const_reference next_plugin = m_plugins[row];

    emit pluginMoved(moved_plugin.second.data());
    //emit signal for layout only in case both plugins are loaded/displayed
    if (!moved_plugin.second.isNull() && !next_plugin.second.isNull())
        emit pluginMovedUp(next_plugin.second.data());
    m_panel->settings()->setValue(m_namesKey, pluginNames());
}

void PanelPluginsModel::onConfigurePlugin(QModelIndex const & index)
{
    if (!isIndexValid(index))
        return;

    Plugin * const plugin = m_plugins[index.row()].second.data();
    if (nullptr != plugin && (IUKUIPanelPlugin::HaveConfigDialog & plugin->iPlugin()->flags()))
        plugin->showConfigureDialog();
}

void PanelPluginsModel::onRemovePlugin(QModelIndex const & index)
{
    if (!isIndexValid(index))
        return;

    auto plugin = m_plugins.begin() + index.row();
    if (plugin->second.isNull())
        removePlugin(std::move(plugin));
    else
        plugin->second->requestRemove();
}
