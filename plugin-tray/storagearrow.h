#ifndef STORAGEARROW_H
#define STORAGEARROW_H

//Qt
#include <QToolButton>
#include <QPushButton>
#include <QGSettings>
enum storageArrowStatus{NORMAL,HOVER,PRESS};
class StorageArrow : public QPushButton {
public :
    StorageArrow(QWidget* parent );
    ~StorageArrow();
protected :
//    void paintEvent(QEvent *);
    /**
     * @brief enterEvent leaveEvent
     * @param event
     *  enterEvent leaveEvent 只是为了解决鼠标离开按钮后依然出现的悬浮现象
     */
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);
private:
    void setArrowIcon();
    int GetTaskbarInfo();
    QGSettings *gsetting;
    int panelPosition;
    int iconsize;
};
#endif
