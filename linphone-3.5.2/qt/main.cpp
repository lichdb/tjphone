#include <QtGui/QApplication>
#include <QTimer>
#include <QFont>
#include <QTextCodec>
#include <QDir>
#include "mainwindow.h"
#include <glib.h>

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
#define FACTORY_CONFIG_FILE "tjphonerc.factory"
static char _factory_config_file[1024];

QApplication *a;

static void linphone_qt_init_liblinphone(const char *config_file, const char *factory_config_file) ;
const char *linphone_get_config_file(const char* filename);
static const char *linphone_get_factory_config_file();
bool verbose=0;
static char * addr_to_call = NULL;
static bool iconified=FALSE;
static char *workingdir=NULL;
static char *progpath=NULL;

#ifdef WIN32
#define chdir _chdir
#endif

char *linphone_logfile=NULL;
char *linphone_cfgfile=NULL;

#ifdef _MSC_VER
static GOptionEntry linphone_options[]={
	{
		"verbose",
		'\0',
		0,
		G_OPTION_ARG_NONE,
		(gpointer)&verbose,
		N_("log to stdout some debug information while running."),
		NULL
	},
	{
	    "logfile",
		'l',
	    0,
		G_OPTION_ARG_STRING,
		&linphone_logfile,
		N_("path to a file to write logs into."),
		NULL
	},
	{
	    "configfile",
		'f',
	    0,
		G_OPTION_ARG_STRING,
		&linphone_cfgfile,
		N_("path to the tjpphonerc config file."),
		NULL
	},
	{
		"iconified",
		'\0',
		0,
		G_OPTION_ARG_NONE,
		(gpointer)&iconified,
		N_("Start only in the system tray, do not show the main interface."),
		NULL
	},
	{
	    "call",
	    'c',
		0,
	    G_OPTION_ARG_STRING,
	    &addr_to_call,
	    N_("address to call right now"),
		NULL
	},
	{
	    "auto-answer",
	    'a',
		0,
	    G_OPTION_ARG_NONE,
	    (gpointer) & auto_answer,
	    N_("if set automatically answer incoming calls"),
		NULL
	},
	{
	    "workdir",
	    '\0',
		0,
	    G_OPTION_ARG_STRING,
	    (gpointer) & workingdir,
	    N_("Specifiy a working directory (should be the base of the installation, eg: c:\\Program Files\\Linphone)"),
		NULL
	},
	{0}
};
#else
static GOptionEntry linphone_options[]={
	{
		.long_name="verbose",
		.short_name= '\0',
		.arg=G_OPTION_ARG_NONE,
		.arg_data= (gpointer)&verbose,
		.description=N_("log to stdout some debug information while running.")
	},
	{
	    .long_name = "logfile",
	    .short_name = 'l',
	    .arg = G_OPTION_ARG_STRING,
	    .arg_data = &linphone_logfile,
	    .description = N_("path to a file to write logs into.")
	},
	{
		.long_name = "configfile",
	    .short_name = 'f',
	    .arg = G_OPTION_ARG_STRING,
	    .arg_data = &linphone_cfgfile,
	    .description = N_("path to the tjpphonerc config file.")
	},
	{
		.long_name="iconified",
		.short_name= '\0',
		.arg=G_OPTION_ARG_NONE,
		.arg_data= (gpointer)&iconified,
		.description=N_("Start only in the system tray, do not show the main interface.")
	},
	{
	    .long_name = "call",
	    .short_name = 'c',
	    .arg = G_OPTION_ARG_STRING,
	    .arg_data = &addr_to_call,
	    .description = N_("address to call right now")
	},
	{
	    .long_name = "auto-answer",
	    .short_name = 'a',
	    .arg = G_OPTION_ARG_NONE,
	    .arg_data = (gpointer) & auto_answer,
	    .description = N_("if set automatically answer incoming calls")
	},
	{
	    .long_name = "workdir",
	    .short_name = '\0',
	    .arg = G_OPTION_ARG_STRING,
	    .arg_data = (gpointer) & workingdir,
	    .description = N_("Specifiy a working directory (should be the base of the installation, eg: c:\\Program Files\\Linphone)")
	},
	{0}
};
#endif

LinphoneCore *linphone_qt_get_core(void){
	return the_core;
}

