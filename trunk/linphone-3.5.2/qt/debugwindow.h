#ifndef DEBUGWINDOW_H
#define DEBUGWINDOW_H

#include <QDialog>
#include <QAbstractButton>
#include "linphone.h"
namespace Ui {
    class debugwindow;
}

class debugwindow : public QDialog
{
    Q_OBJECT

public:
    explicit debugwindow(QWidget *parent = 0);
    ~debugwindow();
	static void log_push(OrtpLogLevel lev, const char *fmt, va_list args);
	void check_logs();

private slots:
    void on_buttonClose_clicked(QAbstractButton *button);

private:
    Ui::debugwindow *ui;
	static void log_file(OrtpLogLevel lev, const char *msg);
	void display_log(OrtpLogLevel lev, QString &msg);
};

#endif // DEBUGWINDOW_H
