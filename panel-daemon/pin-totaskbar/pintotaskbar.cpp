#include "pintotaskbar.h"
#include <QtDBus>

PinToTaskbar::PinToTaskbar(QObject *parent) :
    QObject(parent)
{

}

bool PinToTaskbar::AddToTaskbar(const QString &desktop)
{
    QDBusMessage message = QDBusMessage::createSignal("/taskbar/quicklaunch", "org.ukui.panel.taskbar", "AddToTaskbar");
    message << desktop;
    QDBusConnection::sessionBus().send(message);
    return true;
}

bool PinToTaskbar::RemoveFromTaskbar(const QString &desktop)
{
    QDBusMessage message = QDBusMessage::createSignal("/taskbar/quicklaunch", "org.ukui.panel.taskbar", "RemoveFromTaskbar");
    message << desktop;
    QDBusConnection::sessionBus().send(message);
    return true;
}

bool PinToTaskbar::CheckIfExist(const QString &desktop)
{
    QString fixdDesktop;
    const auto apps = getTaskbarFixedList();

    for (const QMap<QString, QVariant> &app : apps) {
        fixdDesktop = app.value("desktop", "").toString();

        if (fixdDesktop.contains(desktop)) {
            return true;
        }
    }

    return false;

}

QList<QMap<QString, QVariant> > PinToTaskbar::getTaskbarFixedList()
{
    QSettings settings(QDir::homePath() + "/.config/ukui/panel.conf", QSettings::IniFormat);
    QList<QMap<QString, QVariant> > array;
    int size = settings.beginReadArray("/taskbar/apps");

    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        QMap<QString, QVariant> hash;
        const auto keys = settings.childKeys();

        for (const QString &key : keys) {
            hash[key] = settings.value(key);
        }

        array << hash;
    }

    return array;
}
