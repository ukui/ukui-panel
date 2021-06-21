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

#include "statusnotifierbutton.h"

#include <QDir>
#include <QFile>
#include <QApplication>
#include <QDrag>
#include <dbusmenu-qt5/dbusmenuimporter.h>
#include "../panel/iukuipanelplugin.h"
#include "sniasync.h"
#include "../panel/customstyle.h"
#include "../panel/highlight-effect.h"
#include <QDebug>
//#include <XdgIcon>

#define MIMETYPE "ukui/UkuiTaskBar"

namespace
{
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
}

StatusNotifierButton::StatusNotifierButton(QString service, QString objectPath, IUKUIPanelPlugin* plugin, QWidget *parent)
    : QToolButton(parent),
    mMenu(nullptr),
    mStatus(Passive),
    mFallbackIcon(QIcon::fromTheme("application-x-executable")),
    mPlugin(plugin)
{
//    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setAutoRaise(true);
    setAcceptDrops(true);
    interface = new SniAsync(service, objectPath, QDBusConnection::sessionBus(), this);

    connect(interface, &SniAsync::NewIcon, this, &StatusNotifierButton::newIcon);
    connect(interface, &SniAsync::NewOverlayIcon, this, &StatusNotifierButton::newOverlayIcon);
    connect(interface, &SniAsync::NewAttentionIcon, this, &StatusNotifierButton::newAttentionIcon);
    connect(interface, &SniAsync::NewToolTip, this, &StatusNotifierButton::newToolTip);
    connect(interface, &SniAsync::NewStatus, this, &StatusNotifierButton::newStatus);

    interface->propertyGetAsync(QLatin1String("Menu"), [this] (QDBusObjectPath path) {
        if (!path.path().isEmpty())
        {
            mMenu = (new MenuImporter{interface->service(), path.path(), this})->menu();
            mMenu->setObjectName(QLatin1String("StatusNotifierMenu"));
        }
    });

    interface->propertyGetAsync(QLatin1String("Status"), [this] (QString status) {
        newStatus(status);
    });

    interface->propertyGetAsync(QLatin1String("IconThemePath"), [this] (QString value) {
        mThemePath = value;
        //do the logic of icons after we've got the theme path
        refetchIcon(Active);
        refetchIcon(Passive);
        refetchIcon(NeedsAttention);
    });

    newToolTip();
}

StatusNotifierButton::~StatusNotifierButton()
{
    delete interface;
}

void StatusNotifierButton::newIcon()
{
    refetchIcon(Passive);
}

void StatusNotifierButton::newOverlayIcon()
{
    refetchIcon(Active);
}

void StatusNotifierButton::newAttentionIcon()
{
    refetchIcon(NeedsAttention);
}

