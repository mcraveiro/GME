// ObjectInspectorPpg.cpp : Implementation of the CObjectInspectorPropPage property page class.

#include "stdafx.h"
#include "ObjectInspector.h"
#include "ObjectInspectorPpg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CObjectInspectorPropPage, COlePropertyPage)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CObjectInspectorPropPage, COlePropertyPage)
	//{{AFX_MSG_MAP(CObjectInspectorPropPage)
	// NOTE - ClassWizard will add and remove message map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CObjectInspectorPropPage, "GME.ObjectInspectorPropPage.1",
	0xb4b5a75, 0xbf21, 0x4c8b, 0xa7, 0x5, 0xe2, 0x66, 0x6e, 0x1d, 0x9b, 0x15)


/////////////////////////////////////////////////////////////////////////////
// CObjectInspectorPropPage::CObjectInspectorPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CObjectInspectorPropPage

BOOL CObjectInspectorPropPage::CObjectInspectorPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_OBJECTINSPECTOR_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}


/////////////////////////////////////////////////////////////////////////////
// CObjectInspectorPropPage::CObjectInspectorPropPage - Constructor

CObjectInspectorPropPage::CObjectInspectorPropPage() :
	COlePropertyPage(IDD, IDS_OBJECTINSPECTOR_PPG_CAPTION)
{
	//{{AFX_DATA_INIT(CObjectInspectorPropPage)
	// NOTE: ClassWizard will add member initialization here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_INIT
}


/////////////////////////////////////////////////////////////////////////////
// CObjectInspectorPropPage::DoDataExchange - Moves data between page and properties

void CObjectInspectorPropPage::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CObjectInspectorPropPage)
	// NOTE: ClassWizard will add DDP, DDX, and DDV calls here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_MAP
	DDP_PostProcessing(pDX);
}


/////////////////////////////////////////////////////////////////////////////
// CObjectInspectorPropPage message handlers
