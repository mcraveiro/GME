// ConnityDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "ConnityDlg.h"


// CConnityDlg dialog

//static
int          CConnityDlg::m_lastCX     = 0;
int          CConnityDlg::m_lastCY     = 0;
int          CConnityDlg::m_smallestCX = 170;
int          CConnityDlg::m_smallestCY = 150;
int          CConnityDlg::m_leftPad    = 15;
int          CConnityDlg::m_rightPad   = 14;



IMPLEMENT_DYNAMIC(CConnityDlg, CDialog)
CConnityDlg::CConnityDlg( bool p_reverse, CWnd* pParent /*=NULL*/)
	: CDialog(CConnityDlg::IDD, pParent)
	, m_listC( 0)
	, m_selectedC( 0)
	, m_reverse( p_reverse)
{
}

CConnityDlg::~CConnityDlg()
{
}

void CConnityDlg::setList( CGuiConnectionList& p_list)
{
	m_listC = &p_list;
}

CGuiConnection*    CConnityDlg::getSelectedC()
{
	return m_selectedC;
}

void CConnityDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_options);
	DDX_Control(pDX, IDOK, m_ok);
	DDX_Control(pDX, IDCANCEL, m_ca);
}


BEGIN_MESSAGE_MAP(CConnityDlg, CDialog)
	ON_LBN_DBLCLK(IDC_LIST1, OnLbnDblclkList1)
	ON_WM_SIZING()
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CConnityDlg message handlers

BOOL CConnityDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	if( CConnityDlg::m_lastCX && CConnityDlg::m_lastCY)
	{
		CRect rect;
		GetWindowRect( &rect);
		SetWindowPos( 0, rect.left, rect.top, m_lastCX, m_lastCY, SWP_NOZORDER | SWP_NOACTIVATE);
	}

	if( m_listC && m_listC->GetCount() > 0)
	{
		POSITION pos = m_listC->GetHeadPosition();
		while( pos)
		{
			CGuiConnection* one = m_listC->GetNext( pos);
			if( !one) continue;

			CGuiObject* end = m_reverse? one->src    : one->dst;
			CGuiPort*  port = m_reverse? one->srcPort: one->dstPort;

			CString item;
			item += ( !one->name.IsEmpty()? one->name: one->kindName);
			if( end && port && port->IsRealPort())
			{
				item += " -> [ " + end->name + " : " + port->name + " ]";
			}
			else if( end)
				item += " -> " + end->name;

			m_options.AddString( item);
		}
	}

	return TRUE;
}

void CConnityDlg::OnOK()
{
	int which = m_options.GetCurSel();

	// finding the selected object in the list based on the index
	// relies on the unsorted  behaviour of the listbox
	if( m_listC && m_listC->GetCount() > 0)
	{
		int pos_i = -1;
		POSITION pos = m_listC->GetHeadPosition();
		while( pos)
		{
			CGuiConnection* one = m_listC->GetNext( pos);
			if( ++pos_i == which)
				m_selectedC = one;
		}
	}

	// save the actual Height and Width
	CRect rect;
	GetWindowRect( &rect);

	CConnityDlg::m_lastCX = rect.Width();
	CConnityDlg::m_lastCY = rect.Height();

	CDialog::OnOK();
}

void CConnityDlg::OnLbnDblclkList1()
{
	OnOK();
}


void CConnityDlg::OnSizing(UINT fwSide, LPRECT pRect)
{
	if( pRect->bottom - pRect->top > m_smallestCY
	 && pRect->right - pRect->left > m_smallestCX)
		CDialog::OnSizing(fwSide, pRect);
	else
	{
		if( pRect->bottom - pRect->top < m_smallestCY) pRect->bottom = pRect->top + m_smallestCY;
		if( pRect->right - pRect->left < m_smallestCX) pRect->right = pRect->left + m_smallestCX;
	}
}

void CConnityDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	CRect ls_rect, ok_rect, ca_rect;
	if (m_options.GetSafeHwnd() && m_ok.GetSafeHwnd() && m_ca.GetSafeHwnd()) {
		m_options.GetWindowRect( &ls_rect);
		m_ok.GetWindowRect( &ok_rect);
		m_ca.GetWindowRect( &ca_rect);

		ScreenToClient( &ls_rect);
		ScreenToClient( &ok_rect);
		ScreenToClient( &ca_rect);

		int width = cx - m_leftPad - m_rightPad;

		m_options.SetWindowPos( NULL, m_leftPad, ls_rect.top, width > 20? width: 20, cy - ls_rect.top - 2*ok_rect.Height(), SWP_NOZORDER);

		int btn_y = cy - 3* ok_rect.Height()/2;
		m_ok.SetWindowPos( 0, ok_rect.left, btn_y, ok_rect.Width(), ok_rect.Height(), SWP_NOZORDER);
		m_ca.SetWindowPos( 0, ca_rect.left, btn_y, ca_rect.Width(), ok_rect.Height(), SWP_NOZORDER);

		Invalidate();
	}
}

int CConnityDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

void CConnityDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	CRect rc;
	GetClientRect(&rc);

	rc.left = rc.right - ::GetSystemMetrics(SM_CXHSCROLL);
	rc.top = rc.bottom - ::GetSystemMetrics(SM_CYVSCROLL);

	dc.DrawFrameControl(rc, DFC_SCROLL, DFCS_SCROLLSIZEGRIP);
}
