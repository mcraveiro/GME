//################################################################################################
//
// Model complex part class (decorator part)
//	ModelComplexPart.cpp
//
//################################################################################################

#include "StdAfx.h"
#include "PortLabelPart.h"
#include "ModelComplexPart.h"
#include "DecoratorExceptions.h"
#include <algorithm>

#define MAXIMUM_ICON_SIZE_NONOVERLAPPING_WITH_PORT_LABELS 250

namespace DecoratorSDK {

//################################################################################################
//
// CLASS : PortPartData
//
//################################################################################################
class PortPartData {
public:
	PortPartData(PortPart* pD, CComPtr<IMgaMetaPart>& pPart, CComPtr<IMgaFCO>& pFCO):
		portPart(pD), spPart(pPart), spFCO(pFCO) {};
	~PortPartData() { if(portPart) delete portPart; };

	void Destroy() { if(portPart) portPart->Destroy(); }

	PortPart*				portPart;
	CComPtr<IMgaMetaPart>	spPart;
	CComPtr<IMgaFCO>		spFCO;
};

//################################################################################################
//
// CLASS : ModelComplexPart
//
//################################################################################################

ModelComplexPart::ModelComplexPart(PartBase* pPart, CComPtr<IMgaCommonDecoratorEvents>& eventSink):
	TypeableBitmapPart			(pPart, eventSink),

