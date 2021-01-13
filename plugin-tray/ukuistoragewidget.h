#ifndef UKUISTORAGEWIDGET_H
#define UKUISTORAGEWIDGET_H

#include <QObject>
#include <QWidget>

class UKUiStorageWidget : public QWidget
{
public:
    UKUiStorageWidget();

    void setStorageWidgetButtonLayout(int size);
protected:
    void paintEvent(QPaintEvent*);
};

#endif // UKUISTORAGEWIDGET_H
