@echo off
REM	 
REM	Release Builder depends on this script
REM

echo Registering %1

echo Core.dll:
regsvr32 /s "%~dp0%1\Release_PGO\Core.dll"
if errorlevel 1 goto errorlabel
echo    OK

echo Mga.dll:
regsvr32 /s "%~dp0%1\Release_PGO\Mga.dll"
if errorlevel 1 goto errorlabel
echo    OK

echo Meta.dll:
regsvr32 /s "%~dp0%1\Release_PGO\Meta.dll"
if errorlevel 1 goto errorlabel
echo    OK

goto eof

:errorlabel
set tlevel=%ERRORLEVEL%
echo Error occurred. Code: %ERRORLEVEL%
pause
exit /b %tlevel%

:eof
