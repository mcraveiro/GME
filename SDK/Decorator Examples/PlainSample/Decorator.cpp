// Decorator.cpp : Implementation of CDecorator
#include "stdafx.h"
#include "Decorator.h"
#include "DecoratorDefs.h"
#include <sys/types.h>
#include <sys/stat.h>


#define VERIFY_INITIALIZATION	{ if (!m_isInitialized) return E_DECORATOR_UNINITIALIZED; }
#define VERIFY_LOCATION			{ if (!m_isLocSet) return E_DECORATOR_LOCISNOTSET; }

#define SAMPLE_SIZEX	40
#define SAMPLE_SIZEY	20

#define	CTX_MENU_ID_DECORATOR		DECORATOR_CTX_MENU_MINID + 101	// Should be unique
#define	CTX_MENU_STR_DECORATOR		"Decorator Ctx Menu Item"


/////////////////////////////////////////////////////////////////////////////
// CDecorator
STDMETHODIMP CDecorator::Initialize(IMgaProject* pProject, IMgaMetaPart* metaPart, IMgaFCO* pFCO)
{
	//
	// TODO: read all important data from MGA and cache them for later use
	//
	if (pFCO && !m_bInitCallFromEx)
		return E_DECORATOR_USING_DEPRECATED_FUNCTION;

	m_mgaFco = pFCO;		// pFCO == NULL, if we are in the PartBrowser
	
	if (!GetMetaFCO(metaPart, m_metaFco)) {
		return E_DECORATOR_INIT_WITH_NULL;
	} else {
		m_isInitialized = true;
	}	
	
	if (m_mgaFco) {
		CComBSTR bstr;
		COMTHROW(m_mgaFco->get_Name(&bstr));
		m_name = bstr;
	} else {
		CComBSTR bstr;
		COMTHROW(m_metaFco->get_DisplayedName(&bstr));
		if (bstr.Length() == 0 ) {
			bstr.Empty();
			COMTHROW(m_metaFco->get_Name(&bstr));
		}
		m_name = bstr;
	}

	if (!GetColorPreference(m_color, DecoratorSDK::PREF_COLOR)) {
		m_color = DecoratorSDK::COLOR_GRAY;
	}
	if (!GetColorPreference(m_nameColor, DecoratorSDK::PREF_LABELCOLOR)) {
		m_nameColor = DecoratorSDK::COLOR_BLACK;
	}
	return S_OK;
}

STDMETHODIMP CDecorator::Destroy()
{
	//
	// TODO: At least free all references to MGA objects
	//
	VERIFY_INITIALIZATION;
	m_isInitialized = false;
	m_isLocSet = false;
	m_metaFco = NULL;
	m_mgaFco = NULL;
	m_eventSink = NULL;
	return S_OK;
}

STDMETHODIMP CDecorator::GetMnemonic(BSTR* mnemonic)
{
	//
	// TODO: Return the logical name of the decorator (currently not used by GME)
	//
	*mnemonic = CComBSTR(DECORATOR_NAME).Detach();
	return S_OK;
}

STDMETHODIMP CDecorator::GetFeatures(feature_code *features)
{
	//
	// TODO: Return supported features (combine multiple features with bitwise-OR)
	// Available feature codes are found in MgaDecorator.idl
	// (curently not used by GME)
	*features = 0;
	return S_OK;
}

STDMETHODIMP CDecorator::SetParam(BSTR name, VARIANT value)
{
	//
	// TODO:  Parse and set all supported parameters, otherwise return error
	// (currently all values are BSTR type)
	//
	VERIFY_INITIALIZATION;
	return E_DECORATOR_UNKNOWN_PARAMETER;
}

STDMETHODIMP CDecorator::GetParam(BSTR name, VARIANT* value)
{
	//
	// TODO: Return values of supported and previously set parameters, otherwise return error
	// (currently GME does not use this method)
	//
	VERIFY_INITIALIZATION;
	return E_DECORATOR_UNKNOWN_PARAMETER;
}

STDMETHODIMP CDecorator::SetActive(VARIANT_BOOL isActive)
{
	//
	// TODO: If isActive == VARIANT_FALSE, draw your object in GME_GREYED_OUT, otherwise use the color of the object
	//
	VERIFY_INITIALIZATION;
	m_isActive = (isActive != VARIANT_FALSE);
	return S_OK;
}

