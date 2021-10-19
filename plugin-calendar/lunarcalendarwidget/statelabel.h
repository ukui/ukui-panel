#ifndef STATELABEL_H
#define STATELABEL_H

#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QMouseEvent>
#include <QDebug>


class statelabel : public QLabel
{
    Q_OBJECT
public:
    statelabel();

protected:
    void mousePressEvent(QMouseEvent *event);
Q_SIGNALS :
    void labelclick();
};

#endif // STATELABEL_H
