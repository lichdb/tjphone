#ifndef SIP_ACCOUNT_H
#define SIP_ACCOUNT_H

#include <QDialog>
#include <QAbstractButton>
#include "linphone.h"
#include "preference.h"


namespace Ui {
    class SIP_Account;
}

class SIP_Account : public QDialog
{
    Q_OBJECT

public:
    explicit SIP_Account(QWidget *parent = 0);
    ~SIP_Account();
	void set_proxy(LinphoneProxyConfig *cfg);
	void clear_proxy();
	Preference *preferencewindow;

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
	Ui::SIP_Account *ui;
	LinphoneProxyConfig *_cfg;
    
};

#endif // SIP_ACCOUNT_H
