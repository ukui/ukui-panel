#ifndef UKUIPANEL_INFORMATION_H
#define UKUIPANEL_INFORMATION_H
//Qt
#include <QObject>
#include <QGSettings>
#include <QVariant>
#include <QtDBus>
class UKuiPanelInformation : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface","org.ukui.panel")
public:
    explicit UKuiPanelInformation(QObject *parent = 0);

private:
    int screen_x;
    int screen_y;
    int screen_width;
    int screen_height;
    int panelposition;
    int panelsize;

public Q_SLOTS:
    QVariantList GetPrimaryScreenGeometry();
    QVariantList GetPrimaryScreenAvailableGeometry();
    QVariantList GetPrimaryScreenPhysicalGeometry();
    QString GetPanelPosition();

    void setPanelInformation(int ,int ,int, int, int, int);
};

#endif // UKUIPANEL_INFORMATION_H
