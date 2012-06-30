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
public:
    explicit CallViewFrame(QWidget *parent = 0);
    ~CallViewFrame();
	void enable_hold_button(bool sensitive, bool pause);
	void enable_mute_button(bool sensitive);
	void set_duration(QString &duration);
	void set_peer_name(QString &peer_name);
	void set_callstatus(QString &status);
	QPushButton *get_transfer_button();
	void set_transfer_button(QPushButton *transfer_button);
	void show_mute_hold(bool showbutton);
	void show_answer_decline(bool showbutton);

	LinphoneCall *_call;
	int _call_index;

private slots:
        void on_pushButton_answer_clicked();

        void on_pushButton_decline_clicked();

        void on_pushButton_mute_clicked();

        void on_pushButton_pause_clicked();

private:
    Ui::CallViewFrame *ui;
	QPushButton *_transfer_button;
	int muted;
	int holdon;
};

#endif // CALLVIEWFRAME_H
