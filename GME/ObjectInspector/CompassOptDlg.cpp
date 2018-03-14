// CompassOptDlg.cpp : implementation file
//

#include "stdafx.h"
#include "objectinspector.h"
#include "CompassOptDlg.h"
#include "CompassData.h"
#include "InPlaceCommon.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCompassOptDlg dialog


CCompassOptDlg::CCompassOptDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCompassOptDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCompassOptDlg)
	m_nRadio = -1;
	//}}AFX_DATA_INIT

	m_bInited = true;
	m_bClosed = false;
}


void CCompassOptDlg::SetParameters(const CPoint& ptRightTop, UINT uCompassVal)
{
	m_ptRightTop		= ptRightTop;
	m_uCompassVal		= uCompassVal;
}


UINT CCompassOptDlg::GetCompassVal(void) const
{
	return m_uCompassVal;
}


void CCompassOptDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCompassOptDlg)
	DDX_Radio(pDX, IDC_RADIO_CENTER, m_nRadio);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCompassOptDlg, CDialog)
	//{{AFX_MSG_MAP(CCompassOptDlg)
	ON_WM_NCACTIVATE()
	ON_BN_CLICKED(IDC_RADIO_CENTER, OnRadioCenter)
	ON_BN_DOUBLECLICKED(IDC_RADIO_CENTER, OnDoubleclickedRadioCenter)
	ON_BN_CLICKED(IDC_RADIO_EAST, OnRadioEast)
	ON_BN_DOUBLECLICKED(IDC_RADIO_EAST, OnDoubleclickedRadioEast)
	ON_BN_CLICKED(IDC_RADIO_NORTH, OnRadioNorth)
	ON_BN_DOUBLECLICKED(IDC_RADIO_NORTH, OnDoubleclickedRadioNorth)
	ON_BN_CLICKED(IDC_RADIO_NORTHEAST, OnRadioNortheast)
	ON_BN_DOUBLECLICKED(IDC_RADIO_NORTHEAST, OnDoubleclickedRadioNortheast)
	ON_BN_CLICKED(IDC_RADIO_NORTHWEST, OnRadioNorthwest)
	ON_BN_DOUBLECLICKED(IDC_RADIO_NORTHWEST, OnDoubleclickedRadioNorthwest)
	ON_BN_CLICKED(IDC_RADIO_SOUTH, OnRadioSouth)
	ON_BN_DOUBLECLICKED(IDC_RADIO_SOUTH, OnDoubleclickedRadioSouth)
	ON_BN_CLICKED(IDC_RADIO_SOUTHEAST, OnRadioSoutheast)
	ON_BN_DOUBLECLICKED(IDC_RADIO_SOUTHEAST, OnDoubleclickedRadioSoutheast)
	ON_BN_CLICKED(IDC_RADIO_SOUTHWEST, OnRadioSouthwest)
	ON_BN_DOUBLECLICKED(IDC_RADIO_SOUTHWEST, OnDoubleclickedRadioSouthwest)
	ON_BN_CLICKED(IDC_RADIO_WEST, OnRadioWest)
	ON_BN_DOUBLECLICKED(IDC_RADIO_WEST, OnDoubleclickedRadioWest)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCompassOptDlg message handlers


BOOL CCompassOptDlg::OnNcActivate(BOOL bActive) 
{
	if(m_bInited)
	{
		m_bInited=false;
	}
	else
	{	
		if (!bActive && !m_bClosed) {
			m_bClosed = true;
			EndDialog(IDCANCEL);
			DWORD pos = GetMessagePos();
			CPoint msgPoint(GET_X_LPARAM(pos), GET_Y_LPARAM(pos));
			RelayMouseClickToInspectorList(m_pParentWnd, msgPoint);
		}
		// OnOK();
	}
	return FALSE;
}


BOOL CCompassOptDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	ASSERT(CCompassData::bIsSingle(m_uCompassVal));
	
	CRect rectWnd;
	GetWindowRect(rectWnd);
	CPoint ptLeftTop(m_ptRightTop.x-rectWnd.Width(),m_ptRightTop.y);
	SetWindowPos(NULL,ptLeftTop.x,ptLeftTop.y,0,0,SWP_NOSIZE|SWP_NOZORDER);



	if(m_uCompassVal&CMPS_CENTER)
	{
		m_nRadio=0;
	}


	if(m_uCompassVal&CMPS_NORTH)
	{
		m_nRadio=1;
	}

	if(m_uCompassVal&CMPS_EAST)
	{
		m_nRadio=3;
	}
	
	if(m_uCompassVal&CMPS_SOUTH)
	{
		m_nRadio=5;
	}

	if(m_uCompassVal&CMPS_WEST)
	{
		m_nRadio=7;
	}

	if(m_uCompassVal&CMPS_NORTHEAST)
	{
		m_nRadio=2;
	}

	if(m_uCompassVal&CMPS_SOUTHEAST)
	{
		m_nRadio=4;
	}

	if(m_uCompassVal&CMPS_SOUTHWEST)
	{
		m_nRadio=6;
	}

	if(m_uCompassVal&CMPS_NORTHWEST)
	{
		m_nRadio=8;
	}

	UpdateData(FALSE);
	
	m_bInited=true;

    m_background.LoadBitmap( IDB_COMPASS_OPT );
	
	return TRUE;	              
}



