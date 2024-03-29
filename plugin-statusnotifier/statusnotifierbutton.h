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
#include <QGSettings>
#include <dbusmenu-qt5/dbusmenuimporter.h>

#define ORG_UKUI_STYLE  "org.ukui.style"
#define ICON_THEME_NAME "iconThemeName"
#define STYLE_NAME                "styleName"
#define STYLE_NAME_KEY_DARK       "ukui-dark"
#define STYLE_NAME_KEY_DEFAULT    "ukui-default"
#define STYLE_NAME_KEY_BLACK       "ukui-black"
#define STYLE_NAME_KEY_LIGHT       "ukui-light"
#define STYLE_NAME_KEY_WHITE       "ukui-white"


#if QT_VERSION < QT_VERSION_CHECK(5, 5, 0)
template <typename T> inline T qFromUnaligned(const uchar *src)
{
    T dest;
    const size_t size = sizeof(T);
    memcpy(&dest, src, size);
    return dest;
}
#endif

/*! \brief specialized DBusMenuImporter to correctly create actions' icons based
 * on name
 */
class MenuImporter : public DBusMenuImporter
{
public:
    using DBusMenuImporter::DBusMenuImporter;

protected:
    virtual QIcon iconForName(const QString & name) override
    {
        return QIcon::fromTheme(name);
    }
};


class IUKUIPanelPlugin;
class SniAsync;
class MenuImporter;

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
    QImage getBlackThemeIcon(QImage image);

public slots:
    void newIcon();
    void newAttentionIcon();
    void newOverlayIcon();
    void newToolTip();
    void newStatus(QString status);

public:
    QString mId;
    bool mIdStatus=false;
    bool mIconStatus=false;

private:
    SniAsync *interface;
    MenuImporter *mMenuImporter;
    QMenu *mMenu;
    Status mStatus;

    QString mThemePath;
    QIcon mIcon, mOverlayIcon, mAttentionIcon, mFallbackIcon;

    QPoint mDragStart;

    IUKUIPanelPlugin* mPlugin;

    QString drag_status_flag;

    uint mCount = 0;
    bool mParamInit=false;
    QGSettings *mThemeSettings;
    QPoint mCursorLeftPos;


signals:
    void switchButtons(StatusNotifierButton *from, StatusNotifierButton *to);
    void sendTitle(QString arg);
    void sendstatus(QString arg);
    void cleansignal();
    void iconReady();
    void layoutReady();
    void paramReady();

protected:
    void contextMenuEvent(QContextMenuEvent * event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void dragEnterEvent(QDragEnterEvent *e);
    void dragLeaveEvent(QDragLeaveEvent *e);
    void dragMoveEvent(QDragMoveEvent * e);
    void mouseMoveEvent(QMouseEvent *e);
    void mousePressEvent(QMouseEvent *e);
    bool event(QEvent *e);
    virtual QMimeData * mimeData();
    void resizeEvent(QResizeEvent *event);
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);

    void refetchIcon(Status status);
    void resetIcon();

private:
    void systemThemeChanges();
    void updataItemMenu();

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
