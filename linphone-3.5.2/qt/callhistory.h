#ifndef CALLHISTORY_H
#define CALLHISTORY_H

#include <QDialog>
#include <QAbstractButton>
#include <QAbstractItemModel>
#include "linphone.h"

namespace Ui {
    class Callhistory;
}

class Callhistory : public QDialog
{
    Q_OBJECT

public:
    explicit Callhistory(QWidget *parent = 0);
    ~Callhistory();
	void call_log_update();

private slots:
    void on_buttonOk_clicked(QAbstractButton *button);

private:
    Ui::Callhistory *ui;
    QAbstractItemModel *model;
};

#endif // CALLHISTORY_H
