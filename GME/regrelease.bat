@echo off
REM	 
REM	Release Builder depends on this script
REM
REM	Please consult with Peter before editing this file
REM 

REM FIXME Do we need to regasm mgadotnetservices.dll?

echo Registering %1

echo Core.dll:
regsvr32 /s "%~dp0%1\Release\Core.dll"
if errorlevel 1 goto errorlabel
echo    OK

echo Meta.dll:
regsvr32 /s "%~dp0%1\Release\Meta.dll"
if errorlevel 1 goto errorlabel
echo    OK

echo Mga.dll:
regsvr32 /s "%~dp0%1\Release\Mga.dll"
if errorlevel 1 goto errorlabel
echo    OK

echo MgaUtil.dll:
regsvr32 /s "%~dp0%1\Release\MgaUtil.dll"
if errorlevel 1 goto errorlabel
echo    OK

echo Parser.dll:
regsvr32 /s "%~dp0%1\Release\Parser.dll"
if errorlevel 1 goto errorlabel
echo    OK

echo ConstraintManager.dll:
regsvr32 /s "%~dp0%1\Release\ConstraintManager.dll"
if errorlevel 1 goto errorlabel
echo    OK

echo GMEActiveBrowser.ocx:
regsvr32 /s "%~dp0%1\Release\GMEActiveBrowser.ocx"
if errorlevel 1 goto errorlabel
echo    OK

echo MgaDecorators.dll:
regsvr32 /s "%~dp0%1\Release\MgaDecorators.dll"
if errorlevel 1 goto errorlabel
echo    OK

echo Annotator.dll:
regsvr32 /s "%~dp0%1\Release\Annotator.dll"
if errorlevel 1 goto errorlabel
echo    OK

echo ObjectInspector.ocx:
regsvr32 /s "%~dp0%1\Release\ObjectInspector.ocx"
if errorlevel 1 goto errorlabel
echo    OK

echo Search.ocx:
regsvr32 /s "%~dp0%1\Release\Search.ocx"
if errorlevel 1 goto errorlabel
echo    OK

echo Console.ocx:
regsvr32 /s "%~dp0%1\Release\Console.ocx"
if errorlevel 1 goto errorlabel
echo    OK

echo XmlBackEnd.dll:
if %1x NEQ x64x ( regsvr32 /s "%~dp0%1\Release\XmlBackEnd.dll" )
if errorlevel 1 goto errorlabel
echo    OK

echo PartBrowser.ocx:
regsvr32 /s "%~dp0%1\Release\PartBrowser.ocx"
if errorlevel 1 goto errorlabel
echo    OK

echo PanningView.ocx:
regsvr32 /s "%~dp0%1\Release\PanningView.ocx"
if errorlevel 1 goto errorlabel
echo    OK

echo GME.exe
: FIXME regtlibv12.exe doesnt exist on Win8
if exist %windir%\Microsoft.NET\Framework\v4.0.30319\regtlibv12.exe %windir%\Microsoft.NET\Framework\v4.0.30319\regtlibv12.exe "%~dp0%1\Release\GME.exe"
if errorlevel 1 goto errorlabel

echo GME.exe
"%~dp0%1\Release\GME.exe" /regserver
if errorlevel 1 goto errorlabel
echo    OK

goto eof

:errorlabel
set tlevel=%ERRORLEVEL%
echo Error occurred. Code: %ERRORLEVEL%
pause
exit /b %tlevel%

:eof
	