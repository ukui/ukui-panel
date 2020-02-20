#ifndef QCLICKWIDGET_H
#define QCLICKWIDGET_H

#include <QWidget>
#include <QPoint>
#include <QDebug>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QStyle>
#include <QPainter>
#include <QStyleOption>
#include <QProcess>
#include <sys/mount.h>

class MainWindow;
class QClickWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QClickWidget(QWidget *parent = nullptr,bool is_eject = true, QString name=NULL, qlonglong capacity=NULL, QString path=NULL);
    ~QClickWidget(void);

signals:
    void clicked();

public slots:
    void mouseClicked();

protected:
    void mousePressEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *ev);
    void paintEvent(QPaintEvent *);

private:
    bool m_is_eject;
    QString m_name;
    qlonglong m_capacity;
    QString m_path;
    MainWindow *m_mainwindow;
    QPoint mousePos;
};

#endif // QCLICKWIDGET_H
