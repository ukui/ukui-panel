#include "qclickwidget.h"

QClickWidget::QClickWidget(QWidget *parent, bool is_eject, QString name, qlonglong capacity,QString path)
    : QWidget(parent),m_is_eject(is_eject),m_name(name),m_capacity(capacity),m_path(path)
{
    connect(this, SIGNAL(clicked()), this, SLOT(mouseClicked()));
//    this->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    qDebug() << m_name;
    qDebug() << m_capacity;
}
QClickWidget::~QClickWidget(void){
}

void QClickWidget::mouseClicked()
{
    //处理代码
    QString  filename;
    std::string str = m_path.toStdString();
    const char* ch = str.c_str();
    qDebug() << "hhhhhhhhhhhhhh" << m_path << ch;
    if (m_is_eject == true){
        qDebug() << "弹出";
        umount(ch);
        QProcess ::execute("umount "+m_path);
//        this->topLevelWidget()->resize(250, 158);
        this->topLevelWidget()->hide();
    } else{
        QProcess ::execute("peony "+m_path);
        qDebug() << "打开";
     //   m_mainwindow->hide();
        this->topLevelWidget()->hide();
    }
}

void QClickWidget::mousePressEvent(QMouseEvent *ev)
{
    mousePos = QPoint(ev->x(), ev->y());
}

void QClickWidget::mouseReleaseEvent(QMouseEvent *ev)
{
    if(mousePos == QPoint(ev->x(), ev->y())) emit clicked();
}

void QClickWidget::paintEvent(QPaintEvent *)
 {
     //解决QClickWidget类设置样式不生效的问题
     QStyleOption opt;
     opt.init(this);
     QPainter p(this);
     style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
 }
