@echo off
REM
REM Usage: build32.bat  {cl}
REM

set ARCH=ia32

set ARG=%1

if "%ARG%"=="cl" goto :ms
goto :default

:default

set COMPILER=icl.exe
set TOOL="%ProgramFiles%\Intel\Compiler\C++\9.1\IA32\Bin\iclvars.bat"
if exist %TOOL% goto :adjust
set TOOL="%ProgramFiles%\Intel\Compiler\C++\9.0\IA32\Bin\iclvars.bat"
if exist %TOOL% goto :adjust
set TOOL="%ProgramFiles%\Intel\CPP\Compiler80\Ia32\Bin\iclvars.bat"
if exist %TOOL% goto :adjust


:ms
set COMPILER=cl.exe
set TOOL="%ProgramFiles%\Microsoft Visual Studio 8\Common7\Tools\vsvars32.bat"
if exist %TOOL%  goto :adjust

set TOOL="%ProgramFiles%\Microsoft Visual Studio .NET 2003\Common7\Tools\vsvars32.bat"
if exist %TOOL%  goto :adjust

set TOOL="%ProgramFiles%\Microsoft Visual Studio .NET\Common7\Tools\vsvars32.bat"
if exist %TOOL%   goto :adjust

set TOOL="%ProgramFiles%\Microsoft Visual Studio\VC98\Bin\vcvars32.bat"

:adjust
@call %TOOL%

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
@call %IPPENV%

nmake clean ARCH=%ARCH%
nmake ARCH=%ARCH% CC=%COMPILER%
