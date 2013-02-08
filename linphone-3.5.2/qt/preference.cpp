#include "preference.h"
#include "ui_preference.h"
#include "mainwindow.h"
#include <QStandardItem>
#include <QStringListModel>
#include <QStandardItemModel>
#include <QTextCodec>
#include <QString>
#include <QFileDialog>
#include "HTMLDelegate.h"
#include "sip_account.h"

#include "linphone.h"

Q_DECLARE_METATYPE(struct _PayloadType *);


SIP_Account *_sip_Account;

void Preference::linphone_qt_fill_combo_box(QComboBox *combo, const char **devices, const char *selected, DeviceCap cap){
	const char **p=devices;
	int i=0,active=0;

	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("utf8"));
	QTextCodec *codec = QTextCodec::codecForCStrings();
	QString a; 
	
	combo->clear();
	for(;*p!=NULL;++p){
		if ( cap==CAP_IGNORE 
			|| (cap==CAP_CAPTURE && linphone_core_sound_device_can_capture(linphone_qt_get_core(),*p))
			|| (cap==CAP_PLAYBACK && linphone_core_sound_device_can_playback(linphone_qt_get_core(),*p)) ){
				a = codec->toUnicode(*p);
				combo->addItem(a);
			ms_warning("device name:%s\n", *p);
			if (strcmp(selected,*p)==0) active=i;
			i++;
		}
	}
	combo->setCurrentIndex(active);
}

void Preference::linphone_qt_fill_video_sizes(QComboBox *combo)
{
	int i,active=0;
	char vsize_def[256];
	MSVideoSize cur;
	const MSVideoSizeDef *def=linphone_core_get_supported_video_sizes(linphone_qt_get_core());;
	cur=linphone_core_get_preferred_video_size(linphone_qt_get_core());
	/* glade creates a combo box without list model and text renderer,
	unless we fill it with a dummy text.
	This dummy text needs to be removed first*/
	combo->clear();

	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("utf8"));
	QTextCodec *codec = QTextCodec::codecForCStrings();
	QString a; 

	for(i=0;def->name!=NULL;++def,++i){
		snprintf(vsize_def,sizeof(vsize_def),"%s (%ix%i)",def->name,def->vsize.width,def->vsize.height);
		a = codec->toUnicode(vsize_def);
		combo->addItem(a);
		if (cur.width==def->vsize.width && cur.height==def->vsize.height) active=i;
	}
	combo->setCurrentIndex(active);;
}

void Preference::linphone_qt_show_sip_accounts()
{
	
	QStringList accountlist;

	const MSList *elem;
	LinphoneProxyConfig *cfg;
	int account_count = 0;

	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("utf8"));
	QTextCodec *codec = QTextCodec::codecForCStrings();
	QString a;
	//accountlist.append("Account");
	for(elem=linphone_core_get_proxy_config_list(linphone_qt_get_core());elem!=NULL;elem=ms_list_next(elem)){
		cfg=(LinphoneProxyConfig*)elem->data;
		a = codec->toUnicode(linphone_proxy_config_get_identity(cfg));
		accountlist.append(a);
		account_count++;
	}
	ui->listView_proxy_list->setModel(new QStringListModel(accountlist));
	ui->listView_proxy_list->setViewMode(QListView::ListMode);	
}

enum {
	CODEC_NAME,
	CODEC_RATE,
	CODEC_BITRATE,
	CODEC_STATUS,
	CODEC_PARAMS,
	CODEC_PRIVDATA,
	CODEC_COLOR,
	CODEC_INFO,
	CODEC_NCOLUMNS
};

void Preference::linphone_qt_draw_codec_list(int type)
{ /* 0=audio, 1=video*/
	const MSList *list;
	if (type==0) list=linphone_core_get_audio_codecs(linphone_qt_get_core());
	else list=linphone_core_get_video_codecs(linphone_qt_get_core());
	linphone_qt_show_codecs(list);
}

