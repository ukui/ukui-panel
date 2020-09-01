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


#include "plugin.h"
#include "iukuipanelplugin.h"
#include "pluginsettings_p.h"
#include "ukuipanel.h"
#include <QDebug>
#include <QProcessEnvironment>
#include <QStringList>
#include <QDir>
#include <QFileInfo>
#include <QPluginLoader>
#include <QGridLayout>
#include <QDialog>
#include <QEvent>
#include <QMenu>
#include <QMouseEvent>
#include <QApplication>
#include <QWindow>
#include <memory>
#include <KF5/KWindowSystem/KWindowSystem>

//#include <UKUi/Settings>
#include "common/ukuisettings.h"
//#include <UKUi/Translator>
#include "../panel/common/ukuitranslator.h"
#include <XdgIcon>


// statically linked built-in plugins
#if defined(WITH_DESKTOPSWITCH_PLUGIN)
#include "../plugin-desktopswitch/desktopswitch.h" // desktopswitch
extern void * loadPluginTranslation_desktopswitch_helper;
#endif
#if defined(WITH_MAINMENU_PLUGIN)
#include "../plugin-mainmenu/ukuimainmenu.h" // mainmenu
extern void * loadPluginTranslation_mainmenu_helper;
#endif
#if defined(WITH_QUICKLAUNCH_PLUGIN)
#include "../plugin-quicklaunch/ukuiquicklaunchplugin.h" // quicklaunch
extern void * loadPluginTranslation_quicklaunch_helper;
#endif
#if defined(WITH_SHOWDESKTOP_PLUGIN)
#include "../plugin-showdesktop/showdesktop.h" // showdesktop
extern void * loadPluginTranslation_showdesktop_helper;
#endif
#if defined(WITH_SPACERX_PLUGIN)
#include "../plugin-spacerx/spacerx.h" // spacerx
extern void * loadPluginTranslation_spacerx_helper;
#endif
#if defined(WITH_SPACER_PLUGIN)
#include "../plugin-spacer/spacer.h" // spacer
extern void * loadPluginTranslation_spacer_helper;
#endif
#if defined(WITH_STATUSNOTIFIER_PLUGIN)
#include "../plugin-statusnotifier/statusnotifier.h" // statusnotifier
extern void * loadPluginTranslation_statusnotifier_helper;
#endif
#if defined(WITH_TASKBAR_PLUGIN)
#include "../plugin-taskbar/ukuitaskbarplugin.h" // taskbar
extern void * loadPluginTranslation_taskbar_helper;
#endif
#if defined(WITH_TRAY_PLUGIN)
#include "../plugin-tray/ukuitrayplugin.h" // tray
extern void * loadPluginTranslation_tray_helper;
#endif
#if defined(WITH_QUICKLAUNCH_PLUGIN)
#include "../plugin-statusnotifier/statusnotifier.h" // statusnotifier
extern void * loadPluginTranslation_quicklaunch_helper;
#endif
#if defined(WITH_WORLDCLOCK_PLUGIN)
#include "../plugin-worldclock/ukuiworldclock.h" // worldclock
extern void * loadPluginTranslation_worldclock_helper;
#endif
#if defined(WITH_CALENDAR_PLUGIN)
#include "../plugin-calendar/ukuicalendar.h" // indicatorCalendar
extern void * loadPluginTranslation_calendar_helper;
#endif

#if defined(WITH_STARTMENU_PLUGIN)
#include "../plugin-startmenu/startmenu.h" // startmenu
extern void * loadPluginTranslation_startmenu_helper;
#endif
#if defined(WITH_SEGMENTATION_PLUGIN)
#include "../plugin-segmentation/segmentation.h" // startmenu
extern void * loadPluginTranslation_segmentation_helper;
#endif
#if defined(WITH_NIGHTMODE_PLUGIN)
#include "../plugin-nightmode/nightmode.h" // startmenu
extern void * loadPluginTranslation_nightmode_helper;
#endif

