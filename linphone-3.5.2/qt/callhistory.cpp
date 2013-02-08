#include "callhistory.h"
#include "ui_callhistory.h"
#include <QStandardItemModel>
#include <QStringList>
#include "HTMLDelegate.h"
#include "mainwindow.h"

static const int c_calllog_role = Qt::UserRole + 2;
Q_DECLARE_METATYPE(LinphoneCallLog *)

Callhistory::Callhistory(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Callhistory)
{
    ui->setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);
    model = new QStandardItemModel(1,1,this);
    ui->listView->setModelColumn(1);
	ui->listView->setModel(model);
	ui->listView->setSelectionRectVisible(true);
	ui->listView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    model->removeRows(0, model->rowCount(QModelIndex()), QModelIndex());
    HTMLDelegate* delegate = new HTMLDelegate();
  
    ui->listView->setModel(model);
	ui->listView->setItemDelegateForColumn(0, delegate);
	call_log_update();
}

Callhistory::~Callhistory()
{
    delete ui;
	w->history_closed();
}

void Callhistory::on_buttonOk_clicked(QAbstractButton *button)
{
	if(button == (QAbstractButton *)ui->buttonOk->button(QDialogButtonBox::Close))
	{
		this->close();
	} else {
		linphone_core_clear_call_logs(linphone_qt_get_core());
		call_log_update();
	}  
}

void Callhistory::call_log_update()
{
	const MSList *logs;
	int row=0;
	model->removeRows(0, model->rowCount(QModelIndex()), QModelIndex());
	for (logs=linphone_core_get_call_logs(linphone_qt_get_core());logs!=NULL;logs=logs->next){
		LinphoneCallLog *cl=(LinphoneCallLog*)logs->data;
		LinphoneAddress *la=cl->dir==LinphoneCallIncoming ? cl->from : cl->to;
		char *addr= linphone_address_as_string_uri_only (la);
		const char *display;
		QString logtxt;
		display=linphone_address_get_display_name (la);
		if (display==NULL){
			display=linphone_address_get_username (la);
			if (display==NULL)
				display=linphone_address_get_domain (la);
		}
		logtxt.sprintf("<table><tr><td><img src=\"%s\"></td><td><big><b>%s</b></big>\t<small><i>%s</i></small><br>"
		                               "%s\t%i minutes %i seconds</td></tr><table>",
									   cl->dir==LinphoneCallOutgoing ? "icons:go-up.png" : "icons:go-down.png",
									   display, addr, cl->start_date,
		                               cl->duration/60,cl->duration%60);
		model->insertRows(model->rowCount(), 1, QModelIndex());
		model->setData(model->index(row, 0, QModelIndex()),
                                        logtxt);
		model->setData(model->index(row, 0, QModelIndex()),
                                        QVariant::fromValue(cl),c_calllog_role);
		row++;
		ms_free(addr);
	}
}