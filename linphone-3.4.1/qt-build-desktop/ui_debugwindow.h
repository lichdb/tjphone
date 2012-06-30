/********************************************************************************
** Form generated from reading UI file 'debugwindow.ui'
**
** Created: Wed Aug 17 15:47:28 2011
**      by: Qt User Interface Compiler version 4.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DEBUGWINDOW_H
#define UI_DEBUGWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QTextBrowser>

QT_BEGIN_NAMESPACE

class Ui_debugwindow
{
public:
    QGridLayout *gridLayout;
    QTextBrowser *textBrowser;
    QDialogButtonBox *buttonClose;

    void setupUi(QDialog *debugwindow)
    {
        if (debugwindow->objectName().isEmpty())
            debugwindow->setObjectName(QString::fromUtf8("debugwindow"));
        debugwindow->resize(400, 300);
        gridLayout = new QGridLayout(debugwindow);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        textBrowser = new QTextBrowser(debugwindow);
        textBrowser->setObjectName(QString::fromUtf8("textBrowser"));

        gridLayout->addWidget(textBrowser, 0, 0, 1, 1);

        buttonClose = new QDialogButtonBox(debugwindow);
        buttonClose->setObjectName(QString::fromUtf8("buttonClose"));
        buttonClose->setOrientation(Qt::Horizontal);
        buttonClose->setStandardButtons(QDialogButtonBox::Close);

        gridLayout->addWidget(buttonClose, 1, 0, 1, 1);


        retranslateUi(debugwindow);
        QObject::connect(buttonClose, SIGNAL(accepted()), debugwindow, SLOT(accept()));
        QObject::connect(buttonClose, SIGNAL(rejected()), debugwindow, SLOT(reject()));

        QMetaObject::connectSlotsByName(debugwindow);
    } // setupUi

    void retranslateUi(QDialog *debugwindow)
    {
        debugwindow->setWindowTitle(QApplication::translate("debugwindow", "Debug Info", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class debugwindow: public Ui_debugwindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DEBUGWINDOW_H
