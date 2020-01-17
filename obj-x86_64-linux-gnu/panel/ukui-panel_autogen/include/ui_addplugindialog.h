/********************************************************************************
** Form generated from reading UI file 'addplugindialog.ui'
**
** Created by: Qt User Interface Compiler version 5.12.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ADDPLUGINDIALOG_H
#define UI_ADDPLUGINDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_AddPluginDialog
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *searchLabel;
    QLineEdit *searchEdit;
    QListWidget *pluginList;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer;
    QPushButton *addButton;
    QPushButton *closeButton;

    void setupUi(QDialog *AddPluginDialog)
    {
        if (AddPluginDialog->objectName().isEmpty())
            AddPluginDialog->setObjectName(QString::fromUtf8("AddPluginDialog"));
        AddPluginDialog->resize(400, 359);
        verticalLayout = new QVBoxLayout(AddPluginDialog);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        searchLabel = new QLabel(AddPluginDialog);
        searchLabel->setObjectName(QString::fromUtf8("searchLabel"));

        horizontalLayout->addWidget(searchLabel);

        searchEdit = new QLineEdit(AddPluginDialog);
        searchEdit->setObjectName(QString::fromUtf8("searchEdit"));

        horizontalLayout->addWidget(searchEdit);


        verticalLayout->addLayout(horizontalLayout);

        pluginList = new QListWidget(AddPluginDialog);
        pluginList->setObjectName(QString::fromUtf8("pluginList"));
        pluginList->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
        pluginList->setProperty("showDropIndicator", QVariant(true));
        pluginList->setAlternatingRowColors(true);
        pluginList->setSelectionMode(QAbstractItemView::SingleSelection);
        pluginList->setSelectionBehavior(QAbstractItemView::SelectRows);
        pluginList->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
        pluginList->setMovement(QListView::Static);
        pluginList->setFlow(QListView::TopToBottom);
        pluginList->setResizeMode(QListView::Adjust);
        pluginList->setSpacing(0);
        pluginList->setModelColumn(0);
        pluginList->setUniformItemSizes(false);
        pluginList->setSelectionRectVisible(true);
        pluginList->setSortingEnabled(false);

        verticalLayout->addWidget(pluginList);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        addButton = new QPushButton(AddPluginDialog);
        addButton->setObjectName(QString::fromUtf8("addButton"));

        horizontalLayout_2->addWidget(addButton);

        closeButton = new QPushButton(AddPluginDialog);
        closeButton->setObjectName(QString::fromUtf8("closeButton"));
        closeButton->setAutoDefault(false);

        horizontalLayout_2->addWidget(closeButton);


        verticalLayout->addLayout(horizontalLayout_2);

        QWidget::setTabOrder(pluginList, addButton);
        QWidget::setTabOrder(addButton, closeButton);
        QWidget::setTabOrder(closeButton, searchEdit);

        retranslateUi(AddPluginDialog);
        QObject::connect(closeButton, SIGNAL(clicked()), AddPluginDialog, SLOT(close()));

        pluginList->setCurrentRow(-1);
        closeButton->setDefault(true);


        QMetaObject::connectSlotsByName(AddPluginDialog);
    } // setupUi

    void retranslateUi(QDialog *AddPluginDialog)
    {
        AddPluginDialog->setWindowTitle(QApplication::translate("AddPluginDialog", "Add Plugins", nullptr));
        searchLabel->setText(QApplication::translate("AddPluginDialog", "Search:", nullptr));
        addButton->setText(QApplication::translate("AddPluginDialog", "Add Widget", nullptr));
        closeButton->setText(QApplication::translate("AddPluginDialog", "Close", nullptr));
    } // retranslateUi

};

namespace Ui {
    class AddPluginDialog: public Ui_AddPluginDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ADDPLUGINDIALOG_H
