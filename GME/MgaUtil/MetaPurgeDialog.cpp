// MetaPurgeDialog.cpp : implementation file
//

#include "stdafx.h"
#include <algorithm>
#include "mgautil.h"
#include "MetaPurgeDialog.h"
#include "atlsafe.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMetaPurgeDialog dialog

int CheckParadigms(BSTR paradigm, bool remove_invalid_entries)
{
	int num_invalid = 0;
	IMgaRegistrarPtr registrar;
	COMTHROW(registrar.CreateInstance(__uuidof(MgaRegistrar)));
	CComVariant vguids;
	registrar->__QueryParadigmAllGUIDs(paradigm, PutOut(vguids), REGACCESS_BOTH);
	if (vguids.vt != (VT_ARRAY | VT_BSTR))
		throw E_INVALIDARG;
	ATL::CComSafeArray<BSTR> guids(vguids.parray);

	CComBstrObj currentguid;
	{
		CComBSTR dummy;
		CComVariant vv;
		registrar->__QueryParadigm(paradigm, &dummy, &vv, REGACCESS_PRIORITY);
		GUID guid;
		CopyTo(vv, guid);
		CopyTo(guid, currentguid);
	}

	for (int i = guids.GetLowerBound(); i <= guids.GetUpperBound(); ++i)	{
		GUID gg;
		CopyTo(guids[i], gg);
		CComVariant vRegGuid;
		CopyTo(gg, vRegGuid);

		_bstr_t connstr;
		_bstr_t version;
		TCHAR statc = 'u';
		HRESULT hr = registrar->QueryParadigm(paradigm, connstr.GetAddress(), &vRegGuid, REGACCESS_USER);
		if(hr == E_NOTFOUND) {
			statc = 's';
			hr = registrar->QueryParadigm(paradigm, connstr.GetAddress(), &vRegGuid, REGACCESS_SYSTEM);
		} 
		if(hr != S_OK) {
			statc = ' ';
			connstr = _T("<<error reading this reg.entry>>");
		}

		hr = registrar->VersionFromGUID(paradigm, vRegGuid, version.GetAddress(), statc ==  'u' ? REGACCESS_USER : REGACCESS_SYSTEM);
		if (FAILED(hr)) {
			version = _T("N/A");
		}

		bool valid = false;
		IMgaMetaProjectPtr project;
		COMTHROW(project.CreateInstance(__uuidof(MgaMetaProject)));
		try
		{
			project->__Open(connstr);
			try
			{
				if (project->Version != version)
					throw _com_error(E_PROJECT_MISMATCH);
				_variant_t vFileGUID = project->GUID;
				if (vFileGUID.vt != (VT_ARRAY | VT_UI1))
					throw _com_error(E_PROJECT_MISMATCH);
				ATL::CComSafeArray<BYTE> fileGUID(vFileGUID.parray);
				if (fileGUID.GetCount() != sizeof(GUID))
					throw _com_error(E_PROJECT_MISMATCH);
				GUID gFileGUID;
				CopyTo(vFileGUID, gFileGUID);
				if (gFileGUID != gg)
					throw _com_error(E_PROJECT_MISMATCH);
				valid = true;
			}
			catch (_com_error&)
			{
				project->__Close();
				throw;
			}
		}
		catch (_com_error& e)
		{
		}
		if (valid == false)
		{
			num_invalid++;
			if (remove_invalid_entries)
			{
				registrar->UnregisterParadigmGUID(paradigm, vRegGuid, statc ==  'u' ? REGACCESS_USER : REGACCESS_SYSTEM);
			}
		}
	}
	return num_invalid;
}

void CMetaPurgeDialog::OnCheckFiles()
{
	int invalid = CheckParadigms(_bstr_t(paradigm), false);
	if (invalid > 0)
	{
		wchar_t msg[256];
		swprintf_s(msg, L"Found %d invalid registry entries. Do you want to remove them?", invalid);
		if (IDYES == AfxMessageBox(msg, MB_YESNO))
		{
			CheckParadigms(_bstr_t(paradigm), true);
			ResetItems();
		}
	}
}

