# Microsoft Visual C++ Generated NMAKE File, Format Version 2.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

!IF "$(CFG)" == ""
CFG=Win32 Debug
!MESSAGE No configuration specified.  Defaulting to Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Win32 Release" && "$(CFG)" != "Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "GSM.MAK" CFG="Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

################################################################################
# Begin Project
# PROP Target_Last_Scanned "Win32 Debug"
CPP=cl.exe

!IF  "$(CFG)" == "Win32 Release"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WinRel"
# PROP BASE Intermediate_Dir "WinRel"
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "WinRel"
# PROP Intermediate_Dir "WinRel"
OUTDIR=.\WinRel
INTDIR=.\WinRel

ALL : $(OUTDIR)/GSM.lib $(OUTDIR)/GSM.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE CPP /nologo /G3 /MT /W3 /GX /YX /Ox /Ot /Oa /Og /Oi /Ob2 /I "inc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR /c
# ADD CPP /nologo /G3 /MT /W3 /GX /YX /Ox /Ot /Oa /Og /Oi /Ob2 /I "inc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR /c
CPP_PROJ=/nologo /G3 /MT /W3 /GX /YX /Ox /Ot /Oa /Og /Oi /Ob2 /I "inc" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR$(INTDIR)/\
 /Fp$(OUTDIR)/"GSM.pch" /Fo$(INTDIR)/ /c 
CPP_OBJS=.\WinRel/
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"GSM.bsc" 
BSC32_SBRS= \
	$(INTDIR)/ADD.SBR \
	$(INTDIR)/CODE.SBR \
	$(INTDIR)/DEBUG.SBR \
	$(INTDIR)/DECODE.SBR \
	$(INTDIR)/GSM_CREA.SBR \
	$(INTDIR)/GSM_DECO.SBR \
	$(INTDIR)/GSM_DEST.SBR \
	$(INTDIR)/GSM_ENCO.SBR \
	$(INTDIR)/GSM_EXPL.SBR \
	$(INTDIR)/GSM_IMPL.SBR \
	$(INTDIR)/GSM_OPTI.SBR \
	$(INTDIR)/GSM_PRIN.SBR \
	$(INTDIR)/LONG_TER.SBR \
	$(INTDIR)/PREPROCE.SBR \
	$(INTDIR)/RPE.SBR \
	$(INTDIR)/SHORT_TE.SBR \
	$(INTDIR)/TABLE.SBR \
	$(INTDIR)/LPC.SBR

$(OUTDIR)/GSM.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=lib.exe
# ADD BASE LIB32 /NOLOGO
# ADD LIB32 /NOLOGO
LIB32_FLAGS=/NOLOGO /OUT:$(OUTDIR)\"GSM.lib" 
DEF_FLAGS=
DEF_FILE=
LIB32_OBJS= \
	$(INTDIR)/ADD.OBJ \
	$(INTDIR)/CODE.OBJ \
	$(INTDIR)/DEBUG.OBJ \
	$(INTDIR)/DECODE.OBJ \
	$(INTDIR)/GSM_CREA.OBJ \
	$(INTDIR)/GSM_DECO.OBJ \
	$(INTDIR)/GSM_DEST.OBJ \
	$(INTDIR)/GSM_ENCO.OBJ \
	$(INTDIR)/GSM_EXPL.OBJ \
	$(INTDIR)/GSM_IMPL.OBJ \
	$(INTDIR)/GSM_OPTI.OBJ \
	$(INTDIR)/GSM_PRIN.OBJ \
	$(INTDIR)/LONG_TER.OBJ \
	$(INTDIR)/PREPROCE.OBJ \
	$(INTDIR)/RPE.OBJ \
	$(INTDIR)/SHORT_TE.OBJ \
	$(INTDIR)/TABLE.OBJ \
	$(INTDIR)/LPC.OBJ

$(OUTDIR)/GSM.lib : $(OUTDIR)  $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 Debug"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WinDebug"
# PROP BASE Intermediate_Dir "WinDebug"
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "WinDebug"
# PROP Intermediate_Dir "WinDebug"
OUTDIR=.\WinDebug
INTDIR=.\WinDebug

