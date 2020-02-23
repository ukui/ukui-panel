#include "ukuitaskclosebutton.h"
#include <QDebug>
UKUITaskCloseButton::UKUITaskCloseButton(const WId window, QWidget *parent):
    QToolButton(parent),
    mWindow(window)
{
    //connect(parent, &UKUITaskBar::buttonRotationRefreshed, this, &UKUITaskGroup::setAutoRotation);
}


/************************************************

 ************************************************/
void UKUITaskCloseButton::mousePressEvent(QMouseEvent* event)
{
//    const Qt::MouseButton b = event->button();

//    if (Qt::LeftButton == b)
//        mDragStartPosition = event->pos();
//    else if (Qt::MidButton == b && parentTaskBar()->closeOnMiddleClick())
//        closeApplication();

    QToolButton::mousePressEvent(event);
}

/************************************************

 ************************************************/
void UKUITaskCloseButton::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button()== Qt::LeftButton)
    {
        qDebug()<<"Qt::LeftButton";
        emit sigClicked();
    }
    QToolButton::mouseReleaseEvent(event);

}