	m_iMaxPortTextLength		(MAX_PORT_LENGTH),
	m_crPortText				(COLOR_BLACK),
	m_bPortLabelInside			(true),
	m_iLongestPortTextLength	(0)
{
	m_prominentAttrsCY = 0;
	m_prominentAttrsNamesCX = 0;
	m_prominentAttrsValuesCX = 0;
	m_LeftPortsMaxLabelLength = 0;
	m_RightPortsMaxLabelLength = 0;
}

ModelComplexPart::~ModelComplexPart()
{
	m_LeftPorts.clear();
	m_RightPorts.clear();
	for (std::vector<PortPartData*>::iterator ii = m_AllPorts.begin(); ii != m_AllPorts.end(); ++ii)
		delete (*ii);
	m_AllPorts.clear();
}

void ModelComplexPart::Initialize(CComPtr<IMgaProject>& pProject, CComPtr<IMgaMetaPart>& pPart, CComPtr<IMgaFCO>& pFCO)
{
	// Akos: when this gets called, the left and right ports list should not exist
	/*
	for (std::vector<PortPart*>::iterator ii = m_LeftPorts.begin(); ii != m_LeftPorts.end(); ++ii) {
		(*ii)->Initialize(pProject, pPart, pFCO);
	}
	for (std::vector<PortPart*>::iterator ii = m_RightPorts.begin(); ii != m_RightPorts.end(); ++ii) {
		(*ii)->Initialize(pProject, pPart, pFCO);
	}
	*/
	TypeableBitmapPart::Initialize(pProject, pPart, pFCO);
}

void ModelComplexPart::Destroy()
{
	for (std::vector<PortPartData*>::iterator ii = m_AllPorts.begin(); ii != m_AllPorts.end(); ++ii)
		(*ii)->Destroy();
	TypeableBitmapPart::Destroy();
}

CString ModelComplexPart::GetMnemonic(void) const
{
	return TypeableBitmapPart::GetMnemonic();
}

feature_code ModelComplexPart::GetFeatures(void) const
{
	feature_code featureCodes = 0;

	for (std::vector<PortPart*>::const_iterator ii = m_LeftPorts.begin(); ii != m_LeftPorts.end(); ++ii) {
		feature_code portFeatureCodes = (*ii)->GetFeatures();
		featureCodes |= portFeatureCodes;
	}
	for (std::vector<PortPart*>::const_iterator ii = m_RightPorts.begin(); ii != m_RightPorts.end(); ++ii) {
		feature_code portFeatureCodes = (*ii)->GetFeatures();
		featureCodes |= portFeatureCodes;
	}

	feature_code partFeatureCodes = TypeableBitmapPart::GetFeatures();
	featureCodes |= partFeatureCodes;

	return featureCodes;
}

void ModelComplexPart::SetParam(const CString& strName, VARIANT vValue)
{
	CString pName(DEC_CONNECTED_PORTS_ONLY_PARAM);
	if(pName == strName && vValue.boolVal == VARIANT_TRUE) 
		ReOrderConnectedOnlyPorts();
	else
	{
		for (std::vector<PortPart*>::iterator ii = m_LeftPorts.begin(); ii != m_LeftPorts.end(); ++ii) {
			(*ii)->SetParam(strName, vValue);
		}
		for (std::vector<PortPart*>::iterator ii = m_RightPorts.begin(); ii != m_RightPorts.end(); ++ii) {
			(*ii)->SetParam(strName, vValue);
		}
		TypeableBitmapPart::SetParam(strName, vValue);
	}
}

bool ModelComplexPart::GetParam(const CString& strName, VARIANT* pvValue)
{
	try {
		if (TypeableBitmapPart::GetParam(strName, pvValue))
			return true;
	}
	catch(UnknownParameterException&) {
	}
	return false;
}

void ModelComplexPart::SetActive(bool bIsActive)
{
	for (std::vector<PortPart*>::iterator ii = m_LeftPorts.begin(); ii != m_LeftPorts.end(); ++ii) {
		(*ii)->SetActive(bIsActive);
	}
	for (std::vector<PortPart*>::iterator ii = m_RightPorts.begin(); ii != m_RightPorts.end(); ++ii) {
		(*ii)->SetActive(bIsActive);
	}
	TypeableBitmapPart::SetActive(bIsActive);
}

// [Zsolt] Do not know where to put this utility function
template<typename IT, typename F>
int transform_max(IT begin, IT end, F func)
{
	int max_ret = INT_MIN;
	while (begin != end)
	{
		max_ret = max(max_ret, func(*begin));
		++begin;
	}
	return max_ret;
}

CSize ModelComplexPart::GetPreferredSize(void) const
{
	CSize size = ResizablePart::GetPreferredSize();
	bool hasStoredCustomSize = (size.cx * size.cy != 0);

	if (!hasStoredCustomSize && m_LeftPorts.empty() && m_RightPorts.empty()) {
		if (!m_bmp) {
			return CSize(WIDTH_MODEL, HEIGHT_MODEL);
		} else {
			return CSize(m_bmp->GetWidth(), m_bmp->GetHeight());
		}
	}

	const_cast<ModelComplexPart*>(this)->m_prominentAttrsCY = 0;
	const_cast<ModelComplexPart*>(this)->m_prominentAttrsNamesCX = 0;
	const_cast<ModelComplexPart*>(this)->m_prominentAttrsValuesCX = 0;

	int LeftPortsMaxLabelLength = 0;
	int RightPortsMaxLabelLength = 0;
	LOGFONTA logFont;
	getFacilities().GetFont(FONT_PORT)->gdipFont->GetLogFontA(getFacilities().getGraphics(), &logFont);
	{
		CDC dc;
		dc.Attach(GetDC(NULL));
		Gdiplus::Graphics g(dc);
		Gdiplus::Font f(dc, &logFont);
		Gdiplus::PointF zero(0.0, 0.0);
		Gdiplus::RectF box;

		auto measure_f = [&](const PortPart* portPart) -> int {
			CStringW porttext = static_cast<PortLabelPart* /* n.b. this lie doesn't matter */>(portPart->GetTextPart())->GetText();
			g.MeasureString(static_cast<const wchar_t*>(porttext), min(porttext.GetLength(), m_iMaxPortTextLength == 0 ? INT_MAX : m_iMaxPortTextLength), &f, zero, &box);
			return (int)(box.Width + 0.5f);
		};
		int base_port_gap = max(m_iRoundCornerRadius, GAP_LABEL + WIDTH_PORT + GAP_XMODELPORT);
		LeftPortsMaxLabelLength = max(-base_port_gap, transform_max(this->m_LeftPorts.begin(), m_LeftPorts.end(), measure_f)) + base_port_gap;
		RightPortsMaxLabelLength = max(-base_port_gap, transform_max(this->m_RightPorts.begin(), m_RightPorts.end(), measure_f)) + base_port_gap;
		LeftPortsMaxLabelLength = max(m_iRoundCornerRadius, LeftPortsMaxLabelLength);
		RightPortsMaxLabelLength = max(m_iRoundCornerRadius, RightPortsMaxLabelLength);
		const_cast<ModelComplexPart*>(this)->m_LeftPortsMaxLabelLength = LeftPortsMaxLabelLength;
		const_cast<ModelComplexPart*>(this)->m_RightPortsMaxLabelLength = RightPortsMaxLabelLength;

		for (auto prominentIt = prominentAttrs.begin(); prominentIt != prominentAttrs.end(); ++prominentIt)
		{
			Gdiplus::RectF prominentNameSize;
			g.MeasureString(static_cast<const wchar_t*>(prominentIt->name + L": "), -1, &f, zero, &prominentNameSize);
			Gdiplus::RectF prominentValueSize;
			g.MeasureString(static_cast<const wchar_t*>(prominentIt->value), -1, &f, zero, &prominentValueSize);

			const_cast<ModelComplexPart*>(this)->m_prominentAttrsCY += max(prominentNameSize.Height, prominentValueSize.Height);
			const_cast<ModelComplexPart*>(this)->m_prominentAttrsNamesCX = max(m_prominentAttrsNamesCX, prominentNameSize.Width);
			const_cast<ModelComplexPart*>(this)->m_prominentAttrsValuesCX = max(m_prominentAttrsValuesCX, prominentValueSize.Width);
		}
	}

	long lWidth = 0;
	if (m_bPortLabelInside) {
		ASSERT(m_iLongestPortTextLength >= 0 && m_iLongestPortTextLength <= 1000);
		ASSERT(m_iMaxPortTextLength >= 0 && m_iMaxPortTextLength <= 1000);
		lWidth = LeftPortsMaxLabelLength + RightPortsMaxLabelLength + max(m_prominentAttrsSize.cx, min(TypeableBitmapPart::GetPreferredSize().cx, MAXIMUM_ICON_SIZE_NONOVERLAPPING_WITH_PORT_LABELS));
	} else {
		lWidth = (8 * 3 + GAP_LABEL + WIDTH_PORT + GAP_XMODELPORT) * 2 + GAP_PORTLABEL;
	}

	long lHeight = GAP_YMODELPORT * 2 +
					max(m_LeftPorts.size(), m_RightPorts.size()) * (HEIGHT_PORT + GAP_PORT) - GAP_PORT;

	if (hasStoredCustomSize) {
		CSize calcSize = CSize(min(size.cx, lWidth), min(size.cy, lHeight));
		const_cast<DecoratorSDK::ModelComplexPart*>(this)->resizeLogic.SetMinimumSize(calcSize);
		return size;
	}

	CSize bitmapSize = TypeableBitmapPart::GetPreferredSize();
	lWidth = max(lWidth, max(bitmapSize.cx, m_prominentAttrsSize.cx));
	lHeight = max(lHeight, bitmapSize.cy + m_prominentAttrsSize.cy);
	const_cast<DecoratorSDK::ModelComplexPart*>(this)->resizeLogic.SetMinimumSize(CSize(lWidth, lHeight));
	return CSize(max((long) WIDTH_MODEL, lWidth), max((long) HEIGHT_MODEL, lHeight));
}

void ModelComplexPart::SetLocation(const CRect& location)
{
	for (std::vector<PortPart*>::iterator ii = m_LeftPorts.begin(); ii != m_LeftPorts.end(); ++ii) {
		(*ii)->SetLocation(location);
	}
	for (std::vector<PortPart*>::iterator ii = m_RightPorts.begin(); ii != m_RightPorts.end(); ++ii) {
		(*ii)->SetLocation(location);
	}
	TypeableBitmapPart::SetLocation(location);
}

CRect ModelComplexPart::GetLocation(void) const
{
	return TypeableBitmapPart::GetLocation();
}

CRect ModelComplexPart::GetLabelLocation(void) const
{
	CRect labelLocation(0,0,0,0);
	try {
		labelLocation = TypeableBitmapPart::GetLabelLocation();
	}
	catch(NotImplementedException&) {
		for (std::vector<PortPart*>::const_iterator ii = m_LeftPorts.begin(); ii != m_LeftPorts.end(); ++ii) {
			labelLocation = (*ii)->GetLabelLocation();
			if (!labelLocation.IsRectEmpty())
				break;
		}
		if (labelLocation.IsRectEmpty()) {
			for (std::vector<PortPart*>::const_iterator ii = m_RightPorts.begin(); ii != m_RightPorts.end(); ++ii) {
				labelLocation = (*ii)->GetLabelLocation();
				if (!labelLocation.IsRectEmpty())
					break;
			}
		}
	}
	return labelLocation;
}

CRect ModelComplexPart::GetPortLocation(CComPtr<IMgaFCO>& pFCO) const
{
	CRect portLocation(0,0,0,0);
	PortPart* pPortPart = GetPort(pFCO);
	if (pPortPart != NULL) {
		portLocation = pPortPart->GetLocation();
		CPoint offset = GetLocation().TopLeft();
		portLocation.OffsetRect(-offset.x, -offset.y);
	}
	return portLocation;
}

bool ModelComplexPart::GetPorts(CComPtr<IMgaFCOs>& portFCOs) const
{
	CComPtr<IMgaFCOs> spFCOs;
	COMTHROW(spFCOs.CoCreateInstance(OLESTR("Mga.MgaFCOs")));

	for (std::vector<PortPart*>::const_iterator ii = m_LeftPorts.begin(); ii != m_LeftPorts.end(); ++ii) {
		COMTHROW(spFCOs->Append((*ii)->GetFCO()));
	}
	for (std::vector<PortPart*>::const_iterator ii = m_RightPorts.begin(); ii != m_RightPorts.end(); ++ii) {
		COMTHROW(spFCOs->Append((*ii)->GetFCO()));
	}

	portFCOs = spFCOs.Detach();
	return true;
}

void ModelComplexPart::Draw(CDC* pDC, Gdiplus::Graphics* gdip)
{
	TypeableBitmapPart::Draw(pDC, gdip);
}

void ModelComplexPart::SaveState()
{
	for (std::vector<PortPart*>::iterator ii = m_LeftPorts.begin(); ii != m_LeftPorts.end(); ++ii) {
		(*ii)->SaveState();
	}
	for (std::vector<PortPart*>::iterator ii = m_RightPorts.begin(); ii != m_RightPorts.end(); ++ii) {
		(*ii)->SaveState();
	}
	TypeableBitmapPart::SaveState();
}

// New functions
void ModelComplexPart::InitializeEx(CComPtr<IMgaProject>& pProject, CComPtr<IMgaMetaPart>& pPart, CComPtr<IMgaFCO>& pFCO,
									HWND parentWnd, PreferenceMap& preferences)
{
	preferences[PREF_ICONDEFAULT]	= PreferenceVariant(createResString(IDB_MODEL));
	preferences[PREF_TILESDEFAULT]	= PreferenceVariant(getFacilities().getTileVector(TILE_MODELDEFAULT));
	preferences[PREF_TILESUNDEF]	= PreferenceVariant(getFacilities().getTileVector(TILE_PORTDEFAULT));

	if (pFCO) {
		CString strIcon;
		if (preferences.find(PREF_ICON) != preferences.end()) {
			strIcon = *preferences[PREF_ICON].uValue.pstrValue;
		} else {
			getFacilities().getPreference(pFCO, PREF_ICON, strIcon);
			if (!strIcon.IsEmpty()) {
				preferences[PREF_ICON] = PreferenceVariant(strIcon);
			}
		}

		PreferenceMap::iterator it = preferences.find(PREF_PORTLABELCOLOR);
		if (it != preferences.end())
			m_crPortText = it->second.uValue.crValue;
		else
			getFacilities().getPreference(pFCO, PREF_PORTLABELCOLOR, m_crPortText);

		it = preferences.find(PREF_PORTLABELINSIDE);
		if (it != preferences.end())
			m_bPortLabelInside = it->second.uValue.bValue;
		else
			getFacilities().getPreference(pFCO, PREF_PORTLABELINSIDE, m_bPortLabelInside);

		long o = m_iMaxPortTextLength;
		it = preferences.find(PREF_PORTLABELLENGTH);
		if (it != preferences.end()) {
			m_iMaxPortTextLength = it->second.uValue.lValue;
		} else {
			if (getFacilities().getPreference(pFCO, PREF_PORTLABELLENGTH, m_iMaxPortTextLength))
				m_iMaxPortTextLength = abs(m_iMaxPortTextLength);	//convert any negative value to positive
			else	//if not found in registry
				m_iMaxPortTextLength = MAX_PORT_LENGTH;	// the default value in Preferences
		}
		if (m_iMaxPortTextLength == 0)	// if 0 it means it has to show all way long
			m_iMaxPortTextLength = 999;	// so we set a huge value

		TypeableBitmapPart::InitializeEx(pProject, pPart, pFCO, parentWnd, preferences);

		LoadPorts();
	} else {
		TypeableBitmapPart::InitializeEx(pProject, pPart, pFCO, parentWnd, preferences);
	}

	if (m_LeftPorts.empty() && m_RightPorts.empty())
		m_pTileVector = getFacilities().getTileVector(TILE_ATOMDEFAULT);

	PreferenceMap::iterator iconint = preferences.find("iconint");
	PreferenceMap::iterator icon = preferences.find(PREF_ICON);
	if (iconint != preferences.end())
	{
		m_bmp = std::unique_ptr<Gdiplus::Bitmap>(new Gdiplus::Bitmap(((HINSTANCE)&__ImageBase), (WCHAR*) MAKEINTRESOURCE(iconint->second.uValue.lValue)));
	}
	else if (icon != preferences.end() && TypeableBitmapPart::m_bActive)
	{
		if ( ! getFacilities().arePathesValid() )
			return;
		std::vector<CString> vecPathes = getFacilities().getPathes();

		bool success = false;
		CString& strFName = *icon->second.uValue.pstrValue;

		for (unsigned int i = 0; i < vecPathes.size() ; i++ )
		{
			CString imageFileName = vecPathes[ i ] + strFName;
			// The graphics file formats supported by GDI+ are BMP, GIF, JPEG, PNG, TIFF, Exif, WMF, and EMF.
			m_bmp = std::unique_ptr<Gdiplus::Bitmap>(Gdiplus::Bitmap::FromFile(CStringW(imageFileName)));
			if( m_bmp && m_bmp->GetLastStatus() == Gdiplus::Ok)
			{
				UINT widthToSet = m_bmp->GetWidth();
				UINT heightToSet = m_bmp->GetHeight();
				ASSERT( widthToSet > 0);	// valid sizes, otherwise AutoRouter fails
				ASSERT( heightToSet > 0);
				break;
			}
			else
			{
				m_bmp = nullptr;
			}
		}
	}

	if (preferences.find(brushColorVariableName) == preferences.end() &&
		getFacilities().getPreference(m_spFCO, m_spMetaFCO, brushColorVariableName, m_crBrush) == false)
	{
		m_crBrush = RGB(0xdd, 0xdd, 0xdd);
	}

	long radius = 15;
	// FIXME: we are ignoring the 'Round rectangle corner enabled' variable true/false
	if (preferences.find(PREF_ROUNDCORNERRADIUS) == preferences.end())
	{
		if (getFacilities().getPreference(m_spFCO, m_spMetaFCO, PREF_ROUNDCORNERRADIUS, radius) == false)
		{
			m_iRoundCornerRadius = 15;
		} else {
			m_iRoundCornerRadius = (int)radius;
			if (m_iRoundCornerRadius < 1) {
				m_iRoundCornerRadius = 1;
			}
			// TODO: check if it is too big?
		}
	}
	
}

void ModelComplexPart::SetSelected(bool bIsSelected)
{
	for (std::vector<PortPart*>::iterator ii = m_LeftPorts.begin(); ii != m_LeftPorts.end(); ++ii) {
		(*ii)->SetSelected(bIsSelected);
	}
	for (std::vector<PortPart*>::iterator ii = m_RightPorts.begin(); ii != m_RightPorts.end(); ++ii) {
		(*ii)->SetSelected(bIsSelected);
	}
	TypeableBitmapPart::SetSelected(bIsSelected);
}

bool ModelComplexPart::MouseMoved(UINT nFlags, const CPoint& point, HDC transformHDC)
{
	HRESULT retVal = S_OK;
	try {
		if (TypeableBitmapPart::MouseMoved(nFlags, point, transformHDC))
			return true;
	}
	catch(hresult_exception& e) {
		retVal = e.hr;
	}
	catch(DecoratorException& e) {
		retVal = e.GetHResult();
	}
	if (retVal == S_OK || retVal == S_DECORATOR_EVENT_NOT_HANDLED || retVal == E_DECORATOR_NOT_IMPLEMENTED) {
		for (std::vector<PortPart*>::iterator ii = m_LeftPorts.begin(); ii != m_LeftPorts.end(); ++ii) {
			try {
				if ((*ii)->MouseMoved(nFlags, point, transformHDC))
					return true;
			}
			catch(hresult_exception& e) {
				retVal = e.hr;
			}
			catch(DecoratorException& e) {
				retVal = e.GetHResult();
			}
			if (retVal != S_OK && retVal != S_DECORATOR_EVENT_NOT_HANDLED && retVal != E_DECORATOR_NOT_IMPLEMENTED)
				break;
		}
	}
	if (retVal == S_OK || retVal == S_DECORATOR_EVENT_NOT_HANDLED || retVal == E_DECORATOR_NOT_IMPLEMENTED) {
		for (std::vector<PortPart*>::iterator ii = m_RightPorts.begin(); ii != m_RightPorts.end(); ++ii) {
			try {
				if ((*ii)->MouseMoved(nFlags, point, transformHDC))
					return true;
			}
			catch(hresult_exception& e) {
				retVal = e.hr;
			}
			catch(DecoratorException& e) {
				retVal = e.GetHResult();
			}
			if (retVal != S_OK && retVal != S_DECORATOR_EVENT_NOT_HANDLED && retVal != E_DECORATOR_NOT_IMPLEMENTED)
				break;
		}
	}

	return false;
}

bool ModelComplexPart::MouseLeftButtonDown(UINT nFlags, const CPoint& point, HDC transformHDC)
{
	HRESULT retVal = S_OK;
	try {
		if (TypeableBitmapPart::MouseLeftButtonDown(nFlags, point, transformHDC))
			return true;
	}
	catch(hresult_exception& e) {
		retVal = e.hr;
	}
	catch(DecoratorException& e) {
		retVal = e.GetHResult();
	}
	if (retVal == S_OK || retVal == S_DECORATOR_EVENT_NOT_HANDLED || retVal == E_DECORATOR_NOT_IMPLEMENTED) {
		for (std::vector<PortPart*>::iterator ii = m_LeftPorts.begin(); ii != m_LeftPorts.end(); ++ii) {
			try {
				if ((*ii)->MouseLeftButtonDown(nFlags, point, transformHDC))
					return true;
			}
			catch(hresult_exception& e) {
				retVal = e.hr;
			}
			catch(DecoratorException& e) {
				retVal = e.GetHResult();
			}
			if (retVal != S_OK && retVal != S_DECORATOR_EVENT_NOT_HANDLED && retVal != E_DECORATOR_NOT_IMPLEMENTED)
				break;
		}
	}
	if (retVal == S_OK || retVal == S_DECORATOR_EVENT_NOT_HANDLED || retVal == E_DECORATOR_NOT_IMPLEMENTED) {
		for (std::vector<PortPart*>::iterator ii = m_RightPorts.begin(); ii != m_RightPorts.end(); ++ii) {
			try {
				if ((*ii)->MouseLeftButtonDown(nFlags, point, transformHDC))
					return true;
			}
			catch(hresult_exception& e) {
				retVal = e.hr;
			}
			catch(DecoratorException& e) {
				retVal = e.GetHResult();
			}
			if (retVal != S_OK && retVal != S_DECORATOR_EVENT_NOT_HANDLED && retVal != E_DECORATOR_NOT_IMPLEMENTED)
				break;
		}
	}

	return false;
}

bool ModelComplexPart::MouseLeftButtonUp(UINT nFlags, const CPoint& point, HDC transformHDC)
{
	HRESULT retVal = S_OK;
	try {
		if (TypeableBitmapPart::MouseLeftButtonUp(nFlags, point, transformHDC))
			return true;
	}
	catch(hresult_exception& e) {
		retVal = e.hr;
	}
	catch(DecoratorException& e) {
		retVal = e.GetHResult();
	}
	if (retVal == S_OK || retVal == S_DECORATOR_EVENT_NOT_HANDLED || retVal == E_DECORATOR_NOT_IMPLEMENTED) {
		for (std::vector<PortPart*>::iterator ii = m_LeftPorts.begin(); ii != m_LeftPorts.end(); ++ii) {
			try {
				if ((*ii)->MouseLeftButtonUp(nFlags, point, transformHDC))
					return true;
			}
			catch(hresult_exception& e) {
				retVal = e.hr;
			}
			catch(DecoratorException& e) {
				retVal = e.GetHResult();
			}
			if (retVal != S_OK && retVal != S_DECORATOR_EVENT_NOT_HANDLED && retVal != E_DECORATOR_NOT_IMPLEMENTED)
				break;
		}
	}
	if (retVal == S_OK || retVal == S_DECORATOR_EVENT_NOT_HANDLED || retVal == E_DECORATOR_NOT_IMPLEMENTED) {
		for (std::vector<PortPart*>::iterator ii = m_RightPorts.begin(); ii != m_RightPorts.end(); ++ii) {
			try {
				if ((*ii)->MouseLeftButtonUp(nFlags, point, transformHDC))
					return true;
			}
			catch(hresult_exception& e) {
				retVal = e.hr;
			}
			catch(DecoratorException& e) {
				retVal = e.GetHResult();
			}
			if (retVal != S_OK && retVal != S_DECORATOR_EVENT_NOT_HANDLED && retVal != E_DECORATOR_NOT_IMPLEMENTED)
				break;
		}
	}

	return false;
}

bool ModelComplexPart::MouseLeftButtonDoubleClick(UINT nFlags, const CPoint& point, HDC transformHDC)
{
	HRESULT retVal = S_OK;
	try {
		if (TypeableBitmapPart::MouseLeftButtonDoubleClick(nFlags, point, transformHDC))
			return true;
	}
	catch(hresult_exception& e) {
		retVal = e.hr;
	}
	catch(DecoratorException& e) {
		retVal = e.GetHResult();
	}
	if (retVal == S_OK || retVal == S_DECORATOR_EVENT_NOT_HANDLED || retVal == E_DECORATOR_NOT_IMPLEMENTED) {
		for (std::vector<PortPart*>::iterator ii = m_LeftPorts.begin(); ii != m_LeftPorts.end(); ++ii) {
			try {
				if ((*ii)->MouseLeftButtonDoubleClick(nFlags, point, transformHDC))
					return true;
			}
			catch(hresult_exception& e) {
				retVal = e.hr;
			}
			catch(DecoratorException& e) {
				retVal = e.GetHResult();
			}
			if (retVal != S_OK && retVal != S_DECORATOR_EVENT_NOT_HANDLED && retVal != E_DECORATOR_NOT_IMPLEMENTED)
				break;
		}
	}
	if (retVal == S_OK || retVal == S_DECORATOR_EVENT_NOT_HANDLED || retVal == E_DECORATOR_NOT_IMPLEMENTED) {
		for (std::vector<PortPart*>::iterator ii = m_RightPorts.begin(); ii != m_RightPorts.end(); ++ii) {
			try {
				if ((*ii)->MouseLeftButtonDoubleClick(nFlags, point, transformHDC))
					return true;
			}
			catch(hresult_exception& e) {
				retVal = e.hr;
			}
			catch(DecoratorException& e) {
				retVal = e.GetHResult();
			}
			if (retVal != S_OK && retVal != S_DECORATOR_EVENT_NOT_HANDLED && retVal != E_DECORATOR_NOT_IMPLEMENTED)
				break;
		}
	}

	return false;
}

bool ModelComplexPart::MouseRightButtonDown(HMENU hCtxMenu, UINT nFlags, const CPoint& point, HDC transformHDC)
{
	HRESULT retVal = S_OK;
	try {
		if (TypeableBitmapPart::MouseRightButtonDown(hCtxMenu, nFlags, point, transformHDC))
			return true;
	}
	catch(hresult_exception& e) {
		retVal = e.hr;
	}
	catch(DecoratorException& e) {
		retVal = e.GetHResult();
	}
	if (retVal == S_OK || retVal == S_DECORATOR_EVENT_NOT_HANDLED || retVal == E_DECORATOR_NOT_IMPLEMENTED) {
		for (std::vector<PortPart*>::iterator ii = m_LeftPorts.begin(); ii != m_LeftPorts.end(); ++ii) {
			try {
				if ((*ii)->MouseRightButtonDown(hCtxMenu, nFlags, point, transformHDC))
					return true;
			}
			catch(hresult_exception& e) {
				retVal = e.hr;
			}
			catch(DecoratorException& e) {
				retVal = e.GetHResult();
			}
			if (retVal != S_OK && retVal != S_DECORATOR_EVENT_NOT_HANDLED && retVal != E_DECORATOR_NOT_IMPLEMENTED)
				break;
		}
	}
	if (retVal == S_OK || retVal == S_DECORATOR_EVENT_NOT_HANDLED || retVal == E_DECORATOR_NOT_IMPLEMENTED) {
		for (std::vector<PortPart*>::iterator ii = m_RightPorts.begin(); ii != m_RightPorts.end(); ++ii) {
			try {
				if ((*ii)->MouseRightButtonDown(hCtxMenu, nFlags, point, transformHDC))
					return true;
			}
			catch(hresult_exception& e) {
				retVal = e.hr;
			}
			catch(DecoratorException& e) {
				retVal = e.GetHResult();
			}
			if (retVal != S_OK && retVal != S_DECORATOR_EVENT_NOT_HANDLED && retVal != E_DECORATOR_NOT_IMPLEMENTED)
				break;
		}
	}

	return false;
}

bool ModelComplexPart::MouseRightButtonUp(UINT nFlags, const CPoint& point, HDC transformHDC)
{
	HRESULT retVal = S_OK;
	try {
		if (TypeableBitmapPart::MouseRightButtonUp(nFlags, point, transformHDC))
			return true;
	}
	catch(hresult_exception& e) {
		retVal = e.hr;
	}
	catch(DecoratorException& e) {
		retVal = e.GetHResult();
	}
	if (retVal == S_OK || retVal == S_DECORATOR_EVENT_NOT_HANDLED || retVal == E_DECORATOR_NOT_IMPLEMENTED) {
		for (std::vector<PortPart*>::iterator ii = m_LeftPorts.begin(); ii != m_LeftPorts.end(); ++ii) {
			try {
				if ((*ii)->MouseRightButtonUp(nFlags, point, transformHDC))
					return true;
			}
			catch(hresult_exception& e) {
				retVal = e.hr;
			}
			catch(DecoratorException& e) {
				retVal = e.GetHResult();
			}
			if (retVal != S_OK && retVal != S_DECORATOR_EVENT_NOT_HANDLED && retVal != E_DECORATOR_NOT_IMPLEMENTED)
				break;
		}
	}
	if (retVal == S_OK || retVal == S_DECORATOR_EVENT_NOT_HANDLED || retVal == E_DECORATOR_NOT_IMPLEMENTED) {
		for (std::vector<PortPart*>::iterator ii = m_RightPorts.begin(); ii != m_RightPorts.end(); ++ii) {
			try {
				if ((*ii)->MouseRightButtonUp(nFlags, point, transformHDC))
					return true;
			}
			catch(hresult_exception& e) {
				retVal = e.hr;
			}
			catch(DecoratorException& e) {
				retVal = e.GetHResult();
			}
			if (retVal != S_OK && retVal != S_DECORATOR_EVENT_NOT_HANDLED && retVal != E_DECORATOR_NOT_IMPLEMENTED)
				break;
		}
	}

	return false;
}

bool ModelComplexPart::MouseRightButtonDoubleClick(UINT nFlags, const CPoint& point, HDC transformHDC)
{
	HRESULT retVal = S_OK;
	try {
		if (TypeableBitmapPart::MouseRightButtonDoubleClick(nFlags, point, transformHDC))
			return true;
	}
	catch(hresult_exception& e) {
		retVal = e.hr;
	}
	catch(DecoratorException& e) {
		retVal = e.GetHResult();
	}
	if (retVal == S_OK || retVal == S_DECORATOR_EVENT_NOT_HANDLED || retVal == E_DECORATOR_NOT_IMPLEMENTED) {
		for (std::vector<PortPart*>::iterator ii = m_LeftPorts.begin(); ii != m_LeftPorts.end(); ++ii) {
			try {
				if ((*ii)->MouseRightButtonDoubleClick(nFlags, point, transformHDC))
					return true;
			}
			catch(hresult_exception& e) {
				retVal = e.hr;
			}
			catch(DecoratorException& e) {
				retVal = e.GetHResult();
			}
			if (retVal != S_OK && retVal != S_DECORATOR_EVENT_NOT_HANDLED && retVal != E_DECORATOR_NOT_IMPLEMENTED)
				break;
		}
	}
	if (retVal == S_OK || retVal == S_DECORATOR_EVENT_NOT_HANDLED || retVal == E_DECORATOR_NOT_IMPLEMENTED) {
		for (std::vector<PortPart*>::iterator ii = m_RightPorts.begin(); ii != m_RightPorts.end(); ++ii) {
			try {
				if ((*ii)->MouseRightButtonDoubleClick(nFlags, point, transformHDC))
					return true;
			}
			catch(hresult_exception& e) {
				retVal = e.hr;
			}
			catch(DecoratorException& e) {
				retVal = e.GetHResult();
			}
			if (retVal != S_OK && retVal != S_DECORATOR_EVENT_NOT_HANDLED && retVal != E_DECORATOR_NOT_IMPLEMENTED)
				break;
		}
	}

	return false;
}

bool ModelComplexPart::MouseMiddleButtonDown(UINT nFlags, const CPoint& point, HDC transformHDC)
{
	HRESULT retVal = S_OK;
	try {
		if (TypeableBitmapPart::MouseMiddleButtonDown(nFlags, point, transformHDC))
			return true;
	}
	catch(hresult_exception& e) {
		retVal = e.hr;
	}
	catch(DecoratorException& e) {
		retVal = e.GetHResult();
	}
	if (retVal == S_OK || retVal == S_DECORATOR_EVENT_NOT_HANDLED || retVal == E_DECORATOR_NOT_IMPLEMENTED) {
		for (std::vector<PortPart*>::iterator ii = m_LeftPorts.begin(); ii != m_LeftPorts.end(); ++ii) {
			try {
				if ((*ii)->MouseMiddleButtonDown(nFlags, point, transformHDC))
					return true;
			}
			catch(hresult_exception& e) {
				retVal = e.hr;
			}
			catch(DecoratorException& e) {
				retVal = e.GetHResult();
			}
			if (retVal != S_OK && retVal != S_DECORATOR_EVENT_NOT_HANDLED && retVal != E_DECORATOR_NOT_IMPLEMENTED)
				break;
		}
	}
	if (retVal == S_OK || retVal == S_DECORATOR_EVENT_NOT_HANDLED || retVal == E_DECORATOR_NOT_IMPLEMENTED) {
		for (std::vector<PortPart*>::iterator ii = m_RightPorts.begin(); ii != m_RightPorts.end(); ++ii) {
			try {
				if ((*ii)->MouseMiddleButtonDown(nFlags, point, transformHDC))
					return true;
			}
			catch(hresult_exception& e) {
				retVal = e.hr;
			}
			catch(DecoratorException& e) {
				retVal = e.GetHResult();
			}
			if (retVal != S_OK && retVal != S_DECORATOR_EVENT_NOT_HANDLED && retVal != E_DECORATOR_NOT_IMPLEMENTED)
				break;
		}
	}

	return false;
}

bool ModelComplexPart::MouseMiddleButtonUp(UINT nFlags, const CPoint& point, HDC transformHDC)
{
	HRESULT retVal = S_OK;
	try {
		if (TypeableBitmapPart::MouseMiddleButtonUp(nFlags, point, transformHDC))
			return true;
	}
	catch(hresult_exception& e) {
		retVal = e.hr;
	}
	catch(DecoratorException& e) {
		retVal = e.GetHResult();
	}
	if (retVal == S_OK || retVal == S_DECORATOR_EVENT_NOT_HANDLED || retVal == E_DECORATOR_NOT_IMPLEMENTED) {
		for (std::vector<PortPart*>::iterator ii = m_LeftPorts.begin(); ii != m_LeftPorts.end(); ++ii) {
			try {
				if ((*ii)->MouseMiddleButtonUp(nFlags, point, transformHDC))
					return true;
			}
			catch(hresult_exception& e) {
				retVal = e.hr;
			}
			catch(DecoratorException& e) {
				retVal = e.GetHResult();
			}
			if (retVal != S_OK && retVal != S_DECORATOR_EVENT_NOT_HANDLED && retVal != E_DECORATOR_NOT_IMPLEMENTED)
				break;
		}
	}
	if (retVal == S_OK || retVal == S_DECORATOR_EVENT_NOT_HANDLED || retVal == E_DECORATOR_NOT_IMPLEMENTED) {
		for (std::vector<PortPart*>::iterator ii = m_RightPorts.begin(); ii != m_RightPorts.end(); ++ii) {
			try {
				if ((*ii)->MouseMiddleButtonUp(nFlags, point, transformHDC))
					return true;
			}
			catch(hresult_exception& e) {
				retVal = e.hr;
			}
			catch(DecoratorException& e) {
				retVal = e.GetHResult();
			}
			if (retVal != S_OK && retVal != S_DECORATOR_EVENT_NOT_HANDLED && retVal != E_DECORATOR_NOT_IMPLEMENTED)
				break;
		}
	}

	return false;
}

bool ModelComplexPart::MouseMiddleButtonDoubleClick(UINT nFlags, const CPoint& point, HDC transformHDC)
{
	HRESULT retVal = S_OK;
	try {
		if (TypeableBitmapPart::MouseMiddleButtonDoubleClick(nFlags, point, transformHDC))
			return true;
	}
	catch(hresult_exception& e) {
		retVal = e.hr;
	}
	catch(DecoratorException& e) {
		retVal = e.GetHResult();
	}
	if (retVal == S_OK || retVal == S_DECORATOR_EVENT_NOT_HANDLED || retVal == E_DECORATOR_NOT_IMPLEMENTED) {
		for (std::vector<PortPart*>::iterator ii = m_LeftPorts.begin(); ii != m_LeftPorts.end(); ++ii) {
			try {
				if ((*ii)->MouseMiddleButtonDoubleClick(nFlags, point, transformHDC))
					return true;
			}
			catch(hresult_exception& e) {
				retVal = e.hr;
			}
			catch(DecoratorException& e) {
				retVal = e.GetHResult();
			}
			if (retVal != S_OK && retVal != S_DECORATOR_EVENT_NOT_HANDLED && retVal != E_DECORATOR_NOT_IMPLEMENTED)
				break;
		}
	}
	if (retVal == S_OK || retVal == S_DECORATOR_EVENT_NOT_HANDLED || retVal == E_DECORATOR_NOT_IMPLEMENTED) {
		for (std::vector<PortPart*>::iterator ii = m_RightPorts.begin(); ii != m_RightPorts.end(); ++ii) {
			try {
				if ((*ii)->MouseMiddleButtonDoubleClick(nFlags, point, transformHDC))
					return true;
			}
			catch(hresult_exception& e) {
				retVal = e.hr;
			}
			catch(DecoratorException& e) {
				retVal = e.GetHResult();
			}
			if (retVal != S_OK && retVal != S_DECORATOR_EVENT_NOT_HANDLED && retVal != E_DECORATOR_NOT_IMPLEMENTED)
				break;
		}
	}

	return false;
}

bool ModelComplexPart::MouseWheelTurned(UINT nFlags, short distance, const CPoint& point, HDC transformHDC)
{
	HRESULT retVal = S_OK;
	try {
		if (TypeableBitmapPart::MouseWheelTurned(nFlags, distance, point, transformHDC))
			return true;
	}
	catch(hresult_exception& e) {
		retVal = e.hr;
	}
	catch(DecoratorException& e) {
		retVal = e.GetHResult();
	}
	if (retVal == S_OK || retVal == S_DECORATOR_EVENT_NOT_HANDLED || retVal == E_DECORATOR_NOT_IMPLEMENTED) {
		for (std::vector<PortPart*>::iterator ii = m_LeftPorts.begin(); ii != m_LeftPorts.end(); ++ii) {
			try {
				if ((*ii)->MouseWheelTurned(nFlags, distance, point, transformHDC))
					return true;
			}
			catch(hresult_exception& e) {
				retVal = e.hr;
			}
			catch(DecoratorException& e) {
				retVal = e.GetHResult();
			}
			if (retVal != S_OK && retVal != S_DECORATOR_EVENT_NOT_HANDLED && retVal != E_DECORATOR_NOT_IMPLEMENTED)
				break;
		}
	}
	if (retVal == S_OK || retVal == S_DECORATOR_EVENT_NOT_HANDLED || retVal == E_DECORATOR_NOT_IMPLEMENTED) {
		for (std::vector<PortPart*>::iterator ii = m_RightPorts.begin(); ii != m_RightPorts.end(); ++ii) {
			try {
				if ((*ii)->MouseWheelTurned(nFlags, distance, point, transformHDC))
					return true;
			}
			catch(hresult_exception& e) {
				retVal = e.hr;
			}
			catch(DecoratorException& e) {
				retVal = e.GetHResult();
			}
			if (retVal != S_OK && retVal != S_DECORATOR_EVENT_NOT_HANDLED && retVal != E_DECORATOR_NOT_IMPLEMENTED)
				break;
		}
	}

	return false;
}

bool ModelComplexPart::DragEnter(DROPEFFECT* dropEffect, COleDataObject* pDataObject, DWORD dwKeyState, const CPoint& point, HDC transformHDC)
{
	HRESULT retVal = S_OK;
	try {
		if (TypeableBitmapPart::DragEnter(dropEffect, pDataObject, dwKeyState, point, transformHDC))
			return true;
	}
	catch(hresult_exception& e) {
		retVal = e.hr;
	}
	catch(DecoratorException& e) {
		retVal = e.GetHResult();
	}
	if (retVal == S_OK || retVal == S_DECORATOR_EVENT_NOT_HANDLED || retVal == E_DECORATOR_NOT_IMPLEMENTED) {
		for (std::vector<PortPart*>::iterator ii = m_LeftPorts.begin(); ii != m_LeftPorts.end(); ++ii) {
			try {
				if ((*ii)->DragEnter(dropEffect, pDataObject, dwKeyState, point, transformHDC))
					return true;
			}
			catch(hresult_exception& e) {
				retVal = e.hr;
			}
			catch(DecoratorException& e) {
				retVal = e.GetHResult();
			}
			if (retVal != S_OK && retVal != S_DECORATOR_EVENT_NOT_HANDLED && retVal != E_DECORATOR_NOT_IMPLEMENTED)
				break;
		}
	}
	if (retVal == S_OK || retVal == S_DECORATOR_EVENT_NOT_HANDLED || retVal == E_DECORATOR_NOT_IMPLEMENTED) {
		for (std::vector<PortPart*>::iterator ii = m_RightPorts.begin(); ii != m_RightPorts.end(); ++ii) {
			try {
				if ((*ii)->DragEnter(dropEffect, pDataObject, dwKeyState, point, transformHDC))
					return true;
			}
			catch(hresult_exception& e) {
				retVal = e.hr;
			}
			catch(DecoratorException& e) {
				retVal = e.GetHResult();
			}
			if (retVal != S_OK && retVal != S_DECORATOR_EVENT_NOT_HANDLED && retVal != E_DECORATOR_NOT_IMPLEMENTED)
				break;
		}
	}

	return false;
}

bool ModelComplexPart::DragOver(DROPEFFECT* dropEffect, COleDataObject* pDataObject, DWORD dwKeyState, const CPoint& point, HDC transformHDC)
{
	HRESULT retVal = S_OK;
	try {
		if (TypeableBitmapPart::DragOver(dropEffect, pDataObject, dwKeyState, point, transformHDC))
			return true;
	}
	catch(hresult_exception& e) {
		retVal = e.hr;
	}
	catch(DecoratorException& e) {
		retVal = e.GetHResult();
	}
	if (retVal == S_OK || retVal == S_DECORATOR_EVENT_NOT_HANDLED || retVal == E_DECORATOR_NOT_IMPLEMENTED) {
		for (std::vector<PortPart*>::iterator ii = m_LeftPorts.begin(); ii != m_LeftPorts.end(); ++ii) {
			try {
				if ((*ii)->DragOver(dropEffect, pDataObject, dwKeyState, point, transformHDC))
					return true;
			}
			catch(hresult_exception& e) {
				retVal = e.hr;
			}
			catch(DecoratorException& e) {
				retVal = e.GetHResult();
			}
			if (retVal != S_OK && retVal != S_DECORATOR_EVENT_NOT_HANDLED && retVal != E_DECORATOR_NOT_IMPLEMENTED)
				break;
		}
	}
	if (retVal == S_OK || retVal == S_DECORATOR_EVENT_NOT_HANDLED || retVal == E_DECORATOR_NOT_IMPLEMENTED) {
		for (std::vector<PortPart*>::iterator ii = m_RightPorts.begin(); ii != m_RightPorts.end(); ++ii) {
			try {
				if ((*ii)->DragOver(dropEffect, pDataObject, dwKeyState, point, transformHDC))
					return true;
			}
			catch(hresult_exception& e) {
				retVal = e.hr;
			}
			catch(DecoratorException& e) {
				retVal = e.GetHResult();
			}
			if (retVal != S_OK && retVal != S_DECORATOR_EVENT_NOT_HANDLED && retVal != E_DECORATOR_NOT_IMPLEMENTED)
				break;
		}
	}

	return false;
}

bool ModelComplexPart::Drop(COleDataObject* pDataObject, DROPEFFECT dropEffect, const CPoint& point, HDC transformHDC)
{
	HRESULT retVal = S_OK;
	try {
		if (TypeableBitmapPart::Drop(pDataObject, dropEffect, point, transformHDC))
			return true;
	}
	catch(hresult_exception& e) {
		retVal = e.hr;
	}
	catch(DecoratorException& e) {
		retVal = e.GetHResult();
	}
	if (retVal == S_OK || retVal == S_DECORATOR_EVENT_NOT_HANDLED || retVal == E_DECORATOR_NOT_IMPLEMENTED) {
		for (std::vector<PortPart*>::iterator ii = m_LeftPorts.begin(); ii != m_LeftPorts.end(); ++ii) {
			try {
				if ((*ii)->Drop(pDataObject, dropEffect, point, transformHDC))
					return true;
			}
			catch(hresult_exception& e) {
				retVal = e.hr;
			}
			catch(DecoratorException& e) {
				retVal = e.GetHResult();
			}
			if (retVal != S_OK && retVal != S_DECORATOR_EVENT_NOT_HANDLED && retVal != E_DECORATOR_NOT_IMPLEMENTED)
				break;
		}
	}
	if (retVal == S_OK || retVal == S_DECORATOR_EVENT_NOT_HANDLED || retVal == E_DECORATOR_NOT_IMPLEMENTED) {
		for (std::vector<PortPart*>::iterator ii = m_RightPorts.begin(); ii != m_RightPorts.end(); ++ii) {
			try {
				if ((*ii)->Drop(pDataObject, dropEffect, point, transformHDC))
					return true;
			}
			catch(hresult_exception& e) {
				retVal = e.hr;
			}
			catch(DecoratorException& e) {
				retVal = e.GetHResult();
			}
			if (retVal != S_OK && retVal != S_DECORATOR_EVENT_NOT_HANDLED && retVal != E_DECORATOR_NOT_IMPLEMENTED)
				break;
		}
	}

	return false;
}

bool ModelComplexPart::DropFile(HDROP p_hDropInfo, const CPoint& point, HDC transformHDC)
{
	HRESULT retVal = S_OK;
	try {
		if (TypeableBitmapPart::DropFile(p_hDropInfo, point, transformHDC))
			return true;
	}
	catch(hresult_exception& e) {
		retVal = e.hr;
	}
	catch(DecoratorException& e) {
		retVal = e.GetHResult();
	}
	if (retVal == S_OK || retVal == S_DECORATOR_EVENT_NOT_HANDLED || retVal == E_DECORATOR_NOT_IMPLEMENTED) {
		for (std::vector<PortPart*>::iterator ii = m_LeftPorts.begin(); ii != m_LeftPorts.end(); ++ii) {
			try {
				if ((*ii)->DropFile(p_hDropInfo, point, transformHDC))
					return true;
			}
			catch(hresult_exception& e) {
				retVal = e.hr;
			}
			catch(DecoratorException& e) {
				retVal = e.GetHResult();
			}
			if (retVal != S_OK && retVal != S_DECORATOR_EVENT_NOT_HANDLED && retVal != E_DECORATOR_NOT_IMPLEMENTED)
				break;
		}
	}
	if (retVal == S_OK || retVal == S_DECORATOR_EVENT_NOT_HANDLED || retVal == E_DECORATOR_NOT_IMPLEMENTED) {
		for (std::vector<PortPart*>::iterator ii = m_RightPorts.begin(); ii != m_RightPorts.end(); ++ii) {
			try {
				if ((*ii)->DropFile(p_hDropInfo, point, transformHDC))
					return true;
			}
			catch(hresult_exception& e) {
				retVal = e.hr;
			}
			catch(DecoratorException& e) {
				retVal = e.GetHResult();
			}
			if (retVal != S_OK && retVal != S_DECORATOR_EVENT_NOT_HANDLED && retVal != E_DECORATOR_NOT_IMPLEMENTED)
				break;
		}
	}

	return false;
}

bool ModelComplexPart::MenuItemSelected(UINT menuItemId, UINT nFlags, const CPoint& point, HDC transformHDC)
{
	HRESULT retVal = S_OK;
	try {
		if (TypeableBitmapPart::MenuItemSelected(menuItemId, nFlags, point, transformHDC))
			return true;
	}
	catch(hresult_exception& e) {
		retVal = e.hr;
	}
	catch(DecoratorException& e) {
		retVal = e.GetHResult();
	}
	if (retVal == S_OK || retVal == S_DECORATOR_EVENT_NOT_HANDLED || retVal == E_DECORATOR_NOT_IMPLEMENTED) {
		for (std::vector<PortPart*>::iterator ii = m_LeftPorts.begin(); ii != m_LeftPorts.end(); ++ii) {
			try {
				if ((*ii)->MenuItemSelected(menuItemId, nFlags, point, transformHDC))
					return true;
			}
			catch(hresult_exception& e) {
				retVal = e.hr;
			}
			catch(DecoratorException& e) {
				retVal = e.GetHResult();
			}
			if (retVal != S_OK && retVal != S_DECORATOR_EVENT_NOT_HANDLED && retVal != E_DECORATOR_NOT_IMPLEMENTED)
				break;
		}
	}
	if (retVal == S_OK || retVal == S_DECORATOR_EVENT_NOT_HANDLED || retVal == E_DECORATOR_NOT_IMPLEMENTED) {
		for (std::vector<PortPart*>::iterator ii = m_RightPorts.begin(); ii != m_RightPorts.end(); ++ii) {
			try {
				if ((*ii)->MenuItemSelected(menuItemId, nFlags, point, transformHDC))
					return true;
			}
			catch(hresult_exception& e) {
				retVal = e.hr;
			}
			catch(DecoratorException& e) {
				retVal = e.GetHResult();
			}
			if (retVal != S_OK && retVal != S_DECORATOR_EVENT_NOT_HANDLED && retVal != E_DECORATOR_NOT_IMPLEMENTED)
				break;
		}
	}

	return false;
}

bool ModelComplexPart::OperationCanceledByGME(void)
{
	HRESULT retVal = S_OK;
	try {
		if (TypeableBitmapPart::OperationCanceledByGME())
			return true;
	}
	catch(hresult_exception& e) {
		retVal = e.hr;
	}
	catch(DecoratorException& e) {
		retVal = e.GetHResult();
	}
	if (retVal == S_OK || retVal == S_DECORATOR_EVENT_NOT_HANDLED || retVal == E_DECORATOR_NOT_IMPLEMENTED) {
		for (std::vector<PortPart*>::iterator ii = m_LeftPorts.begin(); ii != m_LeftPorts.end(); ++ii) {
			try {
				if ((*ii)->OperationCanceledByGME())
					return true;
			}
			catch(hresult_exception& e) {
				retVal = e.hr;
			}
			catch(DecoratorException& e) {
				retVal = e.GetHResult();
			}
			if (retVal != S_OK && retVal != S_DECORATOR_EVENT_NOT_HANDLED && retVal != E_DECORATOR_NOT_IMPLEMENTED)
				break;
		}
	}
	if (retVal == S_OK || retVal == S_DECORATOR_EVENT_NOT_HANDLED || retVal == E_DECORATOR_NOT_IMPLEMENTED) {
		for (std::vector<PortPart*>::iterator ii = m_RightPorts.begin(); ii != m_RightPorts.end(); ++ii) {
			try {
				if ((*ii)->OperationCanceledByGME())
					return true;
			}
			catch(hresult_exception& e) {
				retVal = e.hr;
			}
			catch(DecoratorException& e) {
				retVal = e.GetHResult();
			}
			if (retVal != S_OK && retVal != S_DECORATOR_EVENT_NOT_HANDLED && retVal != E_DECORATOR_NOT_IMPLEMENTED)
				break;
		}
	}

	return false;
}

void ModelComplexPart::DrawBackground(CDC* pDC, Gdiplus::Graphics* gdip)
{
	CSize cExtentD = pDC->GetViewportExt();
	CSize cExtentL = pDC->GetWindowExt();
	int prominent_x = m_bmp.get() ? 0 : m_LeftPortsMaxLabelLength;

	CRect cRect = TypeableBitmapPart::GetBoxLocation(false);
	cRect.BottomRight() -= CPoint(1, 1);

	if (!m_spFCO) {
		TypeableBitmapPart::DrawBackground(pDC, gdip);
		return;
	}
	if (this->m_bReferenced) {
		TypeableBitmapPart::DrawBackground(pDC, gdip);
		return;
	}
	CRect location = cRect;
	if (m_bmp && TypeableBitmapPart::m_bActive)
	{
		//if (m_LeftPorts.size() != 0 || m_RightPorts.size() != 0)
		// draw this type of background only if icon is defined
		Gdiplus::Rect rect(cRect.left, cRect.top, cRect.Width(), cRect.Height());
		Gdiplus::LinearGradientBrush linearGradientBrush(rect,
			Gdiplus::Color(GetRValue(m_crBrush), GetGValue(m_crBrush), GetBValue(m_crBrush)),
			Gdiplus::Color(GetRValue(m_crGradient), GetGValue(m_crGradient), GetBValue(m_crGradient)),
			m_iGradientDirection);

		Gdiplus::SolidBrush solidBrush(Gdiplus::Color(GetRValue(m_crBrush), GetGValue(m_crBrush), GetBValue(m_crBrush)));
		
		Gdiplus::Brush& brush = m_bGradientFill ? (Gdiplus::Brush&)linearGradientBrush : (Gdiplus::Brush&)solidBrush;
		
		Gdiplus::GraphicsPath path;
		path.AddArc(location.left, location.top, m_iRoundCornerRadius, m_iRoundCornerRadius, 180, 90);
		path.AddArc(location.right - m_iRoundCornerRadius, location.top, m_iRoundCornerRadius, m_iRoundCornerRadius, 270, 90);
		path.AddArc(location.right - m_iRoundCornerRadius, location.bottom - m_iRoundCornerRadius, m_iRoundCornerRadius, m_iRoundCornerRadius, 0, 90);
		path.AddArc(location.left, location.bottom - m_iRoundCornerRadius, m_iRoundCornerRadius, m_iRoundCornerRadius, 90, 90);
		gdip->FillPath(&brush, &path);
	} else {
		// use the old model style if no icon is defined
		int iDepth = (m_bReferenced) ? 2 : ((m_iTypeInfo == 3) ? 4 : 8);
		CRect cRect2 = cRect;
		cRect2.InflateRect(1, 1);
		getFacilities().DrawBox(gdip, cRect2, (!m_bActive) ? COLOR_LIGHTGRAY : (m_bOverlay) ? m_crOverlay : COLOR_GRAY,
							iDepth, m_bRoundCornerRect, m_bRoundCornerRadius);
		getFacilities().DrawRect(gdip, cRect, (m_bActive) ? m_crBorder : COLOR_GRAY, 1, m_bRoundCornerRect,
							m_bRoundCornerRadius);
	}

	if (m_bmp.get())
	{
		int height = m_bmp->GetHeight();
		int width = m_bmp->GetWidth();

		int x; // Render according to m_LeftPortsMaxLabelLength if the image fits inside the rounded rectangle
		// if not, just center it
		int gutter = min(m_LeftPortsMaxLabelLength + m_RightPortsMaxLabelLength, cRect.Width() - width);
		int x_margin = (int)(gutter * (double)m_LeftPortsMaxLabelLength / (m_LeftPortsMaxLabelLength + m_RightPortsMaxLabelLength));
		int left_corner = m_iRoundCornerRadius;
		int right_corner = cRect.Width() - m_iRoundCornerRadius;
		if (x_margin < left_corner - 1 /*slop*/ || x_margin > right_corner - width + 1 /*slop*/)
		{
			// center
			x = cRect.Width() / 2 - (int)m_bmp->GetWidth() / 2;
		}
		else
		{
			x = x_margin;
		}
		x += max(0, m_prominentAttrsSize.cx - (int)m_bmp->GetWidth()) / 2;
		prominent_x = x;

		Gdiplus::Rect grect((int)cRect.left + x, (int)cRect.top + (cRect.Height() - m_prominentAttrsSize.cy) / 2 - (int)m_bmp->GetHeight() / 2,
			width, height);
		gdip->DrawImage(m_bmp.get(), grect, 0, 0, (int)m_bmp->GetWidth(), (int)m_bmp->GetHeight(), Gdiplus::UnitPixel);
	}

	for (std::vector<PortPart*>::iterator ii = m_LeftPorts.begin(); ii != m_LeftPorts.end(); ++ii) {
		(*ii)->Draw(pDC, gdip);
	}
	for (std::vector<PortPart*>::iterator ii = m_RightPorts.begin(); ii != m_RightPorts.end(); ++ii) {
		(*ii)->Draw(pDC, gdip);
	}
	LOGFONTA logFont;
	getFacilities().GetFont(FONT_PORT)->gdipFont->GetLogFontA(getFacilities().getGraphics(), &logFont);
	HDC hdc = gdip->GetHDC();
	Gdiplus::Font f(hdc, &logFont);
	gdip->ReleaseHDC(hdc);
	{
		CRect cRect = TypeableBitmapPart::GetBoxLocation(false);
		cRect.BottomRight() -= CPoint(1, 1);
		Gdiplus::PointF p;
		p.X = (int)cRect.left + cRect.Width() / 2 - prominent_x / 2;
		p.X = (int)cRect.left + prominent_x + m_prominentAttrsNamesCX;
		p.X = (int)cRect.left + (cRect.Width() - m_LeftPortsMaxLabelLength - m_RightPortsMaxLabelLength) / 2 + m_LeftPortsMaxLabelLength + (m_prominentAttrsNamesCX - m_prominentAttrsValuesCX) / 2;
		p.Y = (int)cRect.top + (cRect.Height() - m_prominentAttrsSize.cy) / 2 + (m_bmp.get() ? (int)m_bmp->GetHeight() : 0) / 2;
		for (auto prominentIt = prominentAttrs.begin(); prominentIt != prominentAttrs.end(); ++prominentIt)
		{
			Gdiplus::SolidBrush blackBrush(Gdiplus::Color::Black);
			Gdiplus::RectF prominentBox;
			Gdiplus::StringFormat format;
			format.SetAlignment(Gdiplus::StringAlignmentFar);
			format.SetLineAlignment(Gdiplus::StringAlignmentNear);
			gdip->DrawString(static_cast<const wchar_t*>(prominentIt->name), -1, &f, p, &format, &blackBrush);

			format.SetAlignment(Gdiplus::StringAlignmentNear);
			format.SetLineAlignment(Gdiplus::StringAlignmentNear);
			gdip->DrawString(static_cast<const wchar_t*>(prominentIt->value), -1, &f, p, &format, &blackBrush);
			p.Y += 11; // FIXME depends on font
		}
	}
}


void ModelComplexPart::LoadPorts(void)
{
	CComBSTR regName("showPorts");
	CComBSTR dispPortTxt;
	CComPtr<IMgaFCO> obj = m_parentPart->GetFCO();
	obj->get_RegistryValue(regName, &dispPortTxt);
	if(dispPortTxt.Length())
	{
		if(dispPortTxt == "false")
			return;						// do not need ports for this reference: see Meta paradigm ReferTo connection showPorts attribute 
	}


	CComPtr<IMgaMetaAspect>	spParentAspect;
	COMTHROW(m_spPart->get_ParentAspect(&spParentAspect));

	CComQIPtr<IMgaModel> spModel = m_spFCO;
	CComPtr<IMgaMetaFCO> spMetaFCO;
	COMTHROW(spModel->get_Meta(&spMetaFCO));
	CComQIPtr<IMgaMetaModel> spMetaModel = spMetaFCO;

	CComBSTR bstrAspect;
	COMTHROW(m_spPart->get_KindAspect(&bstrAspect));
	if (bstrAspect.Length() == 0) {
		bstrAspect.Empty();
		COMTHROW( spParentAspect->get_Name(&bstrAspect));
	}
	
	CComPtr<IMgaMetaAspect> spAspect;
	HRESULT hr = spMetaModel->get_AspectByName(bstrAspect, &spAspect);
	
	if (hr == E_NOTFOUND) {
		try {
			// PETER: If the proper aspect cannot be found, use the first one			
			spAspect = NULL;
			CComPtr<IMgaMetaAspects> spAspects;
			COMTHROW(spMetaModel->get_Aspects(&spAspects));
			ASSERT(spAspects);
			long nAspects = 0;
			COMTHROW(spAspects->get_Count(&nAspects));
			if (nAspects > 0) {
				COMTHROW(spAspects->get_Item(1, &spAspect));
			}
		}
		catch(hresult_exception&) {
		}
	}

	if (spAspect) {
		
		CComPtr<IMgaFCOs> spFCOs;
		COMTHROW(spModel->get_ChildFCOs(&spFCOs));
		MGACOLL_ITERATE(IMgaFCO, spFCOs) {
			CComPtr<IMgaPart> spPart;
			if (MGACOLL_ITER->get_Part(spAspect, &spPart) == S_OK) {
				if (spPart) {
					CComPtr<IMgaMetaPart> spMetaPart;
					COMTHROW(spPart->get_Meta(&spMetaPart));
					VARIANT_BOOL vbLinked;
					COMTHROW(spMetaPart->get_IsLinked(&vbLinked));
					if (vbLinked) {
						long lX = 0;
						long lY = 0;
						//COMTHROW( spPart->GetGmeAttrs( 0, &lX, &lY ) );
						// zolmol, in case regnodes are not present or invalid will throw otherwise
						if(spPart->GetGmeAttrs(0, &lX, &lY) != S_OK)
							ASSERT(0);
						PortPart* portPart = new PortPart(static_cast<TypeableBitmapPart*> (this), m_eventSink, CPoint(lX, lY));
						PortPartData* partData = new PortPartData(portPart, spMetaPart, MGACOLL_ITER);
						m_AllPorts.push_back(partData);
					} else {
						COMTHROW(MGACOLL_ITER->Close());
					}
				} else {
					COMTHROW(MGACOLL_ITER->Close());
				}
			}
		} MGACOLL_ITERATE_END;

		OrderPorts();

	}
}

struct PortLess
{
	bool operator()(PortPart* pPortA, PortPart* pPortB)
	{
		CPoint posA = pPortA->GetInnerPosition();
		CPoint posB = pPortB->GetInnerPosition();

		if (posA.y != posB.y)
			return posA.y < posB.y;

		return posA.x < posB.x;
	}
};

void ModelComplexPart::OrderPorts()
{
	long lMin = 100000000;
	long lMax = 0;

	for (std::vector<PortPartData*>::iterator ii = m_AllPorts.begin(); ii != m_AllPorts.end(); ++ii) {
		lMin = min(lMin, (*ii)->portPart->GetInnerPosition().x);
		lMax = max(lMax, (*ii)->portPart->GetInnerPosition().x);
	}

	for (std::vector<PortPartData*>::iterator ii = m_AllPorts.begin(); ii != m_AllPorts.end(); ++ii) {
		PreferenceMap mapPrefs;
		mapPrefs[PREF_LABELCOLOR]		= PreferenceVariant(m_crPortText);
		mapPrefs[PREF_LABELLENGTH]		= PreferenceVariant((long) m_iMaxPortTextLength);
		mapPrefs[PREF_PORTLABELINSIDE]	= PreferenceVariant(m_bPortLabelInside);

		if ((*ii)->portPart->GetInnerPosition().x <= WIDTH_MODELSIDE ||
			(*ii)->portPart->GetInnerPosition().x < lMax / 2)
		{
			mapPrefs[PREF_LABELLOCATION] = PreferenceVariant(m_bPortLabelInside ? L_EAST : L_WEST);
			m_LeftPorts.push_back((*ii)->portPart);
		} else {
			mapPrefs[PREF_LABELLOCATION] = PreferenceVariant(m_bPortLabelInside? L_WEST: L_EAST);
			m_RightPorts.push_back((*ii)->portPart);
		}

		(*ii)->portPart->InitializeEx(m_spProject, (*ii)->spPart, (*ii)->spFCO, m_parentWnd, mapPrefs);
		long k = (*ii)->portPart->GetLongest();
		if (m_iLongestPortTextLength < k)
			m_iLongestPortTextLength = k;
	}

	std::sort(m_LeftPorts.begin(), m_LeftPorts.end(), PortLess());
	std::sort(m_RightPorts.begin(), m_RightPorts.end(), PortLess());
}


void ModelComplexPart::ReOrderConnectedOnlyPorts()
{
	long lMin = 100000000;
	long lMax = 0;

	m_LeftPorts.clear();
	m_RightPorts.clear();
	m_iLongestPortTextLength = 0;

	for (std::vector<PortPartData*>::iterator ii = m_AllPorts.begin(); ii != m_AllPorts.end(); ++ii) {
		lMin = min(lMin, (*ii)->portPart->GetInnerPosition().x);
		lMax = max(lMax, (*ii)->portPart->GetInnerPosition().x);
	}

	for (std::vector<PortPartData*>::iterator ii = m_AllPorts.begin(); ii != m_AllPorts.end(); ++ii) {
		PreferenceMap mapPrefs;
		mapPrefs[PREF_LABELCOLOR]		= PreferenceVariant(m_crPortText);
		mapPrefs[PREF_LABELLENGTH]		= PreferenceVariant((long) m_iMaxPortTextLength);
		mapPrefs[PREF_PORTLABELINSIDE]	= PreferenceVariant(m_bPortLabelInside);

		bool needThisPort = false;
		CComPtr<IMgaConnPoints> cps;
		(*ii)->spFCO->get_PartOfConns(&cps);
		long num;
		cps->get_Count(&num);
		if(num > 0)
		{
			MGACOLL_ITERATE( IMgaConnPoint , cps ) {
				CComPtr<IMgaConnPoint> cp = MGACOLL_ITER;
				CComPtr<IMgaFCOs> refs;
				COMTHROW( cp->get_References( &refs));
				long l;
				COMTHROW( refs->get_Count( &l));

				CComPtr<IMgaConnection> conn;
				COMTHROW( cp->get_Owner( &conn));
				CComPtr<IMgaParts> connParts;
				COMTHROW(conn->get_Parts(&connParts));
				CComPtr<IMgaMetaAspect>	spParentAspect;
				COMTHROW(m_spPart->get_ParentAspect(&spParentAspect));
				bool aspectMatch = false;
				MGACOLL_ITERATE( IMgaPart , connParts ) {
					CComPtr<IMgaPart> connPart = MGACOLL_ITER;
					CComPtr<IMgaMetaAspect> connAsp;
					COMTHROW(connPart->get_MetaAspect(&connAsp));
					if(connAsp == spParentAspect)
					{
						aspectMatch = true;
						break;
					}
				} MGACOLL_ITERATE_END;
				if(aspectMatch)
				{
					CComPtr<IMgaModel> container;
					COMTHROW( conn->get_ParentModel( &container));
					CComPtr<IMgaFCO> parent = m_parentPart->GetFCO();
					CComPtr<IMgaModel> grandparent;
					COMTHROW(parent->get_ParentModel(&grandparent));
	
					if(l == 0)
					{
						needThisPort = (parent == m_spFCO && container == grandparent);
					}
					else
					{
						CComPtr<IMgaFCO> ref;
						COMTHROW( refs->get_Item( 1, &ref));
						needThisPort = (ref == parent && container == grandparent);
					}
					if(needThisPort)
						break;
				}
			} MGACOLL_ITERATE_END;
		}

		if(needThisPort) 
		{
			if ((*ii)->portPart->GetInnerPosition().x <= WIDTH_MODELSIDE ||
				(*ii)->portPart->GetInnerPosition().x < lMax / 2)
			{
				mapPrefs[PREF_LABELLOCATION] = PreferenceVariant(m_bPortLabelInside ? L_EAST : L_WEST);
				m_LeftPorts.push_back((*ii)->portPart);
			} else {
				mapPrefs[PREF_LABELLOCATION] = PreferenceVariant(m_bPortLabelInside? L_WEST: L_EAST);
				m_RightPorts.push_back((*ii)->portPart);
			}
		
			long k = (*ii)->portPart->GetLongest();
			if (m_iLongestPortTextLength < k)
				m_iLongestPortTextLength = k;
		}
	}

	std::sort(m_LeftPorts.begin(), m_LeftPorts.end(), PortLess());
	std::sort(m_RightPorts.begin(), m_RightPorts.end(), PortLess());
}

void ModelComplexPart::SetBoxLocation(const CRect& cRect)
{
	TypeableBitmapPart::SetBoxLocation(cRect);
	long lY = (m_Rect.Height() - m_LeftPorts.size() * (HEIGHT_PORT + GAP_PORT) + GAP_PORT) / 2;

	for (std::vector<PortPart*>::iterator ii = m_LeftPorts.begin(); ii != m_LeftPorts.end(); ++ii) {
		CRect boxRect = CRect(GAP_XMODELPORT, lY, GAP_XMODELPORT + WIDTH_PORT, lY + HEIGHT_PORT);
		boxRect.OffsetRect(cRect.TopLeft());
		(*ii)->SetBoxLocation(boxRect);
		lY += HEIGHT_PORT + GAP_PORT;
	}
	lY = (m_Rect.Height() - m_RightPorts.size() * (HEIGHT_PORT + GAP_PORT) + GAP_PORT) / 2;
	for (std::vector<PortPart*>::iterator ii = m_RightPorts.begin(); ii != m_RightPorts.end(); ++ii) {
		CRect boxRect = CRect(cRect.Width() - GAP_XMODELPORT - WIDTH_PORT, lY, cRect.Width() - GAP_XMODELPORT, lY + HEIGHT_PORT);
		boxRect.OffsetRect(cRect.TopLeft());
		(*ii)->SetBoxLocation(boxRect);
		lY += HEIGHT_PORT + GAP_PORT;
	}
}

void ModelComplexPart::SetReferenced(bool referenced)
{
	for (std::vector<PortPart*>::iterator ii = m_LeftPorts.begin(); ii != m_LeftPorts.end(); ++ii) {
		(*ii)->SetReferenced(referenced);
	}
	for (std::vector<PortPart*>::iterator ii = m_RightPorts.begin(); ii != m_RightPorts.end(); ++ii) {
		(*ii)->SetReferenced(referenced);
	}
	TypeableBitmapPart::SetReferenced(referenced);
}

void ModelComplexPart::SetParentPart(PartBase* pPart)
{
	for (std::vector<PortPart*>::iterator ii = m_LeftPorts.begin(); ii != m_LeftPorts.end(); ++ii) {
		(*ii)->SetParentPart(pPart);
	}
	for (std::vector<PortPart*>::iterator ii = m_RightPorts.begin(); ii != m_RightPorts.end(); ++ii) {
		(*ii)->SetParentPart(pPart);
	}
	TypeableBitmapPart::SetParentPart(pPart);
}

PortPart* ModelComplexPart::GetPort(CComPtr<IMgaFCO> spFCO) const
{
	for (std::vector<PortPart*>::const_iterator ii = m_LeftPorts.begin(); ii != m_LeftPorts.end(); ++ii) {
		if (spFCO == (*ii)->GetFCO()) {
			return (*ii);
		}
	}
	for (std::vector<PortPart*>::const_iterator ii = m_RightPorts.begin(); ii != m_RightPorts.end(); ++ii) {
		if (spFCO == (*ii)->GetFCO()) {
			return (*ii);
		}
	}
	return NULL;
}

}; // namespace DecoratorSDK
