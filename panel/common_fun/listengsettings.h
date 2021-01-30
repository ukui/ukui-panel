#ifndef LISTENGSETTINGS_H
#define LISTENGSETTINGS_H
//Qt
#include <QObject>
#include <QGSettings>
#include <QVariant>
class ListenGsettings : public QObject
{
    Q_OBJECT
public:
    ListenGsettings();
    QGSettings *panel_gsettings;
Q_SIGNALS:
    void panelsizechanged(int panelsize);
    void iconsizechanged(int iconsize);
    void panelpositionchanged(int panelposition);
};

#endif // LISTENGSETTINGS_H
