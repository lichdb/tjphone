#ifndef PREFERENCE_H
#define PREFERENCE_H

#include <QDialog>
#include <QComboBox>
#include <QAbstractButton>
#include "linphone.h"

typedef enum {
	CAP_IGNORE,
	CAP_PLAYBACK,
	CAP_CAPTURE
}DeviceCap;

namespace Ui {
    class Preference;
}

class Preference : public QDialog
{
    Q_OBJECT

public:
    explicit Preference(QWidget *parent = 0);
    ~Preference();
	void linphone_qt_show_sip_accounts();

private slots:
    void on_pushButton_add_proxy_clicked();
    void on_buttonBox_Apply_clicked(QAbstractButton *button);
	void on_comboBox_codec_view_changed(int index);
	void on_checkBox_ui_level_changed(int checkstate);

        void on_checkBox_mtu_set_toggled(bool checked);

        void on_spinBox_mtu_valueChanged(int arg1);

        void on_spinBox_sip_port_valueChanged(int arg1);

        void on_spinBox_audio_rtp_port_valueChanged(int arg1);

        void on_spinBox_video_rtp_port_valueChanged(int arg1);

        void on_radioButton_no_nat_toggled(bool checked);

        void on_radioButton_use_nat_address_toggled(bool checked);

        void on_lineEdit_nat_address_textChanged(const QString &arg1);

        void on_radioButton_use_stun_toggled(bool checked);

        void on_lineEdit_stun_server_textChanged(const QString &arg1);

        void on_comboBox_playback_device_currentIndexChanged(const QString &arg1);

        void on_comboBox_ring_device_currentIndexChanged(const QString &arg1);

        void on_comboBox_capture_device_currentIndexChanged(const QString &arg1);

        void on_pushButton_choose_file_clicked();

        void on_toolButton_play_ring_clicked();

        void on_checkBox_echo_cancelation_clicked();

        void on_comboBox_webcams_currentIndexChanged(const QString &arg1);

        void on_comboBox_video_size_currentIndexChanged(const QString &arg1);

        void on_lineEdit_displayname_textChanged(const QString &arg1);

        void on_lineEdit_username_textChanged(const QString &arg1);

        void on_lineEdit_sip_address_textChanged(const QString &arg1);

        void on_pushButton_edit_proxy_clicked();

        void on_pushButton_remove_proxy_clicked();

        void on_pushButton_up_codec_clicked();

        void on_pushButton_down_codec_clicked();

        void on_pushButton_enable_codec_clicked();

        void on_pushButton_disable_codec_clicked();

        void on_spinBox_upload_bw_valueChanged(int arg1);

        void on_spinBox_download_bw_valueChanged(int arg1);

        void on_comboBox_lang_currentIndexChanged(const QString &arg1);

        void on_comboBox_video_size_currentIndexChanged(int index);

		void on_comboBox_proto_currentIndexChanged(int index);

private:
    Ui::Preference *ui;
	void linphone_qt_fill_combo_box(QComboBox *combo, const char **devices, const char *selected, DeviceCap cap);
	void linphone_qt_fill_video_sizes(QComboBox *combo);
	void linphone_qt_init_codec_list();
	void linphone_qt_draw_codec_list(int type);
	void linphone_qt_show_codecs(const MSList *codeclist);
	void linphone_qt_fill_langs();
	void linphone_qt_ui_level_adapt();
	void parse_item(const char *item, const char *window_name, QWidget *w,  bool show);
	void linphone_qt_visibility_set(const char *hiddens, const char *window_name, QWidget *w, bool show);
	LinphoneProxyConfig* Preference::linphone_qt_get_selected_proxy_config();
	void linphone_qt_codec_move(int dir);
	void linphone_qt_select_codec(PayloadType *pt);
	void linphone_qt_codec_set_enable(bool enabled);

};

#endif // PREFERENCE_H
