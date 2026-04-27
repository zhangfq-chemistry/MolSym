/********************************************************************************
** Form generated from reading UI file 'MetalTemplate.ui'
**
** Created by: Qt User Interface Compiler version 5.13.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_METALTEMPLATE_H
#define UI_METALTEMPLATE_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_AtomTemplate
{
public:
    QWidget *layoutWidget;
    QGridLayout *gridLayout;
    QPushButton *Frag7;
    QPushButton *Frag3;
    QPushButton *Frag2;
    QPushButton *Frag5;
    QPushButton *Frag1;
    QPushButton *Frag6;
    QPushButton *Frag4;
    QPushButton *Frag8;

    void setupUi(QDialog *AtomTemplate)
    {
        if (AtomTemplate->objectName().isEmpty())
            AtomTemplate->setObjectName(QString::fromUtf8("AtomTemplate"));
        AtomTemplate->resize(593, 212);
        layoutWidget = new QWidget(AtomTemplate);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(30, 10, 531, 181));
        gridLayout = new QGridLayout(layoutWidget);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        Frag7 = new QPushButton(layoutWidget);
        Frag7->setObjectName(QString::fromUtf8("Frag7"));
        Frag7->setMinimumSize(QSize(81, 81));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/images/pentbipyr.svg"), QSize(), QIcon::Normal, QIcon::Off);
        Frag7->setIcon(icon);
        Frag7->setIconSize(QSize(80, 80));

        gridLayout->addWidget(Frag7, 1, 2, 1, 1);

        Frag3 = new QPushButton(layoutWidget);
        Frag3->setObjectName(QString::fromUtf8("Frag3"));
        Frag3->setEnabled(true);
        Frag3->setMinimumSize(QSize(81, 81));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/images/tet.svg"), QSize(), QIcon::Normal, QIcon::Off);
        Frag3->setIcon(icon1);
        Frag3->setIconSize(QSize(80, 80));

        gridLayout->addWidget(Frag3, 0, 2, 1, 1);

        Frag2 = new QPushButton(layoutWidget);
        Frag2->setObjectName(QString::fromUtf8("Frag2"));
        Frag2->setEnabled(true);
        Frag2->setMinimumSize(QSize(81, 81));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/images/tetsss.svg"), QSize(), QIcon::Normal, QIcon::Off);
        Frag2->setIcon(icon2);
        Frag2->setIconSize(QSize(80, 80));

        gridLayout->addWidget(Frag2, 0, 1, 1, 1);

        Frag5 = new QPushButton(layoutWidget);
        Frag5->setObjectName(QString::fromUtf8("Frag5"));
        Frag5->setEnabled(true);
        Frag5->setMinimumSize(QSize(81, 81));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/images/oct.svg"), QSize(), QIcon::Normal, QIcon::Off);
        Frag5->setIcon(icon3);
        Frag5->setIconSize(QSize(80, 80));

        gridLayout->addWidget(Frag5, 1, 0, 1, 1);

        Frag1 = new QPushButton(layoutWidget);
        Frag1->setObjectName(QString::fromUtf8("Frag1"));
        Frag1->setMinimumSize(QSize(81, 81));
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/images/tripla.svg"), QSize(), QIcon::Normal, QIcon::Off);
        Frag1->setIcon(icon4);
        Frag1->setIconSize(QSize(80, 80));

        gridLayout->addWidget(Frag1, 0, 0, 1, 1);

        Frag6 = new QPushButton(layoutWidget);
        Frag6->setObjectName(QString::fromUtf8("Frag6"));
        Frag6->setMinimumSize(QSize(81, 81));
        QIcon icon5;
        icon5.addFile(QString::fromUtf8(":/images/captrig.svg"), QSize(), QIcon::Normal, QIcon::Off);
        Frag6->setIcon(icon5);
        Frag6->setIconSize(QSize(80, 80));

        gridLayout->addWidget(Frag6, 1, 1, 1, 1);

        Frag4 = new QPushButton(layoutWidget);
        Frag4->setObjectName(QString::fromUtf8("Frag4"));
        Frag4->setMinimumSize(QSize(81, 81));
        QIcon icon6;
        icon6.addFile(QString::fromUtf8(":/images/seesaw.svg"), QSize(), QIcon::Normal, QIcon::Off);
        Frag4->setIcon(icon6);
        Frag4->setIconSize(QSize(80, 80));

        gridLayout->addWidget(Frag4, 0, 3, 1, 1);

        Frag8 = new QPushButton(layoutWidget);
        Frag8->setObjectName(QString::fromUtf8("Frag8"));
        Frag8->setMinimumSize(QSize(81, 81));
        QIcon icon7;
        icon7.addFile(QString::fromUtf8(":/images/sqaprism.svg"), QSize(), QIcon::Normal, QIcon::Off);
        Frag8->setIcon(icon7);
        Frag8->setIconSize(QSize(80, 80));

        gridLayout->addWidget(Frag8, 1, 3, 1, 1);


        retranslateUi(AtomTemplate);

        QMetaObject::connectSlotsByName(AtomTemplate);
    } // setupUi

    void retranslateUi(QDialog *AtomTemplate)
    {
        AtomTemplate->setWindowTitle(QCoreApplication::translate("AtomTemplate", "Dialog", nullptr));
        Frag7->setText(QString());
        Frag3->setText(QString());
        Frag2->setText(QString());
        Frag5->setText(QString());
        Frag1->setText(QString());
        Frag6->setText(QString());
        Frag4->setText(QString());
        Frag8->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class AtomTemplate: public Ui_AtomTemplate {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_METALTEMPLATE_H
