#include "stdafx.h"
#include "GUIDCreate.h"

/*static*/ const TCHAR * GuidCreate::format_str = _T("{%08lX-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X}");

CComBSTR GuidCreate::newGuid()
{
	CString res;
	GUID t_guid = GUID_NULL;
	::CoCreateGuid(&t_guid);
		
	if (t_guid != GUID_NULL)
	{
		res.Format(format_str,
			t_guid.Data1, t_guid.Data2, t_guid.Data3,
			t_guid.Data4[0], t_guid.Data4[1], t_guid.Data4[2], t_guid.Data4[3],
			t_guid.Data4[4], t_guid.Data4[5], t_guid.Data4[6], t_guid.Data4[7]);
	} // else send blank

	return CComBSTR(res);
}