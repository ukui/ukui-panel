#ifndef PINTOTASKBAR_H
#define PINTOTASKBAR_H

#include <QObject>

class PinToTaskbar : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface","com.ukui.panel.desktop")
public:
    PinToTaskbar(QObject *parent = 0);

public Q_SLOTS:
    bool AddToTaskbar(const QString &desktop);
    bool RemoveFromTaskbar(const QString &desktop);
    bool CheckIfExist(const QString &desktop);

private:
    QList<QMap<QString, QVariant> > getTaskbarFixedList();
};

#endif // PINTOTASKBAR_H
