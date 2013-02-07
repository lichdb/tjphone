@echo off
REM
REM Usage: buildem64t.bat  {cl}
REM

set ARCH=em64t

set SDK="%ProgramFiles%\Microsoft Platform SDK\setenv.cmd"
if exist %SDK% goto :call_sdk

set SDK="%ProgramFiles%\Microsoft Platform SDK\setenv.bat"
if exist %SDK% goto :call_sdk
@echo *************************************************************************
@echo Microsoft(R) Platform SDK was not found!
@echo Please install Microsoft(R) Platform SDK correctly.
@echo *************************************************************************
exit /b

:call_sdk
@call %SDK% /X64 /RETAIL

set ARG=%1

if "%ARG%"=="" goto :default
goto :ms

:default
set COMPILER=icl.exe
set TOOL="%ProgramFiles(x86)%\Intel\Compiler\C++\9.1\EM64T\Bin\iclvars.bat"
if exist %TOOL% goto :adjust
set TOOL="%ProgramFiles(x86)%\Intel\Compiler\C++\9.0\EM64T\Bin\iclvars.bat"
if exist %TOOL% goto :adjust
set TOOL="C:\Program Files\Intel\CPP\Compiler80\Ia32e\Bin\iclvars.bat"
if exist %TOOL% goto :adjust

:ms

set COMPILER=cl.exe

:adjust
@call %TOOL%

set IPPENV="%IPPROOT%\tools\env\ippenvem64t.bat"
if exist %IPPENV% goto :start
set IPPENV="%ProgramFiles%\Intel\IPP\5.1\em64t\tools\env\ippenvem64t.bat"
if exist %IPPENV% goto :start 
  @echo *************************************************************************
  @echo Intel(R) IPP is not found!
  @echo Please install Intel(R) IPP or set IPPROOT environment variable correctly.
  @echo *************************************************************************
  exit /b

:start
@call %IPPENV%


nmake clean ARCH=%ARCH%
nmake ARCH=%ARCH% CC=%COMPILER%
