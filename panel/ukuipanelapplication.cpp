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


#include <signal.h>

#include "ukuipanelapplication.h"
#include "ukuipanelapplication_p.h"
#include "ukuipanel.h"
//#include <UKUi/Settings>
#include "common/ukuisettings.h"
#include <QtDebug>
#include <QUuid>
#include <QScreen>
#include <QWindow>
#include <KWindowEffects>
#include <QCommandLineParser>
#include <QFile>

#define CONFIG_FILE_BACKUP     "/usr/share/ukui/panel.conf"
#define CONFIG_FILE_LOCAL      ".config/ukui/panel.conf"

UKUIPanelApplicationPrivate::UKUIPanelApplicationPrivate(UKUIPanelApplication *q)
    : mSettings(0),
      q_ptr(q)
{
}


UKUIPanelApplication::UKUIPanelApplication(int& argc, char** argv)
    : QApplication(argc, argv, true),
    d_ptr(new UKUIPanelApplicationPrivate(this))

{
    translator();
    // bind to SIGTERM siganl to exit with code 15
    signal(SIGTERM, sigtermHandler);

    Q_D(UKUIPanelApplication);

    QCoreApplication::setApplicationName(QLatin1String("ukui-panel"));
    const QString VERINFO = QStringLiteral(UKUI_PANEL_VERSION
                                           "\nlibukui   " ""
                                           "\nQt        " QT_VERSION_STR);

    QCoreApplication::setApplicationVersion(VERINFO);

    QCommandLineParser parser;
    parser.setApplicationDescription(QLatin1String("UKUi Panel"));
    parser.addHelpOption();
    parser.addVersionOption();

    //添加其他参数
    QCommandLineOption configFileOption(QStringList()
            << QLatin1String("c") << QLatin1String("config") << QLatin1String("configfile"),
            QCoreApplication::translate("main", "Use alternate configuration file."),
            QCoreApplication::translate("main", "Configuration file"));
    parser.addOption(configFileOption);

    QCommandLineOption panelResetOption(QStringList()
            << QLatin1String("r") << QLatin1String("reset") << QLatin1String("panel reset"),
            QCoreApplication::translate("main", "ukui-panel set mode "),
            QCoreApplication::translate("main", "panel set option"));
    parser.addOption(panelResetOption);

    parser.process(*this);

    const QString configFile = parser.value(configFileOption);
    if (configFile.isEmpty())
    {
        QString defaultConf = QString(PLUGIN_DESKTOPS_DIR)+"/../";
        QString loaclCong = QString(qgetenv("HOME"))+"/.config/ukui/";
        QFile file(loaclCong+"panel.conf");
        if(!file.exists()){
            copyFileToPath(defaultConf,loaclCong,"panel.conf",false);
//            QFile::copy(CONFIG_FILE_BACKUP,QString(qgetenv("HOME"))+CONFIG_FILE_LOCAL);
        }
        d->mSettings = new UKUi::Settings(QLatin1String("panel"), this);
        if(!d->mSettings->contains("plugins")){
            QFile::remove(QString(qgetenv("HOME"))+CONFIG_FILE_LOCAL);
            QFile::copy(CONFIG_FILE_BACKUP,QString(qgetenv("HOME"))+CONFIG_FILE_LOCAL);
            d->mSettings = new UKUi::Settings(QLatin1String("panel"), this);
        }
    }
    else
    {
        qDebug()<<"configFile.is not Empty"<<endl;
        d->mSettings = new UKUi::Settings(configFile, QSettings::IniFormat, this);
    }

    const QString panelReset = parser.value(panelResetOption);
    if(panelReset.isEmpty()){qDebug()<<"ukui-panel --reset";}
    if(panelReset == "reset"){system("rm $HOME/.config/ukui/panel.conf");}
    if(panelReset == "replace"){qDebug()<<"ukui-panel --replace";}
    if(panelReset == "calendar-new"){system("/usr/share/ukui/ukui-panel/ukui-panel-config.sh calendar new && killall ukui-panel");}
    if(panelReset == "calendar-old"){system("/usr/share/ukui/ukui-panel/ukui-panel-config.sh calendar old && killall ukui-panel");}

    // This is a workaround for Qt 5 bug #40681.
    const auto allScreens = screens();
    for(QScreen* screen : allScreens)
    {
        connect(screen, &QScreen::destroyed, this, &UKUIPanelApplication::screenDestroyed);
    }
    connect(this, &QGuiApplication::screenAdded, this, &UKUIPanelApplication::handleScreenAdded);
    connect(this, &QCoreApplication::aboutToQuit, this, &UKUIPanelApplication::cleanup);


    QStringList panels = d->mSettings->value("panels").toStringList();

    // WARNING: Giving a separate icon theme to the panel is wrong and has side effects.
    // However, it is optional and can be used as the last resort for avoiding a low
    // contrast in the case of symbolic SVG icons. (The correct way of doing that is
    // using a Qt widget style that can assign a separate theme/QPalette to the panel.)
    m_globalIconTheme = QIcon::themeName();
    const QString iconTheme = d->mSettings->value("iconTheme").toString();
    if (!iconTheme.isEmpty())
        QIcon::setThemeName(iconTheme);

    if (panels.isEmpty())
    {
        panels << "panel1";
    }

#if (QT_VERSION < QT_VERSION_CHECK(5,7,0))
    for(int it=0;it<panels.size();it++){
        const QString &i=panels[it];
#endif
#if (QT_VERSION >= QT_VERSION_CHECK(5,7,0))
    for(const QString& i : qAsConst(panels)){
#endif

        addPanel(i);
    }
}

UKUIPanelApplication::~UKUIPanelApplication()
{
    delete d_ptr;
}

void UKUIPanelApplication::cleanup()
{
    qDeleteAll(m_panels);
}


UKUIPanel* UKUIPanelApplication::addPanel(const QString& name)
{
    Q_D(UKUIPanelApplication);

    UKUIPanel *panel = new UKUIPanel(name, d->mSettings);
    KWindowEffects::enableBlurBehind(panel->winId(),true);
    m_panels << panel;

    // reemit signals
    connect(panel, &UKUIPanel::pluginAdded, this, &UKUIPanelApplication::pluginAdded);
    connect(panel, &UKUIPanel::pluginRemoved, this, &UKUIPanelApplication::pluginRemoved);

    return panel;
}

void UKUIPanelApplication::handleScreenAdded(QScreen* newScreen)
{
    // qDebug() << "UKUIPanelApplication::handleScreenAdded" << newScreen;
    connect(newScreen, &QScreen::destroyed, this, &UKUIPanelApplication::screenDestroyed);
}


void UKUIPanelApplication::screenDestroyed(QObject* screenObj)
{
    // NOTE by PCMan: This is a workaround for Qt 5 bug #40681.
    // With this very dirty workaround, we can fix ukui/ukui bug #204, #205, and #206.
    // Qt 5 has two new regression bugs which breaks ukui-panel in a multihead environment.
    // #40681: Regression bug: QWidget::winId() returns old value and QEvent::WinIdChange event is not emitted sometimes. (multihead setup)
    // #40791: Regression: QPlatformWindow, QWindow, and QWidget::winId() are out of sync.
    // Explanations for the workaround:
    // Internally, Qt mantains a list of QScreens and update it when XRandR configuration changes.
    // When the user turn off an monitor with xrandr --output <xxx> --off, this will destroy the QScreen
    // object which represent the output. If the QScreen being destroyed contains our panel widget,
    // Qt will call QWindow::setScreen(0) on the internal windowHandle() of our panel widget to move it
    // to the primary screen. However, moving a window to a different screen is more than just changing
    // its position. With XRandR, all screens are actually part of the same virtual desktop. However,
    // this is not the case in other setups, such as Xinerama and moving a window to another screen is
    // not possible unless you destroy the widget and create it again for a new screen.
    // Therefore, Qt destroy the widget and re-create it when moving our panel to a new screen.
    // Unfortunately, destroying the window also destroy the child windows embedded into it,
    // using XEMBED such as the tray icons. (#206)
    // Second, when the window is re-created, the winId of the QWidget is changed, but Qt failed to
    // generate QEvent::WinIdChange event so we have no way to know that. We have to set
    // some X11 window properties using the native winId() to make it a dock, but this stop working
    // because we cannot get the correct winId(), so this causes #204 and #205.
    //
    // The workaround is very simple. Just completely destroy the panel before Qt has a chance to do
    // QWindow::setScreen() for it. Later, we reload the panel ourselves. So this can bypassing the Qt bugs.
    QScreen* screen = static_cast<QScreen*>(screenObj);
    bool reloadNeeded = false;
    qApp->setQuitOnLastWindowClosed(false);
#if (QT_VERSION < QT_VERSION_CHECK(5,7,0))
    for(int i=0;i<m_panels.size();i++){
        UKUIPanel *panel=m_panels[i];
#endif
#if (QT_VERSION >= QT_VERSION_CHECK(5,7,0))
    for(UKUIPanel* panel : qAsConst(m_panels)){
#endif
        QWindow* panelWindow = panel->windowHandle();
        if(panelWindow && panelWindow->screen() == screen)
        {
            // the screen containing the panel is destroyed
            // delete and then re-create the panel ourselves
            QString name = panel->name();
            panel->saveSettings(false);
            delete panel; // delete the panel, so Qt does not have a chance to set a new screen to it.
            m_panels.removeAll(panel);
            reloadNeeded = true;
            qDebug() << "Workaround Qt 5 bug #40681: delete panel:" << name;
        }
    }
    if(reloadNeeded)
        QTimer::singleShot(1000, this, SLOT(reloadPanelsAsNeeded()));
    else
        qApp->setQuitOnLastWindowClosed(true);
}

bool UKUIPanelApplication::isPluginSingletonAndRunnig(QString const & pluginId) const
{
    for (auto const & panel : m_panels)
        if (panel->isPluginSingletonAndRunnig(pluginId))
            return true;

    return false;
}

void UKUIPanelApplication::sigtermHandler(int signo)
{
    qDebug() << "Caught SIGTERM signal, exit with SIGTERM";
    exit(signo);
}

void UKUIPanelApplication::translator(){
    m_translator = new QTranslator(this);
     QString locale = QLocale::system().name();
     if (locale == "zh_CN"){
         if (m_translator->load(QM_INSTALL))
             qApp->installTranslator(m_translator);
         else
             qDebug() <<PLUGINNAME<<"Load translations file" << locale << "failed!";
     }
}

bool UKUIPanelApplication::copyFileToPath(QString sourceDir ,QString toDir, QString fileName, bool coverFileIfExist)
{
    if (sourceDir == toDir){
        return true;
    }
    if (!QFile::exists(sourceDir+fileName)){
        return false;
    }
    QDir *createDir = new QDir;
    bool dirExist = createDir->exists(toDir);
    if(!dirExist)
        createDir->mkdir(toDir);
    QFile *createFile = new QFile;
    bool fileExist = createFile->exists(toDir+fileName);
    if (fileExist){
        if(coverFileIfExist){
            createFile->remove(toDir+fileName);
        }
    }//end if
    free(createDir);
    free(createFile);

    if(!QFile::copy(sourceDir+fileName, toDir+fileName))
    {
        return false;
    }
    return true;
}
