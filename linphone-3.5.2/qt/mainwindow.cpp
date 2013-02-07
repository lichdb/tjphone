#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QCompleter>
#include <QStringList>
#include <QtGui/QPushButton>
#include <QStyledItemDelegate>
#include <QTreeWidgetItem>
#include <QTreeWidget>
#include <QTextCodec>
#include <QPainter>
#include <QScrollBar>
#include <QSizePolicy>
#include <QDesktopServices>
#include <QUrl>
#include <windows.h>
#include "linphone.h"
#include "lpconfig.h"

#include "friend_list.h"
#include "custom_tree.h"
#include "callviewframe.h"
#include "conferenceframe.h"
#include "chatroom.h"


MainWindow *w=NULL;
QStringList uri_list;

#define LINPHONE_ICON "linphone.png"

// pointer to item delegate (used by proxy_delegate)
static const int c_friend_role = Qt::UserRole + 3;

Q_DECLARE_METATYPE(friend_record)
Q_DECLARE_METATYPE(LinphoneProxyConfig *)
Q_DECLARE_METATYPE(LinphoneOnlineStatus)

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
	_Preference = NULL;
	_debugwindow = NULL;
	_callhistory = NULL;
	_conferenceframe = NULL;
    ui->setupUi(this);
	connect(ui->pushButton_0, SIGNAL(pressed()), this, SLOT(dtmf_pressed_0()));
	connect(ui->pushButton_1, SIGNAL(pressed()), this, SLOT(dtmf_pressed_1()));
	connect(ui->pushButton_2, SIGNAL(pressed()), this, SLOT(dtmf_pressed_2()));
	connect(ui->pushButton_3, SIGNAL(pressed()), this, SLOT(dtmf_pressed_3()));
	connect(ui->pushButton_4, SIGNAL(pressed()), this, SLOT(dtmf_pressed_4()));
	connect(ui->pushButton_5, SIGNAL(pressed()), this, SLOT(dtmf_pressed_5()));
	connect(ui->pushButton_6, SIGNAL(pressed()), this, SLOT(dtmf_pressed_6()));
	connect(ui->pushButton_7, SIGNAL(pressed()), this, SLOT(dtmf_pressed_7()));
	connect(ui->pushButton_8, SIGNAL(pressed()), this, SLOT(dtmf_pressed_8()));
	connect(ui->pushButton_9, SIGNAL(pressed()), this, SLOT(dtmf_pressed_9()));
	connect(ui->pushButton_10, SIGNAL(pressed()), this, SLOT(dtmf_pressed_asterisk()));
	connect(ui->pushButton_11, SIGNAL(pressed()), this, SLOT(dtmf_pressed_pound()));
	connect(ui->pushButton_A, SIGNAL(pressed()), this, SLOT(dtmf_pressed_A()));
	connect(ui->pushButton_B, SIGNAL(pressed()), this, SLOT(dtmf_pressed_B()));
	connect(ui->pushButton_C, SIGNAL(pressed()), this, SLOT(dtmf_pressed_C()));
	connect(ui->pushButton_D, SIGNAL(pressed()), this, SLOT(dtmf_pressed_D()));

	connect(ui->pushButton_0, SIGNAL(pressed()), this, SLOT(dtmf_released()));
	connect(ui->pushButton_1, SIGNAL(pressed()), this, SLOT(dtmf_released()));
	connect(ui->pushButton_2, SIGNAL(pressed()), this, SLOT(dtmf_released()));
	connect(ui->pushButton_3, SIGNAL(pressed()), this, SLOT(dtmf_released()));
	connect(ui->pushButton_4, SIGNAL(pressed()), this, SLOT(dtmf_released()));
	connect(ui->pushButton_5, SIGNAL(pressed()), this, SLOT(dtmf_released()));
	connect(ui->pushButton_6, SIGNAL(pressed()), this, SLOT(dtmf_released()));
	connect(ui->pushButton_7, SIGNAL(pressed()), this, SLOT(dtmf_released()));
	connect(ui->pushButton_8, SIGNAL(pressed()), this, SLOT(dtmf_released()));
	connect(ui->pushButton_9, SIGNAL(pressed()), this, SLOT(dtmf_released()));
	connect(ui->pushButton_10, SIGNAL(pressed()), this, SLOT(dtmf_released()));
	connect(ui->pushButton_11, SIGNAL(pressed()), this, SLOT(dtmf_released()));
	connect(ui->pushButton_A, SIGNAL(pressed()), this, SLOT(dtmf_released()));
	connect(ui->pushButton_B, SIGNAL(pressed()), this, SLOT(dtmf_released()));
	connect(ui->pushButton_C, SIGNAL(pressed()), this, SLOT(dtmf_released()));
	connect(ui->pushButton_D, SIGNAL(pressed()), this, SLOT(dtmf_released()));
	if(linphone_qt_get_ui_config_int("video", 0)){
		ui->actionAudio_vedio->setChecked(true);
		ui->action_audio_only->setChecked(false);
		ui->actionEnable_selfview->setEnabled(true);
		//enable_self_view(true);
	}else{
		//enable_self_view(false);
		ui->actionAudio_vedio->setChecked(false);
		ui->action_audio_only->setChecked(true);
		ui->actionEnable_selfview->setEnabled(false);
	}
	if(linphone_qt_get_ui_config_int("videoselfview",1)){
		ui->actionEnable_selfview->setChecked(true);
		if(linphone_qt_get_ui_config_int("video", 0))
			enable_self_view(true);
	}else{
		ui->actionEnable_selfview->setChecked(false);
	}
	//on_action_audio_only_triggered();
	_tree = new custom_tree(this);
	QSizePolicy policy(QSizePolicy::Expanding,QSizePolicy::Expanding); 
	policy.setHorizontalStretch(0);
	policy.setVerticalStretch(0);
	policy.setHeightForWidth(_tree->sizePolicy().hasHeightForWidth());
	_tree->setSizePolicy(policy);
	ui->gridLayout_4->addWidget(_tree, 2, 0, 1, 1);
	_tree->friends_ = new friend_list();
	//_tree->setAlternatingRowColors(true);
	//_tree->setBackgroundRole(QPalette::Base);
	//_tree->setFrameStyle();
	_tree->setSelectionMode(QAbstractItemView::ExtendedSelection);
	_tree->setRootIsDecorated(false);
