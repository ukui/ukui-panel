#ifndef STARTMENU_H
#define STARTMENU_H

#include "../panel/iukuipanelplugin.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QFrame>
#include <QFontMetrics>
#include <QLineEdit>
#include <QToolButton>
#include <XdgIcon>


#include <QMainWindow>
#include <QHBoxLayout>
#include <QDebug>
#include <QProcess>

#include "../panel/plugin.h"
#include "../panel/ukuipanel.h"
//#include "../panel/config/configpaneldialog.h"
#include "../panel/config/configpanelwidget.h"
#include <QMenu>
#define DEFAULT_SHORTCUT "Alt+F1"


class Segmentation : public QObject, public IUKUIPanelPlugin
{
    Q_OBJECT
public:
    Segmentation(const IUKUIPanelPluginStartupInfo &startupInfo);
    ~Segmentation();

    virtual QWidget *widget() { return &mButtonx; }
    virtual QString themeId() const { return QStringLiteral("startmenu"); }
    void realign();
    virtual IUKUIPanelPlugin::Flags flags() const { return PreferRightAlignment | HaveConfigDialog ; }
private:
    //StartMenuWidget mWidget;
    QWidget mWidget;
    IUKUIPanelPlugin *mPlugin;
    QToolButton mButtonx;



};

class StartMenuLibrary: public QObject, public IUKUIPanelPluginLibrary
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "lxqt.org/Panel/PluginInterface/3.0")
    Q_INTERFACES(IUKUIPanelPluginLibrary)
public:
    IUKUIPanelPlugin *instance(const IUKUIPanelPluginStartupInfo &startupInfo) const
    {
        return new Segmentation(startupInfo);
    }
};

#endif
