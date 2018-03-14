// AspectSpecDlg.cpp : implementation file
//

#include "stdafx.h"
#include "aspectspectbl.h"
#include "AspectSpecDlg.h"
#include "logger.h"
#include "ModelRep.h"
#include "ReferenceRep.h"

#include "globals.h"
extern Globals global_vars;

// keep in sync with Dumper.cpp's DUMPER_NOT_SPECIFIED_STR
#define DUMPER_NOT_SPECIFIED_STR      "Not specified"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAspectSpecDlg

CAspectSpecDlg::CAspectSpecDlg(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	: CPropertySheet(pszCaption, pParentWnd, iSelectPage)
	, lastID(0)
	, countPages(0)
	, m_bNeedInit(TRUE)
	, m_nMinCX(0)
	, m_nMinCY(0)
{
	m_psh.dwFlags |= PSH_NOAPPLYNOW;
	m_rCrt.SetRectEmpty();
}

CAspectSpecDlg::~CAspectSpecDlg()
{
	POSITION pos = aspectPages.GetHeadPosition();
	while (pos) 
		delete aspectPages.GetNext(pos);

	aspectPages.RemoveAll();
}

// See MS KB Q143291
// "How to resize CPropertyPages at run time in Visual C++"
#define WM_TABPAGESELECTIONCHANGE WM_USER + 111

BEGIN_MESSAGE_MAP(CAspectSpecDlg, CPropertySheet)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_MESSAGE(WM_TABPAGESELECTIONCHANGE, OnTabPageSelectionChange)
	//{{AFX_MSG_MAP(CAspectSpecDlg)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAspectSpecDlg message handlers



int CAspectSpecDlg::AddEntry(CString aspectName, CString roleName, CString kindAspect, CString primaryAsp, const void * ptr)
{
	CAspectPage *aspectPage = NULL;

	POSITION pos = aspectPages.GetHeadPosition();
	while (pos) {
		CAspectPage *page = aspectPages.GetNext(pos);
		CString title = page->GetAspectName();;

		if (title == aspectName)
			aspectPage = page;
	}
	
	if (!aspectPage) {
		aspectPage = new CAspectPage();
		aspectPage->SetAspectName(aspectName);
		aspectNames.AddTail(aspectName);
		AddPage(aspectPage);
		aspectPages.AddTail(aspectPage);
		++countPages;
	}

	aspectPage->AddEntry(lastID, roleName, kindAspect, primaryAsp, ptr);
	return lastID++;
}

void CAspectSpecDlg::GetEntry(int rowID, CString &roleName, CString &kindAspect, CString &isPrimary, const void * &ptr)
{
	POSITION pos = aspectPages.GetHeadPosition();
	while (pos) {
		if (aspectPages.GetNext(pos)->GetEntry(rowID, roleName, kindAspect, isPrimary, ptr))
			break;
	}

}

void CAspectSpecDlg::GetAspects(CStringList &aspects)
{
	aspects.RemoveAll();
	aspects.AddTail(&aspectNames);
}

void CAspectSpecDlg::GetAspects( int rowID, CStringList &aspects)
{
	aspects.RemoveAll();
	CString role_name = "N/A", kind_asp = "N/A", is_prim;
	const void * ptr;
	GetEntry( rowID, role_name, kind_asp, is_prim, ptr);
	if ( role_name == "N/A") 
		global_vars.err << MSG_ERROR << "Internal error: GetEntry failed. Empty rolename\n";
	const FCO * fco_ptr = static_cast<const FCO*>( ptr);
	if ( fco_ptr)
	{
		if ( fco_ptr->getMyKind() == Any::MODEL)
		{
			const ModelRep * m_ptr = static_cast<const ModelRep *>(ptr);
			m_ptr->getAspectNames( aspects);
		}
		else if (fco_ptr->getMyKind() == Any::REF)
		{
			aspects.AddTail( DUMPER_NOT_SPECIFIED_STR);
			const ReferenceRep * r_ptr = static_cast< const ReferenceRep *>(ptr);
			std::vector< AspectRep *> asps = r_ptr->getAspectsIntersection();
			std::vector< AspectRep *>::iterator asp_it = asps.begin();
			for( ; asp_it != asps.end(); ++asp_it)
			{
				aspects.AddTail( (*asp_it)->getName().c_str());
			}
		}
		else global_vars.err << MSG_ERROR << "Internal error: why other type than Model and Ref has Kindaspect?\n";
	}
	else global_vars.err << MSG_ERROR << "Internal error: zero pointer\n";
}

BOOL CAspectSpecDlg::OnInitDialog() 
{
	// this will call the OnInitDialog of the first PropertyPage
	BOOL bResult = CPropertySheet::OnInitDialog();

	// [ Workaround for resizable property sheets
	// http://nibuthomas.wordpress.com/2008/03/27/making-a-property-sheet-window-resizable/
	// the reason why it can’t be resized is because SC_SIZE menu is missing from the property sheet system menu
	// (Press Alt and Spacebar)!
	CMenu* pMenu = GetSystemMenu(FALSE);
	ASSERT(pMenu);
	// Call once more with revert flag set to true to restore original system menu
	GetSystemMenu( TRUE );	// This reverts to original system menu 
	// Also don't forget to change the style to resizable
	ModifyStyle(WS_DLGFRAME, WS_OVERLAPPEDWINDOW, 0);
	// Workaround for resizable property sheets ]

	// this portion may modify the layout of tabs (the number of rows they occupy)
	// since the tabnames may be longer then expected
	// so the first page needs to be resized after this 
	CTabCtrl* tab = GetTabControl();
	int count = tab->GetItemCount();
	POSITION pos = aspectNames.GetHeadPosition();
	for (int i = 0; i < count; i++) {
		TC_ITEM tcItem;
		tcItem.mask = TCIF_TEXT;
		tcItem.pszText = aspectNames.GetNext(pos).GetBuffer(255);
		tab->SetItem(i, &tcItem);
	}

	SetActivePage(0); // activates the first page, and calls AspectPage::OnSize with the updated size (lower than the previous)

	// resizing of controls on the first page
	CAspectPage* firstpage;
	POSITION pos1 = aspectPages.GetHeadPosition();
	if (pos1) {
		firstpage = aspectPages.GetNext(pos1);
		firstpage->resizeTableToFitIn();
	}

	// Init m_nMinCX/Y
	CRect r;
	GetWindowRect(&r);
	m_nMinCX = r.Width();
	m_nMinCY = r.Height();
	// After this point we allow resize code to kick in
	m_bNeedInit = FALSE;
	GetClientRect(&m_rCrt);

	return bResult;
}

BOOL CAspectSpecDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	NMHDR* pnmh = (LPNMHDR) lParam;

	BOOL retVal = CPropertySheet::OnNotify(wParam, lParam, pResult);
	// the sheet resizes the page whenever it is activated
	// so we need to resize it to what we want
	if (TCN_SELCHANGE == pnmh->code) {
		// user-defined message needs to be posted because page must
		// be resized after TCN_SELCHANGE has been processed
		PostMessage(WM_TABPAGESELECTIONCHANGE);
	}

	return retVal;
}

int CAspectSpecDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	// Set for Scrolling Tabs style if the #of pages exceeds 19
	// otherwise the tabs with long aspect names could occupy the whole window area
	// if 19 aspects with really long names existed still fitted into the window
	// and space left for 3 items
	// that is why we enable the ScrollingTabs in case the number of pages > 19
	// unfortunately the length of the names is not calculated
	EnableStackedTabs(countPages <= 19);

	// Call the base class
	return CPropertySheet::OnCreate(lpCreateStruct);
}

// See http://support.microsoft.com/kb/300606 and other related articles on the net
// "How to implement a resizable property sheet class that contains a menu bar in Visual C++ 6.0"
void CAspectSpecDlg::OnSize(UINT nType, int cx, int cy)
{
	CPropertySheet::OnSize(nType, cx, cy);

	CRect r1;

	if (m_bNeedInit)
		return;

	CTabCtrl *pTab = GetTabControl();
	ASSERT(NULL != pTab && IsWindow(pTab->m_hWnd));

	int dx = cx - m_rCrt.Width();
	int dy = cy - m_rCrt.Height();
	GetClientRect(&m_rCrt);

	HDWP hDWP = ::BeginDeferWindowPos(5);

	pTab->GetClientRect(&r1); 
	r1.right += dx;
	r1.bottom += dy;
	::DeferWindowPos(hDWP, pTab->m_hWnd, NULL,
					 0, 0, r1.Width(), r1.Height(),
					 SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);

	// Move all buttons with the lower right sides
	for(CWnd *pChild = GetWindow(GW_CHILD);
		pChild != NULL;
		pChild = pChild->GetWindow(GW_HWNDNEXT))
	{
		if (pChild->SendMessage(WM_GETDLGCODE) & DLGC_BUTTON)
		{
			pChild->GetWindowRect(&r1); ScreenToClient(&r1); 
			r1.top += dy;
			r1.bottom += dy;
			r1.left+= dx;
			r1.right += dx;
			::DeferWindowPos(hDWP, pChild->m_hWnd, NULL,
							 r1.left, r1.top, 0, 0,
							 SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOZORDER);
		}
		// Resize everything else...
		else
		{
			pChild->GetClientRect(&r1);
			r1.right += dx;
			r1.bottom += dy;
			::DeferWindowPos(hDWP, pChild->m_hWnd, NULL, 0, 0, r1.Width(), r1.Height(),
							 SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
		}

	}

	::EndDeferWindowPos(hDWP);

	// resizing of controls on the first page
	CAspectPage* firstpage;
	POSITION pos1 = aspectPages.GetHeadPosition();
	if (pos1) {
		firstpage = aspectPages.GetNext(pos1);
		firstpage->resizeTableToFitIn();
	}
}

void CAspectSpecDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	CPropertySheet::OnGetMinMaxInfo(lpMMI);
	lpMMI->ptMinTrackSize.x = m_nMinCX;
	lpMMI->ptMinTrackSize.y = m_nMinCY;
}

LRESULT CAspectSpecDlg::OnTabPageSelectionChange(WPARAM wParam, LPARAM lParam)
{
	// user-defined message needs to be posted because page must
	// be resized after TCN_SELCHANGE has been processed
	CAspectPage* page = dynamic_cast<CAspectPage*> (GetActivePage());
	ASSERT(page);
	page->resizeTableToFitIn();

	return 0;
}