CMetaPurgeDialog::CMetaPurgeDialog(CString &paradigmname,  IMgaRegistrar *reg, CWnd* pParent /*=NULL*/)
	: CDialog(CMetaPurgeDialog::IDD, pParent), paradigm(paradigmname), registrar(reg)
{
	//{{AFX_DATA_INIT(CMetaPurgeDialog)
	m_delfiles = -1;
	//}}AFX_DATA_INIT
}


void CMetaPurgeDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMetaPurgeDialog)
	DDX_Control(pDX, IDC_PURGELIST, m_list);
	DDX_Radio(pDX, IDC_DELFILES, m_delfiles);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMetaPurgeDialog, CDialog)
	//{{AFX_MSG_MAP(CMetaPurgeDialog)
	ON_WM_CANCELMODE()
	ON_BN_CLICKED(IDC_PURGE2, OnPurge)
	ON_BN_CLICKED(IDC_SETCURRENT, OnSetcurrent)
	ON_BN_CLICKED(IDC_CHECK_FILES, OnCheckFiles)
	ON_BN_CLICKED(IDOK, OnOK)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_PURGELIST, OnParadigmsHeader)
	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMetaPurgeDialog message handlers

struct SortParam {
	CListCtrl& list;
	int columnIndex;
};

int CALLBACK CMetaPurgeDialog::SortFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort) {
	SortParam& param = *(SortParam*)lParamSort;
	
	CString a = param.list.GetItemText(static_cast<int>(lParam1), param.columnIndex);
	CString b = param.list.GetItemText(static_cast<int>(lParam2), param.columnIndex);

	return _tcsicmp(a, b);
}


void CMetaPurgeDialog::OnParadigmsHeader(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NMLISTVIEW *pLV = (NMLISTVIEW *) pNMHDR;
	SortParam s = { this->m_list, pLV->iSubItem };
	m_list.SortItemsEx(SortFunc, (DWORD_PTR)(void*)&s);
	
	*pResult = 0;
}


BOOL CMetaPurgeDialog::OnInitDialog() 
{
	m_delfiles = 0;
	CDialog::OnInitDialog();

	CListCtrl* listctrl = (CListCtrl*)GetDlgItem(IDC_PURGELIST);
    LRESULT dwStyle = listctrl->SendMessage(LVM_GETEXTENDEDLISTVIEWSTYLE,0,0);
    dwStyle |= LVS_EX_FULLROWSELECT;
    listctrl->SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, dwStyle);

	MSGTRY
	{
		ASSERT( registrar != NULL );

		LV_COLUMN lvc;
		lvc.mask = LVCF_WIDTH | LVCF_TEXT;

		lvc.pszText = _T("S");
		lvc.cx = 20;
		VERIFYTHROW( m_list.InsertColumn(0, &lvc) != -1 );

		lvc.pszText = _T("Version");
		lvc.cx = 50;
		VERIFYTHROW( m_list.InsertColumn(1, &lvc) != -1 );

		lvc.pszText = _T("GUID");
		lvc.cx = 250;
		VERIFYTHROW( m_list.InsertColumn(2, &lvc) != -1 );

		lvc.pszText = _T("Connection String");
		lvc.cx = 350;
		VERIFYTHROW( m_list.InsertColumn(3, &lvc) != -1 );

		ResetItems();

		listctrl->SetColumnWidth(1,LVSCW_AUTOSIZE);
		listctrl->SetColumnWidth(2,LVSCW_AUTOSIZE);
		CRect rect;
		listctrl->GetClientRect(&rect);
		int width = std::max(300, rect.Width() - (listctrl->GetColumnWidth(0) + listctrl->GetColumnWidth(1) + listctrl->GetColumnWidth(2)));
		listctrl->SetColumnWidth(3, width);
	}
	MSGCATCH(_T("Error while initializing MetaPurgeDlg"),;)
		
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMetaPurgeDialog::OnCancelMode() 
{
	CDialog::OnCancelMode();
}

