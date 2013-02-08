#include <QMessageBox>

#include "linphone.h"
#include "lpconfig.h"
#include "mainwindow.h"
#include "contact.h"
#include "password.h"


//static GList *pixmaps_directories = NULL;

/* Use this function to set the directory containing installed pixmaps. */
/*
void
add_pixmap_directory                   (const gchar     *directory)
{
  pixmaps_directories = g_list_prepend (pixmaps_directories,
                                        g_strdup (directory));
}
*/

/* This is an internally used function to find pixmap files. */
/*
static gchar*
find_pixmap_file                       (const gchar     *filename)
{
  GList *elem;

  // We step through each of the pixmaps directory to find it. 
  elem = pixmaps_directories;
  while (elem)
    {
      gchar *pathname = g_strdup_printf ("%s%s%s", (gchar*)elem->data,
                                         G_DIR_SEPARATOR_S, filename);
      if (g_file_test (pathname, G_FILE_TEST_EXISTS))
        return pathname;
      g_free (pathname);
      elem = elem->next;
    }
  return NULL;
}
*/


/* This is an internally used function to create pixmaps. */
/*
GtkWidget*
create_pixmap                          (const gchar     *filename)
{
  gchar *pathname = NULL;
  GtkWidget *pixmap;

  if (!filename || !filename[0])
      return gtk_image_new ();

  pathname = find_pixmap_file (filename);

  if (!pathname)
    {
      g_warning (_("Couldn't find pixmap file: %s"), filename);
      return gtk_image_new ();
    }

  pixmap = gtk_image_new_from_file (pathname);
  g_free (pathname);
  return pixmap;
}
*/

/* This is an internally used function to create pixmaps. */
/*
GdkPixbuf*
create_pixbuf                          (const gchar     *filename)
{
  gchar *pathname = NULL;
  GdkPixbuf *pixbuf;
  GError *error = NULL;

  if (!filename || !filename[0])
      return NULL;

  pathname = find_pixmap_file (filename);

  if (!pathname)
    {
      g_warning (_("Couldn't find pixmap file: %s"), filename);
      return NULL;
    }

  pixbuf = gdk_pixbuf_new_from_file (pathname, &error);
  if (!pixbuf)
    {
      fprintf (stderr, "Failed to load pixbuf file: %s: %s\n",
               pathname, error->message);
      g_error_free (error);
    }
  g_free (pathname);
  return pixbuf;
}
*/

/* This is an internally used function to create animations */
/*
GdkPixbufAnimation *
create_pixbuf_animation(const gchar     *filename)
{
	gchar *pathname = NULL;
	GdkPixbufAnimation *pixbuf;
	GError *error = NULL;
	
	if (!filename || !filename[0])
		return NULL;
	
	pathname = find_pixmap_file (filename);
	
	if (!pathname){
		g_warning (_("Couldn't find pixmap file: %s"), filename);
		return NULL;
	}
	
	pixbuf = gdk_pixbuf_animation_new_from_file (pathname, &error);
	if (!pixbuf){
		fprintf (stderr, "Failed to load pixbuf file: %s: %s\n",
			pathname, error->message);
		g_error_free (error);
	}
	g_free (pathname);
	return pixbuf;
}
*/


/* This is used to set ATK action descriptions. */
/*
void
glade_set_atk_action_description       (AtkAction       *action,
                                        const gchar     *action_name,
                                        const gchar     *description)
{
  gint n_actions, i;

  n_actions = atk_action_get_n_actions (action);
  for (i = 0; i < n_actions; i++)
    {
      if (!strcmp (atk_action_get_name (action, i), action_name))
        atk_action_set_description (action, i, description);
    }
}
*/

static char linphone_lang[256]={0};

