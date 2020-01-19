/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Copyright: 2012-2013 Razor team
 * Authors:
 *   Petr Vanek <petr@scribus.info>
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

#ifndef UKUIAPPLICATION_H
#define UKUIAPPLICATION_H

#include <QApplication>
#include <QProxyStyle>
#include "ukuiglobals.h"

namespace UKUi
{

/*! \brief UKUi wrapper around QApplication.
 * It loads various UKUi related stuff by default (window icon, icon theme...)
 *
 * \note This wrapper is intended to be used only inside UKUi project. Using it
 *       in external application will automatically require linking to various
 *       UKUi libraries.
 *
 */
class UKUI_API Application : public QApplication
{
    Q_OBJECT

public:
    /*! Construct a UKUi application object.
     * \param argc standard argc as in QApplication
     * \param argv standard argv as in QApplication
     */
    Application(int &argc, char **argv);
    /*! Construct a UKUi application object.
     * \param argc standard argc as in QApplication
     * \param argv standard argv as in QApplication
     * \param handleQuitSignals flag if signals SIGINT, SIGTERM, SIGHUP should be handled internaly (\sa quit() application)
     */
    Application(int &argc, char **argv, bool handleQuitSignals);
    virtual ~Application() {}
    /*! Install UNIX signal handler for signals defined in \param signalList
     * Upon receiving of any of this signals the \sa unixSignal signal is emitted
     */
    void listenToUnixSignals(QList<int> const & signolList);

private slots:
    void updateTheme();

signals:
    void themeChanged();
    /*! Signal is emitted upon receival of registered unix signal
     * \param signo the received unix signal number
     */
    void unixSignal(int signo);
};

#if defined(ukuiApp)
#undef ukuiApp
#endif
#define ukuiApp (static_cast<UKUi::Application *>(qApp))

} // namespace UKUi
#endif // UKUIAPPLICATION_H
