// CompassCheckDlg.cpp : implementation file
//

#include "stdafx.h"
#include "objectinspector.h"
#include "CompassCheckDlg.h"
#include "CompassData.h"
#include "InPlaceCommon.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCompassCheckDlg dialog


CCompassCheckDlg::CCompassCheckDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCompassCheckDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCompassCheckDlg)
	m_bEast = FALSE;
	m_bNorth = FALSE;
	m_bSouth = FALSE;
	m_bWest = FALSE;
	//}}AFX_DATA_INIT

    m_bInited = true;
	m_bClosed = false;
}


void CCompassCheckDlg::SetParameters(const CPoint& ptRightTop, UINT uCompassVal)
{
	m_ptRightTop		= ptRightTop;
	m_uCompassVal		= uCompassVal;
}


UINT CCompassCheckDlg::GetCompassVal(void) const
{
	return m_uCompassVal;
}


void CCompassCheckDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCompassCheckDlg)
	DDX_Check(pDX, IDC_EAST, m_bEast);
	DDX_Check(pDX, IDC_NORTH, m_bNorth);
	DDX_Check(pDX, IDC_SOUTH, m_bSouth);
	DDX_Check(pDX, IDC_WEST, m_bWest);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCompassCheckDlg, CDialog)
	//{{AFX_MSG_MAP(CCompassCheckDlg)
	ON_WM_NCACTIVATE()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCompassCheckDlg message handlers



BOOL CCompassCheckDlg::OnNcActivate(BOOL bActive) 
{
	if(m_bInited)
	{
		m_bInited=false;
	}
	else
	{
		if (!bActive && !m_bClosed) {
			m_bClosed = true;
			OnOK();
		}
		// OnOK();
	}
	return FALSE;//CDialog::OnNcActivate(bActive);
}

BOOL CCompassCheckDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CRect rectWnd;
	GetWindowRect(rectWnd);
	CPoint ptLeftTop(m_ptRightTop.x-rectWnd.Width(),m_ptRightTop.y);
	SetWindowPos(NULL,ptLeftTop.x,ptLeftTop.y,0,0,SWP_NOSIZE|SWP_NOZORDER);

	m_bNorth=m_bEast=m_bSouth=m_bWest=FALSE;

	if(m_uCompassVal&CMPS_NORTH)
	{
		m_bNorth=TRUE;
	}

	if(m_uCompassVal&CMPS_EAST)
	{
		m_bEast=TRUE;
	}
	
	if(m_uCompassVal&CMPS_SOUTH)
	{
		m_bSouth=TRUE;
	}

	if(m_uCompassVal&CMPS_WEST)
	{
		m_bWest=TRUE;
	}
	
	UpdateData(FALSE);

    m_background.LoadBitmap( IDB_COMPASS_CHECK );
	
	m_bInited=true;
	return TRUE;	              
}


void CCompassCheckDlg::OnOK()
{

	UpdateData(TRUE);

	m_uCompassVal=0;

	if(m_bNorth)
	{
		m_uCompassVal|=CMPS_NORTH;

	}

	if(m_bSouth)
	{
		m_uCompassVal|=CMPS_SOUTH;

	}

	if(m_bEast)
	{
		m_uCompassVal|=CMPS_EAST;

	}

	if(m_bWest)
	{
		m_uCompassVal|=CMPS_WEST;

	}

	m_bClosed = true;
	CDialog::OnOK();
}

void CCompassCheckDlg::OnCancel()
{
	CDialog::OnCancel();
}


void CCompassCheckDlg::OnPaint() 
{
	CPaintDC dc(this);

    CDC         memdc;
    CBitmap   * oldbmp;
    CRect       rect;

    GetClientRect( &rect );

    memdc.CreateCompatibleDC( &dc );
    oldbmp = memdc.SelectObject( &m_background );
    dc.StretchBlt   ( 0, 0, rect.right, rect.bottom
                    , &memdc
                    , 0, 0
                    , 92, 92 //m_background.GetBitmapDimension().cx
                    //, m_background.GetBitmapDimension().cy
                    , SRCCOPY 
                    );

    memdc.SelectObject( oldbmp );
}