//     _tree->setUniformRowHeights(true);
	_tree->setSortingEnabled(true);
	_tree->setAllColumnsShowFocus(true);
//     _tree->setWordWrap(true);

	_tree->setContextMenuPolicy(Qt::DefaultContextMenu);

	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("utf8"));
	QTextCodec *codec = QTextCodec::codecForCStrings();
	QString a; 
	
	QTreeWidgetItem* it = _tree->headerItem();
	a = codec->toUnicode("name");
	it->setText(0, a);
	a = codec->toUnicode("status");
	it->setText(1, a);

	QHeaderView* hdr = _tree->header();
	hdr->resizeSection(0, 80);
	_tree->setItemDelegateForColumn( 1, new status_item_delegate(this) );

	_presence_menu = _tree->create_presence_menu();
	connect(_presence_menu, SIGNAL(triggered(QAction *)), this, SLOT(set_my_presence(QAction *)));
	_presence_button = new QToolButton(this);
	this->statusBar()->addPermanentWidget(_presence_button);
	connect(_presence_button, SIGNAL(pressed()), this, SLOT(show_presence_menu()));

	ui->toolButton_call->setIconSize(QSize(60,52));
	ui->toolButton_call->setIcon(QIcon("icons:startcall-green.png"));
	ui->toolButton_Hangup->setIconSize(QSize(60,54));
	ui->toolButton_Hangup->setIcon(QIcon("icons:stopcall-red.png"));
	ui->toolButton_call->setToolTip(tr("call"));
	ui->toolButton_Hangup->setToolTip(tr("hangup"));
	ui->toolButton_add_call->setIconSize(QSize(60,52));
	ui->toolButton_add_call->setIcon(QIcon("icons:startcall-green.png"));
	ui->toolButton_add_call->setToolTip(tr("add call"));
	ui->toolButton_add_call->setShown(false);
	connect(ui->toolButton_add_call, SIGNAL(pressed()), this, SLOT(on_toolButton_call_clicked()));

}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::preference_closed()
{
    _Preference = NULL;
}

void MainWindow::debug_closed()
{
    _debugwindow = NULL;
}

void MainWindow::history_closed()
{
    _callhistory = NULL;
}
void MainWindow::on_action_Quit_triggered()
{
	LinphoneCore *lc=linphone_qt_get_core();
	if (linphone_core_in_call(lc)){
		linphone_core_terminate_all_calls(lc);
	}
	linphone_core_enable_video_preview(lc,FALSE);
    close();
}

#define VIDEOSELFVIEW_DEFAULT 1

void MainWindow::on_actionAudio_vedio_triggered()
{
    ui->actionAudio_vedio->setChecked(true);
	ui->action_audio_only->setChecked(false);
	ui->actionEnable_selfview->setEnabled(true);
	linphone_core_enable_video(linphone_qt_get_core(),TRUE,TRUE);
	if(linphone_qt_get_ui_config_int("videoselfview",VIDEOSELFVIEW_DEFAULT)){
		ui->actionEnable_selfview->setChecked(true);
	}else{
		ui->actionEnable_selfview->setChecked(false);
	}
	linphone_core_enable_video_preview(linphone_qt_get_core(),linphone_qt_get_ui_config_int("videoselfview",VIDEOSELFVIEW_DEFAULT));
	linphone_qt_set_ui_config_int("video", TRUE);
}

void MainWindow::on_action_audio_only_triggered()
{
    ui->actionAudio_vedio->setChecked(false);
    ui->action_audio_only->setChecked(true);
	ui->actionEnable_selfview->setEnabled(false);
	linphone_core_enable_video(linphone_qt_get_core(),FALSE,FALSE);
	linphone_core_enable_video_preview(linphone_qt_get_core(),FALSE);
	linphone_qt_set_ui_config_int("video", FALSE);
	//enable_self_view(false);
}

void MainWindow::on_action_Homepage_triggered()
{
     QDesktopServices::openUrl(QUrl("http://code.google.com/p/tjphone/"));
}

void MainWindow::on_action_About_triggered()
{
    QApplication::aboutQt();
}

void MainWindow::on_action_Call_history_triggered()
{
	if(!_callhistory){
		_callhistory = new Callhistory(this);
		_callhistory->show();
	}
	_callhistory->raise();
}

