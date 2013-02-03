#include "callviewframe.h"
#include "ui_callviewframe.h"
#include "mainwindow.h"
#include <QMenu>


Q_DECLARE_METATYPE(LinphoneCall *)
CallViewFrame::CallViewFrame(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::CallViewFrame)
{
    ui->setupUi(this);
	this->_call = NULL;
	this->_call_index = 0;
	this->_transfer_button = NULL;
	this->_conference_button = NULL;
	this->muted = 0;
	this->holdon = 0;
	this->_mic_timerid = 0;
	this->_timerid = 0;
	this->_speaker_timerid = 0;
	ui->label_callee->setTextFormat(Qt::RichText);
	ui->pushButton_speaker->setIcon(QIcon("icons:speaker.png"));
	ui->pushButton_mute->setIcon(QIcon("icons:mic_active.png"));
	ui->pushButton_mute->setText("");
	ui->label_codec->setText("");
	ui->pushButton_speaker->setFlat(true);
	ui->progressBar_speaker->hide();
	ui->progressBar_mic->hide();
	ui->pushButton_speaker->hide();
	ui->progressBar_speaker->setStyleSheet("QProgressBar::chunk {background-color: grey;} QProgressBar{text-align: center;}");
	ui->progressBar_mic->setStyleSheet("QProgressBar::chunk {background-color: grey;} QProgressBar{text-align: center;}");
	ui->progressBar_quality_indicator->hide();
	ui->progressBar_quality_indicator->setStyleSheet("QProgressBar::chunk {background-color: blue;} QProgressBar{text-align: center;}");
	ui->progressBar_quality_indicator->setValue(0);
	ui->progressBar_quality_indicator->setToolTip(N_("Call quality rating"));
	ui->progressBar_quality_indicator->setRange(0,50);
}

CallViewFrame::~CallViewFrame()
{
	if (_transfer_button) delete _transfer_button;
    delete ui;
}

static const char *rating_to_text(float rating)
{
	if (rating>=4.0)
		return N_("good");
	if (rating>=3.0)
		return N_("average");
	if (rating>=2.0)
		return N_("poor");
	if (rating>=1.0)
		return N_("very poor");
	if (rating>=0)
		return N_("too bad");
	return N_("unavailable");
}

static const char *rating_to_color(float rating)
{
	if (rating>=4.0)
		return "green";
	else if (rating>=3.0)
		return "white";
	else if (rating>=2.0)
		return "yellow";
	else if (rating>=1.0)
		return "orange";
	else if (rating>=0)
		return "red";
	return "grey";
}


void CallViewFrame::view_refresh()
{
	int duration=linphone_call_get_duration(_call);
	QString tmp;
	int seconds=duration%60;
	int minutes=(duration/60)%60;
	int hours=duration/3600;
	tmp.sprintf("%02i::%02i::%02i",hours,minutes,seconds);
	set_duration(tmp);
	float rating=linphone_call_get_current_quality(_call);

	if (rating>=0){
		ui->progressBar_quality_indicator->setValue(rating*10);
		tmp.sprintf("%.1f (%s)",rating,rating_to_text(rating));
		ui->progressBar_quality_indicator->setFormat(tmp);
	}else{
		ui->progressBar_quality_indicator->setValue(0);
		ui->progressBar_quality_indicator->setFormat(N_("unavailable"));
	}
	tmp.sprintf("QProgressBar::chunk {background-color: %s;} QProgressBar{text-align: center;}",rating_to_color(rating)) ;
	ui->progressBar_quality_indicator->setStyleSheet(tmp);
}

void CallViewFrame::enable_hold_button(bool sensitive, bool pause)
{
	ui->pushButton_pause->setEnabled(sensitive);
	holdon = !pause;
	if(!pause){
		ui->pushButton_pause->setText(N_("Resume"));
	} else {
		ui->pushButton_pause->setText(N_("Hold"));
	}
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
	ui->label_callee->setText(peer_name);
}

/*
 * call accepted，计时器启动，更新时间等信息
 */
void CallViewFrame::in_call_starttimer()
{
	_timerid = startTimer(250);
}

void CallViewFrame::in_call_stoptimer()
{
	 killTimer(_timerid);
	 _timerid = 0;
}

#define UNSIGNIFICANT_VOLUME (-26)
#define SMOOTH 0.15

void CallViewFrame::timerEvent (QTimerEvent *event)
{
	static float mic_last_value=0;
	static float speaker_last_value=0;
	if (event->timerId() == _timerid) {
		if (_call){
			view_refresh();
		}else{
			in_call_stoptimer();
		}
	}else if(event->timerId() == _mic_timerid){
		float volume_db=linphone_call_get_record_volume(_call);
		float frac=(volume_db-UNSIGNIFICANT_VOLUME)/(float)(-UNSIGNIFICANT_VOLUME+3.0);
		if (frac<0) frac=0;
		if (frac>1.0) frac=1.0;
		if (frac<mic_last_value){
			frac=(frac*SMOOTH)+(mic_last_value*(1-SMOOTH));
		}
		mic_last_value=frac;
		ui->progressBar_mic->setValue(frac*100);	
	}else if(event->timerId() == _speaker_timerid){
		float volume_db=linphone_call_get_play_volume(_call);
		float frac=(volume_db-UNSIGNIFICANT_VOLUME)/(float)(-UNSIGNIFICANT_VOLUME+3.0);
		if (frac<0) frac=0;
		if (frac>1.0) frac=1.0;
		if (frac<speaker_last_value){
			frac=(frac*SMOOTH)+(speaker_last_value*(1-SMOOTH));
		}
		speaker_last_value=frac;
		ui->progressBar_speaker->setValue(frac*100);
	}
}

