// CompDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MgaUtil.h"
#include "CompDlg.h"
#include "CompInfoDlg.h"
#include "UACUtils.h"
#include "Resource.h"

#include <atlbase.h>
#include <objbase.h>
#include <comdef.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCompDlg dialog


CCompDlg::CCompDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCompDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCompDlg)
	m_accessmode = 1;
	m_dispmode = 1;
	//}}AFX_DATA_INIT

	type = COMPONENTTYPE_ALL;
	onOKoper = _T("Close");

	firstResize = true;
}


void CCompDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCompDlg)
	DDX_Control(pDX, IDC_ENABLE_DISABLE, m_enable_disable);
	DDX_Control(pDX, IDC_REMOVE, m_remove);
	DDX_Control(pDX, IDC_INSTALL, m_install);
	DDX_Control(pDX, IDC_LIST, m_list);
	DDX_Radio(pDX, IDC_RADIOSYS, m_accessmode);
	DDX_Radio(pDX, IDC_ACTIVEDISP, m_dispmode);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCompDlg, CDialog)
	//{{AFX_MSG_MAP(CCompDlg)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	ON_BN_CLICKED(IDC_INSTALL, OnInstall)
	ON_BN_CLICKED(IDC_ACTIVEDISP, OnActivedisp)
	ON_BN_CLICKED(IDC_ACTIVE_INACTIVE, OnActiveInactive)
	ON_BN_CLICKED(IDC_ALLCOMPS, OnAllcomps)
	ON_BN_CLICKED(IDC_ENABLE_DISABLE, OnEnableDisable)
	//}}AFX_MSG_MAP
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_RADIOSYS, &CCompDlg::OnBnClickedRadiosys)
	ON_BN_CLICKED(IDC_RADIOUSER, &CCompDlg::OnBnClickedRadiouser)
	ON_BN_CLICKED(IDC_RADIOBOTH, &CCompDlg::OnBnClickedRadioboth)
	ON_NOTIFY(HDN_ITEMCLICK, 0, OnAllCompsHeader)
	ON_WM_GETMINMAXINFO()
END_MESSAGE_MAP()


struct SortParam {
	CCompDlg* dlg;
	int columnIndex;
};

int CALLBACK CCompDlg::SortFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort) {
	SortParam& param = *(SortParam*)lParamSort;
	
	CString a = param.dlg->m_list.GetItemText(lParam1, param.columnIndex);
	CString b = param.dlg->m_list.GetItemText(lParam2, param.columnIndex);

	return _tcscmp(a, b);
}


void CCompDlg::OnAllCompsHeader(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NMLISTVIEW *pLV = (NMLISTVIEW *) pNMHDR;
	SortParam s = { this, pLV->iItem };
	m_list.SortItemsEx(SortFunc, (DWORD_PTR)(void*)&s);
	
	*pResult = 0;
}


/////////////////////////////////////////////////////////////////////////////
// CCompDlg message handlers

