#ifndef CONTACT_H
#define CONTACT_H

#include <QDialog>
#include "linphone.h"

namespace Ui {
    class Contact;
}

class Contact : public QDialog
{
    Q_OBJECT

public:
    explicit Contact(QWidget *parent = 0);
    ~Contact();
	void set_uri(QString &uri);
	void set_name(QString &name);
	void set_allow_presence(bool value);
	void set_show_status(bool value);
	void set_friend(LinphoneFriend *lf);
	LinphoneFriend *get_friend(void);

private slots:
        void on_buttonBox_accepted();

        void on_buttonBox_rejected();

private:
    Ui::Contact *ui;
	LinphoneFriend *_lf;
};

#endif // CONTACT_H