STDMETHODIMP CDecorator::GetPreferredSize(long* sizex, long* sizey) 
{
	//
	// TODO: Give GME a hint about the object size. Do not expect GME to take it into account
	//
	VERIFY_INITIALIZATION;
	*sizex = SAMPLE_SIZEX;
	*sizey = SAMPLE_SIZEY;
	return S_OK;
}


STDMETHODIMP CDecorator::SetLocation(long sx, long sy, long ex, long ey)
{
	//
	// TODO: Draw the object exactly to the this location later
	//
	VERIFY_INITIALIZATION;
	m_sx = sx;
	m_sy = sy;
	m_ex = ex;
	m_ey = ey;
	m_isLocSet = true;
	return S_OK;
}

STDMETHODIMP CDecorator::GetLocation(long* sx, long* sy, long* ex, long* ey)
{
	//
	// TODO: Return previously set location parameters
	// (currently GME does not call this)
	//
	VERIFY_INITIALIZATION;
	VERIFY_LOCATION;
	*sx = m_sx;
	*sy = m_sy;
	*ex = m_ex;
	*ey = m_ey;
	return S_OK;
}

STDMETHODIMP CDecorator::GetLabelLocation(long* sx, long* sy, long* ex, long* ey)
{
	//
	// TODO: Return the location of the text box of your label if you support labels.
	// (currently GME does not call this)
	//
	VERIFY_INITIALIZATION;
	VERIFY_LOCATION;
	return S_OK;
}

STDMETHODIMP CDecorator::GetPortLocation(IMgaFCO* fco, long* sx, long* sy, long* ex, long* ey)
{
	//
	// TODO: Return the location of the specified port if ports are supported in the decorator
	//
	VERIFY_INITIALIZATION;
	VERIFY_LOCATION;

	return E_DECORATOR_PORTNOTFOUND;
}

STDMETHODIMP CDecorator::GetPorts(IMgaFCOs** portFCOs)
{
	//
	// TODO: Return a collection of mga objects represented as ports.
	//
	VERIFY_INITIALIZATION;
	return S_OK;
}


STDMETHODIMP CDecorator::Draw(HDC hdc)
{
	//
	// TODO: (In case of old decorator) Draw your object.
	// Saving & restoring the HDC is a good practice, however it is not a strict requirement
	// Use DecoratorSDK::getFacilities() for brushes/pens/fonts for more complex decorators.
	//
	VERIFY_INITIALIZATION;
	VERIFY_LOCATION;

	//CDC dc;
	//dc.Attach(hdc);
	//CRect rect(m_sx, m_sy, m_ex, m_ey);
	//DecoratorSDK::getFacilities().drawBox(&dc, rect, m_color, 7);
	//DecoratorSDK::getFacilities().drawText(&dc, m_name, CPoint(rect.CenterPoint().x, rect.bottom),
	//									   DecoratorSDK::getFacilities().getFont(DecoratorSDK::FONT_LABEL)->pFont,
	//									   m_nameColor, TA_TOP | TA_CENTER);
	//dc.Detach();
	return S_OK;
}

STDMETHODIMP CDecorator::SaveState()
{
	//
	// TODO: The only method where we are in read-write transaction. Store all permanent information
	// (currently GME does not support this) 
	//
	VERIFY_INITIALIZATION;
	return S_OK;
}

// New functions
STDMETHODIMP CDecorator::InitializeEx(IMgaProject* pProject, IMgaMetaPart* pPart, IMgaFCO* pFCO,
									  IMgaCommonDecoratorEvents* eventSink, ULONGLONG parentWnd)
{
	//
	// TODO: handle extra parameters, call Initialize with the rest
	//
	m_bInitCallFromEx = true;

	// If this wouldn't be a simple decorator, we would initialize the Decorator utilities facility framework
	//DecoratorSDK::getFacilities().loadPathes(pProject, true);

	Initialize(pProject, pPart, pFCO);
	m_eventSink = eventSink;
	m_parentWnd = (HWND)parentWnd;

	return S_OK;
}

