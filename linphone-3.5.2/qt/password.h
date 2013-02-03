#ifndef PASSWORD_H
#define PASSWORD_H

#include <QDialog>
#include "linphone.h"

namespace Ui {
    class password;
}

class password : public QDialog
{
    Q_OBJECT

public:
    explicit password(QWidget *parent = 0);
    ~password();
	void set_message(QString &msg);
	void set_userid(QString &username);
	void set_authinfo(LinphoneAuthInfo *info);
	void start_timer();

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

protected:
	virtual void timerEvent(QTimerEvent *event);

private:
    Ui::password *ui;
	LinphoneAuthInfo *_info;
	int _timerid;
};

#endif // PASSWORD_H
