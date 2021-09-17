#include "filewatcher.h"
#include <QDebug>
#include <QStringList>
#include <QDir>

#define APPLICATION_PATH "/usr/share/applications/"

FileWatcher::FileWatcher()
{
    fsWatcher=new QFileSystemWatcher(this);
    fsWatcher->addPath(APPLICATION_PATH);
    initDirMonitor(APPLICATION_PATH);
    connect(fsWatcher,&QFileSystemWatcher::directoryChanged,[this](){
               directoryUpdated(APPLICATION_PATH);
            });

    qDebug()<<"********************";
}

void FileWatcher::initDirMonitor(QString path) {
    const QDir dir(path);
    m_currentContentsMap[path] = dir.entryList(QDir::NoDotAndDotDot  | QDir::AllDirs | QDir::Files, QDir::DirsFirst);
}

//只要任何监控的目录更新（添加、删除、重命名），就会调用。
void FileWatcher::directoryUpdated(const QString &path)
{
        qDebug()<<"22222UKUIQuickLaunch::directoryUpdated "<<path;

    // 比较最新的内容和保存的内容找出区别(变化)
    QStringList currEntryList = m_currentContentsMap[path];
    const QDir dir(path);
    QStringList newEntryList = dir.entryList(QDir::NoDotAndDotDot  | QDir::AllDirs | QDir::Files, QDir::DirsFirst);
    QSet<QString> newDirSet = QSet<QString>::fromList(newEntryList);
    QSet<QString> currentDirSet = QSet<QString>::fromList(currEntryList);

    // 添加了文件
    QSet<QString> newFiles = newDirSet - currentDirSet;
    QStringList newFile = newFiles.toList();

    // 文件已被移除
    QSet<QString> deletedFiles = currentDirSet - newDirSet;
    QStringList deleteFile = deletedFiles.toList();

    // 更新当前设置
    m_currentContentsMap[path] = newEntryList;

    if (!newFile.isEmpty() && !deleteFile.isEmpty())
    {
        // 文件/目录重命名
        if ((newFile.count() == 1) && (deleteFile.count() == 1))
        {
//            qDebug() << QString("File Renamed from %1 to %2").arg(deleteFile.first()).arg(newFile.first());
        }
    }
    else
    {
        // 添加新文件/目录至Dir
        if (!newFile.isEmpty())
        {
//            foreach (QString file, newFile)
//            {
//                // 处理操作每个新文件....
//            }
        }
        // 从Dir中删除文件/目录
        if (!deleteFile.isEmpty())
        {
            qDebug()<<"***************"<<deleteFile;
//            foreach(QString file, deleteFile)
//            {
//                // 处理操作每个被删除的文件....
////                removeButton(path+file);
//               qDebug()<<"222找到要删除的文件"<<path+file;
//            }
        }
    }
}