BOOL CCompDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	MSGTRY
	{
		if( !CUACUtils::isVistaOrLater() ) {
			CRegKey accessTest;
			if (accessTest.Open(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\GME"), KEY_READ | KEY_WRITE) == ERROR_ACCESS_DENIED) {
				GetDlgItem(IDC_RADIOSYS)->EnableWindow(false);
				GetDlgItem(IDC_RADIOBOTH)->EnableWindow(false);
			}
		}
		

		SetDlgItemText(IDOK, onOKoper);
		

		LV_COLUMN lvc;
		lvc.mask = LVCF_WIDTH | LVCF_TEXT;

		lvc.pszText = _T("Name");
		lvc.cx = (int)(150.0 * GetDC()->GetDeviceCaps(LOGPIXELSY) / 96.0);;
		VERIFYTHROW( m_list.InsertColumn(0, &lvc) != -1 );

		lvc.pszText = _T("Type");
		lvc.cx = (int)(80.0 * GetDC()->GetDeviceCaps(LOGPIXELSY) / 96.0);
		VERIFYTHROW( m_list.InsertColumn(1, &lvc) != -1 );

		lvc.pszText = _T("ProgID");
		lvc.cx = (int)(200.0 * GetDC()->GetDeviceCaps(LOGPIXELSY) / 96.0);
		VERIFYTHROW( m_list.InsertColumn(2, &lvc) != -1 );

		lvc.pszText = _T("Path");
		lvc.cx = (int)(300.0 * GetDC()->GetDeviceCaps(LOGPIXELSY) / 96.0);
		VERIFYTHROW( m_list.InsertColumn(3, &lvc) != -1 );

		iconlist.Create(IDB_BITMAP1,16,0,15);
		m_list.SetImageList(&iconlist,LVSIL_SMALL);
		if(parameter.vt == VT_BSTR) {
			sticonlist.Create(IDB_BITMAP2,10,0,15);
			m_list.SetImageList(&sticonlist,LVSIL_STATE);
		}
		UpdateData(false);
		ResetItems();

		RefreshShieldIcons();
		
	}
	MSGCATCH(_T("Error while initializing CompDlg"),;)

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCompDlg::RefreshShieldIcons()
{
	if( CUACUtils::isVistaOrLater() ) {
		UpdateData(TRUE);
		bool shieldOn = regacc_translate(m_accessmode) != REGACCESS_USER;
		CUACUtils::SetShieldIcon(m_install, true);
		CUACUtils::SetShieldIcon(m_remove, true);
		CUACUtils::SetShieldIcon(m_enable_disable, shieldOn);
	}
}

void CCompDlg::ResetItems()
{
	CComPtr<IMgaRegistrar> registrar;
	registrar.CoCreateInstance(OLESTR("Mga.MgaRegistrar"));
	ASSERT( registrar != NULL );

	m_remove.EnableWindow(false);
	m_enable_disable.EnableWindow(false);

	UpdateData();
	VERIFYTHROW( m_list.DeleteAllItems() != 0 );

	CComVariant v;
	if(m_dispmode == 0) {
		COMTHROW( registrar->get_AssociatedComponents(PutInBstr(paradigm), type, REGACCESS_BOTH, PutOut(v)) );
	}
	else {
		COMTHROW( registrar->get_Components(REGACCESS_BOTH, PutOut(v)) );
	}

	CComBstrObjArray progids;
	MoveTo(v, progids);



	for(int i = 0; i < progids.GetSize(); ++i)
	{
		componenttype_enum qtype;
		CComBstrObj desc;
		CComBstrObj localDllDispPath;
		CString localDllDispPathStr;

		bool err_ass = false;
		VARIANT_BOOL is_ass, can_ass;
		HRESULT hr = registrar->QueryComponent(progids[i], &qtype, PutOut(desc), REGACCESS_PRIORITY );
		if(hr != S_OK) {
			err_ass = true;
			desc = CComBstrObj(L"???");
			localDllDispPath = CComBstrObj(L"???");
			qtype = COMPONENTTYPE_NONE;
		}
		else {
			if(! (qtype & type) ) continue;
			if(m_dispmode) {
				HRESULT hr = registrar->IsAssociated(progids[i], PutInBstr(paradigm), &is_ass, &can_ass, REGACCESS_PRIORITY );
				if(hr == E_NOTFOUND) {
					err_ass = true;
				}
				else COMTHROW(hr);
				if(m_dispmode == 1 && (is_ass == VARIANT_FALSE && can_ass == VARIANT_FALSE)) continue;
			}
			else is_ass = can_ass = VARIANT_TRUE;

			HRESULT hr = registrar->GetLocalDllPathDisp(progids[i], PutOut(localDllDispPath));
                        if (SUCCEEDED(hr))
                                localDllDispPathStr = (const TCHAR*)PutInCString(localDllDispPath);
/*#define BUFSIZE 1024
			TCHAR buffer[BUFSIZE]=TEXT("");
			DWORD retval = GetFullPathName(localDllDispPathStr, BUFSIZE, buffer, NULL);
			if (retval != 0)
			{
				localDllDispPathStr = buffer;
			} TODO: inspect this later*/
		}

		int index;
		VERIFYTHROW( (index = m_list.InsertItem(i, PutInCString(desc), err_ass ? 3 : is_ass ? 0 : can_ass ? 1 : 2)) != -1 );


		CString ctype;
		switch(qtype & COMPONENTTYPE_ALL)
		{
		case COMPONENTTYPE_INTERPRETER:
			ctype = _T("Interpreter");
			break;

		case COMPONENTTYPE_ADDON:
			ctype = _T("Add-on");

			if(parameter.vt == VT_BSTR) {
				CComBSTR l = parameter.bstrVal;
				WCHAR * f = wcstok(l,L" ");
				while(f) {
					if(!desc.Compare(CComBSTR(f)) || !progids[i].Compare(CComBSTR(f))) {
						// FIXME: this component is running, would be nice to show to the user, but an exclamation mark means nothing
						// m_list.SetItemState(index, INDEXTOSTATEIMAGEMASK(2), LVIS_STATEIMAGEMASK);
						break;
					}
					f = wcstok(NULL, L" ");
				}
			}
			break;

		case COMPONENTTYPE_PLUGIN:
			ctype = _T("Plug-in");
			break;

		default:
			ctype = _T("???");
		}

		VERIFYTHROW( m_list.SetItemText(index, 1, ctype) != 0 );
		VERIFYTHROW( m_list.SetItemText(index, 2, PutInCString(progids[i])) != 0 );
		VERIFYTHROW( m_list.SetItemText(index, 3, localDllDispPathStr) != 0 );
		if(!desc.Compare( PutInBstr(to_select))) {
			m_list.SetItemState(index, LVIS_SELECTED, LVIS_SELECTED);
		}
	}
	UpdateEnableDisable();
}

void CCompDlg::UpdateEnableDisable() {
	POSITION pos = m_list.GetFirstSelectedItemPosition();
	if (pos != NULL) {
		int i = m_list.GetNextSelectedItem(pos);
		CString progid = m_list.GetItemText(i, 2);
		CComPtr<IMgaRegistrar> registrar;
		registrar.CoCreateInstance(L"Mga.MgaRegistrar");
		ASSERT(registrar != NULL);
		VARIANT_BOOL is_ass, can_ass;
		HRESULT hr = registrar->IsAssociated(PutInBstr(progid), PutInBstr(paradigm), &is_ass, &can_ass, REGACCESS_PRIORITY);
		ASSERT(SUCCEEDED(hr));
		if (SUCCEEDED(hr) && is_ass == VARIANT_TRUE) {
			m_enable_disable.SetWindowText(_T("Disable"));
			return;
		}
	}
	m_enable_disable.SetWindowText(_T("Enable"));
}

void CCompDlg::OnOK() 
{
	POSITION pos = m_list.GetFirstSelectedItemPosition();
	if( pos != NULL )
	{
		int i = m_list.GetNextSelectedItem(pos);
		progid = m_list.GetItemText(i, 2);
	}
	else
	{
		progid.Empty();
	}

	CDialog::OnOK();
}

BOOL CCompDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	if( wParam == IDC_LIST && ((NMHDR*)lParam)->code == LVN_ITEMCHANGED &&
		((NMLISTVIEW*)lParam)->uNewState == (LVIS_FOCUSED|LVIS_SELECTED))
	{
		POSITION pos = m_list.GetFirstSelectedItemPosition();
		m_remove.EnableWindow(pos != NULL);
		m_enable_disable.EnableWindow(pos != NULL);
		to_select = m_list.GetItemText(m_list.GetNextSelectedItem(pos),0);
		UpdateEnableDisable();
		return TRUE;
	} else if (wParam == IDC_LIST && ((NMHDR*)lParam)->code == NM_DBLCLK )
	{
		if (onOKoper == _T("Interpret..."))
		{
			OnOK();
			return TRUE;
		} else {
			OnEnableDisable();
			return TRUE;
		}
	}
	
	return CDialog::OnNotify(wParam, lParam, pResult);
}