void Preference::linphone_qt_init_codec_list()
{
	QStandardItemModel *model;
	model= new QStandardItemModel();
	ui->tableView_codec->setModel(model);
	ui->tableView_codec->verticalHeader()->hide();
	ui->tableView_codec->setSelectionBehavior(QAbstractItemView::SelectRows);
    //ui->tableView_codec->horizontalHeader()->setStretchLastSection(true);
    ui->tableView_codec->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView_codec->setSelectionMode(QAbstractItemView::SingleSelection);
	model->insertColumns(0,6);
	model->setHeaderData(0,Qt::Horizontal,"Name");
	model->setHeaderData(1,Qt::Horizontal,"Rate (Hz)");
	model->setHeaderData(2,Qt::Horizontal,"Status");
	model->setHeaderData(3,Qt::Horizontal,"Min bitrate (kbit/s)");
	model->setHeaderData(4,Qt::Horizontal,"Parameters");


}
void Preference::linphone_qt_show_codecs(const MSList *codeclist)
{
	QStandardItemModel *model;
	const MSList *elem;
	int row = 0;
	
	model = (QStandardItemModel *)ui->tableView_codec->model();
	model->removeRows(0,model->rowCount());
	for(elem=codeclist; elem!=NULL; elem=elem->next){
		char *status;
		int rate;
		float bitrate; 
		char *color;
		const char *params="";
		struct _PayloadType *pt=(struct _PayloadType *)elem->data;
		
		if (linphone_core_payload_type_enabled(linphone_qt_get_core(),pt)) status="Enabled";
		else status="Disabled";
		if (linphone_core_check_payload_type_usability(linphone_qt_get_core(),pt)) color="blue";
		else color="red";
		bitrate=payload_type_get_bitrate(pt)/1000.0;
		rate=payload_type_get_rate(pt);
		if (pt->recv_fmtp!=NULL) params=pt->recv_fmtp;
		
		QString string_rate;
		string_rate = string_rate.sprintf("%d", rate);
		
		QStandardItem *item_name = new QStandardItem(payload_type_get_mime(pt)); 
		QStandardItem *item_rate = new QStandardItem(string_rate);
		QStandardItem *item_status = new QStandardItem(status);
		string_rate = string_rate.sprintf("%f", bitrate);
		QStandardItem *item_bitrate = new QStandardItem(string_rate);
		QStandardItem *item_params = new QStandardItem(params);
		QVariant pt_data;
		pt_data.setValue(pt);
		model->insertRow(row);
		model->setItem(row, 0, item_name);
		model->setItem(row, 1, item_rate);
		model->setItem(row, 2, item_status);
		model->setItem(row, 3, item_bitrate);
		model->setItem(row, 4, item_params);
		model->setData(model->index(row, 5), pt_data);
		row++;
	}
	ui->tableView_codec->resizeColumnsToContents();
	ui->tableView_codec->setColumnHidden(5,true);
}

typedef struct _LangCodes{
	const char *code;
	const char *name;
}LangCodes;

static  LangCodes supported_langs[]={
	{	"C"	,	_("English")	},
	{	"fr"	,	_("French")	},
	{	"sv"	,	_("Swedish")	},
	{	"it"	,	_("Italian")	},
	{	"es"	,	_("Spanish")	},
	{	"pt_BR"	,	_("Brazilian Portugese")	},
	{	"pl"	,	_("Polish")	},
	{	"de"	,	_("German")	},
	{	"ru"	,	_("Russian")	},
	{	"ja"	,	_("Japanese")	},
	{	"nl"	,	_("Dutch")	},
	{	"hu"	,	_("Hungarian")	},
	{	"cs"	,	_("Czech")	},
	{	"zh_CN" ,	_("Chinese")	},
	{	NULL	,	NULL		}
};

static const char *lang_get_name(const char *code){
	LangCodes *p=supported_langs;
	while(p->code!=NULL){
		if (strcmp(p->code,code)==0) return p->name;
		p++;
	}
	return NULL;
}

static bool lang_equals(const char *l1, const char *l2){
	return ((strncmp(l1,l2,5)==0 || strncmp(l1,l2,2)==0));
}

