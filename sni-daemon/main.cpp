#include "snidaemon.h"
#include <ukui-log4qt.h>
#include <QApplication>

int main(int argc, char *argv[])
{
    initUkuiLog4qt("sni-daemon");
    QApplication a(argc, argv);
    SniDaemon w;
    return a.exec();
}
