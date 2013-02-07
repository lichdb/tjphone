@echo off

echo building of usc_lib
cd ./codec
call buildwcex86.bat
cd  ../

echo building of usc_speech_codec
cd ./application/usc_speech_codec
call buildwcex86.bat

echo building of echocanceller
cd  ../usc_ec
call buildwcex86.bat

cd  ../../
