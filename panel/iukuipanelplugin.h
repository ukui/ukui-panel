/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Copyright: 2012 Razor team
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


#ifndef IUKUIPANELPLUGIN_H
#define IUKUIPANELPLUGIN_H

#include <QtPlugin>
#include "iukuipanel.h"
#include "ukuipanelglobals.h"
#include <QDebug>
/**
UKUi panel plugins are standalone sharedlibraries
(*.so) located in PLUGIN_DIR (define provided by CMakeLists.txt).

Plugin for the panel is a library written in C++. One more necessary thing
is a .desktop file describing this plugin. The same may be additional files,
like translations. Themselves plugins will be installed to
/usr/local/lib/ukui-panel or /usr/lib/ukui-panel (dependent on cmake option
-DCMAKE_INSTALL_PREFIX). Desktop files are installed to
/usr/local/share/ukui/ukui-panel, translations to
/usr/local/share/ukui/ukui-panel/PLUGIN_NAME.
**/

class QDialog;
class PluginSettings;
namespace UKUi
{
    class PluginInfo;
}

struct UKUI_PANEL_API IUKUIPanelPluginStartupInfo
{
    IUKUIPanel *ukuiPanel;
    PluginSettings *settings;
    const UKUi::PluginInfo *desktopFile;
};


/** \brief Base abstract class for UKUi panel widgets/plugins.
All plugins *must* be inherited from this one.

This class provides some basic API and inherited/implemented
plugins GUIs will be responsible on the functionality itself.
**/

class UKUI_PANEL_API IUKUIPanelPlugin
{
public:
    /**
      This enum describes the properties of a plugin.
     **/
    enum Flag {
        NoFlags              = 0,   ///< It does not have any properties set.
        PreferRightAlignment = 1,   /**< The plugin prefers right alignment (for example the clock plugin);
                                        otherwise the plugin prefers left alignment (like main menu).
                                        This flag is used only at the first start, later positions of all
                                        plugins are saved in a config, and this saved information is used. */
        HaveConfigDialog     = 2,   ///< The plugin have a configuration dialog.
        SingleInstance       = 4,   ///< The plugin allows only one instance to run.
        NeedsHandle          = 8    ///< The plugin needs a handle for the context menu
    };

    Q_DECLARE_FLAGS(Flags, Flag)

    /**
      This enum describes the reason the plugin was activated.
     **/
    enum ActivationReason {
        Unknown             = 0,    ///< Unknown reason
        DoubleClick         = 2,    ///<	The plugin entry was double clicked
        Trigger             = 3,    ///<	The plugin was clicked
        MiddleClick         = 4     ///< The plugin was clicked with the middle mouse button
    };

   enum CalendarShowMode
   {
       lunarSunday        = 0,//show lunar and first day a week is sunday
       lunarMonday        = 1,//show lunar and first day a week is monday
       solarSunday        = 2,//show solar and first day a week is sunday
       solarMonday        = 3,//show solar and first day a week is monday
       defaultMode        = 0xff
   };

    /**
     Constructs an IUKUIPanelPlugin object with the given startupInfo. You do not have to worry
     about the startupInfo parameters, IUKUIPanelPlugin processes the parameters itself.
     **/
    IUKUIPanelPlugin(const IUKUIPanelPluginStartupInfo &startupInfo):
        mSettings(startupInfo.settings),
        mPanel(startupInfo.ukuiPanel),
        mDesktopFile(startupInfo.desktopFile)
    {}

    /**
     Destroys the object.
     **/
    virtual ~IUKUIPanelPlugin() {}

    /**
    Returns the plugin flags.
    The base class implementation returns a NoFlags.
     **/
    virtual Flags flags() const { return NoFlags; }

    /**
    Returns the string that is used in the theme QSS file.
    If you return "WorldClock" string, theme author may write something like `#WorldClock { border: 1px solid red; }`
    to set a custom border for your plugin.
    **/
    virtual QString themeId() const = 0;