void CCompDlg::OnRemove() 
{
	UpdateData();
	MSGTRY
	{
		POSITION pos = m_list.GetFirstSelectedItemPosition();
		if( pos != NULL )
		{
			CString progid = m_list.GetItemText(m_list.GetNextSelectedItem(pos), 2);

			CComPtr<IMgaRegistrar> registrar;
			HRESULT registrarHr;
			if (CUACUtils::isVistaOrLater()) {
				registrarHr = GetElevatedRegistrar(&registrar);
			}
			else {
				registrarHr = registrar.CoCreateInstance(OLESTR("Mga.MgaRegistrar"));
			}

			if (!registrar) {
				DisplayError(_T("Unable to remove component"), registrarHr);
				return;
			}

			COMTHROW( registrar->UnregisterComponent(PutInBstr(progid), regacc_translate(m_accessmode)) );

			componenttype_enum type;

			CComBstrObj desc;
			switch(regacc_translate(m_accessmode)) {
				case REGACCESS_USER:
					if (S_OK == registrar->QueryComponent(PutInBstr(progid), &type, PutOut(desc), REGACCESS_SYSTEM)) {
						AfxMessageBox(_T("Warning: Component is still present in system registry"));
					}
					break;
				case REGACCESS_SYSTEM:
					if (S_OK == registrar->QueryComponent(PutInBstr(progid), &type, PutOut(desc), REGACCESS_USER)) {
						AfxMessageBox(_T("Warning: Component is still present in user registry"));
					}
					break;
			}
			ResetItems();
		}
	}
	MSGCATCH(_T("Error while removing component"),;)
}


