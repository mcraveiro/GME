// GMEOLEModel.cpp : implementation file
//

#include "stdafx.h"
#include "gme.h"
#include "GMEOLEModel.h"
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
// CGMEOLEModel

IMPLEMENT_DYNCREATE(CGMEOLEModel, CCmdTarget)

CGMEOLEModel::CGMEOLEModel()
{
	EnableAutomation();
}

CGMEOLEModel::~CGMEOLEModel()
{
}


void CGMEOLEModel::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CCmdTarget::OnFinalRelease();
}

bool CGMEOLEModel::AmIValid() {
	if (CGMEDoc::theInstance) {
		POSITION pos = CGMEDoc::theInstance->GetFirstViewPosition();
		while (pos) {
			CGMEView* view = (CGMEView*)(CGMEDoc::theInstance->GetNextView(pos));
			ASSERT(view);
			if (view == m_view) {
				return true;
			}
		}
	}
	return false;
}


BEGIN_MESSAGE_MAP(CGMEOLEModel, CCmdTarget)
	//{{AFX_MSG_MAP(CGMEOLEModel)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CGMEOLEModel, CCmdTarget)
	//{{AFX_DISPATCH_MAP(CGMEOLEModel)
	DISP_PROPERTY_EX(CGMEOLEModel, "Active", GetActive, SetActive, VT_BOOL)
	DISP_PROPERTY_EX(CGMEOLEModel, "Aspects", GetAspects, SetAspects, VT_DISPATCH)
	DISP_PROPERTY_EX(CGMEOLEModel, "MgaModel", GetMgaModel, SetMgaModel, VT_DISPATCH)
	DISP_PROPERTY_EX(CGMEOLEModel, "Valid", GetValid, SetValid, VT_BOOL)
	DISP_FUNCTION(CGMEOLEModel, "Print", Print, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CGMEOLEModel, "PrintDialog", PrintDialog, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CGMEOLEModel, "DumpWindowsMetaFile", DumpWindowsMetaFile, VT_EMPTY, VTS_BSTR)
	DISP_FUNCTION(CGMEOLEModel, "CheckConstraints", CheckConstraints, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CGMEOLEModel, "RunComponent", RunComponent, VT_EMPTY, VTS_BSTR)
	DISP_FUNCTION(CGMEOLEModel, "RunComponentDialog", RunComponentDialog, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CGMEOLEModel, "Close", Close, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CGMEOLEModel, "GrayOutFCO", GrayOutFCO, VT_EMPTY, VTS_BOOL VTS_BOOL VTS_DISPATCH)
	DISP_FUNCTION(CGMEOLEModel, "GrayOutHide", GrayOutHide, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CGMEOLEModel, "ShowSetMembers", ShowSetMembers, VT_EMPTY, VTS_DISPATCH)
	DISP_FUNCTION(CGMEOLEModel, "HideSetMembers", HideSetMembers, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CGMEOLEModel, "Zoom", Zoom, VT_EMPTY, VTS_I4)
	DISP_FUNCTION(CGMEOLEModel, "ZoomTo", ZoomTo, VT_EMPTY, VTS_DISPATCH)
	DISP_FUNCTION(CGMEOLEModel, "Scroll", Scroll, VT_EMPTY, VTS_I2 VTS_I2)
	DISP_FUNCTION(CGMEOLEModel, "DumpModelGeometryXML", DumpModelGeometryXML, VT_HRESULT, VTS_BSTR)
	DISP_FUNCTION(CGMEOLEModel, "GetCurrentAspect", GetCurrentAspect, VT_HRESULT, VTS_BSTR)
	DISP_FUNCTION(CGMEOLEModel, "SetCurrentAspect", SetCurrentAspect, VT_HRESULT, VTS_BSTR)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IGMEOLEModel to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {399A16A0-D209-4d00-9BDF-858D87EC4641}
//static const IID IID_IGMEOLEModel = 
//{ 0x399a16a0, 0xd209, 0x4d00, { 0x9b, 0xdf, 0x85, 0x8d, 0x87, 0xec, 0x46, 0x41 } };


