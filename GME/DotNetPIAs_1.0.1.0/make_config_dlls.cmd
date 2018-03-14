set AL="C:\Program Files (x86)\Microsoft SDKs\Windows\v7.0A\Bin\al.exe"
set GACUTIL="C:\Program Files (x86)\Microsoft SDKs\Windows\v7.0A\Bin\gacutil.exe"

pushd "%~dp0"
%AL% /link:policy.1.0.GME.MGA.Core.config /out:policy.1.0.GME.MGA.Core.dll /keyfile:..\MgaDotNetServices\MgaDotNetServicesKey.snk /platform:anycpu /version:1.0.0.0
%AL% /link:policy.1.0.GME.MGA.Meta.config /out:policy.1.0.GME.MGA.Meta.dll /keyfile:..\MgaDotNetServices\MgaDotNetServicesKey.snk /platform:anycpu /version:1.0.0.0
%AL% /link:policy.1.0.GME.MGA.config /out:policy.1.0.GME.MGA.dll /keyfile:..\MgaDotNetServices\MgaDotNetServicesKey.snk /platform:anycpu /version:1.0.0.0
%AL% /link:policy.1.0.GME.config /out:policy.1.0.GME.dll /keyfile:..\MgaDotNetServices\MgaDotNetServicesKey.snk /platform:anycpu /version:1.0.0.0
%AL% /link:policy.1.0.GME.Util.config /out:policy.1.0.GME.Util.dll /keyfile:..\MgaDotNetServices\MgaDotNetServicesKey.snk /platform:anycpu /version:1.0.0.0
%AL% /link:policy.1.0.GME.MGA.Parser.config /out:policy.1.0.GME.MGA.Parser.dll /keyfile:..\MgaDotNetServices\MgaDotNetServicesKey.snk /platform:anycpu /version:1.0.0.0

FOR %%F IN (*.dll) DO %GACUTIL% /i %%F