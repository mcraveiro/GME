// CompInfoDlg.cpp : implementation file
//
#include "stdafx.h"
#include "mgautil.h"
#include "CompInfoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCompInfoDlg dialog


CCompInfoDlg::CCompInfoDlg(IMgaRegistrar *reg,  CWnd* pParent /*=NULL*/)
	: CDialog(CCompInfoDlg::IDD, pParent), 	registrar(reg)
{

	//{{AFX_DATA_INIT(CCompInfoDlg)
	m_name = _T("");
	m_paradigm = _T("");
	m_engine = _T("");
	m_filename = _T("");
	m_description = _T("");
	m_progid = _T("");
	m_version = _T("");
	//}}AFX_DATA_INIT
}


void CCompInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCompInfoDlg)
	DDX_Control(pDX, IDC_ENGINECOMBO, m_execeng);
	DDX_Text(pDX, IDC_NAME, m_name);
	DDX_Text(pDX, IDC_PARADIGM, m_paradigm);
	DDX_Text(pDX, IDC_FILENAME, m_filename);
	DDX_Text(pDX, IDC_DESCRIPTION, m_description);
	DDX_Text(pDX, IDC_PROGID, m_progid);
	DDX_Text(pDX, IDC_VERSION, m_version);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCompInfoDlg, CDialog)
	//{{AFX_MSG_MAP(CCompInfoDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCompInfoDlg message handlers

void CCompInfoDlg::OnOK() 
{
	UpdateData();

	int v;

	if(m_progid.Find('.') < 0) {
		AfxMessageBox(_T("ProgID must contain at least one '.'"));
	}
	else if(_stscanf_s(m_version, _T("%d"), &v) != 1 || v <= 0) {
		AfxMessageBox(_T("Version ID must start with a number > 0"));
	}
	else if (m_execeng.GetCurSel() == -1)
	{
		AfxMessageBox(_T("No execution engine selected"));
	}
	else {
		m_execeng.GetLBText(m_execeng.GetCurSel(),engine);
		CDialog::OnOK();
	}
}

BOOL CCompInfoDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CComVariant v;
	COMTHROW(registrar->get_Components(REGACCESS_BOTH, &v));

	CComBstrObjArray progids;
	MoveTo(v, progids);

	int sel = 0;
	for(int i = 0; i < progids.GetSize(); ++i)	{
			componenttype_enum type;
			CComBSTR dummy;
			HRESULT hr = registrar->QueryComponent(CComBSTR(progids[i]), &type, &dummy, REGACCESS_PRIORITY);
			if(hr != S_OK || ! (type & COMPONENTTYPE_EXECENGINE)) continue;
			CString name(progids[i]);
			int p = m_execeng.AddString(name);
			if(name.Find(_T("attern")) >= 0) sel = p;
	}
	m_execeng.SetCurSel(sel);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