ALL : $(OUTDIR)/GSM.lib $(OUTDIR)/GSM.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE CPP /nologo /MT /W3 /GX /Z7 /YX /Od /I "inc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /c
# ADD CPP /nologo /MT /W3 /GX /Z7 /YX /Od /I "inc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /c
CPP_PROJ=/nologo /MT /W3 /GX /Z7 /YX /Od /I "inc" /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"GSM.pch" /Fo$(INTDIR)/ /c 
CPP_OBJS=.\WinDebug/
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"GSM.bsc" 
BSC32_SBRS= \
	$(INTDIR)/ADD.SBR \
	$(INTDIR)/CODE.SBR \
	$(INTDIR)/DEBUG.SBR \
	$(INTDIR)/DECODE.SBR \
	$(INTDIR)/GSM_CREA.SBR \
	$(INTDIR)/GSM_DECO.SBR \
	$(INTDIR)/GSM_DEST.SBR \
	$(INTDIR)/GSM_ENCO.SBR \
	$(INTDIR)/GSM_EXPL.SBR \
	$(INTDIR)/GSM_IMPL.SBR \
	$(INTDIR)/GSM_OPTI.SBR \
	$(INTDIR)/GSM_PRIN.SBR \
	$(INTDIR)/LONG_TER.SBR \
	$(INTDIR)/PREPROCE.SBR \
	$(INTDIR)/RPE.SBR \
	$(INTDIR)/SHORT_TE.SBR \
	$(INTDIR)/TABLE.SBR \
	$(INTDIR)/LPC.SBR

$(OUTDIR)/GSM.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=lib.exe
# ADD BASE LIB32 /NOLOGO
# ADD LIB32 /NOLOGO
LIB32_FLAGS=/NOLOGO /OUT:$(OUTDIR)\"GSM.lib" 
DEF_FLAGS=
DEF_FILE=
LIB32_OBJS= \
	$(INTDIR)/ADD.OBJ \
	$(INTDIR)/CODE.OBJ \
	$(INTDIR)/DEBUG.OBJ \
	$(INTDIR)/DECODE.OBJ \
	$(INTDIR)/GSM_CREA.OBJ \
	$(INTDIR)/GSM_DECO.OBJ \
	$(INTDIR)/GSM_DEST.OBJ \
	$(INTDIR)/GSM_ENCO.OBJ \
	$(INTDIR)/GSM_EXPL.OBJ \
	$(INTDIR)/GSM_IMPL.OBJ \
	$(INTDIR)/GSM_OPTI.OBJ \
	$(INTDIR)/GSM_PRIN.OBJ \
	$(INTDIR)/LONG_TER.OBJ \
	$(INTDIR)/PREPROCE.OBJ \
	$(INTDIR)/RPE.OBJ \
	$(INTDIR)/SHORT_TE.OBJ \
	$(INTDIR)/TABLE.OBJ \
	$(INTDIR)/LPC.OBJ

$(OUTDIR)/GSM.lib : $(OUTDIR)  $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

################################################################################
# Begin Group "Source Files"

################################################################################
# Begin Source File

SOURCE=.\ADD.C
DEP_ADD_C=\
	.\inc\private.h\
	.\inc\gsm.h\
	.\inc\proto.h\
	.\inc\unproto.h

$(INTDIR)/ADD.OBJ :  $(SOURCE)  $(DEP_ADD_C) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\CODE.C
DEP_CODE_=\
	.\inc\config.h\
	.\inc\proto.h\
	.\inc\private.h\
	.\inc\gsm.h\
	.\inc\unproto.h

$(INTDIR)/CODE.OBJ :  $(SOURCE)  $(DEP_CODE_) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\DEBUG.C
DEP_DEBUG=\
	.\inc\private.h\
	.\inc\proto.h\
	.\inc\unproto.h

$(INTDIR)/DEBUG.OBJ :  $(SOURCE)  $(DEP_DEBUG) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\DECODE.C
DEP_DECOD=\
	.\inc\private.h\
	.\inc\gsm.h\
	.\inc\proto.h\
	.\inc\unproto.h

