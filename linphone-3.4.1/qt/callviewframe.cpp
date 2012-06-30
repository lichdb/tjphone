#include "callviewframe.h"
#include "ui_callviewframe.h"
#include "mainwindow.h"

CallViewFrame::CallViewFrame(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::CallViewFrame)
{
    ui->setupUi(this);
	_call = NULL;
	_call_index = 0;
	_transfer_button = NULL;
	muted = 0;
	holdon = 0;
}

CallViewFrame::~CallViewFrame()
{
	if (_transfer_button) delete _transfer_button;
    delete ui;
}
void CallViewFrame::enable_hold_button(bool sensitive, bool pause)
{
	ui->pushButton_pause->setEnabled(sensitive);
	if(!pause)
		ui->pushButton_pause->setText("Resume");
	else
		ui->pushButton_pause->setText("Pause");
}
void CallViewFrame::enable_mute_button(bool sensitive)
{
	ui->pushButton_mute->setEnabled(sensitive);
}

void CallViewFrame::set_duration(QString &duration)
{
	ui->label_duration->setText(duration);
}
void CallViewFrame::set_peer_name(QString &peer_name)
{
	ui->label_callee->setTextFormat(Qt::RichText);
	ui->label_callee->setText(peer_name);
}
void CallViewFrame::set_callstatus(QString &status)
{
	ui->label_codec->setTextFormat(Qt::RichText);
	ui->label_codec->setText(status);
}
QPushButton *CallViewFrame::get_transfer_button()
{
	return _transfer_button; 
}
void CallViewFrame::set_transfer_button(QPushButton *transfer_button)
{
	_transfer_button = transfer_button;
}
void CallViewFrame::transfer_button_clicked()
{
	w->transfer_call(_call);
}
void CallViewFrame::on_pushButton_answer_clicked()
{
	if(!_call) return;
	linphone_core_accept_call(linphone_qt_get_core(),_call);
	show_mute_hold(true);
	enable_mute_button(true);
	show_answer_decline(false);
}

void CallViewFrame::on_pushButton_decline_clicked()
{
	if (_call)
		linphone_core_terminate_call(linphone_qt_get_core(),_call);
}

void CallViewFrame::on_pushButton_mute_clicked()
{
	linphone_core_mute_mic(linphone_qt_get_core(),!muted);
	muted = !muted;
	if(muted){
		ui->pushButton_mute->setText("Unmute");
	}else{
		ui->pushButton_mute->setText("Mute");
	}
	
}

void CallViewFrame::on_pushButton_pause_clicked()
{
	if(!holdon)
	{
		linphone_core_pause_call(linphone_qt_get_core(),_call);
		ui->pushButton_pause->setText("Resume");
	}else{
		linphone_core_resume_call(linphone_qt_get_core(),_call);
		ui->pushButton_pause->setText("Pause");
	}
	holdon = !holdon;
}

void CallViewFrame::show_mute_hold(bool showbutton)
{
	if(showbutton)
	{
		ui->pushButton_mute->show();
		ui->pushButton_pause->show();
	}else{
		ui->pushButton_mute->hide();
		ui->pushButton_pause->hide();
	}
}

void CallViewFrame::show_answer_decline(bool showbutton)
{
	if(showbutton){
		ui->pushButton_answer->show();
		ui->pushButton_decline->show();
	} else {
		ui->pushButton_answer->hide();
		ui->pushButton_decline->hide();
	}
}