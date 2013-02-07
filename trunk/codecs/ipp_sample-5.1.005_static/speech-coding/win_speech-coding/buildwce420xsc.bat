@echo off

echo building of usc_lib
cd ./codec
call buildwce420xsc.bat 
cd  ../

echo building of usc_speech_codec
cd ./application/usc_speech_codec
call buildwce420xsc.bat 

echo building of echocanceller
cd  ../usc_ec
call buildwce420xsc.bat 
cd  ../../