void Preference::linphone_qt_fill_langs()
{
	char code[10];
	const char *all_langs="C " LINPHONE_ALL_LANGS;
	const char *name;
	int i=0,index=0;
	const char *cur_lang=getenv("LANG");
	int cur_lang_index=-1;
	char text[256]={0};
	if (cur_lang==NULL) cur_lang="C";

	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("utf8"));
	QTextCodec *codec = QTextCodec::codecForCStrings();
	QString a; 

	ui->comboBox_lang->clear();
	while(sscanf(all_langs+i,"%s",code)==1){
		i+=strlen(code);
		while(all_langs[i]==' ') ++i;
		name=lang_get_name(code);
		snprintf(text,sizeof(text)-1,"%s : %s",code,name!=NULL ? (name) : code);
		a = codec->toUnicode(text);
		ui->comboBox_lang->addItem(a);
		if (cur_lang_index==-1 && lang_equals(cur_lang,code)) 
			cur_lang_index=index;
		index++;
	}
	ui->comboBox_lang->setCurrentIndex(cur_lang_index);
}

 void Preference::parse_item(const char *item, const char *window_name, QWidget *w,  bool show)
 {
	QString a = item;
	//strcpy(tmp,item);
	//dot=strchr(tmp,'.');
	if (item){
		QWidget *wd=w->findChild<QWidget *>(a);
		if (wd) {
			if (!show) wd->hide();
			else wd->show();
		}
	}
}

void Preference::linphone_qt_visibility_set(const char *hiddens, const char *window_name, QWidget *w, bool show)
{
	if(show){
		ui->tabCodecs->show();
		QString a="Codecs";
		ui->tabPreference->insertTab(ui->tabPreference->indexOf(ui->tabInterface), ui->tabCodecs, a);
		ui->groupBox_transport->show();
		ui->groupBox_ports->show();
	}else{
		
		ui->tabPreference->removeTab(ui->tabPreference->indexOf(ui->tabCodecs));
		ui->tabCodecs->hide();
		ui->groupBox_transport->hide();
		ui->groupBox_ports->hide();
	}

}

void Preference::on_checkBox_ui_level_changed(int checkstate)
{	
	if(checkstate){
		linphone_qt_set_ui_config_int("advanced_ui", 1);
	} else {
		linphone_qt_set_ui_config_int("advanced_ui", 0);	
	}
	linphone_qt_ui_level_adapt();
}

void Preference::linphone_qt_ui_level_adapt() {
	bool ui_advanced;
	const char *simple_ui = linphone_qt_get_ui_config("simple_ui", "tabCodecs groupBox_transport groupBox_ports");

	ui_advanced = linphone_qt_get_ui_config_int("advanced_ui", TRUE);
	if (ui_advanced) {
		linphone_qt_visibility_set(simple_ui, "parameters", this, TRUE);
	} else {
		linphone_qt_visibility_set(simple_ui, "parameters", this, FALSE);
	}
}

