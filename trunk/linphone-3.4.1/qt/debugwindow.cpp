#include "debugwindow.h"
#include "ui_debugwindow.h"
#include <QReadWriteLock>
#include "linphone.h"
#include <direct.h>

QReadWriteLock lock;

/* Marker to insert as a line at the start of every log file */
#define LOGFILE_MARKER_START  "----<start>----"
/* Marker to insert as a line at the end of every log file */
#define LOGFILE_MARKER_STOP  "----<end>----"
/* Number of files to keep in history, log file rotation will be
   performed. */
#define LOGFILE_ROTATION 4
/* Pointer to opened log file */
static FILE *_logfile = NULL;

typedef struct _LinphoneGtkLog{
	OrtpLogLevel lev;
	QString msg;
}LinphoneGtkLog;

QList<LinphoneGtkLog *> LogList;

debugwindow::debugwindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::debugwindow)
{
    ui->setupUi(this);
}

debugwindow::~debugwindow()
{
    delete ui;
	lock.lockForWrite();
	qDeleteAll(LogList);
	LogList.clear();
	lock.unlock();
}
void debugwindow::check_logs(){
	lock.lockForWrite();
	LinphoneGtkLog *lgl = NULL;
	for (QList<LinphoneGtkLog *>::iterator i = LogList.begin(); i != LogList.end();)
	{
		lgl =  *i;
		display_log(lgl->lev,lgl->msg);
		i++;
	}
	qDeleteAll(LogList);
	LogList.clear();
	lock.unlock();
	return;
}

void debugwindow::display_log(OrtpLogLevel lev, QString &msg)
{

	const char *lname="undef";
	const char *color="orange";
	QString msgline;
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

	
	if (lev==ORTP_WARNING)color = "red";
	msgline.sprintf("<b><font color=%s >%s:</font></b>", color,lname);
	msgline.append(msg);
	ui->textBrowser->append(msgline);
}

void debugwindow::on_buttonClose_clicked(QAbstractButton *button)
{
    this->close();
}

/* Called on exit, print out the marker, close the file and avoid to
   continue logging. */
static void log_uninit()
{
	if (_logfile != NULL) {
		fprintf(_logfile, "%s\n", LOGFILE_MARKER_STOP);
		fclose(_logfile);
		_logfile = NULL;
	}
}

/* Called when we start logging, find a good place for the log files,
   perform rotation, insert the start marker and return the pointer to
   the file that should be used for logging, or NULL on errors or if
   disabled. */
