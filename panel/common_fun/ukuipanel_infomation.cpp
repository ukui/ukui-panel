#include "ukuipanel_infomation.h"
//Qt
#include <QDebug>

#define PANEL_SETTINGS      "org.ukui.panel.settings"
#define PANEL_POSITION_KEY  "panelposition"
#define ICON_SIZE_KEY       "iconsize"
#define PANEL_SIZE_KEY      "panelsize"

UKuiPanelInformation::UKuiPanelInformation(QObject *parent) : QObject(parent)
{
    qDebug()<<"UKuiPanelInformation&&&&&&&&&&&&&&&&&&&&&&&&&&&&&";
    QDBusConnection::sessionBus().connect(QString(), QString( "/panel/position"),
                                          "org.ukui.panel",
                                          "UKuiPanelPosition",
                                          this,
                                          SLOT(setPanelInformation(int,int,int,int,int,int))
                                          );
}

void UKuiPanelInformation::setPanelInformation(int x, int y, int width, int height, int size, int position)
{
    qDebug()<<"UKuiPanelInformation::setPanelInformation  &&&&&&&&&&&&";
    screen_x=x;
    screen_y=y;
    screen_width=width;
    screen_height=height;
    panelsize=size;
    panelposition=position;
}
QVariantList UKuiPanelInformation::GetPrimaryScreenGeometry()
{
    int available_primary_screen_x;int available_primary_screen_y ; int available_primary_screen_width; int available_primary_screen_height;int available_panel_position;
    QVariantList vlist;
    switch(panelposition){
    case 0:
        available_primary_screen_x=screen_x;
        available_primary_screen_y=screen_y;
        available_primary_screen_width=screen_width;
        available_primary_screen_height=screen_height-panelsize;
        break;
    case 1:
        available_primary_screen_x=screen_x;
        available_primary_screen_y=screen_y+panelsize;
        available_primary_screen_width=screen_width;
        available_primary_screen_height=screen_height-panelsize;
        break;
    case 2:
        available_primary_screen_x=screen_x + panelsize;
        available_primary_screen_y=screen_y;
        available_primary_screen_width=screen_width-panelposition;
        available_primary_screen_height=screen_height;
        break;
    case 3:
        available_primary_screen_x=screen_x;
        available_primary_screen_y=screen_y;
        available_primary_screen_width=screen_width-panelposition;
        available_primary_screen_height=screen_height;
        break;
    default:
        available_primary_screen_x=screen_x;
        available_primary_screen_y=screen_y;
        available_primary_screen_width=screen_width;
        available_primary_screen_height=screen_height-panelsize;
        break;
    }

    qDebug()<<"list*************************"<<vlist;
//    available_primary_screen_x=0;
//    available_primary_screen_y=0;
//    available_primary_screen_width=0;
//    available_primary_screen_height=0;
    vlist<<available_primary_screen_x<<available_primary_screen_y<<available_primary_screen_width<<available_primary_screen_height<<panelposition;
    qDebug()<<"list*************************"<<vlist;
    return vlist;
}

QString UKuiPanelInformation::GetPanelPosition()
{
    qDebug()<<"GetSearchResult";
    QString str="bottom";
    return str;
}
