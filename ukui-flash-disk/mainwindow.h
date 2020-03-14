#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QtWidgets>
#include <QLabel>
#include <QPushButton>
#include "qclickwidget.h"
#include<QSystemTrayIcon>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QVBoxLayout *vboxlayout;
    QLabel *no_device_label;
    QPushButton *eject_image_button;
    void newarea(QString name, qlonglong capacity, QString path,int linestatus);
    void moveBottomRight();
    QString size_human(qlonglong capacity);
    //QSystemTrayIcon m_systray;
    //void initUi();

public Q_SLOTS:
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    //void ejectDevice();
Q_SIGNALS:
    void clicked();
};

#endif
