#ifndef UKUIPANEL_INFORMATION_H
#define UKUIPANEL_INFORMATION_H
//Qt
#include <QObject>
#include <QGSettings>
#include <QVariant>
class UKuiPanelInformation : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface","org.ukui.panel")
public:
    explicit UKuiPanelInformation(QObject *parent = 0);

public Q_SLOTS:
    QVariantList GetPrimaryScreenGeometry();
    QString GetPanelPosition();
};

#endif // UKUIPANEL_INFORMATION_H
