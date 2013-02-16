#include "chatroom.h"
#include "ui_chatroom.h"
#include <QPushButton>
#include "linphone.h"
#include "mainwindow.h"


ChatRoom * linphone_qt_init_chatroom(LinphoneChatRoom *cr, const char *with){
	ChatRoom *chatw;
	QString tmp;
	chatw = new ChatRoom(w);
	tmp.sprintf("Chat with %s",with);
	chatw->setWindowTitle(tmp);
	chatw->setLinphoneChatRoom(cr);
	linphone_chat_room_set_user_data(cr,w);
	chatw->setAttribute(Qt::WA_DeleteOnClose);
	chatw->show();
	return chatw;
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
	linphone_chat_room_set_tobedestroy(cr_, TRUE);
    delete ui;
}

void ChatRoom::push_text(const char *from, const char *message, QString &msgid, bool me)
{
	QTextBrowser *text = ui->textBrowser;
	QString author;
	QString msg;
	author.sprintf("%s", from);
	if(!msgid.isEmpty()){
		author += "\tmsgid:" + msgid;
	}
	msg.sprintf("%s",message);
	text->append(author);
	text->append(msg);
}


void text_received(LinphoneCore *lc, LinphoneChatRoom *room, const LinphoneAddress *from, const char *message)
{
	ChatRoom *w=(ChatRoom*)linphone_chat_room_get_user_data(room);
	if (w==NULL){
		w=linphone_qt_init_chatroom(room,linphone_address_as_string_uri_only(from));
	}
	QString msgid;
	w->push_text(linphone_address_as_string_uri_only(from), message, msgid, false);
	w->activateWindow();
	w->show();
}
void text_status(LinphoneCore *lc, LinphoneChatRoom *room, const LinphoneAddress *from, const char *callid, bool status)
{
	//ChatRoom *chatw=(ChatRoom*)linphone_chat_room_get_user_data(room);
	//if (chatw==NULL){
	//	chatw=linphone_qt_init_chatroom(room,from);
	//}
	//chatw->text_status(linphone_address_as_string_uri_only(from), callid, status);
	QString msg;
	if(!status){
		msg.sprintf(_("IM Message Send Failured ID:%s"), callid);
	}else{
		msg.sprintf(_("IM Message Send Successed ID:%s"), callid);
	}
	linphone_qt_show_status(msg.toUtf8().constData());
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
			QString msgid = linphone_chat_room_send_message(cr_,msg.toLocal8Bit());
            push_text(get_used_identity(),msg.toLocal8Bit(), msgid, true);
            
            ui->textEdit->clear();
    }
}
