/********************************************************************************
** Form generated from reading UI file 'frmlunarcalendarwidget.ui'
**
** Created by: Qt User Interface Compiler version 5.12.8
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FRMLUNARCALENDARWIDGET_H
#define UI_FRMLUNARCALENDARWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "lunarcalendarwidget.h"

QT_BEGIN_NAMESPACE

class Ui_frmLunarCalendarWidget
{
public:
    QVBoxLayout *verticalLayout;
    LunarCalendarWidget *lunarCalendarWidget;
    QWidget *widgetBottom;
    QHBoxLayout *horizontalLayout;
 //   QLabel *labCalendarStyle;
 //   QComboBox *cboxCalendarStyle;
  //  QLabel *labSelectType;
 //   QComboBox *cboxSelectType;
 //   QLabel *labWeekNameFormat;
 //   QComboBox *cboxWeekNameFormat;
  //  QCheckBox *ckShowLunar;
    QSpacerItem *horizontalSpacer;

    void setupUi(QWidget *frmLunarCalendarWidget)
    {
        if (frmLunarCalendarWidget->objectName().isEmpty())
            frmLunarCalendarWidget->setObjectName(QString::fromUtf8("frmLunarCalendarWidget"));
        frmLunarCalendarWidget->resize(600, 500);
        verticalLayout = new QVBoxLayout(frmLunarCalendarWidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        lunarCalendarWidget = new LunarCalendarWidget(frmLunarCalendarWidget);
        lunarCalendarWidget->setObjectName(QString::fromUtf8("lunarCalendarWidget"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(lunarCalendarWidget->sizePolicy().hasHeightForWidth());
        lunarCalendarWidget->setSizePolicy(sizePolicy);

        verticalLayout->addWidget(lunarCalendarWidget);

        widgetBottom = new QWidget(frmLunarCalendarWidget);
        widgetBottom->setObjectName(QString::fromUtf8("widgetBottom"));
        horizontalLayout = new QHBoxLayout(widgetBottom);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
       // labCalendarStyle = new QLabel(widgetBottom);
       // labCalendarStyle->setObjectName(QString::fromUtf8("labCalendarStyle"));

       // horizontalLayout->addWidget(labCalendarStyle);

      //  cboxCalendarStyle = new QComboBox(widgetBottom);
     //   cboxCalendarStyle->addItem(QString());
       // cboxCalendarStyle->setObjectName(QString::fromUtf8("cboxCalendarStyle"));
      //  cboxCalendarStyle->setMinimumSize(QSize(90, 0));

     //   horizontalLayout->addWidget(cboxCalendarStyle);

//        labSelectType = new QLabel(widgetBottom);
//        labSelectType->setObjectName(QString::fromUtf8("labSelectType"));

//        horizontalLayout->addWidget(labSelectType);

//        cboxSelectType = new QComboBox(widgetBottom);
//        cboxSelectType->addItem(QString());
//        cboxSelectType->addItem(QString());
//        cboxSelectType->addItem(QString());
//        cboxSelectType->addItem(QString());
//        cboxSelectType->setObjectName(QString::fromUtf8("cboxSelectType"));
//        cboxSelectType->setMinimumSize(QSize(90, 0));

//        horizontalLayout->addWidget(cboxSelectType);

//        labWeekNameFormat = new QLabel(widgetBottom);
//        labWeekNameFormat->setObjectName(QString::fromUtf8("labWeekNameFormat"));

//        horizontalLayout->addWidget(labWeekNameFormat);

//        cboxWeekNameFormat = new QComboBox(widgetBottom);
//        cboxWeekNameFormat->addItem(QString());
//        cboxWeekNameFormat->addItem(QString());
//        cboxWeekNameFormat->addItem(QString());
//        cboxWeekNameFormat->addItem(QString());
//        cboxWeekNameFormat->setObjectName(QString::fromUtf8("cboxWeekNameFormat"));
//        cboxWeekNameFormat->setMinimumSize(QSize(90, 0));

//        horizontalLayout->addWidget(cboxWeekNameFormat);

//        ckShowLunar = new QCheckBox(widgetBottom);
//        ckShowLunar->setObjectName(QString::fromUtf8("ckShowLunar"));
//        ckShowLunar->setChecked(true);

//        horizontalLayout->addWidget(ckShowLunar);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        verticalLayout->addWidget(widgetBottom);


        retranslateUi(frmLunarCalendarWidget);

        QMetaObject::connectSlotsByName(frmLunarCalendarWidget);
    } // setupUi

    void retranslateUi(QWidget *frmLunarCalendarWidget)
    {
        frmLunarCalendarWidget->setWindowTitle(QApplication::translate("frmLunarCalendarWidget", "Form", nullptr));
      //  labCalendarStyle->setText(QApplication::translate("frmLunarCalendarWidget", "\346\225\264\344\275\223\346\240\267\345\274\217", nullptr));
      //  cboxCalendarStyle->setItemText(0, QApplication::translate("frmLunarCalendarWidget", "\347\272\242\350\211\262\351\243\216\346\240\274", nullptr));

     //   labSelectType->setText(QApplication::translate("frmLunarCalendarWidget", "\351\200\211\344\270\255\346\240\267\345\274\217", nullptr));
//        cboxSelectType->setItemText(0, QApplication::translate("frmLunarCalendarWidget", "\347\237\251\345\275\242\350\203\214\346\231\257", nullptr));
//        cboxSelectType->setItemText(1, QApplication::translate("frmLunarCalendarWidget", "\345\234\206\345\275\242\350\203\214\346\231\257", nullptr));
//        cboxSelectType->setItemText(2, QApplication::translate("frmLunarCalendarWidget", "\350\247\222\346\240\207\350\203\214\346\231\257", nullptr));
//        cboxSelectType->setItemText(3, QApplication::translate("frmLunarCalendarWidget", "\345\233\276\347\211\207\350\203\214\346\231\257", nullptr));

       // labWeekNameFormat->setText(QApplication::translate("frmLunarCalendarWidget", "\346\230\237\346\234\237\346\240\274\345\274\217", nullptr));
//        cboxWeekNameFormat->setItemText(0, QApplication::translate("frmLunarCalendarWidget", "\347\237\255\345\220\215\347\247\260", nullptr));
//        cboxWeekNameFormat->setItemText(1, QApplication::translate("frmLunarCalendarWidget", "\346\231\256\351\200\232\345\220\215\347\247\260", nullptr));
//        cboxWeekNameFormat->setItemText(2, QApplication::translate("frmLunarCalendarWidget", "\351\225\277\345\220\215\347\247\260", nullptr));
//        cboxWeekNameFormat->setItemText(3, QApplication::translate("frmLunarCalendarWidget", "\350\213\261\346\226\207\345\220\215\347\247\260", nullptr));

      //  ckShowLunar->setText(QApplication::translate("frmLunarCalendarWidget", "\346\230\276\347\244\272\345\206\234\345\216\206", nullptr));
    } // retranslateUi

};

namespace Ui {
    class frmLunarCalendarWidget: public Ui_frmLunarCalendarWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FRMLUNARCALENDARWIDGET_H