STDMETHODIMP CDecorator::DrawEx(HDC hdc, ULONGLONG gdipGraphics)
{
	//
	// TODO: gdipGraphics is a Gdiplus::Graphics* variable, it is advisable to use this for drawing and don't use the HDC
	//
	VERIFY_INITIALIZATION
	VERIFY_LOCATION

	Gdiplus::Graphics* gdip = (Gdiplus::Graphics*)gdipGraphics;
	Gdiplus::Color boxColor(GetRValue(m_color), GetGValue(m_color), GetBValue(m_color));
	Gdiplus::Pen boxPen(boxColor);
	gdip->DrawRectangle(&boxPen, m_sx, m_sy, m_ex - m_sx, m_ey - m_sy);
	Gdiplus::Font textFont(L"Arial", 16);
	Gdiplus::PointF origin(m_sx, m_ey);
	Gdiplus::SolidBrush textBrush(Gdiplus::Color(GetRValue(m_nameColor),
												 GetGValue(m_nameColor),
												 GetBValue(m_nameColor)));
	Gdiplus::StringFormat format;
	format.SetAlignment(Gdiplus::StringAlignmentNear);
	format.SetLineAlignment(Gdiplus::StringAlignmentNear);
	CA2W wcText(m_name);
	gdip->DrawString(wcText, m_name.GetLength(), &textFont, origin, &format, &textBrush);

	//DecoratorSDK::getFacilities().drawBox(&dc, rect, m_color, 7);
	//DecoratorSDK::getFacilities().drawText(&dc, m_name, CPoint(rect.CenterPoint().x, rect.bottom),
	//									   DecoratorSDK::getFacilities().getFont(DecoratorSDK::FONT_LABEL)->pFont,
	//									   m_nameColor, TA_TOP | TA_CENTER);

	return Draw(hdc);
}

STDMETHODIMP CDecorator::SetSelected(VARIANT_BOOL vbIsSelected)
{
	//
	// TODO: memorize selected state, it might be needed for some operations later
	//
	VERIFY_INITIALIZATION

	m_bSelected = (vbIsSelected == VARIANT_TRUE);

	return S_OK;
}

STDMETHODIMP CDecorator::MouseMoved(ULONG nFlags, LONG pointx, LONG pointy, ULONGLONG transformHDC)
{
	//
	// TODO: if you respond to the message, for example change the mouse cursor, you should return
	// S_DECORATOR_EVENT_HANDLED, else you should return S_DECORATOR_EVENT_NOT_HANDLED
	// nFlags shows additional button states (see WM_MOUSEMOVE MSDN page for example, see MK_CONTROL, MK_SHIFT, etc constants)
	// you can use transformHDC HDC to transform point coordinates to screen coordinates (GMEView can be zoomed and scrolled)
	//
	return S_DECORATOR_EVENT_NOT_HANDLED;
}

STDMETHODIMP CDecorator::MouseLeftButtonDown(ULONG nFlags, LONG pointx, LONG pointy, ULONGLONG transformHDC)
{
	//
	// TODO: if you respond to the message, you should return
	// S_DECORATOR_EVENT_HANDLED, else you should return S_DECORATOR_EVENT_NOT_HANDLED
	// nFlags shows additional button states (see WM_MOUSEMOVE MSDN page for example, see MK_CONTROL, MK_SHIFT, etc constants)
	// you can use transformHDC HDC to transform point coordinates to screen coordinates (GMEView can be zoomed and scrolled)
	//
	return S_DECORATOR_EVENT_NOT_HANDLED;
}

STDMETHODIMP CDecorator::MouseLeftButtonUp(ULONG nFlags, LONG pointx, LONG pointy, ULONGLONG transformHDC)
{
	//
	// TODO: if you respond to the message, you should return
	// S_DECORATOR_EVENT_HANDLED, else you should return S_DECORATOR_EVENT_NOT_HANDLED
	// nFlags shows additional button states (see WM_MOUSEMOVE MSDN page for example, see MK_CONTROL, MK_SHIFT, etc constants)
	// you can use transformHDC HDC to transform point coordinates to screen coordinates (GMEView can be zoomed and scrolled)
	//
	return S_DECORATOR_EVENT_NOT_HANDLED;
}

STDMETHODIMP CDecorator::MouseLeftButtonDoubleClick(ULONG nFlags, LONG pointx, LONG pointy, ULONGLONG transformHDC)
{
	//
	// TODO: if you respond to the message, you should return
	// S_DECORATOR_EVENT_HANDLED, else you should return S_DECORATOR_EVENT_NOT_HANDLED
	// nFlags shows additional button states (see WM_MOUSEMOVE MSDN page for example, see MK_CONTROL, MK_SHIFT, etc constants)
	// you can use transformHDC HDC to transform point coordinates to screen coordinates (GMEView can be zoomed and scrolled)
	//
	CRect boxRect(m_sx, m_sy, m_ex, m_ey);
	if (boxRect.PtInRect(CPoint(pointx, pointy))) {
		AfxMessageBox("SampleDecorator double clicked!");
		m_eventSink->GeneralOperationStarted(NULL);
		// TODO: do something
		m_eventSink->GeneralOperationFinished(NULL);
		return S_DECORATOR_EVENT_HANDLED;
	}

	return S_DECORATOR_EVENT_NOT_HANDLED;
}

