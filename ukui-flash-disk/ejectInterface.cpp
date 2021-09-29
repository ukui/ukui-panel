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
#include "ejectInterface.h"
#include <qgsettings.h>
#include <KWindowEffects>
#include "flashdiskdata.h"

ejectInterface::ejectInterface(QWidget *parent,QString mount_name,int deviceType,QString strDevId) : QWidget(parent),eject_image_label(nullptr),show_text_label(nullptr),
    mount_name_label(nullptr)
{
    initFontSetting();
    getFontSize();

//interface layout
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Popup);
    EjectScreen = qApp->primaryScreen();
    eject_image_label = new QLabel(this);
    eject_image_label->setFixedSize(24,24);
    //QPixmap pixmap("kylin-media-removable-symbolic");
    QString strNoraml = "";
    QString strIcon = FlashDiskData::getInstance()->getVolumeIcon(strDevId);
    eject_image_icon = QIcon::fromTheme(strIcon);
    strNoraml = mount_name;
    if (eject_image_icon.isNull()) {
        eject_image_icon = QIcon::fromTheme("kylin-media-removable-symbolic");
    }
    QPixmap pixmap = eject_image_icon.pixmap(QSize(24, 24));
    eject_image_label->setPixmap(pixmap);
    //add it to show the eject button

    show_text_label = new QLabel(this);
    show_text_label->setFixedSize(192, 36);
    show_text_label->setFont(QFont("Noto Sans CJK SC",fontSize));
    show_text_label->setAlignment(Qt::AlignHCenter);
    QString normalShow = getElidedText(show_text_label->font(),strNoraml,192);
    //add the text of the eject interface
    show_text_label->setText(normalShow);
    if (strNoraml != normalShow) {
        show_text_label->setToolTip(strNoraml);
    }

    ejectinterface_h_BoxLayout = new QHBoxLayout();
    ejectinterface_h_BoxLayout->setContentsMargins(0,0,0,0);
    ejectinterface_h_BoxLayout->setSpacing(8);
    ejectinterface_h_BoxLayout->addWidget(eject_image_label,0,Qt::AlignLeft|Qt::AlignTop);
    ejectinterface_h_BoxLayout->addWidget(show_text_label,0,Qt::AlignHCenter|Qt::AlignTop);

    QHBoxLayout *ejectTipLayout = new QHBoxLayout();
    ejectTipLayout->setContentsMargins(0,0,0,0);
    ejectTipLayout->setSpacing(0);
    QLabel *ejectTip = new QLabel();
    ejectTip->setText(tr("Storage device can be safely unplugged"));
    ejectTip->setAlignment(Qt::AlignHCenter|Qt::AlignTop);
    ejectTip->setWordWrap(true);
    ejectTipLayout->addWidget(ejectTip,0,Qt::AlignHCenter|Qt::AlignTop);

    main_V_BoxLayput = new QVBoxLayout;
    main_V_BoxLayput->setContentsMargins(8,8,8,8);
    main_V_BoxLayput->setSpacing(0);
    main_V_BoxLayput->addLayout(ejectinterface_h_BoxLayout);
    main_V_BoxLayput->addLayout(ejectTipLayout);
    main_V_BoxLayput->addStretch();

    QString strTipShow = getElidedText(ejectTip->font(),ejectTip->text(),224);
    if (strTipShow != ejectTip->text()) {
        this->setFixedSize(240,120);
    } else {
        this->setFixedSize(240,90);
    }
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Popup);
    this->setAttribute(Qt::WA_TranslucentBackground);//设置窗口背景透明
    this->setAttribute(Qt::WA_DeleteOnClose); //设置窗口关闭时自动销毁

    this->setLayout(main_V_BoxLayput);

    //set the main signal-slot function to complete the eject interface to let it disappear automatically
    interfaceHideTime = new QTimer(this);
    interfaceHideTime->setTimerType(Qt::PreciseTimer);
    connect(interfaceHideTime, SIGNAL(timeout()), this, SLOT(on_interface_hide()));
    interfaceHideTime->start(1000);
    moveEjectInterfaceRight();
    initTransparentState();
    this->getTransparentData();
}

ejectInterface::~ejectInterface()
{
    if (m_transparency_gsettings) {
        delete m_transparency_gsettings;
        m_transparency_gsettings = nullptr;
    }
    if (fontSettings) {
        delete fontSettings;
        fontSettings = nullptr;
    }
}

//If the fillet does not take effect
void ejectInterface::paintEvent(QPaintEvent *event)
 {
    QPainterPath path;
    auto rect = this->rect();
    rect.adjust(1, 1, -1, -1);
    path.addRoundedRect(rect, 6, 6);
    setProperty("blurRegion", QRegion(path.toFillPolygon().toPolygon()));

    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    QRect rectReal = this->rect();
    p.setRenderHint(QPainter::Antialiasing);  // 反锯齿;
    p.setBrush(opt.palette.color(QPalette::Base));
    p.setOpacity(m_transparency);
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(rectReal, 6, 6);
    QWidget::paintEvent(event);

    KWindowEffects::enableBlurBehind(this->winId(), true, QRegion(path.toFillPolygon().toPolygon()));
 }

//slot function to hide eject interface
void ejectInterface::on_interface_hide()
{
    this->hide();
    this->deleteLater();
}

