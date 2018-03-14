// RegistryBrowserDlg.cpp : implementation file
//

#include "stdafx.h"
#include "mgautil.h"
#include "RegistryBrowserDlg.h"

EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRegistryBrowserDlg dialog


CRegistryBrowserDlg::CRegistryBrowserDlg(const CComPtr<IMgaObject> &object, CWnd* pParent /*=NULL*/)
	: CDialog(CRegistryBrowserDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRegistryBrowserDlg)
	m_objectKind = _T("");
	m_objectName = _T("");
	m_objectRole = _T("");
	m_regnodePath = _T("");
	m_regnodeStatus = _T("");
	m_regnodeValue = _T("");
	//}}AFX_DATA_INIT
	m_object = object;
}

CRegistryBrowserDlg::~CRegistryBrowserDlg()
{
	POSITION pos = m_nodes.GetHeadPosition();
	while (pos) {
		delete m_nodes.GetNext(pos);
	}
	m_nodes.RemoveAll();
}


void CRegistryBrowserDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRegistryBrowserDlg)
	DDX_Control(pDX, IDC_REGNODE, m_wndRegNode);
	DDX_Control(pDX, IDC_REGISTRY_TREE, m_wndRegistryTree);
	DDX_Text(pDX, IDC_OBJECTKIND, m_objectKind);
	DDX_Text(pDX, IDC_OBJECTNAME, m_objectName);
	DDX_Text(pDX, IDC_OBJECTROLE, m_objectRole);
	DDX_Text(pDX, IDC_REGNODE_PATH, m_regnodePath);
	DDX_Text(pDX, IDC_REGNODE_STATUS, m_regnodeStatus);
	DDX_Text(pDX, IDC_REGNODE_VALUE, m_regnodeValue);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRegistryBrowserDlg, CDialog)
	//{{AFX_MSG_MAP(CRegistryBrowserDlg)
	ON_WM_SIZE()
	ON_EN_CHANGE(IDC_REGNODE_VALUE, OnChangeRegnodeValue)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CRegistryBrowserDlg message handlers