BEGIN_INTERFACE_MAP(CGMEOLEModel, CCmdTarget)
	INTERFACE_PART(CGMEOLEModel, __uuidof(IGMEOLEModel), Dual)
	DUAL_ERRORINFO_PART(CGMEOLEModel)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGMEOLEModel message handlers

void CGMEOLEModel::Print() 
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEModel::Print()\r\n");

	PRECONDITION_VALID_MODEL

	m_view->SendMessage(WM_COMMAND, ID_FILE_PRINT_DIRECT);
}

void CGMEOLEModel::PrintDialog() 
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEModel::PrintDialog()\r\n");

	PRECONDITION_VALID_MODEL

	m_view->SendMessage(WM_COMMAND, ID_FILE_PRINT);

}

void CGMEOLEModel::DumpWindowsMetaFile(LPCTSTR filePath) 
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEModel::DumpWindowsMetaFile()\r\n");

	PRECONDITION_VALID_MODEL

	CMetaFileDC cDC;
	BOOL ret = cDC.CreateEnhanced(m_view->GetDC(),filePath,NULL,_T("GME Model"));
	if (ret == FALSE) {
		AfxMessageBox(_T("Unable to create metafile."), MB_OK | MB_ICONSTOP);
		return;
	}

	cDC.m_bPrinting = TRUE;		// HACK by Peter (c)
	m_view->OnDraw(&cDC);

	HENHMETAFILE hEmf = cDC.CloseEnhanced();
	if ( hEmf ) {
		DeleteEnhMetaFile(hEmf);
	}
}

void CGMEOLEModel::CheckConstraints() 
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEModel::CheckConstraints()\r\n");

	PRECONDITION_VALID_MODEL
	PRECONDITION_ACTIVE_CONSTMGR

	m_view->OnFileCheck();
}

BOOL CGMEOLEModel::GetActive() 
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEModel::GetActive()\r\n");

	PRECONDITION_VALID_MODEL

	return (CGMEView::GetActiveView() == m_view) ? TRUE : FALSE;
}

void CGMEOLEModel::SetActive(BOOL bNewValue) 
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEModel::SetActive()\r\n");

	PRECONDITION_VALID_MODEL;

	if (bNewValue == TRUE) {
		CMainFrame::theInstance->ActivateView(m_view);
	}
}

void CGMEOLEModel::RunComponent(LPCTSTR appID) 
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEModel::RunComponent()\r\n");

	PRECONDITION_VALID_MODEL

	m_view->RunComponent(appID);
}

void CGMEOLEModel::RunComponentDialog() 
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEModel::RunComponentDialog()\r\n");

	PRECONDITION_VALID_MODEL

	m_view->RunComponent("");
}

LPDISPATCH CGMEOLEModel::GetAspects() 
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEModel::GetAspects()\r\n");

	PRECONDITION_VALID_MODEL

	CGMEOLEColl* coll = new CGMEOLEColl();

	POSITION pos = m_view->guiMeta->aspects.GetHeadPosition();
	while (pos) {
		CGuiMetaAspect* guiaspect = (CGuiMetaAspect*)(m_view->guiMeta->aspects.GetNext(pos));
		ASSERT(guiaspect);
		CGMEOLEAspect* aspect = new CGMEOLEAspect();
		aspect->SetGuiParams(m_view, guiaspect);
		coll->Add(aspect->GetIDispatch(FALSE));
		aspect->GetIDispatch(FALSE)->Release();	// We do not hold reference, hopefully the collection will hold one
	}

	return coll->GetIDispatch(FALSE);
}

void CGMEOLEModel::SetAspects(LPDISPATCH) 
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEModel::SetAspects()\r\n");

	SetNotSupported();
}

LPDISPATCH CGMEOLEModel::GetMgaModel() 
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEModel::GetMgaModel()\r\n");

	PRECONDITION_VALID_MODEL

	return m_view->currentModel;

	//return NULL;//unreachable code
}

