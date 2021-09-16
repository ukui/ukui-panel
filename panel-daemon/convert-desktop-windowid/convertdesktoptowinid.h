#ifndef CONVERTDESKTOPTOWINID_H
#define CONVERTDESKTOPTOWINID_H

#include <QObject>

/**
 * @brief The ConvertDesktopToWinId class
 * 需要实现的功能，desktop文件与windowId的转换
 * 需要暴露的dbus接口：
 * 传入desktop文件的路径，转化为(int)WindowId
 * 传入WindowId 转化为desktop文件路径
 */

class ConvertDesktopToWinId
{
public:
    ConvertDesktopToWinId();
};

#endif // CONVERTDESKTOPTOWINID_H
