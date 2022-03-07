/*
 * Copyright (C) 2019 Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/&gt;.
 *
 */


#ifndef UKUITASKWIDGET_H
#define UKUITASKWIDGET_H

#include <QProxyStyle>
#include "../panel/iukuipanel.h"
//#include <QWinThumbnailToolBar>
#include <QtX11Extras/qtx11extrasversion.h>
#include <QLabel>
#include <QVBoxLayout>
#include <QStyle>
#include <KWindowInfo>

class QPainter;
class QPalette;
class QMimeData;
class UKUITaskGroup;
class UKUITaskBar;
class UKUITaskCloseButton;

class UKUITaskWidget : public QWidget
{
    Q_OBJECT

    Q_PROPERTY(Qt::Corner origin READ origin WRITE setOrigin)

public:
    explicit UKUITaskWidget(const WId window, UKUITaskBar * taskBar, QWidget *parent = 0);
    explicit UKUITaskWidget(QString iconName, const WId window, UKUITaskBar * taskbar, QWidget *parent = 0);
    virtual ~UKUITaskWidget();

    bool isApplicationHidden() const;
    bool isApplicationActive() const;
    WId windowId() const { return m_window; }

    bool hasUrgencyHint() const { return m_urgencyHint; }
    void setUrgencyHint(bool set);

    bool isOnDesktop(int desktop) const;
    bool isOnCurrentScreen() const;
    bool isMinimized() const;
    bool isFocusState() const;
    void setThumbFixedSize(int w);
    void setThumbScale(bool val);
    void setThumbMaximumSize(int w);
    void updateText();

    Qt::Corner origin() const;
    virtual void setAutoRotation(bool value, IUKUIPanel::Position position);

    UKUITaskBar * parentTaskBar() const {return m_parentTaskBar;}

    void refreshIconGeometry(QRect const & geom);
    static QString mimeDataFormat() { return QLatin1String("ukui/UKUITaskWidget"); }
    /*! \return true if this buttom received DragEnter event (and no DragLeave event yet)
     * */
    bool hasDragAndDropHover() const;
    void setThumbNail(QPixmap _pixmap);
    void setTitleFixedWidth(int size);
    void updateTitle();
    void removeThumbNail();
    void addThumbNail();
    void setPixmap(QPixmap m_pixmap);
    int getWidth();
    QPixmap getPixmap();

    void wl_updateTitle(QString caption);
    void wl_updateIcon(QString iconName);

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
    /**
     * @brief setWindowKeepAbove
     * 窗口置顶
     */
    void setWindowKeepAbove();
    /**
     * @brief setWindowStatusClear
     * 取消置顶
     */
    void setWindowStatusClear();

    void setOrigin(Qt::Corner);

    void updateIcon();

protected:
    virtual void dragEnterEvent(QDragEnterEvent *event);
    virtual void dragMoveEvent(QDragMoveEvent * event);
    virtual void dragLeaveEvent(QDragLeaveEvent *event);
    virtual void dropEvent(QDropEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
    void paintEvent(QPaintEvent *);
    void contextMenuEvent(QContextMenuEvent *event);

    void setWindowId(WId wid) {m_window = wid;}
    virtual QMimeData * mimeData();
    static bool m_draggging;

    inline IUKUIPanelPlugin * plugin() const { return m_plugin; }

private:
    WId m_window;
    bool m_urgencyHint;
    QPoint m_dragStartPosition;
    Qt::Corner m_origin;
    QPixmap m_pixmap;
    bool m_drawPixmap;
    UKUITaskBar * m_parentTaskBar;
    IUKUIPanelPlugin * m_plugin;
    QLabel *m_titleLabel;
    QLabel *m_thumbnailLabel;
    QLabel *m_appIcon;
    UKUITaskCloseButton *m_closeBtn;
    QVBoxLayout *m_vWindowsLayout;
    QHBoxLayout *m_topBarLayout;


    // Timer for when draggind something into a button (the button's window
    // must be activated so that the use can continue dragging to the window
    QTimer * m_DNDTimer;
    enum TaskWidgetStatus{NORMAL, HOVER, PRESS};
    TaskWidgetStatus m_status;

    bool m_isWaylandWidget = false;

private slots:
    void activateWithDraggable();
    void closeGroup();

signals:
    void dropped(QObject * dragSource, QPoint const & pos);
    void dragging(QObject * dragSource, QPoint const & pos);
    void windowMaximize();
    void closeSigtoPop();
    void closeSigtoGroup();
};

typedef QHash<WId,UKUITaskWidget*> UKUITaskButtonHash;

#endif // UKUITASKWIDGET_H