QColor Plugin::mMoveMarkerColor= QColor(255, 0, 0, 255);

/************************************************

 ************************************************/
Plugin::Plugin(const UKUi::PluginInfo &desktopFile, UKUi::Settings *settings, const QString &settingsGroup,UKUIPanel *panel) :
    QFrame(panel),
    mDesktopFile(desktopFile),
    mPluginLoader(0),
    mPlugin(0),
    mPluginWidget(0),
    mAlignment(AlignLeft),
    mPanel(panel)
{
    mSettings = PluginSettingsFactory::create(settings, settingsGroup);

    setWindowTitle(desktopFile.name());
    mName = desktopFile.name();

    QStringList dirs;
    dirs << QProcessEnvironment::systemEnvironment().value("UKUIPanel_PLUGIN_PATH").split(":");
    dirs << PLUGIN_DIR;

    bool found = false;
    if(IUKUIPanelPluginLibrary const * pluginLib = findStaticPlugin(desktopFile.id()))
    {
        // this is a static plugin
        found = true;
        loadLib(pluginLib);
    }
    else {
        // this plugin is a dynamically loadable module
        QString baseName = QString("lib%1.so").arg(desktopFile.id());
#if (QT_VERSION < QT_VERSION_CHECK(5,7,0))
        for(int i=0;i<dirs.size();i++){
            const QString &dirName=dirs.at(i);
#endif
#if (QT_VERSION >= QT_VERSION_CHECK(5,7,0))
        for(const QString &dirName : qAsConst(dirs)){
#endif

            QFileInfo fi(QDir(dirName), baseName);
            if (fi.exists())
            {
                found = true;
                if (loadModule(fi.absoluteFilePath()))
                    break;
            }
        }
    }

    if (!isLoaded())
    {
        if (!found)
            qWarning() << QString("Plugin %1 not found in the").arg(desktopFile.id()) << dirs;

        return;
    }

    setObjectName(mPlugin->themeId() + "Plugin");

    // plugin handle for easy context menu
    setProperty("NeedsHandle", mPlugin->flags().testFlag(IUKUIPanelPlugin::NeedsHandle));

    QString s = mSettings->value("alignment").toString();

    // Retrun default value
    if (s.isEmpty())
    {
        mAlignment = (mPlugin->flags().testFlag(IUKUIPanelPlugin::PreferRightAlignment)) ?
                    Plugin::AlignRight :
                    Plugin::AlignLeft;
    }
    else
    {
        mAlignment = (s.toUpper() == "RIGHT") ?
                    Plugin::AlignRight :
                    Plugin::AlignLeft;

    }

    if (mPluginWidget)
    {
        QGridLayout* layout = new QGridLayout(this);
        layout->setSpacing(0);
        layout->setContentsMargins(0, 0, 0, 0);
        setLayout(layout);
        layout->addWidget(mPluginWidget, 0, 0);
    }

    saveSettings();

    // delay the connection to settingsChanged to avoid conflicts
    // while the plugin is still being initialized
    connect(mSettings, &PluginSettings::settingsChanged,
            this, &Plugin::settingsChanged);
}


/************************************************

 ************************************************/
Plugin::~Plugin()
{
    delete mPlugin;
    delete mPluginLoader;
    delete mSettings;
}

void Plugin::setAlignment(Plugin::Alignment alignment)
{
    mAlignment = alignment;
    saveSettings();
}


/************************************************

 ************************************************/
namespace
{
    //helper types for static plugins storage & binary search
    typedef std::unique_ptr<IUKUIPanelPluginLibrary> plugin_ptr_t;
    typedef std::tuple<QString, plugin_ptr_t, void *> plugin_tuple_t;

    //NOTE: Please keep the plugins sorted by name while adding new plugins.
    //NOTE2: we need to reference some (dummy) symbol from (autogenerated) UKUiPluginTranslationLoader.cpp
    // to be not stripped (as unused/unreferenced) in static linking time
    static plugin_tuple_t const static_plugins[] = {

#if defined(WITH_CALENDAR_PLUGIN)
      std::make_tuple(QLatin1String("calendar"), plugin_ptr_t{new IndicatorCalendarPluginLibrary}, loadPluginTranslation_calendar_helper),// desktopswitch
#endif

#if defined(WITH_DESKTOPSWITCH_PLUGIN)
        std::make_tuple(QLatin1String("desktopswitch"), plugin_ptr_t{new DesktopSwitchPluginLibrary}, loadPluginTranslation_desktopswitch_helper),// desktopswitch
#endif
#if defined(WITH_MAINMENU_PLUGIN)
        std::make_tuple(QLatin1String("mainmenu"), plugin_ptr_t{new UKUiMainMenuPluginLibrary}, loadPluginTranslation_mainmenu_helper),// mainmenu
#endif
#if defined(WITH_QUICKLAUNCH_PLUGIN)
        std::make_tuple(QLatin1String("quicklaunch"), plugin_ptr_t{new UKUIQuickLaunchPluginLibrary}, loadPluginTranslation_quicklaunch_helper),// quicklaunch
#endif
#if defined(WITH_SHOWDESKTOP_PLUGIN)
        std::make_tuple(QLatin1String("showdesktop"), plugin_ptr_t{new ShowDesktopLibrary}, loadPluginTranslation_showdesktop_helper),// showdesktop
#endif
#if defined(WITH_SPACERX_PLUGIN)
        std::make_tuple(QLatin1String("spacerx"), plugin_ptr_t{new SpacerXPluginLibrary}, loadPluginTranslation_spacerx_helper),// spacerx
#endif
#if defined(WITH_SPACER_PLUGIN)
        std::make_tuple(QLatin1String("spacer"), plugin_ptr_t{new SpacerPluginLibrary}, loadPluginTranslation_spacer_helper),// spacer
#endif
#if defined(WITH_STATUSNOTIFIER_PLUGIN)
        std::make_tuple(QLatin1String("statusnotifier"), plugin_ptr_t{new StatusNotifierLibrary}, loadPluginTranslation_statusnotifier_helper),// statusnotifier
#endif
#if defined(WITH_TASKBAR_PLUGIN)
        std::make_tuple(QLatin1String("taskbar"), plugin_ptr_t{new UKUITaskBarPluginLibrary}, loadPluginTranslation_taskbar_helper),// taskbar
#endif
#if defined(WITH_TRAY_PLUGIN)
        std::make_tuple(QLatin1String("tray"), plugin_ptr_t{new UKUITrayPluginLibrary}, loadPluginTranslation_tray_helper),// tray
#endif
    #if defined(WITH_QUICKLAUNCH_PLUGIN)
            std::make_tuple(QLatin1String("statusnotifier"), plugin_ptr_t{new StatusNotifierLibrary}, loadPluginTranslation_statusnotifier_helper),// quicklaunch
    #endif
#if defined(WITH_WORLDCLOCK_PLUGIN)
        std::make_tuple(QLatin1String("worldclock"), plugin_ptr_t{new UKUiWorldClockLibrary}, loadPluginTranslation_worldclock_helper),// worldclock
#endif
#if defined(WITH_CALENDAR_PLUGIN)
        std::make_tuple(QLatin1String("calendar"), plugin_ptr_t{new UKUICalendarPluginLibrary}, loadPluginTranslation_calendar_helper),// calendar
#endif
#if defined(WITH_STARTMENU_PLUGIN)
        std::make_tuple(QLatin1String("startmenu"), plugin_ptr_t{new UKUIStartMenuLibrary}, loadPluginTranslation_startmenu_helper),// startmenu
#endif
#if defined(WITH_SEGMENTATION_PLUGIN)
        std::make_tuple(QLatin1String("segmentation"), plugin_ptr_t{new StartMenuLibrary}, loadPluginTranslation_segementation_helper),// startmenu
#endif
#if defined(WITH_NIGHTMODE_PLUGIN)
        std::make_tuple(QLatin1String("nightmode"), plugin_ptr_t{new NightModeLibrary}, loadPluginTranslation_nightmode_helper),// nightmode
#endif
    };
    static constexpr plugin_tuple_t const * const plugins_begin = static_plugins;
    static constexpr plugin_tuple_t const * const plugins_end = static_plugins + sizeof (static_plugins) / sizeof (static_plugins[0]);

    struct assert_helper
    {
        assert_helper()

        {
            Q_ASSERT(std::is_sorted(plugins_begin, plugins_end
                        , [] (plugin_tuple_t const & p1, plugin_tuple_t const & p2) -> bool { return std::get<0>(p1) < std::get<0>(p2); }));
        }
    };
    static assert_helper h;
}

IUKUIPanelPluginLibrary const * Plugin::findStaticPlugin(const QString &libraryName)
{
    // find a static plugin library by name -> binary search
    plugin_tuple_t const * plugin = std::lower_bound(plugins_begin, plugins_end, libraryName
            , [] (plugin_tuple_t const & plugin, QString const & name) -> bool { return std::get<0>(plugin) < name; });
    if (plugins_end != plugin && libraryName == std::get<0>(*plugin))
        return std::get<1>(*plugin).get();
    return nullptr;
}

// load a plugin from a library
bool Plugin::loadLib(IUKUIPanelPluginLibrary const * pluginLib)
{
    IUKUIPanelPluginStartupInfo startupInfo;
    startupInfo.settings = mSettings;
    startupInfo.desktopFile = &mDesktopFile;
    startupInfo.ukuiPanel = mPanel;

    mPlugin = pluginLib->instance(startupInfo);
    if (!mPlugin)
    {
        qWarning() << QString("Can't load plugin \"%1\". Plugin can't build IUKUIPanelPlugin.").arg(mDesktopFile.id());
        return false;
    }

    mPluginWidget = mPlugin->widget();
    if (mPluginWidget)
    {
        mPluginWidget->setObjectName(mPlugin->themeId());
        watchWidgets(mPluginWidget);
    }
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    return true;
}

// load dynamic plugin from a *.so module
bool Plugin::loadModule(const QString &libraryName)
{
    mPluginLoader = new QPluginLoader(libraryName);

    if (!mPluginLoader->load())
    {
        qWarning() << mPluginLoader->errorString();
        return false;
    }

    QObject *obj = mPluginLoader->instance();
    if (!obj)
    {
        qWarning() << mPluginLoader->errorString();
        return false;
    }

    IUKUIPanelPluginLibrary* pluginLib= qobject_cast<IUKUIPanelPluginLibrary*>(obj);
    if (!pluginLib)
    {
        qWarning() << QString("Can't load plugin \"%1\". Plugin is not a IUKUIPanelPluginLibrary.").arg(mPluginLoader->fileName());
        delete obj;
        return false;
    }
    return loadLib(pluginLib);
}

/************************************************

 ************************************************/
void Plugin::watchWidgets(QObject * const widget)
{
    // the QWidget might not be fully constructed yet, but we can rely on the isWidgetType()
    if (!widget->isWidgetType())
        return;
    widget->installEventFilter(this);
    // watch also children (recursive)
    for (auto const & child : widget->children())
    {
        watchWidgets(child);
    }
}

/************************************************

 ************************************************/
void Plugin::unwatchWidgets(QObject * const widget)
{
    widget->removeEventFilter(this);
    // unwatch also children (recursive)
    for (auto const & child : widget->children())
    {
        unwatchWidgets(child);
    }
}

/************************************************

 ************************************************/
void Plugin::settingsChanged()
{
    mPlugin->settingsChanged();
}


/************************************************

 ************************************************/
void Plugin::saveSettings()
{
    mSettings->setValue("alignment", (mAlignment == AlignLeft) ? "Left" : "Right");
    mSettings->setValue("type", mDesktopFile.id());
    mSettings->sync();

}


/************************************************

 ************************************************/
void Plugin::contextMenuEvent(QContextMenuEvent *event)
{
    mPanel->showPopupMenu(this);
}


/************************************************

 ************************************************/
void Plugin::mousePressEvent(QMouseEvent *event)
{
    switch (event->button())
    {
    case Qt::LeftButton:
        mPlugin->activated(IUKUIPanelPlugin::Trigger);
        break;

    case Qt::MidButton:
        mPlugin->activated(IUKUIPanelPlugin::MiddleClick);
        break;

    default:
        break;
    }
}


/************************************************

 ************************************************/
void Plugin::mouseDoubleClickEvent(QMouseEvent*)
{
    mPlugin->activated(IUKUIPanelPlugin::DoubleClick);
}


/************************************************

 ************************************************/
void Plugin::showEvent(QShowEvent *)
{
    if (mPluginWidget)
        mPluginWidget->adjustSize();
}


/************************************************

 ************************************************/
QMenu *Plugin::popupMenu() const
{
    QString name = this->name().replace("&", "&&");
    QMenu* menu = new QMenu(windowTitle());

    /* //set top menu selection ,do not delete until you get the ui finish
    if (mPlugin->flags().testFlag(IUKUIPanelPlugin::HaveConfigDialog))
    {
        QAction* configAction = new QAction(
            XdgIcon::fromTheme(QLatin1String("preferences-other")),
            tr("Configure \"%1\"").arg(name), menu);
        menu->addAction(configAction);
        connect(configAction, SIGNAL(triggered()), this, SLOT(showConfigureDialog()));
    }

    QAction* moveAction = new QAction(XdgIcon::fromTheme("transform-move"), tr("Move \"%1\"").arg(name), menu);
    menu->addAction(moveAction);
    connect(moveAction, SIGNAL(triggered()), this, SIGNAL(startMove()));

    menu->addSeparator();

    QAction* removeAction = new QAction(
        XdgIcon::fromTheme(QLatin1String("list-remove")),
        tr("Remove \"%1\"").arg(name), menu);
    menu->addAction(removeAction);
    connect(removeAction, SIGNAL(triggered()), this, SLOT(requestRemove()));
    */

    return menu;
}


/************************************************

 ************************************************/
bool Plugin::isSeparate() const
{
   return mPlugin->isSeparate();
}


/************************************************

 ************************************************/
bool Plugin::isExpandable() const
{
    return mPlugin->isExpandable();
}


/************************************************

 ************************************************/
bool Plugin::eventFilter(QObject * watched, QEvent * event)
{
    switch (event->type())
    {
        case QEvent::DragLeave:
            emit dragLeft();
            break;
        case QEvent::ChildAdded:
            watchWidgets(dynamic_cast<QChildEvent *>(event)->child());
            break;
        case QEvent::ChildRemoved:
            unwatchWidgets(dynamic_cast<QChildEvent *>(event)->child());
            break;
        default:
            break;
    }
    return false;
}

/************************************************

 ************************************************/
void Plugin::realign()
{
    if (mPlugin)
        mPlugin->realign();
}


/************************************************

 ************************************************/
void Plugin::showConfigureDialog()
{
    if (!mConfigDialog)
        mConfigDialog = mPlugin->configureDialog();

    if (!mConfigDialog)
        return;

    connect(this, &Plugin::destroyed, mConfigDialog.data(), &QWidget::close);
    mPanel->willShowWindow(mConfigDialog);
    mConfigDialog->show();
    mConfigDialog->raise();
    mConfigDialog->activateWindow();

    WId wid = mConfigDialog->windowHandle()->winId();
    KWindowSystem::activateWindow(wid);
    KWindowSystem::setOnDesktop(wid, KWindowSystem::currentDesktop());
}


/************************************************

 ************************************************/
void Plugin::requestRemove()
{
    emit remove();
    deleteLater();
}
