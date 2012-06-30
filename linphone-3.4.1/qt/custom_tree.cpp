
#include <QMenu>
#include <QtGui/QPushButton>
#include <QStyledItemDelegate>
#include <QTreeWidgetItem>
#include <QTreeWidget>
#include <QPainter>
#include <QScrollBar>
#include <QIcon>
#include <QHeaderView>
#include <QMessageBox>
#include "custom_tree.h"
#include "friend_list.h"
#include "contact.h"

static const int c_friend_role = Qt::UserRole + 3;

Q_DECLARE_METATYPE(friend_record)
Q_DECLARE_METATYPE(QAbstractItemDelegate*)

typedef struct _status_picture_tab_t{
	LinphoneOnlineStatus status;
	const char *img;
} status_picture_tab_t;

status_picture_tab_t status_picture_tab[]={
	{	LinphoneStatusOnline,		"status-green.png"	},
	{	LinphoneStatusBusy,		"status-orange.png"		},
	{	LinphoneStatusBeRightBack,	"status-orange.png"		},
	{	LinphoneStatusAway,		"status-orange.png"		},
	{	LinphoneStatusOnThePhone,	"status-orange.png"		},
	{	LinphoneStatusOutToLunch,	"status-orange.png"		},
	{	LinphoneStatusDoNotDisturb,	"status-red.png"	},
	{	LinphoneStatusMoved,		"status-orange.png"	},
	{	LinphoneStatusAltService,	"status-orange.png"	},
	{	LinphoneStatusOffline,	"status-offline.png"		},
	{	LinphoneStatusPending,	"status-offline.png"		},
	{	LinphoneStatusEnd,		NULL			},
};
typedef QMap<QString, QPixmap> status_icons;

static status_icons _status_icons;

const QPixmap &create_status_picture(LinphoneOnlineStatus ss)
{
	
	status_picture_tab_t *t=status_picture_tab;
	while(t->img!=NULL){
		if (ss==t->status){
			QString name(t->img);
			status_icons::const_iterator it = _status_icons.find(name);
			if (it != _status_icons.end())
				return it.value();
			else
				return QPixmap();
		}
		++t;
	}
	return QPixmap();
}


QList<QAction*> acts;
QAction* add_separator_action(QWidget* w, const QString& text)
{
    QAction* separator = new QAction(w);
    separator->setSeparator(true);
    separator->setText(text);
    w->addAction(separator);

    return separator;
}

custom_tree::custom_tree(QWidget *parent)
	: QTreeWidget(parent)
{
	add_action_ = new QAction(QIcon("icons:add.png"), tr("Add new friend"), this);
    connect(add_action_, SIGNAL(triggered()), SLOT(add()));

    edit_action_ = new QAction(QIcon("icons:edit.png"), tr("Edit selected friend"), this);
    connect(edit_action_, SIGNAL(triggered()), SLOT(edit_selected()));

    remove_action_ = new QAction(QIcon("icons:remove.png"), tr("Remove selected"), this);
    connect(remove_action_, SIGNAL(triggered()), SLOT(remove_selected()));

    chat_action_ = new QAction(QIcon("icons:view-refresh.png"), tr("chat whit"), this);
    connect(chat_action_, SIGNAL(triggered()), SLOT(chat_selected()));

    update_all_action_ = new QAction(QIcon("icons:download.png"), tr("Update all servers"), this);
    connect(update_all_action_, SIGNAL(triggered()), SLOT(update_all()));

    wait_for_friend_action_ = new QAction(QIcon("icons:chronometer.png"), tr("Wait for the friend"), this);
    connect(wait_for_friend_action_, SIGNAL(triggered()), SLOT(wait_for_friend()));

	

   acts << edit_action_
        << remove_action_
		<< chat_action_
        << add_separator_action(this,"")
		<< add_action_;
	
	setContextMenuPolicy( Qt::DefaultContextMenu );

	_status_icons.insert("status-green.png", QPixmap("icons:status-green.png"));
	_status_icons.insert("status-orange.png", QPixmap("icons:status-orange.png"));
	_status_icons.insert("status-red.png", QPixmap("icons:status-red.png"));
	_status_icons.insert("status-offline.png", QPixmap("icons:status-offline.png"));
	_status_icons.insert("status-green.png", QPixmap("icons:status-green.png"));

}