void MainWindow::on_action_Show_debug_window_triggered()
{
	if(!_debugwindow){
		linphone_core_enable_logs_with_cb(qt_log_handler);
		ortp_set_log_level_mask(ORTP_WARNING|ORTP_ERROR|ORTP_FATAL|ORTP_DEBUG);
		_debugwindow = new debugwindow(this);
		_debugwindow->show();
	}
	_debugwindow->raise();	
}

void MainWindow::on_action_Preferences_triggered()
{
    if(!_Preference){
        _Preference = new Preference(this);
		_Preference->show();
	}
	_Preference->raise();
}

#define QT

static void set_video_window_decorations(HWND id){
	const char *title=linphone_qt_get_ui_config("title","TJphone");
	const char *icon_path=linphone_qt_get_ui_config("icon",LINPHONE_ICON);
	char video_title[256];
	//GdkPixbuf *pbuf=create_pixbuf(icon_path);
	if (!linphone_core_in_call(linphone_qt_get_core())){
		snprintf(video_title,sizeof(video_title),"%s video",title);
		/* When not in call, treat the video as a normal window */
		//gdk_window_set_keep_above(w, FALSE);
		///////ww->lower();
	}else{
		LinphoneAddress *uri =
			linphone_address_clone(linphone_core_get_current_call_remote_address(linphone_qt_get_core()));
		char *display_name;

		linphone_address_clean(uri);
		if (linphone_address_get_display_name(uri)!=NULL){
			display_name=ms_strdup(linphone_address_get_display_name(uri));
		}else{
			display_name=linphone_address_as_string(uri);
		}
		snprintf(video_title,sizeof(video_title),("Call with %s"),display_name);
		linphone_address_destroy(uri);
		ms_free(display_name);

		/* During calls, bring up the video window, arrange so that
		it is above all the other windows */
		//gdk_window_deiconify(w);
		//gdk_window_set_keep_above(w,TRUE);
		///////ww->raise();
		/* Maybe we should have the following, but then we want to
		have a timer that turns it off after a little while. */
		/* gdk_window_set_urgency_hint(w,TRUE); */
	}

	ShowWindow(id, SW_SHOW); 
	SetClassLong(id, GCL_STYLE, CS_NOCLOSE);
	//SetWindowLong(id, GWL_STYLE, WS_OVERLAPPEDWINDOW);
	QString a(video_title);
	SetWindowText(id, (const wchar_t*)a.utf16());
	UpdateWindow(id);
	/*if (pbuf){
		GList *l=NULL;
		l=g_list_append(l,pbuf);
		gdk_window_set_icon_list(w,l);
		g_list_free(l);
		g_object_unref(G_OBJECT(pbuf));
	}
	*/
}

static gboolean video_needs_update=FALSE;

static void update_video_title(){
	video_needs_update=TRUE;
}

void MainWindow::linphone_qt_check_logs()
{
	_debugwindow->check_logs();
}

void MainWindow::linphone_qt_iterate()
{
	static bool first_time=TRUE;
	unsigned long id;
	static unsigned long previd=0;
	static unsigned long preview_previd=0;
	static bool in_iterate=FALSE;
	
	LinphoneCore *lc = linphone_qt_get_core();
	/*avoid reentrancy*/
	if (in_iterate) return;
	in_iterate=TRUE;
	linphone_core_iterate(lc);
	if (first_time){
		/*after the first call to iterate, SipSetupContexts should be ready, so take actions:*/
		//linphone_gtk_show_directory_search();
		first_time=FALSE;
	}

	//视频输出处理

	id=linphone_core_get_native_video_window_id(lc);
	if (id!=previd || video_needs_update){
#ifdef QT
		QWidget *ww;
#else
		GdkWindow *ww;
#endif
		previd=id;
		if (id!=0){
			ms_message("Updating window decorations");
#ifndef WIN32
			ww=gdk_window_foreign_new(id);
#else
#ifdef QT
			ww=QWidget::find((HWND)id);
#else
			ww=gdk_window_foreign_new((HANDLE)id);
#endif
#endif
			if (id) {
				Sleep(100);
				set_video_window_decorations((HWND)id);
			}
			else ms_error("gdk_window_foreign_new() failed");
			if (video_needs_update) video_needs_update=FALSE;
		}
	}
	id=linphone_core_get_native_preview_window_id (lc);
	if (id!=preview_previd ){
#ifdef QT
		QWidget *ww;
#else
		GdkWindow *ww;
#endif
		preview_previd=id;
		if (id!=0){
			ms_message("Updating window decorations for preview");
#ifndef WIN32
			ww=gdk_window_foreign_new(id);
#else

#ifdef QT
			ww=QWidget::find((HWND)id);
#else
			ww=gdk_window_foreign_new((HANDLE)id);
#endif
#endif
			if (id) {
				set_video_window_decorations((HWND)id);
			}
			else ms_error("gdk_window_foreign_new() failed");
			if (video_needs_update) video_needs_update=FALSE;
		}
	}
	/*
	if (addr_to_call!=NULL){
		GtkWidget *mw=linphone_gtk_get_main_window();
		GtkWidget *login_frame=linphone_gtk_get_widget(mw,"login_frame");
		if (!GTK_WIDGET_VISIBLE(login_frame)){
			GtkWidget *uri_bar=linphone_gtk_get_widget(mw,"uribar");
			gtk_entry_set_text(GTK_ENTRY(uri_bar),addr_to_call);
			addr_to_call=NULL;
			linphone_qt_start_call(uri_bar);
		}
	}
	*/
	in_iterate=FALSE;
}

