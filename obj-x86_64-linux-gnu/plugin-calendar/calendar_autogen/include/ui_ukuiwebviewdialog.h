/********************************************************************************
** Form generated from reading UI file 'ukuiwebviewdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.12.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_UKUIWEBVIEWDIALOG_H
#define UI_UKUIWEBVIEWDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>

QT_BEGIN_NAMESPACE

class Ui_UkuiWebviewDialog
{
public:

    void setupUi(QDialog *UkuiWebviewDialog)
    {
        if (UkuiWebviewDialog->objectName().isEmpty())
            UkuiWebviewDialog->setObjectName(QString::fromUtf8("UkuiWebviewDialog"));
        UkuiWebviewDialog->resize(400, 300);

        retranslateUi(UkuiWebviewDialog);

        QMetaObject::connectSlotsByName(UkuiWebviewDialog);
    } // setupUi

    void retranslateUi(QDialog *UkuiWebviewDialog)
    {
        UkuiWebviewDialog->setWindowTitle(QApplication::translate("UkuiWebviewDialog", "Dialog", nullptr));
    } // retranslateUi

};

namespace Ui {
    class UkuiWebviewDialog: public Ui_UkuiWebviewDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_UKUIWEBVIEWDIALOG_H
