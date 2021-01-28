#include "listengsettings.h"
//Qt
#include <QDebug>

#define PANEL_SETTINGS      "org.ukui.panel.settings"
#define PANEL_POSITION_KEY  "panelposition"
#define ICON_SIZE_KEY       "iconsize"
#define PANEL_SIZE_KEY      "panelsize"

ListenGsettings::ListenGsettings()
{
    const QByteArray id(PANEL_SETTINGS);
    panel_gsettings = new QGSettings(id);
    QObject::connect(panel_gsettings, &QGSettings::changed, this, [=] (const QString &key){
        if(key == PANEL_POSITION_KEY){
            emit panelpositionchanged(panel_gsettings->get(PANEL_POSITION_KEY).toInt());
        }
        if(key == ICON_SIZE_KEY){
            emit iconsizechanged(panel_gsettings->get(ICON_SIZE_KEY).toInt());
        }
        if(key == PANEL_SIZE_KEY){
            emit panelsizechanged(panel_gsettings->get(PANEL_SIZE_KEY).toInt());
        }
    });
}
