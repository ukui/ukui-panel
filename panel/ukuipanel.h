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


#ifndef UKUIPANEL_H
#define UKUIPANEL_H

#include <QFrame>
#include <QString>
#include <QTimer>
#include <QPropertyAnimation>
#include <QPointer>
#include <QDesktopWidget>
#include <QGSettings/QGSettings>
#include <QtDBus/QtDBus>
#include <QStyleOption>
#include "common/ukuisettings.h"
#include "iukuipanel.h"
#include "ukuipanelglobals.h"

class QMenu;
class Plugin;
class QAbstractItemModel;

namespace UKUi {
class Settings;
class PluginInfo;
}
class UKUIPanelLayout;
class ConfigPanelDialog;

class PanelPluginsModel;
class WindowNotifier;

/*! \brief The UKUIPanel class provides a single ukui-panel. All UKUIPanel
 * instances should be created and handled by UKUIPanelApplication. In turn,
 * all Plugins should be created and handled by UKUIPanels.
 *
 * UKUIPanel is just the panel, it does not incorporate any functionality.
 * Each function of the panel is implemented by Plugins, even the mainmenu
 * (plugin-mainmenu) and the taskbar (plugin-taskbar). So the UKUIPanel is
 * just the container for several Plugins while the different Plugins
 * incorporate the functions of the panel. Without the Plugins, the panel
 * is quite useless because it is just a box occupying space on the screen.
 *
 * UKUIPanel itself is a window (QFrame/QWidget) and this class is mainly
 * responsible for handling the size and position of this window on the
 * screen(s) as well as the different settings. The handling of the plugins
 * is outsourced in PanelPluginsModel and UKUIPanelLayout. PanelPluginsModel
 * is responsible for loading/creating and handling the plugins.
 * UKUIPanelLayout is inherited from QLayout and set as layout to the
 * background of UKUIPanel, so UKUIPanelLayout is responsible for the
 * layout of all the Plugins.
 *
 * \sa UKUIPanelApplication, Plugin, PanelPluginsModel, UKUIPanelLayout.
 */
class UKUI_PANEL_API UKUIPanel : public QFrame, public IUKUIPanel
{
    Q_OBJECT


    Q_CLASSINFO("D-Bus Interface", "org.ukui.panel.settings")
    // for configuration dialog
    friend class ConfigPanelWidget;
    friend class ConfigPluginsWidget;
    friend class ConfigPanelDialog;
    friend class PanelPluginsModel;

public:
    /**
     * @brief Stores how the panel should be aligned. Obviously, this applies
     * only if the panel does not occupy 100 % of the available space. If the
     * panel is vertical, AlignmentLeft means align to the top border of the
     * screen, AlignmentRight means align to the bottom.
     */
    enum Alignment {
        AlignmentLeft   = -1, //!< Align the panel to the left or top
        AlignmentCenter =  0, //!< Center the panel
        AlignmentRight  =  1 //!< Align the panel to the right or bottom
    };

    /**
     * @brief Creates and initializes the UKUIPanel. Performs the following
     * steps:
     * 1. Sets Qt window title, flags, attributes.
     * 2. Creates the panel layout.
     * 3. Prepares the timers.
     * 4. Connects signals and slots.
     * 5. Reads the settings for this panel.
     * 6. Optionally moves the panel to a valid screen (position-dependent).
     * 7. Loads the Plugins.
     * 8. Shows the panel, even if it is hidable (but then, starts the timer).
     * @param configGroup The name of the panel which is used as identifier
     * in the config file.
     * @param settings The settings instance of this ukui panel application.
     * @param parent Parent QWidget, can be omitted.
     */
    UKUIPanel(const QString &configGroup, UKUi::Settings *settings, QWidget *parent = 0);
    virtual ~UKUIPanel();

    /**
     * @brief Returns the name of this panel which is also used as identifier
     * in the config file.
     */
    QString name() { return m_configGroup; }

    /**
     * @brief Reads all the necessary settings from mSettings and stores them
     * in local variables. Additionally, calls necessary methods like realign()
     * or updateStyleSheet() which need to get called after changing settings.
     */
    void readSettings();

