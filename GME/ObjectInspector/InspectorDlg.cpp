// InspectorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ObjectInspector.h"
#include "ObjectInspectorCtl.h"
#include "InspectorDlg.h"

TCHAR	*panelTitles[] = {  _T("Attributes"), _T("Preferences"),_T("Properties")};

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInspectorDlg dialog


CInspectorDlg::CInspectorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CInspectorDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInspectorDlg)
	m_forKind = FALSE;
	//}}AFX_DATA_INIT

	
	

	m_oldSize.cx = -1;
	m_oldSize.cy = -1;

	for (int i = 0; i < INSP_PANEL_NUM; i++) 
	{
		m_inspectorLists[i] = new CInspectorList(i==INSP_PREF_PANEL || i==INSP_PROP_PANEL);
	}
	m_inspectorSheet.SetInspectorLists(m_inspectorLists);


	for (int j = 0; j < INSP_PANEL_NUM; j++) {
		m_inspectorEntries[j] = new CInspectorEntryList();
	}

	for (int k = 0; k < INSP_PANEL_NUM; k++) 
	{
		m_bIsPanelVisible[k]=false;
	}

	m_currentPanel = 0;

	// this method is called only once
	CItemData::getRealFmtString();
}


CInspectorDlg::~CInspectorDlg()
{
	for (int i = 0; i < INSP_PANEL_NUM; i++) {
		delete m_inspectorLists[i];
	}
	for (int j = 0; j < INSP_PANEL_NUM; j++) {
		delete m_inspectorEntries[j];
	}
}


void CInspectorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInspectorDlg)
	DDX_Control(pDX, IDC_INSPECTOR_SHEET, m_inspectorSheet);
	DDX_Control(pDX, IDC_STATIC_HELP, m_wndHelp);
	DDX_Check(pDX, IDC_CHECK_KIND, m_forKind);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInspectorDlg, CDialog)
	//{{AFX_MSG_MAP(CInspectorDlg)
	ON_WM_SIZE()
	ON_NOTIFY(TCN_SELCHANGE, IDC_INSPECTOR_SHEET, OnSelchangeInspectorSheet)
	ON_MESSAGE(LBN_ON_ITEM_CHANGED, OnItemChanged)
	ON_MESSAGE(LBN_ON_OPEN_REFERED, OnOpenRefered)
	ON_MESSAGE(MSG_NAME_EDIT_END_OK, OnNameEditEndOK)
	ON_MESSAGE(MSG_NAME_EDIT_END_CANCEL, OnNameEditEndCancel)
	ON_EN_KILLFOCUS(IDC_EDIT_NAME, OnKillfocusEditName)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInspectorDlg message handlers

void CInspectorDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);


	if ((m_oldSize.cx < 0) || (m_oldSize.cy < 0)) {
		m_oldSize.cx = cx;
		m_oldSize.cy = cy;
		return;
	}

	cx = max(INSP_MIN_SIZEX, cx);
	cy = max(INSP_MIN_SIZEY, cy);

	int dx = cx - m_oldSize.cx;
	int dy = cy - m_oldSize.cy;

	CWnd *control = GetDlgItem(IDC_EDIT_NAME);
	if (control->GetSafeHwnd()) {
		CRect ctrlRect;
		control->GetWindowRect(&ctrlRect);
		ScreenToClient(&ctrlRect);
		ctrlRect.right += dx;
		control->MoveWindow(&ctrlRect);
	}
	
	control = GetDlgItem(IDC_CHECK_KIND);
	if (control->GetSafeHwnd()) {
		CRect ctrlRect;
		control->GetWindowRect(&ctrlRect);
		ScreenToClient(&ctrlRect);
		ctrlRect.right += dx;
		ctrlRect.left += dx;
		control->MoveWindow(&ctrlRect);
	}

	control = GetDlgItem(IDC_INSPECTOR_SHEET);
	if (control->GetSafeHwnd()) {
		CRect ctrlRect;
		control->GetWindowRect(&ctrlRect);
		ScreenToClient(&ctrlRect);
		ctrlRect.right += dx;
		ctrlRect.bottom += dy;
		control->MoveWindow(&ctrlRect);
	}

	control = GetDlgItem(IDC_STATIC_HELP);

	if (control->GetSafeHwnd()) 
	{
		CRect ctrlRect;
		control->GetWindowRect(&ctrlRect);
		ScreenToClient(&ctrlRect);
		
		static int nHelpCtrlSize=ctrlRect.Height();

		ctrlRect.right += dx;		
		
		if(ctrlRect.Height()>=nHelpCtrlSize)
		{
			ctrlRect.top += dy;

		}
		else
		{
/*			int nDiff=nHelpCtrlSize-ctrlRect.Height();
			if(nDiff>0 && dy>nDiff)
			{
				ctrlRect.top +=(dy-(nHelpCtrlSize-ctrlRect.Height()));
			}*/
		}

		ctrlRect.bottom += dy;

		control->MoveWindow(&ctrlRect);								
	}		

	m_wndSplitterBar.SetPanes(&m_inspectorSheet, &m_wndHelp);

	m_oldSize.cx = cx;
	m_oldSize.cy = cy;
}

