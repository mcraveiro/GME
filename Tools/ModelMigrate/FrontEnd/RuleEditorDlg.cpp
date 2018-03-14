// RuleEditorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ModelMigrate.h"
#include "RuleEditorDlg.h"
#include ".\ruleeditordlg.h"

// RuleEditorDlg dialog

IMPLEMENT_DYNAMIC(RuleEditorDlg, CDialog)
RuleEditorDlg::RuleEditorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(RuleEditorDlg::IDD, pParent)
	, m_closed( true)
{
	m_ruleList.setParent( this);
}

RuleEditorDlg::~RuleEditorDlg()
{
}

void RuleEditorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RULELIST, m_ruleList);
}


BEGIN_MESSAGE_MAP(RuleEditorDlg, CDialog)
	ON_BN_CLICKED(IDGENERATE, OnBnClickedGenerate)
	ON_BN_CLICKED(IDGENERATESEL, OnBnClickedGeneratesel)
	ON_BN_CLICKED(IDADDRULE, OnBnClickedAddrule)
	ON_BN_CLICKED(IDSAVE, OnBnClickedSave)
	ON_BN_CLICKED(IDLOAD, OnBnClickedLoad)
	ON_WM_SIZE()
	ON_WM_KEYDOWN()
	ON_NOTIFY(LVN_KEYDOWN, IDC_RULELIST, OnLvnKeydownRulelist)
END_MESSAGE_MAP()


// RuleEditorDlg message handlers

void RuleEditorDlg::OnBnClickedGenerate()
{
	bool separate_files = false;
	CButton * bttn = (CButton *) GetDlgItem(IDC_CHECKSEPARATEFILE);
	if( bttn) separate_files = bttn->GetCheck() == BST_CHECKED;

	m_ruleList.genAll( separate_files);
}

void RuleEditorDlg::OnBnClickedGeneratesel()
{
	bool separate_files = false;
	CButton * bttn = (CButton *) GetDlgItem(IDC_CHECKSEPARATEFILE);
	if( bttn) separate_files = bttn->GetCheck() == BST_CHECKED;

	m_ruleList.genSelected( separate_files);
}

void RuleEditorDlg::closeDlg( int pResult)
{
	EndDialog( pResult);
	DestroyWindow();
	m_closed = true;
	m_ruleList.reset();
}

void RuleEditorDlg::OnOK()
{
	//int resp = AfxMessageBox( "Save changes?", MB_YESNOCANCEL);
	//if( resp == IDYES)
	//{
	//	OnBnClickedSave();
	//	closeDlg( IDOK);
	//}
	//else if( resp == IDNO)
	//{
	//	closeDlg( IDOK);
	//}
}

void RuleEditorDlg::OnCancel()
{
	if( AfxMessageBox( "Close dialog and discard changes?", MB_YESNOCANCEL) == IDYES)
		closeDlg( IDCANCEL);
}

void RuleEditorDlg::OnBnClickedAddrule()
{
	m_ruleList.add();
}

BOOL RuleEditorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_closed = false;

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void RuleEditorDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	if( cx < 200) cx = 200;  // to prevent mixing/shuffling controls because of the very
	if( cy < 150)  cy = 150; // small values (the box resizing is not prevented by these)

	if( m_ruleList.GetSafeHwnd())
	{
		CRect rect;
		m_ruleList.GetWindowRect( &rect);
		ScreenToClient( &rect);

		rect.bottom = cy - rect.left;
		rect.right = cx - rect.left; // maintain margin
		m_ruleList.SetWindowPos( 0, 0, 0, rect.Width(), rect.Height(), SWP_NOMOVE|SWP_NOZORDER|SWP_SHOWWINDOW);
	}
}


void RuleEditorDlg::OnBnClickedSave()
{
	m_ruleList.fileSave();
}

void RuleEditorDlg::OnBnClickedLoad()
{
	m_ruleList.fileLoad();
}

