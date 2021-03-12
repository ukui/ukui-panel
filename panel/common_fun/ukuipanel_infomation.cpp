#include "ukuipanel_infomation.h"
//Qt
#include <QDebug>

#define PANEL_SETTINGS      "org.ukui.panel.settings"
#define PANEL_POSITION_KEY  "panelposition"
#define ICON_SIZE_KEY       "iconsize"
#define PANEL_SIZE_KEY      "panelsize"

UKuiPanelInformation::UKuiPanelInformation(QObject *parent) : QObject(parent)
{

}

QVariantList UKuiPanelInformation::GetPrimaryScreenGeometry()
{
//    QList <int>list;
    int r1=0;int r2=1 ; int r3= 1920; int r4=1080;
//    list.append(r1);
//    list.append(r2);
//    list.append(r3);
//    list.append(r4);
//    return list;

    QVariantList vlist;
    vlist<<r1<<r2<<r3<<r4;
    return vlist;
}

QString UKuiPanelInformation::GetPanelPosition()
{
    qDebug()<<"GetSearchResult";
    QString str="bottom";
    return str;
}
