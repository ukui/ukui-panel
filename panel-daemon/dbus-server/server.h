#ifndef SERVER_H
#define SERVER_H
//Qt
#include <QObject>
#include <QGSettings>
#include <QVariant>
#include <QtDBus>
class Server : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface","org.ukui.panel.deamon")
public:
    explicit Server(QObject *parent = 0);

public Q_SLOTS:
	int DesktopToWID(QString desktop);
	QString WIDToDesktop(int id);
};

#endif // UKUIPANEL_INFORMATION_H
