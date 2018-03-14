// GMEConsole.cpp : implementation file
//

#include "stdafx.h"
#include "GMEApp.h"
#include "GMEstd.h"
#include "mainfrm.h"
#include "GMEDoc.h"
#include "GMEView.h"
#include "GMEConsole.h"
#include "guimeta.h"

CGMEConsole* CGMEConsole::theInstance = 0;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGMEConsole dialog


CGMEConsole::CGMEConsole(CWnd* /*pParent =NULL*/)
{
	//{{AFX_DATA_INIT(CGMEConsole)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	VERIFY(theInstance == 0);
	theInstance = this;
}


void CGMEConsole::DoDataExchange(CDataExchange* pDX)
{
	CDockablePane::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGMEConsole)
	DDX_Control(pDX, IDC_CONSOLE_CTRL, m_Console);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGMEConsole, CDockablePane)
	//{{AFX_MSG_MAP(CGMEConsole)
	ON_WM_CREATE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGMEConsole message handlers

int CGMEConsole::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	
	if(!m_Console.Create(_T("Console"),WS_CHILD | WS_BORDER | WS_VISIBLE,CRect(0,0,230,300),this,IDC_CONSOLE_CTRL)) {
		ASSERT(FALSE);
		return -1;
	}

	/*
    m_szFloat = CSize(500,165); 
	m_szHorz = CSize(100,165);
    m_szVert = CSize(120,400);
*/	
/*@@@	m_szFloat = CSize(400,600); 
	m_szHorz = CSize(800,80);
    m_szVert = CSize(120,400);
*/	
	return 0;
}

void CGMEConsole::OnSize(UINT nType, int cx, int cy) 
{
	CDockablePane::OnSize(nType, cx, cy);
	
	CRect rc;
	GetClientRect(rc);	
	m_Console.MoveWindow(rc);
}

BEGIN_EVENTSINK_MAP(CGMEConsole, CDockablePane)
    //{{AFX_EVENTSINK_MAP(CGMEConsole)
	ON_EVENT(CGMEConsole, IDC_CONSOLE_CTRL, 1 /* ClickMGAID */, OnClickMGAIDConsoleCtrl, VTS_BSTR)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CGMEConsole::OnClickMGAIDConsoleCtrl(LPCTSTR objid) 
{
	if (CGMEDoc::theInstance) {
		CGMEDoc::theInstance->ShowObject(objid);
	}	
}

void CGMEConsole::Clear()
{
	m_Console.Clear();
}

void CGMEConsole::Message(CString str, short type)
{
	// ShowPane(TRUE, FALSE, TRUE);
	m_Console.Message(str, type);
}

CString CGMEConsole::GetContents()
{
	return m_Console.GetContents();
}

void CGMEConsole::SetContents(const CString& contents)
{
	m_Console.SetContents(contents);
}

void CGMEConsole::SetGMEApp(IDispatch *idp)
{
	m_Console.SetGMEApp(idp);
}

void CGMEConsole::SetGMEProj(IDispatch *idp)
{
	m_Console.SetGMEProj(idp);
}

void CGMEConsole::NavigateTo(CString url)
{
	m_Console.NavigateTo(url);
}