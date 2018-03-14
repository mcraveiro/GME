@echo off
REM	 
REM	Release Builder depends on this script
REM
REM	Please consult with Peter before editing this file
REM 

echo Core.dll:
regsvr32 /s "%~dp0%1\Debug\Core.dll"
if errorlevel 1 goto errorlabel
echo    OK

echo Meta.dll:
regsvr32 /s "%~dp0%1\Debug\Meta.dll"
if errorlevel 1 goto errorlabel
echo    OK

echo Mga.dll:
regsvr32 /s "%~dp0%1\Debug\Mga.dll"
if errorlevel 1 goto errorlabel
echo    OK

echo MgaUtil.dll:
regsvr32 /s "%~dp0%1\Debug\MgaUtil.dll"
if errorlevel 1 goto errorlabel
echo    OK

echo Parser.dll:
regsvr32 /s "%~dp0%1\Debug\Parser.dll"
if errorlevel 1 goto errorlabel
echo    OK

echo ConstraintManager.dll:
regsvr32 /s "%~dp0%1\Debug\ConstraintManager.dll"
if errorlevel 1 goto errorlabel
echo    OK

echo GMEActiveBrowser.ocx:
regsvr32 /s "%~dp0%1\Debug\GMEActiveBrowser.ocx"
if errorlevel 1 goto errorlabel
echo    OK

echo MgaDecorators.dll:
regsvr32 /s "%~dp0%1\Debug\MgaDecorators.dll"
if errorlevel 1 goto errorlabel
echo    OK

echo Annotator.dll:
regsvr32 /s "%~dp0%1\Debug\Annotator.dll"
if errorlevel 1 goto errorlabel
echo    OK

echo ObjectInspector.ocx:
regsvr32 /s "%~dp0%1\Debug\ObjectInspector.ocx"
if errorlevel 1 goto errorlabel
echo    OK

echo Search.ocx:
regsvr32 /s "%~dp0%1\Debug\Search.ocx"
if errorlevel 1 goto errorlabel
echo    OK

echo Console.ocx:
regsvr32 /s "%~dp0%1\Debug\Console.ocx"
if errorlevel 1 goto errorlabel
echo    OK

echo XmlBackEnd.dll:
if %1x NEQ x64x ( regsvr32 /s "%~dp0%1\Debug\XmlBackEnd.dll" )
if errorlevel 1 goto errorlabel
echo    OK

echo PartBrowser.ocx:
regsvr32 /s "%~dp0%1\Debug\PartBrowser.ocx"
if errorlevel 1 goto errorlabel
echo    OK

echo PanningView.ocx:
regsvr32 /s "%~dp0%1\Debug\PanningView.ocx"
if errorlevel 1 goto errorlabel
echo    OK

goto eof

:errorlabel
set tlevel=%ERRORLEVEL%
echo Error occurred. Code: %ERRORLEVEL%
pause
exit /b %tlevel%

:eof
