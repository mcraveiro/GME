// AspectSyncDlg.cpp : implementation file
//

#include "stdafx.h"
#include "GMEApp.h"
#include "AspectSyncDlg.h"

#include "GMEview.h"

#include "guiobject.h"
#include "guimeta.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAspectSyncDlg dialog


CAspectSyncDlg::CAspectSyncDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAspectSyncDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAspectSyncDlg)
	m_priorityForSrcVisible = FALSE;
	m_priorityForSelected = FALSE;
	//}}AFX_DATA_INIT
}


void CAspectSyncDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAspectSyncDlg)
	DDX_Control(pDX, IDC_DSTASPECT_LIST, m_dstAspectList);
	DDX_Control(pDX, IDC_SELOBJS_LIST, m_syncObjsList);
	DDX_Control(pDX, IDC_SRCASPECT_COMBO, m_srcAspectCombo);
	DDX_Check(pDX, IDC_PRIORITY_CHECK, m_priorityForSrcVisible);
	DDX_Check(pDX, IDC_PRIORITY_CHECK2, m_priorityForSelected);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAspectSyncDlg, CDialog)
	//{{AFX_MSG_MAP(CAspectSyncDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAspectSyncDlg message handlers

BOOL CAspectSyncDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	//  ATTENTION ! Requires IE4
	m_syncObjsList.SetExtendedStyle(m_syncObjsList.GetExtendedStyle() | LVS_EX_FULLROWSELECT);
	m_dstAspectList.SetExtendedStyle(m_dstAspectList.GetExtendedStyle() | LVS_EX_FULLROWSELECT);

	CRect rect;
	m_dstAspectList.GetClientRect(&rect);
	m_dstAspectList.InsertColumn(0, _T("Aspect"), LVCFMT_LEFT, rect.Width());

	m_syncObjsList.GetClientRect(&rect);
	m_syncObjsList.InsertColumn(0, _T("Name"), LVCFMT_LEFT, rect.Width()/2);
	m_syncObjsList.InsertColumn(1, _T("Kind"), LVCFMT_LEFT, rect.Width()/4);
	m_syncObjsList.InsertColumn(2, _T("Role"), LVCFMT_LEFT, rect.Width()/4);
	

	int aspectCount = 0;
	POSITION apos = m_allAspects.GetHeadPosition();
	while (apos) {
		CGuiMetaAspect *metaAspect = m_allAspects.GetNext(apos);
		
		int nItem = m_srcAspectCombo.AddString(metaAspect->name);
		m_srcAspectCombo.SetItemData(nItem, (DWORD_PTR)metaAspect);

		if (m_srcAspect == metaAspect) {
			m_srcAspectCombo.SetCurSel(nItem);
		}

		nItem = m_dstAspectList.InsertItem(aspectCount++, metaAspect->name);
		m_dstAspectList.SetItemData(nItem, (DWORD_PTR)metaAspect);
		if (m_srcAspect != metaAspect) {
			m_dstAspectList.SetItemState(nItem, LVIS_SELECTED, LVIS_SELECTED);
		} else {
			m_dstAspectList.SetItemState(nItem, LVIS_SELECTED, 0);
		}
	}

	int objCount = 0;
	POSITION opos = m_allObjects.GetHeadPosition();
	while (opos) {
		CGuiObject	*obj = m_allObjects.GetNext(opos);


		bool isSelected = (m_movingObjects.Find(obj) != NULL);

		LVITEM	lvItem;

		lvItem.mask = LVIF_TEXT | LVIF_STATE | LVIF_PARAM;
		lvItem.iItem = objCount;
		lvItem.iSubItem = 0;
		lvItem.state = isSelected ? LVIS_SELECTED : 0;
		lvItem.stateMask = isSelected ? LVIS_SELECTED : 0;
		lvItem.pszText = obj->GetName().GetBuffer(obj->GetName().GetLength());
		lvItem.lParam = (LPARAM)obj;

		m_syncObjsList.InsertItem(&lvItem);

		lvItem.mask = LVIF_TEXT;
		lvItem.iSubItem = 1;
		lvItem.pszText = obj->kindName.GetBuffer(obj->kindName.GetLength());

		m_syncObjsList.SetItem(&lvItem);

		lvItem.iSubItem = 2;
		lvItem.pszText = obj->roleName.GetBuffer(obj->roleName.GetLength());

		m_syncObjsList.SetItem(&lvItem);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


BOOL CAspectSyncDlg::PreTranslateMessage(MSG* pMsg) 
{

	if (pMsg->message==WM_KEYDOWN) {
		if (pMsg->wParam=='A' && ::GetKeyState(VK_CONTROL)) {
			if (pMsg->hwnd == m_syncObjsList.m_hWnd) {
				int item = m_syncObjsList.GetNextItem(-1, LVNI_ALL);
				while (item != -1) {
					m_syncObjsList.SetItemState(item, LVIS_SELECTED, LVIS_SELECTED);
					item = m_syncObjsList.GetNextItem(item, LVNI_ALL);
				}
			}

			if (pMsg->hwnd == m_dstAspectList.m_hWnd) {
				int item = m_dstAspectList.GetNextItem(-1, LVNI_ALL);
				while (item != -1) {
					m_dstAspectList.SetItemState(item, LVIS_SELECTED, LVIS_SELECTED);
					item = m_dstAspectList.GetNextItem(item, LVNI_ALL);
				}
			}
					
		}
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}

void CAspectSyncDlg::OnOK() 
{
	
	m_dstAspects.RemoveAll();
	m_movingObjects.RemoveAll();
	m_srcAspect = NULL;

	int nItem;
	POSITION apos = m_dstAspectList.GetFirstSelectedItemPosition();
	while (apos) {
		nItem = m_dstAspectList.GetNextSelectedItem(apos);
		m_dstAspects.AddTail((CGuiMetaAspect*)m_dstAspectList.GetItemData(nItem));
	}

	nItem = m_srcAspectCombo.GetCurSel();
	m_srcAspect = (CGuiMetaAspect*)m_srcAspectCombo.GetItemData(nItem);

	int nCount = m_syncObjsList.GetItemCount();
	for (nItem = 0; nItem < nCount; nItem++) {
		CGuiObject* obj = (CGuiObject*)m_syncObjsList.GetItemData(nItem);
		if (m_syncObjsList.GetItemState(nItem, LVIS_SELECTED) == LVIS_SELECTED) {
			m_movingObjects.AddTail(obj);
		}
	}
	
	CDialog::OnOK();
}
