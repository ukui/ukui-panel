#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Popup);
    this->setWindowOpacity(0.95);
    this->resize( QSize( 250, 158 ));

    //添加这个区域的垂直布局
    vboxlayout = new QVBoxLayout();
//    this->setSizeConstraint(QLayout::SetFixedSize);

//    .setSizeConstraint(QLayout::SetFixedSize);
//    vboxlayout =new QBoxLayout(QBoxLayout::BottomToTop);

    ui->centralWidget->setLayout(vboxlayout);

    //框架的样式设置
    ui->centralWidget->setStyleSheet(
                "#centralWidget{"
                "width:250px;"
                "height:158px;"
                "background:rgb(14,19,22);"
                "border:1px rgba(255, 255, 255, 0.05);"
                "opacity:0.75;"
                "border-radius:1px;"
                "}"
                );

//    m_ScrollArea->setStyleSheet("QScrollArea{border:none;}");
//    m_ScrollArea->viewport()->setStyleSheet("background-color:transparent;");
//    m_ScrollArea->verticalScrollBar()->setStyleSheet("QScrollBar:vertical{margin:0px 0px 0px 0px;background:transparent;border:0px;width:6px;}"
//                                                    "QScrollBar::up-arrow:vertical{height:0px;}"
//                                                    "QScrollBar::sub-line:vertical{border:0px solid;height:0px}"
//                                                    "QScrollBar::sub-page:vertical{background:transparent;}"
//                                                    "QScrollBar::handle:vertical{background-color:#3593b5;}"
//                                                    "QScrollBar::handle:vertical:hover{background-color:#3593b5;}"
//                                                    "QScrollBar::handle:vertical:pressed{background-color:#3593b5;}"
//                                                    "QScrollBar::add-page:vertical{background:transparent;}"
//                                                    "QScrollBar::add-line:vertical{border:0px solid;height:0px}"
//                                                    "QScrollBar::down-arrow:vertical{height:0px;}");

    m_systray.setIcon(QIcon("/usr/share/icons/ukui-icon-theme-default/22x22/devices/drive-removable-media.png"));
    connect(&m_systray, &QSystemTrayIcon::activated, this, &MainWindow::iconActivated);
    m_systray.show();
}

MainWindow::~MainWindow()
{
    delete ui;
}

