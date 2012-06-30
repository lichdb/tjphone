/********************************************************************************
** Form generated from reading UI file 'preference.ui'
**
** Created: Wed Aug 31 17:06:50 2011
**      by: Qt User Interface Compiler version 4.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PREFERENCE_H
#define UI_PREFERENCE_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QFormLayout>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QListView>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QSpinBox>
#include <QtGui/QTabWidget>
#include <QtGui/QTableWidget>
#include <QtGui/QToolButton>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Preference
{
public:
    QGridLayout *gridLayout;
    QDialogButtonBox *buttonBox;
    QTabWidget *tabPreference;
    QWidget *tabNetWork;
    QGridLayout *gridLayout_4;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout_3;
    QLabel *label;
    QSpinBox *spinBox_2;
    QLabel *label_2;
    QSpinBox *spinBox_3;
    QLabel *label_3;
    QSpinBox *spinBox_4;
    QLabel *label_4;
    QSpinBox *spinBox_5;
    QGroupBox *groupBox;
    QGridLayout *gridLayout_5;
    QCheckBox *checkBox;
    QSpinBox *spinBox;
    QCheckBox *checkBox_2;
    QCheckBox *checkBox_3;
    QGroupBox *groupBox_3;
    QGridLayout *gridLayout_2;
    QRadioButton *radioButton;
    QRadioButton *radioButton_2;
    QLabel *label_5;
    QRadioButton *radioButton_3;
    QLabel *label_6;
    QLineEdit *lineEdit_2;
    QLineEdit *lineEdit;
    QWidget *tabMultimedia;
    QGridLayout *gridLayout_6;
    QGroupBox *groupBox_4;
    QGridLayout *gridLayout_7;
    QLabel *label_7;
    QComboBox *comboBox;
    QLabel *label_8;
    QComboBox *comboBox_2;
    QLabel *label_9;
    QComboBox *comboBox_3;
    QLabel *label_10;
    QLineEdit *lineEdit_3;
    QCheckBox *checkBox_4;
    QToolButton *toolButton;
    QPushButton *pushButton;
    QGroupBox *groupBox_5;
    QGridLayout *gridLayout_8;
    QLabel *label_11;
    QComboBox *comboBox_4;
    QLabel *label_12;
    QComboBox *comboBox_5;
    QWidget *tabSIP;
    QGridLayout *gridLayout_12;
    QGroupBox *groupBox_6;
    QGridLayout *gridLayout_9;
    QLabel *label_13;
    QLineEdit *lineEdit_4;
    QLabel *label_14;
    QLineEdit *lineEdit_5;
    QLabel *label_15;
    QLineEdit *lineEdit_6;
    QGroupBox *groupBox_7;
    QGridLayout *gridLayout_10;
    QListView *listView;
    QPushButton *pushButton_3;
    QPushButton *pushButton_4;
    QPushButton *pushButton_5;
    QGroupBox *groupBox_8;
    QGridLayout *gridLayout_11;
    QPushButton *pushButton_2;
    QWidget *tabCodecs;
    QGridLayout *gridLayout_13;
    QGroupBox *groupBox_10;
    QGridLayout *gridLayout_15;
    QLabel *label_16;
    QSpinBox *spinBox_6;
    QLabel *label_17;
    QSpinBox *spinBox_8;
    QGroupBox *groupBox_9;
    QGridLayout *gridLayout_14;
    QComboBox *comboBox_6;
    QTableWidget *tableWidget;
    QPushButton *pushButton_9;
    QPushButton *pushButton_8;
    QPushButton *pushButton_7;
    QPushButton *pushButton_6;
    QWidget *tabInterface;
    QFormLayout *formLayout;
    QGroupBox *groupBox_11;
    QGridLayout *gridLayout_16;
    QComboBox *comboBox_7;
    QGroupBox *groupBox_12;
    QGridLayout *gridLayout_17;
    QCheckBox *checkBox_5;

    void setupUi(QDialog *Preference)
    {
        if (Preference->objectName().isEmpty())
            Preference->setObjectName(QString::fromUtf8("Preference"));
        Preference->resize(469, 416);
        gridLayout = new QGridLayout(Preference);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        buttonBox = new QDialogButtonBox(Preference);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Apply);

        gridLayout->addWidget(buttonBox, 3, 0, 1, 1);

        tabPreference = new QTabWidget(Preference);
        tabPreference->setObjectName(QString::fromUtf8("tabPreference"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(tabPreference->sizePolicy().hasHeightForWidth());
        tabPreference->setSizePolicy(sizePolicy);
        tabPreference->setLayoutDirection(Qt::LeftToRight);
        tabNetWork = new QWidget();
        tabNetWork->setObjectName(QString::fromUtf8("tabNetWork"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Minimum);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(tabNetWork->sizePolicy().hasHeightForWidth());
        tabNetWork->setSizePolicy(sizePolicy1);
        gridLayout_4 = new QGridLayout(tabNetWork);
        gridLayout_4->setSpacing(0);
        gridLayout_4->setContentsMargins(0, 0, 0, 0);
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        groupBox_2 = new QGroupBox(tabNetWork);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(groupBox_2->sizePolicy().hasHeightForWidth());
        groupBox_2->setSizePolicy(sizePolicy2);
        QFont font;
        font.setBold(true);
        font.setWeight(75);
        groupBox_2->setFont(font);
        gridLayout_3 = new QGridLayout(groupBox_2);
        gridLayout_3->setSpacing(0);
        gridLayout_3->setContentsMargins(0, 0, 0, 0);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        label = new QLabel(groupBox_2);
        label->setObjectName(QString::fromUtf8("label"));
        QFont font1;
        font1.setBold(false);
        font1.setWeight(50);
        label->setFont(font1);
        label->setAcceptDrops(false);
        label->setLayoutDirection(Qt::LeftToRight);
        label->setAlignment(Qt::AlignCenter);

        gridLayout_3->addWidget(label, 0, 0, 1, 1);

        spinBox_2 = new QSpinBox(groupBox_2);
        spinBox_2->setObjectName(QString::fromUtf8("spinBox_2"));
        QSizePolicy sizePolicy3(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(spinBox_2->sizePolicy().hasHeightForWidth());
        spinBox_2->setSizePolicy(sizePolicy3);
        spinBox_2->setFont(font1);
        spinBox_2->setAutoFillBackground(true);
        spinBox_2->setFrame(true);

        gridLayout_3->addWidget(spinBox_2, 0, 1, 1, 1);

        label_2 = new QLabel(groupBox_2);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setFont(font1);
        label_2->setFocusPolicy(Qt::NoFocus);
        label_2->setLayoutDirection(Qt::LeftToRight);
        label_2->setAutoFillBackground(false);
        label_2->setFrameShape(QFrame::NoFrame);
        label_2->setTextFormat(Qt::AutoText);
        label_2->setScaledContents(false);
        label_2->setAlignment(Qt::AlignCenter);

        gridLayout_3->addWidget(label_2, 1, 0, 1, 1);

        spinBox_3 = new QSpinBox(groupBox_2);
        spinBox_3->setObjectName(QString::fromUtf8("spinBox_3"));
        spinBox_3->setFont(font1);

        gridLayout_3->addWidget(spinBox_3, 1, 1, 1, 1);

        label_3 = new QLabel(groupBox_2);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setFont(font1);
        label_3->setAlignment(Qt::AlignCenter);

        gridLayout_3->addWidget(label_3, 2, 0, 1, 1);

        spinBox_4 = new QSpinBox(groupBox_2);
        spinBox_4->setObjectName(QString::fromUtf8("spinBox_4"));
        spinBox_4->setFont(font1);

        gridLayout_3->addWidget(spinBox_4, 2, 1, 1, 1);

        label_4 = new QLabel(groupBox_2);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setFont(font1);
        label_4->setAlignment(Qt::AlignCenter);

        gridLayout_3->addWidget(label_4, 3, 0, 1, 1);

        spinBox_5 = new QSpinBox(groupBox_2);
        spinBox_5->setObjectName(QString::fromUtf8("spinBox_5"));
        spinBox_5->setFont(font1);

        gridLayout_3->addWidget(spinBox_5, 3, 1, 1, 1);


        gridLayout_4->addWidget(groupBox_2, 1, 0, 1, 1);

        groupBox = new QGroupBox(tabNetWork);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        sizePolicy2.setHeightForWidth(groupBox->sizePolicy().hasHeightForWidth());
        groupBox->setSizePolicy(sizePolicy2);
        groupBox->setFont(font);
        gridLayout_5 = new QGridLayout(groupBox);
        gridLayout_5->setObjectName(QString::fromUtf8("gridLayout_5"));
        checkBox = new QCheckBox(groupBox);
        checkBox->setObjectName(QString::fromUtf8("checkBox"));
        checkBox->setFont(font1);

        gridLayout_5->addWidget(checkBox, 0, 0, 1, 1);

        spinBox = new QSpinBox(groupBox);
        spinBox->setObjectName(QString::fromUtf8("spinBox"));
        spinBox->setFont(font1);

        gridLayout_5->addWidget(spinBox, 0, 1, 1, 1);

        checkBox_2 = new QCheckBox(groupBox);
        checkBox_2->setObjectName(QString::fromUtf8("checkBox_2"));
        checkBox_2->setFont(font1);

        gridLayout_5->addWidget(checkBox_2, 1, 0, 1, 1);

        checkBox_3 = new QCheckBox(groupBox);
        checkBox_3->setObjectName(QString::fromUtf8("checkBox_3"));
        checkBox_3->setFont(font1);

        gridLayout_5->addWidget(checkBox_3, 2, 0, 1, 1);


        gridLayout_4->addWidget(groupBox, 0, 0, 1, 1);

        groupBox_3 = new QGroupBox(tabNetWork);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        sizePolicy2.setHeightForWidth(groupBox_3->sizePolicy().hasHeightForWidth());
        groupBox_3->setSizePolicy(sizePolicy2);
        groupBox_3->setFont(font);
        gridLayout_2 = new QGridLayout(groupBox_3);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        radioButton = new QRadioButton(groupBox_3);
        radioButton->setObjectName(QString::fromUtf8("radioButton"));
        radioButton->setFont(font1);

        gridLayout_2->addWidget(radioButton, 0, 0, 1, 3);

        radioButton_2 = new QRadioButton(groupBox_3);
        radioButton_2->setObjectName(QString::fromUtf8("radioButton_2"));
        radioButton_2->setFont(font1);

        gridLayout_2->addWidget(radioButton_2, 1, 0, 1, 3);

        label_5 = new QLabel(groupBox_3);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setFont(font1);

        gridLayout_2->addWidget(label_5, 2, 0, 1, 1);

        radioButton_3 = new QRadioButton(groupBox_3);
        radioButton_3->setObjectName(QString::fromUtf8("radioButton_3"));
        radioButton_3->setFont(font1);

        gridLayout_2->addWidget(radioButton_3, 3, 0, 1, 3);

        label_6 = new QLabel(groupBox_3);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setFont(font1);

        gridLayout_2->addWidget(label_6, 4, 0, 1, 1);

        lineEdit_2 = new QLineEdit(groupBox_3);
        lineEdit_2->setObjectName(QString::fromUtf8("lineEdit_2"));
        lineEdit_2->setFont(font1);

        gridLayout_2->addWidget(lineEdit_2, 4, 1, 1, 1);

        lineEdit = new QLineEdit(groupBox_3);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));
        lineEdit->setFont(font1);

        gridLayout_2->addWidget(lineEdit, 2, 1, 1, 1);


        gridLayout_4->addWidget(groupBox_3, 2, 0, 1, 1);

        tabPreference->addTab(tabNetWork, QString());
        tabMultimedia = new QWidget();
        tabMultimedia->setObjectName(QString::fromUtf8("tabMultimedia"));
        sizePolicy1.setHeightForWidth(tabMultimedia->sizePolicy().hasHeightForWidth());
        tabMultimedia->setSizePolicy(sizePolicy1);
        gridLayout_6 = new QGridLayout(tabMultimedia);
        gridLayout_6->setSpacing(0);
        gridLayout_6->setContentsMargins(0, 0, 0, 0);
        gridLayout_6->setObjectName(QString::fromUtf8("gridLayout_6"));
        groupBox_4 = new QGroupBox(tabMultimedia);
        groupBox_4->setObjectName(QString::fromUtf8("groupBox_4"));
        QSizePolicy sizePolicy4(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(groupBox_4->sizePolicy().hasHeightForWidth());
        groupBox_4->setSizePolicy(sizePolicy4);
        groupBox_4->setFont(font);
        gridLayout_7 = new QGridLayout(groupBox_4);
        gridLayout_7->setObjectName(QString::fromUtf8("gridLayout_7"));
        label_7 = new QLabel(groupBox_4);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setFont(font1);
        label_7->setAlignment(Qt::AlignCenter);

        gridLayout_7->addWidget(label_7, 0, 0, 1, 1);

        comboBox = new QComboBox(groupBox_4);
        comboBox->setObjectName(QString::fromUtf8("comboBox"));
        comboBox->setFont(font1);

        gridLayout_7->addWidget(comboBox, 0, 1, 1, 4);

        label_8 = new QLabel(groupBox_4);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        label_8->setFont(font1);
        label_8->setAlignment(Qt::AlignCenter);

        gridLayout_7->addWidget(label_8, 1, 0, 1, 1);

        comboBox_2 = new QComboBox(groupBox_4);
        comboBox_2->setObjectName(QString::fromUtf8("comboBox_2"));
        comboBox_2->setFont(font1);

        gridLayout_7->addWidget(comboBox_2, 1, 1, 1, 4);

        label_9 = new QLabel(groupBox_4);
        label_9->setObjectName(QString::fromUtf8("label_9"));
        label_9->setFont(font1);
        label_9->setAlignment(Qt::AlignCenter);

        gridLayout_7->addWidget(label_9, 2, 0, 1, 1);

        comboBox_3 = new QComboBox(groupBox_4);
        comboBox_3->setObjectName(QString::fromUtf8("comboBox_3"));
        comboBox_3->setFont(font1);

        gridLayout_7->addWidget(comboBox_3, 2, 1, 1, 4);

        label_10 = new QLabel(groupBox_4);
        label_10->setObjectName(QString::fromUtf8("label_10"));
        label_10->setFont(font1);
        label_10->setAlignment(Qt::AlignCenter);

        gridLayout_7->addWidget(label_10, 3, 0, 1, 1);

        lineEdit_3 = new QLineEdit(groupBox_4);
        lineEdit_3->setObjectName(QString::fromUtf8("lineEdit_3"));
        lineEdit_3->setFont(font1);

        gridLayout_7->addWidget(lineEdit_3, 3, 1, 1, 1);

        checkBox_4 = new QCheckBox(groupBox_4);
        checkBox_4->setObjectName(QString::fromUtf8("checkBox_4"));
        QSizePolicy sizePolicy5(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy5.setHorizontalStretch(0);
        sizePolicy5.setVerticalStretch(0);
        sizePolicy5.setHeightForWidth(checkBox_4->sizePolicy().hasHeightForWidth());
        checkBox_4->setSizePolicy(sizePolicy5);
        checkBox_4->setFont(font1);
        checkBox_4->setCheckable(false);
        checkBox_4->setTristate(false);

        gridLayout_7->addWidget(checkBox_4, 4, 1, 1, 1);

        toolButton = new QToolButton(groupBox_4);
        toolButton->setObjectName(QString::fromUtf8("toolButton"));
        toolButton->setFont(font1);

        gridLayout_7->addWidget(toolButton, 3, 3, 1, 1);

        pushButton = new QPushButton(groupBox_4);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setFont(font1);

        gridLayout_7->addWidget(pushButton, 3, 2, 1, 1);


        gridLayout_6->addWidget(groupBox_4, 0, 0, 1, 1);

        groupBox_5 = new QGroupBox(tabMultimedia);
        groupBox_5->setObjectName(QString::fromUtf8("groupBox_5"));
        sizePolicy4.setHeightForWidth(groupBox_5->sizePolicy().hasHeightForWidth());
        groupBox_5->setSizePolicy(sizePolicy4);
        groupBox_5->setFont(font);
        gridLayout_8 = new QGridLayout(groupBox_5);
        gridLayout_8->setSpacing(0);
        gridLayout_8->setContentsMargins(0, 0, 0, 0);
        gridLayout_8->setObjectName(QString::fromUtf8("gridLayout_8"));
        label_11 = new QLabel(groupBox_5);
        label_11->setObjectName(QString::fromUtf8("label_11"));
        label_11->setFont(font1);
        label_11->setAlignment(Qt::AlignCenter);

        gridLayout_8->addWidget(label_11, 0, 0, 1, 1);

        comboBox_4 = new QComboBox(groupBox_5);
        comboBox_4->setObjectName(QString::fromUtf8("comboBox_4"));
        comboBox_4->setFont(font1);

        gridLayout_8->addWidget(comboBox_4, 0, 1, 1, 1);

        label_12 = new QLabel(groupBox_5);
        label_12->setObjectName(QString::fromUtf8("label_12"));
        label_12->setFont(font1);
        label_12->setAlignment(Qt::AlignCenter);

        gridLayout_8->addWidget(label_12, 1, 0, 1, 1);

        comboBox_5 = new QComboBox(groupBox_5);
        comboBox_5->setObjectName(QString::fromUtf8("comboBox_5"));
        comboBox_5->setFont(font1);

        gridLayout_8->addWidget(comboBox_5, 1, 1, 1, 1);


        gridLayout_6->addWidget(groupBox_5, 1, 0, 1, 1);

        tabPreference->addTab(tabMultimedia, QString());
        tabSIP = new QWidget();
        tabSIP->setObjectName(QString::fromUtf8("tabSIP"));
        gridLayout_12 = new QGridLayout(tabSIP);
        gridLayout_12->setSpacing(0);
        gridLayout_12->setContentsMargins(0, 0, 0, 0);
        gridLayout_12->setObjectName(QString::fromUtf8("gridLayout_12"));
        groupBox_6 = new QGroupBox(tabSIP);
        groupBox_6->setObjectName(QString::fromUtf8("groupBox_6"));
        QSizePolicy sizePolicy6(QSizePolicy::Preferred, QSizePolicy::Maximum);
        sizePolicy6.setHorizontalStretch(0);
        sizePolicy6.setVerticalStretch(0);
        sizePolicy6.setHeightForWidth(groupBox_6->sizePolicy().hasHeightForWidth());
        groupBox_6->setSizePolicy(sizePolicy6);
        groupBox_6->setFont(font);
        groupBox_6->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        gridLayout_9 = new QGridLayout(groupBox_6);
        gridLayout_9->setSpacing(3);
        gridLayout_9->setContentsMargins(3, 3, 3, 3);
        gridLayout_9->setObjectName(QString::fromUtf8("gridLayout_9"));
        gridLayout_9->setSizeConstraint(QLayout::SetDefaultConstraint);
        label_13 = new QLabel(groupBox_6);
        label_13->setObjectName(QString::fromUtf8("label_13"));
        label_13->setFont(font1);
        label_13->setAlignment(Qt::AlignCenter);

        gridLayout_9->addWidget(label_13, 0, 0, 1, 1);

        lineEdit_4 = new QLineEdit(groupBox_6);
        lineEdit_4->setObjectName(QString::fromUtf8("lineEdit_4"));

        gridLayout_9->addWidget(lineEdit_4, 0, 1, 1, 1);

        label_14 = new QLabel(groupBox_6);
        label_14->setObjectName(QString::fromUtf8("label_14"));
        label_14->setFont(font1);
        label_14->setAlignment(Qt::AlignCenter);

        gridLayout_9->addWidget(label_14, 1, 0, 1, 1);

        lineEdit_5 = new QLineEdit(groupBox_6);
        lineEdit_5->setObjectName(QString::fromUtf8("lineEdit_5"));

        gridLayout_9->addWidget(lineEdit_5, 1, 1, 1, 1);

        label_15 = new QLabel(groupBox_6);
        label_15->setObjectName(QString::fromUtf8("label_15"));
        label_15->setFont(font1);
        label_15->setAlignment(Qt::AlignCenter);

        gridLayout_9->addWidget(label_15, 2, 0, 1, 1);

        lineEdit_6 = new QLineEdit(groupBox_6);
        lineEdit_6->setObjectName(QString::fromUtf8("lineEdit_6"));

        gridLayout_9->addWidget(lineEdit_6, 2, 1, 1, 1);


        gridLayout_12->addWidget(groupBox_6, 0, 0, 1, 1);

        groupBox_7 = new QGroupBox(tabSIP);
        groupBox_7->setObjectName(QString::fromUtf8("groupBox_7"));
        groupBox_7->setFont(font);
        gridLayout_10 = new QGridLayout(groupBox_7);
        gridLayout_10->setSpacing(3);
        gridLayout_10->setContentsMargins(3, 3, 3, 3);
        gridLayout_10->setObjectName(QString::fromUtf8("gridLayout_10"));
        listView = new QListView(groupBox_7);
        listView->setObjectName(QString::fromUtf8("listView"));
        QSizePolicy sizePolicy7(QSizePolicy::Expanding, QSizePolicy::Minimum);
        sizePolicy7.setHorizontalStretch(0);
        sizePolicy7.setVerticalStretch(0);
        sizePolicy7.setHeightForWidth(listView->sizePolicy().hasHeightForWidth());
        listView->setSizePolicy(sizePolicy7);
        listView->setFont(font1);

        gridLayout_10->addWidget(listView, 0, 0, 3, 1);

        pushButton_3 = new QPushButton(groupBox_7);
        pushButton_3->setObjectName(QString::fromUtf8("pushButton_3"));
        pushButton_3->setFont(font1);

        gridLayout_10->addWidget(pushButton_3, 0, 1, 1, 1);

        pushButton_4 = new QPushButton(groupBox_7);
        pushButton_4->setObjectName(QString::fromUtf8("pushButton_4"));
        pushButton_4->setFont(font1);

        gridLayout_10->addWidget(pushButton_4, 1, 1, 1, 1);

        pushButton_5 = new QPushButton(groupBox_7);
        pushButton_5->setObjectName(QString::fromUtf8("pushButton_5"));
        pushButton_5->setFont(font1);

        gridLayout_10->addWidget(pushButton_5, 2, 1, 1, 1);


        gridLayout_12->addWidget(groupBox_7, 1, 0, 1, 1);

        groupBox_8 = new QGroupBox(tabSIP);
        groupBox_8->setObjectName(QString::fromUtf8("groupBox_8"));
        sizePolicy6.setHeightForWidth(groupBox_8->sizePolicy().hasHeightForWidth());
        groupBox_8->setSizePolicy(sizePolicy6);
        groupBox_8->setFont(font);
        gridLayout_11 = new QGridLayout(groupBox_8);
        gridLayout_11->setSpacing(3);
        gridLayout_11->setContentsMargins(3, 3, 3, 3);
        gridLayout_11->setObjectName(QString::fromUtf8("gridLayout_11"));
        pushButton_2 = new QPushButton(groupBox_8);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));

        gridLayout_11->addWidget(pushButton_2, 0, 0, 1, 1);


        gridLayout_12->addWidget(groupBox_8, 2, 0, 1, 1);

        tabPreference->addTab(tabSIP, QString());
        tabCodecs = new QWidget();
        tabCodecs->setObjectName(QString::fromUtf8("tabCodecs"));
        gridLayout_13 = new QGridLayout(tabCodecs);
        gridLayout_13->setSpacing(0);
        gridLayout_13->setContentsMargins(0, 0, 0, 0);
        gridLayout_13->setObjectName(QString::fromUtf8("gridLayout_13"));
        groupBox_10 = new QGroupBox(tabCodecs);
        groupBox_10->setObjectName(QString::fromUtf8("groupBox_10"));
        sizePolicy6.setHeightForWidth(groupBox_10->sizePolicy().hasHeightForWidth());
        groupBox_10->setSizePolicy(sizePolicy6);
        groupBox_10->setFont(font);
        gridLayout_15 = new QGridLayout(groupBox_10);
#ifndef Q_OS_MAC
        gridLayout_15->setSpacing(6);
#endif
        gridLayout_15->setContentsMargins(3, 3, 3, 3);
        gridLayout_15->setObjectName(QString::fromUtf8("gridLayout_15"));
        label_16 = new QLabel(groupBox_10);
        label_16->setObjectName(QString::fromUtf8("label_16"));
        label_16->setFont(font1);
        label_16->setAlignment(Qt::AlignCenter);

        gridLayout_15->addWidget(label_16, 0, 0, 1, 1);

        spinBox_6 = new QSpinBox(groupBox_10);
        spinBox_6->setObjectName(QString::fromUtf8("spinBox_6"));
        spinBox_6->setFont(font1);

        gridLayout_15->addWidget(spinBox_6, 0, 1, 1, 1);

        label_17 = new QLabel(groupBox_10);
        label_17->setObjectName(QString::fromUtf8("label_17"));
        label_17->setFont(font1);
        label_17->setAlignment(Qt::AlignCenter);

        gridLayout_15->addWidget(label_17, 1, 0, 1, 1);

        spinBox_8 = new QSpinBox(groupBox_10);
        spinBox_8->setObjectName(QString::fromUtf8("spinBox_8"));
        spinBox_8->setFont(font1);

        gridLayout_15->addWidget(spinBox_8, 1, 1, 1, 1);


        gridLayout_13->addWidget(groupBox_10, 1, 0, 1, 1);

        groupBox_9 = new QGroupBox(tabCodecs);
        groupBox_9->setObjectName(QString::fromUtf8("groupBox_9"));
        groupBox_9->setFont(font);
        gridLayout_14 = new QGridLayout(groupBox_9);
        gridLayout_14->setSpacing(3);
        gridLayout_14->setContentsMargins(3, 3, 3, 3);
        gridLayout_14->setObjectName(QString::fromUtf8("gridLayout_14"));
        comboBox_6 = new QComboBox(groupBox_9);
        comboBox_6->setObjectName(QString::fromUtf8("comboBox_6"));
        QSizePolicy sizePolicy8(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy8.setHorizontalStretch(0);
        sizePolicy8.setVerticalStretch(0);
        sizePolicy8.setHeightForWidth(comboBox_6->sizePolicy().hasHeightForWidth());
        comboBox_6->setSizePolicy(sizePolicy8);
        comboBox_6->setFont(font1);

        gridLayout_14->addWidget(comboBox_6, 0, 0, 1, 2);

        tableWidget = new QTableWidget(groupBox_9);
        tableWidget->setObjectName(QString::fromUtf8("tableWidget"));
        QSizePolicy sizePolicy9(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
        sizePolicy9.setHorizontalStretch(0);
        sizePolicy9.setVerticalStretch(0);
        sizePolicy9.setHeightForWidth(tableWidget->sizePolicy().hasHeightForWidth());
        tableWidget->setSizePolicy(sizePolicy9);

        gridLayout_14->addWidget(tableWidget, 1, 0, 4, 1);

        pushButton_9 = new QPushButton(groupBox_9);
        pushButton_9->setObjectName(QString::fromUtf8("pushButton_9"));
        sizePolicy3.setHeightForWidth(pushButton_9->sizePolicy().hasHeightForWidth());
        pushButton_9->setSizePolicy(sizePolicy3);
        pushButton_9->setFont(font1);

        gridLayout_14->addWidget(pushButton_9, 4, 1, 1, 1);

        pushButton_8 = new QPushButton(groupBox_9);
        pushButton_8->setObjectName(QString::fromUtf8("pushButton_8"));
        sizePolicy3.setHeightForWidth(pushButton_8->sizePolicy().hasHeightForWidth());
        pushButton_8->setSizePolicy(sizePolicy3);
        pushButton_8->setFont(font1);

        gridLayout_14->addWidget(pushButton_8, 3, 1, 1, 1);

        pushButton_7 = new QPushButton(groupBox_9);
        pushButton_7->setObjectName(QString::fromUtf8("pushButton_7"));
        sizePolicy3.setHeightForWidth(pushButton_7->sizePolicy().hasHeightForWidth());
        pushButton_7->setSizePolicy(sizePolicy3);
        pushButton_7->setFont(font1);

        gridLayout_14->addWidget(pushButton_7, 2, 1, 1, 1);

        pushButton_6 = new QPushButton(groupBox_9);
        pushButton_6->setObjectName(QString::fromUtf8("pushButton_6"));
        sizePolicy3.setHeightForWidth(pushButton_6->sizePolicy().hasHeightForWidth());
        pushButton_6->setSizePolicy(sizePolicy3);
        pushButton_6->setFont(font1);

        gridLayout_14->addWidget(pushButton_6, 1, 1, 1, 1);


        gridLayout_13->addWidget(groupBox_9, 0, 0, 1, 1);

        tabPreference->addTab(tabCodecs, QString());
        tabInterface = new QWidget();
        tabInterface->setObjectName(QString::fromUtf8("tabInterface"));
        formLayout = new QFormLayout(tabInterface);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        formLayout->setSizeConstraint(QLayout::SetMaximumSize);
        formLayout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
        formLayout->setLabelAlignment(Qt::AlignJustify|Qt::AlignVCenter);
        formLayout->setFormAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        groupBox_11 = new QGroupBox(tabInterface);
        groupBox_11->setObjectName(QString::fromUtf8("groupBox_11"));
        QSizePolicy sizePolicy10(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy10.setHorizontalStretch(0);
        sizePolicy10.setVerticalStretch(0);
        sizePolicy10.setHeightForWidth(groupBox_11->sizePolicy().hasHeightForWidth());
        groupBox_11->setSizePolicy(sizePolicy10);
        groupBox_11->setFont(font);
        groupBox_11->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        gridLayout_16 = new QGridLayout(groupBox_11);
        gridLayout_16->setObjectName(QString::fromUtf8("gridLayout_16"));
        comboBox_7 = new QComboBox(groupBox_11);
        comboBox_7->setObjectName(QString::fromUtf8("comboBox_7"));

        gridLayout_16->addWidget(comboBox_7, 0, 0, 1, 1);


        formLayout->setWidget(0, QFormLayout::SpanningRole, groupBox_11);

        groupBox_12 = new QGroupBox(tabInterface);
        groupBox_12->setObjectName(QString::fromUtf8("groupBox_12"));
        sizePolicy10.setHeightForWidth(groupBox_12->sizePolicy().hasHeightForWidth());
        groupBox_12->setSizePolicy(sizePolicy10);
        groupBox_12->setFont(font);
        groupBox_12->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        gridLayout_17 = new QGridLayout(groupBox_12);
        gridLayout_17->setObjectName(QString::fromUtf8("gridLayout_17"));
        checkBox_5 = new QCheckBox(groupBox_12);
        checkBox_5->setObjectName(QString::fromUtf8("checkBox_5"));

        gridLayout_17->addWidget(checkBox_5, 0, 0, 1, 1);


        formLayout->setWidget(1, QFormLayout::SpanningRole, groupBox_12);

        tabPreference->addTab(tabInterface, QString());

        gridLayout->addWidget(tabPreference, 2, 0, 1, 1);


        retranslateUi(Preference);
        QObject::connect(buttonBox, SIGNAL(accepted()), Preference, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), Preference, SLOT(reject()));

        tabPreference->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(Preference);
    } // setupUi

    void retranslateUi(QDialog *Preference)
    {
        Preference->setWindowTitle(QApplication::translate("Preference", "Dialog", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("Preference", "Ports", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("Preference", "SIP(UDP):", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("Preference", "SIP(TCP):", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("Preference", "Audio RTP/UDP:", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("Preference", "Video RTP/UDP:", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("Preference", "Transport", 0, QApplication::UnicodeUTF8));
        checkBox->setText(QApplication::translate("Preference", "Set Maximum Tranmission Unit:", 0, QApplication::UnicodeUTF8));
        checkBox_2->setText(QApplication::translate("Preference", "Send DTMFs as SIP Info", 0, QApplication::UnicodeUTF8));
        checkBox_3->setText(QApplication::translate("Preference", "Use IPv6 instead of IPv4", 0, QApplication::UnicodeUTF8));
        groupBox_3->setTitle(QApplication::translate("Preference", "Nat and Firewall", 0, QApplication::UnicodeUTF8));
        radioButton->setText(QApplication::translate("Preference", "Direct connection to the Internet", 0, QApplication::UnicodeUTF8));
        radioButton_2->setText(QApplication::translate("Preference", "Behind NAT/Firewall(specify gateway IP below)", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("Preference", "Public IP address:", 0, QApplication::UnicodeUTF8));
        radioButton_3->setText(QApplication::translate("Preference", "Behind NAT/Firewall(use STUN to resolve)", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("Preference", "STUN server:", 0, QApplication::UnicodeUTF8));
        tabPreference->setTabText(tabPreference->indexOf(tabNetWork), QApplication::translate("Preference", "Network settings", 0, QApplication::UnicodeUTF8));
        groupBox_4->setTitle(QApplication::translate("Preference", "Audio", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("Preference", "Playback device:", 0, QApplication::UnicodeUTF8));
        label_8->setText(QApplication::translate("Preference", "Ring device:", 0, QApplication::UnicodeUTF8));
        label_9->setText(QApplication::translate("Preference", "Capture device:", 0, QApplication::UnicodeUTF8));
        label_10->setText(QApplication::translate("Preference", "Ring sound:", 0, QApplication::UnicodeUTF8));
        checkBox_4->setText(QApplication::translate("Preference", "Enable echo cancellation", 0, QApplication::UnicodeUTF8));
        toolButton->setText(QApplication::translate("Preference", "Play", 0, QApplication::UnicodeUTF8));
        pushButton->setText(QApplication::translate("Preference", "Choose File", 0, QApplication::UnicodeUTF8));
        groupBox_5->setTitle(QApplication::translate("Preference", "Video", 0, QApplication::UnicodeUTF8));
        label_11->setText(QApplication::translate("Preference", "Video input device:", 0, QApplication::UnicodeUTF8));
        label_12->setText(QApplication::translate("Preference", "Prefered video resolution:", 0, QApplication::UnicodeUTF8));
        tabPreference->setTabText(tabPreference->indexOf(tabMultimedia), QApplication::translate("Preference", "Multimedia settings", 0, QApplication::UnicodeUTF8));
        groupBox_6->setTitle(QApplication::translate("Preference", "Default identity", 0, QApplication::UnicodeUTF8));
        label_13->setText(QApplication::translate("Preference", "Your display name (eg: John Doe):", 0, QApplication::UnicodeUTF8));
        label_14->setText(QApplication::translate("Preference", "Your username:", 0, QApplication::UnicodeUTF8));
        label_15->setText(QApplication::translate("Preference", "Your resulting SIP address:", 0, QApplication::UnicodeUTF8));
        groupBox_7->setTitle(QApplication::translate("Preference", "Proxy accounts", 0, QApplication::UnicodeUTF8));
        pushButton_3->setText(QApplication::translate("Preference", "Add", 0, QApplication::UnicodeUTF8));
        pushButton_4->setText(QApplication::translate("Preference", "Edit", 0, QApplication::UnicodeUTF8));
        pushButton_5->setText(QApplication::translate("Preference", "Remove", 0, QApplication::UnicodeUTF8));
        groupBox_8->setTitle(QApplication::translate("Preference", "Privacy", 0, QApplication::UnicodeUTF8));
        pushButton_2->setText(QApplication::translate("Preference", "PushButton", 0, QApplication::UnicodeUTF8));
        tabPreference->setTabText(tabPreference->indexOf(tabSIP), QApplication::translate("Preference", "Manage SIP Accounts", 0, QApplication::UnicodeUTF8));
        groupBox_10->setTitle(QApplication::translate("Preference", "Bandwidth control", 0, QApplication::UnicodeUTF8));
        label_16->setText(QApplication::translate("Preference", "Upload speed limit in Kbit/sec:", 0, QApplication::UnicodeUTF8));
        label_17->setText(QApplication::translate("Preference", "Download speed limit in Kbit/sec:", 0, QApplication::UnicodeUTF8));
        groupBox_9->setTitle(QApplication::translate("Preference", "Codecs", 0, QApplication::UnicodeUTF8));
        pushButton_9->setText(QApplication::translate("Preference", "Disable", 0, QApplication::UnicodeUTF8));
        pushButton_8->setText(QApplication::translate("Preference", "Enable", 0, QApplication::UnicodeUTF8));
        pushButton_7->setText(QApplication::translate("Preference", "Down", 0, QApplication::UnicodeUTF8));
        pushButton_6->setText(QApplication::translate("Preference", "Up", 0, QApplication::UnicodeUTF8));
        tabPreference->setTabText(tabPreference->indexOf(tabCodecs), QApplication::translate("Preference", "Codecs", 0, QApplication::UnicodeUTF8));
        groupBox_11->setTitle(QApplication::translate("Preference", "Language", 0, QApplication::UnicodeUTF8));
        groupBox_12->setTitle(QApplication::translate("Preference", "Level", 0, QApplication::UnicodeUTF8));
        checkBox_5->setText(QApplication::translate("Preference", "Show advanced settings", 0, QApplication::UnicodeUTF8));
        tabPreference->setTabText(tabPreference->indexOf(tabInterface), QApplication::translate("Preference", "User interface", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class Preference: public Ui_Preference {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PREFERENCE_H
