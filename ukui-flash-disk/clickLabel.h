#ifndef CLICKLABEL_H
#define CLICKLABEL_H

#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QStyleOption>

class ClickLabel : public QLabel
{
    Q_OBJECT

public:
    explicit ClickLabel(const QString &text, QWidget *parent = 0);
    ~ClickLabel();

protected:
    void mousePressEvent(QMouseEvent * event);
    //virtual void paintEvent(QPaintEvent * event);

Q_SIGNALS:
    void clicked();
};

#endif // CLICKLABEL_H
