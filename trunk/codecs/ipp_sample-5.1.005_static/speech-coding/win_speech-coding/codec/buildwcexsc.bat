@echo off

set ARCH=wcexsc

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
set TOOL="%ProgramFiles%\Microsoft eMbedded C++ 4.0\EVC\wce500\bin\wcearmv4i.bat"
if not exist %TOOL% (
 @echo WCE500 is not installed!
 exit /b
)

@call %TOOL%
@call %IPPENV%

echo %SDKROOT%


nmake clean ARCH=%ARCH%
nmake CC=%COMPILER% ARCH=%ARCH%
