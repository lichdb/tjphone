#include "conferenceframe.h"
#include "ui_conferenceframe.h"
#include <QProgressBar>
#include "mainwindow.h"

conferenceframe::conferenceframe(QWidget *parent)
	: QFrame(parent),
    ui(new Ui::ConferenceFrame)
{
	ui->setupUi(this);
	muted = 0;
	init_local_participant();
}

conferenceframe::~conferenceframe()
{
	sound_indicator* si;
	QHash<LinphoneCall*, sound_indicator*>::iterator it;

	killTimer(_timerid);
	for(it = _conferencealls.begin();it!=_conferencealls.end(); it++){
		si = it.value();
		if(si){
			delete si;
		}
	}
	_conferencealls.clear();
	delete ui;
}

void conferenceframe::set_in_conference(LinphoneCall *call)
{
	const LinphoneAddress *addr;
	QProgressBar *progressBar_speaker=NULL;
	QLabel *urilabel=NULL;
	sound_indicator* si;
	char *tmp;
	QString markup;
	int call_count=0;
	if(!call) return;
	if(_conferencealls.value(call)) return;
	call_count = _conferencealls.count();
	addr=linphone_call_get_remote_address(call);
	if (linphone_address_get_display_name(addr)!=NULL){
		markup.sprintf("<b>%s</b>",linphone_address_get_display_name(addr));
	}else{
		tmp=linphone_address_as_string_uri_only(addr);
		markup.sprintf("%s",tmp);
		ms_free(tmp);
	}
	si = new sound_indicator;
	/* 创建界面 */
	progressBar_speaker = new QProgressBar(this);
	progressBar_speaker->setStyleSheet("QProgressBar::chunk {background-color: grey;} QProgressBar{text-align: center;}");
	progressBar_speaker->setTextVisible(false);
	progressBar_speaker->setEnabled(true);
	progressBar_speaker->setValue(0);
	progressBar_speaker->setRange(0,100);
	QSizePolicy sizePolicy3(QSizePolicy::Expanding, QSizePolicy::Fixed);
	sizePolicy3.setHorizontalStretch(0);
	sizePolicy3.setVerticalStretch(0);
	sizePolicy3.setHeightForWidth(progressBar_speaker->sizePolicy().hasHeightForWidth());
	progressBar_speaker->setSizePolicy(sizePolicy3);
    progressBar_speaker->setAutoFillBackground(false);
	progressBar_speaker->setOrientation(Qt::Horizontal);
	progressBar_speaker->setInvertedAppearance(false);
	urilabel = new QLabel(this);
	urilabel->setText(markup);
	ui->gridLayout->addWidget(progressBar_speaker, call_count+1, 3, 1, 1);
	ui->gridLayout->addWidget(urilabel, call_count+1, 2, 1, 1);

	si->sound_meter = progressBar_speaker;
	si->speaker_last_value = 0;
	si->urilabel = urilabel;
	_conferencealls.insert(call, si);
}

void conferenceframe::init_local_participant(void)
{
	ui->label_me->setText(N_("Me"));
	ui->pushButton_muteMe->setEnabled(true);
	ui->pushButton_muteMe->show();
	_timerid = startTimer(50);
}

void conferenceframe::terminate_conference_participant(LinphoneCall *call)
{
	sound_indicator* si = NULL;
	si = _conferencealls.value(call);
	if(!si) return;
	ui->gridLayout->removeWidget(si->sound_meter);
	ui->gridLayout->removeWidget(si->urilabel);
	si->sound_meter->hide();
	si->urilabel->hide();
}

void conferenceframe::unset_from_conference(LinphoneCall *call)
{
	sound_indicator* si = NULL;
	si = _conferencealls.value(call);
	if(!si) return;
	_conferencealls.remove(call);
	delete si->sound_meter;
	delete si->urilabel;
	delete si;

	if(_conferencealls.count()==1)
	{
		w->close_conference();
	}
}


void conferenceframe::on_pushButton_mute_clicked()
{
	linphone_core_mute_mic(linphone_qt_get_core(),!muted);
	muted = !muted;
	if(muted){
		ui->pushButton_muteMe->setIcon(QIcon("icons:mic_muted.png"));
	}else{
		ui->pushButton_muteMe->setIcon(QIcon("icons:mic_active.png"));
	}
	
}

#define UNSIGNIFICANT_VOLUME (-26)
#define SMOOTH 0.15

void conferenceframe::timerEvent (QTimerEvent *event)
{
	static float speaker_last_value=0;
	QProgressBar *progressBar_speaker=NULL;
	QLabel *urilabel=NULL;
	LinphoneCall *call=NULL;
	sound_indicator* si;
	QHash<LinphoneCall*, sound_indicator*>::iterator it;
	float frac, volume_db;
	for(it = _conferencealls.begin();it!=_conferencealls.end(); it++){
		si = it.value();
		call = it.key();
		if(!call || !si) break;
		progressBar_speaker = si->sound_meter;
		urilabel = si->urilabel;
		
		volume_db=linphone_call_get_play_volume(call);
		frac=(volume_db-UNSIGNIFICANT_VOLUME)/(float)(-UNSIGNIFICANT_VOLUME+3.0);
		if (frac<0) frac=0;
		if (frac>1.0) frac=1.0;
		if (frac<si->speaker_last_value){
			frac=(frac*SMOOTH)+(si->speaker_last_value*(1-SMOOTH));
		}
		si->speaker_last_value = frac;
		progressBar_speaker->setValue(frac*100);
	}
	/* 处理本地播放音量 */
	volume_db = linphone_core_get_conference_local_input_volume(linphone_qt_get_core());
	frac=(volume_db-UNSIGNIFICANT_VOLUME)/(float)(-UNSIGNIFICANT_VOLUME+3.0);
	if (frac<0) frac=0;
	if (frac>1.0) frac=1.0;
	if (frac<speaker_last_value){
		frac=(frac*SMOOTH)+(speaker_last_value*(1-SMOOTH));
	}
	speaker_last_value = frac;
	ui->progressBar_me->setValue(frac*100);
}