/*
 * call holdon，更新界面显示
 */
void CallViewFrame::set_paused()
{
	ui->groupBox_duration->setTitle(N_("Paused call"));
}

/*
 * call resume，更新界面显示
 */
void CallViewFrame::set_incall()
{
	ui->groupBox_duration->setTitle(N_("In call"));	
}

void CallViewFrame::call_view_enable_audio_view(bool val)
{
	if (val){
		/* 执行音量采样显示 */
		if(_mic_timerid == 0){
			_mic_timerid = startTimer(50);
		}
		if(_speaker_timerid == 0)
		{
			_speaker_timerid = startTimer(50);
		}
	}else{
		/* 停止音量采样 */
		this->killTimer(_mic_timerid);
		_mic_timerid = 0;
		this->killTimer(_speaker_timerid);
		_speaker_timerid = 0;
	}
}
/*
 * 显示call状态
 */
void CallViewFrame::set_callstatus(QString status)
{
	ui->groupBox_duration->setTitle(status);
}
QPushButton *CallViewFrame::get_transfer_button()
{
	return _transfer_button; 
}
void CallViewFrame::set_transfer_button(QPushButton *transfer_button)
{
	_transfer_button = transfer_button;
	ui->horizontalLayout->addWidget(_transfer_button);
	connect(_transfer_button, SIGNAL(pressed()), this, SLOT(transfer_button_clicked()));
}

void CallViewFrame::set_conference_button(QPushButton *conference_button)
{
	_conference_button = conference_button;
	ui->horizontalLayout->addWidget(_conference_button);
	connect(_conference_button, SIGNAL(pressed()), this, SLOT(conference_button_clicked()));
}
void CallViewFrame::conference_button_clicked()
{
	linphone_core_add_all_to_conference(linphone_qt_get_core());
	_conference_button->hide();
	return;
}

void CallViewFrame::transfer_button_clicked()
{
	QMenu *menu= new QMenu(this);
	LinphoneCore *lc=linphone_qt_get_core();
	const MSList *elem=linphone_core_get_calls(lc);
	
	for(;elem!=NULL;elem=elem->next){
		LinphoneCall *other_call=(LinphoneCall*)elem->data;
		CallViewFrame *call_view=(CallViewFrame*)linphone_call_get_user_pointer(other_call);
		if (other_call!=_call){
			//创建transfer菜单，选择transfer目标
			int call_index = call_view->_call_index;
			char *remote_uri=linphone_call_get_remote_address_as_string (other_call);
			QString text;
			text = QString(N_("Transfer to call #%1 with %2")).arg(call_index).arg(remote_uri);
			QAction *_action;
			_action = new QAction(this);
			_action->setText(text);
			_action->setData(QVariant::fromValue(other_call));
			_action->setIcon(QPixmap("icons:status-green.png"));
			free(remote_uri);
			menu->addAction(_action);
			connect(menu, SIGNAL(triggered(QAction *)), this, SLOT(transfer_call(QAction *)));
		}
	}
	QCursor cur=this->cursor();
    menu->exec(cur.pos());
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
		ui->pushButton_mute->setIcon(QIcon("icons:mic_muted.png"));
		//ui->pushButton_mute->setToolTip(N_("Unmute"));
	}else{
		ui->pushButton_mute->setIcon(QIcon("icons:mic_active.png"));
		//ui->pushButton_mute->setToolTip(N_("Mute"));
	}
	
}

/*
 * transfer当前view的call到 dest call
 */
void CallViewFrame::transfer_call(QAction *_action)
{
	LinphoneCall *dest_call = _action->data().value<LinphoneCall *>();
	if (_call) linphone_core_transfer_call_to_another(linphone_qt_get_core(),_call,dest_call);
}

void CallViewFrame::on_pushButton_pause_clicked()
{
	if(!holdon)
	{
		linphone_core_pause_call(linphone_qt_get_core(),_call);
		ui->pushButton_pause->setText(N_("Resume"));
	}else{
		linphone_core_resume_call(linphone_qt_get_core(),_call);
		ui->pushButton_pause->setText(N_("Hold"));
	}
	holdon = !holdon;
}

void CallViewFrame::show_mute_hold(bool showbutton)
{
	if(showbutton)
	{
		ui->pushButton_mute->show();
		ui->pushButton_pause->show();
		ui->progressBar_speaker->show();
		ui->progressBar_mic->show();
		ui->pushButton_speaker->show();
		ui->progressBar_quality_indicator->show();
	}else{
		ui->pushButton_mute->hide();
		ui->pushButton_pause->hide();
		ui->progressBar_speaker->hide();
		ui->progressBar_mic->hide();
		ui->pushButton_speaker->hide();
		ui->progressBar_quality_indicator->hide();
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