// PanningViewDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PanningViewDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPanningViewDlg dialog

CPanningViewDlg::CPanningViewDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPanningViewDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPanningViewDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CPanningViewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPanningViewDlg)
	DDX_Control(pDX, IDC_PREVIEW, m_pvbutton);
	//}}AFX_DATA_MAP
}

BOOL CPanningViewDlg::OnInitDialog()
{
	BOOL ret = CDialog::OnInitDialog();
	// TODO: Error checking
	m_pvbutton.Create();
	return ret;
}

void CPanningViewDlg::SetBitmapDC(HWND owner, HDC bdc, HBITMAP oldBmp, CRect& ori, CRect& rect, COLORREF& bkgrnd)
{
	m_pvbutton.SetBitmapDC(owner, bdc, oldBmp, ori, rect, bkgrnd);
}

void CPanningViewDlg::SetViewRect(CRect& vrect)
{
	m_pvbutton.SetViewRect(vrect);
}


BEGIN_MESSAGE_MAP(CPanningViewDlg, CDialog)
	//{{AFX_MSG_MAP(CPanningViewDlg)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPanningViewDlg message handlers

void CPanningViewDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	
	CRect rc;
	GetClientRect(rc);

	if (m_pvbutton.m_hWnd)
		m_pvbutton.MoveWindow(rc);
}


