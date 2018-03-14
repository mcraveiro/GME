// AttrTypeChangeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ModelMigrate.h"
#include "AttrTypeChangeDlg.h"
#include ".\attrtypechangedlg.h"
#include "AttrGlobalDlg.h"
#include "Script.h"
#include <fstream>

// AttrTypeChangeDlg dialog

IMPLEMENT_DYNAMIC(AttrTypeChangeDlg, CDialog)
AttrTypeChangeDlg::AttrTypeChangeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(AttrTypeChangeDlg::IDD, pParent)
	, m_attrName(_T(""))
	, m_bTerm(FALSE)
	, m_global(_T("1"))
	, m_owner(_T(""))
{
}

AttrTypeChangeDlg::~AttrTypeChangeDlg()
{
}

void AttrTypeChangeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_attrName);
	DDX_Check(pDX, IDC_CHECK1, m_bTerm);
}


BEGIN_MESSAGE_MAP(AttrTypeChangeDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_BN_CLICKED(IDGLOBAL, OnBnClickedGlobal)
END_MESSAGE_MAP()

void AttrTypeChangeDlg::init( const CString& par1, const CString& par2, const CString& par3, const CString& par4)
{
	m_attrName = par1;
	m_bTerm = par2 == "1";
	m_global = par3;
	m_owner = par4;
}

const CString& AttrTypeChangeDlg::getPar1() const
{
	return m_attrName;
}

CString AttrTypeChangeDlg::getPar2() const
{
	return m_bTerm?"1":"0";
}

const CString& AttrTypeChangeDlg::getPar3() const
{
	return m_global;
}

const CString& AttrTypeChangeDlg::getPar4() const
{
	return m_owner;
}

// AttrTypeChangeDlg message handlers

void AttrTypeChangeDlg::OnBnClickedButton1()
{
	UpdateData( TRUE);

	if( m_attrName.IsEmpty()) 
	{
		AfxMessageBox( "No Attribute Kind specified!"); 
		return;
	}

	CFileDialog dlg( FALSE, ".xsl", m_attrName + "2IntCheck", OFN_OVERWRITEPROMPT, 
		"XSLT Files (*.xsl;*.xslt)|*.xsl; *.xslt|All Files (*.*)|*.*||");

	if( dlg.DoModal() == IDOK)
	{
		genSeparateScript( m_attrName, dlg.GetPathName(), m_global == "1", m_owner);
	}

	UpdateData( FALSE);
}

void AttrTypeChangeDlg::genSeparateScript( CString pAttrName, CString pOutputFileName, bool pGlobal, CString pOwner)
{
	CString contents = Script::getXSLFromResource( "TAttrTypeCheck2Text.xsl").c_str();
	contents.Replace( "##|par1|##", pAttrName);
	
	// whether global or local attribute
	if( pGlobal)
		contents.Replace( "##|OPTIONALLOCALCONDITION|##", "");
	else
	{
		if( pOwner == "") AfxMessageBox( "No owner specified for local attribute!");
		CString toIns = " and ../@kind='" + pOwner + "'";
		contents.Replace( "##|OPTIONALLOCALCONDITION|##", toIns);
	}

	std::fstream f;
	f.open( (LPCTSTR) pOutputFileName, std::ios_base::out|std::ios_base::binary); // write out as is (not to mess with additional CR-s)
	ASSERT( f.is_open());
	if( !f.is_open())
	{
		AfxMessageBox( "Could not create target file!");
		return;
	}

	if( !contents.IsEmpty())
	{
		f.write( (LPCTSTR) contents, (std::streamsize) contents.GetLength());
	}
	
	f.close();

	AfxMessageBox( pOutputFileName + " saved. You can apply anytime this script to an xme file to check whether its '" + pAttrName + "' values are convertible to numbers.");
}

void AttrTypeChangeDlg::OnBnClickedGlobal()
{
	AttrGlobalDlg dlg( m_global, m_owner);
	if( dlg.DoModal() == IDOK)
	{
		m_global = dlg.getGlobal();
		m_owner  = dlg.getOwner();
	}
}