BOOL CRegistryBrowserDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	CRect rect(0,0,0,0);


	m_wndSplitterBar.Create(WS_CHILD|WS_BORDER|WS_DLGFRAME|WS_VISIBLE, rect,this,999);
	m_wndSplitterBar.SetPanes(&m_wndRegistryTree,&m_wndRegNode);


	CWinApp *pApp = AfxGetApp();
	int cx = ::GetSystemMetrics(SM_CXSMICON);
	int cy = ::GetSystemMetrics(SM_CYSMICON);
	m_imageList.Create(cx, cy, ILC_COLOR4 | ILC_MASK, 5, 1);
	m_imageMap.InitHashTable(10);
	
	HICON hIcon;

	hIcon = (HICON)::LoadImage(HINST_THISCOMPONENT, 
		MAKEINTRESOURCE(IDI_ICON_REGHERE), IMAGE_ICON, cx, cy, LR_DEFAULTCOLOR);
	ASSERT(hIcon);
	if (hIcon) {
		m_imageMap[IDI_ICON_REGHERE] = m_imageList.Add(hIcon);
		::DeleteObject(hIcon);
	}

	hIcon = (HICON)::LoadImage(HINST_THISCOMPONENT, 
		MAKEINTRESOURCE(IDI_ICON_REGINHERIT), IMAGE_ICON, cx, cy, LR_DEFAULTCOLOR);
	ASSERT(hIcon);
	if (hIcon) {
		m_imageMap[IDI_ICON_REGINHERIT] = m_imageList.Add(hIcon);
		::DeleteObject(hIcon);
	}

	hIcon = (HICON)::LoadImage(HINST_THISCOMPONENT, 
		MAKEINTRESOURCE(IDI_ICON_REGINVALID), IMAGE_ICON, cx, cy, LR_DEFAULTCOLOR);
	if (hIcon) {
		m_imageMap[IDI_ICON_REGINVALID] = m_imageList.Add(hIcon);
		::DeleteObject(hIcon);
	}

	hIcon = (HICON)::LoadImage(HINST_THISCOMPONENT, 
		MAKEINTRESOURCE(IDI_ICON_REGMETA), IMAGE_ICON, cx, cy, LR_DEFAULTCOLOR);
	if (hIcon) {
		m_imageMap[IDI_ICON_REGMETA] = m_imageList.Add(hIcon);
		::DeleteObject(hIcon);
	}
	
	hIcon = (HICON)::LoadImage(HINST_THISCOMPONENT, 
		MAKEINTRESOURCE(IDI_ICON_REGUNDEF), IMAGE_ICON, cx, cy, LR_DEFAULTCOLOR);
	if (hIcon) {
		m_imageMap[IDI_ICON_REGUNDEF] = m_imageList.Add(hIcon);
		::DeleteObject(hIcon);
	}

	m_wndRegistryTree.SetImageList(&m_imageList,TVSIL_NORMAL);

	try {
		CComBSTR bstr_tmp;
		COMTHROW(m_object->get_Name(&bstr_tmp));
		CopyTo(bstr_tmp, m_objectName);

		CComPtr<IMgaMetaBase> metaBase;
		COMTHROW(m_object->get_MetaBase(&metaBase));
		if (metaBase) {
			bstr_tmp.Empty();
			COMTHROW(metaBase->get_DisplayedName(&bstr_tmp));
			CopyTo(bstr_tmp,m_objectKind);
		}
		else {
			m_objectKind = "N/A";
		}

		CComPtr<IMgaMetaRole> metaRole;
		
		CComPtr<IMgaFCO> mgaFCO;
		if (SUCCEEDED(m_object.QueryInterface(&mgaFCO))) {
			COMTHROW(mgaFCO->get_MetaRole(&metaRole) );
		}
		if (metaRole) {
			bstr_tmp.Empty();
			COMTHROW(metaRole->get_Name(&bstr_tmp));
			CopyTo(bstr_tmp, m_objectRole);
		}
		else {
			m_objectRole = "N/A";
		}
	}
	catch (hresult_exception &){
		ASSERT(("Error while initializing Registry Browser.", false));
	}
	UpdateData(FALSE);

	if (m_object) {
		FillNodes();
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CRegistryBrowserDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	CRect origTopRect(0, 0, 0+406, 0+18), origBottomRect(0, 200, 0+406, 200+20 ); 
	MapDialogRect(&origTopRect);
	MapDialogRect(&origBottomRect);

	CRect rectTree, rectValue;
	if (m_wndRegistryTree.GetSafeHwnd()) {
		m_wndRegistryTree.GetWindowRect(&rectTree);
		ScreenToClient(&rectTree);
	}
	if (m_wndRegNode.GetSafeHwnd()) {
		m_wndRegNode.GetWindowRect(&rectValue);
		ScreenToClient(&rectValue);
	}

	// Not perfect....
	double ratio = (double)rectTree.Width()/((double)rectValue.Width()+(double)rectTree.Width());

	if (m_wndRegNode.GetSafeHwnd() && m_wndRegistryTree.GetSafeHwnd()) {
		m_wndRegistryTree.SetWindowPos(NULL, 0, origTopRect.bottom, static_cast<int>(cx*ratio), cy - (origBottomRect.bottom-origBottomRect.top) - origTopRect.bottom, SWP_NOZORDER);
		m_wndRegNode.SetWindowPos(NULL, static_cast<int>(cx*ratio)+GetSystemMetrics(SM_CYFRAME), origTopRect.bottom, cx - (static_cast<int>(cx*ratio)+GetSystemMetrics(SM_CYFRAME)), cy - (origBottomRect.bottom-origBottomRect.top) - origTopRect.bottom, SWP_NOZORDER);
		m_wndSplitterBar.SetPanes(&m_wndRegistryTree,&m_wndRegNode);		
	}

	CRect rect3(0, 204, 0+406, 204+16);
	MapDialogRect(&rect3);
	CRect rectDlg;
	GetClientRect(&rectDlg);
	ratio = ((double)(rectDlg.Width())/ 406.0 );

	CWnd* wndOK = this->GetDlgItem(IDOK);
	if (wndOK) {
		CRect origWnd;
		wndOK->GetClientRect(&origWnd);
		wndOK->MoveWindow(static_cast<int>(112.0 * ratio), rectDlg.bottom - (rect3.bottom - rect3.top), origWnd.Width(), origWnd.Height());
	}
	CWnd* wndCANCEL = this->GetDlgItem(IDCANCEL);
	if (wndCANCEL) {
		CRect origWnd;
		wndCANCEL->GetClientRect(&origWnd);
		wndCANCEL->MoveWindow(static_cast<int>(242.0 * ratio), rectDlg.bottom - (rect3.bottom - rect3.top), origWnd.Width(), origWnd.Height());
	}

	
}