int MainWindow::getdevicenum(){
    int number=0;
    QString UDiskPath = "";
    foreach (const QStorageInfo &storage, QStorageInfo::mountedVolumes()) {
        UDiskPath = storage.rootPath();
        if(UDiskPath.contains("media")){
            number++;
        }
    }
    qDebug() << number << "tttttttttttttttttttttttttttttttttttttttttttttttttttttt";
    return number;
}
void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{


//    this->resize(250, 158);
//   QSize minimumWindowSize=this->window()->size();
//   this->window()->resize(minimumWindowSize);

//    this->resize(250, 158);
//    QRect rect(0, 0, 10, 10);
//    this->setGeometry(rect);
//    this->adjustSize();
//    this->setStyleSheet(
//                "MainWindow{"
//                    "height:158px;"
//                "}"
//                );
//    ui->centralWidget->setStyleSheet(
//                "#centralWidget{"
//                "width:250px;"
//                "height:158px;"
//                "background:rgb(14,19,22);"
//                "border:1px rgba(255, 255, 255, 0.05);"
//                "opacity:0.75;"
//                "border-radius:1px;"
//                "}"
//                );


    qDebug() << "MainWindow::iconActivated--------------";
    int num=0;

    if ( this->vboxlayout != NULL )
    {
        QLayoutItem* item;
        while ( ( item = this->vboxlayout->takeAt( 0 ) ) != NULL )
        {
            delete item->widget();
            delete item;
        }
    }

    //    switch (reason) {
    //    case QSystemTrayIcon::Trigger:
    //    case QSystemTrayIcon::DoubleClick:
    //    case QSystemTrayIcon::MiddleClick:
    //        break;
    //    }

    switch(reason){
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::MiddleClick:
        //这里右键点击托盘图标无效
        if(this->isHidden()){
//qk
            QString UDiskPath = "";
            foreach (const QStorageInfo &storage, QStorageInfo::mountedVolumes()) {
                if (storage.isValid() && storage.isReady()){

                    UDiskPath = storage.rootPath();
                    if(UDiskPath.contains("media")){
                        num++;
//                        this->resize(250, 158*num);
                        this->setFixedSize(250,158*num);
                        qDebug() << "pppppppppppppppppppppppppppppppppppppppppppppppppp" << 158*num << this->height();
                        if (getdevicenum() == 1){
                            newarea(storage.name(),storage.bytesTotal(),UDiskPath,0);
                        } else if(num ==1){
                            qDebug() << "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa--1";
                            newarea(storage.name(),storage.bytesTotal(),UDiskPath,1);

                        }else{
                            qDebug() << "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa--2";
                            newarea(storage.name(),storage.bytesTotal(),UDiskPath,2);
                        }
                        qDebug() << "device path:" << UDiskPath;
                        qDebug() << "size:" << storage.bytesTotal()/1000/1000 << "MB";
                        qDebug() << "name:" << storage.name();
                    }
                }

//               qDebug() << storage.rootPath();
//               if (storage.isReadOnly())
//                   qDebug() << "isReadOnly:" << storage.isReadOnly();

//               qDebug() << "name:" << storage.name();
//               qDebug() << "fileSystemType:" << storage.fileSystemType();
//               qDebug() << "size:" << storage.bytesTotal()/1000/1000 << "MB";
//               qDebug() << "availableSize:" << storage.bytesAvailable()/1000/1000 << "MB";

//               QByteArray qbaPath = storage.device();
//               if (qbaPath.startsWith("/dev/sd")) {
//                    qDebug() << "kkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkk" << qbaPath << storage.rootPath();
//               }
            }
            if(num != 0){
               this->showNormal();
                moveBottomRight();
            } else{
                this->no_device_label = new QLabel("未插入移动设备！");
                this->no_device_label->setAlignment(Qt::AlignCenter);
                this->no_device_label->setStyleSheet(
                            "QLabel{"
                            "width:111px;"
                            "height:14px;"
                            "font-size:14px;"
                            "font-family:Microsoft YaHei;"
                            "font-weight:400;"
                            "color:rgba(255,255,255,1);"
                            "line-height:28px;"
                            "}"
                            );
                this->vboxlayout->addWidget(no_device_label);
                qDebug() << "kkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkk";
//                this->resize(250, 60);
                this->setFixedSize(250,60);
                this->showNormal();

                moveBottomRight();
            }
        }else{
            this->hide();
        }

        break;
    default:
        break;

    }
}

void MainWindow::moveBottomRight()
{
    //获取鼠标位置
    QPoint globalCursorPos = QCursor::pos();

    QRect availableGeometry = qApp->primaryScreen()->availableGeometry();
    QRect screenGeometry = qApp->primaryScreen()->geometry();

    QDesktopWidget* pDesktopWidget = QApplication::desktop();
//    int nScreenCount = QApplication::desktop()->screenCount();
//    QRect deskRect = pDesktopWidget->availableGeometry();//可用区域
    QRect screenRect = pDesktopWidget->screenGeometry();//屏幕区域

    if (screenRect.height() != availableGeometry.height()) {
        this->move(globalCursorPos.x()-125, availableGeometry.height() - this->height());
//            int num=this->height()/158;
//            int remainder=this->height()%158;

//            if (remainder==0){
//                qDebug() << "aaaaaaaaaaaaaaaa";
//                qDebug() << num * 158 + (num+4)*3;
//                int overage=num * 158 + (num+4)*3;
//                this->move(globalCursorPos.x()-125, availableGeometry.height() - overage);
//            } else{
//                qDebug() << availableGeometry.height() << this->height();
//                qDebug() << "height===============================================" << availableGeometry.height() - this->height();
//                this->move(globalCursorPos.x()-125, availableGeometry.height() - this->height());
//    //            this->move(globalCursorPos.x()-125, availableGeometry.height() - this->height() - 158);
//            }
    }else {
            this->move(globalCursorPos.x()-125, availableGeometry.height() - this->height() - 40);
    }
}

