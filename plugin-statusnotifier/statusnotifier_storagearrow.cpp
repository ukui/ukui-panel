#include "statusnotifier_storagearrow.h"
#include "statusnotifierbutton.h"
#include "../panel/customstyle.h"

StatusNotifierStorageArrow::StatusNotifierStorageArrow(StatusNotifierWidget *parent)
{
    mParent=parent;
    this->setParent(parent);
    this->setAcceptDrops(true);
    this->setStyle(new CustomStyle);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    const QByteArray id(UKUI_PANEL_SETTINGS);
    if(QGSettings::isSchemaInstalled(id)){
        mGsettings = new QGSettings(id);
        connect(mGsettings, &QGSettings::changed, this, [=] (const QString &key){
            if(key == PANEL_POSITION_KEY){
                mPanelPosition=mGsettings->get(PANEL_POSITION_KEY).toInt();
                setArrowIcon();
            }
        });
    }
    QTimer::singleShot(10,this,[=](){
       setArrowIcon();
    });
    this->setProperty("useIconHighlightEffect", 0x2);
}

StatusNotifierStorageArrow::~StatusNotifierStorageArrow()
{

}

void StatusNotifierStorageArrow::dropEvent(QDropEvent *event){
}

void StatusNotifierStorageArrow::dragEnterEvent(QDragEnterEvent *event){
    const StatusNotifierButtonMimeData *mimeData = qobject_cast<const StatusNotifierButtonMimeData*>(event->mimeData());
    if (mimeData && mimeData->button()){
        emit addButton(mimeData->button()->mTitle);
    }

    if(mGsettings){
        mGsettings->set(SHOW_STATUSNOTIFIER_BUTTON,true);
        setArrowIcon();
    }
    event->accept();
    QToolButton::dragEnterEvent(event);
}

void StatusNotifierStorageArrow::mousePressEvent(QMouseEvent *)
{
    if(mParent->Direction){
        if(mGsettings->get(SHOW_STATUSNOTIFIER_BUTTON).toBool()){
            setIcon(QIcon::fromTheme("pan-start-symbolic"));
            mGsettings->set(SHOW_STATUSNOTIFIER_BUTTON,false);
        }
        else{
            setIcon(QIcon::fromTheme("pan-end-symbolic"));
            mGsettings->set(SHOW_STATUSNOTIFIER_BUTTON,true);
        }
    }else{
        if(mGsettings->get(SHOW_STATUSNOTIFIER_BUTTON).toBool()){
            setIcon(QIcon::fromTheme("pan-up-symbolic"));
            mGsettings->set(SHOW_STATUSNOTIFIER_BUTTON,false);
        }
        else{
            setIcon(QIcon::fromTheme("pan-down-symbolic"));
            mGsettings->set(SHOW_STATUSNOTIFIER_BUTTON,true);
        }
    }
}

void StatusNotifierStorageArrow::setArrowIcon()
{
    if(mParent->Direction){
        if(mGsettings->get(SHOW_STATUSNOTIFIER_BUTTON).toBool()){
            setIcon(QIcon::fromTheme("pan-end-symbolic"));
        }
        else{
            setIcon(QIcon::fromTheme("pan-start-symbolic"));
        }
    }else{
        if(mGsettings->get(SHOW_STATUSNOTIFIER_BUTTON).toBool()){
            setIcon(QIcon::fromTheme("pan-down-symbolic"));
        }
        else{
            setIcon(QIcon::fromTheme("pan-up-symbolic"));
        }
    }
}

void StatusNotifierStorageArrow::resizeEvent(QResizeEvent *event){
    if (mParent->Direction)
    {
        this->setIconSize(QSize(this->width()*0.5,this->width()*0.5));
    }
    else
    {
        this->setIconSize(QSize(this->height()*0.5,this->height()*0.5));
    }

     QToolButton::resizeEvent(event);
}
