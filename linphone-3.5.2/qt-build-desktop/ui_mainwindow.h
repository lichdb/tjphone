/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created: Wed Aug 24 11:30:43 2011
**      by: Qt User Interface Compiler version 4.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QListWidget>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QSplitter>
#include <QtGui/QStatusBar>
#include <QtGui/QTabWidget>
#include <QtGui/QToolBar>
#include <QtGui/QToolButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *action_Preferences;
    QAction *action_Call_history;
    QAction *action_Quit;
    QAction *action_audio_only;
    QAction *actionAudio_vedio;
    QAction *action_About;
    QAction *action_Show_debug_window;
    QAction *actionHomepage;
    QWidget *centralWidget;
    QGridLayout *gridLayout_2;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QToolButton *toolButton;
    QGroupBox *groupBox;
    QGridLayout *gridLayout;
    QLineEdit *lineEdit;
    QToolButton *toolButton_2;
    QSpacerItem *horizontalSpacer;
    QTabWidget *tabWidget;
    QWidget *tabcontacts;
    QGridLayout *gridLayout_3;
    QGroupBox *groupBox_list;
    QGridLayout *gridLayout_4;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label;
    QLineEdit *lineEdit_phonenumber;
    QLabel *label_2;
    QComboBox *comboBox_2;
    QListWidget *listWidget;
    QSpacerItem *horizontalSpacer_4;
    QWidget *tabkey;
    QHBoxLayout *horizontalLayout_3;
    QSpacerItem *verticalSpacer;
    QSplitter *splitter;
    QFrame *frame;
    QGridLayout *gridLayout_5;
    QPushButton *pushButton_1;
    QPushButton *pushButton_2;
    QPushButton *pushButton_3;
    QPushButton *pushButton_A;
    QPushButton *pushButton_4;
    QPushButton *pushButton_5;
    QPushButton *pushButton_6;
    QPushButton *pushButton_B;
    QPushButton *pushButton_7;
    QPushButton *pushButton_8;
    QPushButton *pushButton_9;
    QPushButton *pushButton_C;
    QPushButton *pushButton_10;
    QPushButton *pushButton_0;
    QPushButton *pushButton_11;
    QPushButton *pushButton_D;
    QSpacerItem *verticalSpacer_2;
    QGroupBox *groupBox_identity;
    QVBoxLayout *verticalLayout_2;
    QComboBox *comboBox;
    QMenuBar *menuBar;
    QMenu *menu_TJPhone;
    QMenu *menu_Options;
    QMenu *menu_Help;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(520, 428);
        action_Preferences = new QAction(MainWindow);
        action_Preferences->setObjectName(QString::fromUtf8("action_Preferences"));
        action_Call_history = new QAction(MainWindow);
        action_Call_history->setObjectName(QString::fromUtf8("action_Call_history"));
        action_Quit = new QAction(MainWindow);
        action_Quit->setObjectName(QString::fromUtf8("action_Quit"));
        action_audio_only = new QAction(MainWindow);
        action_audio_only->setObjectName(QString::fromUtf8("action_audio_only"));
        action_audio_only->setCheckable(true);
        actionAudio_vedio = new QAction(MainWindow);
        actionAudio_vedio->setObjectName(QString::fromUtf8("actionAudio_vedio"));
        actionAudio_vedio->setCheckable(true);
        action_About = new QAction(MainWindow);
        action_About->setObjectName(QString::fromUtf8("action_About"));
        action_Show_debug_window = new QAction(MainWindow);
        action_Show_debug_window->setObjectName(QString::fromUtf8("action_Show_debug_window"));
        actionHomepage = new QAction(MainWindow);
        actionHomepage->setObjectName(QString::fromUtf8("actionHomepage"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        gridLayout_2 = new QGridLayout(centralWidget);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setSizeConstraint(QLayout::SetMaximumSize);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(3);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setSizeConstraint(QLayout::SetFixedSize);
        horizontalLayout->setContentsMargins(-1, 0, 0, -1);
        toolButton = new QToolButton(centralWidget);
        toolButton->setObjectName(QString::fromUtf8("toolButton"));

        horizontalLayout->addWidget(toolButton);

        groupBox = new QGroupBox(centralWidget);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(groupBox->sizePolicy().hasHeightForWidth());
        groupBox->setSizePolicy(sizePolicy);
        gridLayout = new QGridLayout(groupBox);
        gridLayout->setSpacing(0);
        gridLayout->setContentsMargins(0, 0, 0, 0);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        lineEdit = new QLineEdit(groupBox);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(lineEdit->sizePolicy().hasHeightForWidth());
        lineEdit->setSizePolicy(sizePolicy1);

        gridLayout->addWidget(lineEdit, 0, 0, 1, 1);


        horizontalLayout->addWidget(groupBox);

        toolButton_2 = new QToolButton(centralWidget);
        toolButton_2->setObjectName(QString::fromUtf8("toolButton_2"));

        horizontalLayout->addWidget(toolButton_2);


        verticalLayout->addLayout(horizontalLayout);


        gridLayout_2->addLayout(verticalLayout, 0, 0, 1, 1);

        horizontalSpacer = new QSpacerItem(499, 13, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_2->addItem(horizontalSpacer, 1, 0, 1, 1);

        tabWidget = new QTabWidget(centralWidget);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        sizePolicy1.setHeightForWidth(tabWidget->sizePolicy().hasHeightForWidth());
        tabWidget->setSizePolicy(sizePolicy1);
        tabcontacts = new QWidget();
        tabcontacts->setObjectName(QString::fromUtf8("tabcontacts"));
        gridLayout_3 = new QGridLayout(tabcontacts);
        gridLayout_3->setSpacing(6);
        gridLayout_3->setContentsMargins(11, 11, 11, 11);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        groupBox_list = new QGroupBox(tabcontacts);
        groupBox_list->setObjectName(QString::fromUtf8("groupBox_list"));
        gridLayout_4 = new QGridLayout(groupBox_list);
        gridLayout_4->setSpacing(0);
        gridLayout_4->setContentsMargins(0, 0, 0, 0);
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        label = new QLabel(groupBox_list);
        label->setObjectName(QString::fromUtf8("label"));

        horizontalLayout_2->addWidget(label);

        lineEdit_phonenumber = new QLineEdit(groupBox_list);
        lineEdit_phonenumber->setObjectName(QString::fromUtf8("lineEdit_phonenumber"));
        QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(lineEdit_phonenumber->sizePolicy().hasHeightForWidth());
        lineEdit_phonenumber->setSizePolicy(sizePolicy2);

        horizontalLayout_2->addWidget(lineEdit_phonenumber);

        label_2 = new QLabel(groupBox_list);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        horizontalLayout_2->addWidget(label_2);

        comboBox_2 = new QComboBox(groupBox_list);
        comboBox_2->setObjectName(QString::fromUtf8("comboBox_2"));

        horizontalLayout_2->addWidget(comboBox_2);


        gridLayout_4->addLayout(horizontalLayout_2, 0, 0, 1, 1);

        listWidget = new QListWidget(groupBox_list);
        listWidget->setObjectName(QString::fromUtf8("listWidget"));
        listWidget->setFrameShape(QFrame::StyledPanel);
        listWidget->setFrameShadow(QFrame::Plain);
        listWidget->setLineWidth(2);

        gridLayout_4->addWidget(listWidget, 2, 0, 1, 1);

        horizontalSpacer_4 = new QSpacerItem(40, 2, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_4->addItem(horizontalSpacer_4, 1, 0, 1, 1);


        gridLayout_3->addWidget(groupBox_list, 0, 0, 1, 1);

        tabWidget->addTab(tabcontacts, QString());
        tabkey = new QWidget();
        tabkey->setObjectName(QString::fromUtf8("tabkey"));
        horizontalLayout_3 = new QHBoxLayout(tabkey);
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        verticalSpacer = new QSpacerItem(60, 241, QSizePolicy::Minimum, QSizePolicy::Preferred);

        horizontalLayout_3->addItem(verticalSpacer);

        splitter = new QSplitter(tabkey);
        splitter->setObjectName(QString::fromUtf8("splitter"));
        splitter->setOrientation(Qt::Vertical);
        frame = new QFrame(splitter);
        frame->setObjectName(QString::fromUtf8("frame"));
        sizePolicy1.setHeightForWidth(frame->sizePolicy().hasHeightForWidth());
        frame->setSizePolicy(sizePolicy1);
        frame->setMaximumSize(QSize(500, 500));
        frame->setAutoFillBackground(false);
        frame->setFrameShape(QFrame::Box);
        frame->setFrameShadow(QFrame::Plain);
        frame->setLineWidth(2);
        gridLayout_5 = new QGridLayout(frame);
        gridLayout_5->setSpacing(6);
        gridLayout_5->setContentsMargins(11, 11, 11, 11);
        gridLayout_5->setObjectName(QString::fromUtf8("gridLayout_5"));
        pushButton_1 = new QPushButton(frame);
        pushButton_1->setObjectName(QString::fromUtf8("pushButton_1"));
        sizePolicy1.setHeightForWidth(pushButton_1->sizePolicy().hasHeightForWidth());
        pushButton_1->setSizePolicy(sizePolicy1);

        gridLayout_5->addWidget(pushButton_1, 0, 0, 1, 1);

        pushButton_2 = new QPushButton(frame);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));
        sizePolicy1.setHeightForWidth(pushButton_2->sizePolicy().hasHeightForWidth());
        pushButton_2->setSizePolicy(sizePolicy1);

        gridLayout_5->addWidget(pushButton_2, 0, 1, 1, 1);

        pushButton_3 = new QPushButton(frame);
        pushButton_3->setObjectName(QString::fromUtf8("pushButton_3"));
        sizePolicy1.setHeightForWidth(pushButton_3->sizePolicy().hasHeightForWidth());
        pushButton_3->setSizePolicy(sizePolicy1);

        gridLayout_5->addWidget(pushButton_3, 0, 2, 1, 1);

        pushButton_A = new QPushButton(frame);
        pushButton_A->setObjectName(QString::fromUtf8("pushButton_A"));
        sizePolicy1.setHeightForWidth(pushButton_A->sizePolicy().hasHeightForWidth());
        pushButton_A->setSizePolicy(sizePolicy1);

        gridLayout_5->addWidget(pushButton_A, 0, 3, 1, 1);

        pushButton_4 = new QPushButton(frame);
        pushButton_4->setObjectName(QString::fromUtf8("pushButton_4"));
        sizePolicy1.setHeightForWidth(pushButton_4->sizePolicy().hasHeightForWidth());
        pushButton_4->setSizePolicy(sizePolicy1);

        gridLayout_5->addWidget(pushButton_4, 1, 0, 1, 1);

        pushButton_5 = new QPushButton(frame);
        pushButton_5->setObjectName(QString::fromUtf8("pushButton_5"));
        sizePolicy1.setHeightForWidth(pushButton_5->sizePolicy().hasHeightForWidth());
        pushButton_5->setSizePolicy(sizePolicy1);

        gridLayout_5->addWidget(pushButton_5, 1, 1, 1, 1);

        pushButton_6 = new QPushButton(frame);
        pushButton_6->setObjectName(QString::fromUtf8("pushButton_6"));
        sizePolicy1.setHeightForWidth(pushButton_6->sizePolicy().hasHeightForWidth());
        pushButton_6->setSizePolicy(sizePolicy1);

        gridLayout_5->addWidget(pushButton_6, 1, 2, 1, 1);

        pushButton_B = new QPushButton(frame);
        pushButton_B->setObjectName(QString::fromUtf8("pushButton_B"));
        sizePolicy1.setHeightForWidth(pushButton_B->sizePolicy().hasHeightForWidth());
        pushButton_B->setSizePolicy(sizePolicy1);

        gridLayout_5->addWidget(pushButton_B, 1, 3, 1, 1);

        pushButton_7 = new QPushButton(frame);
        pushButton_7->setObjectName(QString::fromUtf8("pushButton_7"));
        sizePolicy1.setHeightForWidth(pushButton_7->sizePolicy().hasHeightForWidth());
        pushButton_7->setSizePolicy(sizePolicy1);

        gridLayout_5->addWidget(pushButton_7, 2, 0, 1, 1);

        pushButton_8 = new QPushButton(frame);
        pushButton_8->setObjectName(QString::fromUtf8("pushButton_8"));
        sizePolicy1.setHeightForWidth(pushButton_8->sizePolicy().hasHeightForWidth());
        pushButton_8->setSizePolicy(sizePolicy1);

        gridLayout_5->addWidget(pushButton_8, 2, 1, 1, 1);

        pushButton_9 = new QPushButton(frame);
        pushButton_9->setObjectName(QString::fromUtf8("pushButton_9"));
        sizePolicy1.setHeightForWidth(pushButton_9->sizePolicy().hasHeightForWidth());
        pushButton_9->setSizePolicy(sizePolicy1);

        gridLayout_5->addWidget(pushButton_9, 2, 2, 1, 1);

        pushButton_C = new QPushButton(frame);
        pushButton_C->setObjectName(QString::fromUtf8("pushButton_C"));
        sizePolicy1.setHeightForWidth(pushButton_C->sizePolicy().hasHeightForWidth());
        pushButton_C->setSizePolicy(sizePolicy1);

        gridLayout_5->addWidget(pushButton_C, 2, 3, 1, 1);

        pushButton_10 = new QPushButton(frame);
        pushButton_10->setObjectName(QString::fromUtf8("pushButton_10"));
        sizePolicy1.setHeightForWidth(pushButton_10->sizePolicy().hasHeightForWidth());
        pushButton_10->setSizePolicy(sizePolicy1);

        gridLayout_5->addWidget(pushButton_10, 3, 0, 1, 1);

        pushButton_0 = new QPushButton(frame);
        pushButton_0->setObjectName(QString::fromUtf8("pushButton_0"));
        sizePolicy1.setHeightForWidth(pushButton_0->sizePolicy().hasHeightForWidth());
        pushButton_0->setSizePolicy(sizePolicy1);

        gridLayout_5->addWidget(pushButton_0, 3, 1, 1, 1);

        pushButton_11 = new QPushButton(frame);
        pushButton_11->setObjectName(QString::fromUtf8("pushButton_11"));
        sizePolicy1.setHeightForWidth(pushButton_11->sizePolicy().hasHeightForWidth());
        pushButton_11->setSizePolicy(sizePolicy1);

        gridLayout_5->addWidget(pushButton_11, 3, 2, 1, 1);

        pushButton_D = new QPushButton(frame);
        pushButton_D->setObjectName(QString::fromUtf8("pushButton_D"));
        sizePolicy1.setHeightForWidth(pushButton_D->sizePolicy().hasHeightForWidth());
        pushButton_D->setSizePolicy(sizePolicy1);

        gridLayout_5->addWidget(pushButton_D, 3, 3, 1, 1);

        splitter->addWidget(frame);

        horizontalLayout_3->addWidget(splitter);

        verticalSpacer_2 = new QSpacerItem(60, 241, QSizePolicy::Minimum, QSizePolicy::Preferred);

        horizontalLayout_3->addItem(verticalSpacer_2);

        tabWidget->addTab(tabkey, QString());

        gridLayout_2->addWidget(tabWidget, 2, 0, 1, 1);

        groupBox_identity = new QGroupBox(centralWidget);
        groupBox_identity->setObjectName(QString::fromUtf8("groupBox_identity"));
        QSizePolicy sizePolicy3(QSizePolicy::Minimum, QSizePolicy::Maximum);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(groupBox_identity->sizePolicy().hasHeightForWidth());
        groupBox_identity->setSizePolicy(sizePolicy3);
        verticalLayout_2 = new QVBoxLayout(groupBox_identity);
        verticalLayout_2->setSpacing(0);
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        comboBox = new QComboBox(groupBox_identity);
        comboBox->setObjectName(QString::fromUtf8("comboBox"));

        verticalLayout_2->addWidget(comboBox);


        gridLayout_2->addWidget(groupBox_identity, 3, 0, 1, 1);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 520, 23));
        menu_TJPhone = new QMenu(menuBar);
        menu_TJPhone->setObjectName(QString::fromUtf8("menu_TJPhone"));
        menu_Options = new QMenu(menuBar);
        menu_Options->setObjectName(QString::fromUtf8("menu_Options"));
        menu_Help = new QMenu(menuBar);
        menu_Help->setObjectName(QString::fromUtf8("menu_Help"));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);

        menuBar->addAction(menu_TJPhone->menuAction());
        menuBar->addAction(menu_Options->menuAction());
        menuBar->addAction(menu_Help->menuAction());
        menu_TJPhone->addAction(action_Preferences);
        menu_TJPhone->addAction(action_Call_history);
        menu_TJPhone->addSeparator();
        menu_TJPhone->addAction(action_Quit);
        menu_Options->addAction(action_audio_only);
        menu_Options->addAction(actionAudio_vedio);
        menu_Help->addAction(action_About);
        menu_Help->addAction(action_Show_debug_window);
        menu_Help->addAction(actionHomepage);

        retranslateUi(MainWindow);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0, QApplication::UnicodeUTF8));
        action_Preferences->setText(QApplication::translate("MainWindow", "&Preferences", 0, QApplication::UnicodeUTF8));
        action_Call_history->setText(QApplication::translate("MainWindow", "&Call history", 0, QApplication::UnicodeUTF8));
        action_Quit->setText(QApplication::translate("MainWindow", "&Quit", 0, QApplication::UnicodeUTF8));
        action_audio_only->setText(QApplication::translate("MainWindow", "&audio only", 0, QApplication::UnicodeUTF8));
        actionAudio_vedio->setText(QApplication::translate("MainWindow", "audio & &video", 0, QApplication::UnicodeUTF8));
        action_About->setText(QApplication::translate("MainWindow", "&About", 0, QApplication::UnicodeUTF8));
        action_Show_debug_window->setText(QApplication::translate("MainWindow", "&Show debug window", 0, QApplication::UnicodeUTF8));
        actionHomepage->setText(QApplication::translate("MainWindow", "Homepage", 0, QApplication::UnicodeUTF8));
        toolButton->setText(QApplication::translate("MainWindow", "...", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("MainWindow", "Phone number or SIP address", 0, QApplication::UnicodeUTF8));
        toolButton_2->setText(QApplication::translate("MainWindow", "...", 0, QApplication::UnicodeUTF8));
        groupBox_list->setTitle(QApplication::translate("MainWindow", "Contact list", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("MainWindow", "Lookup:", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("MainWindow", "in", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tabcontacts), QApplication::translate("MainWindow", "Contacts", 0, QApplication::UnicodeUTF8));
        pushButton_1->setText(QApplication::translate("MainWindow", "1", 0, QApplication::UnicodeUTF8));
        pushButton_2->setText(QApplication::translate("MainWindow", "2", 0, QApplication::UnicodeUTF8));
        pushButton_3->setText(QApplication::translate("MainWindow", "3", 0, QApplication::UnicodeUTF8));
        pushButton_A->setText(QApplication::translate("MainWindow", "A", 0, QApplication::UnicodeUTF8));
        pushButton_4->setText(QApplication::translate("MainWindow", "4", 0, QApplication::UnicodeUTF8));
        pushButton_5->setText(QApplication::translate("MainWindow", "5", 0, QApplication::UnicodeUTF8));
        pushButton_6->setText(QApplication::translate("MainWindow", "6", 0, QApplication::UnicodeUTF8));
        pushButton_B->setText(QApplication::translate("MainWindow", "B", 0, QApplication::UnicodeUTF8));
        pushButton_7->setText(QApplication::translate("MainWindow", "7", 0, QApplication::UnicodeUTF8));
        pushButton_8->setText(QApplication::translate("MainWindow", "8", 0, QApplication::UnicodeUTF8));
        pushButton_9->setText(QApplication::translate("MainWindow", "9", 0, QApplication::UnicodeUTF8));
        pushButton_C->setText(QApplication::translate("MainWindow", "C", 0, QApplication::UnicodeUTF8));
        pushButton_10->setText(QApplication::translate("MainWindow", "*", 0, QApplication::UnicodeUTF8));
        pushButton_0->setText(QApplication::translate("MainWindow", "0", 0, QApplication::UnicodeUTF8));
        pushButton_11->setText(QApplication::translate("MainWindow", "#", 0, QApplication::UnicodeUTF8));
        pushButton_D->setText(QApplication::translate("MainWindow", "D", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tabkey), QApplication::translate("MainWindow", "Keypad", 0, QApplication::UnicodeUTF8));
        groupBox_identity->setTitle(QApplication::translate("MainWindow", "My current account", 0, QApplication::UnicodeUTF8));
        menu_TJPhone->setTitle(QApplication::translate("MainWindow", "&TJPhone", 0, QApplication::UnicodeUTF8));
        menu_Options->setTitle(QApplication::translate("MainWindow", "&Options", 0, QApplication::UnicodeUTF8));
        menu_Help->setTitle(QApplication::translate("MainWindow", "&Help", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
