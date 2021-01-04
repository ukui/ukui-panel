#ifndef STORAGEARROW_H
#define STORAGEARROW_H

//Qt
#include <QToolButton>
class TrayButton : public QToolButton {
public :
    TrayButton(QWidget* parent );
    ~TrayButton();
protected :
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
};
#endif
