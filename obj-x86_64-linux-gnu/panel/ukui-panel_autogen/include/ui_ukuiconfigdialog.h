/********************************************************************************
** Form generated from reading UI file 'ukuiconfigdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.12.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_UKUICONFIGDIALOG_H
#define UI_UKUICONFIGDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <common/configdialog/ukuipageselectwidget.h>

QT_BEGIN_NAMESPACE

class Ui_ConfigDialog
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    UKUi::PageSelectWidget *moduleList;
    QVBoxLayout *verticalLayout_3;
    QStackedWidget *stackedWidget;
    QDialogButtonBox *buttons;

    void setupUi(QWidget *ConfigDialog)
    {
        if (ConfigDialog->objectName().isEmpty())
            ConfigDialog->setObjectName(QString::fromUtf8("ConfigDialog"));
        ConfigDialog->resize(272, 231);
        verticalLayout = new QVBoxLayout(ConfigDialog);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        moduleList = new UKUi::PageSelectWidget(ConfigDialog);
        moduleList->setObjectName(QString::fromUtf8("moduleList"));
        moduleList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        moduleList->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);

        horizontalLayout->addWidget(moduleList);

        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        stackedWidget = new QStackedWidget(ConfigDialog);
        stackedWidget->setObjectName(QString::fromUtf8("stackedWidget"));

        verticalLayout_3->addWidget(stackedWidget);


        horizontalLayout->addLayout(verticalLayout_3);


        verticalLayout->addLayout(horizontalLayout);

        buttons = new QDialogButtonBox(ConfigDialog);
        buttons->setObjectName(QString::fromUtf8("buttons"));
        buttons->setOrientation(Qt::Horizontal);
        buttons->setStandardButtons(QDialogButtonBox::Close|QDialogButtonBox::Reset);

        verticalLayout->addWidget(buttons);

        QWidget::setTabOrder(moduleList, buttons);

        retranslateUi(ConfigDialog);
        QObject::connect(moduleList, SIGNAL(currentRowChanged(int)), stackedWidget, SLOT(setCurrentIndex(int)));

        stackedWidget->setCurrentIndex(-1);


        QMetaObject::connectSlotsByName(ConfigDialog);
    } // setupUi

    void retranslateUi(QWidget *ConfigDialog)
    {
        ConfigDialog->setWindowTitle(QApplication::translate("ConfigDialog", "Dialog", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ConfigDialog: public Ui_ConfigDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_UKUICONFIGDIALOG_H
