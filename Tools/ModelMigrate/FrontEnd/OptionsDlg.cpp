// OptionsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ModelMigrate.h"
#include "OptionsDlg.h"
#include ".\optionsdlg.h"
#include "FileTransDlg.h"

// OptionsDlg dialog

IMPLEMENT_DYNAMIC(OptionsDlg, CDialog)
OptionsDlg::OptionsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(OptionsDlg::IDD, pParent)
	, m_dir(_T(""))
	, m_scr(_T(""))
	, m_append(_T(""))
	, m_outputDir(_T(""))
	, m_intermediateFilesDir(_T(""))
{
}

OptionsDlg::~OptionsDlg()
{
}

void OptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_dir);
	DDX_Text(pDX, IDC_EDIT2, m_scr);
	DDX_Text(pDX, IDC_EDIT5, m_append);
	DDX_Text(pDX, IDC_EDIT6, m_outputDir);
	DDX_Text(pDX, IDC_EDIT7, m_intermediateFilesDir);
}


BEGIN_MESSAGE_MAP(OptionsDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON6, OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON7, OnBnClickedButton7)
	ON_BN_CLICKED(IDC_BUTTON8, OnBnClickedButton8)
END_MESSAGE_MAP()


// OptionsDlg message handlers

void OptionsDlg::OnBnClickedButton1()
{
	UpdateData( TRUE);
	CString dir = FileTransDlg::getDirectory("Load .xme files on startup from this directory");
	if( !dir.IsEmpty()) 
	{
		m_dir = dir;
	}
	UpdateData( FALSE);
}

void OptionsDlg::OnBnClickedButton6()
{
	UpdateData( TRUE);
	CFileDialog dlg( TRUE, "xsl", 0, 0,
		"XSLT Files (*.xsl;*.xslt)|*.xsl; *.xslt|All Files (*.*)|*.*||");
	if( dlg.DoModal() == IDOK)
	{
		m_scr = dlg.GetPathName();
	}

	UpdateData( FALSE);
}

BOOL OptionsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void OptionsDlg::OnBnClickedButton7()
{
	UpdateData( TRUE);
	CString dir = FileTransDlg::getDirectory("Please specify a target directory for artifact .xme files");
	if( !dir.IsEmpty()) 
	{
		m_outputDir = dir;
	}
	UpdateData( FALSE);
}

void OptionsDlg::OnOK()
{
	UpdateData( TRUE);
	
	if( m_dir.Right(1) == '\\')
		m_dir.Truncate( m_dir.GetLength() - 1);// delete trailing '\\'

	if( m_outputDir.Right(1) == '\\')
		m_outputDir.Truncate( m_outputDir.GetLength() - 1);// delete trailing '\\'

	if( m_intermediateFilesDir.Right(1) == '\\')
		m_intermediateFilesDir.Truncate( m_intermediateFilesDir.GetLength() - 1);// delete trailing '\\'

	UpdateData( FALSE);

	if( m_outputDir.IsEmpty() && m_append.IsEmpty())
	{
		AfxMessageBox( "The transformation process will save output to a destination file different from the source file.\nEither select a target directory or specify a string which will be appended to destination filenames!");
	}
	else if( !m_outputDir.IsEmpty() && !FileTransDlg::directoryExists( m_outputDir))
	{
		AfxMessageBox( "Invalid target directory specified!");
	}
	else if( !m_intermediateFilesDir.IsEmpty() && !FileTransDlg::directoryExists( m_intermediateFilesDir))
	{
		AfxMessageBox( "Invalid intermediate directory specified!");
	}
	else
	{
		CDialog::OnOK();
	}
}
void OptionsDlg::OnBnClickedButton8()
{
	UpdateData( TRUE);
	CString dir = FileTransDlg::getDirectory("Please specify a directory for intermediate .xme files");
	if( !dir.IsEmpty()) 
	{
		m_intermediateFilesDir = dir;
	}
	UpdateData( FALSE);

}
