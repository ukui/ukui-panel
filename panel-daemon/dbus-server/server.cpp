#include "server.h"
//Qt
#include <QDebug>
#include <QDBusMessage>
#include "convert-desktop-windowid/convertdesktoptowinid.h"


Server::Server(QObject *parent) :
    QObject(parent)
{
    mFileWatcher = new FileWatcher();
    connect(mFileWatcher,&FileWatcher::DesktopDeleteFile,this,&Server::DesktopFileDeleteSlot);
    mDesktop = new ConvertDesktopToWinId();
}


QString Server::WIDToDesktop(int id)
{
    return mDesktop->tranIdToDesktop(id);
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
