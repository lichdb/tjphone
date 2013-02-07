## Directory name for obj-files
ObjDir=..\bin\$(ARCH)\obj\usc

## Directory name for lib-files
LibDir=..\bin\$(ARCH)\lib\

## Name of the library to build
LibNameUSC=usc.lib
LibNameG729=g729.lib


## LINKER

LIB32 = link.exe -lib


SRCDIRLIBamrwb =.\amrwb\src
SRCDIRLIBg711  =.\g711\src
SRCDIRLIBg722  =.\g722\src
SRCDIRLIBg7221 =.\g722.1\src
SRCDIRLIBg7231 =.\g723.1\src
SRCDIRLIBg726  =.\g726\src
SRCDIRLIBg728  =.\g728\src
SRCDIRLIBg729i =.\g729i\src
SRCDIRLIBg729fp=.\g729fp\src
SRCDIRLIBgsmamr=.\gsmamr\src
SRCDIRLIBgsmfr =.\gsmfr\src
SRCDIRLIBec    =.\ec\src
SRCDIRLIBec_int=.\ec_int\src


Objsamrwb= \
 $(ObjDir)\decamrwb.obj \
 $(ObjDir)\dtxamrwb.obj \
 $(ObjDir)\encamrwb.obj \
 $(ObjDir)\ownamrwb.obj \
 $(ObjDir)\tabamrwb.obj \
 $(ObjDir)\uscamrwb.obj \
 $(ObjDir)\utilamrwb.obj

Objsg711= \
 $(ObjDir)\decg711.obj \
 $(ObjDir)\encg711.obj \
 $(ObjDir)\plcg711.obj \
 $(ObjDir)\uscg711.obj

Objsg722= \
 $(ObjDir)\decg722sb.obj \
 $(ObjDir)\encg722sb.obj \
 $(ObjDir)\owng722sb.obj \
 $(ObjDir)\uscg722sb.obj

Objsg7221= \
 $(ObjDir)\decg722.obj \
 $(ObjDir)\encg722.obj \
 $(ObjDir)\g722api.obj \
 $(ObjDir)\owng722.obj \
 $(ObjDir)\uscg722.obj

Objsg7231= \
 $(ObjDir)\decg723.obj \
 $(ObjDir)\encg723.obj \
 $(ObjDir)\owng723.obj \
 $(ObjDir)\uscg723.obj 

Objsg726= \
 $(ObjDir)\uscg726.obj

Objsg728= \
 $(ObjDir)\adapters.obj \
 $(ObjDir)\consealg728.obj \
 $(ObjDir)\g728api.obj \
 $(ObjDir)\g728tables.obj \
 $(ObjDir)\owng728.obj \
 $(ObjDir)\pitchg728.obj \
 $(ObjDir)\pstg728.obj \
 $(ObjDir)\uscg728.obj \
 $(ObjDir)\vq.obj
 
!IF "$(ARCH)" != "wce420xsc" && "$(ARCH)" != "wcexsc"
Objsg729fp= \
 $(ObjDir)\decg729fp.obj \
 $(ObjDir)\encg729fp.obj \
 $(ObjDir)\owng729fp.obj \
 $(ObjDir)\usc729fp.obj 
!ENDIF

Objsg729i= \
 $(ObjDir)\decg729.obj \
 $(ObjDir)\encg729.obj \
 $(ObjDir)\owng729.obj \
 $(ObjDir)\uscg729i.obj

Objsgsmamr= \
 $(ObjDir)\bitgsmamr.obj \
 $(ObjDir)\decgsmamr.obj \
 $(ObjDir)\dtxgsmamr.obj \
 $(ObjDir)\encgsmamr.obj \
 $(ObjDir)\gain_util.obj \
 $(ObjDir)\tabgsmamr.obj \
 $(ObjDir)\uscgsmamr.obj \
 $(ObjDir)\utilgsmamr.obj

Objsgsmfr= \
 $(ObjDir)\decgsmfr.obj \
 $(ObjDir)\encgsmfr.obj \
 $(ObjDir)\owngsmfr.obj \
 $(ObjDir)\uscgsmfr.obj

Objsec= \
 $(ObjDir)\ah.obj \
 $(ObjDir)\ec_fb.obj \
 $(ObjDir)\ec_sb.obj \
 $(ObjDir)\ec_sbf.obj \
 $(ObjDir)\uscec.obj
 
Objsecint= \
 $(ObjDir)\ah_int.obj \
 $(ObjDir)\ec_fb_int.obj \
 $(ObjDir)\ec_sb_int.obj \
 $(ObjDir)\ec_sbf_int.obj \
 $(ObjDir)\uscec_int.obj
 

OWNINC=-I".\amrwb\include" \
       -I".\g711\include" \
       -I".\g722\include" \
       -I".\g722.1\include" \
       -I".\g723.1\include" \
       -I".\g726\include" \
       -I".\g728\include" \
       -I".\g729i\include" \
       -I".\g729fp\include" \
       -I".\gsmamr\include" \
       -I".\gsmfr\include" \
       -I".\ec\include" \
       -I".\ec_int\include" \
       -I".\include" \
       -I"$(IPPROOT)"\include

COpt = -c -W4 -O2 /EHsc

!IF "$(ARCH)" == "wce420xsc" || "$(ARCH)" == "wcexsc"
COpt = $(COpt) /MC
!ELSE
COpt = $(COpt) -MD
!ENDIF

!IF "$(ARCH)" == "wce420xsc"
CEVersion = 420
CePlatform = WIN32_PLATFORM_PSPC=400
!ENDIF

