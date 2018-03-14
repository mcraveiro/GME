// OptionsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "OptionsDlg.h"
#include <afxdlgs.h>
#include ".\optionsdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// OptionsDlg dialog


OptionsDlg::OptionsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(OptionsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(OptionsDlg)
	m_headerName = _T("");
	m_sourceName = _T("");
	m_visitorHeaderName = _T("");
	m_visitorSourceName = _T("");
	m_prevHeaderName = _T("");
	m_namespaceName = _T("");
	m_bVisitor = FALSE;
	m_bParse = FALSE;
	m_bInit = FALSE;
	m_bFinalize = FALSE;
	m_bAcceptTrave = FALSE;
	m_bAcceptSpeci = FALSE;
	m_whichStyle = 0;
	m_version60 = FALSE;
	m_visitorSignature = 0;
	m_specAcceptRetVal = 0;
	m_methodOfOutput = 0;
	//}}AFX_DATA_INIT
}


void OptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(OptionsDlg)
	DDX_Control(pDX, IDC_CHECKVERS60, m_ctrlVersion60);
	DDX_Control(pDX, IDC_BUTTON3, m_ctrlButton3);
	DDX_Control(pDX, IDC_BUTTON4, m_ctrlButton4);
	DDX_Control(pDX, IDC_EDIT4, m_ctrlVisitorSource);
	DDX_Control(pDX, IDC_EDIT3, m_ctrlVisitorHeader);
	DDX_Control(pDX, IDC_BUTTON5, m_ctrlButton5);
	DDX_Control(pDX, IDC_EDIT5, m_ctrlPrevHeaderName);
	DDX_Control(pDX, IDC_RADIOSPACCRETURNSVOID, m_ctrlSpAccRetVoid);
	DDX_Control(pDX, IDC_RADIOSPACCRETURNSBOOL, m_ctrlSpAccRetBool);
	DDX_Text(pDX, IDC_EDIT1, m_headerName);
	DDX_Text(pDX, IDC_EDIT2, m_sourceName);
	DDX_Text(pDX, IDC_EDIT3, m_visitorHeaderName);
	DDX_Text(pDX, IDC_EDIT4, m_visitorSourceName);
	DDX_Text(pDX, IDC_EDIT5, m_prevHeaderName);
	DDX_Text(pDX, IDC_EDIT6, m_namespaceName);
	DDX_Check(pDX, IDC_CHECK34, m_bVisitor);
	DDX_Check(pDX, IDC_CHECK5, m_bParse);
	DDX_Check(pDX, IDC_CHECK2, m_bInit);
	DDX_Check(pDX, IDC_CHECK3, m_bFinalize);
	DDX_Check(pDX, IDC_CHECK4, m_bAcceptTrave);
	DDX_Check(pDX, IDC_CHECK6, m_bAcceptSpeci);
	DDX_Radio(pDX, IDC_RADIO1, m_whichStyle);
	DDX_Check(pDX, IDC_CHECKVERS60, m_version60);
	DDX_Radio(pDX, IDC_RADIOVISITSIGN1, m_visitorSignature);
	DDX_Radio(pDX, IDC_RADIOSPACCRETURNSVOID, m_specAcceptRetVal);
	DDX_Radio(pDX, IDC_RADIOSAMEFILE, m_methodOfOutput);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(OptionsDlg, CDialog)
	//{{AFX_MSG_MAP(OptionsDlg)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_BN_CLICKED(IDC_BUTTON2, OnButton2)
	ON_BN_CLICKED(IDC_BUTTON3, OnButton3)
	ON_BN_CLICKED(IDC_BUTTON4, OnButton4)
	ON_BN_CLICKED(IDC_BUTTON5, OnButton5)
	ON_BN_CLICKED(IDC_CHECK5, OnCheck5)
	ON_BN_CLICKED(IDC_CHECK34, OnCheck34)
	ON_BN_CLICKED(IDC_RADIO1, OnRadio1)
	ON_BN_CLICKED(IDC_RADIO2, OnRadio2)
	ON_BN_CLICKED(IDC_RADIO3, OnRadio3)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_CHECK6, OnBnClickedCheck6)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// OptionsDlg message handlers

