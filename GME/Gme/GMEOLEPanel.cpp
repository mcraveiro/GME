// GMEOLEPanel.cpp : implementation file
//

#include "stdafx.h"
#include "gme.h"
#include "GMEOLEPanel.h"
#include "GMEOLEError.h"
#include "GMEView.h"
#include "MainFrm.h"
#include "GMEEventLogger.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGMEOLEPanel

IMPLEMENT_DYNCREATE(CGMEOLEPanel, CCmdTarget)

CGMEOLEPanel::CGMEOLEPanel()
{
	EnableAutomation();
}

CGMEOLEPanel::~CGMEOLEPanel()
{
}


void CGMEOLEPanel::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CCmdTarget::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(CGMEOLEPanel, CCmdTarget)
	//{{AFX_MSG_MAP(CGMEOLEPanel)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CGMEOLEPanel, CCmdTarget)
	//{{AFX_DISPATCH_MAP(CGMEOLEPanel)
	DISP_PROPERTY_EX(CGMEOLEPanel, "Name", GetName, SetName, VT_BSTR)
	DISP_PROPERTY_EX(CGMEOLEPanel, "Visible", GetVisible, SetVisible, VT_BOOL)
	DISP_PROPERTY_EX(CGMEOLEPanel, "Interface", GetInterface, SetInterface, VT_DISPATCH)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IGMEOLEPanel to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {C047FAEB-7AE7-40fb-8C5C-90C73DAD4BC6}
//static const GUID <<name>> = 
//{ 0xc047faeb, 0x7ae7, 0x40fb, { 0x8c, 0x5c, 0x90, 0xc7, 0x3d, 0xad, 0x4b, 0xc6 } };


BEGIN_INTERFACE_MAP(CGMEOLEPanel, CCmdTarget)
	INTERFACE_PART(CGMEOLEPanel, __uuidof(IGMEOLEPanel), Dual)
	DUAL_ERRORINFO_PART(CGMEOLEPanel)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGMEOLEPanel message handlers

BSTR CGMEOLEPanel::GetName() 
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEPanel::GetName()\r\n");
	
	PRECONDITION_VALID_PANEL

	return m_name.AllocSysString();
}

void CGMEOLEPanel::SetName(LPCTSTR) 
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEPanel::SetName()\r\n");
	
	SetNotSupported();
}

BOOL CGMEOLEPanel::GetVisible() 
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEPanel::GetVisible()\r\n");
	
	PRECONDITION_VALID_PANEL

	return m_control->IsVisible();
}

void CGMEOLEPanel::SetVisible(BOOL bNewValue) 
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEPanel::SetVisible()\r\n");
	
	PRECONDITION_VALID_PANEL

	CMainFrame::theInstance->ShowPane(m_control, bNewValue, FALSE, FALSE);
}

IDispatch* CGMEOLEPanel::GetInterface()
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEPanel::GetInterface()\r\n");
	
	PRECONDITION_VALID_PANEL
	if (m_interf == NULL)
		AfxThrowOleDispatchException(ID_ERROR_PTR_INVALID, _T("GUI panel is invalid."));
/*
	IDispatch* interf = NULL;
	CSizingControlBar * scb = dynamic_cast<CSizingControlBar *>( m_control);
	if( scb) interf = scb->GetInterface();
	*/
	m_interf->AddRef();
	return m_interf;
}

void CGMEOLEPanel::SetInterface(IDispatch* intf)
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEPanel::SetInterface()\r\n");
	
	SetNotSupported();
}


DELEGATE_DUAL_INTERFACE(CGMEOLEPanel, Dual)

// Implement ISupportErrorInfo to indicate we support the
// OLE Automation error handler.
IMPLEMENT_DUAL_ERRORINFO(CGMEOLEPanel, __uuidof(IGMEOLEPanel))


STDMETHODIMP CGMEOLEPanel::XDual::get_Name(BSTR* name)
{
	METHOD_PROLOGUE(CGMEOLEPanel, Dual)

	TRY_DUAL(__uuidof(IGMEOLEPanel))
	{
		CString str = pThis->GetName();
		str.SetSysString(name);
		return NOERROR;
	}
	CATCH_ALL_DUAL
}


STDMETHODIMP CGMEOLEPanel::XDual::put_Visible(VARIANT_BOOL isVisible)
{
	METHOD_PROLOGUE(CGMEOLEPanel, Dual)

	TRY_DUAL(__uuidof(IGMEOLEPanel))
	{
		pThis->SetVisible(isVisible);
		return NOERROR;
	}
	CATCH_ALL_DUAL
}


STDMETHODIMP CGMEOLEPanel::XDual::get_Visible(VARIANT_BOOL* isVisible)
{
	METHOD_PROLOGUE(CGMEOLEPanel, Dual)

	TRY_DUAL(__uuidof(IGMEOLEPanel))
	{
		*isVisible = (pThis->GetVisible() == FALSE) ? VARIANT_FALSE : VARIANT_TRUE;
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEPanel::XDual::get_Interface(IDispatch** interf)
{
	METHOD_PROLOGUE(CGMEOLEPanel, Dual)

	TRY_DUAL(__uuidof(IGMEOLEPanel))
	{
		*interf = pThis->GetInterface();
		return NOERROR;
	}
	CATCH_ALL_DUAL
}