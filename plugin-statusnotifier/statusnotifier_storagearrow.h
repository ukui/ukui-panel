#ifndef STATUSNOTIFIERSTORAGEARROW_H
#define STATUSNOTIFIERSTORAGEARROW_H

#include <QToolButton>
#include <QGSettings>

#include "statusnotifierwidget.h"

#define UKUI_PANEL_SETTINGS              "org.ukui.panel.settings"
#define SHOW_STATUSNOTIFIER_BUTTON       "statusnotifierbutton"
#define PANEL_POSITION_KEY  "panelposition"

class StatusNotifierWidget;

class StatusNotifierStorageArrow : public QToolButton
{
    Q_OBJECT
public:
    StatusNotifierStorageArrow(StatusNotifierWidget *parent = nullptr);
    ~StatusNotifierStorageArrow();
protected:
    void mousePressEvent(QMouseEvent *);
    void dropEvent(QDropEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void resizeEvent(QResizeEvent *event);

private:
    void setArrowIcon();

private:
    QGSettings *mGsettings;
    StatusNotifierWidget *mParent;
    int mPanelPosition;

signals:
    void addButton(QString button);
};

#endif // STATUSNOTIFIERSTORAGEARROW_H