BOOL CInspectorDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_NameCtrl.SubclassWindow(GetDlgItem(IDC_EDIT_NAME)->GetSafeHwnd());
	m_NameCtrl.EnableWindow(FALSE);

	LOGFONT logFont;
	memset(&logFont, 0, sizeof(LOGFONT));
	logFont.lfCharSet = DEFAULT_CHARSET;
	logFont.lfHeight = INSP_ENTRY_FONT_PTSIZE;
	logFont.lfWeight = FW_BOLD;
	lstrcpyn(logFont.lfFaceName, INSP_ENTRY_FONT_FACE, sizeof(logFont.lfFaceName)/sizeof(TCHAR));

	VERIFY(m_BoldFont.CreatePointFontIndirect(&logFont, NULL));

	
	CWnd* pHelpTitle=GetDlgItem(IDC_HELP_TITLE);
	if(pHelpTitle->GetSafeHwnd())
	{
		pHelpTitle->SetFont(&m_BoldFont);
	}

	// Create inspector lists
	for (int i = 0; i < INSP_PANEL_NUM; i++) {
		m_inspectorSheet.InsertItem(i, panelTitles[i]);
	
		CRect listRect;
		m_inspectorSheet.GetClientRect(&listRect);
		m_inspectorSheet.AdjustRect(FALSE, &listRect);
		m_inspectorLists[i]->Create(WS_CHILD | WS_VISIBLE, listRect,&m_inspectorSheet,IDC_INSPECTOR_LIST0+i);
		m_inspectorLists[i]->ShowWindow(SW_HIDE);
	}


	// Test 
/*
	CListItem ListItem;

	CItemData ItemData;
	
	ListItem.bIsContainer=true;

	ListItem.strName="General Properties";
	
	ListItem.Value=ItemData;
	ListItem.DefValue=ItemData;
	m_inspectorLists[0]->AddItem(ListItem);

	ListItem.bIsContainer=false;
	ListItem.strName="Eye color";
	ItemData.SetColorValue(RGB(255,0,0));
	
	ListItem.Value=ItemData;
	ListItem.DefValue=ItemData;
	m_inspectorLists[0]->AddItem(ListItem);
	
	ListItem.strName="Her eyes are blue";
	ItemData.SetBoolValue(false);
	
	ListItem.Value=ItemData;
	ListItem.DefValue=ItemData;
	m_inspectorLists[0]->AddItem(ListItem);

	ListItem.strName="Fat";
	ItemData.SetDoubleValue(2.2);
	
	ListItem.Value=ItemData;
	ListItem.DefValue=ItemData;
	m_inspectorLists[0]->AddItem(ListItem);

	ListItem.strName="Amount";
	ItemData.SetIntValue(2);

	ListItem.Value=ItemData;
	ListItem.DefValue=ItemData;
	m_inspectorLists[0]->AddItem(ListItem);


	ListItem.strName="Say";
	ItemData.SetStringValue("Hello");

	ListItem.Value=ItemData;
	ListItem.DefValue=ItemData;
	m_inspectorLists[0]->AddItem(ListItem);

	ListItem.strName="Where would you go?";
	ItemData.SetCompassValue(1);
	
	ListItem.Value=ItemData;
	ListItem.DefValue=ItemData;
	m_inspectorLists[0]->AddItem(ListItem);

	ListItem.strName="Where is Detroit?";
	ItemData.SetCompassExclValue(1);
	
	ListItem.Value=ItemData;
	ListItem.DefValue=ItemData;
	m_inspectorLists[0]->AddItem(ListItem);

	
	ListItem.strName="Specific Properties";
	ListItem.bIsContainer=true;
	ItemData.dataType=ITEMDATA_NULL;
	
	ListItem.Value=ItemData;
	ListItem.DefValue=ItemData;
	m_inspectorLists[0]->AddItem(ListItem);
	
	ListItem.bIsContainer=false;
	ListItem.strName="Turkey Preference";
	ItemData.SetStringValue("Goes down with Thanksgiving");
	ListItem.bIsDefault=true;
	
	ListItem.Value=ItemData;
	ListItem.DefValue=ItemData;
	m_inspectorLists[0]->AddItem(ListItem);

	ListItem.bIsDefault=false;
	CStringArray strArr;
	CString strElement;

	strElement="Row 1";
	strArr.Add(strElement);

	strElement="Row 2";
	strArr.Add(strElement);
	
	strElement="Row 3";
	strArr.Add(strElement);

	strElement="Row 4";
	strArr.Add(strElement);
	ItemData.SetStringValue(strArr,4);
	
	ListItem.Value=ItemData;
	ListItem.DefValue=ItemData;
	m_inspectorLists[0]->AddItem(ListItem);



	ItemData.SetListValue(strArr,2);

	ListItem.Value=ItemData;
	ListItem.DefValue=ItemData;
	m_inspectorLists[0]->AddItem(ListItem);


*/

	//////////// Test end //////////////


	// Create Splitter
	CRect rect(0,0,0,0);
	m_wndSplitterBar.Create(WS_CHILD|WS_VISIBLE, rect,this,IDC_SPLITTER_BAR,TRUE);
	m_wndSplitterBar.SetPanes(&m_inspectorSheet,&m_wndHelp);
	
	// Initialize Panel contents
	SwitchPanel(INSP_ATTR_PANEL);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CInspectorDlg::OnSelchangeInspectorSheet(NMHDR* pNMHDR, LRESULT* pResult) 
{
	SwitchPanel(m_inspectorSheet.GetCurSel());
	*pResult = 0;
}

