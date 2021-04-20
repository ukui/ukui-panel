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

#include <QDir>

#include "ukuiapplication.h"
#include "ukuisettings.h"

#include <XdgDirs>

using namespace UKUi;

#define COLOR_DEBUG "\033[32;2m"
#define COLOR_WARN "\033[33;2m"
#define COLOR_CRITICAL "\033[31;1m"
#define COLOR_FATAL "\033[33;1m"
#define COLOR_RESET "\033[0m"

#define QAPP_NAME qApp ? qApp->objectName().toUtf8().constData() : ""

#include <cstdio>
#include <unistd.h>
#include <cstring>
#include <csignal>
#include <cerrno>
#include <sys/socket.h>
#include <QDateTime>
#include <QDebug>
#include <QSocketNotifier>
/*! \brief Log qDebug input to file
Used only in pure Debug builds or when is the system environment
variable UKUI_DEBUG set
*/
void dbgMessageOutput(QtMsgType type, const QMessageLogContext &ctx, const QString & msgStr)
{
    Q_UNUSED(ctx)
    QByteArray msgBuf = msgStr.toUtf8();
    const char* msg = msgBuf.constData();
    QDir dir(XdgDirs::configHome() + QLatin1String("/ukui"));
    dir.mkpath(QL1S("."));

    const char* typestr;
    const char* color;
    switch (type) {
    case QtDebugMsg:
        typestr = "Debug";
        color = COLOR_DEBUG;
        break;
    case QtWarningMsg:
        typestr = "Warning";
        color = COLOR_WARN;
        break;
    case QtFatalMsg:
        typestr = "Fatal";
        color = COLOR_FATAL;
        break;
    default: // QtCriticalMsg
        typestr = "Critical";
        color = COLOR_CRITICAL;
    }

    QByteArray dt = QDateTime::currentDateTime().toString(QL1S("yyyy-MM-dd hh:mm:ss.zzz")).toUtf8();
    if (isatty(STDERR_FILENO))
        fprintf(stderr, "%s %s(%p) %s: %s%s\n", color, QAPP_NAME, static_cast<void *>(qApp), typestr, msg, COLOR_RESET);
    else
        fprintf(stderr, "%s(%p) %s: %s\n", QAPP_NAME, static_cast<void *>(qApp), typestr, msg);

    FILE *f = fopen(dir.absoluteFilePath(QL1S("debug.log")).toUtf8().constData(), "a+");
    fprintf(f, "%s %s(%p) %s: %s\n", dt.constData(), QAPP_NAME, static_cast<void *>(qApp), typestr, msg);
    fclose(f);

    if (type == QtFatalMsg)
        abort();
}

Application::Application(int &argc, char** argv)
    : QApplication(argc, argv)
{
    setWindowIcon(QIcon(QFile::decodeName(UKUI_GRAPHICS_DIR) + QL1S("/ukui_logo.png")));
    connect(Settings::globalSettings(), &GlobalSettings::ukuiThemeChanged, this, &Application::updateTheme);
    updateTheme();
}

Application::Application(int &argc, char** argv, bool handleQuitSignals)
    : Application(argc, argv)
{
    if (handleQuitSignals)
    {
        QList<int> signo_list = {SIGINT, SIGTERM, SIGHUP};
        connect(this, &Application::unixSignal, [this, signo_list] (int signo)
            {
                if (signo_list.contains(signo))
                    quit();
            });
        listenToUnixSignals(signo_list);
    }
}

void Application::updateTheme()
{
    const QString styleSheetKey = QFileInfo(applicationFilePath()).fileName();
    setStyleSheet(ukuiTheme.qss(styleSheetKey));
    emit themeChanged();
}

namespace
{
    class SignalHandler
    {
    public:
        static void signalHandler(int signo)
        {
            const int ret = write(instance->mSignalSock[0], &signo, sizeof (int));
            if (sizeof (int) != ret)
                qCritical("unable to write into socketpair: %s", strerror(errno));
        } 

    public:
        template <class Lambda>
        SignalHandler(Application * app, Lambda signalEmitter)
            : mSignalSock{-1, -1}
        {
            if (0 != socketpair(AF_UNIX, SOCK_STREAM, 0, mSignalSock))
            {
                qCritical("unable to create socketpair for correct signal handling: %s", strerror(errno));
                return;
            }

            mNotifier.reset(new QSocketNotifier(mSignalSock[1], QSocketNotifier::Read));
            QObject::connect(mNotifier.data(), &QSocketNotifier::activated, app, [this, signalEmitter] {
                int signo = 0;
                int ret = read(mSignalSock[1], &signo, sizeof (int));
                if (sizeof (int) != ret)
                qCritical("unable to read signal from socketpair, %s", strerror(errno));
                signalEmitter(signo);
            });
        }

        ~SignalHandler()
        {
            close(mSignalSock[0]);
            close(mSignalSock[1]);
        }

        void listenToSignals(QList<int> const & signoList)
        {
            struct sigaction sa;
            sa.sa_handler = signalHandler;
            sigemptyset(&sa.sa_mask);
            sa.sa_flags = 0;
            for (auto const & signo : signoList)
                sigaction(signo, &sa, nullptr);
        }

    public:
        static QScopedPointer<SignalHandler> instance;

    private:
        int mSignalSock[2];
        QScopedPointer<QSocketNotifier> mNotifier;
    };

    QScopedPointer<SignalHandler> SignalHandler::instance;
}

void Application::listenToUnixSignals(QList<int> const & signoList)
{
    static QScopedPointer<QSocketNotifier> signal_notifier;

    if (SignalHandler::instance.isNull())
        SignalHandler::instance.reset(new SignalHandler{this, [this] (int signo) { emit unixSignal(signo); }});
    SignalHandler::instance->listenToSignals(signoList);
}