void RuleEditorDlg::PostNcDestroy()
{
	CDialog::PostNcDestroy();
}

void RuleEditorDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default

	CDialog::OnKeyDown(nChar, nRepCnt, nFlags);
}

void RuleEditorDlg::OnLvnKeydownRulelist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);
	NMLVKEYDOWN &s = *pLVKeyDow;

	bool cntrl = GetKeyState( VK_CONTROL) < 0;
	bool shft =  GetKeyState( VK_SHIFT) < 0;
	bool alt  =  GetKeyState( VK_MENU) < 0; // ALT

	// VK_ENTER -> is handled in OnCommand with when IDOK comes in

	if( s.wVKey == VK_APPS) m_ruleList.showPopupMenu();

	if( s.wVKey == VK_F3) // F3, Load
		SendMessage( WM_COMMAND, ID_MYFILELOAD, 0);

	if( s.wVKey == VK_F2) // F2, Save
		SendMessage( WM_COMMAND, ID_MYFILESAVE, 0);

	if( s.wVKey == VK_UP && alt)
		SendMessage( WM_COMMAND, ID_RULE_MOVEUP, 0);  // MoveUp
	if( s.wVKey == VK_DOWN && alt)
		SendMessage( WM_COMMAND, ID_RULE_MOVEDOWN, 0);// MoveDown
	
	if( s.wVKey == VK_INSERT && !cntrl) // Add Rule
		SendMessage( WM_COMMAND, ID_RULE_ADD, 0);
	if( s.wVKey == VK_INSERT && cntrl) // Clone Rule
		SendMessage( WM_COMMAND, ID_RULE_CLONE, 0);

	if( s.wVKey == VK_DELETE && !cntrl) // Del Sel
		SendMessage( WM_COMMAND, ID_RULE_DELETESELECTED, 0);
	if( s.wVKey == VK_DELETE && cntrl) // Del All
		SendMessage( WM_COMMAND, ID_RULE_DELETEALL, 0);

	*pResult = 0;
}

BOOL RuleEditorDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch( wParam)
	{
	case IDOK: onReturn();          break;
	case ID_MYFILENEW:  onFileNew();break;
	case ID_MYFILELOAD: onFileLoad();break;
	case ID_MYFILESAVE: onFileSave();break;
	case ID_RULE_ADD:                m_ruleList.onAdd();;break;
	case ID_RULE_CLONE:              m_ruleList.onClone();break;
	case ID_RULE_DELETESELECTED:     m_ruleList.onDeleSel();break;
	case ID_RULE_DELETEALL:          m_ruleList.onDeleAll();break;
	case ID_RULE_MOVEUP:             m_ruleList.onMoveUp();break;
	case ID_RULE_MOVEDOWN:           m_ruleList.onMoveDown();break;
	case ID_SCRIPT_GENERATEALL:      OnBnClickedGenerate();break;
	case ID_SCRIPT_GENERATESELECTED: OnBnClickedGeneratesel();break;
	case ID_MYFILECLOSE: onFileClose();break;
	default: return CDialog::OnCommand(wParam, lParam);
	};

	if( wParam == ID_MYFILECLOSE) return TRUE;
	return CDialog::OnCommand(wParam, lParam);
}

void RuleEditorDlg::onReturn()
{
	CWnd* child = this->GetFocus();
	if( !child) return;
	if( child == GetDlgItem(IDC_RULELIST))
	{
		m_ruleList.onEdit();
	}
}

void RuleEditorDlg::onFileNew()
{
	bool empty = m_ruleList.erasable();
	if( !empty)
		empty = IDOK == AfxMessageBox("Create new file?", MB_OKCANCEL);

	if( empty)
		m_ruleList.fileNew();
}

void RuleEditorDlg::onFileLoad()
{
	m_ruleList.fileLoad();
}

void RuleEditorDlg::onFileSave()
{
	m_ruleList.fileSave();
}

void RuleEditorDlg::onFileClose()
{
	closeDlg( IDCANCEL);
}