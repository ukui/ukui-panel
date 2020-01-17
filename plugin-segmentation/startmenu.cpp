#include "startmenu.h"
#include <QMouseEvent>
#include <QHBoxLayout>
#include <QScreen>
#include <QDebug>

#define DESKTOP_HEIGHT  (12)
#define DESKTOP_WIDTH   (46)

Segmentation::Segmentation(const IUKUIPanelPluginStartupInfo &startupInfo) :
    QObject(),
    IUKUIPanelPlugin(startupInfo)
{
    mButtonx.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mButtonx.setAutoRaise(true);
    realign();


}


Segmentation::~Segmentation()
{
}


void Segmentation::realign()
{
    if(panel()->isHorizontal())
    {

        mButtonx.setFixedSize(DESKTOP_HEIGHT,panel()->iconSize());
        mButtonx.setStyleSheet(
                    //正常状态样式
                    "QToolButton{"
                    /*"background-color:rgba(100,225,100,80%);"//背景色（也可以设置图片）*/
                    "qproperty-icon:url(/usr/share/plugin-startmenu/img/segmentation.svg);"
                    "border-style:outset;"                  //边框样式（inset/outset）
                    "border-width:0px;"                     //边框宽度像素
                    "border-radius:0px;"                   //边框圆角半径像素
                    "border-color:rgba(255,255,255,30);"    //边框颜色
                    "font:SimSun 14px;"                       //字体，字体大小
                    "color:rgba(0,0,0,100);"                //字体颜色
                    "padding:0px;"                          //填衬
                    "border-bottom-style:solid"
                    "}"
                    );
    }
    else
    {
        mButtonx.setFixedSize(DESKTOP_WIDTH,DESKTOP_HEIGHT);
        mButtonx.setStyleSheet(
                    //正常状态样式
                    "QToolButton{"
                    /*"background-color:rgba(100,225,100,80%);"//背景色（也可以设置图片）*/
                    "qproperty-icon:url(/usr/share/plugin-startmenu/img/segmentation.svg);"
                    "qproperty-iconSize:40px 40px;"
                    "border-style:outset;"                  //边框样式（inset/outset）
                    "border-width:0px;"                     //边框宽度像素
                    "border-radius:0px;"                   //边框圆角半径像素
                    "border-color:rgba(255,255,255,30);"    //边框颜色
                    "font:SimSun 14px;"                       //字体，字体大小
                    "color:rgba(0,0,0,100);"                //字体颜色
                    "padding:0px;"                          //填衬
                    "border-bottom-style:solid"
                    "}"
                    //鼠标悬停样式
                    "QToolButton:hover{"
                    "background-color:rgba(190,216,239,20%);"
                    "}"
                    //鼠标按下样式
                    "QToolButton:pressed{"
                    "background-color:rgba(190,216,239,12%);"
                    "}"
                    );
    }

}
