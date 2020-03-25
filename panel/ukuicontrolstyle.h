#ifndef UKUICONTROLSTYLE_H
#define UKUICONTROLSTYLE_H
#include <QMenu>
#include <QToolButton>
class UKUiMenu:public QMenu
{
public:
    UKUiMenu();
    ~UKUiMenu();

protected:
    void paintEvent(QPaintEvent*);
};

class UkuiToolButton:public QToolButton
{
public:
    UkuiToolButton();
    ~UkuiToolButton();
    void paintTooltipStyle();
};
#endif
