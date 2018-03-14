// GMEViewPropPage.cpp : Implementation of the CGMEViewPropPage property page class.

#include "stdafx.h"
#include "GMEView.h"
#include "GMEViewPropPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CGMEViewPropPage, COlePropertyPage)



// Message map

BEGIN_MESSAGE_MAP(CGMEViewPropPage, COlePropertyPage)
END_MESSAGE_MAP()



// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CGMEViewPropPage, "GMEVIEW.GMEViewPropPage.1",
	0x72b7b135, 0x99f3, 0x41e9, 0x8f, 0x3c, 0xe6, 0x5f, 0xd6, 0x15, 0xd0, 0x20)



// CGMEViewPropPage::CGMEViewPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CGMEViewPropPage

BOOL CGMEViewPropPage::CGMEViewPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_GMEVIEW_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}



// CGMEViewPropPage::CGMEViewPropPage - Constructor

CGMEViewPropPage::CGMEViewPropPage() :
	COlePropertyPage(IDD, IDS_GMEVIEW_PPG_CAPTION)
{
}



// CGMEViewPropPage::DoDataExchange - Moves data between page and properties

void CGMEViewPropPage::DoDataExchange(CDataExchange* pDX)
{
	DDP_PostProcessing(pDX);
}



// CGMEViewPropPage message handlers
