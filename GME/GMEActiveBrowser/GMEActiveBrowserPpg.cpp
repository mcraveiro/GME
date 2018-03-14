// GMEActiveBrowserPpg.cpp : Implementation of the CGMEActiveBrowserPropPage property page class.

#include "stdafx.h"
#include "GMEActiveBrowser.h"
#include "GMEActiveBrowserPpg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CGMEActiveBrowserPropPage, COlePropertyPage)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CGMEActiveBrowserPropPage, COlePropertyPage)
	//{{AFX_MSG_MAP(CGMEActiveBrowserPropPage)
	// NOTE - ClassWizard will add and remove message map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CGMEActiveBrowserPropPage, "GME.GMEActiveBrowserPropPage.1",
	0x4b4c56da, 0xa2a0, 0x4ae3, 0xbc, 0x4b, 0xb0, 0x71, 0x51, 0xde, 0x73, 0x2c)


/////////////////////////////////////////////////////////////////////////////
// CGMEActiveBrowserPropPage::CGMEActiveBrowserPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CGMEActiveBrowserPropPage

BOOL CGMEActiveBrowserPropPage::CGMEActiveBrowserPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_GMEACTIVEBROWSER_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}


/////////////////////////////////////////////////////////////////////////////
// CGMEActiveBrowserPropPage::CGMEActiveBrowserPropPage - Constructor

CGMEActiveBrowserPropPage::CGMEActiveBrowserPropPage() :
	COlePropertyPage(IDD, IDS_GMEACTIVEBROWSER_PPG_CAPTION)
{
	//{{AFX_DATA_INIT(CGMEActiveBrowserPropPage)
	// NOTE: ClassWizard will add member initialization here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_INIT
}


/////////////////////////////////////////////////////////////////////////////
// CGMEActiveBrowserPropPage::DoDataExchange - Moves data between page and properties

void CGMEActiveBrowserPropPage::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CGMEActiveBrowserPropPage)
	// NOTE: ClassWizard will add DDP, DDX, and DDV calls here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_MAP
	DDP_PostProcessing(pDX);
}


/////////////////////////////////////////////////////////////////////////////
// CGMEActiveBrowserPropPage message handlers