void CGMEOLEModel::SetMgaModel(LPDISPATCH) 
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEModel::SetMgaModel()\r\n");

	SetNotSupported();
}

BOOL CGMEOLEModel::GetValid() 
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEModel::GetValid()\r\n");
	
	// PRECONDITIONS: None

	return (AmIValid() ? TRUE : FALSE);
}

void CGMEOLEModel::SetValid(BOOL) 
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEModel::SetValid()\r\n");

	SetNotSupported();
}

void CGMEOLEModel::Close() 
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEModel::Close()\r\n");

	PRECONDITION_VALID_MODEL;

	m_view->GetParentFrame()->DestroyWindow();
}

void CGMEOLEModel::GrayOutFCO(BOOL bGray, BOOL bNeighbours, LPDISPATCH mgaFCOs) 
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEModel::GrayOutFCO()\r\n");

	PRECONDITION_VALID_MODEL;

	if (CGMEDoc::theInstance) 
		CGMEDoc::theInstance->SetMode(5);

	CComPtr<IDispatch> alienFCOs(mgaFCOs);
	CComPtr<IMgaFCOs> mFCOColl;
	if (!SUCCEEDED(alienFCOs.QueryInterface(&mFCOColl))) {
		return;
	}

	long l = 0;
	COMTHROW( mFCOColl->get_Count( &l ));
	
	for( long i = 1; i <= l; ++i)
	{
		CComPtr<IMgaFCO> item;
		COMTHROW( mFCOColl->get_Item( i, &item));

		// collection is coming from an unknown territory, do not use it as-is. (except for this search)
		CGuiObject *gObj = CGuiObject::FindObject(item, m_view->children);
		CGuiConnection *gConn = CGuiConnection::FindConnection(item, m_view->connections);

		if (gObj) {
			gObj->GrayOut(bGray == TRUE);
			if (bNeighbours == TRUE) {
				gObj->GrayOutNeighbors();
			}
			CGuiFco::GrayOutNonInternalConnections(m_view->connections);
		}
		else if (gConn) {
			gConn->GrayOut(bGray == TRUE);
			gConn->GrayOutEndPoints();
			if (bNeighbours == TRUE) {
				CGuiFco::GrayOutNonInternalConnections(m_view->connections);
			}
		}
	}

	m_view->Invalidate();
}

void CGMEOLEModel::GrayOutHide() 
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEModel::GrayOutHide()\r\n");

	PRECONDITION_VALID_MODEL;

	if (CGMEDoc::theInstance) 
		CGMEDoc::theInstance->SetMode(5);

	CGuiFco::GrayOutFcos(m_view->children, TRUE);
	CGuiFco::GrayOutFcos(m_view->connections, TRUE);
	m_view->Invalidate();
}

void CGMEOLEModel::ShowSetMembers(LPDISPATCH mgaFCO) 
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEModel::ShowSetMembers()\r\n");

	PRECONDITION_VALID_MODEL;
  
	if (CGMEDoc::theInstance) 
		CGMEDoc::theInstance->SetMode(3);

	CGuiFco::GrayOutFcos(m_view->children, TRUE);

	CComPtr<IDispatch> alienFCO(mgaFCO);
	CComPtr<IMgaFCO> mFCO;
	if (!SUCCEEDED(alienFCO.QueryInterface(&mFCO))) 
		return;

	// mgaFCO is comming from an unknown territory, do not use it as-is. (except for this search)
	CGuiObject *gObj = CGuiObject::FindObject(mFCO, m_view->children);
	if (gObj) 
	{
		gObj->GrayOut(false);

		// members of the set too 
		CComPtr<IMgaSet> mSet;
		if (!SUCCEEDED(mFCO.QueryInterface(&mSet))) 
			return;
		m_view->BeginTransaction();

		try
		{
			CComPtr<IMgaFCOs> mSetMembers = NULL;
			COMTHROW(mSet->get_Members(&mSetMembers));

			long num = 0;
			COMTHROW(mSetMembers->get_Count(&num));
			for (int i=1; i<=num; i++)
			{
				CComPtr<IMgaFCO> memb = NULL;
				COMTHROW(mSetMembers->get_Item(i, &memb));
				CGuiObject *gObj = CGuiObject::FindObject(memb, m_view->children);
				if (gObj) 
					gObj->GrayOut(false);
			}
			m_view->CommitTransaction();
		}
		catch(hresult_exception &e) 
		{
			m_view->AbortTransaction(e.hr);
		}
	}

	m_view->Invalidate();
}

