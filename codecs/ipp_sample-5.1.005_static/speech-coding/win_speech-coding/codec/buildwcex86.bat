@echo off

set ARCH=wcex86


set IPPENV="%IPPROOT%\tools\env\ippenv.bat"
if exist %IPPENV% goto :start
set IPPENV="%ProgramFiles%\Intel\IPP\5.1\ia32\tools\env\ippenv.bat"
if exist %IPPENV% goto :start 
  @echo *************************************************************************
  @echo Intel(R) IPP is not found!
  @echo Please install Intel(R) IPP or set IPPROOT environment variable correctly.
  @echo *************************************************************************
  exit /b
 
:start


set COMPILER=cl.exe
set TOOL="%ProgramFiles%\Microsoft eMbedded C++ 4.0\EVC\wce500\bin\WCEx86.BAT"
if not exist %TOOL% (
 @echo WCE x86 is not installed!
 exit /b
)

@call %TOOL%
@call %IPPENV%


nmake clean ARCH=%ARCH%
nmake CC=%COMPILER% ARCH=%ARCH%
