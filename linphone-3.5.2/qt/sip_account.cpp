#include "sip_account.h"
#include "ui_sip_account.h"
#include <QTextCodec>
#include <QString>
#include "mainwindow.h"

SIP_Account::SIP_Account(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SIP_Account)
{
    ui->setupUi(this);
	_cfg = NULL;
	ui->spinBox_Regtime->setValue(60);
	ui->spinBox_Regtime->setValue(60);
	ui->lineEdit_account->setText("sip:");
	ui->lineEdit_proxy->setText("sip:");
}

SIP_Account::~SIP_Account()
{
    delete ui;
}

void SIP_Account::set_proxy(LinphoneProxyConfig *cfg)
{
	const char *tmp;
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("utf8"));
	QTextCodec *codec = QTextCodec::codecForCStrings();
	QString a;
	_cfg = cfg;
	if (cfg){
		linphone_proxy_config_edit(cfg);
		tmp = linphone_proxy_config_get_identity(cfg);
		a = codec->toUnicode(tmp);
		ui->lineEdit_account->setText(a);
		tmp = linphone_proxy_config_get_addr(cfg);
		a = codec->toUnicode(tmp);
		ui->lineEdit_proxy->setText(a);
		tmp=linphone_proxy_config_get_route(cfg);
		if (tmp) {
			a = codec->toUnicode(tmp);
			ui->lineEdit_route->setText(a);
		}
		ui->checkBox_register->setChecked(linphone_proxy_config_register_enabled(cfg));
		ui->spinBox_Regtime->setValue(linphone_proxy_config_get_expires(cfg));
		ui->checkBox_publish->setChecked(linphone_proxy_config_publish_enabled(cfg));
	}
}
void SIP_Account::clear_proxy()
{
	ui->checkBox_register->setChecked(false);
	ui->checkBox_publish->setChecked(false);
	ui->spinBox_Regtime->setValue(60);
	ui->lineEdit_account->setText("sip:");
	ui->lineEdit_passwd->clear();
	ui->lineEdit_proxy->setText("sip:");
	ui->lineEdit_route->clear();
	_cfg = NULL;
}
void SIP_Account::on_buttonBox_accepted()
{
	QString a;
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("utf8"));
	QTextCodec *codec = QTextCodec::codecForCStrings();
	LinphoneProxyConfig *cfg = _cfg;
	bool was_editing = true;
	if (!cfg){
		was_editing = false;
		cfg=linphone_proxy_config_new();
	}
	a = ui->lineEdit_account->text();
	linphone_proxy_config_set_identity(cfg, a.toUtf8().data());

	a = ui->lineEdit_proxy->text();
	linphone_proxy_config_set_server_addr(cfg, a.toUtf8().data());

	a = ui->lineEdit_route->text();
	linphone_proxy_config_set_route(cfg, a.toUtf8().data());

	linphone_proxy_config_expires(cfg, ui->spinBox_Regtime->value());
	bool publish = ui->checkBox_publish->isChecked();
	bool_t published = FALSE;
	if (publish) published = TRUE;
	linphone_proxy_config_enable_publish(cfg, published);

	bool reg = ui->checkBox_register->isChecked();
	bool_t reged = FALSE;
	if (reg) reged = TRUE;
	linphone_proxy_config_enable_register(cfg, reged);

	if (was_editing){
		if (linphone_proxy_config_done(cfg)==-1)
			return;
	}
	else {
		if (linphone_core_add_proxy_config(linphone_qt_get_core(),cfg)==-1) return;
		linphone_core_set_default_proxy(linphone_qt_get_core(),cfg);
	}
	
	_cfg = cfg;
	preferencewindow->linphone_qt_show_sip_accounts();
	w->qt_load_identities();
	this->clear_proxy();
	this->close();
}

void SIP_Account::on_buttonBox_rejected()
{
	this->clear_proxy();
	this->close();
}
