// PanningViewPropPage.cpp : Implementation of the CPanningViewPropPage property page class.

#include "stdafx.h"
#include "PanningView.h"
#include "PanningViewPropPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CPanningViewPropPage, COlePropertyPage)



// Message map

BEGIN_MESSAGE_MAP(CPanningViewPropPage, COlePropertyPage)
END_MESSAGE_MAP()



// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CPanningViewPropPage, "PANNINGVIEW.PanningViewPropPage.1",
	0x458d3476, 0xb3ff, 0x4545, 0x96, 0x45, 0xc2, 0x47, 0xa7, 0xc1, 0x7e, 0x36)



// CPanningViewPropPage::CPanningViewPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CPanningViewPropPage

BOOL CPanningViewPropPage::CPanningViewPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_PANNINGVIEW_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}



// CPanningViewPropPage::CPanningViewPropPage - Constructor

CPanningViewPropPage::CPanningViewPropPage() :
	COlePropertyPage(IDD, IDS_PANNINGVIEW_PPG_CAPTION)
{
}



// CPanningViewPropPage::DoDataExchange - Moves data between page and properties

void CPanningViewPropPage::DoDataExchange(CDataExchange* pDX)
{
	DDP_PostProcessing(pDX);
}



// CPanningViewPropPage message handlers
