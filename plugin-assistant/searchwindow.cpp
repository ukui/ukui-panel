#include "searchwindow.h"
#include "ui_searchwindow.h"

SearchWindow::SearchWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SearchWindow)
{
    ui->setupUi(this);
    button1=ui->pushButton;
    button2=ui->pushButton_2;
    button3=ui->pushButton_3;
    lineedit=ui->lineEdit;

}

SearchWindow::~SearchWindow()
{
    delete ui;
}