STDMETHODIMP CDecorator::MouseRightButtonDown(ULONGLONG hCtxMenu, ULONG nFlags, LONG pointx, LONG pointy, ULONGLONG transformHDC)
{
	//
	// TODO: if you respond to the message, you should return
	// S_DECORATOR_EVENT_HANDLED, else you should return S_DECORATOR_EVENT_NOT_HANDLED
	// hCtxMenu is a HMENU type variable, you can add menu items to it which will be displayed in the context menu
	// nFlags shows additional button states (see WM_MOUSEMOVE MSDN page for example, see MK_CONTROL, MK_SHIFT, etc constants)
	// you can use transformHDC HDC to transform point coordinates to screen coordinates (GMEView can be zoomed and scrolled)
	//
	CRect boxRect(m_sx, m_sy, m_ex, m_ey);
	if (boxRect.PtInRect(CPoint(pointx, pointy))) {
		::AppendMenu((HMENU)hCtxMenu, MF_STRING | MF_ENABLED, CTX_MENU_ID_DECORATOR, CTX_MENU_STR_DECORATOR);
		return S_DECORATOR_EVENT_HANDLED;
	}

	return S_DECORATOR_EVENT_NOT_HANDLED;
}

STDMETHODIMP CDecorator::MouseRightButtonUp(ULONG nFlags, LONG pointx, LONG pointy, ULONGLONG transformHDC)
{
	//
	// TODO: if you respond to the message, you should return
	// S_DECORATOR_EVENT_HANDLED, else you should return S_DECORATOR_EVENT_NOT_HANDLED
	// nFlags shows additional button states (see WM_MOUSEMOVE MSDN page for example, see MK_CONTROL, MK_SHIFT, etc constants)
	// you can use transformHDC HDC to transform point coordinates to screen coordinates (GMEView can be zoomed and scrolled)
	//
	return S_DECORATOR_EVENT_NOT_HANDLED;
}

STDMETHODIMP CDecorator::MouseRightButtonDoubleClick(ULONG nFlags, LONG pointx, LONG pointy, ULONGLONG transformHDC)
{
	//
	// TODO: if you respond to the message, you should return
	// S_DECORATOR_EVENT_HANDLED, else you should return S_DECORATOR_EVENT_NOT_HANDLED
	// nFlags shows additional button states (see WM_MOUSEMOVE MSDN page for example, see MK_CONTROL, MK_SHIFT, etc constants)
	// you can use transformHDC HDC to transform point coordinates to screen coordinates (GMEView can be zoomed and scrolled)
	//
	return S_DECORATOR_EVENT_NOT_HANDLED;
}

STDMETHODIMP CDecorator::MouseMiddleButtonDown(ULONG nFlags, LONG pointx, LONG pointy, ULONGLONG transformHDC)
{
	//
	// TODO: if you respond to the message, you should return
	// S_DECORATOR_EVENT_HANDLED, else you should return S_DECORATOR_EVENT_NOT_HANDLED
	// nFlags shows additional button states (see WM_MOUSEMOVE MSDN page for example, see MK_CONTROL, MK_SHIFT, etc constants)
	// you can use transformHDC HDC to transform point coordinates to screen coordinates (GMEView can be zoomed and scrolled)
	//
	return S_DECORATOR_EVENT_NOT_HANDLED;
}

STDMETHODIMP CDecorator::MouseMiddleButtonUp(ULONG nFlags, LONG pointx, LONG pointy, ULONGLONG transformHDC)
{
	//
	// TODO: if you respond to the message, you should return
	// S_DECORATOR_EVENT_HANDLED, else you should return S_DECORATOR_EVENT_NOT_HANDLED
	// nFlags shows additional button states (see WM_MOUSEMOVE MSDN page for example, see MK_CONTROL, MK_SHIFT, etc constants)
	// you can use transformHDC HDC to transform point coordinates to screen coordinates (GMEView can be zoomed and scrolled)
	//
	return S_DECORATOR_EVENT_NOT_HANDLED;
}

