@echo off

echo building of usc_lib
cd ./codec
call buildwcexsc.bat
cd  ../

echo building of usc_speech_codec
cd ./application/usc_speech_codec
call buildwcexsc.bat

echo building of echocanceller
cd  ../usc_ec
call buildwcexsc.bat
cd  ../../
