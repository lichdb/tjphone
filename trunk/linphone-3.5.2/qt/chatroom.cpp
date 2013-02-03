#include "chatroom.h"
#include "ui_chatroom.h"
#include <QPushButton>
#include "linphone.h"

ChatRoom * linphone_qt_init_chatroom(LinphoneChatRoom *cr, const char *with){
	ChatRoom *w;
	QString tmp;
	w = new ChatRoom();
	tmp.sprintf("Chat with %s",with);
	w->setWindowTitle(tmp);
	w->setLinphoneChatRoom(cr);
	linphone_chat_room_set_user_data(cr,w);
	w->setAttribute(Qt::WA_DeleteOnClose);
	w->show();
	return w;
}

void linphone_qt_create_chatroom(const char *with){
	LinphoneChatRoom *cr=linphone_core_create_chat_room(linphone_qt_get_core(),with);
	if (!cr) return;
	linphone_qt_init_chatroom(cr,with);
}



ChatRoom::ChatRoom(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChatRoom)
{
	QPushButton *button;
    ui->setupUi(this);
	button = ui->buttonBox->button(QDialogButtonBox::Ok);
	button->setText("Send");
}

ChatRoom::~ChatRoom()
{
	linphone_chat_room_destroy(cr_);
    delete ui;
}

void ChatRoom::push_text(const char *from, const char *message, bool me)
{
	QTextBrowser *text = ui->textBrowser;
	QString author;
	QString msg;
	author.sprintf("%s:\t", from);
	msg.sprintf("%s",message);
	text->append(from);
	text->append(msg);
}


void text_received(LinphoneCore *lc, LinphoneChatRoom *room, const LinphoneAddress *from, const char *message)
{
	ChatRoom *w=(ChatRoom*)linphone_chat_room_get_user_data(room);
	if (w==NULL){
		w=linphone_qt_init_chatroom(room,linphone_address_as_string_uri_only(from));
	}
	w->push_text(linphone_address_as_string_uri_only(from),message,false);
	w->activateWindow();
	w->show();
}

void ChatRoom::on_buttonBox_rejected()
{
	linphone_chat_room_set_user_data(cr_, NULL);
	this->close();
	return;
}

const char* ChatRoom::get_used_identity()
{
	LinphoneCore *lc=linphone_qt_get_core();
	LinphoneProxyConfig *cfg;
	linphone_core_get_default_proxy(lc,&cfg);
	if (cfg) return linphone_proxy_config_get_identity(cfg);
	else return linphone_core_get_primary_contact(lc);
}


void ChatRoom::on_buttonBox_clicked(QAbstractButton *button)
{
	//if(button->text()!="send") return;
    QString msg = ui->textEdit->toPlainText();
    if (msg.length()>0) {
            push_text(get_used_identity(),msg.toLocal8Bit(),true);
            linphone_chat_room_send_message(cr_,msg.toLocal8Bit());
            ui->textEdit->clear();
    }
}
