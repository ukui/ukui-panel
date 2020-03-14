/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Copyright: 2014 LXQt team
 * Authors:
 *  Luís Pereira <luis.artur.pereira@gmail.com>
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

#ifndef UKUISINGLEAPPLICATION_H
#define UKUISINGLEAPPLICATION_H

#include "ukuiapplication.h"

class QWidget;

namespace UKUi {

/*! \class SingleApplication
 *  \brief The SingleApplication class provides an single instance Application.
 *
 *  This class allows the user to create applications where only one instance
 *  is allowed to be running at an given time. If the user tries to launch
 *  another instance, the already running instance will be activated instead.
 *
 *  The user has to set the activation window with setActivationWindow. If it
 *  doesn't the second instance will quietly exit without activating the first
 *  instance window. In any case only one instance is allowed.
 *
 *  These classes depend on D-Bus and KF5::WindowSystem
 *
 *  \code
 *
 *  // Original code
 *  int main(int argc, char **argv)
 *  {
 *      UKUi::Application app(argc, argv);
 *
 *      MainWidget w;
 *      w.show();
 *
 *      return app.exec();
 *  }
 *
 *  // Using the library
 *  int main(int argc, char **argv)
 *  {
 *      UKUi::SingleApplication app(argc, argv);
 *
 *      MainWidget w;
 *      app.setActivationWindow(&w);
 *      w.show();
 *
 *      return app.exec();
 *  }
 *  \endcode
 *  \sa SingleApplication
 */

class UKUI_API SingleApplication : public Application {
    Q_OBJECT

public:
    /*!
     * \brief Options to control the D-Bus failure related application behaviour
     *
     * By default (ExitOnDBusFailure) if an instance can't connect to the D-Bus
     * session bus, that instance calls ::exit(1). Not even the first instance
     * will run. Connecting to the D-Bus session bus is an condition to
     * guarantee that only one instance will run.
     *
     * If an user wants to allow an application to run without D-Bus, it must
     * use the NoExitOnDBusFailure option.
     *
     * ExitOnDBusFailure is the default.
     */
    enum StartOptions {
        /** Exit if the connection to the D-Bus session bus fails.
          * It's the default
          */
        ExitOnDBusFailure,
        /** Don't exit if the connection to the D-Bus session bus fails.*/
        NoExitOnDBusFailure
    };
    Q_ENUM(StartOptions)

    /*!
     * \brief Construct a UKUi SingleApplication object.
     * \param argc standard argc as in QApplication
     * \param argv standard argv as in QApplication
     * \param options Control the on D-Bus failure application behaviour
     *
     * \sa StartOptions.
     */
    SingleApplication(int &argc, char **argv, StartOptions options = ExitOnDBusFailure);
    virtual ~SingleApplication();

    /*!
     * \brief Sets the activation window.
     * \param w activation window.
     *
     * Sets the activation window of this application to w. The activation
     * window is the widget that will be activated by \a activateWindow().
     *
     * \sa activationWindow() \sa activateWindow();
     */
    void setActivationWindow(QWidget *w);

    /*!
     * \brief Gets the current activation window.
     * \return The current activation window.
     *
     * \sa setActivationWindow();
     */
    QWidget *activationWindow() const;

public Q_SLOTS:
    /*!
     * \brief Activates this application activation window.
     *
     * Changes to the desktop where this applications is. It then de-minimizes,
     * raises and activates the application's activation window.
     * If no activation window has been set, this function does nothing.
     *
     * \sa setActivationWindow();
     */
    void activateWindow();

private:
    QWidget *mActivationWindow;
};

#if defined(ukuiSingleApp)
#undef ukuiSingleApp
#endif
#define ukuiSingleApp (static_cast<UKUi::SingleApplication *>(qApp))

} // namespace UKUi

#endif // UKUISINGLEAPPLICATION_H
