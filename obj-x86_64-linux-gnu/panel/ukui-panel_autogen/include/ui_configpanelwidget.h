/********************************************************************************
** Form generated from reading UI file 'configpanelwidget.ui'
**
** Created by: Qt User Interface Compiler version 5.12.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONFIGPANELWIDGET_H
#define UI_CONFIGPANELWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ConfigPanelWidget
{
public:
    QVBoxLayout *verticalLayout;
    QGroupBox *groupBox_size;
    QHBoxLayout *horizontalLayout_2;
    QWidget *widget_8;
    QGridLayout *gridLayout_4;
    QSpinBox *spinBox_length;
    QLabel *label;
    QLabel *label_length;
    QComboBox *comboBox_lenghtType;
    QSpinBox *spinBox_panelSize;
    QSpacerItem *horizontalSpacer;
    QWidget *widget_9;
    QGridLayout *gridLayout;
    QSpinBox *spinBox_iconSize;
    QLabel *label_iconSize;
    QLabel *label_lineCount;
    QSpinBox *spinBox_lineCount;
    QGroupBox *groupBox;
    QFormLayout *formLayout;
    QLabel *label_alignment;
    QComboBox *comboBox_alignment;
    QLabel *label_position;
    QComboBox *comboBox_position;
    QGroupBox *checkBox_hidable;
    QGridLayout *gridLayout_2;
    QLabel *label_animation;
    QSpacerItem *horizontalSpacer_3;
    QSpinBox *spinBox_animation;
    QLabel *label_delay;
    QSpinBox *spinBox_delay;
    QCheckBox *checkBox_visibleMargin;
    QCheckBox *checkBox_reserveSpace;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout_3;
    QWidget *widget_6;
    QHBoxLayout *horizontalLayout_6;
    QCheckBox *checkBox_customFontColor;
    QPushButton *pushButton_customFontColor;
    QSpacerItem *horizontalSpacer_2;
    QCheckBox *checkBox_customBgColor;
    QPushButton *pushButton_customBgColor;
    QWidget *widget_3;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_2;
    QSlider *slider_opacity;
    QLabel *compositingL;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QCheckBox *checkBox_customBgImage;
    QWidget *widget_4;
    QHBoxLayout *horizontalLayout_4;
    QLineEdit *lineEdit_customBgImage;
    QPushButton *pushButton_customBgImage;
    QGroupBox *groupBox_icon;
    QFormLayout *formLayout_2;
    QLabel *label_3;
    QComboBox *comboBox_icon;

    void setupUi(QWidget *ConfigPanelWidget)
    {
        if (ConfigPanelWidget->objectName().isEmpty())
            ConfigPanelWidget->setObjectName(QString::fromUtf8("ConfigPanelWidget"));
        ConfigPanelWidget->resize(382, 517);
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(ConfigPanelWidget->sizePolicy().hasHeightForWidth());
        ConfigPanelWidget->setSizePolicy(sizePolicy);
        verticalLayout = new QVBoxLayout(ConfigPanelWidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        groupBox_size = new QGroupBox(ConfigPanelWidget);
        groupBox_size->setObjectName(QString::fromUtf8("groupBox_size"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Minimum);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(groupBox_size->sizePolicy().hasHeightForWidth());
        groupBox_size->setSizePolicy(sizePolicy1);
        groupBox_size->setCheckable(false);
        horizontalLayout_2 = new QHBoxLayout(groupBox_size);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        widget_8 = new QWidget(groupBox_size);
        widget_8->setObjectName(QString::fromUtf8("widget_8"));
        gridLayout_4 = new QGridLayout(widget_8);
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        gridLayout_4->setContentsMargins(0, 0, 0, 0);
        spinBox_length = new QSpinBox(widget_8);
        spinBox_length->setObjectName(QString::fromUtf8("spinBox_length"));
        spinBox_length->setMinimum(1);
        spinBox_length->setMaximum(100);

        gridLayout_4->addWidget(spinBox_length, 1, 1, 1, 1);

        label = new QLabel(widget_8);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout_4->addWidget(label, 0, 0, 1, 1);

        label_length = new QLabel(widget_8);
        label_length->setObjectName(QString::fromUtf8("label_length"));

        gridLayout_4->addWidget(label_length, 1, 0, 1, 1);

        comboBox_lenghtType = new QComboBox(widget_8);
        comboBox_lenghtType->addItem(QString());
        comboBox_lenghtType->addItem(QString());
        comboBox_lenghtType->setObjectName(QString::fromUtf8("comboBox_lenghtType"));

        gridLayout_4->addWidget(comboBox_lenghtType, 1, 2, 1, 1);

        spinBox_panelSize = new QSpinBox(widget_8);
        spinBox_panelSize->setObjectName(QString::fromUtf8("spinBox_panelSize"));
        spinBox_panelSize->setValue(24);

        gridLayout_4->addWidget(spinBox_panelSize, 0, 1, 1, 2);


        horizontalLayout_2->addWidget(widget_8);

        horizontalSpacer = new QSpacerItem(5, 20, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        widget_9 = new QWidget(groupBox_size);
        widget_9->setObjectName(QString::fromUtf8("widget_9"));
        gridLayout = new QGridLayout(widget_9);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        spinBox_iconSize = new QSpinBox(widget_9);
        spinBox_iconSize->setObjectName(QString::fromUtf8("spinBox_iconSize"));
        spinBox_iconSize->setMinimum(10);
        spinBox_iconSize->setMaximum(128);

        gridLayout->addWidget(spinBox_iconSize, 0, 1, 1, 1);

        label_iconSize = new QLabel(widget_9);
        label_iconSize->setObjectName(QString::fromUtf8("label_iconSize"));

        gridLayout->addWidget(label_iconSize, 0, 0, 1, 1);

        label_lineCount = new QLabel(widget_9);
        label_lineCount->setObjectName(QString::fromUtf8("label_lineCount"));

        gridLayout->addWidget(label_lineCount, 1, 0, 1, 1);

        spinBox_lineCount = new QSpinBox(widget_9);
        spinBox_lineCount->setObjectName(QString::fromUtf8("spinBox_lineCount"));
        spinBox_lineCount->setMinimum(1);
        spinBox_lineCount->setMaximum(20);

        gridLayout->addWidget(spinBox_lineCount, 1, 1, 1, 1);


        horizontalLayout_2->addWidget(widget_9);


        verticalLayout->addWidget(groupBox_size);

        groupBox = new QGroupBox(ConfigPanelWidget);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        sizePolicy1.setHeightForWidth(groupBox->sizePolicy().hasHeightForWidth());
        groupBox->setSizePolicy(sizePolicy1);
        formLayout = new QFormLayout(groupBox);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        label_alignment = new QLabel(groupBox);
        label_alignment->setObjectName(QString::fromUtf8("label_alignment"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label_alignment);

        comboBox_alignment = new QComboBox(groupBox);
        comboBox_alignment->addItem(QString());
        comboBox_alignment->addItem(QString());
        comboBox_alignment->addItem(QString());
        comboBox_alignment->setObjectName(QString::fromUtf8("comboBox_alignment"));

        formLayout->setWidget(0, QFormLayout::FieldRole, comboBox_alignment);

        label_position = new QLabel(groupBox);
        label_position->setObjectName(QString::fromUtf8("label_position"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label_position);

        comboBox_position = new QComboBox(groupBox);
        comboBox_position->setObjectName(QString::fromUtf8("comboBox_position"));

        formLayout->setWidget(1, QFormLayout::FieldRole, comboBox_position);

        checkBox_hidable = new QGroupBox(groupBox);
        checkBox_hidable->setObjectName(QString::fromUtf8("checkBox_hidable"));
        checkBox_hidable->setEnabled(true);
        checkBox_hidable->setCheckable(true);
        checkBox_hidable->setChecked(false);
        gridLayout_2 = new QGridLayout(checkBox_hidable);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        label_animation = new QLabel(checkBox_hidable);
        label_animation->setObjectName(QString::fromUtf8("label_animation"));

        gridLayout_2->addWidget(label_animation, 0, 0, 1, 1);

        horizontalSpacer_3 = new QSpacerItem(102, 5, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_2->addItem(horizontalSpacer_3, 0, 1, 1, 1);

        spinBox_animation = new QSpinBox(checkBox_hidable);
        spinBox_animation->setObjectName(QString::fromUtf8("spinBox_animation"));
        spinBox_animation->setMaximum(500);
        spinBox_animation->setSingleStep(50);

        gridLayout_2->addWidget(spinBox_animation, 0, 2, 1, 1);

        label_delay = new QLabel(checkBox_hidable);
        label_delay->setObjectName(QString::fromUtf8("label_delay"));

        gridLayout_2->addWidget(label_delay, 1, 0, 1, 1);

        spinBox_delay = new QSpinBox(checkBox_hidable);
        spinBox_delay->setObjectName(QString::fromUtf8("spinBox_delay"));
        spinBox_delay->setMaximum(2000);
        spinBox_delay->setSingleStep(50);

        gridLayout_2->addWidget(spinBox_delay, 1, 2, 1, 1);

        checkBox_visibleMargin = new QCheckBox(checkBox_hidable);
        checkBox_visibleMargin->setObjectName(QString::fromUtf8("checkBox_visibleMargin"));

        gridLayout_2->addWidget(checkBox_visibleMargin, 2, 0, 1, 3);


        formLayout->setWidget(2, QFormLayout::SpanningRole, checkBox_hidable);

        checkBox_reserveSpace = new QCheckBox(groupBox);
        checkBox_reserveSpace->setObjectName(QString::fromUtf8("checkBox_reserveSpace"));

        formLayout->setWidget(3, QFormLayout::SpanningRole, checkBox_reserveSpace);


        verticalLayout->addWidget(groupBox);

        groupBox_2 = new QGroupBox(ConfigPanelWidget);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        sizePolicy1.setHeightForWidth(groupBox_2->sizePolicy().hasHeightForWidth());
        groupBox_2->setSizePolicy(sizePolicy1);
        gridLayout_3 = new QGridLayout(groupBox_2);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        widget_6 = new QWidget(groupBox_2);
        widget_6->setObjectName(QString::fromUtf8("widget_6"));
        horizontalLayout_6 = new QHBoxLayout(widget_6);
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        horizontalLayout_6->setContentsMargins(0, 0, 0, 0);
        checkBox_customFontColor = new QCheckBox(widget_6);
        checkBox_customFontColor->setObjectName(QString::fromUtf8("checkBox_customFontColor"));

        horizontalLayout_6->addWidget(checkBox_customFontColor);

        pushButton_customFontColor = new QPushButton(widget_6);
        pushButton_customFontColor->setObjectName(QString::fromUtf8("pushButton_customFontColor"));
        pushButton_customFontColor->setEnabled(false);
        pushButton_customFontColor->setStyleSheet(QString::fromUtf8(""));
        QIcon icon;
        QString iconThemeName = QString::fromUtf8("color-picker");
        if (QIcon::hasThemeIcon(iconThemeName)) {
            icon = QIcon::fromTheme(iconThemeName);
        } else {
            icon.addFile(QString::fromUtf8("../../../../../.designer/backup"), QSize(), QIcon::Normal, QIcon::Off);
        }
        pushButton_customFontColor->setIcon(icon);

        horizontalLayout_6->addWidget(pushButton_customFontColor);

        horizontalSpacer_2 = new QSpacerItem(5, 20, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);

        horizontalLayout_6->addItem(horizontalSpacer_2);

        checkBox_customBgColor = new QCheckBox(widget_6);
        checkBox_customBgColor->setObjectName(QString::fromUtf8("checkBox_customBgColor"));

        horizontalLayout_6->addWidget(checkBox_customBgColor);

        pushButton_customBgColor = new QPushButton(widget_6);
        pushButton_customBgColor->setObjectName(QString::fromUtf8("pushButton_customBgColor"));
        pushButton_customBgColor->setEnabled(false);
        pushButton_customBgColor->setStyleSheet(QString::fromUtf8(""));
        pushButton_customBgColor->setIcon(icon);

        horizontalLayout_6->addWidget(pushButton_customBgColor);


        gridLayout_3->addWidget(widget_6, 0, 0, 1, 5);

        widget_3 = new QWidget(groupBox_2);
        widget_3->setObjectName(QString::fromUtf8("widget_3"));
        horizontalLayout_3 = new QHBoxLayout(widget_3);
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(6, -1, -1, -1);
        label_2 = new QLabel(widget_3);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setEnabled(false);

        horizontalLayout_3->addWidget(label_2);

        slider_opacity = new QSlider(widget_3);
        slider_opacity->setObjectName(QString::fromUtf8("slider_opacity"));
        slider_opacity->setEnabled(false);
        slider_opacity->setMaximum(100);
        slider_opacity->setPageStep(5);
        slider_opacity->setValue(100);
        slider_opacity->setOrientation(Qt::Horizontal);

        horizontalLayout_3->addWidget(slider_opacity);


        gridLayout_3->addWidget(widget_3, 2, 0, 1, 5);

        compositingL = new QLabel(groupBox_2);
        compositingL->setObjectName(QString::fromUtf8("compositingL"));
        compositingL->setEnabled(false);
        compositingL->setAlignment(Qt::AlignCenter);

        gridLayout_3->addWidget(compositingL, 3, 0, 1, 5);

        widget = new QWidget(groupBox_2);
        widget->setObjectName(QString::fromUtf8("widget"));
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);

        gridLayout_3->addWidget(widget, 6, 0, 1, 2);

        checkBox_customBgImage = new QCheckBox(groupBox_2);
        checkBox_customBgImage->setObjectName(QString::fromUtf8("checkBox_customBgImage"));

        gridLayout_3->addWidget(checkBox_customBgImage, 5, 0, 1, 1);

        widget_4 = new QWidget(groupBox_2);
        widget_4->setObjectName(QString::fromUtf8("widget_4"));
        horizontalLayout_4 = new QHBoxLayout(widget_4);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        horizontalLayout_4->setContentsMargins(0, 0, 0, 0);
        lineEdit_customBgImage = new QLineEdit(widget_4);
        lineEdit_customBgImage->setObjectName(QString::fromUtf8("lineEdit_customBgImage"));
        lineEdit_customBgImage->setEnabled(false);

        horizontalLayout_4->addWidget(lineEdit_customBgImage);

        pushButton_customBgImage = new QPushButton(widget_4);
        pushButton_customBgImage->setObjectName(QString::fromUtf8("pushButton_customBgImage"));
        pushButton_customBgImage->setEnabled(false);
        pushButton_customBgImage->setStyleSheet(QString::fromUtf8(""));
        QIcon icon1;
        iconThemeName = QString::fromUtf8("insert-image");
        if (QIcon::hasThemeIcon(iconThemeName)) {
            icon1 = QIcon::fromTheme(iconThemeName);
        } else {
            icon1.addFile(QString::fromUtf8("../../../../../.designer/backup"), QSize(), QIcon::Normal, QIcon::Off);
        }
        pushButton_customBgImage->setIcon(icon1);

        horizontalLayout_4->addWidget(pushButton_customBgImage);


        gridLayout_3->addWidget(widget_4, 5, 1, 1, 4);


        verticalLayout->addWidget(groupBox_2);

        groupBox_icon = new QGroupBox(ConfigPanelWidget);
        groupBox_icon->setObjectName(QString::fromUtf8("groupBox_icon"));
        groupBox_icon->setCheckable(true);
        groupBox_icon->setChecked(false);
        formLayout_2 = new QFormLayout(groupBox_icon);
        formLayout_2->setObjectName(QString::fromUtf8("formLayout_2"));
        formLayout_2->setFormAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        label_3 = new QLabel(groupBox_icon);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        formLayout_2->setWidget(0, QFormLayout::LabelRole, label_3);

        comboBox_icon = new QComboBox(groupBox_icon);
        comboBox_icon->setObjectName(QString::fromUtf8("comboBox_icon"));

        formLayout_2->setWidget(0, QFormLayout::FieldRole, comboBox_icon);


        verticalLayout->addWidget(groupBox_icon);


        retranslateUi(ConfigPanelWidget);
        QObject::connect(checkBox_customBgColor, SIGNAL(toggled(bool)), pushButton_customBgColor, SLOT(setEnabled(bool)));
        QObject::connect(checkBox_customBgImage, SIGNAL(toggled(bool)), lineEdit_customBgImage, SLOT(setEnabled(bool)));
        QObject::connect(checkBox_customBgImage, SIGNAL(toggled(bool)), pushButton_customBgImage, SLOT(setEnabled(bool)));
        QObject::connect(checkBox_customFontColor, SIGNAL(toggled(bool)), pushButton_customFontColor, SLOT(setEnabled(bool)));
        QObject::connect(checkBox_customBgColor, SIGNAL(toggled(bool)), slider_opacity, SLOT(setEnabled(bool)));
        QObject::connect(checkBox_customBgColor, SIGNAL(toggled(bool)), label_2, SLOT(setEnabled(bool)));
        QObject::connect(checkBox_customBgColor, SIGNAL(toggled(bool)), compositingL, SLOT(setEnabled(bool)));

        comboBox_alignment->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(ConfigPanelWidget);
    } // setupUi

    void retranslateUi(QWidget *ConfigPanelWidget)
    {
        ConfigPanelWidget->setWindowTitle(QApplication::translate("ConfigPanelWidget", "Configure panel", nullptr));
        groupBox_size->setTitle(QApplication::translate("ConfigPanelWidget", "Size", nullptr));
#ifndef QT_NO_TOOLTIP
        spinBox_length->setToolTip(QApplication::translate("ConfigPanelWidget", "<p>Negative pixel value sets the panel length to that many pixels less than available screen space.</p><p/><p><i>E.g. \"Length\" set to -100px, screen size is 1000px, then real panel length will be 900 px.</i></p>", nullptr));
#endif // QT_NO_TOOLTIP
        label->setText(QApplication::translate("ConfigPanelWidget", "Size:", nullptr));
        label_length->setText(QApplication::translate("ConfigPanelWidget", "Length:", nullptr));
        comboBox_lenghtType->setItemText(0, QApplication::translate("ConfigPanelWidget", "%", nullptr));
        comboBox_lenghtType->setItemText(1, QApplication::translate("ConfigPanelWidget", "px", nullptr));

        spinBox_panelSize->setSuffix(QApplication::translate("ConfigPanelWidget", " px", nullptr));
        spinBox_iconSize->setSuffix(QApplication::translate("ConfigPanelWidget", " px", nullptr));
        label_iconSize->setText(QApplication::translate("ConfigPanelWidget", "Icon size:", nullptr));
        label_lineCount->setText(QApplication::translate("ConfigPanelWidget", "Rows:", nullptr));
        groupBox->setTitle(QApplication::translate("ConfigPanelWidget", "Alignment && position", nullptr));
        label_alignment->setText(QApplication::translate("ConfigPanelWidget", "Alignment:", nullptr));
        comboBox_alignment->setItemText(0, QApplication::translate("ConfigPanelWidget", "Left", nullptr));
        comboBox_alignment->setItemText(1, QApplication::translate("ConfigPanelWidget", "Center", nullptr));
        comboBox_alignment->setItemText(2, QApplication::translate("ConfigPanelWidget", "Right", nullptr));

        label_position->setText(QApplication::translate("ConfigPanelWidget", "Position:", nullptr));
        checkBox_hidable->setTitle(QApplication::translate("ConfigPanelWidget", "A&uto-hide", nullptr));
#ifndef QT_NO_TOOLTIP
        label_animation->setToolTip(QApplication::translate("ConfigPanelWidget", "Zero means no animation", nullptr));
#endif // QT_NO_TOOLTIP
        label_animation->setText(QApplication::translate("ConfigPanelWidget", "Animation duration:", nullptr));
#ifndef QT_NO_TOOLTIP
        spinBox_animation->setToolTip(QApplication::translate("ConfigPanelWidget", "Zero means no animation", nullptr));
#endif // QT_NO_TOOLTIP
        spinBox_animation->setSuffix(QApplication::translate("ConfigPanelWidget", " ms", nullptr));
#ifndef QT_NO_TOOLTIP
        label_delay->setToolTip(QApplication::translate("ConfigPanelWidget", "Zero means no delay", nullptr));
#endif // QT_NO_TOOLTIP
        label_delay->setText(QApplication::translate("ConfigPanelWidget", "Show with delay:", nullptr));
#ifndef QT_NO_TOOLTIP
        spinBox_delay->setToolTip(QApplication::translate("ConfigPanelWidget", "Zero means no delay", nullptr));
#endif // QT_NO_TOOLTIP
        spinBox_delay->setSuffix(QApplication::translate("ConfigPanelWidget", " ms", nullptr));
        checkBox_visibleMargin->setText(QApplication::translate("ConfigPanelWidget", "Visible thin margin for hidden panel", nullptr));
#ifndef QT_NO_TOOLTIP
        checkBox_reserveSpace->setToolTip(QApplication::translate("ConfigPanelWidget", "Don't allow maximized windows go under the panel window", nullptr));
#endif // QT_NO_TOOLTIP
        checkBox_reserveSpace->setText(QApplication::translate("ConfigPanelWidget", "Reserve space on display", nullptr));
        groupBox_2->setTitle(QApplication::translate("ConfigPanelWidget", "Custom styling", nullptr));
        checkBox_customFontColor->setText(QApplication::translate("ConfigPanelWidget", "Font color:", nullptr));
        pushButton_customFontColor->setText(QString());
        checkBox_customBgColor->setText(QApplication::translate("ConfigPanelWidget", "Background color:", nullptr));
        pushButton_customBgColor->setText(QString());
        label_2->setText(QApplication::translate("ConfigPanelWidget", "Background opacity:", nullptr));
        compositingL->setText(QApplication::translate("ConfigPanelWidget", "<small>Compositing is required for panel transparency.</small>", nullptr));
        checkBox_customBgImage->setText(QApplication::translate("ConfigPanelWidget", "Background image:", nullptr));
        pushButton_customBgImage->setText(QString());
#ifndef QT_NO_TOOLTIP
        groupBox_icon->setToolTip(QApplication::translate("ConfigPanelWidget", "A partial workaround for widget styles that\n"
"cannot give a separate theme to the panel.\n"
"\n"
"You might also want to disable:\n"
"\n"
"UKUi Appearance Configuration \342\206\222\n"
"Icons Theme \342\206\222\n"
"Colorize icons based on widget style (palette)", nullptr));
#endif // QT_NO_TOOLTIP
        groupBox_icon->setTitle(QApplication::translate("ConfigPanelWidget", "Override icon &theme", nullptr));
        label_3->setText(QApplication::translate("ConfigPanelWidget", "Icon theme for panels:", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ConfigPanelWidget: public Ui_ConfigPanelWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONFIGPANELWIDGET_H