void CCompDlg::OnEnableDisable() 
{
	UpdateData();
	MSGTRY
	{
		POSITION pos = m_list.GetFirstSelectedItemPosition();
		if( pos != NULL ) {
			CComBSTR progid = m_list.GetItemText(m_list.GetNextSelectedItem(pos), 2);

			VARIANT_BOOL is_ass, can_ass;
			
			CComPtr<IMgaRegistrar> registrar;
			HRESULT registrarHr;
			if (CUACUtils::isVistaOrLater() && (regacc_translate(m_accessmode) != REGACCESS_USER) ) {
				registrarHr = GetElevatedRegistrar(&registrar);
			}
			else {
				registrarHr = registrar.CoCreateInstance(OLESTR("Mga.MgaRegistrar"));
			}

			if (!registrar) {
				DisplayError(_T("Unable to enable/disable component"), registrarHr);
				return;
			}

			HRESULT hr = registrar->IsAssociated(progid, PutInBstr(paradigm), &is_ass, &can_ass, REGACCESS_PRIORITY);
			if (hr != S_OK)
				DisplayError(_T("Cannot enable/disable this component"), hr);
			CString enable_or_disable;
			if (is_ass) {
				enable_or_disable = _T("disable");
				hr = registrar->Disassociate(progid, PutInBstr(paradigm), regacc_translate(m_accessmode));
			} else {
				enable_or_disable = _T("enable");
				if(!can_ass) {
					if(AfxMessageBox(_T("This component reports to be incompatible with the paradigm\nAre you sure you want to proceed?"), MB_YESNO) != IDYES) return;
				}
				hr = (registrar->Associate(progid, PutInBstr(paradigm), regacc_translate(m_accessmode)) );
			}
			if(hr != S_OK) {
				DisplayError(_T("The ") + enable_or_disable + _T(" operation failed"), hr);
			}

			ResetItems();
		}
	}
	MSGCATCH(_T("Error while (dis)associating component"),;)
}

static TCHAR filter[] = 
	_T("Component Files (*.dll; *.ocx)|*.dll; *.ocx|")
	_T("Pattern Files (*.pat)|*.pat|")
	_T("All Files (*.*)|*.*||");

void CCompDlg::OnInstall() 
{
	UpdateData();
	MSGTRY
	{
		CFileDialog dlg(true, NULL, NULL, 
			OFN_EXPLORER | OFN_HIDEREADONLY | OFN_FILEMUSTEXIST,
			filter);

		if( dlg.DoModal() != IDOK )
			return;

		CString ext = dlg.GetFileExt();

		if(ext.CompareNoCase(_T("DLL")) == 0) {
				RegisterDll(dlg.GetPathName());
		}
		else if(ext.CompareNoCase(_T("PAT")) == 0){
				RegisterPattern(dlg.GetPathName());
		}
		else {
				RegisterUnknownFile(dlg.GetPathName());
		}
		ResetItems();
	}
	MSGCATCH(_T("Error while installing component"),;)
}