void MainWindow::qt_load_identities(void)
{
	
	const MSList *elem;
	QString def_identity;
	LinphoneProxyConfig *def=NULL;
	int def_index=0,i,count=0;

	ui->comboBox_account->clear();
	linphone_core_get_default_proxy(linphone_qt_get_core(),&def);
	
	def_identity.sprintf("%s",linphone_core_get_primary_contact(linphone_qt_get_core()));
	def_identity += _(" (Local)");

	ui->comboBox_account->insertItem(count, def_identity, QVariant::fromValue(NULL));

	for(i=1,elem=linphone_core_get_proxy_config_list(linphone_qt_get_core());
			elem!=NULL;
			elem=ms_list_next(elem),i++){
		LinphoneProxyConfig *cfg=(LinphoneProxyConfig*)elem->data;
		def_identity = linphone_proxy_config_get_identity(cfg);
		count++;
		ui->comboBox_account->insertItem(count, def_identity, QVariant::fromValue(def));
		if (cfg==def) {
			def_index=i;
		}
	}
	ui->comboBox_account->setCurrentIndex(def_index);
	
}

void MainWindow::load_uri_history()
{
	int i;
	char key[20];
	uri_list.clear();
	for (i=0;;i++)
	{
		const char *uri;
		snprintf(key,sizeof(key),"uri%i",i);
		uri=linphone_qt_get_ui_config(key,NULL);
		if (uri!=NULL) {
			uri_list.append(uri);
			if (i==0) ui->lineEdit_Uri->setText(uri);
		}
		else break;
	}
	QCompleter *completer = new QCompleter(uri_list, this);
	completer->setCaseSensitivity(Qt::CaseInsensitive);
	ui->lineEdit_Uri->setCompleter(completer);
}

void MainWindow::save_uri_history()
{
	LinphoneCore *lc=linphone_qt_get_core();
	LpConfig *cfg=linphone_core_get_config(lc);
	char key[20];
	int i=0;
	QCompleter *completer;
	completer = ui->lineEdit_Uri->completer();
	for (i = 0; completer->setCurrentRow(i); i++)
	{
		snprintf(key,sizeof(key),"uri%i",i);
		lp_config_set_string(cfg,"QtUi",key,completer->currentCompletion().toAscii());
		if (i>10) break;
	}
	lp_config_sync(cfg);
}

void MainWindow::completion_add_text(QString &text)
{
	int i;
	QCompleter *completer;
	i = uri_list.indexOf(text);
	if (-1 == i)
	{
		uri_list.insert(0, text);
	}else{
		uri_list.removeAt(i);
		uri_list.insert(0, text);
	}
	completer = ui->lineEdit_Uri->completer();
	delete completer;
	completer = new QCompleter(uri_list, this);
	completer->setCaseSensitivity(Qt::CaseInsensitive);
	ui->lineEdit_Uri->setCompleter(completer);
	save_uri_history();
}

void MainWindow::linphone_qt_call_terminated(LinphoneCall *call, const char *error)
{
	ui->toolButton_Hangup->setEnabled(false);
	ui->toolButton_call->setEnabled(true);
	if (linphone_qt_use_in_call_view() && call)
		this->call_view_terminate(call,QString(error));
	update_video_title();
}

void MainWindow::linphone_qt_start_call_do(QString &uri)
{
	if (linphone_core_invite(linphone_qt_get_core(),uri.toAscii())!=NULL) {
		completion_add_text(uri);
	}else{
		linphone_qt_call_terminated(NULL, NULL);
	}
}

void MainWindow::on_toolButton_call_clicked()
{
	LinphoneCore *lc=linphone_qt_get_core();
	LinphoneCall *call;

	call=get_currently_displayed_call(NULL);
	if (call!=NULL && linphone_call_get_state(call)==LinphoneCallIncomingReceived){
		linphone_core_accept_call(lc,call);
	}else{
		/*immediately disable the button and delay a bit the execution the linphone_core_invite()
		so that we don't freeze the button. linphone_core_invite() might block for some hundreds of milliseconds*/
		ui->toolButton_call->setEnabled(false);
		_callouttimerid = this->startTimer(100);
	}
	
}

void MainWindow::on_toolButton_Hangup_clicked()
{
	bool_t is_conf;
	LinphoneCall *call=get_currently_displayed_call(&is_conf);
	if (call){
		linphone_core_terminate_call(linphone_qt_get_core(),call);
	}else if (is_conf){
		linphone_core_terminate_conference(linphone_qt_get_core());
	}
}