    /**
     * @brief Creates and shows the popup menu (right click menu). If a plugin
     * is given as parameter, the menu will be divided in two groups:
     * plugin-specific options and panel-related options. As these two are
     * shown together, this menu has to be created by UKUIPanel.
     * @param plugin The plugin whose menu options will be included in the
     * context menu.
     */
    void showPopupMenu(Plugin *plugin = 0);

    // IUKUIPanel overrides ........
    IUKUIPanel::Position position() const override { return m_position; }
    QRect globalGeometry() const override;
    /**
     * @brief calculatePopupWindowPos 计算任务栏弹窗的位置
     * @param absolutePos
     * @param windowSize 窗口尺寸
     * @return
     */
    QRect calculatePopupWindowPos(QPoint const & absolutePos, QSize const & windowSize) const override;
    QRect calculatePopupWindowPos(const IUKUIPanelPlugin *plugin, const QSize &windowSize) const override;
    void willShowWindow(QWidget * w) override;
    void pluginFlagsChanged(const IUKUIPanelPlugin * plugin) override;
    // ........ end of IUKUIPanel overrides

    /**
     * @brief Searches for a Plugin in the Plugins-list of this panel. Takes
     * an IUKUIPanelPlugin as parameter and returns the corresponding Plugin.
     * @param iPlugin IUKUIPanelPlugin that we are looking for.
     * @return The corresponding Plugin if it is loaded in this panel, nullptr
     * otherwise.
     */
    Plugin *findPlugin(const IUKUIPanelPlugin *iPlugin) const;


    /**
     * @brief Checks if this UKUIPanel can be placed at a given position
     * on the screen with the given screenNum. The condition for doing so
     * is that the panel is not located between two screens.
     *
     * For example, if position is PositionRight, there should be no screen to
     * the right of the given screen. That means that there should be no
     * screen whose left border has a higher x-coordinate than the x-coordinate
     * of the right border of the given screen. This method iterates over all
     * screens and checks these conditions.
     * @param screenNum screen index as it is used by QDesktopWidget methods
     * @param position position where the panel should be placed
     * @return true if this panel can be placed at the given position on the
     * given screen.
     *
     * \sa findAvailableScreen(), mScreenNum, mActualScreenNum.
     */
    static bool canPlacedOn(int screenNum, UKUIPanel::Position position);
    /**
     * @brief Returns a string representation of the given position. This
     * string is human-readable and can be used in config files.
     * @param position position that should be converted to a string.
     * @return the string representation of the given position, i.e.
     * "Top", "Left", "Right" or "Bottom".
     *
     * \sa strToPosition()
     */
    static QString positionToStr(IUKUIPanel::Position position);
    /**
     * @brief Returns an IUKUIPanel::Position from the given string. This can
     * be used to retrieve IUKUIPanel::Position values from the config files.
     * @param str string that should be converted to IUKUIPanel::Position
     * @param defaultValue value that will be returned if the string can not
     * be converted to an IUKUIPanel::Position.
     * @return IUKUIPanel::Position that was determined from str or
     * defaultValue if str could not be converted.
     *
     * \sa positionToStr()
     */
    static IUKUIPanel::Position strToPosition(const QString &str, IUKUIPanel::Position defaultValue);
    static IUKUIPanel::Position intToPosition(const int p, IUKUIPanel::Position defaultValue);


    // Settings
    int iconSize() const override { return m_iconSize; } //!< Implement IUKUIPanel::iconSize().
    int lineCount() const override { return m_lineCount; } //!< Implement IUKUIPanel::lineCount().
    int panelSize() const override{ return m_panelSize; }
    int length() const { return m_length; }
    bool lengthInPercents() const { return m_lengthInPercents; }
    UKUIPanel::Alignment alignment() const { return m_alignment; }
    int screenNum() const { return m_screenNum; }
    QColor fontColor() const { return m_fontColor; }
    int reserveSpace() const { return m_reserveSpace; }
    bool hidable() const { return m_hidable; }
    bool visibleMargin() const { return m_visibleMargin; }
    int animationTime() const { return m_animationTime; }
    int showDelay() const { return m_showDelayTimer.interval(); }

