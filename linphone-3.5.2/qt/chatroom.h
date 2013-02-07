#ifndef CHATROOM_H
#define CHATROOM_H

#include <QDialog>
#include <QAbstractButton>
#include "linphone.h"

namespace Ui {
    class ChatRoom;
}

class ChatRoom : public QDialog
{
    Q_OBJECT

public:
    explicit ChatRoom(QWidget *parent = 0);
    ~ChatRoom();
	void setLinphoneChatRoom(LinphoneChatRoom *cr){cr_ = cr;};
	LinphoneChatRoom *getLinphoneChatRoom(){return cr_;};
	void ChatRoom::push_text(const char *from, const char *message, QString &msgid, bool me);

private slots:
        void on_buttonBox_rejected();

        void on_buttonBox_clicked(QAbstractButton *button);

private:
	const char* get_used_identity();
    Ui::ChatRoom *ui;
	LinphoneChatRoom *cr_;
};

#endif // CHATROOM_H