void CCompDlg::RegisterDll(const CString &path)
{
	CComPtr<IMgaRegistrar> registrar;
	HRESULT hr;
	if (CUACUtils::isVistaOrLater()) {
		hr = GetElevatedRegistrar(&registrar);
	} else {
		hr = registrar.CoCreateInstance(OLESTR("Mga.MgaRegistrar"));
	}

	if (!registrar) {
		DisplayError(_T("Unable to create component registrar"), hr);
		return;
	}


	MSGTRY
	{
		hr = registrar->__RegisterComponentLibrary(_bstr_t(path), regacc_translate(m_accessmode));
	}
	MSGCATCH(_T("Unable to register component"),;)
}

HRESULT CCompDlg::GetElevatedRegistrar(IMgaRegistrar** registrar)
{
	HRESULT hr = S_OK;
	if (m_elevatedRegistrar == NULL)
		hr = CUACUtils::CreateElevatedInstance(__uuidof(MgaRegistrar), &m_elevatedRegistrar);
	if (m_elevatedRegistrar != NULL)
		m_elevatedRegistrar.CopyTo(registrar);
	return hr;
}


void CCompDlg::RegisterPattern(const CString &path)
{

	// FIXME: should use TCHAR
	char buf[300];
	{
		// FIXME: will this read a UTF-16 file?
		std::ifstream fin(path, std::ios::in);//z! ios::nocreate used previously, but if opened for read it won't create in this way
		if(fin.good()) fin.getline(buf, 300);
		if(!fin.good()) {
			AfxMessageBox(_T("Could not open or read the specified file: ") + path);
			return;
		}
	}

	regaccessmode_enum acmode = regacc_translate(m_accessmode);
	CComPtr<IMgaRegistrar> registrar;
	HRESULT registrarHr;
	if (CUACUtils::isVistaOrLater() && (acmode != REGACCESS_USER) ) {
		registrarHr = GetElevatedRegistrar(&registrar);
	}
	else {
		registrarHr = registrar.CoCreateInstance(OLESTR("Mga.MgaRegistrar"));
	}

	if (!registrar) {
		DisplayError(_T("Unable to register pattern component"), registrarHr);
		return;
	}

	CCompInfoDlg dlg(registrar);
	dlg.m_filename = path;

	TCHAR paren, info[200] = {0};
	if (swscanf_s(CString(buf), _T(" $!COMMENT( %[^)]%lc"), info, (unsigned)(sizeof(info)/sizeof(info[0]))-1, &paren, 1) != 2 || paren != ')') {
nothing_understood:
		AfxMessageBox("Cannot read component info in file " + path);
	}
	else {
	
	
// NAME=nnn, DESCRIPTION=ddd, PROGID=ii, PARADIGM=xx,yy, VERSION=vvv,
		TCHAR *p = _tcstok(info, _T(","));
		bool ready = false;
		bool parsing_paradigm = false;
		bool good = false;
		while(p) {
			CString item(p);
			item.TrimLeft();
			item.TrimRight();
			int k1 = item.Find(';');
			if(ready && !item.IsEmpty()) goto item_err;
			if(k1 > 0) {
				ready = true;
				item = item.Left(k1);
				item.TrimRight();
			}
			k1 = item.Find('=');
			if(k1 > 0) {
				parsing_paradigm = false;
				CString i_name = item.Left(k1);
				i_name.TrimRight();
				CString i_value = item.Mid(k1+1);
				i_value.TrimLeft();
				if(i_name.Compare(_T("NAME")) == 0) {
					dlg.m_name = i_value;
				}
				else if(i_name.Compare(_T("DESCRIPTION")) == 0 ||
					i_name.Compare(_T("DESC")) == 0) {
					dlg.m_description = i_value;
				}
				else if(i_name.Compare(_T("PROGID")) == 0) {
					dlg.m_progid = i_value;
				}
				else if(i_name.Compare(_T("PARADIGM")) == 0) {
					dlg.m_paradigm = i_value;
					parsing_paradigm  = true;
				}
				else if(i_name.Compare(_T("VERSION")) == 0) {
					dlg.m_version = i_value;
				}
				else goto item_err;
				good = true;
			}
			else {
				if(parsing_paradigm) {
					dlg.m_paradigm += _T(",") + item;
				}
				else {
	item_err:
					AfxMessageBox(_T("Syntax error parsing component info: ") + item); 
				}
			}
			p = _tcstok(NULL, _T(","));
		}
		if(!good) goto nothing_understood;
	}

	if(dlg.m_progid.IsEmpty() && !dlg.m_name.IsEmpty()) {
		dlg.m_progid = _T("MGA.Pattern.") + dlg.m_name;
	}
	if(dlg.m_description.IsEmpty() && !dlg.m_name.IsEmpty()) {
		dlg.m_description = dlg.m_name;
	}
	if(dlg.m_version.IsEmpty()) {
		dlg.m_version = _T("1.0");
	}

	if(dlg.DoModal() != IDOK) return;


	CComBSTR progid(dlg.m_progid);

	
	COMTHROW(registrar->RegisterComponent(progid, 
				(componenttype_enum)(COMPONENTTYPE_INTERPRETER|COMPONENTTYPE_SCRIPT),
				PutInBstr(dlg.m_description), acmode));
	COMTHROW(registrar->put_ComponentExtraInfo(acmode, progid, CComBSTR(L"Name"), CComBSTR(dlg.m_name)));
	COMTHROW(registrar->put_ComponentExtraInfo(acmode, progid, CComBSTR(L"ExecEngine"), CComBSTR(dlg.engine)));
	COMTHROW(registrar->put_ComponentExtraInfo(acmode, progid, CComBSTR(L"ScriptFile"), CComBSTR(path)));
	COMTHROW(registrar->put_ComponentExtraInfo(acmode, progid, CComBSTR(L"ScriptVersion"), CComBSTR(dlg.m_version)));

	std::unique_ptr<TCHAR[]> mpardup(new TCHAR[dlg.m_paradigm.GetLength()+1]);
	_tcscpy(mpardup.get(), dlg.m_paradigm);
	const TCHAR *par = _tcstok(mpardup.get(), _T("\t ,"));
	while(par) {
		COMTHROW(registrar->Associate(progid, CComBSTR(par), acmode));
		par = _tcstok(NULL, _T("\t ,"));
	}
}