BOOL OptionsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_ctrlPrevHeaderName.EnableWindow( FALSE);
	m_ctrlButton5.EnableWindow( FALSE);

	m_ctrlVisitorHeader.EnableWindow( m_bVisitor);
	m_ctrlVisitorSource.EnableWindow( m_bVisitor);
	m_ctrlButton3.EnableWindow( m_bVisitor);
	m_ctrlButton4.EnableWindow( m_bVisitor);

	m_ctrlSpAccRetVoid.EnableWindow( m_bAcceptSpeci);
	m_ctrlSpAccRetBool.EnableWindow( m_bAcceptSpeci);

	m_ctrlVersion60.EnableWindow( m_whichStyle >= 1);

	UpdateData( FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void OptionsDlg::doInit()
{
	//m_headerName = m_defName + "BonX.h";
	//m_sourceName = m_defName + "BonX.cpp";
	//m_prevHeaderName = m_headerName + ".bak";

	////global_vars.output_directory_name = "";
	////global_vars.err_file_name = proj_name + "BONExt.log";

	//m_visitorHeaderName = m_defName + "Visitor.h";
	//m_visitorSourceName = m_defName + "Visitor.cpp";
}

void OptionsDlg::OnButton1()
{
	UpdateData( TRUE);
	// header file name selection
	try {
		CFileDialog dlg2(FALSE, "h", m_headerName,
			OFN_EXPLORER | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT
			,"C++ header files (*.h)|*.h|All Files (*.*)|*.*||");
		if( dlg2.DoModal() == IDOK) 
		{
			m_sourceName = m_headerName = dlg2.GetPathName();
			m_prevHeaderName = m_headerName + ".copy";

			int p = m_sourceName.ReverseFind( '.');
			if ( p != -1)
			{
				m_sourceName = m_sourceName.Left(p) + ".cpp";
			}
			else
			{
				m_sourceName = m_sourceName + ".cpp";
			}
			
		}
		else // IDCANCEL , using default name
		{
			return ;
		}
	}	catch (...)	{ }

	UpdateData( FALSE);
	/*if ( existsFile( global_vars.header_file_name.c_str())) // make a copy of the header
	{
		global_vars.header_backup_name = global_vars.header_file_name + ".copy";
		int res = makeFileCopy( global_vars.header_file_name.c_str(), global_vars.header_backup_name.c_str());
		if ( res == 2)
		{
			TO("During backup process could not read file: " + global_vars.header_file_name);
			return 2;
		}
		else if ( res == 3)
		{
			TO("Cannot create backup file: " + global_vars.header_backup_name);
			return 3;
		}

		CString msg = CString("A file with ") + global_vars.header_file_name.c_str() + " name already exists." +
			"\nWould you like the user part extracted from this file?";
		if ( AfxMessageBox( msg, MB_YESNO | MB_ICONQUESTION) != IDYES)
			global_vars.header_backup_name = ""; // notify the dumper NOT to search for UP (user part) declarations
	}*/
}

void OptionsDlg::OnButton2() 
{
	UpdateData( TRUE);
	// source file name selection
	try {
		CFileDialog dlg2(FALSE, "cpp", m_sourceName,
			OFN_EXPLORER | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT
			,"C++ source files (*.cpp)|*.cpp|All Files (*.*)|*.*||");
		if( dlg2.DoModal() == IDOK) 
		{
			m_sourceName = dlg2.GetPathName();
		}
		else // IDCANCEL , using default name
		{
			return ;
		}
	}	catch (...)	{ }

	UpdateData( FALSE);
}

void OptionsDlg::OnButton3() 
{
	UpdateData( TRUE);
	// visitor header file name selection
	try {
		CFileDialog dlg2(FALSE, "h", m_visitorHeaderName,
			OFN_EXPLORER | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT
			,"C++ header files (*.h)|*.h|All Files (*.*)|*.*||");

		if( dlg2.DoModal() == IDOK) 
		{
			m_visitorSourceName = m_visitorHeaderName = (LPCTSTR) dlg2.GetPathName();
			int p = m_visitorSourceName.ReverseFind( '.');

			if ( p != -1)
				m_visitorSourceName = m_visitorSourceName.Left( p) + ".cpp";
			else
				m_visitorSourceName = m_visitorSourceName + ".cpp";

		}
		else // IDCANCEL , using default name
		{
			return ;
		}
	}	catch (...)	{ }

	UpdateData( FALSE);
}

void OptionsDlg::OnButton4() 
{
	UpdateData( TRUE);
	// visitor source file name selection
	try {
		CFileDialog dlg2(FALSE, "cpp", m_visitorSourceName,
			OFN_EXPLORER | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT
			,"C++ source files (*.cpp)|*.cpp|All Files (*.*)|*.*||");
		if(dlg2.DoModal() == IDOK) 
		{
			m_visitorSourceName = dlg2.GetPathName();
		}
	}	catch (...)	{ }

	UpdateData( FALSE);
}

void OptionsDlg::OnButton5() 
{
	UpdateData( TRUE);
	// previous header file name selection
	try {
		CFileDialog dlg2(TRUE, "h.copy", "",
			OFN_EXPLORER | OFN_HIDEREADONLY //| OFN_OVERWRITEPROMPT
			,"Previously generated header files (*.h.copy)|*.h.copy|C++ header files (*.h)|*.h|All Files (*.*)|*.*||");
		if( dlg2.DoModal() == IDOK) 
		{
			m_prevHeaderName = dlg2.GetPathName();
		}
		else // IDCANCEL , using default name
		{
			return ;
		}
	}	catch (...)	{
	}

	UpdateData( FALSE);
}

void OptionsDlg::OnCheck5() 
{
	UpdateData( TRUE);
	m_ctrlPrevHeaderName.EnableWindow( m_bParse);
	m_ctrlButton5.EnableWindow( m_bParse);
	UpdateData( FALSE);
}

void OptionsDlg::OnCheck34() 
{
	UpdateData( TRUE);
	m_ctrlVisitorHeader.EnableWindow( m_bVisitor);
	m_ctrlVisitorSource.EnableWindow( m_bVisitor);
	m_ctrlButton3.EnableWindow( m_bVisitor);
	m_ctrlButton4.EnableWindow( m_bVisitor);
	UpdateData( FALSE);
}

void OptionsDlg::OnRadio1() 
{
	m_ctrlVersion60.EnableWindow( FALSE);
}

void OptionsDlg::OnRadio2() 
{
	m_ctrlVersion60.EnableWindow( TRUE);
}

void OptionsDlg::OnRadio3() 
{
	m_ctrlVersion60.EnableWindow( TRUE);
}

void OptionsDlg::OnBnClickedCheck6()
{
	UpdateData( TRUE);
	m_ctrlSpAccRetVoid.EnableWindow( m_bAcceptSpeci);
	m_ctrlSpAccRetBool.EnableWindow( m_bAcceptSpeci);
	UpdateData( FALSE);
}
