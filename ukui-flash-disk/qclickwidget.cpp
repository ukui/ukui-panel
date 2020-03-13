#include "qclickwidget.h"

QClickWidget::QClickWidget(QWidget *parent, bool is_eject, QString name, qlonglong capacity,QString path)
    : QWidget(parent),m_is_eject(is_eject),m_name(name),m_capacity(capacity),m_path(path)
{
    connect(this, SIGNAL(clicked()), this, SLOT(mouseClicked()));
}

QClickWidget::~QClickWidget()
{

}

void QClickWidget::mouseClicked()
{
    //处理代码
//        std::string str = m_path.toStdString();
//        const char* ch = str.c_str();
    qDebug() << "打开" << m_path;
    QProcess::startDetached("peony "+m_path);

    this->topLevelWidget()->hide();
}

void QClickWidget::mousePressEvent(QMouseEvent *ev)
{
    mousePos = QPoint(ev->x(), ev->y());
}

void QClickWidget::mouseReleaseEvent(QMouseEvent *ev)
{
    if(mousePos == QPoint(ev->x(), ev->y())) Q_EMIT clicked();
}

void QClickWidget::paintEvent(QPaintEvent *)
 {
     //解决QClickWidget类设置样式不生效的问题
     QStyleOption opt;
     opt.init(this);
     QPainter p(this);
     style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
 }
