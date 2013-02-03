#-------------------------------------------------
#
# Project created by QtCreator 2011-08-16T16:47:33
#
#-------------------------------------------------

QT       += core gui

CONFIG = debug

TARGET = linphone-qt
TEMPLATE = vcapp

win32:INCLUDEPATH +=". ../../../../libosip2-3.3.0/include ../../ ../../../ ../../../coreapi/ ../../../mediastreamer2/include ../../../oRTP/src ../../../oRTP/include ../../../../libeXosip2-3.3.0/include ../../../../speex-1.2rc1/include ../../../../codecs/libresample-0.1.3/include ../../../../codecs/gsm/inc ../../../../videos/libtheora-1.0/include ../../../../videos/libogg-1.1.3/include ../../../../SDK/ffmpeg-bin-win32/"
win32:LIBS +=" -L$(QTDIR)/lib -L../../../../sdk/Libs/Release/ipp/ -L../../../../sdk/Libs/Release/SDL -L../../../../sdk/Libs/Release/ffmpeg-bin-win32 -L../../../../sdk/Libs/Release/g729fp -L../../../../sdk/Libs/Debug  qtmaind.lib QtGuid4.lib QtCored4.lib eXosip.lib osipparser2.lib osip2.lib Ws2_32.lib mediastreamer2.lib ortp.lib gsm.lib Winmm.lib libcoreapi.lib libspeex.lib libspeexdsp.lib libresample.lib libtheora.lib vfw32.lib avcodec-52.lib avdevice-52.lib avformat-52.lib avutil-49.lib swscale-0.lib libvorbis.lib libvorbisfile.lib ogg_static_d.lib Wininet.lib Iphlpapi.lib dnsapi.lib Qwave.lib SDL.lib libcmtd.lib ippcorel.lib ippsemerged.lib ippsmerged.lib ippscmerged.lib ippscemerged.lib ippsrmerged.lib ippsremerged.lib usc.lib QtMultimediad4.lib"

DEFINES += "_WINDOWS UNICODE WIN32 QT_LARGEFILE_SUPPORT QT_DLL QT_GUI_LIB QT_CORE_LIB QT_HAVE_MMX QT_HAVE_3DNOW QT_HAVE_SSE QT_HAVE_MMXEXT QT_HAVE_SSE2 QT_THREAD_SUPPORT _DEBUG _CRT_SECURE_NO_DEPRECATE _LIB HAVE_SPEEXDSP HAVE_SPEEX_NOISE ORTP_INET6 IN_LINPHONE WINDOW_NATIVE ENABLE_TRACE OSIP_MT HAVE_CONFIG_H QT_MULTIMEDIA_LIB"

SOURCES += ../../main.cpp\
	../../linphone_callback.cpp \
	../../friend_treemodel.cpp \
	../../support.cpp \
        ../../mainwindow.cpp \
    ../../callhistory.cpp \
    ../../debugwindow.cpp \
    ../../preference.cpp \
    ../../HTMLDelegate.cpp \
    ../../sip_account.cpp \
    ../../chatroom.cpp \
    ../../contact.cpp \
    ../../callviewframe.cpp \
    ../../password.cpp

HEADERS  += ../../mainwindow.h \
    ../../friend_treemodel.h \
    ../../linphone.h \
    ../../callhistory.h \
    ../../debugwindow.h \
    ../../preference.h \
    ../../HTMLDelegate.h \
    ../../sip_account.h \
    ../../chatroom.h \
    ../../contact.h \
    ../../callviewframe.h \
    ../../password.h

FORMS    += ../../mainwindow.ui \
    ../../callhistory.ui \
    ../../debugwindow.ui \
    ../../preference.ui \
    ../../sip_account.ui \
    ../../chatroom.ui \
    ../../contact.ui \
    ../../callviewframe.ui \
    ../../password.ui
