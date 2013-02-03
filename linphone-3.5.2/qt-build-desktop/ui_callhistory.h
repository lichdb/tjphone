/********************************************************************************
** Form generated from reading UI file 'callhistory.ui'
**
** Created: Wed Aug 17 16:15:19 2011
**      by: Qt User Interface Compiler version 4.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CALLHISTORY_H
#define UI_CALLHISTORY_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QListView>

QT_BEGIN_NAMESPACE

class Ui_Callhistory
{
public:
    QGridLayout *gridLayout;
    QDialogButtonBox *buttonOk;
    QListView *listView;

    void setupUi(QDialog *Callhistory)
    {
        if (Callhistory->objectName().isEmpty())
            Callhistory->setObjectName(QString::fromUtf8("Callhistory"));
        Callhistory->setEnabled(true);
        Callhistory->resize(400, 300);
        gridLayout = new QGridLayout(Callhistory);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        buttonOk = new QDialogButtonBox(Callhistory);
        buttonOk->setObjectName(QString::fromUtf8("buttonOk"));
        buttonOk->setOrientation(Qt::Horizontal);
        buttonOk->setStandardButtons(QDialogButtonBox::Ok);

        gridLayout->addWidget(buttonOk, 1, 0, 1, 1);

        listView = new QListView(Callhistory);
        listView->setObjectName(QString::fromUtf8("listView"));

        gridLayout->addWidget(listView, 0, 0, 1, 1);


        retranslateUi(Callhistory);
        QObject::connect(buttonOk, SIGNAL(accepted()), Callhistory, SLOT(accept()));
        QObject::connect(buttonOk, SIGNAL(rejected()), Callhistory, SLOT(reject()));

        QMetaObject::connectSlotsByName(Callhistory);
    } // setupUi

    void retranslateUi(QDialog *Callhistory)
    {
        Callhistory->setWindowTitle(QApplication::translate("Callhistory", "CallHistory", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class Callhistory: public Ui_Callhistory {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CALLHISTORY_H