$(INTDIR)/DECODE.OBJ :  $(SOURCE)  $(DEP_DECOD) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\GSM_CREA.C
DEP_GSM_C=\
	.\inc\config.h\
	.\inc\proto.h\
	.\inc\gsm.h\
	.\inc\private.h\
	.\inc\unproto.h

$(INTDIR)/GSM_CREA.OBJ :  $(SOURCE)  $(DEP_GSM_C) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\GSM_DECO.C
DEP_GSM_D=\
	.\inc\private.h\
	.\inc\gsm.h\
	.\inc\proto.h\
	.\inc\unproto.h

$(INTDIR)/GSM_DECO.OBJ :  $(SOURCE)  $(DEP_GSM_D) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\GSM_DEST.C
DEP_GSM_DE=\
	.\inc\gsm.h\
	.\inc\private.h\
	.\inc\proto.h\
	.\inc\unproto.h

$(INTDIR)/GSM_DEST.OBJ :  $(SOURCE)  $(DEP_GSM_DE) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\GSM_ENCO.C
DEP_GSM_E=\
	.\inc\private.h\
	.\inc\gsm.h\
	.\inc\proto.h\
	.\inc\unproto.h

$(INTDIR)/GSM_ENCO.OBJ :  $(SOURCE)  $(DEP_GSM_E) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\GSM_EXPL.C
DEP_GSM_EX=\
	.\inc\private.h\
	.\inc\gsm.h\
	.\inc\proto.h\
	.\inc\unproto.h

$(INTDIR)/GSM_EXPL.OBJ :  $(SOURCE)  $(DEP_GSM_EX) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\GSM_IMPL.C
DEP_GSM_I=\
	.\inc\private.h\
	.\inc\gsm.h\
	.\inc\proto.h\
	.\inc\unproto.h

$(INTDIR)/GSM_IMPL.OBJ :  $(SOURCE)  $(DEP_GSM_I) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\GSM_OPTI.C
DEP_GSM_O=\
	.\inc\private.h\
	.\inc\gsm.h\
	.\inc\proto.h\
	.\inc\unproto.h

$(INTDIR)/GSM_OPTI.OBJ :  $(SOURCE)  $(DEP_GSM_O) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\GSM_PRIN.C
DEP_GSM_P=\
	.\inc\private.h\
	.\inc\gsm.h\
	.\inc\proto.h\
	.\inc\unproto.h

$(INTDIR)/GSM_PRIN.OBJ :  $(SOURCE)  $(DEP_GSM_P) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\LONG_TER.C
DEP_LONG_=\
	.\inc\private.h\
	.\inc\gsm.h\
	.\inc\proto.h\
	.\inc\unproto.h

$(INTDIR)/LONG_TER.OBJ :  $(SOURCE)  $(DEP_LONG_) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\PREPROCE.C
DEP_PREPR=\
	.\inc\private.h\
	.\inc\gsm.h\
	.\inc\proto.h\
	.\inc\unproto.h

$(INTDIR)/PREPROCE.OBJ :  $(SOURCE)  $(DEP_PREPR) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\RPE.C
DEP_RPE_C=\
	.\inc\private.h\
	.\inc\gsm.h\
	.\inc\proto.h\
	.\inc\unproto.h

$(INTDIR)/RPE.OBJ :  $(SOURCE)  $(DEP_RPE_C) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SHORT_TE.C
DEP_SHORT=\
	.\inc\private.h\
	.\inc\gsm.h\
	.\inc\proto.h\
	.\inc\unproto.h

$(INTDIR)/SHORT_TE.OBJ :  $(SOURCE)  $(DEP_SHORT) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\TABLE.C
DEP_TABLE=\
	.\inc\private.h\
	.\inc\gsm.h\
	.\inc\proto.h\
	.\inc\unproto.h

$(INTDIR)/TABLE.OBJ :  $(SOURCE)  $(DEP_TABLE) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\LPC.C
DEP_LPC_C=\
	.\inc\private.h\
	.\inc\gsm.h\
	.\inc\proto.h\
	.\inc\unproto.h

$(INTDIR)/LPC.OBJ :  $(SOURCE)  $(DEP_LPC_C) $(INTDIR)

# End Source File
# End Group
# End Project
################################################################################
