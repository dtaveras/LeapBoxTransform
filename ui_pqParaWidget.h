/********************************************************************************
** Form generated from reading UI file 'pqParaWidget.ui'
**
** Created by: Qt User Interface Compiler version 4.8.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_pqParaWidget_H
#define UI_pqParaWidget_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QWidget>
#include "pqLineEdit.h"

#include <QDebug>

QT_BEGIN_NAMESPACE

class Ui_pqParaWidget
{
public:
    QGridLayout *gridLayout;
    QCheckBox *show3DWidget;
    QGridLayout *gridLayout1;
    QLabel *label;
    pqLineEdit *positionX;
    pqLineEdit *positionY;
    pqLineEdit *positionZ;
    QLabel *label_2;
    pqLineEdit *rotationX;
    pqLineEdit *rotationY;
    pqLineEdit *rotationZ;
    QLabel *label_3;
    pqLineEdit *scaleX;
    pqLineEdit *scaleY;
    pqLineEdit *scaleZ;
    QSpacerItem *spacerItem;
    QPushButton *resetBounds;

    void setupUi(QWidget *pqParaWidget)
    {
      qDebug() << "setupUI";
        if (pqParaWidget->objectName().isEmpty())
            pqParaWidget->setObjectName(QString::fromUtf8("pqParaWidget"));
        pqParaWidget->resize(289, 167);
        gridLayout = new QGridLayout(pqParaWidget);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        show3DWidget = new QCheckBox(pqParaWidget);
        show3DWidget->setObjectName(QString::fromUtf8("show3DWidget"));

        gridLayout->addWidget(show3DWidget, 0, 0, 1, 2);

        gridLayout1 = new QGridLayout();
        gridLayout1->setObjectName(QString::fromUtf8("gridLayout1"));
        gridLayout1->setHorizontalSpacing(2);
        gridLayout1->setVerticalSpacing(2);
        label = new QLabel(pqParaWidget);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout1->addWidget(label, 0, 0, 1, 1);

        positionX = new pqLineEdit(pqParaWidget);
        positionX->setObjectName(QString::fromUtf8("positionX"));

        gridLayout1->addWidget(positionX, 0, 1, 1, 1);

        positionY = new pqLineEdit(pqParaWidget);
        positionY->setObjectName(QString::fromUtf8("positionY"));

        gridLayout1->addWidget(positionY, 0, 2, 1, 1);

        positionZ = new pqLineEdit(pqParaWidget);
        positionZ->setObjectName(QString::fromUtf8("positionZ"));

        gridLayout1->addWidget(positionZ, 0, 3, 1, 1);

        label_2 = new QLabel(pqParaWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout1->addWidget(label_2, 1, 0, 1, 1);

        rotationX = new pqLineEdit(pqParaWidget);
        rotationX->setObjectName(QString::fromUtf8("rotationX"));

        gridLayout1->addWidget(rotationX, 1, 1, 1, 1);

        rotationY = new pqLineEdit(pqParaWidget);
        rotationY->setObjectName(QString::fromUtf8("rotationY"));

        gridLayout1->addWidget(rotationY, 1, 2, 1, 1);

        rotationZ = new pqLineEdit(pqParaWidget);
        rotationZ->setObjectName(QString::fromUtf8("rotationZ"));

        gridLayout1->addWidget(rotationZ, 1, 3, 1, 1);

        label_3 = new QLabel(pqParaWidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        gridLayout1->addWidget(label_3, 2, 0, 1, 1);

        scaleX = new pqLineEdit(pqParaWidget);
        scaleX->setObjectName(QString::fromUtf8("scaleX"));

        gridLayout1->addWidget(scaleX, 2, 1, 1, 1);

        scaleY = new pqLineEdit(pqParaWidget);
        scaleY->setObjectName(QString::fromUtf8("scaleY"));

        gridLayout1->addWidget(scaleY, 2, 2, 1, 1);

        scaleZ = new pqLineEdit(pqParaWidget);
        scaleZ->setObjectName(QString::fromUtf8("scaleZ"));

        gridLayout1->addWidget(scaleZ, 2, 3, 1, 1);


        gridLayout->addLayout(gridLayout1, 1, 0, 1, 2);

        spacerItem = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(spacerItem, 2, 0, 1, 1);

        resetBounds = new QPushButton(pqParaWidget);
        resetBounds->setObjectName(QString::fromUtf8("resetBounds"));

        gridLayout->addWidget(resetBounds, 2, 1, 1, 1);

        retranslateUi(pqParaWidget);

        QMetaObject::connectSlotsByName(pqParaWidget);
    } // setupUi

    void retranslateUi(QWidget *pqParaWidget)
    {
        pqParaWidget->setWindowTitle(QApplication::translate("pqParaWidget", "Form", 0, QApplication::UnicodeUTF8));
        show3DWidget->setText(QApplication::translate("pqParaWidget", "Show Box", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("pqParaWidget", "Translate", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("pqParaWidget", "Rotate", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("pqParaWidget", "Scale", 0, QApplication::UnicodeUTF8));
        resetBounds->setText(QApplication::translate("pqParaWidget", "Reset Bounds", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class pqParaWidget: public Ui_pqParaWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_pqParaWidget_H
