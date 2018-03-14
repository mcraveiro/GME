// GMEOLEColl.cpp : implementation file
//

#include "stdafx.h"
#include "gme.h"
#include "GMEOLEColl.h"
#include "EnumVar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CStringCollect

IMPLEMENT_DYNCREATE(CGMEOLEColl, CCmdTarget)

CGMEOLEColl::CGMEOLEColl()
{
	EnableAutomation();

	// To keep the application running as long as an OLE automation
	//  object is active, the constructor calls AfxOleLockApp.

	AfxOleLockApp();
}

CGMEOLEColl::~CGMEOLEColl()
{
	// To terminate the application when all objects created with
	//  with OLE automation, the destructor calls AfxOleUnlockApp.
	RemoveAll();
	AfxOleUnlockApp();
}

void CGMEOLEColl::OnFinalRelease()
{
	// When the last reference for an automation object is released
	//  OnFinalRelease is called.  This implementation deletes the
	//  object.  Add additional cleanup required for your object before
	//  deleting it from memory.

	delete this;
}

BEGIN_MESSAGE_MAP(CGMEOLEColl, CCmdTarget)
	//{{AFX_MSG_MAP(CGMEOLEColl)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BEGIN_DISPATCH_MAP(CGMEOLEColl, CCmdTarget)
	//{{AFX_DISPATCH_MAP(CGMEOLEColl)
	DISP_PROPERTY_EX(CGMEOLEColl, "Count", GetCount, SetNotSupported, VT_I4)
	DISP_FUNCTION(CGMEOLEColl, "Add", Add, VT_EMPTY, VTS_DISPATCH)
	DISP_FUNCTION(CGMEOLEColl, "Find", Find, VT_I4, VTS_DISPATCH)
	DISP_FUNCTION(CGMEOLEColl, "Remove", Remove, VT_EMPTY, VTS_VARIANT)
	DISP_FUNCTION(CGMEOLEColl, "RemoveAll", RemoveAll, VT_EMPTY, VTS_NONE)
	DISP_PROPERTY_PARAM(CGMEOLEColl, "Item", GetItem, SetItem, VT_DISPATCH, VTS_I4)
	//}}AFX_DISPATCH_MAP
	DISP_PROPERTY_EX_ID(CGMEOLEColl, "_NewEnum", DISPID_NEWENUM, GetNewEnum, SetNotSupported, VT_UNKNOWN)
	DISP_DEFVALUE(CGMEOLEColl, "Item")
END_DISPATCH_MAP()

// {36C7B797-6BDE-46d0-8870-70189000EDF9}
//static const IID IID_IGMEOLEColl = 
//{ 0x36c7b797, 0x6bde, 0x46d0, { 0x88, 0x70, 0x70, 0x18, 0x90, 0x0, 0xed, 0xf9 } };



// Note: we add support for IID_IGMEOLEModels to support typesafe binding
// from VBA.  This IID must match the GUID that is attached to the
// dispinterface in the .ODL file.

BEGIN_INTERFACE_MAP(CGMEOLEColl, CCmdTarget)
	INTERFACE_PART(CGMEOLEColl, __uuidof(IGMEOLEColl), Dual)
	DUAL_ERRORINFO_PART(CGMEOLEColl)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStringCollect message handlers

void CGMEOLEColl::CheckIndex(long nIndex)
{
	if (nIndex <= 0 || nIndex > m_ptrArray.GetSize())
		AfxThrowOleException(E_INVALIDARG);
}

LPUNKNOWN CGMEOLEColl::GetNewEnum()
{
	std::unique_ptr<CEnumVariant> pEnum(new CEnumVariant);
	int nCount = (int)m_ptrArray.GetSize();
	std::unique_ptr<VARIANT[]> pContents(new VARIANT[nCount]);
	int i;

	TRY
	{
		for (i = 0; i < nCount; ++i)
		{
			VariantInit(&pContents[i]);
			pContents[i].pdispVal = (LPDISPATCH)(m_ptrArray.ElementAt(i));
			pContents[i].pdispVal->AddRef();
			pContents[i].vt = VT_DISPATCH;
		}
	}
	CATCH_ALL(e)
	{
		while (--i >= 0) {
			pContents[i].pdispVal->Release();
			VariantClear(&pContents[i]);
		}

		THROW_LAST();
	}
	END_CATCH_ALL
	pEnum->SetContents(pContents.release(), nCount);

	return pEnum.release()->GetInterface(&IID_IUnknown);
}

long CGMEOLEColl::GetCount()
{
	return (long)m_ptrArray.GetSize();
}

LPDISPATCH CGMEOLEColl::GetItem(long nIndex)
{
	CheckIndex(nIndex);
	LPDISPATCH ret = (LPDISPATCH)(m_ptrArray.ElementAt((int)nIndex-1));
	ret->AddRef();
	return ret;
}

