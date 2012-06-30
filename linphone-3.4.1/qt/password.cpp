#include "password.h"
#include "ui_password.h"

password::password(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::password)
{
    ui->setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose, true);
	_info = NULL;
}

password::~password()
{
	if(_info) linphone_auth_info_destroy(_info);
    delete ui;
}

void password::set_message(QString &msg)
{
	ui->label->setTextFormat(Qt::RichText);
	ui->label_Message->setText(msg);
}

void password::set_userid(QString &username)
{
	ui->lineEdit_userid->setText(username);
}

void password::set_authinfo(LinphoneAuthInfo *info)
{
	_info = info;
}

void password::on_buttonBox_accepted()
{
	
	linphone_auth_info_set_passwd(_info, ui->lineEdit_password->text().toAscii());
	linphone_auth_info_set_userid(_info, ui->lineEdit_userid->text().toAscii());
	linphone_core_add_auth_info(linphone_qt_get_core(),_info);
}

void password::on_buttonBox_rejected()
{
	killTimer(_timerid);
	close();
}

void password::start_timer()
{
	_timerid = startTimer(30000);
}

void password::timerEvent(QTimerEvent *event)
{
	killTimer(_timerid);
	this->close();
}