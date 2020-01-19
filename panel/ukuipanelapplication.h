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


#ifndef UKUIPanelAPPLICATION_H
#define UKUIPanelAPPLICATION_H

//#include <UKUi/Application>
#include "common/ukuiapplication.h"
#include "iukuipanelplugin.h"

class QScreen;

class UKUIPanel;
class UKUIPanelApplicationPrivate;

/*!
 * \brief The UKUIPanelApplication class inherits from UKUi::Application and
 * is therefore the QApplication that we will create and execute in our
 * main()-function.
 *
 * UKUIPanelApplication itself is not a visible panel, rather it is only
 * the container which holds the visible panels. These visible panels are
 * UKUIPanel objects which are stored in mPanels. This approach enables us
 * to have more than one panel (for example one panel at the top and one
 * panel at the bottom of the screen) without additional effort.
 */
class UKUIPanelApplication : public UKUi::Application
{
    Q_OBJECT
public:
    /*!
     * \brief Creates a new UKUIPanelApplication with the given command line
     * arguments. Performs the following steps:
     * 1. Initializes the UKUi::Application, sets application name and version.
     * 2. Handles command line arguments. Currently, the only cmdline argument
     * is -c = -config = -configfile which chooses a different config file
     * for the UKUi::Settings.
     * 3. Creates the UKUi::Settings.
     * 4. Connects QCoreApplication::aboutToQuit to cleanup().
     * 5. Calls addPanel() for each panel found in the config file. If there is
     * none, adds a new panel.
     * \param argc
     * \param argv
     */
    explicit UKUIPanelApplication(int& argc, char** argv);
    ~UKUIPanelApplication();

    void setIconTheme(const QString &iconTheme);
    void updateStylesheet(QString themeName);
    /*!
     * \brief Determines the number of UKUIPanel objects
     * \return the current number of UKUIPanel objects
     */
    int count() const { return mPanels.count(); }

    /*!
     * \brief Checks if a given Plugin is running and has the
     * IUKUIPanelPlugin::SingleInstance flag set. As Plugins are added to
     * UKUIPanel instances, this method only iterates over these UKUIPanel
     * instances and lets them check the conditions.
     * \param pluginId Plugin Identifier which is the basename of the .desktop
     * file that specifies the plugin.
     * \return true if the Plugin is running and has the
     * IUKUIPanelPlugin::SingleInstance flag set, false otherwise.
     */
    bool isPluginSingletonAndRunnig(QString const & pluginId) const;

public slots:
    /*!
     * \brief Adds a new UKUIPanel which consists of the following steps:
     * 1. Create id/name.
     * 2. Create the UKUIPanel: call addPanel(name).
     * 3. Update the config file (add the new panel id to the list of panels).
     * 4. Show the panel configuration dialog so that the user can add plugins.
     *
     * This method will create a new UKUIPanel with a new name and add this
     * to the config file. So this should only be used while the application
     * is running and the user decides to add a new panel. At application
     * startup, addPanel() should be used instead.
     *
     * \note This slot will be used from the UKUIPanel right-click menu. As we
     * can only add new panels from a visible panel, we should never run
     * lxqt-panel without an UKUIPanel. Without a panel, we have just an
     * invisible application.
     */
    void addNewPanel();

signals:
    /*!
     * \brief Signal that re-emits the signal pluginAdded() from UKUIPanel.
     */
    void pluginAdded();
    /*!
     * \brief Signal that re-emits the signal pluginRemoved() from UKUIPanel.
     */
    void pluginRemoved();

private:
    /*!
     * \brief Holds all the instances of UKUIPanel.
     */
    QList<UKUIPanel*> mPanels;
    /*!
     * \brief The global icon theme used by all apps (except for panels perhaps).
     */
    QString mGlobalIconTheme;
    /*!
     * \brief Creates a new UKUIPanel with the given name and connects the
     * appropriate signals and slots.
     * This method can be used at application startup.
     * \param name Name of the UKUIPanel as it is used in the config file.
     * \return The newly created UKUIPanel.
     */
    UKUIPanel* addPanel(const QString &name);

private slots:
    /*!
     * \brief Removes the given UKUIPanel which consists of the following
     * steps:
     * 1. Remove the panel from mPanels.
     * 2. Remove the panel from the config file.
     * 3. Schedule the QObject for deletion: QObject::deleteLater().
     * \param panel UKUIPanel instance that should be removed.
     */
    void removePanel(UKUIPanel* panel);

    /*!
     * \brief Connects the QScreen::destroyed signal of a new screen to
     * the screenDestroyed() slot so that we can handle this screens'
     * destruction as soon as it happens.
     * \param newScreen The QScreen that was created and added.
     */
    void handleScreenAdded(QScreen* newScreen);
    /*!
     * \brief Handles screen destruction. This is a workaround for a Qt bug.
     * For further information, see the implementation notes.
     * \param screenObj The QScreen that was destroyed.
     */
    void screenDestroyed(QObject* screenObj);
    /*!
     * \brief Reloads the panels. This is the second part of the workaround
     * mentioned above.
     */
    void reloadPanelsAsNeeded();
    /*!
     * \brief Deletes all UKUIPanel instances that are stored in mPanels.
     */
    void cleanup();

private:
    /*!
     * \brief mSettings is the UKUi::Settings object that is used for the
     * current instance of ukui-panel. Normally, this refers to the config file
     * $HOME/.config/ukui/panel.conf (on Unix systems). This behaviour can be
     * changed with the -c command line option.
     */

    UKUIPanelApplicationPrivate *const d_ptr;

    Q_DECLARE_PRIVATE(UKUIPanelApplication)
    Q_DISABLE_COPY(UKUIPanelApplication)
};


#endif // UKUIPanelAPPLICATION_H