void StatusNotifierButton::refetchIcon(Status status)
{
    QString nameProperty, pixmapProperty;
    if (status == Active)
    {
        nameProperty = QLatin1String("OverlayIconName");
        pixmapProperty = QLatin1String("OverlayIconPixmap");
    }
    else if (status == NeedsAttention)
    {
        nameProperty = QLatin1String("AttentionIconName");
        pixmapProperty = QLatin1String("AttentionIconPixmap");
    }
    else // status == Passive
    {
        nameProperty = QLatin1String("IconName");
        pixmapProperty = QLatin1String("IconPixmap");
    }

    interface->propertyGetAsync(nameProperty, [this, status, pixmapProperty] (QString iconName) {
        QIcon nextIcon;
        if (!iconName.isEmpty())
        {
            if (QIcon::hasThemeIcon(iconName))
                nextIcon = QIcon::fromTheme(iconName);
            else
            {
                QDir themeDir(mThemePath);
                if (themeDir.exists())
                {
                    if (themeDir.exists(iconName + ".png"))
                        nextIcon.addFile(themeDir.filePath(iconName + ".png"));

                    if (themeDir.cd("hicolor") || (themeDir.cd("icons") && themeDir.cd("hicolor")))
                    {
                        const QStringList sizes = themeDir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
                        for (const QString &dir : sizes)
                        {
                            const QStringList dirs = QDir(themeDir.filePath(dir)).entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
                            for (const QString &innerDir : dirs)
                            {
                                QString file = themeDir.absolutePath() + "/" + dir + "/" + innerDir + "/" + iconName + ".png";
                                if (QFile::exists(file))
                                    nextIcon.addFile(file);
                            }
                        }
                    }
                }
            }
            nextIcon=HighLightEffect::drawSymbolicColoredIcon(nextIcon);

            switch (status)
            {
                case Active:
                    mOverlayIcon = nextIcon;
                    break;
                case NeedsAttention:
                    mAttentionIcon = nextIcon;
                    break;
                case Passive:
                    mIcon = nextIcon;
                    break;
            }

            resetIcon();
        }
        else
        {
            interface->propertyGetAsync(pixmapProperty, [this, status, pixmapProperty] (IconPixmapList iconPixmaps) {
                if (iconPixmaps.empty())
                    return;

                QIcon nextIcon;

                for (IconPixmap iconPixmap: iconPixmaps)
                {
                    if (!iconPixmap.bytes.isNull())
                    {
                        QImage image((uchar*) iconPixmap.bytes.data(), iconPixmap.width,
                                     iconPixmap.height, QImage::Format_ARGB32);

                        const uchar *end = image.constBits() + image.byteCount();
                        uchar *dest = reinterpret_cast<uchar*>(iconPixmap.bytes.data());
                        for (const uchar *src = image.constBits(); src < end; src += 4, dest += 4)
                            qToUnaligned(qToBigEndian<quint32>(qFromUnaligned<quint32>(src)), dest);

                        nextIcon.addPixmap(QPixmap::fromImage(image));
                    }
                }

                switch (status)
                {
                    case Active:
                        mOverlayIcon = nextIcon;
                        break;
                    case NeedsAttention:
                        mAttentionIcon = nextIcon;
                        break;
                    case Passive:
                        mIcon = nextIcon;
                        break;
                }

                resetIcon();
            });
        }
    });
}

void StatusNotifierButton::newToolTip()
{
    interface->propertyGetAsync(QLatin1String("ToolTip"), [this] (ToolTip tooltip) {

        QString toolTipTitle = tooltip.title;
        if (!toolTipTitle.isEmpty())
            setToolTip(toolTipTitle);
        else
            interface->propertyGetAsync(QLatin1String("Title"), [this] (QString title) {
                // we should get here only in case the ToolTip.title was empty
                if (!title.isEmpty())
                    setToolTip(title);
            });
    });
}

void StatusNotifierButton::newStatus(QString status)
{
    Status newStatus;
    if (status == QLatin1String("Passive"))
        newStatus = Passive;
    else if (status == QLatin1String("Active"))
        newStatus = Active;
    else
        newStatus = NeedsAttention;

    if (mStatus == newStatus)
        return;

    mStatus = newStatus;
    resetIcon();
}

void StatusNotifierButton::contextMenuEvent(QContextMenuEvent* event)
{
    //XXX: avoid showing of parent's context menu, we are (optionaly) providing context menu on mouseReleaseEvent
    //QWidget::contextMenuEvent(event);
}

void StatusNotifierButton::mouseMoveEvent(QMouseEvent *e)
{

    if (e->button() == Qt::RightButton)
        return;
    if (!(e->buttons() & Qt::LeftButton))
        return;
    if ((e->pos() - mDragStart).manhattanLength() < QApplication::startDragDistance())
        return;

    if (e->modifiers() == Qt::ControlModifier)
    {
        return;
    }
    QDrag *drag = new QDrag(this);
    QIcon ico = icon();
    int size = mPlugin->panel()->iconSize();
    QPixmap img = ico.pixmap(ico.actualSize({size, size}));

    drag->setMimeData(mimeData());
    drag->setPixmap(img);

    switch (mPlugin->panel()->position())
    {
        case IUKUIPanel::PositionLeft:
        case IUKUIPanel::PositionTop:
            drag->setHotSpot({0, 0});
            break;
        case IUKUIPanel::PositionRight:
        case IUKUIPanel::PositionBottom:
            drag->setHotSpot(img.rect().bottomRight());
            break;
    }

    drag->exec();
    drag->deleteLater();

    //QAbstractButton::mouseMoveEvent(e);
}

