// ArrowButton.cpp : implementation file
//

#include "stdafx.h"
#include "objectinspector.h"
#include "ArrowButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CArrowButton

CArrowButton::CArrowButton()
{
}

CArrowButton::~CArrowButton()
{
}


BEGIN_MESSAGE_MAP(CArrowButton, CButton)
	//{{AFX_MSG_MAP(CArrowButton)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CArrowButton message handlers

void CArrowButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	    
    CDC*    pDC     = CDC::FromHandle(lpDrawItemStruct->hDC);
    CRect   rect    = lpDrawItemStruct->rcItem;
    UINT    state   = lpDrawItemStruct->itemState;


    // Draw arrow
    pDC->DrawFrameControl(&rect, DFC_SCROLL, DFCS_SCROLLDOWN  | 
                          ((state & ODS_SELECTED) ? DFCS_PUSHED : 0) |
                          ((state & ODS_DISABLED) ? DFCS_INACTIVE : 0));
  
}

void CArrowButton::PreSubclassWindow() 
{
	ModifyStyle(0, BS_OWNERDRAW);        // Make it owner drawn
    	
	CButton::PreSubclassWindow();
}
