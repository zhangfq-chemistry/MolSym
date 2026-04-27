/********************************************************************************
** Form generated from reading UI file 'ColorProperty.ui'
**
** Created by: Qt User Interface Compiler version 5.13.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_COLORPROPERTY_H
#define UI_COLORPROPERTY_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>

QT_BEGIN_NAMESPACE

class Ui_ColorProperty
{
public:
    QFormLayout *formLayout_2;
    QGroupBox *groupBox;
    QFormLayout *formLayout;
    QGridLayout *gridLayout;
    QSpinBox *spinBox_bk_green;
    QSpinBox *spinBox_bk_blue;
    QSpinBox *spinBox_D_opacity;
    QSpinBox *spinBox_D_red;
    QSpinBox *spinBox_V_blue;
    QLabel *label;
    QSpinBox *spinBox_V_green;
    QSpinBox *spinBox_H_green;
    QSpinBox *spinBox_V_opacity;
    QSpinBox *spinBox_D_green;
    QSpinBox *spinBox_Cn_blue;
    QLabel *label_2;
    QLabel *label_7;
    QPushButton *pushButton_D;
    QSpinBox *spinBox_bk_red;
    QPushButton *pushButton_Cn;
    QPushButton *pushButton_V;
    QPushButton *pushButton_bk;
    QLabel *label_4;
    QPushButton *pushButton_H;
    QSpinBox *spinBox_H_blue;
    QSpinBox *spinBox_H_red;
    QSpinBox *spinBox_Cn_green;
    QSpinBox *spinBox_H_opacity;
    QSpinBox *spinBox_Cn_red;
    QSpinBox *spinBox_D_blue;
    QSpinBox *spinBox_V_red;
    QLabel *label_3;
    QLabel *label_8;
    QPushButton *pushButton_ok;
    QPushButton *pushButton_cancel;
    QSpinBox *spinBox_Cn_opacity;
    QDoubleSpinBox *doubleSpinBox_V_thickness;
    QDoubleSpinBox *doubleSpinBox_D_thickness;
    QDoubleSpinBox *doubleSpinBox_H_thickness;
    QDoubleSpinBox *doubleSpinBox_Cn_thickness;

    void setupUi(QDialog *ColorProperty)
    {
        if (ColorProperty->objectName().isEmpty())
            ColorProperty->setObjectName(QString::fromUtf8("ColorProperty"));
        ColorProperty->resize(546, 322);
        formLayout_2 = new QFormLayout(ColorProperty);
        formLayout_2->setObjectName(QString::fromUtf8("formLayout_2"));
        groupBox = new QGroupBox(ColorProperty);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        formLayout = new QFormLayout(groupBox);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        spinBox_bk_green = new QSpinBox(groupBox);
        spinBox_bk_green->setObjectName(QString::fromUtf8("spinBox_bk_green"));
        spinBox_bk_green->setMaximum(255);
        spinBox_bk_green->setSingleStep(3);

        gridLayout->addWidget(spinBox_bk_green, 2, 5, 1, 1);

        spinBox_bk_blue = new QSpinBox(groupBox);
        spinBox_bk_blue->setObjectName(QString::fromUtf8("spinBox_bk_blue"));
        spinBox_bk_blue->setMaximum(255);
        spinBox_bk_blue->setSingleStep(3);

        gridLayout->addWidget(spinBox_bk_blue, 3, 5, 1, 1);

        spinBox_D_opacity = new QSpinBox(groupBox);
        spinBox_D_opacity->setObjectName(QString::fromUtf8("spinBox_D_opacity"));
        spinBox_D_opacity->setMaximum(255);
        spinBox_D_opacity->setSingleStep(5);

        gridLayout->addWidget(spinBox_D_opacity, 4, 2, 1, 1);

        spinBox_D_red = new QSpinBox(groupBox);
        spinBox_D_red->setObjectName(QString::fromUtf8("spinBox_D_red"));
        spinBox_D_red->setMaximum(255);
        spinBox_D_red->setSingleStep(3);

        gridLayout->addWidget(spinBox_D_red, 1, 2, 1, 1);

        spinBox_V_blue = new QSpinBox(groupBox);
        spinBox_V_blue->setObjectName(QString::fromUtf8("spinBox_V_blue"));
        spinBox_V_blue->setMaximum(255);
        spinBox_V_blue->setSingleStep(3);

        gridLayout->addWidget(spinBox_V_blue, 3, 1, 1, 1);

        label = new QLabel(groupBox);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout->addWidget(label, 1, 0, 1, 1);

        spinBox_V_green = new QSpinBox(groupBox);
        spinBox_V_green->setObjectName(QString::fromUtf8("spinBox_V_green"));
        spinBox_V_green->setMaximum(255);
        spinBox_V_green->setSingleStep(3);

        gridLayout->addWidget(spinBox_V_green, 2, 1, 1, 1);

        spinBox_H_green = new QSpinBox(groupBox);
        spinBox_H_green->setObjectName(QString::fromUtf8("spinBox_H_green"));
        spinBox_H_green->setMaximum(255);
        spinBox_H_green->setSingleStep(3);

        gridLayout->addWidget(spinBox_H_green, 2, 3, 1, 1);

        spinBox_V_opacity = new QSpinBox(groupBox);
        spinBox_V_opacity->setObjectName(QString::fromUtf8("spinBox_V_opacity"));
        spinBox_V_opacity->setMaximum(255);
        spinBox_V_opacity->setSingleStep(5);

        gridLayout->addWidget(spinBox_V_opacity, 4, 1, 1, 1);

        spinBox_D_green = new QSpinBox(groupBox);
        spinBox_D_green->setObjectName(QString::fromUtf8("spinBox_D_green"));
        spinBox_D_green->setMaximum(255);
        spinBox_D_green->setSingleStep(3);

        gridLayout->addWidget(spinBox_D_green, 2, 2, 1, 1);

        spinBox_Cn_blue = new QSpinBox(groupBox);
        spinBox_Cn_blue->setObjectName(QString::fromUtf8("spinBox_Cn_blue"));
        spinBox_Cn_blue->setMaximum(255);
        spinBox_Cn_blue->setSingleStep(3);

        gridLayout->addWidget(spinBox_Cn_blue, 3, 4, 1, 1);

        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout->addWidget(label_2, 2, 0, 1, 1);

        label_7 = new QLabel(groupBox);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(label_7->sizePolicy().hasHeightForWidth());
        label_7->setSizePolicy(sizePolicy);

        gridLayout->addWidget(label_7, 4, 0, 1, 1);

        pushButton_D = new QPushButton(groupBox);
        pushButton_D->setObjectName(QString::fromUtf8("pushButton_D"));

        gridLayout->addWidget(pushButton_D, 0, 2, 1, 1);

        spinBox_bk_red = new QSpinBox(groupBox);
        spinBox_bk_red->setObjectName(QString::fromUtf8("spinBox_bk_red"));
        spinBox_bk_red->setMaximum(255);
        spinBox_bk_red->setSingleStep(3);

        gridLayout->addWidget(spinBox_bk_red, 1, 5, 1, 1);

        pushButton_Cn = new QPushButton(groupBox);
        pushButton_Cn->setObjectName(QString::fromUtf8("pushButton_Cn"));

        gridLayout->addWidget(pushButton_Cn, 0, 4, 1, 1);

        pushButton_V = new QPushButton(groupBox);
        pushButton_V->setObjectName(QString::fromUtf8("pushButton_V"));

        gridLayout->addWidget(pushButton_V, 0, 1, 1, 1);

        pushButton_bk = new QPushButton(groupBox);
        pushButton_bk->setObjectName(QString::fromUtf8("pushButton_bk"));

        gridLayout->addWidget(pushButton_bk, 0, 5, 1, 1);

        label_4 = new QLabel(groupBox);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        sizePolicy.setHeightForWidth(label_4->sizePolicy().hasHeightForWidth());
        label_4->setSizePolicy(sizePolicy);

        gridLayout->addWidget(label_4, 0, 0, 1, 1);

        pushButton_H = new QPushButton(groupBox);
        pushButton_H->setObjectName(QString::fromUtf8("pushButton_H"));

        gridLayout->addWidget(pushButton_H, 0, 3, 1, 1);

        spinBox_H_blue = new QSpinBox(groupBox);
        spinBox_H_blue->setObjectName(QString::fromUtf8("spinBox_H_blue"));
        spinBox_H_blue->setMaximum(255);
        spinBox_H_blue->setSingleStep(3);

        gridLayout->addWidget(spinBox_H_blue, 3, 3, 1, 1);

        spinBox_H_red = new QSpinBox(groupBox);
        spinBox_H_red->setObjectName(QString::fromUtf8("spinBox_H_red"));
        spinBox_H_red->setMaximum(255);
        spinBox_H_red->setSingleStep(3);

        gridLayout->addWidget(spinBox_H_red, 1, 3, 1, 1);

        spinBox_Cn_green = new QSpinBox(groupBox);
        spinBox_Cn_green->setObjectName(QString::fromUtf8("spinBox_Cn_green"));
        spinBox_Cn_green->setMaximum(255);
        spinBox_Cn_green->setSingleStep(3);

        gridLayout->addWidget(spinBox_Cn_green, 2, 4, 1, 1);

        spinBox_H_opacity = new QSpinBox(groupBox);
        spinBox_H_opacity->setObjectName(QString::fromUtf8("spinBox_H_opacity"));
        spinBox_H_opacity->setMaximum(255);
        spinBox_H_opacity->setSingleStep(3);

        gridLayout->addWidget(spinBox_H_opacity, 4, 3, 1, 1);

        spinBox_Cn_red = new QSpinBox(groupBox);
        spinBox_Cn_red->setObjectName(QString::fromUtf8("spinBox_Cn_red"));
        spinBox_Cn_red->setMaximum(255);
        spinBox_Cn_red->setSingleStep(3);

        gridLayout->addWidget(spinBox_Cn_red, 1, 4, 1, 1);

        spinBox_D_blue = new QSpinBox(groupBox);
        spinBox_D_blue->setObjectName(QString::fromUtf8("spinBox_D_blue"));
        spinBox_D_blue->setMaximum(255);
        spinBox_D_blue->setSingleStep(3);

        gridLayout->addWidget(spinBox_D_blue, 3, 2, 1, 1);

        spinBox_V_red = new QSpinBox(groupBox);
        spinBox_V_red->setObjectName(QString::fromUtf8("spinBox_V_red"));
        spinBox_V_red->setMaximum(255);
        spinBox_V_red->setSingleStep(3);

        gridLayout->addWidget(spinBox_V_red, 1, 1, 1, 1);

        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        gridLayout->addWidget(label_3, 3, 0, 1, 1);

        label_8 = new QLabel(groupBox);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        sizePolicy.setHeightForWidth(label_8->sizePolicy().hasHeightForWidth());
        label_8->setSizePolicy(sizePolicy);

        gridLayout->addWidget(label_8, 5, 0, 1, 1);

        pushButton_ok = new QPushButton(groupBox);
        pushButton_ok->setObjectName(QString::fromUtf8("pushButton_ok"));

        gridLayout->addWidget(pushButton_ok, 5, 5, 1, 1);

        pushButton_cancel = new QPushButton(groupBox);
        pushButton_cancel->setObjectName(QString::fromUtf8("pushButton_cancel"));

        gridLayout->addWidget(pushButton_cancel, 4, 5, 1, 1);

        spinBox_Cn_opacity = new QSpinBox(groupBox);
        spinBox_Cn_opacity->setObjectName(QString::fromUtf8("spinBox_Cn_opacity"));
        spinBox_Cn_opacity->setMaximum(255);
        spinBox_Cn_opacity->setSingleStep(5);

        gridLayout->addWidget(spinBox_Cn_opacity, 4, 4, 1, 1);

        doubleSpinBox_V_thickness = new QDoubleSpinBox(groupBox);
        doubleSpinBox_V_thickness->setObjectName(QString::fromUtf8("doubleSpinBox_V_thickness"));
        doubleSpinBox_V_thickness->setSingleStep(0.100000000000000);

        gridLayout->addWidget(doubleSpinBox_V_thickness, 5, 1, 1, 1);

        doubleSpinBox_D_thickness = new QDoubleSpinBox(groupBox);
        doubleSpinBox_D_thickness->setObjectName(QString::fromUtf8("doubleSpinBox_D_thickness"));
        doubleSpinBox_D_thickness->setSingleStep(0.100000000000000);

        gridLayout->addWidget(doubleSpinBox_D_thickness, 5, 2, 1, 1);

        doubleSpinBox_H_thickness = new QDoubleSpinBox(groupBox);
        doubleSpinBox_H_thickness->setObjectName(QString::fromUtf8("doubleSpinBox_H_thickness"));
        doubleSpinBox_H_thickness->setSingleStep(0.100000000000000);

        gridLayout->addWidget(doubleSpinBox_H_thickness, 5, 3, 1, 1);

        doubleSpinBox_Cn_thickness = new QDoubleSpinBox(groupBox);
        doubleSpinBox_Cn_thickness->setObjectName(QString::fromUtf8("doubleSpinBox_Cn_thickness"));
        doubleSpinBox_Cn_thickness->setSingleStep(0.100000000000000);

        gridLayout->addWidget(doubleSpinBox_Cn_thickness, 5, 4, 1, 1);


        formLayout->setLayout(0, QFormLayout::LabelRole, gridLayout);


        formLayout_2->setWidget(1, QFormLayout::FieldRole, groupBox);


        retranslateUi(ColorProperty);

        QMetaObject::connectSlotsByName(ColorProperty);
    } // setupUi

    void retranslateUi(QDialog *ColorProperty)
    {
        ColorProperty->setWindowTitle(QCoreApplication::translate("ColorProperty", "Dialog", nullptr));
        groupBox->setTitle(QCoreApplication::translate("ColorProperty", "                                                     Color    Selection", nullptr));
        label->setText(QCoreApplication::translate("ColorProperty", "Red", nullptr));
        label_2->setText(QCoreApplication::translate("ColorProperty", "Green", nullptr));
        label_7->setText(QCoreApplication::translate("ColorProperty", "Opacity", nullptr));
        pushButton_D->setText(QCoreApplication::translate("ColorProperty", "Sigma-II", nullptr));
        pushButton_Cn->setText(QCoreApplication::translate("ColorProperty", "Cn", nullptr));
        pushButton_V->setText(QCoreApplication::translate("ColorProperty", "Sigma-I", nullptr));
        pushButton_bk->setText(QCoreApplication::translate("ColorProperty", "bkground", nullptr));
        label_4->setText(QCoreApplication::translate("ColorProperty", "Color", nullptr));
        pushButton_H->setText(QCoreApplication::translate("ColorProperty", "Sigma-H", nullptr));
        label_3->setText(QCoreApplication::translate("ColorProperty", "Blue", nullptr));
        label_8->setText(QCoreApplication::translate("ColorProperty", "Thick", nullptr));
        pushButton_ok->setText(QCoreApplication::translate("ColorProperty", "OK", nullptr));
        pushButton_cancel->setText(QCoreApplication::translate("ColorProperty", "Cancel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ColorProperty: public Ui_ColorProperty {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_COLORPROPERTY_H