    /**
     From the user's point of view, your plugin is some visual widget on the panel. This function returns a pointer to it.
     This method is called only once, so you are free to return the pointer on a class member, or create the widget on the fly.
     **/
    virtual QWidget *widget() = 0;

    /**
    Returns the plugin settings dialog. Reimplement this function if your plugin has it.
    The panel does not take ownership of the dialog, it is probably a good idea to set Qt::WA_DeleteOnClose
    attribute for the dialog.
    The default implementation returns 0, no dialog;

    Note that the flags method has to return HaveConfigDialog flag.
    To save the settings you should use a ready-to-use IUKUIPanelPlugin::settings() object.

    **/
    virtual QDialog *configureDialog() { return 0; }

    /**
    This function is called when values are changed in the plugin settings.
    Reimplement this function to your plugin corresponded the new settings.

    The default implementation do nothing.
    **/
    virtual void settingsChanged() {}

    /**
    This function is called when the user activates the plugin. reason specifies the reason for activation.
    IUKUIPanelPlugin::ActivationReason enumerates the various reasons.

    The default implementation do nothing.
     **/
    virtual void activated(ActivationReason reason) {}

    /**
    This function is called when the panel geometry or lines count are changed.

    The default implementation do nothing.

     **/
    virtual void realign() {}

    /**
    Returns the panel object.
     **/
    IUKUIPanel *panel() const { return mPanel; }


    PluginSettings *settings() const { return mSettings; }
    const UKUi::PluginInfo *desktopFile() const { return mDesktopFile; }

    /**
     Helper functions for calculating global screen position of some popup window with windowSize size.
     If you need to show some popup window, you can use it, to get global screen position for the new window.
     **/
    virtual QRect calculatePopupWindowPos(const QSize &windowSize)
    {
        return mPanel->calculatePopupWindowPos(this, windowSize);
    }

    /*!
     * \brief By calling this function plugin notifies the panel about showing a (standalone) window/menu.
     *
     * \param w the shown window
     *
     */
    inline void willShowWindow(QWidget * w)
    {
        mPanel->willShowWindow(w);
    }

    /*!
     * \brief By calling this function, a plugin notifies the panel about change of it's "static"
     * configuration
     *
     * \sa isSeparate(), isExpandable
     */
    inline void pluginFlagsChanged()
    {
        mPanel->pluginFlagsChanged(this);
    }

    virtual bool isSeparate() const { return false;  }
    virtual bool isExpandable() const { return false; }
private:
    PluginSettings *mSettings;
    IUKUIPanel *mPanel;
    const UKUi::PluginInfo *mDesktopFile;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(IUKUIPanelPlugin::Flags)

/**
Every plugin must have the IUKUIPanelPluginLibrary loader. You should only reimplement the instance() method which should return your plugin.
Example:
@code
class UKUiClockPluginLibrary: public QObject, public IUKUIPanelPluginLibrary
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "ukui.org/Panel/PluginInterface/3.0")
    Q_INTERFACES(IUKUIPanelPluginLibrary)
public:
    IUKUIPanelPlugin *instance(const IUKUIPanelPluginStartupInfo &startupInfo) { return new UKUiClock(startupInfo);}
};
@endcode
**/
class UKUI_PANEL_API IUKUIPanelPluginLibrary
{
public:
    /**
     Destroys the IUKUIPanelPluginLibrary object.
     **/
    virtual ~IUKUIPanelPluginLibrary() {}

    /**
    Returns the root component object of the plugin. When the library is finally unloaded, the root component will automatically be deleted.
     **/
    virtual IUKUIPanelPlugin* instance(const IUKUIPanelPluginStartupInfo &startupInfo) const = 0;
};


Q_DECLARE_INTERFACE(IUKUIPanelPluginLibrary,
                    "ukui.org/Panel/PluginInterface/3.0")

#endif // IUKUIPANELPLUGIN_H
