#include "convertdesktoptowinid.h"
#include <KWindowSystem>
#include <QFile>
#include <QDebug>
#include <QDir>


ConvertDesktopToWinId::ConvertDesktopToWinId()
{
    connect(KWindowSystem::self(), &KWindowSystem::windowAdded, this, &ConvertDesktopToWinId::onWindowAdded);
}

QString ConvertDesktopToWinId::onWindowAdded(WId windowId)
{
    KWindowInfo info(windowId, 0, NET::WM2AllProperties);
    QString desktopName = confirmDesktopFileName(info);
    qDebug()<<"desktopName  is  :"<<desktopName;
    return desktopName;
}

QString ConvertDesktopToWinId::tranIdToDesktop(WId id)
{
    KWindowInfo info(id, 0, NET::WM2AllProperties);
    QString desktopName=confirmDesktopFileName(info);
    qDebug()<<"desktopName  is  :" << desktopName;
    return desktopName;
}
QString ConvertDesktopToWinId::confirmDesktopFileName(KWindowInfo info)
{
    desktop_file_path = "";

    dir = new QDir(DEKSTOP_FILE_PATH);
    list = dir->entryInfoList();
    //跳过 ./ 和 ../ 目录
    list.removeAll(QFile(USR_SHARE_APP_CURRENT));
    list.removeAll(QFile(USR_SHARE_APP_UPER));

    //第一种方法：或许点击应用时大部分desktop文件名
    searchFromEnviron(info);

    //第二种方法：比较名字一致性
    if (desktop_file_path.isEmpty()) {
        classClass = info.windowClassClass().toLower();
        className = info.windowClassName();

        QFile file(QString("/proc/%1/status").arg(info.pid()));
        if (file.open(QIODevice::ReadOnly)) {
            char buf[1024];
            qint64 len=file.readLine(buf,sizeof(buf));
            if (len!=-1) {
                statusName = QString::fromLocal8Bit(buf).remove("Name:").remove("\t").remove("\n");
            }
        }

        compareClassName(); // 40/59
    }

    //第三种方法：比较cmd命令行操作一致性
    if (desktop_file_path.isEmpty()) {
        QFile file(QString("/proc/%1/cmdline").arg(info.pid()));
        if (file.open(QIODevice::ReadOnly)) {
            char buf[1024];
            qint64 len=file.readLine(buf,sizeof(buf));
            if (len!=-1) {
                cmd_line = QString::fromLocal8Bit(buf).remove("\n");
            }
        }
        compareCmdExec();
    }

    //第四种方法：匹配部分字段
    //目前主要用来解决WPS应用的匹配
    if (desktop_file_path.isEmpty()) {
        compareLastStrategy();
    }
    //返回desktop文件名
    return desktop_file_path;
}

void ConvertDesktopToWinId::searchFromEnviron(KWindowInfo info)
{
    QFile file("/proc/" + QString::number(info.pid()) + "/environ");
    file.open(QIODevice::ReadOnly);
    QByteArray BA = file.readAll();
    file.close();
    QList<QByteArray> list_BA = BA.split('\0');
    for (int i = 0; i < list_BA.length(); i++) {
        if (list_BA.at(i).startsWith("GIO_LAUNCHED_DESKTOP_FILE=")) {
            desktop_file_path = list_BA.at(i);
            desktop_file_path = desktop_file_path.mid(desktop_file_path.indexOf("=") + 1);
            //desktop文件地址需要重写
            desktop_file_path = desktop_file_path.mid(desktop_file_path.lastIndexOf("/") + 1);
        }
    }
    //desktop文件地址重写
    if (!desktop_file_path.isEmpty()) {
        for (int i = 0; i < list.size(); i++) {
            QFileInfo fileInfo = list.at(i);;
            if (fileInfo.filePath() == DEKSTOP_FILE_PATH + desktop_file_path) {
                desktop_file_path = fileInfo.filePath();
            }
        }
    }

}

void ConvertDesktopToWinId::compareClassName()
{
    for (int i = 0; i < list.size(); i++) {
        QFileInfo fileInfo = list.at(i);;
        QString path_desktop_name = fileInfo.filePath();
        if (!fileInfo.filePath().endsWith(".desktop")) {
            continue;
        }
        path_desktop_name = path_desktop_name.mid(path_desktop_name.lastIndexOf("/") + 1);
        path_desktop_name = path_desktop_name.left(path_desktop_name.lastIndexOf("."));
        if (path_desktop_name == classClass || path_desktop_name == className || path_desktop_name == statusName)  {
            desktop_file_path = fileInfo.filePath();
        }
    }
}