Preference::Preference(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Preference)
{
	LinphoneCore *lc;
	const char **sound_devices;
	const char *tmp;
	LinphoneAddress *contact;
	LinphoneFirewallPolicy pol;
	int mtu;
	int ui_advanced;
	LCSipTransports tr;

	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("utf8"));
	QTextCodec *codec = QTextCodec::codecForCStrings();
	QString a;

	lc=linphone_qt_get_core();
	sound_devices=linphone_core_get_sound_devices(lc);

    ui->setupUi(this);
	/* NETWORK CONFIG */
	ui->checkBox_ipv6->setChecked(linphone_core_ipv6_enabled(lc));

	linphone_core_get_sip_transports(lc,&tr);
	if(tr.tcp_port > 0){
		ui->comboBox_proto->setCurrentIndex(1);
		ui->spinBox_sip_port->setValue(tr.tcp_port);
    }
    else if (tr.tls_port > 0) {
		ui->comboBox_proto->setCurrentIndex(2);
		ui->spinBox_sip_port->setValue(tr.tls_port);
    }
    else {
        ui->comboBox_proto->setCurrentIndex(0);
		ui->spinBox_sip_port->setValue(tr.udp_port);
	}
	ui->spinBox_audio_rtp_port->setValue(linphone_core_get_audio_port(lc));
	ui->spinBox_video_rtp_port->setValue(linphone_core_get_video_port(lc));
	ui->radioButton_no_nat->setChecked(false);
	ui->radioButton_use_nat_address->setChecked(false);
	ui->radioButton_use_stun->setChecked(false);
	tmp=linphone_core_get_nat_address(lc);
	if (tmp) {
		ui->lineEdit_nat_address->setText(tmp);
	}
	tmp=linphone_core_get_stun_server(lc);
	if (tmp) {
		ui->lineEdit_stun_server->setText(tmp);
	}
	pol=linphone_core_get_firewall_policy(lc);
	switch(pol){
		case LinphonePolicyNoFirewall:
			ui->radioButton_no_nat->setChecked(true);
		break;
		case LinphonePolicyUseNatAddress:
			ui->radioButton_use_nat_address->setChecked(true);
		break;
		case LinphonePolicyUseStun:
			ui->radioButton_use_stun->setChecked(true);
		break;
	}
	mtu=linphone_core_get_mtu(lc);
	if (mtu<=0){
		ui->spinBox_mtu->setEnabled(false);
		ui->spinBox_mtu->setValue(1500);
		ui->checkBox_mtu_set->setChecked(false);
	}else{
		ui->spinBox_mtu->setEnabled(true);
		ui->spinBox_mtu->setValue(mtu);
		ui->checkBox_mtu_set->setChecked(true);
	}
	
	ui->checkBox_dtmf_sipinfo->setEnabled(linphone_core_get_use_info_for_dtmf(lc));

	/* MUTIMEDIA CONFIG */
	linphone_qt_fill_combo_box(ui->comboBox_playback_device, sound_devices,
					linphone_core_get_playback_device(lc),CAP_PLAYBACK);
	linphone_qt_fill_combo_box(ui->comboBox_ring_device, sound_devices,
					linphone_core_get_ringer_device(lc),CAP_PLAYBACK);
	linphone_qt_fill_combo_box(ui->comboBox_capture_device, sound_devices,
					linphone_core_get_capture_device(lc),CAP_CAPTURE);
	linphone_qt_fill_combo_box(ui->comboBox_webcams, linphone_core_get_video_devices(lc),
					linphone_core_get_video_device(lc),CAP_IGNORE);

	linphone_qt_fill_video_sizes(ui->comboBox_video_size);
	ui->checkBox_echo_cancelation->setChecked(linphone_core_echo_cancellation_enabled(lc)); 
	a = codec->toUnicode(linphone_core_get_ring(lc));
	ui->lineEdit_ring_file->setText(a);

	/* SIP CONFIG */
	contact=linphone_core_get_primary_contact_parsed(lc);
	if (contact){
		if (linphone_address_get_display_name(contact)) {
			const char *dn=linphone_address_get_display_name(contact);
			a=codec->toUnicode(dn);
			ui->lineEdit_displayname->setText(a);
		}
		if (linphone_address_get_username(contact)){
			a=codec->toUnicode(linphone_address_get_username(contact));
			ui->lineEdit_username->setText(a);
		}
	}
	linphone_address_destroy(contact);
	linphone_qt_show_sip_accounts();

	/* CODECS CONFIG */
	ui->comboBox_codec_view->setCurrentIndex(0);
	linphone_qt_init_codec_list();
	linphone_qt_draw_codec_list(0);


	ui->spinBox_download_bw->setValue(linphone_core_get_download_bandwidth(lc));
	ui->spinBox_upload_bw->setValue(linphone_core_get_upload_bandwidth(lc));

	connect(ui->comboBox_codec_view, SIGNAL(currentIndexChanged(int)), this, SLOT(on_comboBox_codec_view_changed(int)));
	connect(ui->checkBox_ui_level ,SIGNAL(stateChanged(int)), this, SLOT(on_checkBox_ui_level_changed(int)));

	connect(ui->comboBox_proto, SIGNAL(currentIndexChanged(int)), this, SLOT(on_comboBox_proto_currentIndexChanged(int)));
	connect(ui->spinBox_sip_port,SIGNAL(valueChanged(int)), this, SLOT(on_spinBox_sip_port_valueChanged(int)));

	/* UI CONFIG */

	linphone_qt_fill_langs();
	ui_advanced = linphone_qt_get_ui_config_int("advanced_ui", 1);
	linphone_qt_set_ui_config_int("advanced_ui", ui_advanced);
	if(ui_advanced)
		ui->checkBox_ui_level->setChecked(true);
	linphone_qt_ui_level_adapt();
	ui->tabPreference->setCurrentIndex(0);
	_sip_Account = NULL;
	_sip_Account = new SIP_Account(this);
	_sip_Account->preferencewindow = this;
	setAttribute(Qt::WA_DeleteOnClose);
}

