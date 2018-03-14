// AspectPage.cpp : implementation file
//

#include "stdafx.h"
#include "aspectspectbl.h"
#include "aspectpage.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAspectPage property page

//IMPLEMENT_DYNCREATE(CAspectPage, CPropertyPage)

int CAspectPage::m_actHeight = 0;//static 
int CAspectPage::m_actWidth = 0; //static

CAspectPage::CAspectPage() 
	: CPropertyPage(IDD_ASPECT_PAGE)
	, m_deflateVal(10)
{
	//{{AFX_DATA_INIT(CAspectPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


CAspectPage::~CAspectPage()
{
	POSITION pos = entries.GetHeadPosition();
	while (pos) {
		delete entries.GetNext(pos);
	}
	entries.RemoveAll();

	m_actWidth = m_actHeight = 0;// reset the values
}

void CAspectPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAspectPage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAspectPage, CPropertyPage)
	ON_WM_SIZE()
	//{{AFX_MSG_MAP(CAspectPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAspectPage message handlers

CString CAspectPage::GetAspectName() {
	return aspectName;
}

void CAspectPage::SetAspectName(CString name) {
	aspectName = name;
}

BOOL CAspectPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	// TODO: Add extra initialization here

	CRect loc;
	calcPlace( loc);

	aspectTable.Create(WS_CHILD|WS_VISIBLE|WS_BORDER|LVS_REPORT|LVS_SINGLESEL|LVS_NOSORTHEADER, loc, this, 1);
	POSITION pos = entries.GetHeadPosition();
	while (pos) {
		entry	*e = entries.GetNext(pos);
		aspectTable.AddRow(e->rowID, e->roleName, e->kindAspect, e->isPrimary);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAspectPage::AddEntry(int rowID, CString roleName, CString kindAspect, CString isPrimary, const void * ptr)
{
	entry* newent = new entry;;

	newent->rowID = rowID;

	newent->roleName = roleName;
	newent->kindAspect = kindAspect;
	newent->isPrimary = isPrimary;
	newent->ptr = ptr;

	entries.AddTail(newent);
}


bool CAspectPage::GetEntry(int rowID, CString &roleName, CString& kindAspect, CString& isPrimary, const void * &ptr)
{
	bool retval = false;

	POSITION pos = entries.GetHeadPosition();
	while (pos) {
		entry	*e = entries.GetNext(pos);
		if (e->rowID == rowID) {
			roleName = e->roleName;
			kindAspect = e->kindAspect;
			isPrimary = e->isPrimary;
			ptr = e->ptr;
			retval = true;
		}
	}
	return retval;
}

void CAspectPage::OnOK() 
{
	// TODO: Add your specialized code here and/or call the base class
	POSITION pos = entries.GetHeadPosition();
	while (pos) {
		entry	*e = entries.GetNext(pos);
		aspectTable.GetRow(e->rowID, e->roleName, e->kindAspect, e->isPrimary);
	}
	
	CPropertyPage::OnOK();
}

void CAspectPage::OnSize(UINT ntype, int cx, int cy)
{
	// this methods captures the size of the remaining area
	// after the stacking tabs have been drawn, so that 
	// the initial page needs to be resized accordingly
	//
	// this method is called several times, first is called
	// with larger cy values (close to the total size of the window)
	// but after the tabs were drawn the it is called with a smaller
	// cy value

	CPropertyPage::OnSize(ntype, cx, cy);

	m_actHeight = cy;
	m_actWidth = cx;
}

void CAspectPage::calcPlace( CRect &loc)
{
	//this->GetClientRect(&loc);
	loc.left = loc.top = 0;
	loc.bottom = m_actHeight;
	loc.right = m_actWidth;

	loc.DeflateRect( m_deflateVal, m_deflateVal);
}

void CAspectPage::resizeTableToFitIn()
{
	CRect loc;
	calcPlace( loc);
	aspectTable.SetWindowPos( 0, loc.left, loc.top, // ignored values
		loc.right - loc.left, loc.bottom - loc.top,
		SWP_SHOWWINDOW|SWP_NOZORDER|SWP_NOMOVE);
}

