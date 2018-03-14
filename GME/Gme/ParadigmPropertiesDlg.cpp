// ParadigmPropertiesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "gme.h"
#include "GMEApp.h"
#include "GMEstd.h"
#include "ParadigmPropertiesDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CParadigmPropertiesDlg dialog


CParadigmPropertiesDlg::CParadigmPropertiesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CParadigmPropertiesDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CParadigmPropertiesDlg)
	//}}AFX_DATA_INIT
}


void CParadigmPropertiesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CParadigmPropertiesDlg)
	DDX_Control(pDX, IDC_PARADIGM_VERSION, version);
	DDX_Control(pDX, IDC_PARADIGM_NAME, name);
	DDX_Control(pDX, IDC_PARADIGM_GUID, guid);
	DDX_Control(pDX, IDC_PARADIGM_MODIFIED, modified);
	DDX_Control(pDX, IDC_PARADIGM_CREATED, created);
	DDX_Control(pDX, IDC_PARADIGM_COMMENT, comment);
	DDX_Control(pDX, IDC_PARADIGM_AUTHOR, author);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CParadigmPropertiesDlg, CDialog)
	//{{AFX_MSG_MAP(CParadigmPropertiesDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CParadigmPropertiesDlg message handlers

BOOL CParadigmPropertiesDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CComObjPtr<IMgaTerritory> terry;
	COMTHROW( theApp.mgaProject->CreateTerritory(NULL, PutOut(terry), NULL) );
	try {
		theApp.mgaProject->BeginTransaction(terry,TRANSACTION_READ_ONLY);
		CComPtr<IMgaMetaProject> metaProject;
		COMTHROW(theApp.mgaProject->get_RootMeta(&metaProject));

		{
			CComBSTR bstr;
			COMTHROW(metaProject->get_Name(&bstr));
			CString str;
			CopyTo(bstr,str);
			name.SetWindowText(str);
		}
		{
			CComBSTR bstr;
			COMTHROW(metaProject->get_Author(&bstr));
			CString str;
			CopyTo(bstr,str);
			author.SetWindowText(str);
		}
		{
			CComBSTR bstr;
			COMTHROW(metaProject->get_Version(&bstr));
			CString str;
			CopyTo(bstr,str);
			version.SetWindowText(str);
		}
		{
			CComBSTR bstr;
			COMTHROW(metaProject->get_CreatedAt(&bstr));
			CString str;
			CopyTo(bstr,str);
			created.SetWindowText(str);
		}
		{
			CComBSTR bstr;
			COMTHROW(metaProject->get_ModifiedAt(&bstr));
			CString str;
			CopyTo(bstr,str);
			modified.SetWindowText(str);
		}
		{
			CComBSTR bstr;
			COMTHROW(metaProject->get_Comment(&bstr));
			CString str;
			CopyTo(bstr,str);
			comment.SetWindowText(str);
		}
		{
			CComVariant vguid;
			COMTHROW(metaProject->get_GUID(&vguid));
			GUID guid1;
			CopyTo(vguid, guid1);
			CComBstrObj bstro;
 			CopyTo(guid1, bstro);
			CString str;
			CopyTo(bstro, str);

			guid.SetWindowText(str);
		}
	
		theApp.mgaProject->CommitTransaction();
	}
	catch(hresult_exception e) {
		theApp.mgaProject->AbortTransaction();
		AfxMessageBox(_T("Error reading paradigm properties"));
	}
		
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CParadigmPropertiesDlg::PreTranslateMessage(MSG* pMsg)
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
