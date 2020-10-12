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

private Q_SLOTS:
    void initForm();
    void on_cboxCalendarStyle_currentIndexChanged(int index);
    void on_cboxSelectType_currentIndexChanged(int index);
    void on_cboxWeekNameFormat_currentIndexChanged(int index);
    void on_ckShowLunar_stateChanged(int arg1);
};

#endif // FRMLUNARCALENDARWIDGET_H
