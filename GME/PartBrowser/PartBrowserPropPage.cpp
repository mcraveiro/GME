// PartBrowserPropPage.cpp : Implementation of the CPartBrowserPropPage property page class.

#include "stdafx.h"
#include "PartBrowser.h"
#include "PartBrowserPropPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CPartBrowserPropPage, COlePropertyPage)



// Message map

BEGIN_MESSAGE_MAP(CPartBrowserPropPage, COlePropertyPage)
END_MESSAGE_MAP()



// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CPartBrowserPropPage, "PARTBROWSER.PartBrowserPropPage.1",
	0x8e82d238, 0xd222, 0x493d, 0x8e, 0xd2, 0x31, 0x75, 0xa5, 0xb1, 0x8, 0x6c)



// CPartBrowserPropPage::CPartBrowserPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CPartBrowserPropPage

BOOL CPartBrowserPropPage::CPartBrowserPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_PARTBROWSER_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}



// CPartBrowserPropPage::CPartBrowserPropPage - Constructor

CPartBrowserPropPage::CPartBrowserPropPage() :
	COlePropertyPage(IDD, IDS_PARTBROWSER_PPG_CAPTION)
{
}



// CPartBrowserPropPage::DoDataExchange - Moves data between page and properties

void CPartBrowserPropPage::DoDataExchange(CDataExchange* pDX)
{
	DDP_PostProcessing(pDX);
}



// CPartBrowserPropPage message handlers