void MainWindow::update_friends(LinphoneCore *lc, LinphoneFriend * fid)
{
	const MSList *itf;
	bool found=false;
	LinphoneCore *core=lc;
	const LinphoneFriend *lf;
	LinphoneOnlineStatus status = linphone_friend_get_status(fid);
	const LinphoneAddress *f_addr = linphone_friend_get_address(fid);
	char *furi = linphone_address_as_string_uri_only(f_addr);
	for(itf=linphone_core_get_friend_list(core);itf!=NULL;itf=ms_list_next(itf)){
		LinphoneFriend *lf=(LinphoneFriend*)itf->data;
		if(fid == lf) found= true;
	}
	if(!found){
		return;	
	}
	for (int i = 0; i < _tree->topLevelItemCount(); i++){
			QTreeWidgetItem* ti = _tree->topLevelItem(i);
            friend_record fr = ti->data(1, c_friend_role).value<friend_record>();
			if(fr.isgroup()) continue;
			lf = fr.frienddata();
			if(lf){
				const LinphoneAddress *addr=linphone_friend_get_address(lf);
				char *uri = linphone_address_as_string_uri_only(addr);
				if(strcmp(uri, furi)==0){
				}
			}
			
    }

	//const friend_list::friend_records_t& fl = _tree->friends_->list();
	//update_tree_contents(fl, c_friend_role, _tree, 0);
}
void MainWindow::show_friends()
{
	const MSList *itf;
	LinphoneCore *core=linphone_qt_get_core();
	gboolean online_only=FALSE,lookup=FALSE;
	const char *search=NULL;
	friend_record aa;
	_tree->clear();
	_tree->friends_->remove_all();
	for(itf=linphone_core_get_friend_list(core);itf!=NULL;itf=ms_list_next(itf)){
		LinphoneFriend *lf=(LinphoneFriend*)itf->data;
		const LinphoneAddress *f_uri=linphone_friend_get_address(lf);
		//char *uri=linphone_address_as_string(f_uri);
		char *uri = linphone_address_as_string_uri_only(f_uri);
		const char *name=linphone_address_get_display_name(f_uri);
		const char *display=name;
		char *escaped=NULL;
		if (lookup){
			if (strstr(uri,search)==NULL){
				ms_free(uri);
				continue;
			}
		}
		if (!online_only || (linphone_friend_get_status(lf)!=LinphoneStatusOffline)){
			BuddyInfo *bi;
			if (name==NULL || name[0]=='\0') display=uri;
			bi=linphone_friend_get_info(lf);
			if (bi!=NULL && bi->image_data!=NULL){
				/* 用户的头像
				GdkPixbuf *pbuf=
					_gdk_pixbuf_new_from_memory_at_scale(bi->image_data,bi->image_length,-1,40,TRUE);
				if (pbuf) {
					gtk_list_store_set(store,&iter,FRIEND_ICON,pbuf,-1);
					g_object_unref(G_OBJECT(pbuf));
				}
				*/
			}
			aa.set_name(display);
			aa.set_group("default");
			aa.set_isgroup(false);
			aa.set_uri(uri);
			aa.set_comment(lf);
			aa.set_presence_status(linphone_online_status_to_string(linphone_friend_get_status(lf)));
			_tree->friends_->add(aa);
			ms_free(uri);
		}
	}
	const friend_list::friend_records_t& fl = _tree->friends_->list();
	update_tree_contents(fl, c_friend_role, _tree, 0);
}

void MainWindow::update_friend_item(QTreeWidgetItem* item, const friend_record& fr)
{
	item->setText(0, fr.name());
	item->setText(1, fr.presence_status());
}
void MainWindow::update_tree_contents(const const friend_list::friend_records_t& fl, int role, custom_tree* tree, QTreeWidgetItem* parent_item)
{
	foreach (const friend_record& item, fl)
    {
        // search item
		QTreeWidgetItem* ti = new QTreeWidgetItem((QTreeWidget *)tree);
        ti->setData(1, role, QVariant::fromValue(item));
		update_friend_item(ti, item);
    }
	return;
}


void MainWindow::dtmf_pressed_0()
{
	dtmf_play("0");
}
void MainWindow::dtmf_pressed_1()
{
	dtmf_play("1");
}
void MainWindow::dtmf_pressed_2()
{
	dtmf_play("2");
}
void MainWindow::dtmf_pressed_3()
{
	dtmf_play("3");
}
void MainWindow::dtmf_pressed_4()
{
	dtmf_play("4");
}
void MainWindow::dtmf_pressed_5()
{
	dtmf_play("5");
}
void MainWindow::dtmf_pressed_6()
{
	dtmf_play("6");
}
void MainWindow::dtmf_pressed_7()
{
	dtmf_play("7");
}
void MainWindow::dtmf_pressed_8()
{
	dtmf_play("8");
}
void MainWindow::dtmf_pressed_9()
{
	dtmf_play("9");
}
void MainWindow::dtmf_pressed_asterisk()
{
	dtmf_play("*");
}
void MainWindow::dtmf_pressed_pound()
{
	dtmf_play("#");
}
void MainWindow::dtmf_pressed_A()
{
	dtmf_play("A");
}
void MainWindow::dtmf_pressed_B()
{
	dtmf_play("B");
}
void MainWindow::dtmf_pressed_C()
{
	dtmf_play("C");
}
void MainWindow::dtmf_pressed_D()
{
	dtmf_play("D");
}


void MainWindow::dtmf_play(const char *label)
{
	QString str;
	str.clear();
	str[0] = label[0];
	ui->lineEdit_Uri->insert(str);
	linphone_core_play_dtmf (linphone_qt_get_core(),label[0],-1);
	if (linphone_core_in_call(linphone_qt_get_core())){
		linphone_core_send_dtmf(linphone_qt_get_core(),label[0]);
	}
}

void MainWindow::dtmf_released()
{
	linphone_core_stop_dtmf (linphone_qt_get_core());
}