Preference::~Preference()
{
	//if(_sip_Account) _sip_Account->close();
    delete ui;
	w->preference_closed();
}

void Preference::on_pushButton_add_proxy_clicked()
{
    _sip_Account->show();
}

void Preference::on_buttonBox_Apply_clicked(QAbstractButton *button)
{
    this->close();
}

void Preference::on_comboBox_codec_view_changed(int index)
{
	linphone_qt_draw_codec_list(index);
}
void Preference::on_checkBox_mtu_set_toggled(bool checked)
{
	if (checked){
		ui->spinBox_mtu->setEnabled(true);
		on_spinBox_mtu_valueChanged(ui->spinBox_mtu->value());
	}else{
		ui->spinBox_mtu->setEnabled(false);
		linphone_core_set_mtu(linphone_qt_get_core(),0);
	}
}

void Preference::on_spinBox_mtu_valueChanged(int arg1)
{
    linphone_core_set_mtu(linphone_qt_get_core(),arg1);
}

void Preference::on_comboBox_proto_currentIndexChanged(int index)
{	    
	int port = ui->spinBox_sip_port->value();

	if (port==0) { // We use default port if not specified
        if (/* ui->comboBox_proto->currentText() == "SIP (UCP)"*/ index == 0) {
			ui->spinBox_sip_port->setValue(5060);
        }
        else if (/* ui->comboBox_proto->currentText() == "SIP (TCP)" */ index == 1) {
            ui->spinBox_sip_port->setValue(5060);
        }
        else if (/* ui->comboBox_proto->currentText() == "SIP (TLS)" */ index == 2){
            ui->spinBox_sip_port->setValue(5061);
        }
	}else{
		on_spinBox_sip_port_valueChanged(port);
	}
}

void Preference::on_spinBox_sip_port_valueChanged(int arg1)
{
    LCSipTransports tr;
	LinphoneCore *lc=linphone_qt_get_core();
	linphone_core_get_sip_transports(lc,&tr);
	if(ui->comboBox_proto->currentText() == "SIP (TCP)"){
		tr.tcp_port = arg1;
		tr.udp_port = 0;
		tr.tls_port = 0;
	} else if (ui->comboBox_proto->currentText() == "SIP (UDP)"){
		tr.udp_port = arg1;
		tr.tls_port = 0;
		tr.tcp_port = 0;
	} else {
		tr.udp_port = 0;
		tr.tcp_port = 0;
		tr.tls_port = arg1;	
	}
	linphone_core_set_sip_transports(lc,&tr);
	linphone_core_refresh_registers(lc);
}

void Preference::on_spinBox_audio_rtp_port_valueChanged(int arg1)
{
	linphone_core_set_audio_port(linphone_qt_get_core(), arg1);
}

void Preference::on_spinBox_video_rtp_port_valueChanged(int arg1)
{
	linphone_core_set_video_port(linphone_qt_get_core(), arg1);
}

void Preference::on_radioButton_no_nat_toggled(bool checked)
{
	if(checked)
		linphone_core_set_firewall_policy(linphone_qt_get_core(),LinphonePolicyNoFirewall);
}

