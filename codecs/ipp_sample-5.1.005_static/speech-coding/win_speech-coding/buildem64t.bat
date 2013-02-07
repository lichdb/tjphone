@echo off

echo building of usc_lib
cd ./codec
call buildem64t.bat %1
cd  ../

echo building of usc_speech_codec
cd ./application/usc_speech_codec
call buildem64t.bat %1

echo building of echocanceller
cd  ../usc_ec
call buildem64t.bat %1

cd  ../../
