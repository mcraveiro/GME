// PartBrowserDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PartBrowserDlg.h"
#include "PartBrowserCtrl.h"
#include "CommonError.h"
#include "CommonSmart.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPartBrowserDlg dialog


CPartBrowserDlg::CPartBrowserDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPartBrowserDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPartBrowserDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	mgaMetaModel = NULL;
	mgaProject = NULL;
}

void CPartBrowserDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPartBrowserDlg)
	DDX_Control(pDX, IDC_ASPECT_TAB, tab);
	//}}AFX_DATA_MAP
}

void CPartBrowserDlg::SetCurrentProject(CComPtr<IMgaProject> project)
{
	if (mgaProject != NULL)
		mgaProject.Release();
	mgaProject = project;
	partFrame.GetPane().SetCurrentProject(project);
}

void CPartBrowserDlg::SetMetaModel(CComPtr<IMgaMetaModel> meta)
{
	// FIXME: check if mgaMetaModel == meta
	if (mgaMetaModel != NULL)
		mgaMetaModel.Release();
	mgaMetaModel = meta;
	partFrame.GetPane().SetMetaModel(meta);

	SetTabs();
}

void CPartBrowserDlg::SetBgColor(COLORREF bgColor)
{
	partFrame.GetPane().SetBgColor(bgColor);
}

void CPartBrowserDlg::ChangeAspect(long ind)
{
	tab.SetCurSel(ind);
	partFrame.GetPane().ChangeAspect(ind);
	Resize();
}

void CPartBrowserDlg::CycleAspect()
{
	if (!mgaMetaModel)
		return;

	CComPtr<IMgaMetaParts> mgaMetaParts;
	try {
		CComPtr<IMgaMetaAspects> spAspects;
		COMTHROW(mgaMetaModel->get_Aspects(&spAspects));
		ASSERT(spAspects);
		if (spAspects) {
			long nAspects = 0;
			COMTHROW(spAspects->get_Count(&nAspects));
			if (nAspects <= 0)
				return;

			long ind = (tab.GetCurSel() + 1) % nAspects;
			tab.SetCurSel(ind);

			partFrame.GetPane().ChangeAspect(ind);
			SendAspectChange(ind);
		}
	}
	catch (hresult_exception&) {
	}

	Resize();
}

void CPartBrowserDlg::SetTabs()
{
	tab.DeleteAllItems();
	if (mgaMetaModel) {
		try {
			CComPtr<IMgaMetaAspects> spAspects;
			COMTHROW(mgaMetaModel->get_Aspects(&spAspects));
			ASSERT(spAspects);
			if (spAspects) {
				long nAspects = 0;
				COMTHROW(spAspects->get_Count(&nAspects));
				TC_ITEM tcItem;
				for (long ind = 0; ind < nAspects; ind++) {
					CComPtr<IMgaMetaAspect>	spAspect;
					COMTHROW(spAspects->get_Item(ind + 1, &spAspect));
					tcItem.mask = TCIF_TEXT;
					CComBSTR displayedNameBstr;
					COMTHROW(spAspect->get_DisplayedName(&displayedNameBstr));
					CComBSTR nameBStr;
					COMTHROW(spAspect->get_Name(&nameBStr));
					CString name;
					name = displayedNameBstr ? displayedNameBstr : nameBStr;
					tcItem.pszText = name.GetBuffer();
					tcItem.cchTextMax = name.GetLength();
					tab.InsertItem(ind, &tcItem);
				}
			}
		}
		catch (hresult_exception&) {
		}
	}
	Resize();
}

void CPartBrowserDlg::Resize()
{
	CRect r;
	GetClientRect(&r);
	if (::IsWindow(tab.m_hWnd))
		tab.MoveWindow(&r);
	if (::IsWindow(partFrame.m_hWnd)) {
		tab.AdjustRect(FALSE, &r);
		r.DeflateRect(1, 1);
		partFrame.Resize(r);
	}
}

BEGIN_MESSAGE_MAP(CPartBrowserDlg, CDialog)
	//{{AFX_MSG_MAP(CPartBrowserDlg)
	ON_WM_SIZE()
	ON_NOTIFY(TCN_SELCHANGE, IDC_ASPECT_TAB, OnSelchangeAspectTab)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPartBrowserDlg message handlers

BOOL CPartBrowserDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	tab.SetWindowPos(&wndBottom, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	partFrame.Create(IDD_PARTBROWSERFRAME_DIALOG, this);

	// Opportunity for some users to bring the tabs to the bottom again (TCS_BOTTOM is obsolete now)
	// TCS_BOTTOM style is obsolete (see MSDN: "This style is not supported if you use ComCtl32.dll version 6.")
	// this style appeared with Internet Explorer 3.x, ComCtl 6.0 is the Windows XP and Windows Vista version of ComCtl.
	// If user really wants to switch it back, he can force PartBrowser to use TCS_BOTTOM style by creating a
	// string registry key with "1" value under the "HKCU\Software\GME\GUI\PartBrowser\BottomTabs"
	HKEY hKey;
	if (RegOpenKeyEx(HKEY_CURRENT_USER, _T("Software\\GME\\GUI\\PartBrowser"),
					 0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS)
	{
		TCHAR szData[128];
		DWORD dwKeyDataType;
		DWORD dwDataBufSize = sizeof(szData)/sizeof(TCHAR);

		if (RegQueryValueEx(hKey, _T("BottomTabs"), NULL, &dwKeyDataType,
							(LPBYTE) &szData, &dwDataBufSize) == ERROR_SUCCESS)
		{
			UINT uBottomTabs = _tcstoul(szData, NULL, 10);
			if (uBottomTabs > 0) {
				LONG dwStyle = ::GetWindowLong(tab.m_hWnd, GWL_STYLE);
				dwStyle = dwStyle | TCS_BOTTOM;
				::SetWindowLong(tab.m_hWnd, GWL_STYLE, dwStyle);
			}
		}
		RegCloseKey(hKey);
	}

	SetTabs();

	return TRUE;	// return TRUE unless you set the focus to a control
					// EXCEPTION: OCX Property Pages should return FALSE
}

void CPartBrowserDlg::OnOK()
{
	// do nothing to avoid getting kick out
}

void CPartBrowserDlg::OnCancel()
{
	// do nothing to avoid getting kick out
}

void CPartBrowserDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	Resize();
}

void CPartBrowserDlg::OnSelchangeAspectTab(NMHDR* /*pNMHDR*/, LRESULT* pResult)
{
	long s = tab.GetCurSel();
	if (s >= 0) {
		partFrame.GetPane().ChangeAspect(s);
		SendAspectChange(s);
	}
	Resize();
	*pResult = 0;
}

void CPartBrowserDlg::SendAspectChange(long index)
{
	CWnd* wnd = GetParent();
	if (wnd->IsKindOf(RUNTIME_CLASS(CPartBrowserCtrl))) {
		CPartBrowserCtrl* ctrl = STATIC_DOWNCAST(CPartBrowserCtrl, wnd);
		ctrl->SendAspectChanged(index);
	}
}
