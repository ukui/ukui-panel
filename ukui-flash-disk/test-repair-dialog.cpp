#include "repair-dialog-box.h"

#include <QDebug>
#include <QApplication>
#include <QMessageBox>


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

//    RepairDialogBox box(nullptr);
//    box.show();

//    RepairProgressBar test(QObject::tr("正在进行磁盘修复..."));
//    test.exec();

//    FormateDialog test(nullptr);
//    test.exec();

    return app.exec();
}