STDMETHODIMP CDecorator::MouseMiddleButtonDoubleClick(ULONG nFlags, LONG pointx, LONG pointy, ULONGLONG transformHDC)
{
	//
	// TODO: if you respond to the message, you should return
	// S_DECORATOR_EVENT_HANDLED, else you should return S_DECORATOR_EVENT_NOT_HANDLED
	// nFlags shows additional button states (see WM_MOUSEMOVE MSDN page for example, see MK_CONTROL, MK_SHIFT, etc constants)
	// you can use transformHDC HDC to transform point coordinates to screen coordinates (GMEView can be zoomed and scrolled)
	//
	return S_DECORATOR_EVENT_NOT_HANDLED;
}

STDMETHODIMP CDecorator::MouseWheelTurned(ULONG nFlags, LONG distance, LONG pointx, LONG pointy, ULONGLONG transformHDC)
{
	//
	// TODO: if you respond to the message, you should return
	// S_DECORATOR_EVENT_HANDLED, else you should return S_DECORATOR_EVENT_NOT_HANDLED
	// nFlags shows additional button states (see WM_MOUSEMOVE MSDN page for example, see MK_CONTROL, MK_SHIFT, etc constants)
	// you can use transformHDC HDC to transform point coordinates to screen coordinates (GMEView can be zoomed and scrolled)
	//
	return S_DECORATOR_EVENT_NOT_HANDLED;
}

STDMETHODIMP CDecorator::DragEnter(ULONG* dropEffect, ULONGLONG pCOleDataObject, ULONG keyState, LONG pointx, LONG pointy, ULONGLONG transformHDC)
{
	//
	// TODO: if you respond to the message, you should return
	// S_DECORATOR_EVENT_HANDLED, else you should return S_DECORATOR_EVENT_NOT_HANDLED
	// If you are interested only file drag-drops you should only hook up to DropFile event and inhibit DragEnter, DragOver, Drop
	// If you want more complicated things though, you should handle the three mentioned event in Windows fashion.
	// See OnDragEnter, OnDragOver, OnDrop MFC notifications in MSDN or GME source
	// pCOleDataObject is a COleDataObject pointer
	//
	return S_DECORATOR_EVENT_NOT_HANDLED;
}

STDMETHODIMP CDecorator::DragOver(ULONG* dropEffect, ULONGLONG pCOleDataObject, ULONG keyState, LONG pointx, LONG pointy, ULONGLONG transformHDC)
{
	//
	// TODO: if you respond to the message, you should return
	// S_DECORATOR_EVENT_HANDLED, else you should return S_DECORATOR_EVENT_NOT_HANDLED
	// If you are interested only file drag-drops you should only hook up to DropFile event and inhibit DragEnter, DragOver, Drop
	// If you want more complicated things though, you should handle the three mentioned event in Windows fashion.
	// See OnDragEnter, OnDragOver, OnDrop MFC notifications in MSDN or GME source
	// pCOleDataObject is a COleDataObject pointer
	//
	return S_DECORATOR_EVENT_NOT_HANDLED;
}

STDMETHODIMP CDecorator::Drop(ULONGLONG pCOleDataObject, ULONG dropEffect, LONG pointx, LONG pointy, ULONGLONG transformHDC)
{
	//
	// TODO: if you respond to the message, you should return
	// S_DECORATOR_EVENT_HANDLED, else you should return S_DECORATOR_EVENT_NOT_HANDLED
	// If you are interested only file drag-drops you should only hook up to DropFile event and inhibit DragEnter, DragOver, Drop
	// If you want more complicated things though, you should handle the three mentioned event in Windows fashion.
	// See OnDragEnter, OnDragOver, OnDrop MFC notifications in MSDN or GME source
	// pCOleDataObject is a COleDataObject pointer
	//
	return S_DECORATOR_EVENT_NOT_HANDLED;
}