void CCompassOptDlg::OnOK()
{

	UpdateData(TRUE);

	m_uCompassVal=0;

	switch(m_nRadio)
	{
		case 0:
		{
			m_uCompassVal|=CMPS_CENTER;

		}break;

		case 1:
		{
			m_uCompassVal|=CMPS_NORTH;

		}break;
		
		case 2:
		{
			m_uCompassVal|=CMPS_NORTHEAST;

		}break;

		case 3:
		{
			m_uCompassVal|=CMPS_EAST;

		}break;

		case 4:
		{
			m_uCompassVal|=CMPS_SOUTHEAST;

		}break;

		case 5:
		{
			m_uCompassVal|=CMPS_SOUTH;

		}break;

		case 6:
		{
			m_uCompassVal|=CMPS_SOUTHWEST;

		}break;

		case 7:
		{
			m_uCompassVal|=CMPS_WEST;

		}break;

		case 8:
		{
			m_uCompassVal|=CMPS_NORTHWEST;

		}break;

		default:
		{
			ASSERT(0);
			return;
		}
	}
	
	m_bClosed = true;
	CDialog::OnOK();
}


void CCompassOptDlg::OnRadioCenter() 
{
	if(m_bInited)
	{
		OnOK();
	}
}

void CCompassOptDlg::OnDoubleclickedRadioCenter() 
{
	if(m_bInited)
	{
		OnOK();
	}
}

void CCompassOptDlg::OnRadioEast() 
{
	if(m_bInited)
	{
		OnOK();
	}
}

void CCompassOptDlg::OnDoubleclickedRadioEast() 
{
	if(m_bInited)
	{
		OnOK();
	}	
}

void CCompassOptDlg::OnRadioNorth() 
{
	if(m_bInited)
	{
		OnOK();
	}	
}

void CCompassOptDlg::OnDoubleclickedRadioNorth() 
{
	if(m_bInited)
	{
		OnOK();
	}
}

void CCompassOptDlg::OnRadioNortheast() 
{
	if(m_bInited)
	{
		OnOK();
	}
}

void CCompassOptDlg::OnDoubleclickedRadioNortheast() 
{
	if(m_bInited)
	{
		OnOK();
	}
}

void CCompassOptDlg::OnRadioNorthwest() 
{
	if(m_bInited)
	{
		OnOK();
	}
}

void CCompassOptDlg::OnDoubleclickedRadioNorthwest() 
{
	if(m_bInited)
	{
		OnOK();
	}	
}

void CCompassOptDlg::OnRadioSouth() 
{
	if(m_bInited)
	{
		OnOK();
	}
}

void CCompassOptDlg::OnDoubleclickedRadioSouth() 
{
	if(m_bInited)
	{
		OnOK();
	}
}

void CCompassOptDlg::OnRadioSoutheast() 
{
	if(m_bInited)
	{
		OnOK();
	}	
}

void CCompassOptDlg::OnDoubleclickedRadioSoutheast() 
{
	if(m_bInited)
	{
		OnOK();
	}	
}

void CCompassOptDlg::OnRadioSouthwest() 
{
	if(m_bInited)
	{
		OnOK();
	}

	
}

void CCompassOptDlg::OnDoubleclickedRadioSouthwest() 
{
	if(m_bInited)
	{
		OnOK();
	}
}

void CCompassOptDlg::OnRadioWest() 
{
	if(m_bInited)
	{
		OnOK();
	}	
}

void CCompassOptDlg::OnDoubleclickedRadioWest() 
{
	if(m_bInited)
	{
		OnOK();
	}	
}

void CCompassOptDlg::OnPaint() 
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
                    , 130, 130 //m_background.GetBitmapDimension().cx
                    //, m_background.GetBitmapDimension().cy
                    , SRCCOPY 
                    );

    memdc.SelectObject( oldbmp );
}
