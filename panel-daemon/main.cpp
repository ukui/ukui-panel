#include "watchermanager.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    WatcherManager manager;
    return a.exec();
}