void MainWindow::mouseClicked()
{
    qDebug() << "aaaaa";
    printf("asssssssssssssssss\n");
}
QString MainWindow::size_human(qlonglong capacity)
{
//    float capacity = this->size();
    QStringList list;
    list << "KB" << "MB" << "GB" << "TB";

    QStringListIterator i(list);
    QString unit("bytes");

    while(capacity >= 1024.0 && i.hasNext())
     {
        unit = i.next();
        capacity /= 1024.0;
    }
    qDebug() << "capacity==============" << capacity << unit;
    QString str_capacity=QString(" %1%2").arg(capacity).arg(unit);
    return str_capacity;
 //   return QString().setNum(capacity,'f',2)+" "+unit;
}


void MainWindow::newarea(QString name, qlonglong capacity, QString path, int linestatus){
    QLabel *eject_label, *open_label;
    QString  str_capacity=size_human(capacity);

    /*----------------添加弹出设备------------------*/
    QClickWidget *eject_widget;
    eject_widget = new QClickWidget(NULL,true,name,capacity,path);

    //添加弹出区域的垂直布局
    QVBoxLayout *eject_vboxlayout = new QVBoxLayout;
    //添加弹出区域的水平布局
    QHBoxLayout *eject_hboxlayout = new QHBoxLayout;
    //添加弹出图标
    QPushButton *eject_image_button= new QPushButton();
    QIcon eject_icon("/usr/share/icons/ukui-icon-theme-default/22x22/emblems/emblem-unreadable.png");
    eject_image_button->setIcon(eject_icon);
    eject_image_button->setFlat(true);
    eject_image_button->setDisabled(true);
    //添加弹出按钮
    if (name.isEmpty()){
        eject_label = new QLabel("弹出  "+ str_capacity+" 卷");
        qDebug() << "eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee-0";
    } else{
        eject_label = new QLabel("弹出  "+name);
            qDebug() << "eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee-1";
    }
    eject_label->setDisabled(true);
//    eject_label->setFlat(true);
//    eject_label->setDisabled(true);
    //添加容量
    QLabel *eject_capacity = new QLabel(this);
//    QString str_capacity=QString("- %1(磁盘容量)").arg(capacity);
//    eject_capacity->setText("- UKUI(磁盘容量)");
    eject_capacity->setText(str_capacity+" (磁盘容量)");
    eject_capacity->setAlignment(Qt::AlignCenter);
    eject_capacity->setDisabled(true);

//    eject_hboxlayout->setContentsMargins(0,9,0,9);
    eject_hboxlayout->addWidget(eject_image_button);
    eject_hboxlayout->addWidget(eject_label);
    eject_hboxlayout->addStretch();
    eject_vboxlayout->addLayout(eject_hboxlayout);
    eject_vboxlayout->addWidget(eject_capacity);

    /*----------------添加打开设备------------------*/
    QClickWidget *open_widget;
    open_widget = new QClickWidget(NULL,false,name,capacity,path);
    QVBoxLayout *open_vboxlayout = new QVBoxLayout;
    QHBoxLayout *open_hboxlayout = new QHBoxLayout;
    QPushButton *open_image_button= new QPushButton();
    QIcon open_icon("/usr/share/icons/ukui-icon-theme-default/22x22/emblems/emblem-default.png");
    open_image_button->setIcon(open_icon);
    open_image_button->setFlat(true);
    open_image_button->setDisabled(true);
    if (name.isEmpty()){
        open_label = new QLabel("打开  "+str_capacity+" 卷");
        qDebug() << "eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee-0";
    } else{
        open_label = new QLabel("打开  "+name);
            qDebug() << "eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee-1";
    }

    open_label->setDisabled(true);

    QLabel *open_capacity = new QLabel(this);
//    open_capacity->setText("- UKUI(磁盘容量)");
    open_capacity->setText(str_capacity+" (磁盘容量)");
    open_capacity->setAlignment(Qt::AlignCenter);
    open_capacity->setDisabled(true);

    //添加布局
    open_hboxlayout->addWidget(open_image_button);

    open_hboxlayout->addWidget(open_label);
    open_hboxlayout->addStretch();
    open_vboxlayout->addLayout(open_hboxlayout);
    open_vboxlayout->addWidget(open_capacity);

    //分割线
    QWidget *line = new QWidget;
    line->setFixedHeight(1);
    line->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
//    eject_vboxlayout->setContentsMargins(0,9,0,9);
    if (linestatus == 2){
        this->vboxlayout->addWidget(line);
    }
    eject_widget->setLayout(eject_vboxlayout);
    this->vboxlayout->addWidget(eject_widget);
    if (linestatus == 0){
        this->vboxlayout->addWidget(line);
    }
//   open_vboxlayout->setContentsMargins(0,9,0,9);
    open_widget->setLayout(open_vboxlayout);
    this->vboxlayout->addWidget(open_widget);
//    vboxlayout->setContentsMargins(0,9,0,9);

    eject_widget->setStyleSheet(
                //正常状态样式
                "QClickWidget{"
                "height:79px;"
                "}"
                "QClickWidget:hover{"
                "background-color:rgba(255,255,255,30);"
                "}"
                );
    open_widget->setStyleSheet(
                //正常状态样式
                "QClickWidget{"
                "height:79px;"
                "}"
                "QClickWidget:hover{"
                "background-color:rgba(255,255,255,30);"
                "}"
                );
    line->setStyleSheet(
                "QWidget{"
                "background-color: rgba(255,255,255,0.2);"
                "width:244px;"
                "height:1px;"
                "}"
                );
    eject_capacity->setStyleSheet(
                "QLabel{"
                "width:111px;"
                "height:14px;"
                "font-size:14px;"
                "font-family:Microsoft YaHei;"
                "font-weight:400;"
                "color:rgba(255,255,255,0.35);"
                "line-height:28px;"
                "}"
                );
    open_capacity->setStyleSheet(
                "QLabel{"
                "width:111px;"
                "height:14px;"
                "font-size:14px;"
                "font-family:Microsoft YaHei;"
                "font-weight:400;"
                "color:rgba(255,255,255,0.35);"
                "line-height:28px;"
                "opacity:0.35;"
                "}"
                );
    eject_image_button->setStyleSheet(
                    "QPushButton{"
                        "height:15px;"
                    "}"
                );
    open_image_button->setStyleSheet(
                    "QPushButton{"
                        "height:15px;"
                    "}"
                );

    //设置弹出按钮样式
    eject_label->setStyleSheet(
                //正常状态样式
                "QLabel{"
                "width:167px;"
                "height:15px;"
                "font-size:16px;"
                "font-family:Microsoft YaHei;"
                "font-weight:400;"
                "color:rgba(255,255,255,1);"
                "line-height:28px;"
                "opacity:0.91;"
                "}"
//                 鼠标按下样式
                 "QPushButton:pressed{"
//                 "background-color:rgba(255,255,255,0);"
//                "background-color:rgba(100,255,100,0);"
//               "background-color:transparent;"
//                "background-color: rgba(14,19,22,1);"
//                "border-width:0px;"
//                 "border-color:rgba(255,255,255,30);"
//                 "border-style:inset;"
//                 "color:rgba(0,0,0,100);"
//                   "color:rgba(100,255,100,200);"
//                    "color:rgba(14,19,22,1)"
                 "}"
                //鼠标悬停样式
                     "QPushButton:hover{"
 //                    "background-color:rgba(100,255,100,100);"
//                     "border-color:rgba(255,255,255,200);"
//                     "color:rgba(100,255,100,200);"
                     "}"
                 );

    open_label->setStyleSheet(
                //正常状态样式
                "QLabel{"
                "width:167px;"
                "height:15px;"
                "font-size:16px;"
                "font-family:Microsoft YaHei;"
                "font-weight:400;"
                "color:rgba(255,255,255,1);"
                "line-height:28px;"
                "opacity:0.91;"
                "}"
//                 鼠标按下样式
                 "QPushButton:pressed{"
//                 "background-color:rgba(255,255,255,0);"
//                "background-color:rgba(100,255,100,0);"
//               "background-color:transparent;"
//                "background-color: rgba(14,19,22,1);"
//                "border-width:0px;"
//                 "border-color:rgba(255,255,255,30);"
//                 "border-style:inset;"
//                 "color:rgba(0,0,0,100);"
//                   "color:rgba(100,255,100,200);"
//                    "color:rgba(14,19,22,1)"
                 "}"
                //鼠标悬停样式
                     "QPushButton:hover{"
 //                    "background-color:rgba(100,255,100,100);"
//                     "border-color:rgba(255,255,255,200);"
//                     "color:rgba(100,255,100,200);"
                     "}"
                 );
}

