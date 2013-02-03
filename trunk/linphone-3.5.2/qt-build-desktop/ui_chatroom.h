/********************************************************************************
** Form generated from reading UI file 'chatroom.ui'
**
** Created: Wed Aug 31 17:06:50 2011
**      by: Qt User Interface Compiler version 4.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CHATROOM_H
#define UI_CHATROOM_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QTextBrowser>
#include <QtGui/QTextEdit>

QT_BEGIN_NAMESPACE

class Ui_ChatRoom
{
public:
    QGridLayout *gridLayout;
    QTextBrowser *textBrowser;
    QTextEdit *textEdit;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *ChatRoom)
    {
        if (ChatRoom->objectName().isEmpty())
            ChatRoom->setObjectName(QString::fromUtf8("ChatRoom"));
        ChatRoom->resize(379, 287);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(ChatRoom->sizePolicy().hasHeightForWidth());
        ChatRoom->setSizePolicy(sizePolicy);
        gridLayout = new QGridLayout(ChatRoom);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setSizeConstraint(QLayout::SetMinimumSize);
        textBrowser = new QTextBrowser(ChatRoom);
        textBrowser->setObjectName(QString::fromUtf8("textBrowser"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(textBrowser->sizePolicy().hasHeightForWidth());
        textBrowser->setSizePolicy(sizePolicy1);

        gridLayout->addWidget(textBrowser, 0, 0, 1, 2);

        textEdit = new QTextEdit(ChatRoom);
        textEdit->setObjectName(QString::fromUtf8("textEdit"));
        QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(textEdit->sizePolicy().hasHeightForWidth());
        textEdit->setSizePolicy(sizePolicy2);

        gridLayout->addWidget(textEdit, 3, 0, 1, 1);

        buttonBox = new QDialogButtonBox(ChatRoom);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        QSizePolicy sizePolicy3(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(buttonBox->sizePolicy().hasHeightForWidth());
        buttonBox->setSizePolicy(sizePolicy3);
        buttonBox->setOrientation(Qt::Vertical);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        buttonBox->setCenterButtons(true);

        gridLayout->addWidget(buttonBox, 3, 1, 1, 1);


        retranslateUi(ChatRoom);
        QObject::connect(buttonBox, SIGNAL(accepted()), ChatRoom, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), ChatRoom, SLOT(reject()));

        QMetaObject::connectSlotsByName(ChatRoom);
    } // setupUi

    void retranslateUi(QDialog *ChatRoom)
    {
        ChatRoom->setWindowTitle(QApplication::translate("ChatRoom", "Dialog", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class ChatRoom: public Ui_ChatRoom {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CHATROOM_H