void CGMEOLEModel::HideSetMembers() 
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEModel::HideSetMembers()\r\n");

	PRECONDITION_VALID_MODEL;
	 
	if (CGMEDoc::theInstance) 
		CGMEDoc::theInstance->SetMode(3);

	CGuiFco::GrayOutFcos(m_view->children, TRUE);
	m_view->Invalidate();
}

void CGMEOLEModel::Zoom(long percent) 
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEModel::Zoom()\r\n");

	PRECONDITION_VALID_MODEL;

	m_view->ZoomPercent(percent);
	m_view->Invalidate();
}

void CGMEOLEModel::ZoomTo(LPDISPATCH mgaFCOs) 
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEModel::ZoomTo()\r\n");

	PRECONDITION_VALID_MODEL;
 
	if (CGMEDoc::theInstance) 
		CGMEDoc::theInstance->SetMode(4);

	CComPtr<IDispatch> disp(mgaFCOs);
	CComPtr<IMgaFCOs> fcos;
	if (!SUCCEEDED(disp.QueryInterface(&fcos))) 
		return;

	// calculate the total size of them
	CRect totalr(0,0,0,0);
	m_view->BeginTransaction();
	try
	{
		long num = 0;
		COMTHROW(fcos->get_Count(&num));
		for (int i=1; i<=num; i++)
		{
			CComPtr<IMgaFCO> memb = NULL;
			COMTHROW(fcos->get_Item(i, &memb));
			// size ??
			CGuiObject *gObj = CGuiObject::FindObject(memb, m_view->children);
			if (gObj) 
			{
				CRect rec = gObj->GetLocation();
				totalr.UnionRect(totalr, rec);
			}
		}
		m_view->CommitTransaction();
	}
	catch(hresult_exception &e) 
	{
		m_view->AbortTransaction(e.hr);
	}

	m_view->ZoomToFCOs(totalr);
	m_view->Invalidate();

}

void CGMEOLEModel::Scroll(long bar, long scroll) 
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEModel::Scroll()\r\n");

	PRECONDITION_VALID_MODEL;

	UINT code = SB_LINEDOWN;
	switch (scroll)
	{
	case 0:
		code = SB_LINEUP;
		break;
	case 1: 
		code = SB_LINEDOWN;
		break;
	case 2:
		code = SB_PAGEUP;
		break;
	case 3: 
		code = SB_PAGEDOWN;
		break;
	case 4:
		code = SB_TOP;
		break;
	case 5:
		code = SB_BOTTOM;
		break;
	}

	if (bar == 0)
		m_view->OnHScroll(code, 1, NULL);
	else
		m_view->OnVScroll(code, 1, NULL);

	m_view->Invalidate();
}

HRESULT CGMEOLEModel::DumpModelGeometryXML(LPCTSTR filePath) 
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEModel::DumpModelGeometryXML()\r\n");

	PRECONDITION_VALID_MODEL

	return m_view->DumpModelGeometryXML(filePath);
}

CString CGMEOLEModel::GetCurrentAspect(void) 
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEModel::GetCurrentAspect()\r\n");

	PRECONDITION_VALID_MODEL

	return m_view->GetCurrentAspectName();
}

void CGMEOLEModel::SetCurrentAspect(const CString& aspectName) 
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEModel::SetCurrentAspect()\r\n");

	PRECONDITION_VALID_MODEL

	m_view->ChangeAspect(aspectName);
}

DELEGATE_DUAL_INTERFACE(CGMEOLEModel, Dual)

