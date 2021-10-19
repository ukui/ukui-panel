#ifndef TASKVIEWBUTTON_H
#define TASKVIEWBUTTON_H

#include <QToolButton>
#include <QMouseEvent>

#include "../panel/customstyle.h"
#include "../panel/iukuipanelplugin.h"
class TaskViewButton :public QToolButton
{
    Q_OBJECT
public:
    TaskViewButton(IUKUIPanelPlugin *plugin,QWidget *parent=nullptr);
    ~TaskViewButton();
    void realign();
protected:
    void mousePressEvent(QMouseEvent* event);

private:
    QWidget *mParent;
    IUKUIPanelPlugin * mPlugin;

};

#endif // TASKVIEWBUTTON_H
