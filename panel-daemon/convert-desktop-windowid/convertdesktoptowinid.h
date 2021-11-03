#ifndef CONVERTDESKTOPTOWINID_H
#define CONVERTDESKTOPTOWINID_H

#include "convertdesktoptowinid.h"
#include <QObject>
#include <KWindowSystem>
#include <QDir>

#define DEKSTOP_FILE_PATH       "/usr/share/applications/"
#define USR_SHARE_APP_CURRENT   "/usr/share/applications/."
#define USR_SHARE_APP_UPER      "/usr/share/applications/.."
#define PEONY_TRASH             "/usr/share/applications/peony-trash.desktop"
#define PEONY_COMUTER           "/usr/share/applications/peony-computer.desktop"
#define PEONY_HOME              "/usr/share/applications/peony-home.desktop"
#define PEONY_MAIN              "/usr/share/applications/peony.desktop"

#define GET_DESKTOP_EXEC_NAME_MAIN          "cat %s | awk '{if($1~\"Exec=\")if($2~\"\%\"){print $1} else print}' | cut -d '=' -f 2"

/**
 * @brief The ConvertDesktopToWinId class
 * 需要实现的功能，desktop文件与windowId的转换
 * 需要暴露的dbus接口：
 * 传入desktop文件的路径，转化为(int)WindowId
 * 传入WindowId 转化为desktop文件路径
 */

class ConvertDesktopToWinId: public QObject
{
    Q_OBJECT

public:
    ConvertDesktopToWinId();
    ~ConvertDesktopToWinId();
    //QList<int> InfoPidList;
    QString desktop_file_path = nullptr;
    QString classClass = nullptr;
    QString className = nullptr;
    QString statusName = nullptr;
    QString cmd_line = nullptr;

    QDir *dir = nullptr;
    QFileInfoList list;

    QString tranIdToDesktop(WId id);

private:
    QString onWindowAdded(WId windowId);
    QString confirmDesktopFileName(KWindowInfo info);
    void searchFromEnviron(KWindowInfo info);
    void compareClassName();
    void compareCmdExec();
    void compareLastStrategy();
    QString getDesktopFileName(QString cmd);
};

#endif // CONVERTDESKTOPTOWINID_H
