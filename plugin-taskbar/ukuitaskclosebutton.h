#ifndef UKUITASKCLOSEBUTTON_H
#define UKUITASKCLOSEBUTTON_H

#include <QToolButton>
#include <QMouseEvent>
class UKUITaskCloseButton : public QToolButton
{
    Q_OBJECT
public:
    explicit UKUITaskCloseButton(const WId window, QWidget *parent = 0);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
private:
    WId mWindow;
signals:
    void sigClicked();
};

#endif // UKUITASKCLOSEBUTTON_H