/*lang has to be read before the config file is parsed...*/
const char *linphone_qt_get_lang(const char *config_file){
	FILE *f=fopen(config_file,"r");
	if (f){
		char tmp[256];
		while(fgets(tmp,sizeof(tmp),f)!=NULL){
			char *p;
			if ((p=strstr(tmp,"lang="))!=NULL){
				p+=5;
				sscanf(p,"%s",linphone_lang);
				//g_message("Found lang %s",linphone_lang);
				break;
			}
		}
		fclose(f);
	}
	return linphone_lang;
}

void linphone_qt_set_lang(const char *code)
{
	LpConfig *cfg=linphone_core_get_config(linphone_qt_get_core());
	const char *curlang;
	char tmp[128];
	curlang=getenv("LANG");
	if (curlang!=NULL && strncmp(curlang,code,2)==0) {
		/* do not loose the _territory@encoding part*/
		return;
	}
	lp_config_set_string(cfg,"QtUi","lang",code);
#ifdef WIN32
	snprintf(tmp,sizeof(tmp),"LANG=%s",code);
	_putenv(tmp);
#else
	setenv("LANG",code,1);
#endif
}

const char *linphone_qt_get_ui_config(const char *key, const char *def)
{
	LinphoneCore *lc=linphone_qt_get_core();
	if (lc){
		LpConfig *cfg=linphone_core_get_config(linphone_qt_get_core());
		return lp_config_get_string(cfg,"QtUi",key,def);
	}else{
		//g_error ("Cannot read config, no core created yet.");
		return NULL;
	}
}

int linphone_qt_get_ui_config_int(const char *key, int def){
	LpConfig *cfg=linphone_core_get_config(linphone_qt_get_core());
	return lp_config_get_int(cfg,"QtUi",key,def);
}

void linphone_qt_set_ui_config_int(const char *key , int val){
	LpConfig *cfg=linphone_core_get_config(linphone_qt_get_core());
	lp_config_set_int(cfg,"QtUi",key,val);
}

void linphone_qt_set_ui_config(const char *key , const char * val){
	LpConfig *cfg=linphone_core_get_config(linphone_qt_get_core());
	lp_config_set_string(cfg,"QtUi",key,val);
}

gboolean linphone_qt_use_in_call_view()
{
	static int val=-1;
	if (val==-1) val=linphone_qt_get_ui_config_int("use_incall_view",1);
	return val;
}
void linphone_qt_in_call_view_set_transfer_status(LinphoneCall *transfered, LinphoneCallState new_call_state)
{
	w->call_view_set_transfer_status(transfered, new_call_state);
}


void linphone_qt_create_in_call_view(LinphoneCall *call)
{
	w->create_in_call_view(call);
}

void linphone_qt_in_call_view_set_calling(LinphoneCall *call)
{
	if(!w) return;
	w->call_view_set_calling(call);
}

void linphone_qt_in_call_view_set_in_call(LinphoneCall *call)
{
	w->call_view_set_in_call(call);
}

QString get_peer_name(const LinphoneAddress *from)
{
	const char *displayname=NULL;
	const char *id;
	QString a;
	displayname=linphone_address_get_display_name(from);
	id=linphone_address_as_string_uri_only(from);
	if (displayname!=NULL)
	{
		a.sprintf("<span size=\"large\">%s</span>\n<i>%s</i>", displayname,id);
	} else {
		a.sprintf("<span size=\"large\"><i>%s</i></span>\n",id);
	}
	return a;
}

void linphone_qt_enable_mute_button(LinphoneCall *call, bool sensitive)
{
	if(!w) return;
	w->enable_mute_button(call, sensitive);
}
void linphone_qt_enable_hold_button(LinphoneCall *call, bool sensitive, bool holdon)
{
	if(!w) return;
	w->enable_hold_button(call, sensitive, holdon);
}

void linphone_qt_in_call_view_terminate(LinphoneCall *call, const char *error_msg)
{
	if(!w) return;
	QString a(error_msg);
	w->call_view_terminate(call, a);
}

