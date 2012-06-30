#ifndef CUSTOM_TREE_H
#define CUSTOM_TREE_H

#include <QtGui/QTreeWidget>
#include <QAction>
#include <QContextMenuEvent>
#include "friend_list.h"

class custom_tree : public QTreeWidget
{
Q_OBJECT
public:
	custom_tree(QWidget *parent = 0);
	~custom_tree();
	friend_list* friends_;

protected:
	void contextMenuEvent ( QContextMenuEvent * event);
	//void 	paintEvent ( QPaintEvent * event );
	//void drawRow ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const;

private slots:    
    void add();
    void edit_selected();
    void remove_selected();
    void wait_for_friend();
    void chat_selected();
    //void update_bookmarks();
    void update_all();

private:
	friend_record get_selected_friend() const;
	QAction* add_action_;
    QAction* edit_action_;
    QAction* remove_action_;
    QAction* chat_action_;
    QAction* update_all_action_;
    QAction* wait_for_friend_action_;

};


#include <QStyledItemDelegate>

class QStyleOptionViewItem;

class status_item_delegate : public QStyledItemDelegate
{
public:
    status_item_delegate(QObject* parent);
    virtual void paint(QPainter* painter, const QStyleOptionViewItem& option,
                       const QModelIndex& index) const;

    virtual QSize sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const;

private:
    void next_icon(QRect& icon) const;
};


#endif // CUSTOM_TREE_H