    /*!
     * \brief Checks if a given Plugin is running and has the
     * IUKUIPanelPlugin::SingleInstance flag set.
     * \param pluginId Plugin Identifier which is the basename of the
     * .desktop file that specifies the plugin.
     * \return true if the Plugin is running and has the
     * IUKUIPanelPlugin::SingleInstance flag set, false otherwise.
     */
    bool isPluginSingletonAndRunnig(QString const & pluginId) const;
    /*!
     * \brief Updates the config dialog. Used for updating its icons
     * when the panel-specific icon theme changes.
     */
//    void updateConfigDialog() const;

public slots:
    /**
     * @brief Shows the QWidget and makes it visible on all desktops. This
     * method is NOT related to showPanel(), hidePanel() and hidePanelWork()
     * which handle the UKUi hiding by resizing the panel.
     */
    void show();
    /**
     * @brief Shows the panel (immediately) after it had been hidden before.
     * Stops the QTimer mHideTimer. This it NOT the same as QWidget::show()
     * because hiding the panel in UKUi is done by making it very thin. So
     * this method in fact restores the original size of the panel.
     * \param animate flag for the panel show-up animation disabling (\sa mAnimationTime).
     *
     * \sa mHidable, mHidden, mHideTimer, hidePanel(), hidePanelWork()
     */
    void showPanel(bool animate);
    /**
     * @brief Hides the panel (delayed) by starting the QTimer mHideTimer.
     * When this timer times out, hidePanelWork() will be called. So this
     * method is called when the cursor leaves the panel area but the panel
     * will be hidden later.
     *
     * \sa mHidable, mHidden, mHideTimer, showPanel(), hidePanelWork()
     */
    void hidePanel();
    /**
     * @brief Actually hides the panel. Will be invoked when the QTimer
     * mHideTimer times out. That timer will be started by showPanel(). This
     * is NOT the same as QWidget::hide() because hiding the panel in UKUi is
     * done by making the panel very thin. So this method in fact makes the
     * panel very thin while the QWidget stays visible.
     *
     * \sa mHidable, mHidden, mHideTimer, showPanel(), hidePanel()
     */
    void hidePanelWork();

    // Settings
    /**
     * @brief All the setter methods are  designed similar:
     * 1. Check if the given value is different from the current value. If not,
     * do not do anything and return.
     * 2. Set the value.
     * 3. If parameter save is true, call saveSettings(true) to store the
     * new settings on the disk.
     * 4. If necessary, propagate the new value to child objects, e.g. to
     * mLayout.
     * 5. If necessary, call update methods like realign() or
     * updateStyleSheet().
     * @param value The value that should be set.
     * @param save If true, saveSettings(true) will be called.
     */
    void setPanelSize(int value, bool save);
    void setIconSize(int value, bool save); //!< \sa setPanelSize()
    void setLineCount(int value, bool save); //!< \sa setPanelSize()
    void setLength(int length, bool inPercents, bool save); //!< \sa setPanelSize()
    void setPosition(int screen, IUKUIPanel::Position position, bool save); //!< \sa setPanelSize()
    void setAlignment(UKUIPanel::Alignment value, bool save); //!< \sa setPanelSize()
    void setHidable(bool hidable, bool save); //!< \sa setPanelSize()
    void setVisibleMargin(bool visibleMargin, bool save); //!< \sa setPanelSize()
    void setAnimationTime(int animationTime, bool save); //!< \sa setPanelSize()
    void setShowDelay(int showDelay, bool save); //!< \sa setPanelSize()

