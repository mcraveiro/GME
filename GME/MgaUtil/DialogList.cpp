// DialogList.cpp : implementation file
//

#include "stdafx.h"
#include "DialogList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogList dialog


CDialogList::CDialogList(CString _caption, dialogmode _mode, bool hasIgnore, CWnd* pParent )
	: CDialog(CDialogList::IDD, pParent), mn_selection_index(LB_ERR),
	m_caption(_caption), cb_mode(_mode)
{
	//{{AFX_DATA_INIT(CDialogList)
	m_sz_listselection = _T("");
	mb_check_once = FALSE;
	//}}AFX_DATA_INIT

    m_hasIgnore = hasIgnore;
}


void CDialogList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogList)
	DDX_Control(pDX, IDC_BUTTON_IGNORE, m_btnIgnore);
	DDX_Control(pDX, IDC_CHECK1, mcb_check_once);
	DDX_Control(pDX, IDC_LIST, m_list);
	DDX_LBString(pDX, IDC_LIST, m_sz_listselection);
	DDX_Check(pDX, IDC_CHECK1, mb_check_once);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogList, CDialog)
	//{{AFX_MSG_MAP(CDialogList)
	ON_BN_CLICKED(IDC_BUTTON_IGNORE, OnButtonIgnore)
	ON_LBN_DBLCLK(IDC_LIST, OnDblclkList)
	ON_LBN_SELCHANGE(IDC_LIST, OnChangeList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogList message handlers

BOOL CDialogList::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	this->SetWindowText(this->m_caption);

	
	if(cb_mode == CHKTEXT_NONE) {
		mcb_check_once.EnableWindow(FALSE);
	}
	else {
		static CString chkboxtexts[] = { 
			"Remember this decision", 
			"Apply this selection in future until Shift is pressed" 
		};
		ASSERT(cb_mode <= sizeof(chkboxtexts)/sizeof(chkboxtexts[0]));
		mcb_check_once.SetWindowText(chkboxtexts[cb_mode-1]);
		this->mcb_check_once.SetCheck(((this->mb_check_once == FALSE) ? 0 : 1));
	}
	POSITION pos = this->m_sz_prelist.GetHeadPosition();

	while (pos != NULL) {

		this->m_list.AddString( this->m_sz_prelist.GetNext(pos) );
	}

    m_btnIgnore.ShowWindow( m_hasIgnore ? SW_SHOW : SW_HIDE );

	CWnd* wndOK = this->GetDlgItem(IDOK);
	wndOK->EnableWindow(FALSE);
	
	return TRUE;
}

void CDialogList::OnOK() 
{
	// TODO: Add extra validation here

	this->mn_selection_index = this->m_list.GetCurSel();
	
	CDialog::OnOK();
}

void CDialogList::OnButtonIgnore() 
{
	UpdateData(TRUE);
    EndDialog( IDIGNORE );
}

void CDialogList::OnDblclkList(void)
{
	OnOK();
}

void CDialogList::OnChangeList(void)
{
	CWnd* wndOK = this->GetDlgItem(IDOK);
	wndOK->EnableWindow(TRUE);
}
