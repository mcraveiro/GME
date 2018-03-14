// ProjectPropertiesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "GMEApp.h"
#include "GMEstd.h"
#include "ProjectPropertiesDlg.h"
#include "ParadigmPropertiesDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProjectPropertiesDlg dialog


CProjectPropertiesDlg::CProjectPropertiesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProjectPropertiesDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CProjectPropertiesDlg)
	//}}AFX_DATA_INIT
}


void CProjectPropertiesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProjectPropertiesDlg)
	DDX_Control(pDX, IDC_PROJECT_GUID, guid);
	DDX_Control(pDX, IDC_PROJECT_VERSION, version);
	DDX_Control(pDX, IDC_PROJECT_PARVERSION, m_parversion);
	DDX_Control(pDX, IDC_PROJECT_PARGUID, m_parguid);
	DDX_Control(pDX, IDC_PROJECT_PARADIGM, m_parname);
	DDX_Control(pDX, IDC_PROJECT_MODIFIED, modified);
	DDX_Control(pDX, IDC_PROJECT_CREATED, created);
	DDX_Control(pDX, IDC_PROJECT_COMMENT, comment);
	DDX_Control(pDX, IDC_PROJECT_AUTHOR, author);
	DDX_Control(pDX, IDC_PROJECT_NAME, name);
	DDX_Control(pDX, IDC_PROJECT_CONNECTION, conn);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProjectPropertiesDlg, CDialog)
	//{{AFX_MSG_MAP(CProjectPropertiesDlg)
	ON_BN_CLICKED(IDC_BUTTON_PARADIGM, OnButtonParadigm)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProjectPropertiesDlg message handlers

void CProjectPropertiesDlg::OnOK() 
{
	CComObjPtr<IMgaTerritory> terry;
	COMTHROW( theApp.mgaProject->CreateTerritory(NULL, PutOut(terry), NULL) );
	try {
		COMTHROW(theApp.mgaProject->BeginTransaction(terry,TRANSACTION_GENERAL));
		{
			CComBSTR bstr;
			CString str;
			name.GetWindowText(str);
			CopyTo(str,bstr);
			COMTHROW(theApp.mgaProject->put_Name(bstr));
			CComPtr<IMgaFolder>	rootFolder;
			COMTHROW(theApp.mgaProject->get_RootFolder(&rootFolder));
			COMTHROW(rootFolder->put_Name(bstr));
			theApp.projectName = str;
		}
		{
			CComBSTR bstr;
			CString str;
			author.GetWindowText(str);
			CopyTo(str,bstr);
			COMTHROW(theApp.mgaProject->put_Author(bstr));
		}
		{
			CComBSTR bstr;
			CString str;
			comment.GetWindowText(str);
			str.Replace(_T("\r\n"),_T("\n"));
			CopyTo(str,bstr);
			COMTHROW(theApp.mgaProject->put_Comment(bstr));
		}
		{
			CComBSTR bstr;
			CString str;
			version.GetWindowText(str);
			CopyTo(str,bstr);
			COMTHROW(theApp.mgaProject->put_Version(bstr));
		}
		COMTHROW(theApp.mgaProject->CommitTransaction());
	}
	catch(hresult_exception e) {
		theApp.mgaProject->AbortTransaction();
		AfxMessageBox(_T("Error writing project properties"));
	}
	CDialog::OnOK();
}

BOOL CProjectPropertiesDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CComObjPtr<IMgaTerritory> terry;
	try {
		COMTHROW( theApp.mgaProject->CreateTerritory(NULL, PutOut(terry), NULL) );
		theApp.mgaProject->BeginTransaction(terry,TRANSACTION_READ_ONLY);
		{
			CComBSTR bstr;
			COMTHROW(theApp.mgaProject->get_Name(&bstr));
			CString str;
			CopyTo(bstr,str);
			name.SetWindowText(str);
		}
		{
			CComBSTR bstr;
			COMTHROW(theApp.mgaProject->get_ProjectConnStr(&bstr));
			CString str;
			CopyTo(bstr,str);
			conn.SetWindowText(str.Mid(4));
		}
		{
			CComBSTR bstr;
			COMTHROW(theApp.mgaProject->get_Author(&bstr));
			CString str;
			CopyTo(bstr,str);
			author.SetWindowText(str);
		}
		{
			CComBSTR bstr;
			COMTHROW(theApp.mgaProject->get_Version(&bstr));
			CString str;
			CopyTo(bstr,str);
			version.SetWindowText(str);
		}
		{
			CComBSTR bstr;
			COMTHROW(theApp.mgaProject->get_CreateTime(&bstr));
			CString str;
			CopyTo(bstr,str);
			created.SetWindowText(str);
		}
		{
			CComBSTR bstr;
			COMTHROW(theApp.mgaProject->get_ChangeTime(&bstr));
			CString str;
			CopyTo(bstr,str);
			modified.SetWindowText(str);
		}
		{
			CComBSTR bstr;
			COMTHROW(theApp.mgaProject->get_Comment(&bstr));
			CString str;
			CopyTo(bstr,str);
			str.Replace(_T("\n"), _T("\r\n"));
			comment.SetWindowText(str);
		}
		{
			CComVariant vguid;
			COMTHROW(theApp.mgaProject->get_GUID(&vguid));
			GUID guid1;
			CopyTo(vguid, guid1);
			CComBstrObj bstro;
 			CopyTo(guid1, bstro);
			CString str;
			CopyTo(bstro, str);

			guid.SetWindowText(str);
		}
		{
			CComBSTR bstr;
			COMTHROW(theApp.mgaProject->get_MetaName(&bstr));
			CString str;
			CopyTo(bstr,str);
			m_parname.SetWindowText(str);
		}
		{
			CComBSTR bstr;
			COMTHROW(theApp.mgaProject->get_MetaVersion(&bstr));
			CString str;
			CopyTo(bstr,str);
			m_parversion.SetWindowText(str);
		}
		{
			CComVariant guid;
			COMTHROW(theApp.mgaProject->get_MetaGUID(&guid));
			GUID guid1;
			CopyTo(guid, guid1);
			CComBstrObj bstro;
 			CopyTo(guid1, bstro);
			CString str;
			CopyTo(bstro, str);

			m_parguid.SetWindowText(str);
		}


		theApp.mgaProject->CommitTransaction();
	}
	catch(hresult_exception e) {
		theApp.mgaProject->AbortTransaction();
		AfxMessageBox(_T("Error reading project properties"));
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CProjectPropertiesDlg::OnButtonParadigm() 
{
	CParadigmPropertiesDlg dlg;
	dlg.DoModal();
}

BOOL CProjectPropertiesDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		switch (pMsg->wParam)
		{
		case 'A':
			{
				if (GetKeyState(VK_CONTROL) & 0x8000)
				{
					CWnd* focus = GetFocus();
					if (focus && focus->IsKindOf(RUNTIME_CLASS(CEdit)))
					{
						((CEdit*)GetFocus())->SetSel(0, 1000);
						return TRUE;
					}
				}
			}
			break;
		}
	}
	return __super::PreTranslateMessage(pMsg);
}
