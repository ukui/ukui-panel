/********************************************************************************
** Form generated from reading UI file 'configpluginswidget.ui'
**
** Created by: Qt User Interface Compiler version 5.12.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONFIGPLUGINSWIDGET_H
#define UI_CONFIGPLUGINSWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListView>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ConfigPluginsWidget
{
public:
    QHBoxLayout *horizontalLayout;
    QWidget *widget;
    QVBoxLayout *verticalLayout;
    QListView *listView_plugins;
    QLabel *label;
    QWidget *widget_5;
    QVBoxLayout *verticalLayout_3;
    QToolButton *pushButton_moveUp;
    QToolButton *pushButton_moveDown;
    QFrame *line;
    QToolButton *pushButton_addPlugin;
    QToolButton *pushButton_removePlugin;
    QFrame *line_2;
    QToolButton *pushButton_pluginConfig;
    QSpacerItem *verticalSpacer;

    void setupUi(QWidget *ConfigPluginsWidget)
    {
        if (ConfigPluginsWidget->objectName().isEmpty())
            ConfigPluginsWidget->setObjectName(QString::fromUtf8("ConfigPluginsWidget"));
        ConfigPluginsWidget->resize(339, 220);
        horizontalLayout = new QHBoxLayout(ConfigPluginsWidget);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        widget = new QWidget(ConfigPluginsWidget);
        widget->setObjectName(QString::fromUtf8("widget"));
        verticalLayout = new QVBoxLayout(widget);
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        listView_plugins = new QListView(widget);
        listView_plugins->setObjectName(QString::fromUtf8("listView_plugins"));
        listView_plugins->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
        listView_plugins->setAlternatingRowColors(true);
        listView_plugins->setSelectionMode(QAbstractItemView::SingleSelection);
        listView_plugins->setSelectionBehavior(QAbstractItemView::SelectRows);
        listView_plugins->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
        listView_plugins->setFlow(QListView::TopToBottom);
        listView_plugins->setResizeMode(QListView::Adjust);
        listView_plugins->setSpacing(0);
        listView_plugins->setUniformItemSizes(false);
        listView_plugins->setSelectionRectVisible(true);

        verticalLayout->addWidget(listView_plugins);

        label = new QLabel(widget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setWordWrap(true);

        verticalLayout->addWidget(label);


        horizontalLayout->addWidget(widget);

        widget_5 = new QWidget(ConfigPluginsWidget);
        widget_5->setObjectName(QString::fromUtf8("widget_5"));
        verticalLayout_3 = new QVBoxLayout(widget_5);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        verticalLayout_3->setContentsMargins(0, 0, 0, 0);
        pushButton_moveUp = new QToolButton(widget_5);
        pushButton_moveUp->setObjectName(QString::fromUtf8("pushButton_moveUp"));
        QIcon icon;
        QString iconThemeName = QString::fromUtf8("go-up");
        if (QIcon::hasThemeIcon(iconThemeName)) {
            icon = QIcon::fromTheme(iconThemeName);
        } else {
            icon.addFile(QString::fromUtf8("../../../../../.designer/backup"), QSize(), QIcon::Normal, QIcon::Off);
        }
        pushButton_moveUp->setIcon(icon);

        verticalLayout_3->addWidget(pushButton_moveUp);

        pushButton_moveDown = new QToolButton(widget_5);
        pushButton_moveDown->setObjectName(QString::fromUtf8("pushButton_moveDown"));
        QIcon icon1;
        iconThemeName = QString::fromUtf8("go-down");
        if (QIcon::hasThemeIcon(iconThemeName)) {
            icon1 = QIcon::fromTheme(iconThemeName);
        } else {
            icon1.addFile(QString::fromUtf8("../../../../../.designer/backup"), QSize(), QIcon::Normal, QIcon::Off);
        }
        pushButton_moveDown->setIcon(icon1);

        verticalLayout_3->addWidget(pushButton_moveDown);

        line = new QFrame(widget_5);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        verticalLayout_3->addWidget(line);

        pushButton_addPlugin = new QToolButton(widget_5);
        pushButton_addPlugin->setObjectName(QString::fromUtf8("pushButton_addPlugin"));
        QIcon icon2;
        iconThemeName = QString::fromUtf8("list-add");
        if (QIcon::hasThemeIcon(iconThemeName)) {
            icon2 = QIcon::fromTheme(iconThemeName);
        } else {
            icon2.addFile(QString::fromUtf8("../../../../../.designer/backup"), QSize(), QIcon::Normal, QIcon::Off);
        }
        pushButton_addPlugin->setIcon(icon2);

        verticalLayout_3->addWidget(pushButton_addPlugin);

        pushButton_removePlugin = new QToolButton(widget_5);
        pushButton_removePlugin->setObjectName(QString::fromUtf8("pushButton_removePlugin"));
        QIcon icon3;
        iconThemeName = QString::fromUtf8("list-remove");
        if (QIcon::hasThemeIcon(iconThemeName)) {
            icon3 = QIcon::fromTheme(iconThemeName);
        } else {
            icon3.addFile(QString::fromUtf8("../../../../../.designer/backup"), QSize(), QIcon::Normal, QIcon::Off);
        }
        pushButton_removePlugin->setIcon(icon3);

        verticalLayout_3->addWidget(pushButton_removePlugin);

        line_2 = new QFrame(widget_5);
        line_2->setObjectName(QString::fromUtf8("line_2"));
        line_2->setFrameShape(QFrame::HLine);
        line_2->setFrameShadow(QFrame::Sunken);

        verticalLayout_3->addWidget(line_2);

        pushButton_pluginConfig = new QToolButton(widget_5);
        pushButton_pluginConfig->setObjectName(QString::fromUtf8("pushButton_pluginConfig"));
        QIcon icon4;
        iconThemeName = QString::fromUtf8("preferences-other");
        if (QIcon::hasThemeIcon(iconThemeName)) {
            icon4 = QIcon::fromTheme(iconThemeName);
        } else {
            icon4.addFile(QString::fromUtf8("../../../../../.designer/backup"), QSize(), QIcon::Normal, QIcon::Off);
        }
        pushButton_pluginConfig->setIcon(icon4);

        verticalLayout_3->addWidget(pushButton_pluginConfig);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_3->addItem(verticalSpacer);


        horizontalLayout->addWidget(widget_5);


        retranslateUi(ConfigPluginsWidget);

        QMetaObject::connectSlotsByName(ConfigPluginsWidget);
    } // setupUi

    void retranslateUi(QWidget *ConfigPluginsWidget)
    {
        ConfigPluginsWidget->setWindowTitle(QApplication::translate("ConfigPluginsWidget", "Configure Plugins", nullptr));
        label->setText(QApplication::translate("ConfigPluginsWidget", "Note: changes made in this page cannot be reset.", nullptr));
#ifndef QT_NO_TOOLTIP
        pushButton_moveUp->setToolTip(QApplication::translate("ConfigPluginsWidget", "Move up", nullptr));
#endif // QT_NO_TOOLTIP
        pushButton_moveUp->setText(QApplication::translate("ConfigPluginsWidget", "...", nullptr));
#ifndef QT_NO_TOOLTIP
        pushButton_moveDown->setToolTip(QApplication::translate("ConfigPluginsWidget", "Move down", nullptr));
#endif // QT_NO_TOOLTIP
        pushButton_moveDown->setText(QApplication::translate("ConfigPluginsWidget", "...", nullptr));
#ifndef QT_NO_TOOLTIP
        pushButton_addPlugin->setToolTip(QApplication::translate("ConfigPluginsWidget", "Add", nullptr));
#endif // QT_NO_TOOLTIP
        pushButton_addPlugin->setText(QApplication::translate("ConfigPluginsWidget", "...", nullptr));
#ifndef QT_NO_TOOLTIP
        pushButton_removePlugin->setToolTip(QApplication::translate("ConfigPluginsWidget", "Remove", nullptr));
#endif // QT_NO_TOOLTIP
        pushButton_removePlugin->setText(QApplication::translate("ConfigPluginsWidget", "...", nullptr));
#ifndef QT_NO_TOOLTIP
        pushButton_pluginConfig->setToolTip(QApplication::translate("ConfigPluginsWidget", "Configure", nullptr));
#endif // QT_NO_TOOLTIP
        pushButton_pluginConfig->setText(QApplication::translate("ConfigPluginsWidget", "...", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ConfigPluginsWidget: public Ui_ConfigPluginsWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONFIGPLUGINSWIDGET_H
