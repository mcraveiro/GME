//################################################################################################
//
// Meta Decorator COM side Implementation
//	MetaDecoratorImpl.cpp
//
//################################################################################################

#include "StdAfx.h"
#include "DecoratorConfig.h"
#include "MetaDecoratorImpl.h"
#include "MetaDecorator.h"
#include "DecoratorExceptions.h"
#include "DecoratorInterface.h"


//################################################################################################
//
// CLASS : CMetaDecoratorImpl
//
//################################################################################################

#define VERIFY_INITIALIZATION					\
	if (!m_pElementDecorator)					\
		return E_DECORATOR_UNINITIALIZED;

#define VERIFY_LOCATION							\
	if (!m_bLocationSet)						\
		return E_DECORATOR_LOCISNOTSET;

CMetaDecoratorImpl::CMetaDecoratorImpl():
	m_pElementDecorator	(NULL),
	m_bLocationSet		(false),
	m_bInitCallFromEx	(false)
{
}

CMetaDecoratorImpl::~CMetaDecoratorImpl()
{
}

STDMETHODIMP CMetaDecoratorImpl::Initialize(IMgaProject* pProject, IMgaMetaPart* pPart, IMgaFCO* pFCO)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (pFCO && !m_bInitCallFromEx)
		return E_DECORATOR_USING_DEPRECATED_FUNCTION;

	return S_OK;
}

STDMETHODIMP CMetaDecoratorImpl::Destroy()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	VERIFY_INITIALIZATION

	HRESULT retVal = S_OK;
	try {
		m_pElementDecorator->Destroy();
	}
	catch(hresult_exception& e) {
		retVal = e.hr;
	}
	catch(DecoratorException& e) {
		retVal = e.GetHResult();
	}

	m_bLocationSet = false;
	if (m_pElementDecorator) {
		delete m_pElementDecorator;
		m_pElementDecorator = NULL;
	}

	return retVal;
}

STDMETHODIMP CMetaDecoratorImpl::GetMnemonic(BSTR* bstrMnemonic)
{
	*bstrMnemonic = CComBSTR(DECORATOR_NAME).Detach();

	return S_OK;
}

STDMETHODIMP CMetaDecoratorImpl::GetFeatures(feature_code* pFeatureCodes)
{
	if (m_pElementDecorator != NULL)
		*pFeatureCodes = m_pElementDecorator->GetFeatures();
	else
		*pFeatureCodes = F_RESIZABLE | F_HASLABEL | F_HASPORTS;

	return S_OK;
}

STDMETHODIMP CMetaDecoratorImpl::SetParam(BSTR bstrName, VARIANT vValue)
{
	VERIFY_INITIALIZATION

	HRESULT retVal = S_OK;
	try {
		m_pElementDecorator->SetParam(bstrName, vValue);
	}
	catch(hresult_exception& e) {
		retVal = e.hr;
	}
	catch(DecoratorException& e) {
		retVal = e.GetHResult();
	}
	return retVal;
}

STDMETHODIMP CMetaDecoratorImpl::GetParam(BSTR bstrName, VARIANT* pvValue)
{
	VERIFY_INITIALIZATION

	HRESULT retVal = S_OK;
	try {
		m_pElementDecorator->GetParam(bstrName, pvValue);
	}
	catch(hresult_exception& e) {
		retVal = e.hr;
	}
	catch(DecoratorException& e) {
		retVal = e.GetHResult();
	}
	return retVal;
}

STDMETHODIMP CMetaDecoratorImpl::SetActive(VARIANT_BOOL vbIsActive)
{
	VERIFY_INITIALIZATION

	HRESULT retVal = S_OK;
	try {
		m_pElementDecorator->SetActive(vbIsActive == VARIANT_TRUE);
	}
	catch(hresult_exception& e) {
		retVal = e.hr;
	}
	catch(DecoratorException& e) {
		retVal = e.GetHResult();
	}
	return retVal;
}

STDMETHODIMP CMetaDecoratorImpl::GetPreferredSize(LONG* plWidth, LONG* plHeight)
{
	VERIFY_INITIALIZATION

	HRESULT retVal = S_OK;
	try {
		CSize cSize = m_pElementDecorator->GetPreferredSize();
		*plWidth = cSize.cx;
		*plHeight = cSize.cy;
	}
	catch(hresult_exception& e) {
		retVal = e.hr;
	}
	catch(DecoratorException& e) {
		retVal = e.GetHResult();
	}
	return retVal;
}


STDMETHODIMP CMetaDecoratorImpl::SetLocation(LONG sx, LONG sy, LONG ex, LONG ey)
{
	VERIFY_INITIALIZATION

	HRESULT retVal = S_OK;
	try {
		m_pElementDecorator->SetLocation(CRect(sx, sy, ex, ey));
		m_bLocationSet = true;
	}
	catch(hresult_exception& e) {
		retVal = e.hr;
	}
	catch(DecoratorException& e) {
		retVal = e.GetHResult();
	}
	return retVal;
}

STDMETHODIMP CMetaDecoratorImpl::GetLocation(LONG* sx, LONG* sy, LONG* ex, LONG* ey)
{
	VERIFY_INITIALIZATION
	VERIFY_LOCATION

	HRESULT retVal = S_OK;
	try {
		CRect cRect = m_pElementDecorator->GetLocation();
		*sx = cRect.left;
		*sy = cRect.top;
		*ex = cRect.right;
		*ey = cRect.bottom;
	}
	catch(hresult_exception& e) {
		retVal = e.hr;
	}
	catch(DecoratorException& e) {
		retVal = e.GetHResult();
	}
	return retVal;
}

STDMETHODIMP CMetaDecoratorImpl::GetLabelLocation(LONG* sx, LONG* sy, LONG* ex, LONG* ey)
{
	VERIFY_INITIALIZATION
	VERIFY_LOCATION

	HRESULT retVal = S_OK;
	try {
		CRect labelRect = m_pElementDecorator->GetLabelLocation();
		*sx = labelRect.left;
		*sy = labelRect.top;
		*ex = labelRect.right;
		*ey = labelRect.bottom;
	}
	catch(hresult_exception& e) {
		retVal = e.hr;
	}
	catch(DecoratorException& e) {
		retVal = e.GetHResult();
	}
	return retVal;
}

STDMETHODIMP CMetaDecoratorImpl::GetPortLocation(IMgaFCO* pFCO, LONG* sx, LONG* sy, LONG* ex, LONG* ey)
{
	VERIFY_INITIALIZATION
	VERIFY_LOCATION

	HRESULT retVal = S_OK;
	try {
		CRect portLocation = m_pElementDecorator->GetPortLocation(CComPtr<IMgaFCO>(pFCO));
		*sx = portLocation.left;
		*sy = portLocation.top;
		*ex = portLocation.right;
		*ey = portLocation.bottom;
	}
	catch(hresult_exception& e) {
		retVal = e.hr;
	}
	catch(DecoratorException& e) {
		retVal = e.GetHResult();
	}
	return retVal;
}

STDMETHODIMP CMetaDecoratorImpl::GetPorts(IMgaFCOs** portFCOs)
{
	VERIFY_INITIALIZATION

	HRESULT retVal = S_OK;
	try {
		CComPtr<IMgaFCOs> portMgaFCOs;
		m_pElementDecorator->GetPorts(portMgaFCOs);
		*portFCOs = portMgaFCOs;
	}
	catch(hresult_exception& e) {
		retVal = e.hr;
	}
	catch(DecoratorException& e) {
		retVal = e.GetHResult();
	}
	return retVal;
}

STDMETHODIMP CMetaDecoratorImpl::Draw(ULONG hdc)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	VERIFY_INITIALIZATION
	VERIFY_LOCATION

	return E_DECORATOR_USING_DEPRECATED_FUNCTION;
}

STDMETHODIMP CMetaDecoratorImpl::SaveState()
{
	VERIFY_INITIALIZATION

	return S_OK;
}

// New functions
STDMETHODIMP CMetaDecoratorImpl::InitializeEx(IMgaProject* pProject, IMgaMetaPart* pPart, IMgaFCO* pFCO,
											  IMgaCommonDecoratorEvents* eventSink, ULONGLONG parentWnd)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_bInitCallFromEx = true;

	HRESULT retVal = S_OK;
	try {
		MetaDecor::MetaDecorator* metaDecorator = new MetaDecor::MetaDecorator(CComPtr<IMgaCommonDecoratorEvents>(eventSink));
		m_pElementDecorator = metaDecorator;

		metaDecorator->InitializeEx(CComPtr<IMgaProject>(pProject), CComPtr<IMgaMetaPart>(pPart),
									CComPtr<IMgaFCO>(pFCO), (HWND)parentWnd);
	}
	catch(hresult_exception& e) {
		retVal = e.hr;
	}
	catch(DecoratorException& e) {
		retVal = e.GetHResult();
	}

	return retVal;
}

STDMETHODIMP CMetaDecoratorImpl::DrawEx(ULONG hdc, ULONGLONG gdipGraphics)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	VERIFY_INITIALIZATION
	VERIFY_LOCATION

	HRESULT retVal = S_OK;

	CDC dc;
	dc.Attach((HDC)hdc);
	{
		try {
			m_pElementDecorator->Draw(&dc, (Gdiplus::Graphics*)gdipGraphics);
		}
		catch(hresult_exception& e) {
			retVal = e.hr;
		}
		catch(DecoratorException& e) {
			retVal = e.GetHResult();
		}
	}
	dc.Detach();

	return retVal;
}

STDMETHODIMP CMetaDecoratorImpl::SetSelected(VARIANT_BOOL vbIsSelected)
{
	VERIFY_INITIALIZATION

	HRESULT retVal = S_OK;
	try {
		m_pElementDecorator->SetSelected(vbIsSelected == VARIANT_TRUE);
	}
	catch(hresult_exception& e) {
		retVal = e.hr;
	}
	catch(DecoratorException& e) {
		retVal = e.GetHResult();
	}
	return retVal;
}

STDMETHODIMP CMetaDecoratorImpl::MouseMoved(ULONG nFlags, LONG pointx, LONG pointy, ULONGLONG transformHDC)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	HRESULT retVal = S_OK;
	try {
		if (m_pElementDecorator->MouseMoved(nFlags, CPoint(pointx, pointy), (HDC)transformHDC))
			retVal = S_DECORATOR_EVENT_HANDLED;
		else
			retVal = S_DECORATOR_EVENT_NOT_HANDLED;
	}
	catch(hresult_exception& e) {
		retVal = e.hr;
	}
	catch(DecoratorException& e) {
		retVal = e.GetHResult();
	}
	return retVal;
}

STDMETHODIMP CMetaDecoratorImpl::MouseLeftButtonDown(ULONG nFlags, LONG pointx, LONG pointy, ULONGLONG transformHDC)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	HRESULT retVal = S_OK;
	try {
		if (m_pElementDecorator->MouseLeftButtonDown(nFlags, CPoint(pointx, pointy), (HDC)transformHDC))
			retVal = S_DECORATOR_EVENT_HANDLED;
		else
			retVal = S_DECORATOR_EVENT_NOT_HANDLED;
	}
	catch(hresult_exception& e) {
		retVal = e.hr;
	}
	catch(DecoratorException& e) {
		retVal = e.GetHResult();
	}
	return retVal;
}

STDMETHODIMP CMetaDecoratorImpl::MouseLeftButtonUp(ULONG nFlags, LONG pointx, LONG pointy, ULONGLONG transformHDC)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	HRESULT retVal = S_OK;
	try {
		if (m_pElementDecorator->MouseLeftButtonUp(nFlags, CPoint(pointx, pointy), (HDC)transformHDC))
			retVal = S_DECORATOR_EVENT_HANDLED;
		else
			retVal = S_DECORATOR_EVENT_NOT_HANDLED;
	}
	catch(hresult_exception& e) {
		retVal = e.hr;
	}
	catch(DecoratorException& e) {
		retVal = e.GetHResult();
	}
	return retVal;
}

STDMETHODIMP CMetaDecoratorImpl::MouseLeftButtonDoubleClick(ULONG nFlags, LONG pointx, LONG pointy, ULONGLONG transformHDC)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	HRESULT retVal = S_OK;
	try {
		if (m_pElementDecorator->MouseLeftButtonDoubleClick(nFlags, CPoint(pointx, pointy), (HDC)transformHDC))
			retVal = S_DECORATOR_EVENT_HANDLED;
		else
			retVal = S_DECORATOR_EVENT_NOT_HANDLED;
	}
	catch(hresult_exception& e) {
		retVal = e.hr;
	}
	catch(DecoratorException& e) {
		retVal = e.GetHResult();
	}
	return retVal;
}

STDMETHODIMP CMetaDecoratorImpl::MouseRightButtonDown(ULONGLONG hCtxMenu, ULONG nFlags, LONG pointx, LONG pointy, ULONGLONG transformHDC)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	HRESULT retVal = S_OK;
	try {
		if (m_pElementDecorator->MouseRightButtonDown((HMENU) hCtxMenu, nFlags, CPoint(pointx, pointy), (HDC)transformHDC))
			retVal = S_DECORATOR_EVENT_HANDLED;
		else
			retVal = S_DECORATOR_EVENT_NOT_HANDLED;
	}
	catch(hresult_exception& e) {
		retVal = e.hr;
	}
	catch(DecoratorException& e) {
		retVal = e.GetHResult();
	}
	return retVal;
}

STDMETHODIMP CMetaDecoratorImpl::MouseRightButtonUp(ULONG nFlags, LONG pointx, LONG pointy, ULONGLONG transformHDC)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	HRESULT retVal = S_OK;
	try {
		if (m_pElementDecorator->MouseRightButtonUp(nFlags, CPoint(pointx, pointy), (HDC)transformHDC))
			retVal = S_DECORATOR_EVENT_HANDLED;
		else
			retVal = S_DECORATOR_EVENT_NOT_HANDLED;
	}
	catch(hresult_exception& e) {
		retVal = e.hr;
	}
	catch(DecoratorException& e) {
		retVal = e.GetHResult();
	}
	return retVal;
}

STDMETHODIMP CMetaDecoratorImpl::MouseRightButtonDoubleClick(ULONG nFlags, LONG pointx, LONG pointy, ULONGLONG transformHDC)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	HRESULT retVal = S_OK;
	try {
		if (m_pElementDecorator->MouseRightButtonDoubleClick(nFlags, CPoint(pointx, pointy), (HDC)transformHDC))
			retVal = S_DECORATOR_EVENT_HANDLED;
		else
			retVal = S_DECORATOR_EVENT_NOT_HANDLED;
	}
	catch(hresult_exception& e) {
		retVal = e.hr;
	}
	catch(DecoratorException& e) {
		retVal = e.GetHResult();
	}
	return retVal;
}

STDMETHODIMP CMetaDecoratorImpl::MouseMiddleButtonDown(ULONG nFlags, LONG pointx, LONG pointy, ULONGLONG transformHDC)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	HRESULT retVal = S_OK;
	try {
		if (m_pElementDecorator->MouseMiddleButtonDown(nFlags, CPoint(pointx, pointy), (HDC)transformHDC))
			retVal = S_DECORATOR_EVENT_HANDLED;
		else
			retVal = S_DECORATOR_EVENT_NOT_HANDLED;
	}
	catch(hresult_exception& e) {
		retVal = e.hr;
	}
	catch(DecoratorException& e) {
		retVal = e.GetHResult();
	}
	return retVal;
}

STDMETHODIMP CMetaDecoratorImpl::MouseMiddleButtonUp(ULONG nFlags, LONG pointx, LONG pointy, ULONGLONG transformHDC)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	HRESULT retVal = S_OK;
	try {
		if (m_pElementDecorator->MouseMiddleButtonUp(nFlags, CPoint(pointx, pointy), (HDC)transformHDC))
			retVal = S_DECORATOR_EVENT_HANDLED;
		else
			retVal = S_DECORATOR_EVENT_NOT_HANDLED;
	}
	catch(hresult_exception& e) {
		retVal = e.hr;
	}
	catch(DecoratorException& e) {
		retVal = e.GetHResult();
	}
	return retVal;
}

STDMETHODIMP CMetaDecoratorImpl::MouseMiddleButtonDoubleClick(ULONG nFlags, LONG pointx, LONG pointy, ULONGLONG transformHDC)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	HRESULT retVal = S_OK;
	try {
		if (m_pElementDecorator->MouseMiddleButtonDoubleClick(nFlags, CPoint(pointx, pointy), (HDC)transformHDC))
			retVal = S_DECORATOR_EVENT_HANDLED;
		else
			retVal = S_DECORATOR_EVENT_NOT_HANDLED;
	}
	catch(hresult_exception& e) {
		retVal = e.hr;
	}
	catch(DecoratorException& e) {
		retVal = e.GetHResult();
	}
	return retVal;
}

STDMETHODIMP CMetaDecoratorImpl::MouseWheelTurned(ULONG nFlags, LONG distance, LONG pointx, LONG pointy, ULONGLONG transformHDC)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	HRESULT retVal = S_OK;
	try {
		if (m_pElementDecorator->MouseWheelTurned(nFlags, (short)distance, CPoint(pointx, pointy), (HDC)transformHDC))
			retVal = S_DECORATOR_EVENT_HANDLED;
		else
			retVal = S_DECORATOR_EVENT_NOT_HANDLED;
	}
	catch(hresult_exception& e) {
		retVal = e.hr;
	}
	catch(DecoratorException& e) {
		retVal = e.GetHResult();
	}
	return retVal;
}

