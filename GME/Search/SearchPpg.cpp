// SearchPpg.cpp : Implementation of the CSearchPropPage property page class.

#include "stdafx.h"
#include "Search.h"
#include "SearchPpg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CSearchPropPage, COlePropertyPage)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CSearchPropPage, COlePropertyPage)
	//{{AFX_MSG_MAP(CSearchPropPage)
	// NOTE - ClassWizard will add and remove message map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CSearchPropPage, "GME.SearchPropPage.1",
	0xf299943a, 0x2397, 0x45b1, 0x8c, 0x77, 0xa9, 0xd2, 0x9f, 0x2f, 0xe5, 0x8e)


/////////////////////////////////////////////////////////////////////////////
// CSearchPropPage::CSearchPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CSearchPropPage

BOOL CSearchPropPage::CSearchPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_SEARCH_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}


/////////////////////////////////////////////////////////////////////////////
// CSearchPropPage::CSearchPropPage - Constructor

CSearchPropPage::CSearchPropPage() :
	COlePropertyPage(IDD, IDS_SEARCH_PPG_CAPTION)
{
	//{{AFX_DATA_INIT(CSearchPropPage)
	// NOTE: ClassWizard will add member initialization here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_INIT
}


/////////////////////////////////////////////////////////////////////////////
// CSearchPropPage::DoDataExchange - Moves data between page and properties

void CSearchPropPage::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CSearchPropPage)
	// NOTE: ClassWizard will add DDP, DDX, and DDV calls here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_MAP
	DDP_PostProcessing(pDX);
}


/////////////////////////////////////////////////////////////////////////////
// CSearchPropPage message handlers
