#ifndef FILEWATCHER_H
#define FILEWATCHER_H

#include <QObject>
#include <QFileSystemWatcher>
#include <QWidget>
#include <QStringList>
#include <QMap>


class FileWatcher : public QObject
{
    Q_OBJECT
public:
    FileWatcher();

    QFileSystemWatcher *fsWatcher;
    void initDirMonitor(QString path);
    QMap<QString, QStringList> m_currentContentsMap; // 当前每个监控的内容目录列表
    void directoryUpdated(const QString &path);

signals:
    void DesktopDeleteFile(const QString &path, QStringList deleteFile);
};

#endif // FILEWACTHER_H