void CMetaPurgeDialog::ResetItems()
{
	ASSERT( registrar != NULL );

	VERIFYTHROW( m_list.DeleteAllItems() != 0 );

	CComVariant v;
	COMTHROW( registrar->QueryParadigmAllGUIDs(CComBSTR(paradigm), PutOut(v), REGACCESS_BOTH) );

	CComBstrObjArray guidnames;
	MoveTo(v, guidnames);


	CComBstrObj currentguid;
	{
		CComBSTR dummy;
		CComVariant vv;
		COMTHROW( registrar->QueryParadigm(CComBSTR(paradigm), &dummy, &vv, REGACCESS_PRIORITY ));
		GUID guid;
		CopyTo(vv, guid);
		CopyTo(guid, currentguid);
	}

	for(int i = 0; i < guidnames.GetSize(); ++i)	{
		CString name;
		CopyTo(guidnames[i], name);

		GUID gg;
		CopyTo(guidnames[i],gg);
		CComVariant vv;
		CopyTo(gg, vv);

		CString connstr;
		CString version;
		TCHAR statc = 'u';
		HRESULT hr = registrar->QueryParadigm(CComBSTR(paradigm), PutOut(connstr), &vv, REGACCESS_USER);
		if(hr == E_NOTFOUND) {
			statc = 's';
			hr = registrar->QueryParadigm(CComBSTR(paradigm), PutOut(connstr), &vv, REGACCESS_SYSTEM);
		} 
		if(hr != S_OK) {
			statc = ' ';
			connstr = _T("<<error reading this reg.entry>>");
		}

		hr = registrar->VersionFromGUID(CComBSTR(paradigm), vv, PutOut(version), statc ==  'u' ? REGACCESS_USER : REGACCESS_SYSTEM);
		if (FAILED(hr)) {
			version = _T("N/A");
		}

		int j;
		VERIFYTHROW( (j = m_list.InsertItem(i, CString(currentguid == guidnames[i] ? _T("*") : _T(" ")) + statc)) != -1 );
		VERIFYTHROW( m_list.SetItemText(j, 1, version) != 0 );
		VERIFYTHROW( m_list.SetItemText(j, 2, PutInCString(guidnames[i])) != 0 );
		VERIFYTHROW( m_list.SetItemText(j, 3, connstr) != 0 );

	}

}

void CMetaPurgeDialog::OnPurge() 
{
	UpdateData();
	MSGTRY
	{
		POSITION pos = m_list.GetFirstSelectedItemPosition();

		while(pos) {
			int i = m_list.GetNextSelectedItem(pos);
			CString cur = m_list.GetItemText(i,0);
			if(cur[0] == '*') continue;
			CString delguid = m_list.GetItemText(i,2);
			GUID gg;
			CopyTo(CComBSTR(delguid),gg);
			CComVariant vv;
			CopyTo(gg, vv);
			COMTHROW( registrar->UnregisterParadigmGUID(CComBSTR(paradigm), vv, cur[1] == 's' ? REGACCESS_SYSTEM : REGACCESS_USER) );
			CString delcstr = m_list.GetItemText(i,3);
			if(m_delfiles != 0 || delcstr.Find(_T("MGA=")) != 0) continue;
			DeleteFile(LPCTSTR(delcstr)+4);
		}

		ResetItems();
	}
	MSGCATCH(_T("Error while removing items"),;)
}

void CMetaPurgeDialog::OnSetcurrent() 
{
	MSGTRY
	{
		POSITION pos = m_list.GetFirstSelectedItemPosition();

		if(pos) {
			int i = m_list.GetNextSelectedItem(pos);
			CString cur = m_list.GetItemText(i,0);
			if(cur[0] == '*') return;
			CString setver = m_list.GetItemText(i,1);
			CString setguid = m_list.GetItemText(i,2);
			GUID gg;
			CopyTo(CComBSTR(setguid),gg);
			CComVariant vv;
			CopyTo(gg, vv);
			CString setcstr = m_list.GetItemText(i,3);
			if (setver == L"N/A")
				setver = L"";
			COMTHROW( registrar->RegisterParadigm(CComBSTR(paradigm), CComBSTR(setcstr), CComBSTR(setver), vv, 
				cur[1] == 's' ? REGACCESS_SYSTEM : REGACCESS_USER) );
		}

		ResetItems();
	}
	MSGCATCH(_T("Error while setting current version"),;)
}

void CMetaPurgeDialog::OnOK()
{
	if (m_list.GetFirstSelectedItemPosition())
	{
		if (::MessageBox(GetSafeHwnd(), L"Do you want to purge the selected items?", L"Purge/Select", MB_YESNO) == IDYES)
		{
			OnPurge();
		}
	}
	__super::OnOK();
}

void CMetaPurgeDialog::OnClose() 
{
	CDialog::OnClose();
}