    /**
     * @brief Saves the current configuration, i.e. writes the current
     * configuration varibles to mSettings.
     * @param later Determines if the settings are written immediately or
     * after a short delay. If later==true, the QTimer mDelaySave is started.
     * As soon as this timer times out, saveSettings(false) will be called. If
     * later==false, settings will be written.
     */
    void saveSettings(bool later=false);
    /**
     * @brief Checks if the panel can be placed on the current screen at the
     * current position. If it can not, it will be moved on another screen
     * where the desired position is possible.
     */
    void ensureVisible();

signals:
    /**
     * @brief This signal gets emitted whenever this panel receives a
     * QEvent::LayoutRequest, i.e. "Widget layout needs to be redone.".
     * The PanelPluginsModel will connect this signal to the individual
     * plugins so they can realign, too.
     */
    void realigned();
    /**
     * @brief This signal gets emitted at the end of
     * userRequestForDeletion() which in turn gets called when the user
     * decides to remove a panel. This signal is used by
     * UKUIPanelApplication to get notified whenever an UKUIPanel should
     * be removed.
     * @param self This UKUIPanel. UKUIPanelApplication will use this
     * parameter to identify the UKUIPanel that should be removed.
     */
    void deletedByUser(UKUIPanel *self);
    /**
     * @brief This signal is just a relay signal. The pluginAdded signal
     * of the PanelPluginsModel (mPlugins) will be connected to this
     * signal. Thereby, we can make this signal of a private member
     * available as a public signal.
     * Currently, this signal is used by UKUIPanelApplication which
     * will further re-emit this signal.
     */
    void pluginAdded();
    /**
     * @brief This signal is just a relay signal. The pluginRemoved signal
     * of the PanelPluginsModel (mPlugins) will be connected to this
     * signal. Thereby, we can make this signal of a private member
     * available as a public signal.
     * Currently, this signal is used by UKUIPanelApplication which
     * will further re-emit this signal.
     */
    void pluginRemoved();

protected:
    /**
     * @brief Overrides QObject::event(QEvent * e). Some functions of
     * the panel will be triggered by these events, e.g. showing/hiding
     * the panel or showing the context menu.
     * @param event The event that was received.
     * @return "QObject::event(QEvent *e) should return true if the event e
     * was recognized and processed." This is done by passing the event to
     * QFrame::event(QEvent *e) at the end.
     */
    bool event(QEvent *event) override;
    /**
     * @brief Overrides QWidget::showEvent(QShowEvent * event). This
     * method is called when a widget (in this case: the UKUIPanel) is
     * shown. The call could happen before and after the widget is shown.
     * This method is just overridden to get notified when the UKUIPanel
     * will be shown. Then, UKUIPanel will call realign().
     * @param event The QShowEvent sent by Qt.
     */
    void showEvent(QShowEvent *event) override;
    void paintEvent(QPaintEvent *);

    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mousePressEvent(QMouseEvent *event);

public slots:
    /**
     * @brief Shows the ConfigPanelDialog and shows the "Config Panel"
     * page, i.e. calls showConfigPanelPage(). If the dialog does not
     * exist yet, it will be created before.
     *
     * The "Configure Panel" button in the context menu of the panel will
     * be connected to this slot so this method gets called whenever the
     * user clicks that button.
     *
     * Furthermore, this method will be called by UKUIPanelApplication
     * when a new plugin gets added (the UKUIPanel instances are handled
     * by UKUIPanelApplication). That is why this method/slot has to be
     * public.
     */
//    void showConfigDialog();

private slots:
    /**
     * @brief Shows the ConfigPanelDialog and shows the "Config Plugins"
     * page, i.e. calls showConfigPluginsPage(). If the dialog does not
     * exist yet, it will be created before.
     *
     * The "Manage Widgets" button in the context menu of the panel will
     * be connected to this slot so this method gets called whenever the
     * user clicks that button.
     */
//    void showAddPluginDialog();
    /**
     * @brief Recalculates the geometry of the panel and reserves the
     * window manager strut, i.e. it calls setPanelGeometry() and
     * updateWmStrut().
     * Two signals will be connected to this slot:
     * 1. QDesktopWidget::workAreaResized(int screen) which will be emitted
     * when the work area available (on screen) changes.
     * 2. UKUi::Application::themeChanged(), i.e. when the user changes
     * the theme.
     */
    void systeMonitor();
    void showDesktop();
    void showTaskView();
    void showNightModeButton();
    void adjustPanel();
    void realign();
    /**
     * @brief Moves a plugin in PanelPluginsModel, i.e. calls
     * PanelPluginsModel::movePlugin(Plugin * plugin, QString const & nameAfter).
     * UKUIPanelLayout::pluginMoved() will be connected to this slot so
     * it gets called whenever a plugin was moved in the layout by the user.
     * @param plug
     */
    void pluginMoved(Plugin * plug);
private:
    /**
     * @brief The UKUIPanelLayout of this panel. All the Plugins will be added
     * to the UI via this layout.
     */
    UKUIPanelLayout* m_layout;
    /**
     * @brief The UKUi::Settings instance as retrieved from
     * UKUIPanelApplication.
     */
    UKUi::Settings *m_settings;
    /**
     * @brief The background widget for the panel. This background widget will
     * have the background color or the background image if any of these is
     * set. This background widget will have the UKUIPanelLayout mLayout which
     * will in turn contain all the Plugins.
     */
    QFrame *UKUIPanelWidget;
    /**
     * @brief The name of the panel which will also be used as an identifier
     * for config files.
     */
    QString m_configGroup;
    /**
     * @brief Pointer to the PanelPluginsModel which will store all the Plugins
     * that are loaded.
     */
    QScopedPointer<PanelPluginsModel> m_plugins;
    /**
     * @brief object for storing info if some standalone window is shown
     * (for preventing hide)
     */
    QScopedPointer<WindowNotifier> m_standaloneWindows;