LinphoneCall* MainWindow::get_currently_displayed_call(bool_t *is_conf)
{
	LinphoneCall *call=NULL;
	LinphoneCore *lc=linphone_qt_get_core();	const MSList *calls=linphone_core_get_calls(lc);
	if (!linphone_qt_use_in_call_view() || ms_list_size(calls)==1){
		if (calls) return (LinphoneCall*)calls->data;
	}else{
		int idx = ui->tabWidget->currentIndex();
		QFrame *frame = (QFrame *)ui->tabWidget->currentWidget();
		if (frame!=NULL && idx>1){
			if(frame->objectName()=="CallViewFrame"){
				CallViewFrame *callview =  qobject_cast<CallViewFrame *>(frame);
				if(callview) call = callview->_call;
			}
			frame = (QFrame *)ui->tabWidget->findChild<QFrame *>("ConferenceFrame");
			if (frame){
				if (is_conf)
					*is_conf=TRUE;
			}else{
				if (is_conf)
					*is_conf=FALSE;
			}
			return call;
		}
	}
	return NULL;
}


void MainWindow::create_in_call_view(LinphoneCall *call)
{
	static int call_index=1;
	int idx;
	CallViewFrame *callview = new CallViewFrame(ui->tabWidget);

	if (ms_list_size(linphone_core_get_calls(linphone_qt_get_core()))==1){
		/*this is the only call at this time */
		call_index=1;
	}
	callview->_call = call;
	callview->_call_index = call_index;

	linphone_call_set_user_pointer (call,callview);
	linphone_call_ref(call);

	callview->enable_hold_button(false,true);
	callview->enable_mute_button(false);
	callview->show_answer_decline(false);

	QString label;
	label.sprintf("Call #%d", call_index);
	ui->tabWidget->addTab(callview, label);
	idx = ui->tabWidget->indexOf(callview);
	ui->tabWidget->setCurrentIndex(idx);
	call_index++;
}

void MainWindow::call_view_set_calling(LinphoneCall *call)
{
	CallViewFrame *callview=(CallViewFrame *)linphone_call_get_user_pointer(call);
	
	callview->set_callstatus(_("Calling..."));
	QString a = "00::00::00";
	callview->set_duration(a);
	callview->set_peer_name(get_peer_name(linphone_call_get_remote_address (call)));
	callview->show_mute_hold(false);
	//callview->enable_mute_button(false);
	/*if (pbuf!=NULL){
		gtk_image_set_from_animation(GTK_IMAGE(animation),pbuf);
		g_object_unref(G_OBJECT(pbuf));
	}else gtk_image_set_from_stock(GTK_IMAGE(animation),GTK_STOCK_FIND,GTK_ICON_SIZE_DIALOG);
	*/
}


void MainWindow::set_in_conference(LinphoneCall *call)
{
	if(!_conferenceframe){
		_conferenceframe = new conferenceframe(this);
		ui->tabWidget->addTab(_conferenceframe, "Conference");
	}
	_conferenceframe->set_in_conference(call);
}
void MainWindow::call_view_set_in_call(LinphoneCall *call)
{

	CallViewFrame *callview=(CallViewFrame *)linphone_call_get_user_pointer(call);
	if(!callview) return;

	callview->set_incall();
	callview->set_peer_name(get_peer_name(linphone_call_get_remote_address (call)));
	bool_t in_conf=linphone_call_params_local_conference_mode(linphone_call_get_current_params(call));
	callview->call_view_enable_audio_view(!in_conf);
	callview->show_mute_hold(!in_conf);
	callview->enable_mute_button(!in_conf);
	//linphone_gtk_in_call_view_show_encryption(call);
	if (in_conf) set_in_conference(call);	
}

void MainWindow::enable_mute_button(LinphoneCall *call, bool sensitive)
{
	CallViewFrame *callview=(CallViewFrame *)linphone_call_get_user_pointer(call);
	if (callview)
		callview->enable_mute_button(sensitive);
}

void MainWindow::enable_hold_button(LinphoneCall *call, bool sensitive, bool holdon)
{
	CallViewFrame *callview=(CallViewFrame *)linphone_call_get_user_pointer(call);
	if (callview)
		callview->enable_hold_button(sensitive, holdon);
}

void MainWindow::call_view_terminate(LinphoneCall *call, QString &error_msg)
{
	CallViewFrame *callview=(CallViewFrame *)linphone_call_get_user_pointer(call);
	if(!callview) return;
	bool_t in_conf=linphone_call_params_local_conference_mode(linphone_call_get_current_params(call));	
	if (error_msg.isEmpty()){
		callview->set_callstatus(_("Call ended"));
	}else{
		callview->set_callstatus(error_msg);
	}
	/*
	if (pbuf!=NULL){
		gtk_image_set_from_pixbuf(GTK_IMAGE(animation),pbuf);
		g_object_unref(G_OBJECT(pbuf));
	}
	gtk_widget_hide(linphone_gtk_get_widget(callview,"answer_decline_panel"));
	*/
	callview->enable_mute_button(false);
	callview->enable_hold_button(false, true);

	//延时hide callview，可能同时有多个views，所以使用hash表
	int timerid = this->startTimer(2000);//2s
	_terminatetimerid.insert(timerid, call);
	if (in_conf && _conferenceframe)
		_conferenceframe->terminate_conference_participant(call);
}