// Implement ISupportErrorInfo to indicate we support the
// OLE Automation error handler.
IMPLEMENT_DUAL_ERRORINFO(CGMEOLEModel, __uuidof(IGMEOLEModel))


STDMETHODIMP CGMEOLEModel::XDual::put_Active(VARIANT_BOOL isActive)
{
	METHOD_PROLOGUE(CGMEOLEModel, Dual)

	TRY_DUAL(__uuidof(IGMEOLEModel))
	{
		pThis->SetActive(isActive);
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEModel::XDual::get_Active(VARIANT_BOOL* isActive)
{
	METHOD_PROLOGUE(CGMEOLEModel, Dual)

	TRY_DUAL(__uuidof(IGMEOLEModel))
	{
		*isActive = (pThis->GetActive() == FALSE) ? VARIANT_FALSE : VARIANT_TRUE;
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEModel::XDual::get_Aspects(IGMEOLEColl** coll)
{
	METHOD_PROLOGUE(CGMEOLEModel, Dual)

	TRY_DUAL(__uuidof(IGMEOLEModel))
	{
		LPDISPATCH lpDisp = pThis->GetAspects();
		lpDisp->QueryInterface(__uuidof(IGMEOLEColl), (LPVOID*)coll);
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEModel::XDual::get_MgaModel(IMgaModel** model)
{
	METHOD_PROLOGUE(CGMEOLEModel, Dual)

	TRY_DUAL(__uuidof(IGMEOLEModel))
	{
		LPDISPATCH lpDisp = pThis->GetMgaModel();
		lpDisp->QueryInterface(__uuidof(IMgaModel), (LPVOID*)model);
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEModel::XDual::get_Valid(VARIANT_BOOL* isValid)
{
	METHOD_PROLOGUE(CGMEOLEModel, Dual)

	TRY_DUAL(__uuidof(IGMEOLEModel))
	{
		*isValid = (pThis->GetValid() == FALSE) ? VARIANT_FALSE : VARIANT_TRUE;
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEModel::XDual::Print()
{
	METHOD_PROLOGUE(CGMEOLEModel, Dual)

	TRY_DUAL(__uuidof(IGMEOLEModel))
	{
		pThis->Print();
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEModel::XDual::PrintDialog()
{
	METHOD_PROLOGUE(CGMEOLEModel, Dual)

	TRY_DUAL(__uuidof(IGMEOLEModel))
	{
		pThis->PrintDialog();
		return NOERROR;
	}
	CATCH_ALL_DUAL
}


STDMETHODIMP CGMEOLEModel::XDual::DumpWindowsMetaFile(BSTR filePath)
{
	METHOD_PROLOGUE(CGMEOLEModel, Dual)

	TRY_DUAL(__uuidof(IGMEOLEModel))
	{
		pThis->DumpWindowsMetaFile(CString(filePath));
		return NOERROR;
	}
	CATCH_ALL_DUAL
}


STDMETHODIMP CGMEOLEModel::XDual::CheckConstraints()
{
	METHOD_PROLOGUE(CGMEOLEModel, Dual)

	TRY_DUAL(__uuidof(IGMEOLEModel))
	{
		pThis->CheckConstraints();
		return NOERROR;
	}
	CATCH_ALL_DUAL
}


STDMETHODIMP CGMEOLEModel::XDual::RunComponent(BSTR appID)
{
	METHOD_PROLOGUE(CGMEOLEModel, Dual)

	TRY_DUAL(__uuidof(IGMEOLEModel))
	{
		pThis->RunComponent(CString(appID));
		return NOERROR;
	}
	CATCH_ALL_DUAL
}


STDMETHODIMP CGMEOLEModel::XDual::RunComponentDialog()
{
	METHOD_PROLOGUE(CGMEOLEModel, Dual)

	TRY_DUAL(__uuidof(IGMEOLEModel))
	{
		pThis->RunComponentDialog();
		return NOERROR;
	}
	CATCH_ALL_DUAL
}


STDMETHODIMP CGMEOLEModel::XDual::Close()
{
	METHOD_PROLOGUE(CGMEOLEModel, Dual)

	TRY_DUAL(__uuidof(IGMEOLEModel))
	{
		pThis->Close();
		return NOERROR;
	}
	CATCH_ALL_DUAL
}


STDMETHODIMP CGMEOLEModel::XDual::GrayOutFCO(VARIANT_BOOL bGray, VARIANT_BOOL bNeighbours, IMgaFCOs* mgaFCO)
{
	METHOD_PROLOGUE(CGMEOLEModel, Dual)

	TRY_DUAL(__uuidof(IGMEOLEModel))
	{
		LPDISPATCH lpDisp = NULL;
		mgaFCO->QueryInterface(IID_IDispatch, (LPVOID*)&lpDisp);
		pThis->GrayOutFCO(bGray, bNeighbours, lpDisp);
		lpDisp->Release();
		return NOERROR;
	}
	CATCH_ALL_DUAL
}


STDMETHODIMP CGMEOLEModel::XDual::GrayOutHide()
{
	METHOD_PROLOGUE(CGMEOLEModel, Dual)

	TRY_DUAL(__uuidof(IGMEOLEModel))
	{
		pThis->GrayOutHide();
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEModel::XDual::ShowSetMembers(IMgaFCO* mgaFCO)
{
	METHOD_PROLOGUE(CGMEOLEModel, Dual)

	TRY_DUAL(__uuidof(IGMEOLEModel))
	{
		LPDISPATCH lpDisp = NULL;
		mgaFCO->QueryInterface(IID_IDispatch, (LPVOID*)&lpDisp);
		pThis->ShowSetMembers(lpDisp);
		lpDisp->Release();
		return NOERROR;
	}
	CATCH_ALL_DUAL
}


STDMETHODIMP CGMEOLEModel::XDual::HideSetMembers()
{
	METHOD_PROLOGUE(CGMEOLEModel, Dual)

	TRY_DUAL(__uuidof(IGMEOLEModel))
	{
		pThis->HideSetMembers();
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEModel::XDual::Zoom(DWORD percent)
{
	METHOD_PROLOGUE(CGMEOLEModel, Dual)

	TRY_DUAL(__uuidof(IGMEOLEModel))
	{
		pThis->Zoom(percent);
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEModel::XDual::ZoomTo(IMgaFCOs* mgaFCOs)
{
	METHOD_PROLOGUE(CGMEOLEModel, Dual)

	TRY_DUAL(__uuidof(IGMEOLEModel))
	{
		pThis->ZoomTo(mgaFCOs);
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEModel::XDual::Scroll(bar_enum bar, scroll_enum scroll)
{
	METHOD_PROLOGUE(CGMEOLEModel, Dual)

	TRY_DUAL(__uuidof(IGMEOLEModel))
	{
		pThis->Scroll(bar, scroll);
		return NOERROR;
	}
	CATCH_ALL_DUAL
}


STDMETHODIMP CGMEOLEModel::XDual::DumpModelGeometryXML(BSTR filePath)
{
	METHOD_PROLOGUE(CGMEOLEModel, Dual)

	TRY_DUAL(__uuidof(IGMEOLEModel))
	{
		return pThis->DumpModelGeometryXML(CString(filePath));
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEModel::XDual::GetCurrentAspect(BSTR* aspectName)
{
	METHOD_PROLOGUE(CGMEOLEModel, Dual)

	TRY_DUAL(__uuidof(IGMEOLEModel))
	{
		CComBSTR string_asp = pThis->GetCurrentAspect();
		*aspectName = string_asp.Detach();
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEModel::XDual::SetCurrentAspect(BSTR aspectName)
{
	METHOD_PROLOGUE(CGMEOLEModel, Dual)

	TRY_DUAL(__uuidof(IGMEOLEModel))
	{
		pThis->SetCurrentAspect(CString(aspectName));
		return S_OK;
	}
	CATCH_ALL_DUAL
}