STDMETHODIMP CMetaDecoratorImpl::DragEnter(ULONG* dropEffect, ULONGLONG pCOleDataObject, ULONG keyState, LONG pointx, LONG pointy, ULONGLONG transformHDC)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	HRESULT retVal = S_OK;
	try {
		if (m_pElementDecorator->DragEnter((DROPEFFECT*)dropEffect, (COleDataObject*)pCOleDataObject, (DWORD)keyState, CPoint(pointx, pointy), (HDC)transformHDC)) {
			retVal = S_DECORATOR_EVENT_HANDLED;
		} else {
			retVal = S_DECORATOR_EVENT_NOT_HANDLED;
			*dropEffect = DROPEFFECT_NONE;
		}
	}
	catch(hresult_exception& e) {
		retVal = e.hr;
	}
	catch(DecoratorException& e) {
		retVal = e.GetHResult();
	}
	return retVal;
}

STDMETHODIMP CMetaDecoratorImpl::DragOver(ULONG* dropEffect, ULONGLONG pCOleDataObject, ULONG keyState, LONG pointx, LONG pointy, ULONGLONG transformHDC)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	HRESULT retVal = S_OK;
	try {
		if (m_pElementDecorator->DragOver((DROPEFFECT*)dropEffect, (COleDataObject*)pCOleDataObject, (DWORD)keyState, CPoint(pointx, pointy), (HDC)transformHDC)) {
			retVal = S_DECORATOR_EVENT_HANDLED;
		} else {
			retVal = S_DECORATOR_EVENT_NOT_HANDLED;
			*dropEffect = DROPEFFECT_NONE;
		}
	}
	catch(hresult_exception& e) {
		retVal = e.hr;
	}
	catch(DecoratorException& e) {
		retVal = e.GetHResult();
	}
	return retVal;
}

STDMETHODIMP CMetaDecoratorImpl::Drop(ULONGLONG pCOleDataObject, ULONG dropEffect, LONG pointx, LONG pointy, ULONGLONG transformHDC)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	HRESULT retVal = S_OK;
	try {
		if (m_pElementDecorator->Drop((COleDataObject*)pCOleDataObject, (DROPEFFECT)dropEffect, CPoint(pointx, pointy), (HDC)transformHDC))
			retVal = S_DECORATOR_EVENT_HANDLED;
		else
			retVal = S_DECORATOR_EVENT_NOT_HANDLED;
	}
	catch(hresult_exception& e) {
		retVal = e.hr;
	}
	catch(DecoratorException& e) {
		retVal = e.GetHResult();
	}
	return retVal;
}

STDMETHODIMP CMetaDecoratorImpl::DropFile(ULONGLONG hDropInfo, LONG pointx, LONG pointy, ULONGLONG transformHDC)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	HRESULT retVal = S_OK;
	try {
		if (m_pElementDecorator->DropFile((HDROP)hDropInfo, CPoint(pointx, pointy), (HDC)transformHDC))
			retVal = S_DECORATOR_EVENT_HANDLED;
		else
			retVal = S_DECORATOR_EVENT_NOT_HANDLED;
	}
	catch(hresult_exception& e) {
		retVal = e.hr;
	}
	catch(DecoratorException& e) {
		retVal = e.GetHResult();
	}
	return retVal;
}

STDMETHODIMP CMetaDecoratorImpl::MenuItemSelected(ULONG menuItemId, ULONG nFlags, LONG pointx, LONG pointy, ULONGLONG transformHDC)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	HRESULT retVal = S_OK;
	try {
		if (m_pElementDecorator->MenuItemSelected(menuItemId, nFlags, CPoint(pointx, pointy), (HDC)transformHDC))
			retVal = S_DECORATOR_EVENT_HANDLED;
		else
			retVal = S_DECORATOR_EVENT_NOT_HANDLED;
	}
	catch(hresult_exception& e) {
		retVal = e.hr;
	}
	catch(DecoratorException& e) {
		retVal = e.GetHResult();
	}
	return retVal;
}

STDMETHODIMP CMetaDecoratorImpl::OperationCanceled()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	HRESULT retVal = S_OK;
	try {
		if (m_pElementDecorator->OperationCanceledByGME())
			retVal = S_DECORATOR_EVENT_HANDLED;
		else
			retVal = S_DECORATOR_EVENT_NOT_HANDLED;
	}
	catch(hresult_exception& e) {
		retVal = e.hr;
	}
	catch(DecoratorException& e) {
		retVal = e.GetHResult();
	}
	return retVal;
}
