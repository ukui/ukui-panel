/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Copyright: 2011 Razor team
 *            2014 LXQt team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.com>
 *   Kuzma Shapran <kuzma.shapran@gmail.com>
 *
 * Copyright: 2019 Tianjin KYLIN Information Technology Co., Ltd. *
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

 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */


#ifndef UKUITASKBUTTON_H
#define UKUITASKBUTTON_H

#include <QGSettings/QGSettings>
#include <QToolButton>
#include <QProxyStyle>
#include <QLabel>
#include "../panel/iukuipanel.h"
//#include <QWinThumbnailToolBar>
#include <QtX11Extras/qtx11extrasversion.h>

#include "quicklaunchaction.h"
#include <QMimeData>
#include "../panel/customstyle.h"
#include <QStyleOption>
#include <QGSettings>
#include <QPainter>
#include <QMenu>

class QPainter;
class QPalette;
class QMimeData;
class UKUITaskGroup;
class UKUITaskBar;
class IUKUIPanelPlugin;
class QuicklaunchMenu:public QMenu
{
public:
    QuicklaunchMenu();
    ~QuicklaunchMenu();
protected:
    void contextMenuEvent(QContextMenuEvent*);

};
class LeftAlignedTextStyle : public QProxyStyle
{
    using QProxyStyle::QProxyStyle;
public:

    virtual void drawItemText(QPainter * painter, const QRect & rect, int flags
            , const QPalette & pal, bool enabled, const QString & text
            , QPalette::ColorRole textRole = QPalette::NoRole) const override;
};


class UKUITaskButton : public QToolButton
{
    Q_OBJECT

    Q_PROPERTY(Qt::Corner origin READ origin WRITE setOrigin)

public:
    explicit UKUITaskButton(QString appName,const WId window, UKUITaskBar * taskBar, QWidget *parent = 0);
    explicit UKUITaskButton(QString iconName, QString caption, const WId window, UKUITaskBar * taskbar, QWidget *parent = 0);
    UKUITaskButton(QuickLaunchAction * act, IUKUIPanelPlugin * plugin, QWidget* parent = 0);
    virtual ~UKUITaskButton();

    bool isApplicationHidden() const;
    bool isApplicationActive() const;
    WId windowId() const { return m_window; }

    bool hasUrgencyHint() const { return m_urgencyHint; }
    void setUrgencyHint(bool set);

    bool isOnDesktop(int desktop) const;
    bool isOnCurrentScreen() const;
    bool isMinimized() const;
    void updateText();
    void setLeaderWindow(WId leaderWindow);
    bool isLeaderWindow(WId compare) { return m_window == compare; }

    Qt::Corner origin() const;
    virtual void setAutoRotation(bool value, IUKUIPanel::Position position);

    UKUITaskBar * parentTaskBar() const {return m_parentTaskBar;}

    void refreshIconGeometry(QRect const & geom);
    static QString mimeDataFormat() { return QLatin1String("ukui/UKUITaskButton"); }
    /*! \return true if this buttom received DragEnter event (and no DragLeave event yet)
     * */
    bool hasDragAndDropHover() const;

    /////////////////////////////////
    QHash<QString,QString> settingsMap();
    QString m_fileName;
    QString m_file;
    QString m_name;
    QString m_exec;

    void toDomodifyQuicklaunchMenuAction(bool direction) { modifyQuicklaunchMenuAction(direction);}

    bool m_isWinActivate;  //1为激活状态，0为隐藏状态
    QString m_iconName;
    QString m_caption;

public slots:
    void raiseApplication();
    void minimizeApplication();
    void maximizeApplication();
    void deMaximizeApplication();
    void shadeApplication();
    void unShadeApplication();
    void closeApplication();
    void moveApplicationToDesktop();
    void moveApplication();
    void resizeApplication();
    void setApplicationLayer();
    void setOrigin(Qt::Corner);

    void updateIcon();

    //////

    void selfRemove();
    void this_customContextMenuRequested(const QPoint & pos);
    void setGroupIcon(QIcon ico);


protected:
    virtual void dragEnterEvent(QDragEnterEvent *event);
    virtual void dragMoveEvent(QDragMoveEvent * event);
    virtual void dragLeaveEvent(QDragLeaveEvent *event);
    virtual void dropEvent(QDropEvent *event);
    void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    virtual void contextMenuEvent(QContextMenuEvent *event);
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
    void paintEvent(QPaintEvent *);

    void setWindowId(WId wid) {m_window = wid;}
    virtual QMimeData * mimeData();
    static bool m_draggging;

    inline IUKUIPanelPlugin * plugin() const { return m_plugin; }

    /////////////////////////////////////
    //virtual QMimeData * mimeData();

private:
    bool m_statFlag = true;

    WId m_window;
    QString m_appName;
    bool m_urgencyHint;
    QPoint m_dragStartPosition;
    Qt::Corner m_origin;
    bool m_drawPixmap;
    UKUITaskBar * m_parentTaskBar;
    IUKUIPanelPlugin * m_plugin;
    enum TaskButtonStatus{NORMAL, HOVER, PRESS};
    TaskButtonStatus m_taskButtonStatus;
    QIcon m_icon;

    // Timer for when draggind something into a button (the button's window
    // must be activated so that the use can continue dragging to the window
    QTimer * m_DNDTimer;
    QGSettings *m_gsettings;


    ///////////////////////////////////
    QuickLaunchAction *m_act;
    QAction *m_deleteAct;
    QuicklaunchMenu *m_menu;
    QPoint m_dragStart;
    TaskButtonStatus m_quickLanuchStatus;
    CustomStyle m_toolButtonStyle;
    QGSettings *m_gsettingsQuickLaunch;

    void modifyQuicklaunchMenuAction(bool direction);
private slots:
    void activateWithDraggable();


signals:
    void dropped(QObject * dragSource, QPoint const & pos);
    void dragging(QObject * dragSource, QPoint const & pos);
    //////////////////////////////////
    void buttonDeleted();
    void switchButtons(UKUITaskButton *from, UKUITaskButton *to);
    void t_saveSettings();
};


class ButtonMimeData: public QMimeData
{
    Q_OBJECT
public:
    ButtonMimeData():
        QMimeData(),
        m_button(0)
    {
    }

    UKUITaskButton *button() const { return m_button; }
    void setButton(UKUITaskButton *button) { m_button = button; }

private:
    UKUITaskButton *m_button;
};
//typedef QHash<WId,UKUITaskButton*> UKUITaskButtonHash;
//typedef QHash<WId,QWidget*> UKUITaskButtonHash;

#endif // UKUITASKBUTTON_H
