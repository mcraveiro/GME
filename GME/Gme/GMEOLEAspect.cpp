// GMEOLEAspect.cpp : implementation file
//

#include "stdafx.h"
#include "gme.h"
#include "GMEOLEAspect.h"
#include "GMEOLEError.h"
#include "GMEOLEColl.h"
#include "GMEDoc.h"
#include "MainFrm.h"
#include "GMEEventLogger.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGMEOLEAspect

IMPLEMENT_DYNCREATE(CGMEOLEAspect, CCmdTarget)

CGMEOLEAspect::CGMEOLEAspect()
{
	EnableAutomation();
}

CGMEOLEAspect::~CGMEOLEAspect()
{
}


void CGMEOLEAspect::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CCmdTarget::OnFinalRelease();
}


bool CGMEOLEAspect::AmIValid() {

	if (CGMEDoc::theInstance) {
		POSITION pos = CGMEDoc::theInstance->GetFirstViewPosition();
		while (pos) {
#if !defined (ACTIVEXGMEVIEW)
			CGMEView* view = (CGMEView*)(CGMEDoc::theInstance->GetNextView(pos));
			ASSERT(view);
			if (view == m_view) {
				POSITION apos = view->guiMeta->aspects.GetHeadPosition();
				while (apos) {
					CGuiMetaAspect *guiAspect = view->guiMeta->aspects.GetNext(apos);
					ASSERT(guiAspect);
					if (guiAspect == m_guiAspect) {
						return true;
					}
				}
			}
#endif
		}
	}
	return false;
}

BEGIN_MESSAGE_MAP(CGMEOLEAspect, CCmdTarget)
	//{{AFX_MSG_MAP(CGMEOLEAspect)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CGMEOLEAspect, CCmdTarget)
	//{{AFX_DISPATCH_MAP(CGMEOLEAspect)
	DISP_PROPERTY_EX(CGMEOLEAspect, "MgaAspect", GetMgaAspect, SetMgaAspect, VT_DISPATCH)
	DISP_PROPERTY_EX(CGMEOLEAspect, "Active", GetActive, SetActive, VT_BOOL)
	DISP_PROPERTY_EX(CGMEOLEAspect, "Valid", GetValid, SetValid, VT_BOOL)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IGMEOLEAspect to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {EDEEB1F8-BC44-40d2-BE91-83E8CCD59845}
//static const IID IID_IGMEOLEAspect = 
//{ 0xedeeb1f8, 0xbc44, 0x40d2, { 0xbe, 0x91, 0x83, 0xe8, 0xcc, 0xd5, 0x98, 0x45 } };


BEGIN_INTERFACE_MAP(CGMEOLEAspect, CCmdTarget)
	INTERFACE_PART(CGMEOLEAspect, __uuidof(IGMEOLEAspect), Dual)
	DUAL_ERRORINFO_PART(CGMEOLEAspect)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGMEOLEAspect message handlers

LPDISPATCH CGMEOLEAspect::GetMgaAspect() 
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEAspect::GetMgaAspect()\r\n");

	PRECONDITION_VALID_ASPECT;

	return m_guiAspect->mgaMeta;
}

void CGMEOLEAspect::SetMgaAspect(LPDISPATCH) 
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEAspect::SetMgaAspect()\r\n");
	
	SetNotSupported();
}

BOOL CGMEOLEAspect::GetActive() 
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEAspect::GetActive()\r\n");

	PRECONDITION_VALID_ASPECT;

	return (m_view->currentAspect == m_guiAspect) ? TRUE : FALSE;
}

void CGMEOLEAspect::SetActive(BOOL bNewValue) 
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEAspect::SetActive()\r\n");

	PRECONDITION_VALID_ASPECT;

	if (bNewValue == TRUE) {
		m_view->ChangeAspect(m_guiAspect->name);
	}
}


BOOL CGMEOLEAspect::GetValid() 
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEAspect::GetValid()\r\n");
	
	// PRECONDITIONS: None

	return (AmIValid() ? TRUE : FALSE);
}

void CGMEOLEAspect::SetValid(BOOL) 
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEAspect::SetValid()\r\n");

	SetNotSupported();

}

DELEGATE_DUAL_INTERFACE(CGMEOLEAspect, Dual)

// Implement ISupportErrorInfo to indicate we support the
// OLE Automation error handler.
IMPLEMENT_DUAL_ERRORINFO(CGMEOLEAspect, __uuidof(IGMEOLEAspect))

STDMETHODIMP CGMEOLEAspect::XDual::get_MgaAspect(IMgaMetaAspect** aspect)
{
	METHOD_PROLOGUE(CGMEOLEAspect, Dual)

	TRY_DUAL(__uuidof(IGMEOLEAspect))
	{
		LPDISPATCH lpDisp = pThis->GetMgaAspect();
		lpDisp->QueryInterface(__uuidof(IMgaMetaAspect), (LPVOID*)aspect);
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEAspect::XDual::put_Active(VARIANT_BOOL isActive)
{
	METHOD_PROLOGUE(CGMEOLEAspect, Dual)

	TRY_DUAL(__uuidof(IGMEOLEAspect))
	{
		pThis->SetActive(isActive);
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEAspect::XDual::get_Active(VARIANT_BOOL* isActive)
{
	METHOD_PROLOGUE(CGMEOLEAspect, Dual)

	TRY_DUAL(__uuidof(IGMEOLEAspect))
	{
		*isActive = (pThis->GetActive() == FALSE) ? VARIANT_FALSE : VARIANT_TRUE;
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEAspect::XDual::get_Valid(VARIANT_BOOL* isValid)
{
	METHOD_PROLOGUE(CGMEOLEAspect, Dual)

	TRY_DUAL(__uuidof(IGMEOLEAspect))
	{
		*isValid = (pThis->GetValid() == FALSE) ? VARIANT_FALSE : VARIANT_TRUE;
		return NOERROR;
	}
	CATCH_ALL_DUAL
}
