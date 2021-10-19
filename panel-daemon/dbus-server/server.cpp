#include "server.h"
//Qt
#include <QDebug>
#include <QDBusMessage>

Server::Server(QObject *parent) :
    QObject(parent)
{
    mFileWatcher = new FileWatcher();
    connect(mFileWatcher,&FileWatcher::DesktopDeleteFile,this,&Server::DesktopFileDeleteSlot);
}


QString Server::WIDToDesktop(int id)
{

}

int Server::DesktopToWID(QString desktop)
{

}

void Server::DesktopFileDeleteSlot(QString path,QStringList deleteFile){
//    emit DesktopFileDelete(path,deleteFile);
}

//QString Server::TimeChanged()
//{

//}
