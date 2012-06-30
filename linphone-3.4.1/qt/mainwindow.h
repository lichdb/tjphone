#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "callhistory.h"
#include "debugwindow.h"
#include "preference.h"
#include "custom_tree.h"
#include "linphone.h"

class name_delegate;
namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
	Callhistory _callhistory;
	Ui::MainWindow *ui;

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void preference_closed(void);
	void qt_load_identities(void);
	void load_uri_history(void);
	void save_uri_history(void);
	void completion_add_text(QString &text);
	void show_friends(void);
	void create_in_call_view(LinphoneCall *call);
	void call_view_set_calling(LinphoneCall *call);
	void call_view_set_in_call(LinphoneCall *call);
	void enable_mute_button(LinphoneCall *call, bool sensitive);
	void enable_hold_button(LinphoneCall *call, bool sensitive, bool holdon);
	void call_view_terminate(LinphoneCall *call, QString &error_msg);
	void remove_in_call_view(LinphoneCall *call);
	void call_view_update_duration(LinphoneCall *call);
	void call_view_set_incoming(LinphoneCall *call, bool with_pause);
	void call_view_set_paused(LinphoneCall *call);
	void update_call_buttons(LinphoneCall *call);
	void enable_transfer_button(LinphoneCore *lc, bool value);
	void transfer_call(LinphoneCall *call);
	void show_status(QString &status_string);
	void in_call_starttimer();
	void in_call_stoptimer();
	void auto_answer(LinphoneCall *call);
	void update_call_log();
	void update_friend_item(QTreeWidgetItem* item, const friend_record& fr);
	void update_tree_contents(const const friend_list::friend_records_t& fl, int role, custom_tree* tree, QTreeWidgetItem* parent_item);
	void log_push(OrtpLogLevel lev, const char *fmt, va_list args);
public slots:
	void linphone_qt_iterate();
	void linphone_qt_check_logs();

private slots:
    void on_action_Quit_triggered();

    void on_actionAudio_vedio_triggered();

    void on_action_audio_only_triggered();

    void on_action_About_triggered();

    void on_action_Call_history_triggered();

    void on_action_Show_debug_window_triggered();

    void on_action_Preferences_triggered();

	void on_action_Homepage_triggered();

    void on_toolButton_call_clicked();

    void on_toolButton_Hangup_clicked();

	void dtmf_pressed_0();
	void dtmf_pressed_1();
	void dtmf_pressed_2();
	void dtmf_pressed_3();
	void dtmf_pressed_4();
	void dtmf_pressed_5();
	void dtmf_pressed_6();
	void dtmf_pressed_7();
	void dtmf_pressed_8();
	void dtmf_pressed_9();
	void dtmf_pressed_asterisk();
	void dtmf_pressed_pound();
	void dtmf_pressed_A();
	void dtmf_pressed_B();
	void dtmf_pressed_C();
	void dtmf_pressed_D();

	void dtmf_released();

        void on_comboBox_account_currentIndexChanged(int index);

        void on_actionEnable_selfview_toggled(bool arg1);

protected:
	virtual void timerEvent (QTimerEvent *event);

private:
    debugwindow _debugwindow;
    Preference *_Preference;
	custom_tree *tree_;
	int _timerid;
	int _autoanswertimerid;
	int _callouttimerid;
	LinphoneCall* _auto_answer_call;
	name_delegate* name_delegate_;

	void dtmf_play(const char *label);
	LinphoneCall* get_currently_displayed_call();
	void linphone_qt_call_terminated(LinphoneCall *call, const char *error);
	void linphone_qt_start_call_do(QString &uri);
	void enable_self_view(bool val);
};

extern MainWindow *w;

#endif // MAINWINDOW_H
