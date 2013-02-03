#include "contact.h"
#include "ui_contact.h"
#include <QMessageBox>
#include "mainwindow.h"

Contact::Contact(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Contact)
{
    ui->setupUi(this);
	_lf = NULL;
	ui->lineEdit_uri->setText("sip:");
}

Contact::~Contact()
{
    delete ui;
}
void Contact::set_uri(QString &uri)
{
	ui->lineEdit_uri->setText(uri);
}

void Contact::set_name(QString &name)
{
	ui->lineEdit_name->setText(name);
}

void Contact::set_allow_presence(bool value)
{
	ui->checkBox_allow->setChecked(value);
}
void Contact::set_show_status(bool value)
{
	ui->checkBox_showstatus->setChecked(value);
}
void Contact::set_friend(LinphoneFriend *lf)
{
	char *uri;
	const char *name;
	
	const LinphoneAddress *f_uri= NULL;
	if (!lf) return;
	_lf = lf;
	f_uri = linphone_friend_get_address(lf);
	uri=linphone_address_as_string_uri_only(f_uri);
	name=linphone_address_get_display_name(f_uri);
	QString a;
	if (uri) {
		a = uri;
		ui->lineEdit_uri->setText(a);
		ms_free(uri);
	}
	if (name){
		a = name;
		ui->lineEdit_name->setText(a);
	}
	if(linphone_friend_get_send_subscribe(lf))
	{
		ui->checkBox_showstatus->setChecked(true);
	}else{
		ui->checkBox_showstatus->setChecked(false);
	}
	if (linphone_friend_get_inc_subscribe_policy(lf)==LinphoneSPAccept)
	{
		ui->checkBox_allow->setChecked(true);
	}else{
		ui->checkBox_allow->setChecked(false);
	}
}

LinphoneFriend *Contact::get_friend(void)
{
	return _lf;
}

void Contact::on_buttonBox_accepted()
{
	LinphoneFriend *lf = _lf;
	char *fixed_uri=NULL;
	gboolean show_presence=FALSE,allow_presence=FALSE;
	QString name, uri;
	LinphoneAddress* friend_address;
	if (lf==NULL){
		lf=linphone_friend_new();
		if (linphone_qt_get_ui_config_int("use_subscribe_notify",1)==1){
			show_presence=FALSE;
			allow_presence=FALSE;
		}
		linphone_friend_set_inc_subscribe_policy(lf,allow_presence ? LinphoneSPAccept : LinphoneSPDeny);
		linphone_friend_send_subscribe(lf,show_presence);
	}
	name = ui->lineEdit_name->text();
	uri = ui->lineEdit_uri->text();
	if (ui->checkBox_showstatus->isChecked())
	{
		show_presence = TRUE;
	}else{ 
		show_presence = FALSE;
	}
	if (ui->checkBox_allow->isChecked())
	{
		allow_presence = TRUE;
	}else{ 
		allow_presence = FALSE;
	}

	linphone_core_interpret_friend_uri(linphone_qt_get_core(),uri.toAscii(),&fixed_uri);
	if (fixed_uri==NULL){
		QMessageBox::warning(this, "Warning", "Invalid sip contact !");
		return ;
	}
	friend_address = linphone_address_new(fixed_uri);
	linphone_address_set_display_name(friend_address,name.toAscii());
	linphone_friend_set_addr(lf,friend_address);
	ms_free(fixed_uri);
	linphone_address_destroy(friend_address);

	linphone_friend_send_subscribe(lf,show_presence);
	linphone_friend_set_inc_subscribe_policy(lf,allow_presence==TRUE ? LinphoneSPAccept : LinphoneSPDeny);
	if (linphone_friend_in_list(lf)) {
		linphone_friend_done(lf);
	}else{
		linphone_core_add_friend(linphone_qt_get_core(),lf);
	}
	w->show_friends();
}

void Contact::on_buttonBox_rejected()
{
	_lf = NULL;
}
