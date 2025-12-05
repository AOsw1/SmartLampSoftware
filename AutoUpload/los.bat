@echo off
echo ===============================
echo   Flashing ESP Firmware
echo ===============================

REM --- EINSTELLUNGEN -------------------------
set COMPORT=COM5
set BAUD=460800
set FW=firmware.bin
REM -------------------------------------------

echo Verwende Port: %COMPORT%
echo Verwende Baud: %BAUD%
echo Firmware-Datei: %FW%


REM Prüfe ob Datei existiert
if not exist %FW% (
    echo Fehler: Firmware %FW% wurde nicht gefunden!
    pause
    exit /b 1
)


C:\Users\AO\.platformio\penv\Scripts\python.exe  C:\Users\AO\.platformio\packages\tool-esptoolpy\esptool.py  --chip esp32 --port %COMPORT% --baud %BAUD% write_flash -z 0x10000 %FW%

echo Fertig!
pause
