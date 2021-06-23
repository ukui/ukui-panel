#ifndef FRMLUNARCALENDARWIDGET_H
#define FRMLUNARCALENDARWIDGET_H

#include <QWidget>
#include <QGSettings>

namespace Ui {
class frmLunarCalendarWidget;
}

class frmLunarCalendarWidget : public QWidget
{
    Q_OBJECT

public:
    explicit frmLunarCalendarWidget(QWidget *parent = 0);
    ~frmLunarCalendarWidget();

protected:
    void paintEvent(QPaintEvent *);

private:
    Ui::frmLunarCalendarWidget *ui;
    QGSettings *transparency_gsettings;
    QGSettings *calendar_gsettings;

private Q_SLOTS:
    void initForm();
    void cboxCalendarStyle_currentIndexChanged(int index);
    void cboxSelectType_currentIndexChanged(int index);
    void cboxWeekNameFormat_currentIndexChanged(bool FirstDayisSun);
    void ckShowLunar_stateChanged(bool arg1);
    void changeUpSize();
    void changeDownSize();

Q_SIGNALS:
    void yijiChangeUp();
    void yijiChangeDown();
};

#endif // FRMLUNARCALENDARWIDGET_H
