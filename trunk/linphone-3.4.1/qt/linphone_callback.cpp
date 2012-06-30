#include "linphone.h"

#ifdef WIN32
LinphoneCoreVTable callback_vtable={
	linphone_qt_global_state_changed,
	linphone_qt_registration_state_changed,
	linphone_qt_call_state_changed,
	linphone_qt_notify_presence_recv,
	linphone_qt_new_unknown_subscriber,
	linphone_qt_auth_info_requested,
	linphone_qt_call_log_updated,
	linphone_qt_text_received,
	linphone_qt_dtmf_received,
	linphone_qt_refer_received,
	linphone_qt_buddy_info_updated,
	linphone_qt_notify_recv,
	linphone_qt_display_status,
	linphone_qt_display_message,
	linphone_qt_display_warning,
	linphone_qt_display_url,
	linphone_qt_show
};
#else
LinphoneCoreVTable callback_vtable={
	.show=linphone_qt_show,
	.inv_recv=linphone_qt_inv_recv,
	.bye_recv=linphone_qt_bye_recv,
	.notify_presence_recv=linphone_qt_notify_presence_recv,
	.new_unknown_subscriber=linphone_qt_new_unknown_subscriber,
	.auth_info_requested=linphone_qt_auth_info_requested,
	.display_status=linphone_qt_display_status,
	.display_message=linphone_qt_display_message,
	.display_warning=linphone_qt_display_warning,
	.display_url=linphone_qt_display_url,
	.display_question=linphone_qt_display_question,
	.call_log_updated=linphone_qt_call_log_updated,
	.text_received=linphone_qt_text_received,
	.general_state=linphone_qt_general_state,
	.refer_received=linphone_qt_refer_received,
	.buddy_info_updated=linphone_qt_buddy_info_updated,
	.notify_recv = linphone_qt_notify_recv
};
#endif


static void linphone_qt_notify_recv(LinphoneCore *lc, LinphoneCall *call, const char *from, const char *event)
{
	//linphone_qt_show_friends();
	linphone_qt_show_status(event);
}


static void linphone_qt_dtmf_received(LinphoneCore *lc, LinphoneCall *call, int dtmf)
{
	return;
}

static void linphone_qt_global_state_changed(LinphoneCore *lc, LinphoneGlobalState gstate, const char *message)
{
	return;
}

static void linphone_qt_registration_state_changed(LinphoneCore *lc, LinphoneProxyConfig *cfg, LinphoneRegistrationState rs, const char *msg)
{
	switch (rs){
		case LinphoneRegistrationOk:
			if (cfg){
				SipSetup *ss=linphone_proxy_config_get_sip_setup(cfg);
				if (ss && (sip_setup_get_capabilities(ss) & SIP_SETUP_CAP_LOGIN)){
					//linphone_gtk_exit_login_frame();
					;
				}
			}
		break;
		default:
		break;
	}
}

static void linphone_qt_show(LinphoneCore *lc)
{
	linphone_qt_show_main_window();
}

static void linphone_qt_notify_presence_recv(LinphoneCore *lc, LinphoneFriend * fid)
{
	linphone_qt_show_friends();
}

static void linphone_qt_new_unknown_subscriber(LinphoneCore *lc, LinphoneFriend *lf, const char *url)
{
	new_unknown_subscriber(lc, lf, url);
}

static void linphone_qt_auth_info_requested(LinphoneCore *lc, const char *realm, const char *username)
{
	auth_info_requested(lc, realm, username);
}

static void linphone_qt_display_status(LinphoneCore *lc, const char *status)
{
	linphone_qt_show_status(status);
}

static void linphone_qt_display_message(LinphoneCore *lc, const char *msg)
{
	display_message(msg);
}

static void linphone_qt_display_warning(LinphoneCore *lc, const char *warning)
{
	display_warning(warning);
}

static void linphone_qt_display_url(LinphoneCore *lc, const char *msg, const char *url)
{
	char richtext[4096];
	snprintf(richtext,sizeof(richtext),"%s %s",msg,url);
	display_message(richtext);
}

static void linphone_qt_call_log_updated(LinphoneCore *lc, LinphoneCallLog *cl)
{
	call_log_updated(lc, cl);
}

static void linphone_qt_refer_received(LinphoneCore *lc, const char  *refer_to)
{

}

static void linphone_qt_call_state_changed(LinphoneCore *lc, LinphoneCall *call, LinphoneCallState cs, const char *msg)
{
	switch(cs){
		case LinphoneCallOutgoingInit:
			linphone_qt_create_in_call_view (call);
		break;
		case LinphoneCallOutgoingProgress:
			linphone_qt_in_call_view_set_calling (call);
		break;
		case LinphoneCallStreamsRunning:
			linphone_qt_in_call_view_set_in_call(call);
			linphone_qt_enable_mute_button(call, true);
			in_call_timer();
		break;
		case LinphoneCallError:
			linphone_qt_in_call_view_terminate (call,msg);
		break;
		case LinphoneCallEnd:
			linphone_qt_in_call_view_terminate(call,NULL);
		break;
		case LinphoneCallIncomingReceived:
			linphone_qt_create_in_call_view (call);
			linphone_qt_in_call_view_set_incoming(call,!all_other_calls_paused (call,linphone_core_get_calls(lc)));
			if (auto_answer)  {
				linphone_call_ref(call);
				linphone_qt_auto_answer(call);
			}		
		break;
		case LinphoneCallResuming:
			linphone_qt_enable_hold_button(call,TRUE,TRUE);
			linphone_qt_in_call_view_set_in_call (call);
		break;
		case LinphoneCallPausing:
			linphone_qt_enable_hold_button(call,TRUE,FALSE);
		case LinphoneCallPausedByRemote:
			linphone_qt_in_call_view_set_paused(call);
		break;
		case LinphoneCallConnected:
			linphone_qt_enable_hold_button (call,TRUE,TRUE);
		break;
		default:
		break;
	}
	linphone_qt_update_call_buttons (call);
	
}

static void linphone_qt_buddy_info_updated(LinphoneCore *lc, LinphoneFriend *lf)
{

}

static void linphone_qt_text_received(LinphoneCore *lc, LinphoneChatRoom *room, const LinphoneAddress *from, const char *message)
{
	text_received(lc, room,from, message);
}