void Preference::on_radioButton_use_nat_address_toggled(bool checked)
{
    if(checked)
		linphone_core_set_firewall_policy(linphone_qt_get_core(),LinphonePolicyUseNatAddress);
}

void Preference::on_lineEdit_nat_address_textChanged(const QString &arg1)
{
	linphone_core_set_nat_address(linphone_qt_get_core(),arg1.toUtf8().data());
}

void Preference::on_radioButton_use_stun_toggled(bool checked)
{
	if (checked)
		linphone_core_set_firewall_policy(linphone_qt_get_core(),LinphonePolicyUseStun);;
}

void Preference::on_lineEdit_stun_server_textChanged(const QString &arg1)
{
	linphone_core_set_stun_server(linphone_qt_get_core(),arg1.toUtf8().data());
}

void Preference::on_comboBox_playback_device_currentIndexChanged(const QString &arg1)
{
	linphone_core_set_playback_device(linphone_qt_get_core(),arg1.toUtf8().data());
}

void Preference::on_comboBox_ring_device_currentIndexChanged(const QString &arg1)
{
	linphone_core_set_ringer_device(linphone_qt_get_core(),arg1.toUtf8().data());
}

void Preference::on_comboBox_capture_device_currentIndexChanged(const QString &arg1)
{
	linphone_core_set_capture_device(linphone_qt_get_core(),arg1.toUtf8().data());
}

void Preference::on_pushButton_choose_file_clicked()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Ring File"),
                                                 "./",
                                                 tr("WAV (*.wav)"));
	if(!fileName.isEmpty()){
		linphone_core_set_ring(linphone_qt_get_core(),fileName.toLocal8Bit().constData());
		ui->lineEdit_ring_file->setText(fileName);
	}
}


void linphone_qt_end_of_ring(LinphoneCore *lc, void *user_data){
	QDialogButtonBox *playbutton = (QDialogButtonBox *)user_data;
	playbutton->setEnabled(true);
}
void Preference::on_toolButton_play_ring_clicked()
{
	if (linphone_core_preview_ring(linphone_qt_get_core(),
				linphone_core_get_ring(linphone_qt_get_core()),
				linphone_qt_end_of_ring,
				ui->buttonBox_Apply)==0){
		ui->buttonBox_Apply->setEnabled(false);
	}
}

void Preference::on_checkBox_echo_cancelation_clicked()
{
    linphone_core_enable_echo_cancellation(linphone_qt_get_core(),
		ui->checkBox_echo_cancelation->isChecked());
}

void Preference::on_comboBox_webcams_currentIndexChanged(const QString &arg1)
{
	linphone_core_set_video_device(linphone_qt_get_core(),arg1.toUtf8().data());
}

void Preference::on_comboBox_video_size_currentIndexChanged(const QString &arg1)
{
	return;
}

void Preference::on_lineEdit_displayname_textChanged(const QString &arg1)
{
	QString displayname=arg1;
	QString username=ui->lineEdit_username->text();
	int port=linphone_core_get_sip_port(linphone_qt_get_core());
	LinphoneAddress *parsed = NULL;
	parsed = linphone_core_get_primary_contact_parsed(linphone_qt_get_core());
	char *contact;
	if (!parsed) return;
	if (username[0]=='\0') return;

	linphone_address_set_display_name(parsed,displayname.toUtf8().data());
	linphone_address_set_username(parsed,username.toUtf8().data());
	linphone_address_set_port_int(parsed,port);
	contact=linphone_address_as_string(parsed);
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("utf8"));
	QTextCodec *codec = QTextCodec::codecForCStrings();
	QString a;
	a = codec->toUnicode(contact);
	ui->lineEdit_sip_address->setText(a);
	linphone_core_set_primary_contact(linphone_qt_get_core(),contact);
	ms_free(contact);
	linphone_address_destroy(parsed);
}