static void on_call_updated_response(LinphoneCall *call){
	if (linphone_call_get_state(call)==LinphoneCallUpdatedByRemote){
		LinphoneCore *lc=linphone_call_get_core(call);
		LinphoneCallParams *params=linphone_call_params_copy(linphone_call_get_current_params(call));
		linphone_call_params_enable_video(params,TRUE);
		linphone_core_accept_call_update(lc,call,params);
		linphone_call_params_destroy(params);
	}
	linphone_call_unref(call);
}

void linphone_qt_call_updated_by_remote(LinphoneCall *call)
{
	LinphoneCore *lc=linphone_call_get_core(call);
	const LinphoneVideoPolicy *pol=linphone_core_get_video_policy(lc);
	const LinphoneCallParams *rparams=linphone_call_get_remote_params(call);
	const LinphoneCallParams *current_params=linphone_call_get_current_params(call);
	gboolean video_requested=linphone_call_params_video_enabled(rparams);
	gboolean video_used=linphone_call_params_video_enabled(current_params);
	//g_message("Video used=%i, video requested=%i, automatically_accept=%i",
	//          video_used,video_requested,pol->automatically_accept);
	if (video_used==FALSE && video_requested && !pol->automatically_accept){
		linphone_core_defer_call_update(lc,call);
		{
			const LinphoneAddress *addr=linphone_call_get_remote_address(call);
			const char *dname=linphone_address_get_display_name(addr);
			if (dname==NULL) dname=linphone_address_get_username(addr);
			if (dname==NULL) dname=linphone_address_get_domain(addr);
			/*dialog=gtk_message_dialog_new(GTK_WINDOW(linphone_gtk_get_main_window()),
			                                         GTK_DIALOG_DESTROY_WITH_PARENT,
			                                         GTK_MESSAGE_WARNING,
			                                         GTK_BUTTONS_YES_NO,
			                                         _("%s proposed to start video. Do you accept ?"),dname);
			g_signal_connect(G_OBJECT(dialog),"response",(GCallback)on_call_updated_response,linphone_call_ref(call));
			g_timeout_add(20000,(GSourceFunc)on_call_updated_timeout,dialog);
			gtk_widget_show(dialog);
			*/
			on_call_updated_response(call);
		}
	}
}
void linphone_qt_remove_in_call_view(LinphoneCall *call)
{
	if(!w) return;
	w->remove_in_call_view(call);
}

bool in_call_view_terminated(LinphoneCall *call)
{
	if(!w) return false;
	w->remove_in_call_view(call);
	return false;
}

void linphone_qt_in_call_view_set_incoming(LinphoneCall *call)
{
	if(!w) return;
	w->call_view_set_incoming(call);
}

bool all_other_calls_paused(LinphoneCall *refcall, const MSList *calls)
{
	for(;calls!=NULL;calls=calls->next){
		LinphoneCall *call=(LinphoneCall*)calls->data;
		LinphoneCallState cs=linphone_call_get_state(call);
		if (refcall!=call){
			if (cs!=LinphoneCallPaused  && cs!=LinphoneCallPausing)
				return false;
		}
	}
	return true;
}

void linphone_qt_in_call_view_set_paused(LinphoneCall *call)
{
	if(!w) return;
	w->call_view_set_paused(call);
}

void linphone_qt_update_call_buttons(LinphoneCall *call)
{
	if(!w) return;
	w->update_call_buttons(call);
}

void linphone_qt_enable_transfer_button(LinphoneCore *lc, bool value)
{
	if(!w) return;
	w->enable_transfer_button(lc, value);
}

void linphone_qt_enable_conference_button(LinphoneCore *lc, bool value)
{
	if(!w) return;
	w->enable_conference_button(lc, value);
}

void linphone_qt_show_friends(LinphoneCore *lc, LinphoneFriend * fid)
{
	if(!w) return;
	w->update_friends(lc, fid);
}
void linphone_qt_show_main_window()
{
	return;
}