void MainWindow::remove_in_call_view(LinphoneCall *call)
{
	CallViewFrame *callview=(CallViewFrame *)linphone_call_get_user_pointer(call);
	int idx;
	if (!callview) return;
	bool_t in_conf=linphone_call_params_local_conference_mode(linphone_call_get_current_params(call));
	idx=ui->tabWidget->indexOf(callview);
	ui->tabWidget->removeTab(idx);
	callview->close();
	delete callview;
	if (in_conf && _conferenceframe){
		_conferenceframe->unset_from_conference(call);
	}
	linphone_call_set_user_pointer (call,NULL);
	linphone_call_unref(call);
	call=linphone_core_get_current_call(linphone_qt_get_core());
	if (call==NULL){
		if (linphone_core_is_in_conference(linphone_qt_get_core())){
			/*show the conference*/
			ui->tabWidget->setCurrentIndex(ui->tabWidget->indexOf(_conferenceframe));
		}else ui->tabWidget->setCurrentIndex(0);
	}else{
		/*show the active call*/
		callview=(CallViewFrame *)linphone_call_get_user_pointer(call);
		ui->tabWidget->setCurrentIndex(ui->tabWidget->indexOf(callview));
	}
}

void MainWindow::call_view_set_incoming(LinphoneCall *call)
{
	CallViewFrame *callview=(CallViewFrame *)linphone_call_get_user_pointer(call);

	callview->set_callstatus(_("Incoming call"));
	callview->enable_hold_button(false, false);
	callview->show_mute_hold(false);
	callview->show_answer_decline(true);
	callview->set_peer_name(get_peer_name(linphone_call_get_remote_address (call)));

}

void MainWindow::call_view_set_paused(LinphoneCall *call)
{
	CallViewFrame *callview=(CallViewFrame *)linphone_call_get_user_pointer(call);
	callview->set_paused();
}

void MainWindow::update_call_buttons(LinphoneCall *call)
{
	/* 有call的时候，隐藏start call按钮，显示 add_call 按钮*/
	LinphoneCore *lc=linphone_qt_get_core();
	const MSList *calls=linphone_core_get_calls(lc);
	bool start_active=true;
	bool stop_active=false;
	bool add_call=false;
	int call_list_size=ms_list_size(calls);
	
	if (calls==NULL){
		start_active=true;
		stop_active=false;
	}else{
		stop_active=true;	
		start_active=true;
		add_call=true;
	}
	ui->toolButton_call->setEnabled(start_active);
	ui->toolButton_call->setShown(!add_call);

	if (linphone_core_sound_resources_locked(lc) || (call && linphone_call_get_state(call)==LinphoneCallIncomingReceived)) {
		ui->toolButton_add_call->setEnabled(false);
	} else {
		ui->toolButton_add_call->setEnabled(start_active);
	}
	ui->toolButton_add_call->setShown(add_call);
	ui->toolButton_Hangup->setEnabled(stop_active);

	linphone_qt_enable_transfer_button(lc,call_list_size>1);
	linphone_qt_enable_conference_button(lc,call_list_size>1);
	update_video_title();
}
void MainWindow::enable_transfer_button(LinphoneCore *lc, bool value)
{
	const MSList *elem=linphone_core_get_calls(lc);
	for(;elem!=NULL;elem=elem->next){
		LinphoneCall *call=(LinphoneCall*)elem->data;
		CallViewFrame *callview=(CallViewFrame*)linphone_call_get_user_pointer(call);
		QPushButton *transfer_button = callview->get_transfer_button();
		if (transfer_button && value==false){
			delete transfer_button;
			transfer_button=NULL;
		}else if (!transfer_button && value==true){
			transfer_button=new QPushButton(_("transfer"), callview);
		}
		callview->set_transfer_button(transfer_button);
	}
}

void MainWindow::enable_conference_button(LinphoneCore *lc, bool value)
{
	const MSList *elem=linphone_core_get_calls(lc);
	for(;elem!=NULL;elem=elem->next){
		LinphoneCall *call=(LinphoneCall*)elem->data;
		CallViewFrame *callview=(CallViewFrame*)linphone_call_get_user_pointer(call);
		QPushButton *button = callview->get_conference_button();
		if (button && value==FALSE){
			delete button;
			button=NULL;
		}else if (!button && value==TRUE){
			button=new QPushButton(_("Conference"), callview);
		}
		callview->set_conference_button(button);
	}
}

void MainWindow::show_status(QString &status_string)
{
	if(ui)
		ui->statusBar->showMessage(status_string);
}

void MainWindow::in_call_starttimer(LinphoneCall *call)
{
	CallViewFrame *callview=(CallViewFrame*)linphone_call_get_user_pointer(call);
	callview->in_call_starttimer();
}


void MainWindow::in_call_stoptimer(LinphoneCall *call)
{
	CallViewFrame *callview=(CallViewFrame*)linphone_call_get_user_pointer(call);
	callview->in_call_stoptimer();
}

void MainWindow::timerEvent (QTimerEvent *event)
{
	int timerid = event->timerId();
	if(timerid == _autoanswertimerid){
		killTimer(_autoanswertimerid);
		_autoanswertimerid = 0;
		if (linphone_call_get_state(_auto_answer_call)==LinphoneCallIncomingReceived){
			linphone_core_accept_call (linphone_qt_get_core(),_auto_answer_call);
			linphone_call_unref(_auto_answer_call);
			_auto_answer_call = NULL;
		}
	} else if (timerid == _callouttimerid){
		killTimer(_callouttimerid);
		_callouttimerid = 0;
		linphone_qt_start_call_do(ui->lineEdit_Uri->text());
		ui->toolButton_call->setEnabled(true);
	} else {
		killTimer(timerid);
		LinphoneCall *call = _terminatetimerid.value(timerid);
		if(call){
			_terminatetimerid.remove(timerid);
			remove_in_call_view(call);	
		}	
	}
}