void CRegistryBrowserDlg::FillNodes(void)
{
	CComPtr<IMgaRegNodes>	regNodes;
	CComPtr<IMgaFolder> mgaFolder;
	CComPtr<IMgaFCO> mgaFCO;
	if (SUCCEEDED(m_object.QueryInterface(&mgaFCO))) {
		COMTHROW(mgaFCO->get_Registry(VARIANT_TRUE, &regNodes));
	}
	else if (SUCCEEDED(m_object.QueryInterface(&mgaFolder))) {
		COMTHROW(mgaFolder->get_Registry(VARIANT_TRUE, &regNodes));
	}
	else {
		ASSERT(("Unexpected MGA object.", false));
		return;
	}
	AddNodes(NULL, regNodes);
}

void CRegistryBrowserDlg::AddNodes(CRegBrwNode *parent, CComPtr<IMgaRegNodes> &regNodes)
{
	MGACOLL_ITERATE(IMgaRegNode, regNodes) {
		CComPtr<IMgaRegNode> regNode = MGACOLL_ITER;

		CRegBrwNode *brNode = new CRegBrwNode();
		m_nodes.AddTail(brNode);

		brNode->parent = parent;

		CComBSTR bstr;
		COMTHROW(regNode->get_Name(&bstr));
		CopyTo(bstr, brNode->name);

		bstr.Empty();
		COMTHROW(regNode->get_Path(&bstr));
		CopyTo(bstr, brNode->path);

		bstr.Empty();
		COMTHROW(regNode->get_Value(&bstr));
		CopyTo(bstr, brNode->value);

		COMTHROW(regNode->get_Status(&(brNode->status)));

		HTREEITEM hnd;
		if (parent) {
			hnd = m_wndRegistryTree.InsertItem(brNode->name, (HTREEITEM)(parent->handle));
		}
		else {
			hnd = m_wndRegistryTree.InsertItem(brNode->name);
		}

		brNode->handle = (void*)hnd;
		m_wndRegistryTree.SetItemData(hnd,(DWORD_PTR)brNode);
		
		int imageNum;
		switch (brNode->status) {
		case ATTSTATUS_HERE:
			m_imageMap.Lookup(IDI_ICON_REGHERE, imageNum);
			break;
		case ATTSTATUS_METADEFAULT:
			m_imageMap.Lookup(IDI_ICON_REGMETA, imageNum);
			break;
		case ATTSTATUS_UNDEFINED:
			m_imageMap.Lookup(IDI_ICON_REGUNDEF, imageNum);
			break;
		case ATTSTATUS_INVALID:
			m_imageMap.Lookup(IDI_ICON_REGINVALID, imageNum);
			break;
		default:
			m_imageMap.Lookup(IDI_ICON_REGINHERIT, imageNum);
			break;
		}
		

		VERIFY(m_wndRegistryTree.SetItemImage(hnd,imageNum,imageNum));
		
		CComPtr<IMgaRegNodes> subRegNodes;
		COMTHROW(regNode->get_SubNodes(VARIANT_TRUE, &subRegNodes));
		AddNodes(brNode, subRegNodes);
	}
	MGACOLL_ITERATE_END;
}