void new_unknown_subscriber(LinphoneCore *lc, LinphoneFriend *lf, const char *url)
{
	QString message;

	if (linphone_qt_get_ui_config_int("subscribe_deny_all",0)){
		linphone_core_reject_subscriber(linphone_qt_get_core(),lf);
		return;
	}

	message.sprintf("%s would like to add you to his contact list.\nWould you allow him to see your presence status or add him to your contact list ?\nIf you answer no, this person will be temporarily blacklisted.",url);
	int ret = QMessageBox::warning(NULL, "New unknown subscriber", message, QMessageBox::Yes | QMessageBox::No);
	if(ret == QMessageBox::Yes)
	{
		linphone_qt_show_contact(lf);
	} else {
		linphone_core_reject_subscriber(linphone_qt_get_core(),lf);
	}
}

void linphone_qt_show_contact(LinphoneFriend *lf)
{
	Contact *contactw= new Contact(w);
	contactw->set_friend(lf);
	contactw->show();
	//delete contactw;
}

void linphone_qt_show_status(const char *status)
{
	if(!w) return;
	QString status_string(status);
	w->show_status(status_string);
}

void display_message(const char *msg)
{
	QString str_msg(msg);
	QMessageBox::information(NULL, "Infomation", str_msg);
}

void display_warning(const char *msg)
{
	QString str_msg(msg);
	QMessageBox::warning(NULL, "Warning", str_msg);
}

void in_call_timer(LinphoneCall *call)
{
	if(!w) return;
	w->in_call_starttimer(call);
}



void auth_info_requested(LinphoneCore *lc, const char *realm, const char *username)
{
	password *passw = new password(w);
	LinphoneAuthInfo *info;
	QString msg;
	
	//if (mw && GTK_WIDGET_VISIBLE(linphone_gtk_get_widget(mw,"login_frame"))){
		/*don't prompt for authentication when login frame is visible*/
	//	linphone_core_abort_authentication(lc,NULL);
	//	return;
	//}

	msg.sprintf("Please enter your password for username <i>%s</i>\n at domain <i>%s</i>:",username,realm);
	passw->set_message(msg);
	QString a(username);
	passw->set_userid(a);

	info=linphone_auth_info_new(username, NULL, NULL, NULL,realm);
	passw->set_authinfo(info);
	passw->start_timer();
	passw->show();
}


void linphone_qt_auto_answer(LinphoneCall *call)
{
	if(!w) return;
	w->auto_answer(call);
}

void call_log_updated(LinphoneCore *lc, LinphoneCallLog *cl)
{
	if(!w) return;
	w->update_call_log();
}

void update_registration_status(LinphoneProxyConfig *cfg, LinphoneRegistrationState rs)
{
	w->update_registration_status(cfg, rs);
}

void qt_log_handler(OrtpLogLevel lev, const char *fmt, va_list args)
{
	if (verbose){
		const char *lname="undef";
		char msg[1024];
	#if defined(__linux) || defined(__APPLE__)
		va_list cap;/*copy of our argument list: a va_list cannot be re-used (SIGSEGV on linux 64 bits)*/
		#endif
		switch(lev){
			case ORTP_DEBUG:
				lname="debug";
				break;
			case ORTP_MESSAGE:
				lname="message";
				break;
			case ORTP_WARNING:
				lname="warning";
				break;
			case ORTP_ERROR:
				lname="error";
				break;
			case ORTP_FATAL:
				lname="fatal";
				break;
			default:
				//g_error("Bad level !");
				;
		}
#if defined(__linux) || defined(__APPLE__)
		va_copy(cap,args);
		vsprintf(msg,fmt,cap);
		va_end(cap);
#else
		//msg=g_strdup_vprintf(fmt,args);
		vsprintf(msg,fmt,args);
#endif
		//fprintf(stdout,"tjphone-%s : %s\n",lname,msg);
	}
	//display log in dialog
	w->log_push(lev,fmt,args);
}

void linphone_qt_set_my_presence(LinphoneOnlineStatus ss)
{
	w->set_my_presence(ss);
}

void linphone_qt_start_call(LinphoneCore *lc, const char  *refer_to)
{
	w->setUriText(refer_to);
	w->on_toolButton_call_clicked();
}