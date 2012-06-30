#ifndef TREEMODEL_H
#define TREEMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <QList>
#include <QVariant>
#include <QVector>
#include <QString>
#include <QPixmap>
#include <QObject>
#include <QMap>

#include "linphone.h"
//! [0]

#include <QString>

class friend_record
{
public:
	friend_record();
	~friend_record();
    const QString& name() const {return name_;}
    void set_name(const QString& val) {name_ = val;}

    const QString& uri() const {return uri_;}
    void set_uri(const QString& val) {uri_ = val;}

	const QString& group() const {return group_;}
    void set_group(const QString& val) {group_ = val;}

    bool isgroup() const {return isgroup_;}
    void set_isgroup(bool val) {isgroup_ = val;}

    const QString& presence_status() const {return presence_status_;}
    void set_presence_status(const QString& val) {presence_status_ = val;}
    
    const LinphoneFriend* frienddata() const {return lf_;}
    void set_comment(LinphoneFriend* val) {lf_ = val;}

    //const QPixmap* friend_pixmap() const {return friend_pixmap_;}
    //void set_friend_pixmap(QPixmap* val) {friend_pixmap_ = val;}

    bool is_empty() const {return name_.isEmpty();}

    bool operator<(const friend_record& src) const;
    bool operator==(const friend_record& src) const;

private:
        QString name_;
        QString uri_;
        QString presence_status_;
		bool isgroup_;
		LinphoneFriend *lf_;
		QString group_;
		//QPixmap *friend_pixmap_;
};


class friend_list : public QObject
{
    Q_OBJECT
public:
    typedef QMap<QString, friend_record> friend_records_t;

    friend_list(QObject* parent = NULL);
    
    void add(const friend_record& rec);
	void remove_all();
    void remove(const QString& nick_name);
    void remove(const QList<QString> nick_names);
    void change(const QString& old_nick_name, const friend_record& rec);

    const friend_records_t& list() const {return friends_;}
    
    friend_record get_by_nick_name(const QString& nick_name) const;
    
signals:
    void changed();
    void added(const QString& nick_name);
    void removed(const QList<QString>& nicks);
	void removed_all();
    void changed(const QString& old_nick_name, const QString& nick_name);

private:
    void add_rec(const friend_record& rec);
    friend_records_t friends_;
};

#endif
