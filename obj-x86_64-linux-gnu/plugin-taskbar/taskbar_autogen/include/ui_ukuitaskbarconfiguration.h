/********************************************************************************
** Form generated from reading UI file 'ukuitaskbarconfiguration.ui'
**
** Created by: Qt User Interface Compiler version 5.12.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_UKUITASKBARCONFIGURATION_H
#define UI_UKUITASKBARCONFIGURATION_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListView>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_UKUITaskbarConfiguration
{
public:
    QVBoxLayout *verticalLayout;
    QGroupBox *WLContentGB;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout;
    QCheckBox *limitByDesktopCB;
    QComboBox *showDesktopNumCB;
    QCheckBox *limitByScreenCB;
    QCheckBox *limitByMinimizedCB;
    QCheckBox *raiseOnCurrentDesktopCB;
    QCheckBox *middleClickCB;
    QCheckBox *cycleOnWheelScroll;
    QGroupBox *groupingGB;
    QVBoxLayout *verticalLayout_3;
    QCheckBox *showGroupOnHoverCB;
    QGroupBox *WLApperanceGB;
    QFormLayout *formLayout;
    QLabel *buttonStyleL;
    QComboBox *buttonStyleCB;
    QLabel *buttonWidthL;
    QSpinBox *buttonWidthSB;
    QLabel *buttonHeightL;
    QSpinBox *buttonHeightSB;
    QCheckBox *autoRotateCB;
    QCheckBox *iconByClassCB;
    QListView *listView;
    QSpacerItem *verticalSpacer;
    QDialogButtonBox *buttons;

    void setupUi(QDialog *UKUITaskbarConfiguration)
    {
        if (UKUITaskbarConfiguration->objectName().isEmpty())
            UKUITaskbarConfiguration->setObjectName(QString::fromUtf8("UKUITaskbarConfiguration"));
        UKUITaskbarConfiguration->resize(401, 512);
        verticalLayout = new QVBoxLayout(UKUITaskbarConfiguration);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        WLContentGB = new QGroupBox(UKUITaskbarConfiguration);
        WLContentGB->setObjectName(QString::fromUtf8("WLContentGB"));
        verticalLayout_2 = new QVBoxLayout(WLContentGB);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        limitByDesktopCB = new QCheckBox(WLContentGB);
        limitByDesktopCB->setObjectName(QString::fromUtf8("limitByDesktopCB"));

        horizontalLayout->addWidget(limitByDesktopCB);

        showDesktopNumCB = new QComboBox(WLContentGB);
        showDesktopNumCB->setObjectName(QString::fromUtf8("showDesktopNumCB"));

        horizontalLayout->addWidget(showDesktopNumCB);


        verticalLayout_2->addLayout(horizontalLayout);

        limitByScreenCB = new QCheckBox(WLContentGB);
        limitByScreenCB->setObjectName(QString::fromUtf8("limitByScreenCB"));

        verticalLayout_2->addWidget(limitByScreenCB);

        limitByMinimizedCB = new QCheckBox(WLContentGB);
        limitByMinimizedCB->setObjectName(QString::fromUtf8("limitByMinimizedCB"));

        verticalLayout_2->addWidget(limitByMinimizedCB);

        raiseOnCurrentDesktopCB = new QCheckBox(WLContentGB);
        raiseOnCurrentDesktopCB->setObjectName(QString::fromUtf8("raiseOnCurrentDesktopCB"));

        verticalLayout_2->addWidget(raiseOnCurrentDesktopCB);

        middleClickCB = new QCheckBox(WLContentGB);
        middleClickCB->setObjectName(QString::fromUtf8("middleClickCB"));

        verticalLayout_2->addWidget(middleClickCB);

        cycleOnWheelScroll = new QCheckBox(WLContentGB);
        cycleOnWheelScroll->setObjectName(QString::fromUtf8("cycleOnWheelScroll"));

        verticalLayout_2->addWidget(cycleOnWheelScroll);


        verticalLayout->addWidget(WLContentGB);

        groupingGB = new QGroupBox(UKUITaskbarConfiguration);
        groupingGB->setObjectName(QString::fromUtf8("groupingGB"));
        groupingGB->setCheckable(true);
        verticalLayout_3 = new QVBoxLayout(groupingGB);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        showGroupOnHoverCB = new QCheckBox(groupingGB);
        showGroupOnHoverCB->setObjectName(QString::fromUtf8("showGroupOnHoverCB"));

        verticalLayout_3->addWidget(showGroupOnHoverCB);


        verticalLayout->addWidget(groupingGB);

        WLApperanceGB = new QGroupBox(UKUITaskbarConfiguration);
        WLApperanceGB->setObjectName(QString::fromUtf8("WLApperanceGB"));
        formLayout = new QFormLayout(WLApperanceGB);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        formLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
        buttonStyleL = new QLabel(WLApperanceGB);
        buttonStyleL->setObjectName(QString::fromUtf8("buttonStyleL"));

        formLayout->setWidget(0, QFormLayout::LabelRole, buttonStyleL);

        buttonStyleCB = new QComboBox(WLApperanceGB);
        buttonStyleCB->setObjectName(QString::fromUtf8("buttonStyleCB"));

        formLayout->setWidget(0, QFormLayout::FieldRole, buttonStyleCB);

        buttonWidthL = new QLabel(WLApperanceGB);
        buttonWidthL->setObjectName(QString::fromUtf8("buttonWidthL"));

        formLayout->setWidget(1, QFormLayout::LabelRole, buttonWidthL);

        buttonWidthSB = new QSpinBox(WLApperanceGB);
        buttonWidthSB->setObjectName(QString::fromUtf8("buttonWidthSB"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(buttonWidthSB->sizePolicy().hasHeightForWidth());
        buttonWidthSB->setSizePolicy(sizePolicy);
        buttonWidthSB->setMinimum(1);
        buttonWidthSB->setMaximum(2147483647);

        formLayout->setWidget(1, QFormLayout::FieldRole, buttonWidthSB);

        buttonHeightL = new QLabel(WLApperanceGB);
        buttonHeightL->setObjectName(QString::fromUtf8("buttonHeightL"));

        formLayout->setWidget(2, QFormLayout::LabelRole, buttonHeightL);

        buttonHeightSB = new QSpinBox(WLApperanceGB);
        buttonHeightSB->setObjectName(QString::fromUtf8("buttonHeightSB"));
        sizePolicy.setHeightForWidth(buttonHeightSB->sizePolicy().hasHeightForWidth());
        buttonHeightSB->setSizePolicy(sizePolicy);
        buttonHeightSB->setMinimum(1);
        buttonHeightSB->setMaximum(500);

        formLayout->setWidget(2, QFormLayout::FieldRole, buttonHeightSB);

        autoRotateCB = new QCheckBox(WLApperanceGB);
        autoRotateCB->setObjectName(QString::fromUtf8("autoRotateCB"));

        formLayout->setWidget(4, QFormLayout::SpanningRole, autoRotateCB);

        iconByClassCB = new QCheckBox(WLApperanceGB);
        iconByClassCB->setObjectName(QString::fromUtf8("iconByClassCB"));

        formLayout->setWidget(5, QFormLayout::SpanningRole, iconByClassCB);

        listView = new QListView(WLApperanceGB);
        listView->setObjectName(QString::fromUtf8("listView"));

        formLayout->setWidget(2, QFormLayout::LabelRole, listView);


        verticalLayout->addWidget(WLApperanceGB);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        buttons = new QDialogButtonBox(UKUITaskbarConfiguration);
        buttons->setObjectName(QString::fromUtf8("buttons"));
        buttons->setOrientation(Qt::Horizontal);
        buttons->setStandardButtons(QDialogButtonBox::Close|QDialogButtonBox::Reset);

        verticalLayout->addWidget(buttons);


        retranslateUi(UKUITaskbarConfiguration);
        QObject::connect(buttons, SIGNAL(accepted()), UKUITaskbarConfiguration, SLOT(accept()));
        QObject::connect(buttons, SIGNAL(rejected()), UKUITaskbarConfiguration, SLOT(reject()));

        QMetaObject::connectSlotsByName(UKUITaskbarConfiguration);
    } // setupUi

    void retranslateUi(QDialog *UKUITaskbarConfiguration)
    {
        UKUITaskbarConfiguration->setWindowTitle(QApplication::translate("UKUITaskbarConfiguration", "Task Manager Settings", nullptr));
        WLContentGB->setTitle(QApplication::translate("UKUITaskbarConfiguration", "General", nullptr));
        limitByDesktopCB->setText(QApplication::translate("UKUITaskbarConfiguration", "Show only windows from desktop", nullptr));
        limitByScreenCB->setText(QApplication::translate("UKUITaskbarConfiguration", "Show only windows from &panel's screen", nullptr));
        limitByMinimizedCB->setText(QApplication::translate("UKUITaskbarConfiguration", "Show only minimized windows", nullptr));
        raiseOnCurrentDesktopCB->setText(QApplication::translate("UKUITaskbarConfiguration", "Raise minimized windows on current desktop", nullptr));
        middleClickCB->setText(QApplication::translate("UKUITaskbarConfiguration", "Close on middle-click", nullptr));
        cycleOnWheelScroll->setText(QApplication::translate("UKUITaskbarConfiguration", "Cycle windows on wheel scrolling", nullptr));
        groupingGB->setTitle(QApplication::translate("UKUITaskbarConfiguration", "Window &grouping", nullptr));
        showGroupOnHoverCB->setText(QApplication::translate("UKUITaskbarConfiguration", "Show popup on mouse hover", nullptr));
        WLApperanceGB->setTitle(QApplication::translate("UKUITaskbarConfiguration", "Appearance", nullptr));
        buttonStyleL->setText(QApplication::translate("UKUITaskbarConfiguration", "Button style", nullptr));
        buttonWidthL->setText(QApplication::translate("UKUITaskbarConfiguration", "Maximum button width", nullptr));
        buttonWidthSB->setSuffix(QApplication::translate("UKUITaskbarConfiguration", " px", nullptr));
        buttonHeightL->setText(QApplication::translate("UKUITaskbarConfiguration", "Maximum button height", nullptr));
        buttonHeightSB->setSuffix(QApplication::translate("UKUITaskbarConfiguration", " px", nullptr));
        autoRotateCB->setText(QApplication::translate("UKUITaskbarConfiguration", "Auto&rotate buttons when the panel is vertical", nullptr));
        iconByClassCB->setText(QApplication::translate("UKUITaskbarConfiguration", "Use icons by WindowClass, if available", nullptr));
    } // retranslateUi

};

namespace Ui {
    class UKUITaskbarConfiguration: public Ui_UKUITaskbarConfiguration {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_UKUITASKBARCONFIGURATION_H
