// FrontEndDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FrontEnd.h"
#include "FrontEndDlg.h"
#include ".\frontenddlg.h"
#include "NewRuleDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CFrontEndDlg dialog



CFrontEndDlg::CFrontEndDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFrontEndDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFrontEndDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_LIST1, m_list);
	DDX_Control(pDX, IDC_LIST2, m_choice);
}

BEGIN_MESSAGE_MAP(CFrontEndDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_CREATE()
	ON_LBN_DBLCLK(IDC_LIST2, OnLbnDblclkList2)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_BN_CLICKED(IDOK2, OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)
END_MESSAGE_MAP()


// CFrontEndDlg message handlers

BOOL CFrontEndDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	//CRect loc(10, 10, 100, 100);
	//m_list.Create(WS_CHILD|WS_VISIBLE|WS_BORDER|LVS_REPORT|LVS_SINGLESEL, loc, this, 1);
	//m_list.ShowWindow
	m_choice.AddString("Kind");
	m_choice.AddString("AttrName");
	m_choice.AddString("AttrType");
	m_choice.AddString("AttrItem");
	m_choice.AddString("Position");

	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CFrontEndDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CFrontEndDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CFrontEndDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

int CFrontEndDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect loc(10, 10, 100, 100);
	GetClientRect( &loc);
	loc.DeflateRect( 5, 5, 90, 5);
	m_list.Create(WS_CHILD|WS_VISIBLE|WS_BORDER|WS_TABSTOP|LVS_REPORT|LVS_SINGLESEL, loc, this, 1);

	return 0;
}

void CFrontEndDlg::OnLbnDblclkList2()
{
	int chc = m_choice.GetCurSel();
	CString txt;
	m_choice.GetText( chc, txt);
	m_list.addRule(chc, txt);
}

void CFrontEndDlg::OnBnClickedButton1()
{
	NewRuleDlg nrd;
	if( nrd.DoModal() == IDOK)
	{
		m_list.addRule( nrd.m_choice, nrd.m_choiceStr);
	}
}
void CFrontEndDlg::OnOK()
{
	if( AfxMessageBox( "Close dialog and discard changes?", MB_YESNOCANCEL) == IDYES)
		closeDlg( IDOK);
}

void CFrontEndDlg::OnCancel()
{
	if( AfxMessageBox( "Close dialog and discard changes?", MB_YESNOCANCEL) == IDYES)
		closeDlg( IDCANCEL);
}

void CFrontEndDlg::closeDlg( int pResult)
{
	EndDialog( pResult);
	DestroyWindow();
}

void CFrontEndDlg::OnBnClickedOk()
{
	closeDlg(IDOK);
}

void CFrontEndDlg::OnBnClickedButton2()
{
	CFileDialog dlg( FALSE, ".xsl", "script1", OFN_OVERWRITEPROMPT, 
		"XSLT Files (*.xsl;*.xslt)|*.xsl; *.xslt|All Files (*.*)|*.*||");
	if( dlg.DoModal() == IDOK)
	{
		std::string my_target = (LPCTSTR) dlg.GetPathName();
		m_list.gen( my_target);
	}
}
