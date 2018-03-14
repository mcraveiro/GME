set DBGTOOLS=%ProgramFiles%\Debugging Tools for Windows (x86)
IF "%PROCESSOR_ARCHITECTURE%" == "AMD64" set DBGTOOLS=%ProgramFiles%\Debugging Tools for Windows (x64)
IF "%PROCESSOR_ARCHITEW6432%" == "AMD64" set DBGTOOLS=%ProgramW6432%\Debugging Tools for Windows (x64)

call "%DBGTOOLS%\srcsrv\svnindex.cmd" /debug /Ini="%~dp0srcsrv.ini" /source="%GME_ROOT%" /symbols="%GME_ROOT%"
IF %ERRORLEVEL% NEQ 0 exit /b