static FILE *log_init()
{
	static char _logdir[1024];
	static char _logfname[1024];
	static gboolean _log_init = FALSE;
	const char *dst_fname;

	if (!_log_init) {
		dst_fname = linphone_qt_get_ui_config("logfile",NULL);
		/* For anything to happen, we need a logfile configuration variable,
		 this is our trigger */
		if (dst_fname) {
			/* arrange for _logdir to contain a
			 directory that has been created and _logfname to contain the
			 path to a file to which we will log */
#ifdef WIN32
			const char *appdata=getenv("LOCALAPPDATA");
			if (appdata) {
				snprintf(_logdir, sizeof(_logdir),"%s\\Linphone", appdata);
				_mkdir(_logdir);
			} else {
				_logdir[0] = '\0';
			}
#define PATH_SEPARATOR '\\'
#else
			const char *home=getenv("HOME");
			if (home) {
				snprintf(_logdir, sizeof(_logdir),"%s/.linphone", home);
				mkdir(_logdir,S_IRUSR | S_IWUSR | S_IRGRP);
			} else {
				_logdir[0] = '\0';
			}
#define PATH_SEPARATOR '/'
#endif
			/* We have a directory, fix the path to the log file in it and
			 open the file so that we will be appending to it. */
			if (_logdir[0] != '\0') {
				snprintf(_logfname, sizeof(_logfname), "%s%c%s",
					_logdir, PATH_SEPARATOR, dst_fname);
				/* If the constant LOGFILE_ROTATION is greater than zero, then
				 we kick away a simple rotation that will ensure that there
				 are never more than LOGFILE_ROTATION+1 old copies of the
				 log file on the disk.  The oldest file is always rotated
				 "away" as expected.  Rotated files have the same name as
				 the main log file, though with a number 0..LOGFILE_ROTATION
				 at the end, where the greater the number is, the older the
				 file is. */
				if (ortp_file_exist(_logfname)==0 && LOGFILE_ROTATION > 0) {
					int i;
					char old_fname[1024];
					char new_fname[1024];

					/* Rotate away existing files.  We make sure to remove the
					 old files otherwise rename() would not work properly.  We
					 have to loop in reverse here. */
					for (i=LOGFILE_ROTATION-1;i>=0;i--) {
						snprintf(old_fname, sizeof(old_fname), "%s%c%s.%d",
							_logdir, PATH_SEPARATOR, dst_fname, i);
						snprintf(new_fname, sizeof(new_fname), "%s%c%s.%d",
							_logdir, PATH_SEPARATOR, dst_fname, i+1);
						if (ortp_file_exist(old_fname)==0) {
							if (ortp_file_exist(new_fname)==0)
								unlink(new_fname);
							rename(old_fname, new_fname);
						}
					}
					/* Move current log file as the first of the rotation.  Make
					 sure to remove the old .0 also, since otherwise rename()
					 would not work as expected. */
					snprintf(new_fname, sizeof(new_fname), "%s%c%s.%d",
						_logdir, PATH_SEPARATOR, dst_fname, 0);
					if (ortp_file_exist(new_fname)==0)
						unlink(new_fname);
					rename(_logfname, new_fname);
				}
				/* Start a new log file and mark that we have now initialised */
				_logfile = fopen(_logfname, "w");
				fprintf(_logfile, "%s\n", LOGFILE_MARKER_START);
			}
		}
		_log_init = TRUE;
	}
	return _logfile;
}

void debugwindow::log_file(OrtpLogLevel lev, const char *msg)
{
	LinphoneCore *lc;
	time_t now;
	FILE *outlog;

	lc = linphone_qt_get_core();
	/* Nothing to do until the core has initialised */
	if (lc == NULL)
		return;

	/* lc->config will turn NULL at exit, close the file to flush and
	 return to stop logging */
	if (linphone_core_get_config(lc) == NULL) {
		log_uninit();
		return;
	}

	outlog = log_init();
	if (outlog != NULL) {
		/* We have an opened file and we have initialised properly, it's
		 time to write all these log messages. We convert the log level
		 from oRTP into something readable and timestamp each log
		 message.  The format of the timestamp can be controlled by
		 logfile_date_format in the GtkUi section of the config file,
		 but it defaults to something compact, but yet readable. */
		const char *lname="undef";
		const char *dateformat=linphone_qt_get_ui_config("logfile_date_format",
		    "%Y%m%d-%H:%M:%S");
		char date[256];

		/* Convert level constant to text */
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
				lname="undef";
				break;
		}
		/* Get current time and format it properly */
		now = time(NULL);
		strftime(date, sizeof(date), dateformat, localtime(&now));
		/* Now print out the message to the logfile.  We don't flush,
		 maybe we should do to ensure that we have all the messages in
		 case of a crash (which is one of the main reasons we have a
		     log facility in the first place). */
		fprintf(outlog, "[%s] [%s] %s\n", date, lname, msg);
	}
}

void debugwindow::log_push(OrtpLogLevel lev, const char *fmt, va_list args)
{
	QString msg;
	msg.vsprintf(fmt,args);
	LinphoneGtkLog *lgl=new LinphoneGtkLog();
	lgl->lev=lev;
	lgl->msg=msg;
	lock.lockForWrite();
	LogList.append(lgl);
	log_file(lev, msg.toAscii());
	lock.unlock();
}