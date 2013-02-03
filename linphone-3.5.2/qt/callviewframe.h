#ifndef CALLVIEWFRAME_H
#define CALLVIEWFRAME_H

#include <QFrame>
#include <QtGui/QPushButton>
#include "linphone.h"

namespace Ui {
    class CallViewFrame;
}

class CallViewFrame : public QFrame
{
    Q_OBJECT
public slots:
	void transfer_button_clicked();
	void transfer_call(QAction *_action);
	void conference_button_clicked();
public:
    explicit CallViewFrame(QWidget *parent = 0);
    ~CallViewFrame();
	void enable_hold_button(bool sensitive, bool pause);
	void enable_mute_button(bool sensitive);
	void set_duration(QString &duration);
	void set_peer_name(QString &peer_name);
	void set_callstatus(QString status);
	QPushButton *get_transfer_button();
	QPushButton *get_conference_button(){ return _conference_button; };
	void set_transfer_button(QPushButton *transfer_button);
	void set_conference_button(QPushButton *conference_button);
	void show_mute_hold(bool showbutton);
	void show_answer_decline(bool showbutton);
	void view_refresh();
	void in_call_starttimer();
	void in_call_stoptimer();
	void set_incall();
	void set_paused();
	void call_view_enable_audio_view(bool val);
	LinphoneCall *_call;
	int _call_index;

private slots:
        void on_pushButton_answer_clicked();

        void on_pushButton_decline_clicked();

        void on_pushButton_mute_clicked();

        void on_pushButton_pause_clicked();
protected:
	virtual void timerEvent (QTimerEvent *event);

private:
    Ui::CallViewFrame *ui;
	QPushButton *_transfer_button;
	QPushButton *_conference_button;
	int muted;
	int holdon;
	int _timerid;
	int _mic_timerid;
	int _speaker_timerid;
};

#endif // CALLVIEWFRAME_H