void ConvertDesktopToWinId::compareCmdExec()
{
    for (int i = 0; i < list.size(); i++) {
        QString _cmd;
        QFileInfo fileInfo = list.at(i);
        if (!fileInfo.filePath().endsWith(".desktop")) {
            continue;
        }
        _cmd.sprintf(GET_DESKTOP_EXEC_NAME_MAIN, fileInfo.filePath().toStdString().data());
        QString desktopFileExeName = getDesktopFileName(_cmd).remove("\n");

        if (desktopFileExeName.isEmpty()) {
            continue;
        }

        if (desktopFileExeName == cmd_line || desktopFileExeName.startsWith(cmd_line) || cmd_line.startsWith(desktopFileExeName)) {
            desktop_file_path = fileInfo.filePath();
        }

        //仅仅是为了适配微信
        if (desktop_file_path.isEmpty()) {
            desktopFileExeName = "/usr/lib/" + desktopFileExeName;
            if (desktopFileExeName == cmd_line || desktopFileExeName.startsWith(cmd_line) || cmd_line.startsWith(desktopFileExeName)) {
                desktop_file_path = fileInfo.filePath();
            }
        }
    }
}

//最后的匹配策略汇总
void ConvertDesktopToWinId::compareLastStrategy()
{
    for (int i = 0; i < list.size(); i++) {
        QString _cmd;
        QFileInfo fileInfo = list.at(i);
        if (!fileInfo.filePath().endsWith(".desktop")) {
            continue;
        }
        _cmd.sprintf(GET_DESKTOP_EXEC_NAME_MAIN, fileInfo.filePath().toStdString().data());
        QString desktopFileExeName = getDesktopFileName(_cmd).remove("\n");

        if (desktopFileExeName.isEmpty()) {
            continue;
        }

        if (desktopFileExeName.startsWith(className) || desktopFileExeName.endsWith(className)) {
            desktop_file_path = fileInfo.filePath();
        }
    }

    if (desktop_file_path.isEmpty()) {
        for (int i = 0; i < list.size(); i++) {
            QFileInfo fileInfo = list.at(i);
            QString path_desktop_name = fileInfo.filePath();
            if (!fileInfo.filePath().endsWith(".desktop")) {
                continue;
            }
            path_desktop_name = path_desktop_name.mid(path_desktop_name.lastIndexOf("/") + 1);
            path_desktop_name = path_desktop_name.left(path_desktop_name.lastIndexOf("."));

            if (path_desktop_name.startsWith(className) || path_desktop_name.endsWith(className)) {
                desktop_file_path = fileInfo.filePath();
            }
            else if (className.startsWith(path_desktop_name) || className.endsWith(path_desktop_name)) {
                desktop_file_path = fileInfo.filePath();
            }
        }
    }

    if (desktop_file_path.isEmpty()) {
        for (int i = 0; i < list.size(); i++) {
            QString _cmd;
            QFileInfo fileInfo = list.at(i);
            QString path_desktop_name = fileInfo.filePath();

            if (!fileInfo.filePath().endsWith(".desktop")) {
                continue;
            }

            _cmd.sprintf(GET_DESKTOP_EXEC_NAME_MAIN, fileInfo.filePath().toStdString().data());
            QString desktopFileExeName = getDesktopFileName(_cmd).remove("\n");

            path_desktop_name = path_desktop_name.mid(path_desktop_name.lastIndexOf("/") + 1);
            path_desktop_name = path_desktop_name.left(path_desktop_name.lastIndexOf("."));

            if (path_desktop_name.contains(className) || desktopFileExeName.contains(className)) {
                desktop_file_path = fileInfo.filePath();
            }
        }
    }
}

//执行头文件中宏定义写好的终端指令获取对应的Exec字段
QString ConvertDesktopToWinId::getDesktopFileName(QString cmd)
{
    char name[200];
    FILE *fp1 = NULL;
    if ((fp1 = popen(cmd.toStdString().data(), "r")) == NULL)
        return QString();
    memset(name, 0, sizeof(name));
    fgets(name, sizeof(name), fp1);
    pclose(fp1);
    return QString(name);
}


ConvertDesktopToWinId::~ConvertDesktopToWinId()
{
}
