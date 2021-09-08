#include "statelabel.h"

statelabel::statelabel() : QLabel()
{


}

//鼠标点击事件
void statelabel::mousePressEvent(QMouseEvent *event)
{
    if (event->buttons() == Qt::LeftButton){
        Q_EMIT labelclick();

    }
    return;
}
