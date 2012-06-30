# Microsoft Visual C++ generated build script - Do not modify

PROJ = TESTGSM
DEBUG = 0
PROGTYPE = 3
CALLER = 
ARGS = 
DLLS = 
D_RCDEFINES = -d_DEBUG
R_RCDEFINES = -dNDEBUG
ORIGIN = MSVC
ORIGIN_VER = 1.00
PROJPATH = D:\NETFONE\NETFONE\GSM\
USEMFC = 0
CC = cl
CPP = cl
CXX = cl
CCREATEPCHFLAG = 
CPPCREATEPCHFLAG = 
CUSEPCHFLAG = 
CPPUSEPCHFLAG = 
FIRSTC = TESTGSM.C   
FIRSTCPP =             
RC = rc
CFLAGS_D_WTTY = /nologo /G2 /Mq /W3 /Zi /AM /Od /D "_DEBUG" /I "inc" /I "inc" /FR /Fd"TESTGSM.PDB"
CFLAGS_R_WTTY = /nologo /Gs /G3 /Mq /W3 /AM /Oa /Oe /Og /Oi /Ol /Or /Ot /Ox /Oz /D "NDEBUG" /I "inc" /FR 
LFLAGS_D_WTTY = /NOLOGO /ONERROR:NOEXE /NOD /PACKC:57344 /CO /ALIGN:16
LFLAGS_R_WTTY = /NOLOGO /ONERROR:NOEXE /NOD /PACKC:57344 /ALIGN:16
LIBS_D_WTTY = oldnames libw mlibcewq
LIBS_R_WTTY = oldnames libw mlibcewq
RCFLAGS = /nologo
RESFLAGS = /nologo
RUNFLAGS = 
OBJS_EXT = 
LIBS_EXT = GSM.LIB 
!if "$(DEBUG)" == "1"
CFLAGS = $(CFLAGS_D_WTTY)
LFLAGS = $(LFLAGS_D_WTTY)
LIBS = $(LIBS_D_WTTY)
MAPFILE = nul
RCDEFINES = $(D_RCDEFINES)
DEFFILE=D:\L\MSVC\BIN\CL.DEF
!else
CFLAGS = $(CFLAGS_R_WTTY)
LFLAGS = $(LFLAGS_R_WTTY)
LIBS = $(LIBS_R_WTTY)
MAPFILE = nul
RCDEFINES = $(R_RCDEFINES)
DEFFILE=D:\L\MSVC\BIN\CL.DEF
!endif
!if [if exist MSVC.BND del MSVC.BND]
!endif
SBRS = TESTGSM.SBR


GSM_DEP = 

all:	$(PROJ).EXE $(PROJ).BSC

TESTGSM.OBJ:	TESTGSM.C $(TESTGSM_DEP)
	$(CC) $(CFLAGS) $(CCREATEPCHFLAG) /c TESTGSM.C


$(PROJ).EXE::	TESTGSM.OBJ $(OBJS_EXT) $(DEFFILE)
	echo >NUL @<<$(PROJ).CRF
TESTGSM.OBJ +
$(OBJS_EXT)
$(PROJ).EXE
$(MAPFILE)
d:\l\msvc\lib\+
d:\l\msvc\mfc\lib\+
GSM.LIB+
$(LIBS)
$(DEFFILE);
<<
	link $(LFLAGS) @$(PROJ).CRF
	$(RC) $(RESFLAGS) $@


run: $(PROJ).EXE
	$(PROJ) $(RUNFLAGS)


$(PROJ).BSC: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<