custom_tree::~custom_tree()
{
	delete add_action_;
	delete edit_action_;
	delete remove_action_;
	delete chat_action_;
	delete update_all_action_;
	delete wait_for_friend_action_;
}
/*
void custom_tree::paintEvent ( QPaintEvent * event )
{
	
	QBrush a;
	a.setStyle(Qt::SolidPattern);
	int i = index.row();
	if(i%2){
		a.setColor(QColor(233,233,33));
		painter->setBackground(a);
	}else{
		a.setColor(QColor(160,200,73));
		painter->setBackground(a);
	}
	//painter->setBrush(a);
	painter->setBackgroundMode(Qt::OpaqueMode);
}
*/
/*
void custom_tree::drawRow ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
	QBrush a;
	a.setStyle(Qt::SolidPattern);
	int i = index.row();
	if(i%2){
		a.setColor(QColor(233,233,33));
		painter->setBackground(a);
	}else{
		a.setColor(QColor(160,200,73));
		painter->setBackground(a);
	}
	//painter->setBrush(a);
	painter->setBackgroundMode(Qt::OpaqueMode);
	QTreeWidget::drawRow( painter, option, index );
}
*/

void custom_tree::contextMenuEvent ( QContextMenuEvent * event)
{
	friend_record item;
	QString name;
	QMenu* popMenu = new QMenu(this);
	if(itemAt(event->pos()) != NULL) //如果有item则添加"修改"菜单 [1]*
    {
		item = itemAt(event->pos())->data(1, c_friend_role).value<friend_record>();
		name = "edit ";
		name.append(item.name());
		edit_action_->setText(name);
		name = "chat with ";
		name.append(item.name());
		chat_action_->setText(name);
		name = "edit ";
		name.append(item.name());
		edit_action_->setText(name);
		name = "remove ";
		name.append(item.name());
		remove_action_->setText(name);
		popMenu->addActions(acts); 	
	}else{
		popMenu->addAction(add_action_);
	}
    popMenu->exec(QCursor::pos());
	delete popMenu;
}

void custom_tree::add()
{
	Contact contactwin(this);
	contactwin.show();
	contactwin.exec();
}
void custom_tree::chat_selected()
{
	const LinphoneFriend *lf;
	char *uri;
	QTreeWidgetItem* item = currentItem();
	friend_record currentfr;
	currentfr = item->data(1, c_friend_role).value<friend_record>();
	lf = currentfr.frienddata();
	uri=linphone_address_as_string(linphone_friend_get_address(lf));
	linphone_qt_create_chatroom(uri);
	ms_free(uri);
}

void custom_tree::update_all()
{

}

friend_record custom_tree::get_selected_friend() const
{
	QTreeWidgetItem* item = currentItem();
    if (item)
        return item->data(1, c_friend_role).value<friend_record>();
    else
        return friend_record();
}

void custom_tree::edit_selected()
{
	const LinphoneFriend *lf;
    const friend_record& fr = get_selected_friend();
    QString old_nick_name = fr.name();
	lf = fr.frienddata();
	Contact contactwin(this);
	contactwin.set_friend((LinphoneFriend *)lf);
	contactwin.show();
	contactwin.exec();
}

void custom_tree::remove_selected()
{
    QList<QString> nn;
	friend_record fr;
	const LinphoneFriend *lf;
    for (int i = 0; i < topLevelItemCount(); i++)
    {
        QTreeWidgetItem* ti = topLevelItem(i);
        if (ti->isSelected())
        {
            fr = ti->data(1, c_friend_role).value<friend_record>();
            nn.append(fr.name());
			lf = fr.frienddata();
			linphone_core_remove_friend(linphone_qt_get_core(), (LinphoneFriend *)lf);
        }
    }
    friends_->remove(nn);
	linphone_qt_show_friends();
}

