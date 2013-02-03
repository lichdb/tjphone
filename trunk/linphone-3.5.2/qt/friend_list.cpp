#include <QStringList>
//#include <QtGui>
#include "friend_list.h"

friend_record::friend_record()
{
		isgroup_ = false;
		lf_ = NULL;
}

friend_record::~friend_record()
{
}

bool friend_record::operator<(const friend_record& src) const
{
    return name_ < src.name_;
}

bool friend_record::operator==(const friend_record& src) const
{
    return name_ == src.name_;
}

friend_list::friend_list(QObject* parent)
: QObject(parent)
{
}

void friend_list::add_rec(const friend_record& rec)
{
    friends_[rec.name()] = rec;
}

void friend_list::add(const friend_record& rec)
{
    add_rec(rec);
    emit added(rec.name());
    emit changed();
}

void friend_list::remove(const QString& nick_name)
{
    QList<QString> nn;
    nn.append(nick_name);
    remove(nn);
}
void friend_list::remove_all()
{	
	friends_.clear();
	emit removed_all();
}
void friend_list::remove(const QList<QString> nick_names)
{
    QList<QString> removed_nicks;
    foreach (const QString& nn, nick_names)
    {
        if (friends_.remove(nn))
        {
            removed_nicks.append(nn);
        }
    }
    if (removed_nicks.size() > 0)
    {
        emit changed();
        emit removed(removed_nicks);
    }
}

friend_record friend_list::get_by_nick_name(const QString& nick_name) const
{
    friend_records_t::const_iterator it = friends_.find(nick_name);
    if (it != friends_.end())
        return it.value();
    else
        return friend_record();
}

void friend_list::change(const QString& old_nick_name, const friend_record& rec)
{
    if (old_nick_name != rec.name())
        friends_.remove(old_nick_name);
    friends_[rec.name()] = rec;
    emit changed(old_nick_name, rec.name());
    emit changed();
}

