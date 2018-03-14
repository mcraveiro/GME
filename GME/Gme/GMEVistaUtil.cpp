
#define NTDDI_VERSION 0x06000000
#define _WIN32_WINNT 0x0600

#include <afxwin.h>
#include <ShObjIdl.h>
#include <atlcomcli.h>
#include <atlstr.h>

HRESULT VistaBrowseDirectory(CString& directory)
{
	CComPtr<IFileDialog> pfd;
	HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, 
					  NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd));
	if (FAILED(hr))
		return hr;

	DWORD dwFlags;
	hr = pfd->GetOptions(&dwFlags);
	if (FAILED(hr))
		return hr;
	hr = pfd->SetOptions(dwFlags | FOS_FORCEFILESYSTEM | FOS_PICKFOLDERS);
	if (FAILED(hr))
		return hr;
	hr = pfd->Show(NULL);
	if (FAILED(hr))
		return hr; // may be HRESULT_FROM_WIN32(ERROR_CANCELLED)

	CComPtr<IShellItem> psiResult;
	hr = pfd->GetResult(&psiResult);
	if (FAILED(hr))
		return hr;
	PWSTR pszFilePath = NULL;
	hr = psiResult->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
	if (FAILED(hr))
		return hr;
	directory = pszFilePath;
	CoTaskMemFree(pszFilePath);

	return hr;
}
