/********************************************************************************
** Form generated from reading UI file 'spacerconfiguration.ui'
**
** Created by: Qt User Interface Compiler version 5.12.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SPACERCONFIGURATION_H
#define UI_SPACERCONFIGURATION_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpinBox>

QT_BEGIN_NAMESPACE

class Ui_SpacerConfiguration
{
public:
    QGridLayout *gridLayout;
    QLabel *labelSize;
    QSpinBox *sizeSB;
    QLabel *labelType;
    QDialogButtonBox *buttons;
    QRadioButton *sizeFixedRB;
    QComboBox *typeCB;
    QRadioButton *sizeExpandRB;

    void setupUi(QDialog *SpacerConfiguration)
    {
        if (SpacerConfiguration->objectName().isEmpty())
            SpacerConfiguration->setObjectName(QString::fromUtf8("SpacerConfiguration"));
        SpacerConfiguration->resize(289, 135);
        gridLayout = new QGridLayout(SpacerConfiguration);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        labelSize = new QLabel(SpacerConfiguration);
        labelSize->setObjectName(QString::fromUtf8("labelSize"));

        gridLayout->addWidget(labelSize, 0, 0, 1, 1);

        sizeSB = new QSpinBox(SpacerConfiguration);
        sizeSB->setObjectName(QString::fromUtf8("sizeSB"));
        sizeSB->setMinimum(4);
        sizeSB->setMaximum(2048);
        sizeSB->setValue(8);

        gridLayout->addWidget(sizeSB, 0, 2, 1, 1);

        labelType = new QLabel(SpacerConfiguration);
        labelType->setObjectName(QString::fromUtf8("labelType"));

        gridLayout->addWidget(labelType, 2, 0, 1, 1);

        buttons = new QDialogButtonBox(SpacerConfiguration);
        buttons->setObjectName(QString::fromUtf8("buttons"));
        buttons->setOrientation(Qt::Horizontal);
        buttons->setStandardButtons(QDialogButtonBox::Close);

        gridLayout->addWidget(buttons, 3, 0, 1, 3);

        sizeFixedRB = new QRadioButton(SpacerConfiguration);
        sizeFixedRB->setObjectName(QString::fromUtf8("sizeFixedRB"));

        gridLayout->addWidget(sizeFixedRB, 0, 1, 1, 1);

        typeCB = new QComboBox(SpacerConfiguration);
        typeCB->setObjectName(QString::fromUtf8("typeCB"));
        typeCB->setEditable(false);

        gridLayout->addWidget(typeCB, 2, 1, 1, 2);

        sizeExpandRB = new QRadioButton(SpacerConfiguration);
        sizeExpandRB->setObjectName(QString::fromUtf8("sizeExpandRB"));

        gridLayout->addWidget(sizeExpandRB, 1, 1, 1, 2);


        retranslateUi(SpacerConfiguration);
        QObject::connect(buttons, SIGNAL(clicked(QAbstractButton*)), SpacerConfiguration, SLOT(close()));
        QObject::connect(sizeFixedRB, SIGNAL(toggled(bool)), sizeSB, SLOT(setEnabled(bool)));

        QMetaObject::connectSlotsByName(SpacerConfiguration);
    } // setupUi

    void retranslateUi(QDialog *SpacerConfiguration)
    {
        SpacerConfiguration->setWindowTitle(QApplication::translate("SpacerConfiguration", "Spacer Settings", nullptr));
        labelSize->setText(QApplication::translate("SpacerConfiguration", "Space width:", nullptr));
        labelType->setText(QApplication::translate("SpacerConfiguration", "Space type:", nullptr));
        sizeFixedRB->setText(QApplication::translate("SpacerConfiguration", "fixed", nullptr));
        sizeExpandRB->setText(QApplication::translate("SpacerConfiguration", "expandable", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SpacerConfiguration: public Ui_SpacerConfiguration {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SPACERCONFIGURATION_H