void CInspectorDlg::SwitchPanel(int panel)
{
	CWnd *checkBox = GetDlgItem(IDC_CHECK_KIND);

	m_inspectorLists[m_currentPanel]->ShowWindow(SW_HIDE);

	if(m_bIsPanelVisible[panel])
	{
		m_inspectorLists[panel]->ShowWindow(TRUE);
		checkBox->EnableWindow(panel == INSP_PREF_PANEL);
	}
	else
	{
		checkBox->EnableWindow(FALSE);
	}
	m_currentPanel = panel;

	m_inspectorLists[panel]->RefreshState();
}

void CInspectorDlg::Reset(bool preserveScrollPos)
{
}

void CInspectorDlg::ShowPanel(int panel)
{
	m_inspectorSheet.SetCurSel(panel);
	SwitchPanel(panel);
}

void CInspectorDlg::CyclePanel(bool frwd)
{
	int k = m_inspectorSheet.GetCurSel();
	ShowPanel( (frwd?++k:k+2) % 3);
}


void CInspectorDlg::SetHelp(const CString &strTitle, const CString &strText)
{
	CStatic* pHelpTitle=(CStatic*)this->GetDlgItem(IDC_HELP_TITLE);
	if(pHelpTitle->GetSafeHwnd())
	{
		pHelpTitle->SetWindowText(strTitle);
	}

	CStatic* pHelpText=(CStatic*)this->GetDlgItem(IDC_HELP_TEXT);
	if(pHelpText->GetSafeHwnd())
	{
		pHelpText->SetWindowText(strText);
	}
}

LRESULT CInspectorDlg::OnItemChanged(WPARAM wParam, LPARAM lParam)
{
/*
#ifdef _DEBUG
	CString strTemp;
	strTemp.Format(_T("Item Changed: %u on Page %u"),wParam,lParam);
	MessageBox(strTemp,_T("Item Changed"));
#endif
*/
	if(lParam==INSP_PREF_PANEL)
	{
		OnItemChangedPreference(wParam);
	}
	else if(lParam==INSP_ATTR_PANEL)
	{
		OnItemChangedAttribute(wParam);
	}
	else if (lParam == INSP_PROP_PANEL)
	{
		CListItem ListItem;
		m_inspectorLists[INSP_PROP_PANEL]->GetItem(wParam, ListItem);

		CObjectInspectorCtrl* pParent = (CObjectInspectorCtrl*)GetParent();

		if (ListItem.strName == L"Type") {
			pParent->DetachFromArchetype(ListItem);
		}
	}
	return TRUE;
}