    /**
     * @brief Returns the screen index of a screen on which this panel could
     * be placed at the given position. If possible, the current screen index
     * is preserved. So, if the panel can be placed on the current screen, the
     * index of that screen will be returned.
     * @param position position at which the panel should be placed.
     * @return The current screen index if the panel can be placed on the
     * current screen or the screen index of a screen that it can be placed on.
     *
     * \sa canPlacedOn(), mScreenNum, mActualScreenNum.
     */
    int findAvailableScreen(UKUIPanel::Position position);
    /**
     * @brief Update the window manager struts _NET_WM_PARTIAL_STRUT and
     * _NET_WM_STRUT for this widget. "The purpose of struts is to reserve
     * space at the borders of the desktop. This is very useful for a
     * docking area, a taskbar or a panel, for instance. The Window Manager
     * should take this reserved area into account when constraining window
     * positions - maximized windows, for example, should not cover that
     * area."
     * \sa http://standards.freedesktop.org/wm-spec/wm-spec-latest.html#NETWMSTRUT
     */
    void updateWmStrut();

    /**
     * @brief Loads the plugins, i.e. creates a new PanelPluginsModel.
     * Connects the signals and slots and adds all the plugins to the
     * layout.
     */
    void loadPlugins();
    void reloadPlugins(QString model);
    QStringList readConfig(QString model);
    void checkPlugins(QStringList list);
    void movePlugins(QStringList list);

    /**
     * @brief Calculates and sets the geometry (i.e. the position and the size
     * on the screen) of the panel. Considers alignment, position, if the panel
     * is hidden and if its geometry should be set with animation.
     * \param animate flag if showing/hiding the panel should be animated.
     */
    void setPanelGeometry(bool animate = false);
    /**
     * @brief Sets the contents margins of the panel according to its position
     * and hiddenness. All margins are zero for visible panels.
     */
    void setMargins();
    /**
     * @brief Calculates the height of the panel if it is horizontal or the
     * width if the panel is vertical. Considers if the panel is hidden and
     * ensures that the result is at least PANEL_MINIMUM_SIZE.
     * @return The height/width of the panel.
     */
    int getReserveDimension();

    /**
     * @brief Stores the size of the panel, i.e. the height of a horizontal
     * panel or the width of a vertical panel in pixels. If the panel is
     * hidden (which is achieved by making the panel very thin), this value
     * is unchanged. So this value stores the size of the non-hidden panel.
     *
     * \sa panelSize(), setPanelSize().
     */
    void styleAdjust();
    int m_panelSize;
    /**
     * @brief Stores the edge length of the panel icons in pixels.
     *
     * \sa IUKUIPanel::iconSize(), setIconSize().
     */
    int m_iconSize;
    /**
     * @brief Stores the number of lines/rows of the panel.
     *
     * \sa IUKUIPanel::lineCount(), setLineCount().
     */
    int m_lineCount;

    /**
     * @brief Stores the length of the panel, i.e. the width of a horizontal
     * panel or the height of a vertical panel. The unit of this value is
     * determined by mLengthInPercents.
     *
     * \sa mLengthInPercents
     */
    int m_length;
    /**
     * @brief Stores if mLength is stored in pixels or relative to the
     * screen size in percents. If true, the length is stored in percents,
     * otherwise in pixels.
     *
     * \sa mLength
     */
    bool m_lengthInPercents;

    /**
     * @brief Stores how this panel is aligned. The meaning of this value
     * differs for horizontal and vertical panels.
     *
     * \sa Alignment.
     */
    Alignment m_alignment;

