#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QScrollArea>
#include <QScrollBar>
#include <QtWidgets>
#include "qclickwidget.h"

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
    QScrollArea *m_ScrollArea;
    QSystemTrayIcon m_systray;
    QVBoxLayout *vboxlayout;
//    QBoxLayout *vboxlayout;
    QLabel *no_device_label;

private slots:
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void mouseClicked();

private:
    void moveBottomRight();
    void newarea(QString name, qlonglong capacity, QString path,int linestatus);
    int getdevicenum();
    QString size_human(qlonglong capacity);
signals:
    void clicked();
};

#endif // MAINWINDOW_H
