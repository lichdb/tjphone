#include <QtGui/QApplication>
#include <QTimer>
#include <QFont>
#include <QTextCodec>
#include <QDir>
#include "mainwindow.h"

#define VIDEOSELFVIEW_DEFAULT 1

extern "C" {

#include <sys/types.h>
#include <sys/stat.h>
#ifdef _MSC_VER
#include <io.h>
#include <direct.h>
//#include <strsafe.h>
#else
#include <unistd.h>
#endif /*_MSC_VER*/
}

#include "linphone.h"

#define LINPHONE_ICON "linphone.png"

const char *this_program_ident_string="linphone_ident_string=" LINPHONE_VERSION;


#define LINPHONE_CONFIG_DIR "linphone"
#define PACKAGE_DATA_DIR "."


#ifndef WIN32
#define CONFIG_FILE ".tjphonerc"
#else
#define CONFIG_FILE "tjphonerc"
#endif
LinphoneCore *the_core=NULL;
bool auto_answer = false;
static char _config_file[1024];
#define FACTORY_CONFIG_FILE "tjphonerc.factory"
static char _factory_config_file[1024];

QApplication *a;

static void linphone_qt_init_liblinphone(const char *config_file, const char *factory_config_file) ;
const char *linphone_get_config_file();
static const char *linphone_get_factory_config_file();
static bool verbose=1;
static char * addr_to_call = NULL;
static bool iconified=FALSE;
#ifdef WIN32
static char *workingdir=NULL;
#endif
static char *progpath=NULL;

LinphoneCore *linphone_qt_get_core(void){
	return the_core;
}

