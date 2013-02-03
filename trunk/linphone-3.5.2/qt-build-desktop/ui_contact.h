/********************************************************************************
** Form generated from reading UI file 'contact.ui'
**
** Created: Wed Aug 31 17:19:24 2011
**      by: Qt User Interface Compiler version 4.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONTACT_H
#define UI_CONTACT_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>

QT_BEGIN_NAMESPACE

class Ui_Contact
{
public:
    QGridLayout *gridLayout;
    QGroupBox *groupBox;
    QGridLayout *gridLayout_2;
    QLabel *label;
    QLineEdit *lineEdit;
    QLabel *label_2;
    QLineEdit *lineEdit_2;
    QCheckBox *checkBox;
    QCheckBox *checkBox_2;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *Contact)
    {
        if (Contact->objectName().isEmpty())
            Contact->setObjectName(QString::fromUtf8("Contact"));
        Contact->resize(325, 169);
        gridLayout = new QGridLayout(Contact);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        groupBox = new QGroupBox(Contact);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        gridLayout_2 = new QGridLayout(groupBox);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        label = new QLabel(groupBox);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout_2->addWidget(label, 0, 0, 1, 1);

        lineEdit = new QLineEdit(groupBox);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));

        gridLayout_2->addWidget(lineEdit, 0, 1, 1, 1);

        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout_2->addWidget(label_2, 1, 0, 1, 1);

        lineEdit_2 = new QLineEdit(groupBox);
        lineEdit_2->setObjectName(QString::fromUtf8("lineEdit_2"));

        gridLayout_2->addWidget(lineEdit_2, 1, 1, 1, 1);

        checkBox = new QCheckBox(groupBox);
        checkBox->setObjectName(QString::fromUtf8("checkBox"));

        gridLayout_2->addWidget(checkBox, 2, 0, 1, 2);

        checkBox_2 = new QCheckBox(groupBox);
        checkBox_2->setObjectName(QString::fromUtf8("checkBox_2"));

        gridLayout_2->addWidget(checkBox_2, 3, 0, 1, 2);


        gridLayout->addWidget(groupBox, 0, 0, 1, 1);

        buttonBox = new QDialogButtonBox(Contact);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        gridLayout->addWidget(buttonBox, 1, 0, 1, 1);


        retranslateUi(Contact);
        QObject::connect(buttonBox, SIGNAL(accepted()), Contact, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), Contact, SLOT(reject()));

        QMetaObject::connectSlotsByName(Contact);
    } // setupUi

    void retranslateUi(QDialog *Contact)
    {
        Contact->setWindowTitle(QApplication::translate("Contact", "Contact", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("Contact", "Contact Information", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("Contact", "Name", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("Contact", "SIP Address", 0, QApplication::UnicodeUTF8));
        checkBox->setText(QApplication::translate("Contact", "Show this contact presence status", 0, QApplication::UnicodeUTF8));
        checkBox_2->setText(QApplication::translate("Contact", "Allow this contact to see my presence status", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class Contact: public Ui_Contact {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONTACT_H
