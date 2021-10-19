#include "snidaemon.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SniDaemon w;
    return a.exec();
}