void Preference::on_lineEdit_username_textChanged(const QString &arg1)
{
	QString displayname=ui->lineEdit_displayname->text();
	QString username=arg1;
	int port=linphone_core_get_sip_port(linphone_qt_get_core());
	LinphoneAddress *parsed = NULL;
	parsed = linphone_core_get_primary_contact_parsed(linphone_qt_get_core());
	char *contact;
	if (!parsed) return;
	if (username[0]=='\0') return;

	linphone_address_set_display_name(parsed,displayname.toUtf8().data());
	linphone_address_set_username(parsed,username.toUtf8().data());
	linphone_address_set_port_int(parsed,port);
	contact=linphone_address_as_string(parsed);
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("utf8"));
	QTextCodec *codec = QTextCodec::codecForCStrings();
	QString a;
	a = codec->toUnicode(contact);
	ui->lineEdit_sip_address->setText(a);
	linphone_core_set_primary_contact(linphone_qt_get_core(),contact);
	ms_free(contact);
	linphone_address_destroy(parsed);
}

void Preference::on_lineEdit_sip_address_textChanged(const QString &arg1)
{
    return;
}

LinphoneProxyConfig* Preference::linphone_qt_get_selected_proxy_config()
{
/*	GtkTreeView *v=GTK_TREE_VIEW(linphone_gtk_get_widget(pb,"proxy_list"));
	GtkTreeSelection *selection=gtk_tree_view_get_selection(v);
	GtkTreeIter iter;
	GtkTreeModel *model;
	if (gtk_tree_selection_get_selected(selection,&model,&iter)){
		LinphoneProxyConfig *cfg=NULL;
		gtk_tree_model_get(model,&iter,PROXY_CONFIG_REF,&cfg,-1);
		return cfg;
	}
	return NULL;
*/
	return NULL;
}

void Preference::on_pushButton_edit_proxy_clicked()
{
	QStringList accountlist;
	QStringListModel *proxymodel;
	int current_row = -1;

	proxymodel = (QStringListModel *)ui->listView_proxy_list->model();
	current_row = ui->listView_proxy_list->currentIndex().row();
	if (current_row < 0) return;
	accountlist = proxymodel->stringList();
	QString account = accountlist.at(current_row);

	const MSList *elem;
	LinphoneProxyConfig *cfg;
	bool finded = false;

	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("utf8"));
	QTextCodec *codec = QTextCodec::codecForCStrings();
	QString a;
	for(elem=linphone_core_get_proxy_config_list(linphone_qt_get_core());elem!=NULL;elem=ms_list_next(elem)){
		cfg=(LinphoneProxyConfig*)elem->data;
		a = codec->toUnicode(linphone_proxy_config_get_identity(cfg));	
		if (a == account){
			finded = true;
			break;
		}
	}
	if (!finded) return;
	if (cfg){
		_sip_Account->clear_proxy();
		_sip_Account->set_proxy(cfg);
		/* also update the main window's list of identities*/
		w->qt_load_identities();
		_sip_Account->show();
	}
}

void Preference::on_pushButton_remove_proxy_clicked()
{
   	QStringList accountlist;
	QStringListModel *proxymodel;
	int current_row = -1;

	proxymodel = (QStringListModel *)ui->listView_proxy_list->model();
	current_row = ui->listView_proxy_list->currentIndex().row();
	if (current_row < 0) return;
	accountlist = proxymodel->stringList();
	QString account = accountlist.at(current_row);

	const MSList *elem;
	LinphoneProxyConfig *cfg;
	bool finded = false;

	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("utf8"));
	QTextCodec *codec = QTextCodec::codecForCStrings();
	QString a;
	for(elem=linphone_core_get_proxy_config_list(linphone_qt_get_core());elem!=NULL;elem=ms_list_next(elem)){
		cfg=(LinphoneProxyConfig*)elem->data;
		a = codec->toUnicode(linphone_proxy_config_get_identity(cfg));	
		if (a == account){
			finded = true;
			break;
		}
	}
	if (!finded) return;
	if (cfg){
		linphone_core_remove_proxy_config(linphone_qt_get_core(),cfg);
		linphone_qt_show_sip_accounts();
		/* also update the main window's list of identities*/
		w->qt_load_identities();
	}
}

