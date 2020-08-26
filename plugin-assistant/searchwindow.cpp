#include "searchwindow.h"
#include "ui_searchwindow.h"
#include <QDebug>
#include <QProcess>
#include <stdio.h>
#include <pwd.h>
#include <unistd.h>
SearchWindow::SearchWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SearchWindow)
{
    ui->setupUi(this);
    button1=ui->pushButton;
    button2=ui->pushButton_2;
    button3=ui->pushButton_3;
    lineedit=ui->lineEdit;

    connect(lineedit,&QLineEdit::textChanged,this,[=](){
         x=ui->lineEdit->text();

    });
    connect(button3,&QPushButton::clicked,this,[=](){

        qDebug()<<x;
        struct passwd *pwd;
        pwd=getpwuid(getuid());
        qDebug()<<"user name"<<pwd->pw_name;
        QString str = QString("search:///search_uris=file:///home/%1&name_regexp=%2&recursive=1").arg(pwd->pw_name).arg(x);
        QProcess p;
        p.setProgram(QString("peony"));
       // p.setArguments(QStringList()<<"search:///search_uris=file:///home/sunfengsheng&name_regexp=kjhkjh&recursive=1");
        p.setArguments(QStringList()<<str);
        p.startDetached(p.program(), p.arguments());
        p.waitForFinished(-1);
    });
}

SearchWindow::~SearchWindow()
{
    delete ui;
}


void SearchWindow::on_pushButton_3_clicked()
{

}
