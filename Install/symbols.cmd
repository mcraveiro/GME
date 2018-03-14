set DBGTOOLS=%ProgramFiles%\Debugging Tools for Windows (x86)
IF "%PROCESSOR_ARCHITECTURE%" == "AMD64" set DBGTOOLS=%ProgramFiles%\Debugging Tools for Windows (x64)
IF "%PROCESSOR_ARCHITEW6432%" == "AMD64" set DBGTOOLS=%ProgramW6432%\Debugging Tools for Windows (x64)

set VERSION=10.12.2

"%DBGTOOLS%\symstore.exe" add /f %GME_ROOT% /s \\atlantis\project\GME\symbols /r /t GME /v %VERSION% /c "GME Release %VERSION% symbols added" /compress
