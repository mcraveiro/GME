// SimpleRepl.cpp : implementation file
//

#include "stdafx.h"
#include "ModelMigrate.h"
#include "SimpleRepl.h"


// SimpleReplace dialog

IMPLEMENT_DYNAMIC(SimpleReplace, CDialog)
SimpleReplace::SimpleReplace(CWnd* pParent /*=NULL*/)
	: CDialog(SimpleReplace::IDD, pParent)
	, m_oldV(_T(""))
	, m_newV(_T(""))
{
}

SimpleReplace::~SimpleReplace()
{
}

void SimpleReplace::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_oldV);
	DDX_Text(pDX, IDC_EDIT2, m_newV);
}


BEGIN_MESSAGE_MAP(SimpleReplace, CDialog)
END_MESSAGE_MAP()
// SimpleReplace message handlers

const CString& SimpleReplace::getPar1() const
{
	return m_oldV;
}

const CString& SimpleReplace::getPar2() const
{
	return m_newV;
}

void SimpleReplace::init( const CString& par1, const CString& par2)
{
	m_oldV = par1;
	m_newV = par2;
}