const char *linphone_get_config_file()
{
	/*try accessing a local file first if exists*/
#ifdef _MSC_VER
	if (_access(CONFIG_FILE,0) != -1){
#else
	if (access(CONFIG_FILE,F_OK)==0){
#endif
		snprintf(_config_file,sizeof(_config_file),"%s",CONFIG_FILE);
	}else{
#ifdef WIN32
		const char *appdata=getenv("APPDATA");
		if (appdata){
			snprintf(_config_file,sizeof(_config_file),"%s\\%s",appdata,LINPHONE_CONFIG_DIR);
			//CreateDirectory(_config_file,NULL);
			//_mkdir(_config_file);
			CreateDirectoryA(_config_file,NULL);
			snprintf(_config_file,sizeof(_config_file),"%s\\%s",appdata,LINPHONE_CONFIG_DIR "\\" CONFIG_FILE);
		}
#else
		const char *home=getenv("HOME");
		if (home==NULL) home=".";
		snprintf(_config_file,sizeof(_config_file),"%s/%s",home,CONFIG_FILE);
#endif
	}
	return _config_file;
}

static const char *linphone_get_factory_config_file()
{
	/*try accessing a local file first if exists*/
#ifdef _MSC_VER
	if (_access(FACTORY_CONFIG_FILE,0)!=-1){
#else
	if (access(FACTORY_CONFIG_FILE,F_OK)==0){
#endif
		snprintf(_factory_config_file,sizeof(_factory_config_file),
						 "%s",FACTORY_CONFIG_FILE);
	} else {
		char *progdir;
		
		if (progpath != NULL) {
			char *basename;
			progdir = strdup(progpath);
#ifdef WIN32
			basename = strrchr(progdir, '\\');
			if (basename != NULL) {
				basename ++;
				*basename = '\0';
				snprintf(_factory_config_file, sizeof(_factory_config_file),
								 "%s\\..\\%s", progdir, FACTORY_CONFIG_FILE);
			} else {
				if (workingdir!=NULL) {
					snprintf(_factory_config_file, sizeof(_factory_config_file),
									 "%s\\%s", workingdir, FACTORY_CONFIG_FILE);
				} else {
					free(progdir);
					return NULL;
				}
			}
#else
			basename = strrchr(progdir, '/');
			if (basename != NULL) {
				basename ++;
				*basename = '\0';
				snprintf(_factory_config_file, sizeof(_factory_config_file),
								 "%s/../share/Linphone/%s", progdir, FACTORY_CONFIG_FILE);
			} else {
				free(progdir);
				return NULL;
			}
#endif
			free(progdir);
		}
	}
	return _factory_config_file;
}



static void linphone_qt_init_liblinphone(const char *config_file,
		const char *factory_config_file) {
	linphone_core_set_user_agent("Linphone", LINPHONE_VERSION);
	the_core=linphone_core_new(&callback_vtable,config_file,factory_config_file,NULL);
	linphone_core_set_waiting_callback(the_core,linphone_qt_wait,NULL);
}

static void linphone_qt_init_main_window(MainWindow *mainw)
{
	mainw->qt_load_identities();
	//linphone_gtk_manage_login();//是否显示login窗口
	mainw->load_uri_history();//在号码栏位显示历史uri
	//linphone_gtk_set_my_presence(linphone_core_get_presence_info(linphone_gtk_get_core()));显示在线信息
	mainw->show_friends();//显示好友
	//linphone_gtk_connect_digits();
	//linphone_gtk_check_menu_items();
	/*main_window=linphone_gtk_get_main_window();
	linphone_gtk_enable_mute_button(GTK_BUTTON(linphone_gtk_get_widget(main_window,
					"main_mute")),FALSE);
	if (!linphone_gtk_use_in_call_view()) {
		gtk_widget_show(linphone_gtk_get_widget(main_window, "main_mute"));
	}
	linphone_gtk_update_call_buttons (NULL);
	*/
	/*prevent the main window from being destroyed by a user click on WM controls, instead we hide it*/
	//g_signal_connect (G_OBJECT (main_window), "delete-event",
	//	G_CALLBACK (linphone_gtk_close), main_window);
}


void qt_log_handler(OrtpLogLevel lev, const char *fmt, va_list args){
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
		fprintf(stdout,"tjphone-%s : %s\n",lname,msg);
	}
	//display log in dialog
	w->log_push(lev,fmt,args);
}

int main(int argc, char *argv[])
{
    int result;
	const char *config_file;
	const char *factory_config_file;
	int trace_level = 1;

#ifdef WIN32
	if (workingdir!=NULL)
		_chdir(workingdir);
#endif

	a = new QApplication(argc, argv);
	QDir::addSearchPath("icons", QString(":icons/icons/"));
    QDir::addSearchPath("images", QString(":images/icons/"));
	config_file=linphone_get_config_file();
	factory_config_file = linphone_get_factory_config_file();
/*	if (linphone_core_wake_up_possible_already_running_instance(
		config_file, addr_to_call) == 0){
		
		//g_message("addr_to_call=%s",addr_to_call);
		//g_warning("Another running instance of linphone has been detected. It has been woken-up.");
		//g_warning("This instance is going to exit now.");
		//gdk_threads_leave();
		return 0;
	}
*/
	linphone_core_enable_logs_with_cb(qt_log_handler);
	linphone_qt_init_liblinphone(config_file, factory_config_file);

	QTextCodec *tl=QTextCodec::codecForName("utf8");
	QTextCodec::setCodecForLocale(tl);
	ortp_set_log_level_mask(ORTP_WARNING|ORTP_ERROR);
	if (trace_level > 0)
	{
		ortp_set_log_level_mask(ORTP_WARNING|ORTP_ERROR);
	}
	else
	{
		linphone_core_disable_logs();
	}

	w = new MainWindow();
	QFont font("simsun",12,QFont::Normal,FALSE);
	a->setFont(font);
	
	QTimer *timer_iterate = new QTimer(a);
	a->connect(timer_iterate,SIGNAL(timeout()),w,SLOT(linphone_qt_iterate()));
	timer_iterate->start(30);
	QTimer *timer_logs = new QTimer(a);
	bool test = a->connect(timer_logs,SIGNAL(timeout()),w,SLOT(linphone_qt_check_logs()));
	timer_logs->start(30);
	linphone_qt_init_main_window(w);
/*
	linphone_gtk_init_main_window();
	linphone_gtk_log_show();
	linphone_gtk_init_status_icon();
*/
    w->show();
    result = a->exec();

	linphone_core_destroy(the_core);
	timer_iterate->stop();
	timer_logs->stop();
    delete w;
    delete a;
    return result;
}

void * linphone_qt_wait(LinphoneCore *lc, void *ctx, LinphoneWaitingState ws, const char *purpose, float progress)
{
	/*显示进度条*/
	/* 暂不显示
		switch(ws){
		case LinphoneWaitingStart:
			gdk_threads_enter();
			w=linphone_gtk_create_window("waiting");
			gtk_window_set_transient_for(GTK_WINDOW(w),GTK_WINDOW(linphone_gtk_get_main_window()));
			gtk_window_set_position(GTK_WINDOW(w),GTK_WIN_POS_CENTER_ON_PARENT);
			if (purpose) {
				gtk_progress_bar_set_text(
					GTK_PROGRESS_BAR(linphone_gtk_get_widget(w,"progressbar")),
					purpose);
			}
			gtk_widget_show(w);
			run_gtk();
			gdk_threads_leave();
			return w;
		break;
		case LinphoneWaitingProgress:
			w=(GtkWidget*)ctx;
			gdk_threads_enter();
			if (progress>=0){
				gtk_progress_bar_set_fraction(
					GTK_PROGRESS_BAR(linphone_gtk_get_widget(w,"progressbar")),
					progress);
				
				
			}else {
				gtk_progress_bar_pulse(
					GTK_PROGRESS_BAR(linphone_gtk_get_widget(w,"progressbar"))
				);
			}
			run_gtk();
			gdk_threads_leave();
			g_usleep(50000);
			return w;
		break;
		case LinphoneWaitingFinished:
			w=(GtkWidget*)ctx;
			gdk_threads_enter();
			gtk_widget_destroy(w);
			run_gtk();
			gdk_threads_leave();
			return NULL;
		break;
	}
	*/
	return NULL;
}