const char *linphone_get_config_file(const char* filename)
{
	const int path_max=1024;
	char *config_file=(char *)malloc(path_max);
	if(config_file){
		memset(config_file, 0, path_max);
	} else {
		return NULL;
	}
	if (filename==NULL) filename = CONFIG_FILE;
	/*try accessing a local file first if exists*/
#ifdef _MSC_VER
	if (_access(filename,0) != -1){
#else
	if (access(filename,F_OK)==0){
#endif
		snprintf(config_file,path_max,"%s",filename);
	}else{
#ifdef WIN32
		const char *appdata=getenv("APPDATA");
		if (appdata){
			snprintf(config_file,path_max,"%s\\%s",appdata,LINPHONE_CONFIG_DIR);
			CreateDirectoryA(config_file,NULL);
			snprintf(config_file,path_max,"%s\\%s",appdata,LINPHONE_CONFIG_DIR "\\" CONFIG_FILE);
		}
#else
		const char *home=getenv("HOME");
		if (home==NULL) home=".";
		snprintf(config_file,path_max,"%s/%s",home,CONFIG_FILE);
#endif
	}
	return config_file;
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
	
	linphone_core_set_user_agent("TJphone", LINPHONE_VERSION);
	the_core=linphone_core_new(&callback_vtable,config_file,factory_config_file,NULL);
	linphone_core_set_waiting_callback(the_core,linphone_qt_wait,NULL);
	/*如果要支持zrtp，需要增加 */ 
	/* linphone_core_set_zrtp_secrets_file(the_core,secrets_file);*/
}

static void linphone_qt_init_main_window(MainWindow *mainw)
{
	mainw->qt_load_identities();
	//linphone_gtk_manage_login();//是否显示login窗口
	mainw->load_uri_history();//在号码栏位显示历史uri
	mainw->show_friends();//显示好友
	linphone_qt_set_my_presence(linphone_core_get_presence_info(linphone_qt_get_core()));//显示在线信息
	
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


int main(int argc, char *argv[])
{
    int result;
	const char *config_file;
	const char *factory_config_file;
	QString appname="TJphone";

	GError *error = NULL;
	GOptionContext *context;
	context = g_option_context_new (N_("- An internet video phone using the standard SIP (rfc3261) protocol."));
	g_option_context_add_main_entries (context, linphone_options, GETTEXT_PACKAGE);
	if (!g_option_context_parse (context, &argc, &argv, &error))  
    {  
      g_print ("option parsing failed: %s/n", error->message);  
      return -1;
    }

	if (workingdir!=NULL){
		if (chdir(workingdir)==-1){
			g_error("Could not change directory to %s : %s",workingdir,strerror(errno));
		}
	}
	a = new QApplication(argc, argv);
	QDir::addSearchPath("icons", QString(":icons/icons/"));
    QDir::addSearchPath("images", QString(":images/icons/"));
	config_file=linphone_get_config_file(linphone_cfgfile);
	factory_config_file = linphone_get_factory_config_file();

	if (linphone_logfile)
	{
		linphone_core_enable_logs_with_cb(qt_log_handler);
	} else {
		linphone_core_disable_logs();
	}
	linphone_qt_init_liblinphone(config_file, factory_config_file);

	QTextCodec *tl=QTextCodec::codecForName("utf8");
	QTextCodec::setCodecForLocale(tl);
	

	w = new MainWindow();
	QFont font("simsun",12,QFont::Normal,FALSE);
	a->setFont(font);
	a->setApplicationName(appname);
	
	QTimer *timer_iterate = new QTimer(a);
	a->connect(timer_iterate,SIGNAL(timeout()),w,SLOT(linphone_qt_iterate()));
	timer_iterate->start(30);
	QTimer *timer_logs = new QTimer(a);
	a->connect(timer_logs,SIGNAL(timeout()),w,SLOT(linphone_qt_check_logs()));
	timer_logs->start(500);

	//MyThread timerthread;
	//timerthread.start();
	linphone_qt_init_main_window(w);
    w->show();
    
	result = a->exec();

	linphone_core_destroy(the_core);
	//timerthread.terminate();
	timer_iterate->stop();
	timer_logs->stop();
    delete w;
    delete a;
	free((void*)config_file);
    return result;
}