!IF "$(ARCH)" == "wcexsc"
CEVersion = 500
CePlatform = WCE_PLATFORM_STANDARDSDK_500
!ENDIF
CDEFINED = -D "NO_SCRATCH_MEMORY_USED" -D "NDEBUG"

!IF "$(ARCH)" == "wce420xsc"
CDEFINED = $(CDEFINED) -DARM -D_ARM_ -DARMV4 -DUNICODE -D_UNICODE \
                       -D UNDER_CE=$(CEVersion) -D "$(CePlatform)" \
                       -D _NO_CONSOLE_OUTPUT_ -D _FORCE_BATCH_MODE
!ELSE
!IF "$(ARCH)" == "wcexsc"
CDEFINED = $(CDEFINED) -DUNICODE -D_UNICODE \
                       -D UNDER_CE=$(CEVersion) -D "$(CePlatform)"  -D _WIN32_WCE=$(CEVersion) \
                       -D _NO_CONSOLE_OUTPUT_ -D _FORCE_BATCH_MODE
!ENDIF
!ENDIF

## for scratchmem.h##
!IF "$(ARCH)"=="ia32"
CDEFINED = $(CDEFINED) /D "_WIN32"
!ELSE
!IF "$(ARCH)"=="ia64"
CDEFINED = $(CDEFINED) /D "_WIN64"
!ELSE
!IF "$(ARCH)"=="em64t"
CDEFINED = $(CDEFINED) /D "_WIN64"
!ELSE
!IF "$(ARCH)" == "wce420xsc"
CDEFINED = $(CDEFINED) /D _WIN32_WCE
!ELSE
!IF "$(ARCH)" == "wcexsc"
CDEFINED = $(CDEFINED) /D _WIN32_WCE
!ENDIF
!ENDIF
!ENDIF
!ENDIF
!ENDIF
            
CFLAGS=$(COpt) $(CDEFINED) $(OWNINC)

################################################################

all : prepare $(LibDir)\$(LibNameUSC) $(LibDir)\$(LibNameG729)


{$(SRCDIRLIBamrwb)}.c{$(ObjDir)}.obj::
   $(CC)  $(CFLAGS) /Fo$(ObjDir)\ $<

{$(SRCDIRLIBg711)}.c{$(ObjDir)}.obj::
   $(CC)  $(CFLAGS) /Fo$(ObjDir)\ $<
 
{$(SRCDIRLIBg722)}.c{$(ObjDir)}.obj::
   $(CC)  $(CFLAGS) /Fo$(ObjDir)\ $<
  
{$(SRCDIRLIBg7221)}.c{$(ObjDir)}.obj::
   $(CC)  $(CFLAGS) /Fo$(ObjDir)\ $<
 
{$(SRCDIRLIBg7231)}.c{$(ObjDir)}.obj::
   $(CC)  $(CFLAGS) /Fo$(ObjDir)\ $<
 
{$(SRCDIRLIBg726)}.c{$(ObjDir)}.obj::
   $(CC)  $(CFLAGS) /Fo$(ObjDir)\ $<
  
{$(SRCDIRLIBg728)}.c{$(ObjDir)}.obj::
   $(CC)  $(CFLAGS) /Fo$(ObjDir)\ $<
 
{$(SRCDIRLIBg729i)}.c{$(ObjDir)}.obj::
   $(CC)  $(CFLAGS) /Fo$(ObjDir)\ $<

{$(SRCDIRLIBg729fp)}.c{$(ObjDir)}.obj::
   $(CC)  $(CFLAGS) /Fo$(ObjDir)\ $<

{$(SRCDIRLIBgsmamr)}.c{$(ObjDir)}.obj::
   $(CC)  $(CFLAGS) /Fo$(ObjDir)\ $<

{$(SRCDIRLIBgsmfr)}.c{$(ObjDir)}.obj::
   $(CC)  $(CFLAGS) /Fo$(ObjDir)\ $<

{$(SRCDIRLIBec)}.c{$(ObjDir)}.obj::
   $(CC)  $(CFLAGS) /Fo$(ObjDir)\ $<

{$(SRCDIRLIBec_int)}.c{$(ObjDir)}.obj::
   $(CC)  $(CFLAGS) /Fo$(ObjDir)\ $<

$(LibDir)\$(LibNameUSC) : $(Objsamrwb) $(Objsg711) $(Objsg722) $(Objsg7221) \
                          $(Objsg7231) $(Objsg726) $(Objsg728) $(Objsg729fp) \
                          $(Objsg729i) $(Objsgsmamr) $(Objsgsmfr) \
                          $(Objsec) $(Objsecint)
   $(LIB32) /nologo /out:$@ $(Objsamrwb) $(Objsg711) $(Objsg722) $(Objsg7221) \
                            $(Objsg7231) $(Objsg726) $(Objsg728) $(Objsg729fp) \
                            $(Objsg729i) $(Objsgsmamr) $(Objsgsmfr) \
                            $(Objsec) $(Objsecint)

$(LibDir)\$(LibNameG729) : $(Objsg729fp) 
   $(LIB32) /nologo /out:$@ $(Objsg729fp) 

prepare :
   @if not exist $(LibDir)      mkdir $(LibDir)
   @if not exist $(ObjDir)      mkdir $(ObjDir)

clean:
   @echo Cleaning ...
   @if exist $(ObjDir)      del /s /q $(ObjDir)
   @if exist $(LibDir)      del /s /q $(LibDir)\$(LibNameUSC)

help:
   @echo Usage of this Makefile:
   @echo "nmake"            - prepare and build phases
   @echo "nmake prepare"    - creates directories
   @echo "nmake clean"      - removes directories created
   @echo "nmake build"      - builds
   @echo "nmake help"       - help
