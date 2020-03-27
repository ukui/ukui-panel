#include "clickLabel.h"

ClickLabel::ClickLabel(const QString &text, QWidget *parent)
{
    setText(text);
    adjustSize();
}

ClickLabel::~ClickLabel()
{
}

void ClickLabel::mousePressEvent(QMouseEvent *event){
    if (event->button() == Qt::LeftButton)
        Q_EMIT clicked();
    QLabel::mousePressEvent(event);
}

//void ClickLabel::paintEvent(QPaintEvent *event){
////    Q_UNUSED(event)
////    QStyleOption opt;
////    opt.init(this);
////    QPainter p(this);
////    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
//}