    /**
     * @brief Stores the position where the panel is shown
     */
    IUKUIPanel::Position m_position;
    /**
     * @brief Returns the index of the screen on which this panel should be
     * shown. This is the user configured value which can differ from the
     * screen that the panel is actually shown on. If the panel can not be
     * shown on the configured screen, UKUIPanel will determine another
     * screen. The screen that the panel is actually shown on is stored in
     * mActualScreenNum.
     *
     * @return The index of the screen on which this panel should be shown.
     *
     * \sa mActualScreenNum, canPlacedOn(), findAvailableScreen().
     */
    int m_screenNum;
    /**
     * @brief screen that the panel is currently shown at (this could
     * differ from mScreenNum).
     *
     * \sa mScreenNum, canPlacedOn(), findAvailableScreen().
     */
    int m_actualScreenNum;
    /**
     * @brief QTimer for delayed saving of changed settings. In many cases,
     * instead of storing changes to disk immediately we start this timer.
     * If this timer times out, we store the changes to disk. This has the
     * advantage that we can store a couple of changes with only one write to
     * disk.
     *
     * \sa saveSettings()
     */
    QTimer mDelaySave;
    /**
     * @brief Stores if the panel is hidable, i.e. if the panel will be
     * hidden after the cursor has left the panel area.
     *
     * \sa mVisibleMargin, mHidden, mHideTimer, showPanel(), hidePanel(), hidePanelWork()
     */
    bool m_hidable;
    /**
     * @brief Stores if the hidable panel should have a visible margin.
     *
     * \sa mHidable, mHidden, mHideTimer, showPanel(), hidePanel(), hidePanelWork()
     */
    bool m_visibleMargin;
    /**
     * @brief Stores if the panel is currently hidden.
     *
     * \sa mHidable, mVisibleMargin, mHideTimer, showPanel(), hidePanel(), hidePanelWork()
     */
    bool m_hidden;
    /**
     * @brief QTimer for hiding the panel. When the cursor leaves the panel
     * area, this timer will be started. After this timer has timed out, the
     * panel will actually be hidden.
     *
     * \sa mHidable, mVisibleMargin, mHidden, showPanel(), hidePanel(), hidePanelWork()
     */
    QTimer m_hideTimer;
    /**
     * @brief Stores the duration of auto-hide animation.
     *
     * \sa mHidden, mHideTimer, showPanel(), hidePanel(), hidePanelWork()
     */
    int m_animationTime;
    /**
     * @brief The timer used for showing an auto-hiding panel wih delay.
     *
     * \sa showPanel()
     */
    QTimer m_showDelayTimer;

    QColor m_fontColor; //!< Font color that is used in the style sheet.
    /**
     * @brief Determines the opacity of the background color. The value
     * should be in the range from 0 to 100. This will not affect the opacity
     * of a background image.
     */
    int mOpacity;
    /*!
     * \brief Flag if the panel should reserve the space under it as not usable
     * for "normal" windows. Usable for not 100% wide/hight or hiddable panels,
     * if user wants maximized windows go under the panel.
     *
     * \sa updateWmStrut()
     */
    bool m_reserveSpace;

    /**
     * @brief Pointer to the current ConfigPanelDialog if there is any. Make
     * sure to test this pointer for validity because it is lazily loaded.
     */
//   QPointer<ConfigPanelDialog> mConfigDialog;
    //ConfigPanelDialog  *mConfigDialog;
    //QPointer<ConfigPanelWidget> mConfigWidget;

    /**
     * @brief The animation used for showing/hiding an auto-hiding panel.
     */
    QPropertyAnimation *m_animation;

    /**
     * @brief Flag for providing the configuration options in panel's context menu
     */
    bool m_lockPanel;
    double m_transparency;

    // settings should be kept private for security
    UKUi::Settings *settings() const { return m_settings; }

    QDBusInterface *m_cloudInterface;

    IUKUIPanel::Position m_oldPos;

    QMenu * m_menu;
    QAction * m_lockAction;
    QRect m_currentScreenRect;

    IUKUIPanel::Position areaDivid(QPoint globalpos);
    int m_moveLock = -1;

    void connectToServer();

private slots:
    void keyChangedSlot(const QString &key);
    void priScreenChanged(int x, int y, int width, int height);
    void setPanelHide(bool model);


public:
    QGSettings *m_gsettings;
    QGSettings *m_transparencyGsettings;
    QTimer *m_time;

    QDBusInterface *m_interface;

};


#endif // UKUIPANEL_H
