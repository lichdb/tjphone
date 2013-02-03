/********************************************************************************
** Form generated from reading UI file 'sip_account.ui'
**
** Created: Wed Aug 31 16:31:05 2011
**      by: Qt User Interface Compiler version 4.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SIP_ACCOUNT_H
#define UI_SIP_ACCOUNT_H

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
#include <QtGui/QSpinBox>

QT_BEGIN_NAMESPACE

class Ui_SIP_Account
{
public:
    QGridLayout *gridLayout;
    QGroupBox *groupBox;
    QGridLayout *gridLayout_2;
    QLabel *label;
    QLineEdit *lineEdit;
    QLabel *label_2;
    QLineEdit *lineEdit_2;
    QLabel *label_3;
    QLineEdit *lineEdit_3;
    QLabel *label_4;
    QSpinBox *spinBox;
    QCheckBox *checkBox;
    QCheckBox *checkBox_2;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *SIP_Account)
    {
        if (SIP_Account->objectName().isEmpty())
            SIP_Account->setObjectName(QString::fromUtf8("SIP_Account"));
        SIP_Account->setWindowModality(Qt::WindowModal);
        SIP_Account->resize(471, 221);
        QFont font;
        font.setBold(true);
        font.setWeight(75);
        SIP_Account->setFont(font);
        gridLayout = new QGridLayout(SIP_Account);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        groupBox = new QGroupBox(SIP_Account);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        gridLayout_2 = new QGridLayout(groupBox);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        label = new QLabel(groupBox);
        label->setObjectName(QString::fromUtf8("label"));
        QFont font1;
        font1.setBold(false);
        font1.setWeight(50);
        label->setFont(font1);

        gridLayout_2->addWidget(label, 0, 0, 1, 1);

        lineEdit = new QLineEdit(groupBox);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));
        lineEdit->setFont(font1);

        gridLayout_2->addWidget(lineEdit, 0, 1, 1, 1);

        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setFont(font1);

        gridLayout_2->addWidget(label_2, 1, 0, 1, 1);

        lineEdit_2 = new QLineEdit(groupBox);
        lineEdit_2->setObjectName(QString::fromUtf8("lineEdit_2"));
        lineEdit_2->setFont(font1);

        gridLayout_2->addWidget(lineEdit_2, 1, 1, 1, 1);

        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setFont(font1);

        gridLayout_2->addWidget(label_3, 2, 0, 1, 1);

        lineEdit_3 = new QLineEdit(groupBox);
        lineEdit_3->setObjectName(QString::fromUtf8("lineEdit_3"));
        lineEdit_3->setFont(font1);

        gridLayout_2->addWidget(lineEdit_3, 2, 1, 1, 1);

        label_4 = new QLabel(groupBox);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setFont(font1);

        gridLayout_2->addWidget(label_4, 3, 0, 1, 1);

        spinBox = new QSpinBox(groupBox);
        spinBox->setObjectName(QString::fromUtf8("spinBox"));
        spinBox->setFont(font1);

        gridLayout_2->addWidget(spinBox, 3, 1, 1, 1);

        checkBox = new QCheckBox(groupBox);
        checkBox->setObjectName(QString::fromUtf8("checkBox"));
        checkBox->setFont(font1);

        gridLayout_2->addWidget(checkBox, 4, 0, 1, 1);

        checkBox_2 = new QCheckBox(groupBox);
        checkBox_2->setObjectName(QString::fromUtf8("checkBox_2"));
        checkBox_2->setFont(font1);

        gridLayout_2->addWidget(checkBox_2, 5, 0, 1, 2);


        gridLayout->addWidget(groupBox, 0, 0, 1, 1);

        buttonBox = new QDialogButtonBox(SIP_Account);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setFont(font1);
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        gridLayout->addWidget(buttonBox, 1, 0, 1, 1);


        retranslateUi(SIP_Account);
        QObject::connect(buttonBox, SIGNAL(accepted()), SIP_Account, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), SIP_Account, SLOT(reject()));

        QMetaObject::connectSlotsByName(SIP_Account);
    } // setupUi

    void retranslateUi(QDialog *SIP_Account)
    {
        SIP_Account->setWindowTitle(QApplication::translate("SIP_Account", "Configure a SIP account", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("SIP_Account", "Configure a SIP Account", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("SIP_Account", "Your SIP identity:", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("SIP_Account", "SIP Proxy address:", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("SIP_Account", "Route (optional):", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("SIP_Account", "Registration duration (sec):", 0, QApplication::UnicodeUTF8));
        checkBox->setText(QApplication::translate("SIP_Account", "Register at startup", 0, QApplication::UnicodeUTF8));
        checkBox_2->setText(QApplication::translate("SIP_Account", "Publish presence information", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class SIP_Account: public Ui_SIP_Account {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SIP_ACCOUNT_H