//set the location of the eject interface
void ejectInterface::moveEjectInterfaceRight()
{
//    if(EjectScreen->availableGeometry().x() == EjectScreen->availableGeometry().y() && EjectScreen->availableSize().height() < EjectScreen->size().height())
//    {
//        qDebug()<<"the position of panel is down";
//        this->move(EjectScreen->availableGeometry().x() + EjectScreen->size().width() -
//                   this->width() - DistanceToPanel,EjectScreen->availableGeometry().y() +
//                   EjectScreen->availableSize().height() - this->height() - DistanceToPanel);
//    }

//    if(EjectScreen->availableGeometry().x() < EjectScreen->availableGeometry().y() && EjectScreen->availableSize().height() < EjectScreen->size().height())
//    {
//        qDebug()<<"this position of panel is up";
//        this->move(EjectScreen->availableGeometry().x() + EjectScreen->size().width() -
//                   this->width() - DistanceToPanel,EjectScreen->availableGeometry().y());
//    }

//    if(EjectScreen->availableGeometry().x() > EjectScreen->availableGeometry().y() && EjectScreen->availableSize().width() < EjectScreen->size().width())
//    {
//        qDebug()<<"this position of panel is left";
//        this->move(EjectScreen->availableGeometry().x() + DistanceToPanel,EjectScreen->availableGeometry().y()
//                   + EjectScreen->availableSize().height() - this->height());
//    }

//    if(EjectScreen->availableGeometry().x() == EjectScreen->availableGeometry().y() && EjectScreen->availableSize().width() < EjectScreen->size().width())
//    {
//        qDebug()<<"this position of panel is right";
//        this->move(EjectScreen->availableGeometry().x() + EjectScreen->availableSize().width() -
//                   DistanceToPanel - this->width(),EjectScreen->availableGeometry().y() +
//                   EjectScreen->availableSize().height() - (this->height())*(DistanceToPanel - 1));
//    }

//show the ejectinterface by primaryscreen()
    int position=0;
    int panelSize=0;
    if(QGSettings::isSchemaInstalled(QString("org.ukui.panel.settings").toLocal8Bit()))
    {
        QGSettings* gsetting=new QGSettings(QString("org.ukui.panel.settings").toLocal8Bit());
        if(gsetting->keys().contains(QString("panelposition")))
            position=gsetting->get("panelposition").toInt();
        else
            position=0;
        if(gsetting->keys().contains(QString("panelsize")))
            panelSize=gsetting->get("panelsize").toInt();
        else
            panelSize=SmallPanelSize;
    }
    else
    {
        position=0;
        panelSize=SmallPanelSize;
    }

    int x=QApplication::primaryScreen()->geometry().x();
    int y=QApplication::primaryScreen()->geometry().y();

    if(position==0)
        this->setGeometry(QRect(x + QApplication::primaryScreen()->geometry().width()-this->width(),y+QApplication::primaryScreen()->geometry().height()-panelSize-this->height(),this->width(),this->height()));
    else if(position==1)
        this->setGeometry(QRect(x + QApplication::primaryScreen()->geometry().width()-this->width(),y+panelSize,this->width(),this->height()));  // Style::minw,Style::minh the width and the height of the interface  which you want to show
    else if(position==2)
        this->setGeometry(QRect(x+panelSize,y + QApplication::primaryScreen()->geometry().height() - this->height(),this->width(),this->height()));
    else
        this->setGeometry(QRect(x+QApplication::primaryScreen()->geometry().width()-panelSize-this->width(),y + QApplication::primaryScreen()->geometry().height() - this->height(),this->width(),this->height()));
}

int ejectInterface::getPanelPosition(QString str)
{
    QDBusInterface interface( "com.ukui.panel.desktop",
                              "/",
                              "com.ukui.panel.desktop",
                              QDBusConnection::sessionBus() );
    QDBusReply<int> reply = interface.call("GetPanelPosition", str);

    return reply;
}

/*
    use the dbus to get the height of the panel
*/
int ejectInterface::getPanelHeight(QString str)
{
    QDBusInterface interface( "com.ukui.panel.desktop",
                              "/",
                              "com.ukui.panel.desktop",
                              QDBusConnection::sessionBus() );
    QDBusReply<int> reply = interface.call("GetPanelSize", str);
    return reply;
}

void ejectInterface::initTransparentState()
{
    const QByteArray idtrans(THEME_QT_TRANS);

    if(QGSettings::isSchemaInstalled(idtrans))
    {
        m_transparency_gsettings = new QGSettings(idtrans);
    }
}

void ejectInterface::getTransparentData()
{
    if (!m_transparency_gsettings)
    {
       m_transparency = 0.95;
       return;
    }

    QStringList keys = m_transparency_gsettings->keys();
    if (keys.contains("transparency"))
    {
        m_transparency = m_transparency_gsettings->get("transparency").toDouble();
    }

}

void ejectInterface::initFontSetting()
{
    const QByteArray id(THEME_QT_SCHEMA);

    if(QGSettings::isSchemaInstalled(id))
    {
        fontSettings = new QGSettings(id);
    }
}

void ejectInterface::getFontSize()
{

    if (!fontSettings)
    {
       fontSize = 11;
       return;
    }

    QStringList keys = fontSettings->keys();
    if (keys.contains("systemFont") || keys.contains("systemFontSize"))
    {
        fontSize = fontSettings->get("system-font").toInt();
    }
}