void StatusNotifierButton::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        interface->Activate(QCursor::pos().x(), QCursor::pos().y());
    else if (event->button() == Qt::MidButton)
        interface->SecondaryActivate(QCursor::pos().x(), QCursor::pos().y());
    else if (Qt::RightButton == event->button())
    {
        if (mMenu)
        {
            mPlugin->willShowWindow(mMenu);
            mMenu->popup(mPlugin->panel()->calculatePopupWindowPos(QCursor::pos(), mMenu->sizeHint()).topLeft());
        } else
            interface->ContextMenu(QCursor::pos().x(), QCursor::pos().y());
    }

    QToolButton::mouseReleaseEvent(event);
}

void StatusNotifierButton::wheelEvent(QWheelEvent *event)
{
    interface->Scroll(event->delta(), "vertical");
}

void StatusNotifierButton::resetIcon()
{
    if (mStatus == Active && !mOverlayIcon.isNull())
        setIcon(mOverlayIcon);
    else if (mStatus == NeedsAttention && !mAttentionIcon.isNull())
        setIcon(mAttentionIcon);
    else if (!mIcon.isNull()) // mStatus == Passive
        setIcon(mIcon);
    else if (!mOverlayIcon.isNull())
        setIcon(mOverlayIcon);
    else if (!mAttentionIcon.isNull())
        setIcon(mAttentionIcon);
    else
        setIcon(mFallbackIcon);
}

void StatusNotifierButton::dragMoveEvent(QDragMoveEvent * e)
{

    if (e->mimeData()->hasFormat(MIMETYPE))
        e->acceptProposedAction();
    else
        e->ignore();

}

void StatusNotifierButton::dragEnterEvent(QDragEnterEvent *e)
{
    e->acceptProposedAction();
    const StatusNotifierButtonMimeData *mimeData = qobject_cast<const StatusNotifierButtonMimeData*>(e->mimeData());
    if (mimeData && mimeData->button()){
        emit switchButtons(mimeData->button(), this);
        emit sendTitle(mimeData->button()->hideAbleStatusNotifierButton());
    }
    QToolButton::dragEnterEvent(e);
}

QMimeData * StatusNotifierButton::mimeData()
{
    StatusNotifierButtonMimeData *mimeData = new StatusNotifierButtonMimeData();
//    QByteArray ba;
//    mimeData->setData(mimeDataFormat(), ba);
    mimeData->setButton(this);
    return mimeData;
}

void StatusNotifierButton::mousePressEvent(QMouseEvent *e)
{
//    if (e->button() == Qt::LeftButton && e->modifiers() == Qt::ControlModifier)
//    {
//        mDragStart = e->pos();
//        return;
//    }

//    QToolButton::mousePressEvent(e);
}

bool StatusNotifierButton::event(QEvent *e)
{

//    if(e->type() != QEvent::ToolTipChange && e->type()!=QEvent::HoverMove && e->type()!=QEvent::Paint &&
//            e->type() != QEvent::HoverLeave && e->type()!=QEvent::Paint &&e->type() != QEvent::DragMove &&
//            e->type() != QEvent::Leave && e->type()!=QEvent::Enter &&e->type() != QEvent::DragMove &&
//            e->type() != QEvent::Gesture && e->type() != QEvent::MouseButtonPress && e->type() != QEvent::MouseButtonRelease &&
//            e->type() != QEvent::GestureOverride && e->type() !=QEvent::HoverEnter && e->type() != QEvent::MouseMove &&
//            e->type() !=QEvent::ChildAdded   && e->type() != QEvent::DragEnter )
//        qDebug()<<e->type();

    if(e->type() == QEvent::ChildRemoved) {
        emit cleansignal();
    }
    return QToolButton::event(e);
}

QString StatusNotifierButton::hideAbleStatusNotifierButton()
{
    interface->propertyGetAsync(QLatin1String("Title"), [this] (QString title) {
        mTitle = title;
    });
    return mTitle;
}
