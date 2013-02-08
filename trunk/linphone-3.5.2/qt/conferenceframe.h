#ifndef CONFERENCEFRAME_H
#define CONFERENCEFRAME_H

#include <QFrame>
#include <QtGui/QPushButton>
#include "linphone.h"
#include <QHash>

namespace Ui {
    class ConferenceFrame;
}

class QProgressBar;
class QLabel;

struct __sound_indicator{
	float speaker_last_value;
	QProgressBar *sound_meter;
	QLabel	*urilabel;
};

typedef  struct __sound_indicator sound_indicator ;

class conferenceframe : public QFrame
{
	Q_OBJECT

public:
	conferenceframe(QWidget *parent = 0);
	~conferenceframe();
	void set_in_conference(LinphoneCall *call);
	void unset_from_conference(LinphoneCall *call);
	void terminate_conference_participant(LinphoneCall *call);
private slots:
	void on_pushButton_mute_clicked();

protected:
	virtual void timerEvent (QTimerEvent *event);

private:
	void init_local_participant(void);
	Ui::ConferenceFrame *ui;
	int muted;
	int _timerid;
	QHash<LinphoneCall *, sound_indicator*> _conferencealls;
};

#endif // CONFERENCEFRAME_H
