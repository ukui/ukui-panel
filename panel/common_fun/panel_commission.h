#ifndef PANEL_COMMISSION_H
#define PANEL_COMMISSION_H

//Qt
#include <QObject>
#include <QGSettings>
#include <QVariant>
class PanelCommission : public QObject
{
    Q_OBJECT
public:
    PanelCommission();
    ~PanelCommission();

    static void panelConfigFileReset(bool reset);
    static void panelConfigFileValueInit(bool set);
};


#endif
