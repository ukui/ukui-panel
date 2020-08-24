#ifndef SEARCHWINDOW_H
#define SEARCHWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLineEdit>
namespace Ui {
class SearchWindow;
}

class SearchWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit SearchWindow(QWidget *parent = 0);
    ~SearchWindow();
    QPushButton *button1;
    QPushButton *button2;
    QPushButton *button3;
    QLineEdit *lineedit;

private slots:

private:
    Ui::SearchWindow *ui;
};

#endif // MAINWINDOW_H
