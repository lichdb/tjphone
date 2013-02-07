## Directory name for obj-files
ObjDir=..\..\bin\$(ARCH)\obj\usc_ec

## Directory name for exe-files
BinDir=..\..\bin\$(ARCH)

## Directory name for lib-files
LibDir=..\..\bin\$(ARCH)\lib

## Name of the test application to build
!IF "$(ARCH)" == "wce420xsc"
BinNameExe=usc_ec_$(CPU).exe
!ELSE
BinNameExe=usc_ec.exe
!ENDIF


## LINKER

!IF "$(ARCH)" == "wcex86"
VCLIB = commctrl.lib coredll.lib corelibc.lib
!ELSE
!IF "$(ARCH)" == "ia32" || "$(ARCH)" == "ia64" || "$(ARCH)" == "em64t"
VCLIB = kernel32.lib advapi32.lib
!ENDIF
!ENDIF
LINK32 = link.exe

!IF "$(ARCH)" == "ia64" || "$(ARCH)" == "em64t"
VCLIB = $(VCLIB) bufferoverflowu.lib
!ENDIF

!INCLUDE $(ARCH).ini

USCLIB=$(LibDir)\usc.lib


# check if tools.ini file is used

SRCDIRCOMMON =.\src
SRCDIRVM     =..\..\codec\vm\src

ObjsSrc= \
 $(ObjDir)\loadec.obj \
 $(ObjDir)\usc_ec.obj \
 $(ObjDir)\util.obj 

ObjsSrcVm= \
 $(ObjDir)\vm_sys_info_win32.obj
OWNINC= -I".\include" \
        -I"..\..\codec\include" \
        -I"..\..\codec\ec\include" \
        -I"..\..\codec\ec_int\include" \
        -I"..\..\codec\vm\include" \
        -I"..\..\codec\vm\include\sys" \
        -I"$(IPPROOT)"\include

!IF "$(ARCH)" == "wce420xsc"
CEVersion = 420
CePlatform = WIN32_PLATFORM_PSPC=400
CECrtMT = C
!ENDIF
!IF "$(ARCH)" == "wcexsc"
CEVersion = 500
#CePlatform = WCE_PLATFORM_STANDARDSDK_500
CePlatform = WIN32_PLATFORM_PSPC=500
!ENDIF

!IF "$(ARCH)" == "wcex86"
CEVersion = 500
CePlatform = 830_WinCE5_DMI2_Custom
!ENDIF

COpt=-c -W4 -O2 /EHsc

!IF "$(CC)" == "icl.exe"
COpt = $(COpt) -Qopenmp
!ENDIF


!IF "$(ARCH)" == "wce420xsc" || "$(ARCH)" == "wcexsc"
COpt = $(COpt) /M$(CECrtMT)
!ELSE
!IF "$(ARCH)" == "wcex86"
COpt = $(COpt) -Gs8192 -GF
!ELSE
COpt = $(COpt) -MD
!ENDIF
!ENDIF

            
CDEFINED = -D "NO_SCRATCH_MEMORY_USED" -D "NDEBUG"

!IF "$(ARCH)" == "wce420xsc"
CDEFINED = $(CDEFINED) -DARM -D_ARM_ -DARMV4 -DUNICODE -D_UNICODE \
                       -D_ARCH_XSC -D_USC_EC_INT\
                       -D UNDER_CE=$(CEVersion) -D "$(CePlatform)" \
                       -D _WIN32_WCE=$(CEVersion) \
                       -D _NO_CONSOLE_OUTPUT_ -D _FORCE_BATCH_MODE
!ELSE
!IF "$(ARCH)" == "wcexsc"
CDEFINED = $(CDEFINED) -D ARM -D _ARM_ -DUNICODE -D_UNICODE \
                       -D_USC_EC_INT\
                       -D UNDER_CE=$(CEVersion) -D "$(CePlatform)"  -D _WIN32_WCE=$(CEVersion) \
                       -D _NO_CONSOLE_OUTPUT_ -D _FORCE_BATCH_MODE
!ELSE
CDEFINED=$(CDEFINED) -D_USC_EC_ALL -D "_WINDOWS" /D "_MBCS"
!IF "$(ARCH)" == "wcex86"
CDEFINED = $(CDEFINED) -DWIN32 -Di_386_ -Dx86 -D_X86_ -DUNICODE -D_UNICODE \
                       -D UNDER_CE=$(CEVersion) -D "$(CePlatform)" \
                       -D _WIN32_WCE=$(CEVersion)
!ENDIF
!IF "$(ARCH)" == "ia32"
CDEFINED=$(CDEFINED) -D "WIN32" /D "_WIN32_WINNT=0x0500"
!ENDIF
!IF "$(ARCH)"=="ia64"
CDEFINED = $(CDEFINED) -D "WIN64" -D "_WIN64"
!ENDIF
!IF "$(ARCH)"=="em64t"
CDEFINED = $(CDEFINED) /D "WIN64" /D "_WIN64"
!ENDIF
!ENDIF
!ENDIF
            
CFLAGS=$(COpt) $(CDEFINED) $(OWNINC)

################################################################

all : prepare $(BinDir)\$(BinNameExe)

{$(SRCDIRCOMMON)}.c{$(ObjDir)}.obj::
   $(CC)  $(CFLAGS) /Fo$(ObjDir)\ $<

{$(SRCDIRVM)}.c{$(ObjDir)}.obj::
   $(CC)  $(CFLAGS) /Fo$(ObjDir)\ $<
!IF "$(ARCH)" == "wcex86"
nolib = /nodefaultlib:OLDNAMES.lib /nodefaultlib:libc.lib \
        /nodefaultlib:libcd.lib /nodefaultlib:libcmt.lib \
        /nodefaultlib:libcmtd.lib /nodefaultlib:msvcrt.lib
winmainflag = /entry:WinMainCRTStartup
!ELSE
!IF "$(ARCH)" == "ia32" || "$(ARCH)" == "em64t"
nolib = /nodefaultlib:libc.lib /opt:noref
!ENDIF
!IF "$(ARCH)" == "ia64"
nolib = /nodefaultlib:libc.lib /nodefaultlib:libcmt.lib
!ENDIF
!ENDIF

$(BinDir)\$(BinNameExe) : $(ObjsSrc) $(ObjsSrcVm)
   $(LINK32) $(IPPLIB) $(USCLIB) $(nolib) /nologo \
    $(winmainflag) \
    /incremental:no $(LIBLINKFLAGS)\
    /SUBSYSTEM:$(SUBSYSTEMFlag) $(VCLIB) \
    /out:$@ $(ObjsSrc) $(ObjsSrcVm)

run : $(BinDir)\$(BinNameExe)

prepare :
   @if not exist $(ObjDir)   mkdir $(ObjDir)
   @if not exist $(BinDir)   mkdir $(BinDir)

clean:
   @echo Cleaning ...
   @if exist $(ObjDir) del /s /q $(ObjDir)
   @if exist $(BinDir) del /s /q $(BinDir)\$(BinNameExe)

help:
   @echo Usage of this Makefile:
   @echo "nmake"            - prepare and build phases
   @echo "nmake prepare"    - creates directories
   @echo "nmake clean"      - removes directories created
   @echo "nmake build"      - builds
   @echo "nmake help"       - help
