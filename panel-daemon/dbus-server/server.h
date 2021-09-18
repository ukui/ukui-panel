#ifndef _SERVER_H
#define _SERVER_H
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
	//desktop 文件路径转换为wid
	int DesktopToWID(QString desktop);
	//window Id 转化为desktop文件
	QString WIDToDesktop(int id);

Q_SIGNALS:
//	//desktop文件被删除
//    QString DesktopFileDelete();

//	//时间改变
//    QString TimeChanged();

};

#endif // UKUIPANEL_INFORMATION_H