void CGMEOLEColl::SetItem(long nIndex, LPDISPATCH newValue)
{
	CheckIndex(nIndex);
	m_ptrArray.ElementAt((int)nIndex-1) = newValue;	
	newValue->AddRef();
}

void CGMEOLEColl::Add(LPDISPATCH newValue)
{
	m_ptrArray.Add(newValue);
	newValue->AddRef();
}

long CGMEOLEColl::Find(LPDISPATCH findValue)
{
	int nCount = (int)m_ptrArray.GetSize();
	for (int i = 0; i < nCount; ++i)
	{
		if (m_ptrArray.ElementAt(i) == findValue)
			return i+1;
	}
	return -1;
}

void CGMEOLEColl::Remove(const VARIANT FAR& removeValue)
{
	int nIndex = -1;

	VARIANT varTemp;
	VariantInit(&varTemp);
	const VARIANT* pvar = &removeValue;
	if (removeValue.vt != VT_DISPATCH)
	{
		if (VariantChangeType(&varTemp, (VARIANT*)&removeValue, 0, VT_I4) == NOERROR)
			pvar = &varTemp;
		else if (VariantChangeType(&varTemp, (VARIANT*)&removeValue, 0, VT_DISPATCH) == NOERROR)
			pvar = &varTemp;
		else
			AfxThrowOleException(DISP_E_TYPEMISMATCH);
	}
	if (pvar->vt == VT_DISPATCH)
		nIndex = (int)Find(pvar->pdispVal);
	else if (pvar->vt == VT_I4)
		nIndex = (int)pvar->lVal;
	VariantClear(&varTemp);

	CheckIndex(nIndex);

	LPDISPATCH ptr = (LPDISPATCH)(m_ptrArray.ElementAt(nIndex));
	ptr->Release();

	m_ptrArray.RemoveAt(nIndex);
}

void CGMEOLEColl::RemoveAll()
{
	int nCount = m_ptrArray.GetSize();
	for (int i = 0; i < nCount; ++i)
	{
		LPDISPATCH ptr = (LPDISPATCH)(m_ptrArray.ElementAt(i));
		ptr->Release();
	}
	m_ptrArray.RemoveAll();
}


DELEGATE_DUAL_INTERFACE(CGMEOLEColl, Dual)

// Implement ISupportErrorInfo to indicate we support the
// OLE Automation error handler.
IMPLEMENT_DUAL_ERRORINFO(CGMEOLEColl, __uuidof(IGMEOLEColl))


STDMETHODIMP CGMEOLEColl::XDual::get_Count(long *cnt)
{
	METHOD_PROLOGUE(CGMEOLEColl, Dual)

	TRY_DUAL(__uuidof(IGMEOLEColl))
	{
		*cnt = pThis->GetCount();
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEColl::XDual::Add(IDispatch* newValue)
{
	METHOD_PROLOGUE(CGMEOLEColl, Dual)

	TRY_DUAL(__uuidof(IGMEOLEColl))
	{
		pThis->Add(newValue);
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEColl::XDual::Find(IDispatch* findValue, long *cnt)
{
	METHOD_PROLOGUE(CGMEOLEColl, Dual)

	TRY_DUAL(__uuidof(IGMEOLEColl))
	{
		*cnt = pThis->Find(findValue);
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEColl::XDual::Remove(VARIANT removeValue)
{
	METHOD_PROLOGUE(CGMEOLEColl, Dual)

	TRY_DUAL(__uuidof(IGMEOLEColl))
	{
		pThis->Remove(removeValue);
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEColl::XDual::RemoveAll()
{
	METHOD_PROLOGUE(CGMEOLEColl, Dual)

	TRY_DUAL(__uuidof(IGMEOLEColl))
	{
		pThis->RemoveAll();
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEColl::XDual::get_Item(long nIndex, IDispatch** val)
{
	METHOD_PROLOGUE(CGMEOLEColl, Dual)

	TRY_DUAL(__uuidof(IGMEOLEColl))
	{
		*val = pThis->GetItem(nIndex);
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEColl::XDual::put_Item(long nIndex, IDispatch* newValue)
{
	METHOD_PROLOGUE(CGMEOLEColl, Dual)

	TRY_DUAL(__uuidof(IGMEOLEColl))
	{
		pThis->SetItem(nIndex, newValue);
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEColl::XDual::get__NewEnum(IUnknown** e)
{
	METHOD_PROLOGUE(CGMEOLEColl, Dual)

	TRY_DUAL(__uuidof(IGMEOLEColl))
	{
		*e = pThis->GetNewEnum();
		return NOERROR;
	}
	CATCH_ALL_DUAL
}
