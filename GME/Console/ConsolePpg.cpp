// ConsolePpg.cpp : Implementation of the CConsolePropPage property page class.

#include "stdafx.h"
#include "Console.h"
#include "ConsolePpg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CConsolePropPage, COlePropertyPage)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CConsolePropPage, COlePropertyPage)
	//{{AFX_MSG_MAP(CConsolePropPage)
	// NOTE - ClassWizard will add and remove message map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CConsolePropPage, "GME.ConsolePropPage.1",
	0xa9b2b82a, 0xb753, 0x4fb0, 0x8f, 0xb, 0x36, 0x61, 0xbc, 0x30, 0x70, 0x53)


/////////////////////////////////////////////////////////////////////////////
// CConsolePropPage::CConsolePropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CConsolePropPage

BOOL CConsolePropPage::CConsolePropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_CONSOLE_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}


/////////////////////////////////////////////////////////////////////////////
// CConsolePropPage::CConsolePropPage - Constructor

CConsolePropPage::CConsolePropPage() :
	COlePropertyPage(IDD, IDS_CONSOLE_PPG_CAPTION)
{
	//{{AFX_DATA_INIT(CConsolePropPage)
	// NOTE: ClassWizard will add member initialization here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_INIT
}


/////////////////////////////////////////////////////////////////////////////
// CConsolePropPage::DoDataExchange - Moves data between page and properties

void CConsolePropPage::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CConsolePropPage)
	// NOTE: ClassWizard will add DDP, DDX, and DDV calls here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_MAP
	DDP_PostProcessing(pDX);
}


/////////////////////////////////////////////////////////////////////////////
// CConsolePropPage message handlers