void MainWindow::auto_answer(LinphoneCall *call)
{
	_auto_answer_call = call;
	_autoanswertimerid = startTimer(2000);
}

void MainWindow::update_call_log()
{
	if(_callhistory){
		_callhistory->call_log_update();
	}
}
void MainWindow::on_comboBox_account_currentIndexChanged(int index)
{
	if(ui->comboBox_account->currentText().isEmpty()) return;
	//index=0, default没有变。index-1 是因为proxy list和combobox的对应关系是从1开始的。combobox所显示的第一条(index=0)是get_default_proxy得到的。
	linphone_core_set_default_proxy_index(linphone_qt_get_core(),(index==0) ? -1 : (index-1));
	//linphone_gtk_show_directory_search();
}

void MainWindow::enable_self_view(bool val)
{
	gboolean value=FALSE;
	if (val) value=TRUE;
	LinphoneCore *lc=linphone_qt_get_core();
	if (!lc) return;
	linphone_core_enable_video_preview(lc,val);
	linphone_core_enable_self_view(lc,val);
}

void MainWindow::on_actionEnable_selfview_toggled(bool arg1)
{
	gboolean value=FALSE;
	if (arg1) value=TRUE;
	if(ui->actionAudio_vedio->isChecked()){
		enable_self_view(arg1);
	}
	linphone_qt_set_ui_config_int("videoselfview",value);
}

void MainWindow::log_push(OrtpLogLevel lev, const char *fmt, va_list args)
{
	//if(_debugwindow)
	debugwindow::log_push(lev, fmt, args);
}

void MainWindow::update_registration_status(LinphoneProxyConfig *cfg, LinphoneRegistrationState rs)
{
	bool found = false;
	QIcon stock_id;
	int i, count = ui->comboBox_account->count();
	LinphoneProxyConfig *p;
	for(i=0; i<count; i++){
		p = ui->comboBox_account->itemData(i).value<LinphoneProxyConfig *>();
		if (p==cfg) {
			found=true;
			break;
		}
	}
	if (!found) {
		//g_warning("Could not find proxy config in combo box of identities.");
		return;
	}

	switch (rs){
		case LinphoneRegistrationOk:
			stock_id = QIcon("icons:gtk-yes.png");//GTK_STOCK_YES;
		break;
		case LinphoneRegistrationProgress:
			stock_id = this->style()->standardIcon(QStyle::SP_BrowserReload);//QIcon("icons:startcall-green.png");//GTK_STOCK_REFRESH;
		break;
		case LinphoneRegistrationCleared:
			//stock_id=NULL;
		break;
		case LinphoneRegistrationFailed:
			stock_id = QIcon("icons:gtk-no.png");//GTK_STOCK_DIALOG_WARNING;
		break;
		default:
		break;
	}
	ui->comboBox_account->setLayoutDirection(Qt::LayoutDirectionAuto);
	ui->comboBox_account->setItemIcon(i, stock_id);
}

bool MainWindow::debugWindowIsOpen(void)
{
	return _debugwindow?true:false;
}

void MainWindow::set_my_presence(LinphoneOnlineStatus ss)
{
	_presence_button->setToolTip(linphone_online_status_to_string(ss));
	_presence_button->setIcon(create_status_picture(ss));
	linphone_core_set_presence_info(linphone_qt_get_core(),0,NULL,ss);
}

void MainWindow::set_my_presence(QAction *_action)
{
	LinphoneOnlineStatus ss = _action->data().value<LinphoneOnlineStatus>();
	set_my_presence(ss);
}

void MainWindow::show_presence_menu()
{
	QCursor cur=this->cursor();
    _presence_menu->exec(cur.pos());
}

void MainWindow::setUriText(const char *uri)
{
	ui->lineEdit_Uri->setText(uri);
}

void MainWindow::close_conference()
{
	ui->tabWidget->removeTab(ui->tabWidget->indexOf(_conferenceframe));
	delete _conferenceframe;
	_conferenceframe = NULL;
}
void MainWindow::call_view_set_transfer_status(LinphoneCall *transfered, LinphoneCallState new_call_state)
{
	CallViewFrame *callview=(CallViewFrame*)linphone_call_get_user_pointer(transfered);
	if (callview){
		const char *transfer_status="unknown";
		switch(new_call_state){
			case LinphoneCallOutgoingProgress:
				transfer_status=_("Transfer in progress");
			break;
			case LinphoneCallConnected:
				transfer_status=_("Transfer done.");
			break;
			case LinphoneCallError:
				transfer_status=_("Transfer failed.");
			break;
			default:
			break;
		}
		callview->set_callstatus(transfer_status);
	}
}

void MyThread::run()
{

	int res;
	int sock = 1024;
	struct timeval tv;
	fd_set wrset;
	int valopt;
	socklen_t sock_len;

	while(1){

		tv.tv_sec = 1;
		tv.tv_usec = 0;

		FD_ZERO(&wrset);
		FD_SET(sock, &wrset);
		res = select(sock + 1, NULL, &wrset, NULL, &tv);
		w->linphone_qt_check_logs();
	}
}