void custom_tree::wait_for_friend()
{
	/*
    const friend_record& fr = get_selected_friend();
    using namespace tracking;

    // creating task
    task_t* task = new task_t(this);
    task->set_caption(tr("Wait for a friend %1").arg(fr.nick_name()));
    task->set_operation_mode(task_t::om_destroy_after_trigger);
    QUuid uid = QUuid::createUuid();
    task->set_id(uid.toString());

    // assign filter condition to the task
    condition_class_p cc(new server_filter_condition_class(context().track_ctx()));
    condition_p cond = cc->create();
    server_filter_condition* sfc = dynamic_cast<server_filter_condition*>(cond.get());
    task->set_condition(cond);

    // add regexp filter
    composite_filter* cf = dynamic_cast<composite_filter*>(sfc->filters()->root_filter().get());

    filter_class_p fc(new player_filter_class);
    filter_p flt = fc->create_filter();
    cf->add_filter(flt);
    player_filter* rf = dynamic_cast<player_filter*>(flt.get());
    rf->set_name(QUuid::createUuid().toString());
    rf->set_pattern(fr.nick_name());

    // add select action
    action_class_p ac(new select_server_action_class(context().track_ctx()));
    action_p a = ac->create();
    task->add_action(a);

    // add play sound action
    ac.reset(new play_sound_action_class(context().track_ctx()));
    a = ac->create();
    play_sound_action* psa = static_cast<play_sound_action*>(a.get());
    psa->set_sound_file(app_settings().notification_sound());
    task->add_action(a);

    // add query action
    ac.reset(new show_query_action_class(context().track_ctx()));
    a = ac->create();
    show_query_action* qa = dynamic_cast<show_query_action*>(a.get());
    qa->set_title(tr("Friend found"));
    qa->set_message(tr("Your friend <b>%nickname</b> found on the server:<br><b>%server</b>.<br><br>"
                       "Do you want to join him?"));
    task->add_action(a);

    // add connect action
    ac.reset(new connect_action_class(context().track_ctx()));
    a = ac->create();
    task->add_action(a);

    context().track_man()->add_task(task);
    task->condition()->start();
	*/
}






////////////////////////////////////////////////////////////////////////////////
// status_item_delegate

status_item_delegate::status_item_delegate(QObject* parent)
: QStyledItemDelegate(parent)
{
}

void status_item_delegate::paint(QPainter* painter, 
                                 const QStyleOptionViewItem& option,
                                 const QModelIndex& index) const
{
    //Draw base styled-item(gradient backgroud and other)
    QStyledItemDelegate::paint(painter, option, index);

    if (index.data(c_friend_role).toBool())
        return;

    //Rect for drawing icons
	friend_record sl;
    const QRect& optr = option.rect;
    //QRect icon_rect(optr.x() + 2, optr.y() + 2, optr.height() - 4, optr.height() - 4);
	
	sl = index.data(c_friend_role).value<friend_record>();
	if(sl.is_empty()) return;
	const LinphoneFriend* lf = sl.frienddata();
	LinphoneOnlineStatus status = linphone_friend_get_status(lf);
    const QPixmap& icon_status = create_status_picture(status);
    //First icon - status icon
	if(icon_status.isNull())return;
    painter->setRenderHint(QPainter::SmoothPixmapTransform, true);
	//icon_status.height();
	QRect icon_rect(optr.right()-icon_status.width()-2, optr.y() + 2, optr.height() - 4, optr.height() - 4);

    painter->drawPixmap(icon_rect, icon_status);

    //next_icon(icon_rect);

    //painter->drawPixmap(icon_rect, icon_passwd);

    //next_icon(icon_rect);

    //if (si->get_info("pure", "-1").toInt() == 0)
    //painter->drawPixmap(icon_rect, QPixmap("icons:user-identity.png"));
    
    //next_icon(icon_rect);

    //if ( !si->forbidden_gears().empty() )
    //painter->drawPixmap(icon_rect, QPixmap("icons:weapons/pistol.svg"));
}

void status_item_delegate::next_icon(QRect& icon) const
{
    icon.adjust(0-icon.width(), 0, 0-icon.width(), 0);
}

QSize status_item_delegate::sizeHint(const QStyleOptionViewItem & option,
                                     const QModelIndex & index) const
{
    QSize sz = QStyledItemDelegate::sizeHint(option, index);
    if (sz.height() < 16)
        sz.setHeight(16);
    return sz;
}
