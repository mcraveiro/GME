#include "stdafx.h"
#include "Console.h"

#include "Gme.h"
#include <comdef.h>

namespace GMEConsole
{
	CComPtr<IGMEOLEApp> Console::gmeoleapp = 0;

	void Console::SetupConsole(CComPtr<IMgaProject> project)
	{
		CComPtr<IMgaClient> client;	
		CComPtr<IDispatch> pDispatch;
		HRESULT s1 = project->GetClientByName(_bstr_t(L"GME.Application"), &client);

		if ((SUCCEEDED(s1)) && (client != 0))
		{
			HRESULT s2 = client->get_OLEServer(&pDispatch);
			if ((SUCCEEDED(s2)) && (pDispatch != 0) && gmeoleapp == 0)
			{
#ifdef __AFX_H__
				COMTHROW(pDispatch->QueryInterface(__uuidof(IGMEOLEApp), (void**)&gmeoleapp));
#else
				COMTHROW(pDispatch.Get()->QueryInterface(__uuidof(IGMEOLEApp), (void**)&gmeoleapp));
#endif
			}
		}
	}
	void Console::ReleaseConsole() {
		if (gmeoleapp)
		{
			gmeoleapp.Detach()->Release();
		}
	}

	void Console::WriteLine(const TCHAR* message, msgtype_enum type)
	{
		if (gmeoleapp == 0) {
			switch (type) {
			case MSG_NORMAL:
			case MSG_INFO:
			case MSG_WARNING:
				_tprintf(_T("%s\n"), message);
				break;
			case MSG_ERROR:
				_ftprintf(stderr, _T("%s\n"), message);
				break;
			}
		}
		else {
			COMTHROW(gmeoleapp->ConsoleMessage(_bstr_t(message), type));
		}
	}

	void Console::Clear()
	{
		if (gmeoleapp != 0) {
			COMTHROW(gmeoleapp->put_ConsoleContents(NULL));
		}
	}

	void Console::SetContents(const TCHAR* contents)
	{
		if (gmeoleapp != 0) {
			COMTHROW(gmeoleapp->put_ConsoleContents(_bstr_t(contents)));
		}
	}

	void Console::NavigateTo(const TCHAR* url)
	{
		if (gmeoleapp != 0) {
			COMTHROW(gmeoleapp->ConsoleNavigateTo(_bstr_t(url)));
		}
	}

	void Console::Error::WriteLine(const TCHAR* message)
	{
		Console::WriteLine(message,MSG_ERROR);
	}

	void Console::Out::WriteLine(const TCHAR* message)
	{
		Console::WriteLine(message, MSG_NORMAL);
	}
	void Console::Warning::WriteLine(const TCHAR* message)
	{
		Console::WriteLine(message, MSG_WARNING);
	}
	void Console::Info::writeLine(const TCHAR* message)
	{
		Console::WriteLine(message,MSG_INFO);
	}
	void Console::Info::WriteLine(const TCHAR* message)
	{
		Console::WriteLine(message,MSG_INFO);
	}
}