void Preference::linphone_qt_codec_move(int dir)
{
	const MSList *list;
	QStandardItemModel *model;
	int row = 0;

	if(ui->comboBox_codec_view->currentIndex()==0){
		list=linphone_core_get_audio_codecs(linphone_qt_get_core());
	}else{
		list=linphone_core_get_video_codecs(linphone_qt_get_core());
	}
	model = (QStandardItemModel *)ui->tableView_codec->model();
	row = ui->tableView_codec->currentIndex().row();
	if (row<0) return;

	PayloadType *pt=NULL;
	LinphoneCore *lc=linphone_qt_get_core();
	QVariant pt_data;
	pt_data = model->data(model->index(row, 5));
	pt = pt_data.value<PayloadType *>();
	MSList *sel_elem,*before;
	MSList *codec_list;
	if (pt->type==PAYLOAD_VIDEO)
		codec_list=ms_list_copy(linphone_core_get_video_codecs(lc));
	else codec_list=ms_list_copy(linphone_core_get_audio_codecs(lc));
	sel_elem=ms_list_find(codec_list,pt);
	if (dir>0) {
			if (sel_elem->prev) before=sel_elem->prev;
			else before=sel_elem;
			codec_list=ms_list_insert(codec_list,before,pt);
	}
	else{
			if (sel_elem->next) before=sel_elem->next->next;
			else before=sel_elem;
			codec_list=ms_list_insert(codec_list,before,pt);
	}
	codec_list=ms_list_remove_link(codec_list,sel_elem);
	if (pt->type==PAYLOAD_VIDEO)
		linphone_core_set_video_codecs(lc,codec_list);
	else linphone_core_set_audio_codecs(lc,codec_list);
	linphone_qt_show_codecs(codec_list);
	linphone_qt_select_codec(pt);
}

void Preference::linphone_qt_select_codec(PayloadType *pt)
{
	QStandardItemModel *model;
	model = (QStandardItemModel *)ui->tableView_codec->model();
	int row = model->rowCount();
	if (row<0) return;
	QVariant pt_data;
	PayloadType *ref;
	//QItemSelectionModel *selectionModel = ui->tableView_codec->selectionModel();
	for(int i=0;i<row;i++){
		pt_data = model->data(model->index(i, 5));
		ref = pt_data.value<PayloadType *>();
		if(ref == pt){
			ui->tableView_codec->selectRow(i);
			//selectionModel->select
		}
	}

}

void Preference::on_pushButton_up_codec_clicked()
{
	linphone_qt_codec_move(1);
}

void Preference::on_pushButton_down_codec_clicked()
{
    linphone_qt_codec_move(0);
}

void Preference::linphone_qt_codec_set_enable(bool enabled)
{
	QStandardItemModel *model;
	int row = -1;
	QVariant pt_data;
	PayloadType *pt;

	model = (QStandardItemModel *)ui->tableView_codec->model();
	row = ui->tableView_codec->currentIndex().row();
	if (row<0) return;
	pt_data = model->data(model->index(row, 5));
	pt = pt_data.value<PayloadType *>();
	linphone_core_enable_payload_type(linphone_qt_get_core(),pt,enabled);
	QStandardItem *item_status = new QStandardItem(enabled ? "Enabled": "Disabled");
	model->setItem(row, 2, item_status);
	linphone_qt_select_codec(pt);
}

void Preference::on_pushButton_enable_codec_clicked()
{
    linphone_qt_codec_set_enable(true);
}

void Preference::on_pushButton_disable_codec_clicked()
{
	linphone_qt_codec_set_enable(false);
}

void Preference::on_spinBox_upload_bw_valueChanged(int arg1)
{
    return;
}

void Preference::on_spinBox_download_bw_valueChanged(int arg1)
{
    return;
}

void Preference::on_comboBox_lang_currentIndexChanged(const QString &arg1)
{
    return;
}

void Preference::on_comboBox_video_size_currentIndexChanged(int index)
{
	const MSVideoSizeDef *defs=linphone_core_get_supported_video_sizes(linphone_qt_get_core());
	if (index<0) return;
	linphone_core_set_preferred_video_size(linphone_qt_get_core(),
					defs[index].vsize);
}
