@echo off

set ARCH=wce420xsc

set IPPENV="%IPPROOT%\tools\env\ippenvixp.bat"
if exist %IPPENV% goto :start
set IPPENV="%ProgramFiles%\Intel\IPP\5.1\ixp\tools\env\ippenvixp.bat"
if exist %IPPENV% goto :start 
  @echo *************************************************************************
  @echo Intel(R) IPP is not found!
  @echo Please install Intel(R) IPP or set IPPROOT environment variable correctly.
  @echo *************************************************************************
  exit /b

:start


set COMPILER=clarm.exe
set TOOL="%ProgramFiles%\Microsoft eMbedded C++ 4.0\EVC\wce420\bin\wcearmv4.bat"
if not exist %TOOL% (
 @echo WCE420 is not installed!
 exit /b
)

@call %TOOL%
@call %IPPENV%

set CPU=%1
if "%CPU%"=="" goto :default_cpu
goto :begin

:default_cpu
 set CPU=s2

:begin
nmake clean ARCH=%ARCH% CPU=%CPU%
nmake CC=%COMPILER% ARCH=%ARCH% CPU=%CPU%