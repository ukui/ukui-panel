/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * https://lxqt.org
 *
 * Copyright: 2015 LXQt team
 * Authors:
 *  Balázs Béla <balazsbela[at]gmail.com>
 *  Paulo Lieuthier <paulolieuthier@gmail.com>
 *
 * This program or library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */

#ifndef STATUSNOTIFIERBUTTON_H
#define STATUSNOTIFIERBUTTON_H
#include "../panel/iukuipanelplugin.h"

#include <QDBusArgument>
#include <QDBusMessage>
#include <QDBusInterface>
#include <QMimeData>
#include <QMouseEvent>
#include <QToolButton>
#include <QWheelEvent>
#include <QEvent>
#include <QMenu>
#include <QString>

#if QT_VERSION < QT_VERSION_CHECK(5, 5, 0)
template <typename T> inline T qFromUnaligned(const uchar *src)
{
    T dest;
    const size_t size = sizeof(T);
    memcpy(&dest, src, size);
    return dest;
}
#endif

class IUKUIPanelPlugin;
class SniAsync;

class StatusNotifierButton : public QToolButton
{
    Q_OBJECT

public:
    StatusNotifierButton(QString service, QString objectPath, IUKUIPanelPlugin* plugin,  QWidget *parent = 0);
    ~StatusNotifierButton();

    enum Status
    {
        Passive, Active, NeedsAttention
    };
    QString hideAbleStatusNotifierButton();
    static QString mimeDataFormat() { return QLatin1String("x-ukui/statusnotifier-button"); }


public slots:
    void newIcon();
    void newAttentionIcon();
    void newOverlayIcon();
    void newToolTip();
    void newStatus(QString status);

public:
    QString mTitle;

private:
    SniAsync *interface;
    QMenu *mMenu;
    Status mStatus;

    QString mThemePath;
//    QString mTitle;
    QIcon mIcon, mOverlayIcon, mAttentionIcon, mFallbackIcon;

    QPoint mDragStart;

    IUKUIPanelPlugin* mPlugin;

    QString drag_status_flag;

signals:
    void switchButtons(StatusNotifierButton *from, StatusNotifierButton *to);
    void sendTitle(QString arg);
    void sendstatus(QString arg);
    void cleansignal();
    void iconReady();

protected:
    void contextMenuEvent(QContextMenuEvent * event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void dragEnterEvent(QDragEnterEvent *e);
    void dragMoveEvent(QDragMoveEvent * e);
    void mouseMoveEvent(QMouseEvent *e);
    void mousePressEvent(QMouseEvent *e);
    bool event(QEvent *e);
    virtual QMimeData * mimeData();
    void resizeEvent(QResizeEvent *event);

    void refetchIcon(Status status);
    void resetIcon();
};

class StatusNotifierButtonMimeData: public QMimeData
{
    Q_OBJECT
public:
    StatusNotifierButtonMimeData():
        QMimeData(),
        mButton(0)
    {
    }

    StatusNotifierButton *button() const { return mButton; }
    void setButton(StatusNotifierButton *button) { mButton = button; }

private:
    StatusNotifierButton *mButton;
};

#endif // STATUSNOTIFIERBUTTON_H