STDMETHODIMP CDecorator::DropFile(ULONGLONG hDropInfo, LONG pointx, LONG pointy, ULONGLONG transformHDC)
{
	//
	// TODO: if you respond to the message, you should return
	// S_DECORATOR_EVENT_HANDLED, else you should return S_DECORATOR_EVENT_NOT_HANDLED
	// hDropInfo is a HDROP type variable, you should use this to extract the needed data
	//
	UINT nFiles = DragQueryFile((HDROP)hDropInfo, 0xFFFFFFFF, NULL, 0);
	if (nFiles < 1)
		return false;

	for (UINT iF = 0; iF < nFiles; ++iF) {
		TCHAR szFileName[_MAX_PATH];
		UINT res = DragQueryFile((HDROP)hDropInfo, iF, szFileName, _MAX_PATH);
		if (res > 0) {
			bool is_dir = false; 
			struct _stat fstatus;
			if (0 == _tstat(szFileName, &fstatus))
				is_dir = (fstatus.st_mode & _S_IFDIR) == _S_IFDIR;

			CString conn(szFileName);
			if (!is_dir && conn.Right(4).CompareNoCase(".txt") == 0) {
				CFile txtFile(conn, CFile::modeRead);
				char pbufRead[100];
				UINT readLen = txtFile.Read(pbufRead, sizeof(pbufRead) - 1);
				pbufRead[readLen] = 0;
				AfxMessageBox("SampleDecorator drop: '" + conn + "' first 100 bytes: " + pbufRead + ".");
				m_eventSink->GeneralOperationStarted(NULL);
				// TODO: do something
				m_eventSink->GeneralOperationFinished(NULL);
				return S_DECORATOR_EVENT_HANDLED;
			} else {
				AfxMessageBox("SampleDecorator drop: '.txt' files may be dropped only. Can't open file: " + conn + "!");
			}
		} else {
			AfxMessageBox("SampleDecorator drop: Can't inquire file information!");
		}
	}

	return S_DECORATOR_EVENT_NOT_HANDLED;
}

STDMETHODIMP CDecorator::MenuItemSelected(ULONG menuItemId, ULONG nFlags, LONG pointx, LONG pointy, ULONGLONG transformHDC)
{
	//
	// TODO: if you respond to the message, you should return
	// S_DECORATOR_EVENT_HANDLED, else you should return S_DECORATOR_EVENT_NOT_HANDLED
	// In menuItemId you should get back one of the menu IDs you previously added to the context menu
	//
	if (menuItemId == CTX_MENU_ID_DECORATOR) {
		AfxMessageBox("Sample Decorator Ctx Menu Item clicked!");
		m_eventSink->GeneralOperationStarted(NULL);
		// TODO: do something
		m_eventSink->GeneralOperationFinished(NULL);
		return S_DECORATOR_EVENT_HANDLED;
	}

	return S_DECORATOR_EVENT_NOT_HANDLED;
}

STDMETHODIMP CDecorator::OperationCanceled()
{
	//
	// TODO: if you handle the message, you should return S_DECORATOR_EVENT_HANDLED,
	// else you should return S_DECORATOR_EVENT_NOT_HANDLED
	//
	return S_DECORATOR_EVENT_NOT_HANDLED;
}

//////////// Decorator private functions
CDecorator::CDecorator() :
	m_sx				(0),
	m_sy				(0),
	m_ex				(0),
	m_ey				(0),
	m_isActive			(true),
	m_mgaFco			(0),
	m_metaFco			(0),
	m_isInitialized		(false),
	m_isLocSet			(false),
	m_bInitCallFromEx	(false),
	m_bSelected			(false),
	m_parentWnd			(NULL),
	m_color				(DecoratorSDK::COLOR_GRAY),
	m_nameColor			(DecoratorSDK::COLOR_BLACK)
{
}

CDecorator::~CDecorator()
{
}

bool CDecorator::GetMetaFCO(const CComPtr<IMgaMetaPart>& metaPart, CComPtr<IMgaMetaFCO> &metaFco)
{
	if (!metaPart) {
		return false;
	}

	metaFco = NULL;
	CComPtr<IMgaMetaRole> metaRole;
	try {
		COMTHROW(metaPart->get_Role(&metaRole));
		COMTHROW(metaRole->get_Kind(&metaFco));
	}
	catch (hresult_exception &) {
		metaFco = NULL;
	}
	return (metaFco != NULL);
}

bool CDecorator::GetPreference(CString& val, const CString& path)
{
	CComBSTR pathBstr(path);
	CComBSTR bstrVal;
	if (m_mgaFco) {
		COMTHROW(m_mgaFco->get_RegistryValue(pathBstr, &bstrVal));
	}
	else {
		COMTHROW(m_metaFco->get_RegistryValue(pathBstr, &bstrVal));
	}
	val = bstrVal;
	return !val.IsEmpty();
}

bool CDecorator::GetPreference(int& val, const CString& path, bool hex)
{
	CString strVal;
	GetPreference(strVal, path);
	return (sscanf(strVal, hex ? "%x" : "%d", &val) == 1);
}

bool CDecorator::GetColorPreference(unsigned long& color, const CString& path)
{
	int i;
	if (GetPreference(i, path, true)) {
		unsigned int r = (i & 0xff0000) >> 16;
		unsigned int g = (i & 0xff00) >> 8;
		unsigned int b = i & 0xff;
		color = RGB(r, g, b);
		return true;
	}
	return false;
}