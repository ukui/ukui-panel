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
private:
    void setArrowIcon();
    int GetTaskbarInfo();
    QGSettings *gsetting;
    int panelPosition;
    int iconsize;
};
#endif