void CCompDlg::RegisterUnknownFile(const CString &path)
{

}

void CCompDlg::OnActivedisp() 
{
	ResetItems();
}

void CCompDlg::OnActiveInactive() 
{
	ResetItems();
}

void CCompDlg::OnAllcomps() 
{
	ResetItems();
}


void CCompDlg::OnSize(UINT nType, int cx, int cy)
{
	long deltaWidth = 0;
	long deltaHeight = 0;

	if (firstResize) {
		CRect rectInResource = GetWindowSizeFromResource();
		CRect currentDlgRect;
		deltaWidth = cx - rectInResource.Width();
		deltaHeight = cy - rectInResource.Height();
		firstResize = false;
	} else {
		deltaWidth = cx - lastRect.Width();
		deltaHeight = cy - lastRect.Height();
	}

	CDialog::OnSize(nType, cx, cy);
	GetClientRect(&lastRect);

	HDWP dwp = NULL;
	bool defer = (deltaHeight != 0);
	if (defer)
		dwp = ::BeginDeferWindowPos(13);

	MoveControl(IDC_LIST,					0, 0, deltaWidth, deltaHeight, defer, &dwp);
	if (defer) {
		MoveControl(IDC_REMOVE,					0, deltaHeight, 0, 0, defer, &dwp);
		MoveControl(IDC_INSTALL,				0, deltaHeight, 0, 0, defer, &dwp);
        MoveControl(IDC_ENABLE_DISABLE,                 0, deltaHeight, 0, 0, defer, &dwp);
		MoveControl(IDC_RADIOSYS,				0, deltaHeight, 0, 0, defer, &dwp);
		MoveControl(IDC_RADIOUSER,				0, deltaHeight, 0, 0, defer, &dwp);
		MoveControl(IDC_RADIOBOTH,				0, deltaHeight, 0, 0, defer, &dwp);
		MoveControl(IDC_REGISTERSTATIC,			0, deltaHeight, 0, 0, defer, &dwp);
		MoveControl(IDOK,						0, deltaHeight, 0, 0, defer, &dwp);
	}

	if (defer) {
		ASSERT(dwp != NULL);
		if (dwp != NULL) {
			BOOL success = ::EndDeferWindowPos(dwp);
			ASSERT(success == TRUE);
		}
	}
}