void CRegistryBrowserDlg::OnChangeRegnodeValue() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	HTREEITEM hItem = m_wndRegistryTree.GetSelectedItem();
	if (hItem) {
		CRegBrwNode *node = (CRegBrwNode *)m_wndRegistryTree.GetItemData(hItem);
		if (node) {
			node->status = ATTSTATUS_HERE;
		}
		int imageNum;
		m_imageMap.Lookup(IDI_ICON_REGHERE, imageNum);
		m_wndRegistryTree.SetItemImage(hItem, imageNum, imageNum);
		
		UpdateData(TRUE);
		m_regnodeStatus = "HERE";
		UpdateData(FALSE);
	}
	
}

void CRegistryBrowserDlg::UpdateRegistry()
{
	CComPtr<IMgaRegNodes>	regNodes;
	CComPtr<IMgaFolder> mgaFolder;
	CComPtr<IMgaFCO> mgaFCO;
	
	if (SUCCEEDED(m_object.QueryInterface(&mgaFCO))) {
		COMTHROW(mgaFCO->get_Registry(VARIANT_TRUE, &regNodes));
	}
	else if (SUCCEEDED(m_object.QueryInterface(&mgaFolder))) {
		COMTHROW(mgaFolder->get_Registry(VARIANT_TRUE, &regNodes));
	}
	else {
		ASSERT(("Unexpected MGA object.", false));
		return;
	}

	MGACOLL_ITERATE(IMgaRegNode, regNodes) {
		CComPtr<IMgaRegNode> regNode = MGACOLL_ITER;
		COMTHROW(regNode->RemoveTree());
	}
	MGACOLL_ITERATE_END;

	POSITION pos = m_nodes.GetHeadPosition();
	while (pos) {
		CRegBrwNode * node = m_nodes.GetNext(pos);

		if (node->status == ATTSTATUS_HERE) {
			CComBSTR path, value;
			CopyTo(node->path, path);
			CopyTo(node->value, value);
			if (mgaFCO) {
				COMTHROW(mgaFCO->put_RegistryValue(path, value));
			}
			else {
				ASSERT(mgaFolder);
				COMTHROW(mgaFolder->put_RegistryValue(path, value));
			}
			
		}

	}

}

void CRegistryBrowserDlg::OnOK() 
{
	// TODO: Add extra validation here
	CRegBrwNode *oldnode;
	HTREEITEM hItem = m_wndRegistryTree.GetSelectedItem();
	if (hItem) {
		oldnode = (CRegBrwNode*)m_wndRegistryTree.GetItemData(hItem);
		if (oldnode) {
			UpdateData(TRUE);
			if (oldnode->status == ATTSTATUS_HERE) {
				oldnode->value = m_regnodeValue;
				oldnode->value.Replace(_T("\r\n"),_T("\n")); // Remove Win32 GUI line ends
			}
		}
	}

	CDialog::OnOK();
}

BOOL CRegistryBrowserDlg::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message==WM_KEYDOWN && pMsg->wParam==VK_RETURN)
	{
		DWORD def_id=GetDefID();
		if (def_id!=0)
		{
            if (HIBYTE(GetKeyState(VK_CONTROL))) {
                OnOK();
                return TRUE;
            }

            if (pMsg->hwnd == m_wndRegistryTree.m_hWnd)
			{
				GetDlgItem(LOWORD(def_id))->SetFocus();
				return TRUE;
				// discard the message!
			}


			CWnd *wnd=FromHandle(pMsg->hwnd);
			if (wnd->GetParent()->m_hWnd == m_wndRegistryTree.m_hWnd) {
				GetDlgItem(LOWORD(def_id))->SetFocus();
				return TRUE;
				// discard the message!
			}
		}
	}
	// be a good citizen - call the base class
	return CDialog::PreTranslateMessage(pMsg);
}
