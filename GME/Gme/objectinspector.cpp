// Machine generated IDispatch wrapper class(es) created by Microsoft Visual C++

// NOTE: Do not modify the contents of this file.  If this class is regenerated by
//  Microsoft Visual C++, your modifications will be overwritten.


#include "stdafx.h"
#include "objectinspector.h"

/////////////////////////////////////////////////////////////////////////////
// CObjectInspector

IMPLEMENT_DYNCREATE(CObjectInspector, CWnd)

/////////////////////////////////////////////////////////////////////////////
// CObjectInspector properties

LPUNKNOWN CObjectInspector::GetMgaProject()
{
	LPUNKNOWN result;
	GetProperty(0x1, VT_UNKNOWN, (void*)&result);
	return result;
}

void CObjectInspector::SetMgaProject(LPUNKNOWN propVal)
{
	SetProperty(0x1, VT_UNKNOWN, propVal);
}

LPUNKNOWN CObjectInspector::GetMgaObject()
{
	LPUNKNOWN result;
	GetProperty(0x2, VT_UNKNOWN, (void*)&result);
	return result;
}

void CObjectInspector::SetMgaObject(LPUNKNOWN propVal)
{
	SetProperty(0x2, VT_UNKNOWN, propVal);
}

/////////////////////////////////////////////////////////////////////////////
// CObjectInspector operations

void CObjectInspector::ShowPanel(long panelID)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x3, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 panelID);
}

void CObjectInspector::EditName()
{
	InvokeHelper(0x4, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void CObjectInspector::CyclePanel(VARIANT_BOOL frwd)
{
	static BYTE parms[] =
		VTS_BOOL;
	InvokeHelper(0x5, DISPATCH_METHOD, VT_EMPTY, NULL, parms);
}

void CObjectInspector::AboutBox()
{
	InvokeHelper(0xfffffdd8, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}