void CCompDlg::OnBnClickedRadiosys()
{
	RefreshShieldIcons();
}

void CCompDlg::OnBnClickedRadiouser()
{
	RefreshShieldIcons();
}

void CCompDlg::OnBnClickedRadioboth()
{
	RefreshShieldIcons();
}

// CodeGuru: Finding Display Size of Dialog From Resource
//		by Shridhar Guravannavar
CRect CCompDlg::GetWindowSizeFromResource(void) const
{
	CRect rectSize;

	// if the dialog resource resides in a DLL ...
	//

	HINSTANCE hInst = AfxFindResourceHandle(MAKEINTRESOURCE(IDD), RT_DIALOG);

	ASSERT(hInst != NULL);

	HRSRC hRsrc = ::FindResource(hInst, MAKEINTRESOURCE(IDD), RT_DIALOG);
	ASSERT(hRsrc != NULL);

	HGLOBAL hTemplate = ::LoadResource(hInst, hRsrc);
	ASSERT(hTemplate != NULL);

	DLGTEMPLATE* pTemplate = (DLGTEMPLATE*)::LockResource(hTemplate);

	//Load coresponding DLGINIT resource
	//(if we have any ActiveX components)
	//
	void* lpDlgInit;
	HGLOBAL hDlgInit = NULL;
	CDialog dlg;

	HRSRC hsDlgInit = ::FindResource(hInst, MAKEINTRESOURCE(IDD), RT_DLGINIT);
	if (hsDlgInit != NULL) {
		// load it
		hDlgInit = ::LoadResource(hInst, hsDlgInit);
		ASSERT(hDlgInit != NULL);

		// lock it
		lpDlgInit = ::LockResource(hDlgInit);
		ASSERT(lpDlgInit != NULL);

		dlg.CreateIndirect(pTemplate, NULL, lpDlgInit);
	} else {
		dlg.CreateIndirect(pTemplate, NULL);
	}

	CRect rect;
	dlg.GetClientRect(rectSize);

	dlg.DestroyWindow();

	::UnlockResource(hTemplate);
	::FreeResource(hTemplate);
	if (hDlgInit) {
		::UnlockResource(hDlgInit);
		::FreeResource(hDlgInit);
	}

	return rectSize;
}

void CCompDlg::MoveControl(int nID, int offsetX, int offsetY, int deltaWidth, int deltaHeight, bool defer, HDWP* pdwp)
{
	CWnd* controlWnd = GetDlgItem(nID);
	if (controlWnd != NULL && (offsetX != 0 || offsetY != 0 || deltaWidth != 0 || deltaHeight != 0)) {
		CRect controlClientRect;
		controlWnd->GetClientRect(&controlClientRect);
		CRect controlWindowRect;
		controlWnd->GetWindowRect(&controlWindowRect);
		ScreenToClient(&controlWindowRect);

		if (defer) {
			ASSERT(pdwp != NULL);
			if (*pdwp != NULL)
				*pdwp = ::DeferWindowPos(*pdwp, controlWnd->m_hWnd, NULL,
										 controlWindowRect.left + offsetX, controlWindowRect.top + offsetY,
										 controlWindowRect.Width() + deltaWidth, controlWindowRect.Height() + deltaHeight,
										 SWP_NOZORDER /*| SWP_NOCOPYBITS*/);
		} else {
			controlWnd->SetWindowPos(NULL, controlWindowRect.left + offsetX, controlWindowRect.top + offsetY,
									 controlWindowRect.Width() + deltaWidth, controlWindowRect.Height() + deltaHeight,
									 SWP_NOZORDER);
		}
	}
}


void CCompDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	lpMMI->ptMinTrackSize.x = 800;
	lpMMI->ptMinTrackSize.y = 380;
}