LRESULT CInspectorDlg::OnOpenRefered(WPARAM wParam, LPARAM lParam)
{
	CListItem ListItem;
	m_inspectorLists[INSP_PROP_PANEL]->GetItem(wParam, ListItem);

	CObjectInspectorCtrl* pParent = (CObjectInspectorCtrl*)GetParent();

	pParent->OpenRefered();
	return TRUE;
}

void CInspectorDlg::SetName(const CString &strName, bool bIsReadOnly,bool bIsEnabled)
{
	m_NameCtrl.SetWindowText(strName);
	
	m_NameCtrl.SetReadOnly(bIsReadOnly);
	m_NameCtrl.EnableWindow(bIsEnabled);
	
	m_strName=strName;
}

void CInspectorDlg::ShowPanel(int nPanel,bool bIsVisible)
{
	m_bIsPanelVisible[nPanel]=bIsVisible;
	
	if(nPanel==m_currentPanel)
	{
		m_inspectorLists[nPanel]->ShowWindow(bIsVisible?SW_SHOW:SW_HIDE);
				
	}		
}

void CInspectorDlg::OnItemChangedAttribute(int nItem)
{
	CListItem ListItem;
	m_inspectorLists[INSP_ATTR_PANEL]->GetItem(nItem,ListItem);

	CObjectInspectorCtrl* pParent=(CObjectInspectorCtrl*)GetParent();

	pParent->WriteAttributeItemToMga(ListItem);
}


void CInspectorDlg::OnItemChangedPreference(int nItem)
{

	CListItem ListItem;
	m_inspectorLists[INSP_PREF_PANEL]->GetItem(nItem,ListItem);
	

	UINT uSrcValue;
	UINT uDstValue;
	CString strSrcValue;
	CString strDstValue;
	CString strValue;


	if(ListItem.dwUserData==AUTOROUTER_SOURCE)
	{
		CListItem ListItemDst;

		VERIFY(m_inspectorLists[INSP_PREF_PANEL]->FindByKey(ListItem.dwKeyValue,AUTOROUTER_DESTINATION,ListItemDst));

		uSrcValue=ListItem.Value.compassVal;
		uDstValue=ListItemDst.Value.compassVal;
		
		CCompassData::toMgaStringCheck(strSrcValue,uSrcValue,true);
		CCompassData::toMgaStringCheck(strDstValue,uDstValue,false);
		
		strValue=strDstValue+strSrcValue;
		ListItem.Value.stringVal.SetAtGrow(0,strValue);
	}

	if(ListItem.dwUserData==AUTOROUTER_DESTINATION)
	{
		CListItem ListItemSrc;

		VERIFY(m_inspectorLists[INSP_PREF_PANEL]->FindByKey(ListItem.dwKeyValue,AUTOROUTER_SOURCE,ListItemSrc));

		uSrcValue=ListItemSrc.Value.compassVal;
		uDstValue=ListItem.Value.compassVal;
		
		CCompassData::toMgaStringCheck(strSrcValue,uSrcValue,true);
		CCompassData::toMgaStringCheck(strDstValue,uDstValue,false);
		
		strValue=strDstValue+strSrcValue;
		ListItem.Value.stringVal.SetAtGrow(0,strValue);
	}

	CObjectInspectorCtrl* pParent=(CObjectInspectorCtrl*)GetParent();

	UpdateData(TRUE); // For Kind checkbox
	bool bIsKind=(m_forKind!=0);
	pParent->WritePreferenceItemToMga(ListItem,bIsKind);
}



afx_msg LRESULT CInspectorDlg::OnNameEditEndOK(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	if(m_NameCtrl.GetModify())
	{
		m_NameCtrl.GetWindowText(m_strName);
		CObjectInspectorCtrl* pParent=(CObjectInspectorCtrl*)GetParent();
		pParent->WriteNameToMga(m_strName);
	}

	return TRUE;
}


afx_msg LRESULT CInspectorDlg::OnNameEditEndCancel(WPARAM /*wParam*/, LPARAM /*lParam*/)
{

	if(m_NameCtrl.GetModify())
	{
		m_NameCtrl.SetWindowText(m_strName);
		// The following instructions would issue a whole undo to the MgaProject, causing JIRA GME-231
		//CObjectInspectorCtrl* pParent=(CObjectInspectorCtrl*)GetParent();
		//if( pParent) pParent->UndoRedo();
	}
	return TRUE;
}

void CInspectorDlg::OnKillfocusEditName() 
{
	OnNameEditEndOK(0,0);

	m_inspectorSheet.SetFocus();
	
}

void CInspectorDlg::Refresh()
{
	ShowPanel(m_currentPanel);	
}
