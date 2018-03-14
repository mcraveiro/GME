#pragma once

#include "strsafe.h"

class CUACUtils
{
public:
	static bool isVistaOrLater() {
		OSVERSIONINFO osvi;

		::ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

		GetVersionEx(&osvi);

		return (osvi.dwMajorVersion >= 6);
	}

	template <typename T>
	static HRESULT CreateElevatedInstance(REFCLSID classId,
                               T** object, HWND window = 0)
	{
		BIND_OPTS3 bindOptions;
		::ZeroMemory(&bindOptions, sizeof (BIND_OPTS3));
		bindOptions.cbStruct = sizeof (BIND_OPTS3);
		bindOptions.hwnd = window;
		bindOptions.dwClassContext = CLSCTX_LOCAL_SERVER;

		WCHAR wszMonikerName[300];
		WCHAR wszCLSID[50];

		#define cntof(a) (sizeof(a)/sizeof(a[0]))

		::StringFromGUID2(classId, wszCLSID,
									 cntof(wszCLSID));

		HRESULT hr = ::StringCchPrintfW(wszMonikerName, cntof(wszMonikerName), L"Elevation:Administrator!new:%s", wszCLSID);
	
		if (FAILED(hr))
		{
			return hr;
		}

		return ::CoGetObject(wszMonikerName,
							 &bindOptions,
							 __uuidof(T),
							 reinterpret_cast<void**>(object));
	}

	static void SetShieldIcon(const CButton& button, bool on=true);
};
