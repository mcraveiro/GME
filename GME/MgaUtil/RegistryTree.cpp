// RegistryTree.cpp : implementation file
//

#include "stdafx.h"
#include "mgautil.h"
#include "RegistryBrowserDlg.h"
#include "resource.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRegistryTree

CRegistryTree::CRegistryTree()
{
}

CRegistryTree::~CRegistryTree()
{
}


BEGIN_MESSAGE_MAP(CRegistryTree, CTreeCtrl)
	//{{AFX_MSG_MAP(CRegistryTree)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelchanged)
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(ID_CNTX_ADDNODE, OnCntxAddnode)
	ON_COMMAND(ID_CNTX_CLEARNODE, OnCntxClearnode)
	ON_COMMAND(ID_CNTX_REMOVEALL, OnCntxRemoveall)
	ON_COMMAND(ID_CNTX_REMOVETREE, OnCntxRemovetree)
	ON_NOTIFY_REFLECT(TVN_ENDLABELEDIT, OnEndlabeledit)
	ON_COMMAND(ID_CNTX_RENAMENODE, OnCntxRenamenode)
	ON_NOTIFY_REFLECT(TVN_BEGINLABELEDIT, OnBeginlabeledit)
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRegistryTree message handlers



void CRegistryTree::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	CRegistryBrowserDlg *dlg = (CRegistryBrowserDlg *)(GetParent());

	CRegBrwNode *oldnode;
	if (pNMTreeView->itemOld.hItem) {
		oldnode = (CRegBrwNode *)GetItemData(pNMTreeView->itemOld.hItem);
	
		if (oldnode) {
			dlg->UpdateData(TRUE);
			if (oldnode->status == ATTSTATUS_HERE) {
				oldnode->value = dlg->m_regnodeValue;
				oldnode->value.Replace(_T("\r\n"),_T("\n")); // Remove Win32 GUI line ends
			}
		}
	}

	CRegBrwNode *newnode;
	if (pNMTreeView->itemNew.hItem) {
		newnode = (CRegBrwNode *)GetItemData(pNMTreeView->itemNew.hItem);

		if (newnode) {
			dlg->m_regnodePath = newnode->path;
			switch (newnode->status) {
			case ATTSTATUS_HERE:
				dlg->m_regnodeStatus = _T("HERE");
				break;
			case ATTSTATUS_METADEFAULT:
				dlg->m_regnodeStatus = _T("METADEFAULT");
				break;
			case ATTSTATUS_UNDEFINED:
				dlg->m_regnodeStatus = _T("UNDEFINED");
				break;
			case ATTSTATUS_INVALID:
				dlg->m_regnodeStatus = _T("INVALID");
				break;
			default:
				CString inherited;
				inherited.Format(_T("INHERITED (distance: %ld)"), newnode->status);
				dlg->m_regnodeStatus = inherited;
				break;
			}
			dlg->m_regnodeValue = newnode->value;
			dlg->m_regnodeValue.Replace(_T("\n"), _T("\r\n")); // Add Win32 GUI line ends
			dlg->UpdateData(FALSE);
		}
	}

	*pResult = 0;
}


void CRegistryTree::OnRButtonDown(UINT nFlags, CPoint point) 
{
	CMenu menu;
	menu.LoadMenu(IDR_CNTXMENU_REGBRW);
	CMenu* popupMenu = menu.GetSubMenu(0);
	popupMenu->EnableMenuItem(ID_CNTX_ADDNODE, MF_ENABLED);
	popupMenu->EnableMenuItem(ID_CNTX_CLEARNODE, MF_ENABLED);
	popupMenu->EnableMenuItem(ID_CNTX_REMOVETREE, MF_ENABLED);
	popupMenu->EnableMenuItem(ID_CNTX_RENAMENODE, MF_ENABLED);

	UINT uFlags;
	HTREEITEM hItem = 0, hOldItem = 0;

	hItem = HitTest(point, &uFlags);
	if (hItem && (TVHT_ONITEM & uFlags)) {
		hOldItem = GetSelectedItem();
		SelectItem(hItem);
		m_cntxSelected = hItem;
	}
	else {
		hOldItem = GetSelectedItem();
		hItem = NULL;
		SelectItem(hItem);
		m_cntxSelected = hItem;
	}

	if (!hItem) {
		popupMenu->EnableMenuItem(ID_CNTX_CLEARNODE, MF_GRAYED);
		popupMenu->EnableMenuItem(ID_CNTX_REMOVETREE, MF_GRAYED);
		popupMenu->EnableMenuItem(ID_CNTX_RENAMENODE, MF_GRAYED);
	}
	else {
		CRegBrwNode *node = (CRegBrwNode*)GetItemData(hItem);
		if (node->status != ATTSTATUS_HERE) {
			popupMenu->EnableMenuItem(ID_CNTX_CLEARNODE, MF_GRAYED);
		}
		if ( (node->status != ATTSTATUS_HERE) || ItemHasChildren(hItem)) {
			popupMenu->EnableMenuItem(ID_CNTX_RENAMENODE, MF_GRAYED);
		}
	}


	ClientToScreen(&point);
    int command = popupMenu->TrackPopupMenuEx(TPM_RETURNCMD, point.x, point.y, this, NULL);
    SendMessage(WM_COMMAND, command, 0);
    if (command != ID_CNTX_ADDNODE /* dont interfere with editing label */ && hOldItem)
        SelectItem(hOldItem);
	
	//CTreeCtrl::OnRButtonDown(nFlags, point);
}

void CRegistryTree::OnCntxAddnode() 
{
	HTREEITEM hItem = m_cntxSelected ? m_cntxSelected : TVI_ROOT;
	CRegistryBrowserDlg *dlg = (CRegistryBrowserDlg *)(GetParent());
	CRegBrwNode *oldNode = (hItem == TVI_ROOT) ? NULL : (CRegBrwNode*)GetItemData(hItem);
	
	CRegBrwNode *newNode = new CRegBrwNode();
	newNode->name = _T("New Node");
	newNode->parent = oldNode;
	newNode->status = ATTSTATUS_HERE;
	newNode->value = _T("");
	newNode->path = (hItem == TVI_ROOT) ? newNode->name : oldNode->path + _T("/") + newNode->name;
	
	int imageNum;
	dlg->m_imageMap.Lookup(IDI_ICON_REGHERE, imageNum);
	newNode->handle = InsertItem(newNode->name, imageNum, imageNum, hItem, TVI_LAST);
	SetItemData((HTREEITEM)newNode->handle, (DWORD_PTR) newNode);

	dlg->m_nodes.AddTail(newNode);
    if (hItem)
        VERIFY(Expand(hItem, TVE_EXPAND));
    VERIFY(SelectItem((HTREEITEM)newNode->handle));
    //UpdateWindow();

	// Dirty trick...
	CEdit *eLabel = EditLabel((HTREEITEM)newNode->handle);
	eLabel->SetWindowText(newNode->name);
	eLabel->SetSel(0, -1);
}

void CRegistryTree::OnCntxClearnode() 
{
	HTREEITEM hItem = m_cntxSelected;
	if (hItem) {
		CRegBrwNode* node= (CRegBrwNode*)GetItemData(hItem);
		node->status = ATTSTATUS_UNDEFINED;
		node->value = _T("");
		CRegistryBrowserDlg *dlg = (CRegistryBrowserDlg *)(GetParent());
		dlg->m_regnodeStatus = _T("UNDEFINED");
		dlg->m_regnodeValue = _T("");
		dlg->UpdateData(FALSE);
		int imageNum;
		dlg->m_imageMap.Lookup(IDI_ICON_REGUNDEF, imageNum);
		SetItemImage(hItem, imageNum, imageNum);
		AfxMessageBox(_T("Inherited nodes will not be shown until you save the changes.\nClick on the OK button, and start the Registry Browser again to see them !"), MB_ICONINFORMATION | MB_OK);
	}
}

void CRegistryTree::OnCntxRemoveall() 
{
	if( AfxMessageBox(_T("Would you like to delete all nodes?"), MB_YESNO | MB_ICONWARNING) == IDYES)
	{
		HTREEITEM hNextItem, hItem = GetRootItem();
		while ( hItem != NULL)
		{
			hNextItem = GetNextItem( hItem, TVGN_NEXT);
			RemoveSubTree(hItem);
			DeleteItem(hItem);
			hItem = hNextItem;
		}
		AfxMessageBox(_T("Inherited nodes will not be shown until you save the changes.\nClick on the OK button, and start the Registry Browser again to see them !"), MB_ICONINFORMATION | MB_OK);
	}
}

void CRegistryTree::OnCntxRemovetree() 
{
	HTREEITEM hItem = m_cntxSelected;
	if (hItem) {
		RemoveSubTree(hItem);
		DeleteItem(hItem);
		AfxMessageBox(_T("Inherited nodes will not be shown until you save the changes.\nClick on the OK button, and start the Registry Browser again to see them !"), MB_ICONINFORMATION | MB_OK);
	}
	
}

void CRegistryTree::RemoveSubTree(HTREEITEM hItem)
{
	CRegBrwNode* node = (CRegBrwNode*)GetItemData(hItem);
	node->status = ATTSTATUS_INVALID;

	if (ItemHasChildren(hItem)) {
		HTREEITEM hNextItem;
		HTREEITEM hChildItem = GetChildItem(hItem);

		while (hChildItem) {
			hNextItem = GetNextItem(hChildItem, TVGN_NEXT);
			RemoveSubTree(hChildItem);
			hChildItem = hNextItem;
		}

	}
}

void CRegistryTree::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;
	// TODO: Add your control notification handler code here
	if (pTVDispInfo->item.pszText) {
		HTREEITEM hItem = pTVDispInfo->item.hItem;
		if (hItem) {
				CRegBrwNode *node = (CRegBrwNode*)GetItemData(hItem);
				if (node) {
					if (CString(pTVDispInfo->item.pszText).Find('/') != -1) {
						AfxMessageBox(_T("Cannot rename registry node.\nThe name you specified contains '/' character. Specify a different name!"), MB_ICONSTOP | MB_OK);
						*pResult = FALSE;
						return;
					}

					if ( (node->status != ATTSTATUS_HERE) || ItemHasChildren(hItem)) {
						AfxMessageBox(_T("Only leaf nodes defined in this FCO can be renamed."), MB_ICONSTOP | MB_OK);
						*pResult = FALSE;
						return;
					}

					HTREEITEM hParent = GetParentItem(hItem);
					if (!hParent)
						hParent = TVI_ROOT;
					HTREEITEM hNextItem = GetChildItem(hParent);
					while (hNextItem) {
						CString nodeName = GetItemText(hNextItem);
						if ((nodeName == pTVDispInfo->item.pszText) && (hNextItem != hItem)) {
							AfxMessageBox(_T("Cannot rename registry node.\nA node with the name you specified already exists. Specify a different name!"), MB_ICONSTOP | MB_OK);
							CEdit *eLabel = EditLabel(hItem);
							eLabel->SetWindowText(pTVDispInfo->item.pszText);
							eLabel->SetSel(0,-1);
							*pResult = FALSE;
							return;
						}
						hNextItem = GetNextItem(hNextItem, TVGN_NEXT);
					}

					node->name = pTVDispInfo->item.pszText;
					int dStart = node->path.ReverseFind('/') + 1;
					int dCount = node->path.GetLength() - dStart;
					node->path.Delete(dStart, dCount);
					node->path += pTVDispInfo->item.pszText;

					if (GetSelectedItem() == hItem) {
						CRegistryBrowserDlg *dlg = (CRegistryBrowserDlg *)(GetParent());
						dlg->m_regnodePath = node->path;
						dlg->UpdateData(FALSE);
					}
				}
		}
	}
	*pResult = TRUE;
}

void CRegistryTree::OnCntxRenamenode() 
{
	HTREEITEM hItem = m_cntxSelected;
	if (hItem) {
		EditLabel(hItem);
	}
}

void CRegistryTree::OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;
	HTREEITEM hItem = pTVDispInfo->item.hItem;
	if (hItem) {
		CRegBrwNode *node = (CRegBrwNode*)GetItemData(hItem);
		if ( (node->status != ATTSTATUS_HERE) || ItemHasChildren(hItem))
			*pResult = TRUE;
		else
			*pResult = FALSE;
	}

}

void CRegistryTree::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	if (nChar == VK_DELETE) {
		HTREEITEM hItem = GetSelectedItem();
		if (hItem) {
			m_cntxSelected = hItem;
			SendMessage(WM_COMMAND, ID_CNTX_REMOVETREE);
		}
	}
    if (nChar == VK_F2) {
        HTREEITEM hItem = GetSelectedItem();
        if (hItem) {
            m_cntxSelected = hItem;
            SendMessage(WM_COMMAND, ID_CNTX_RENAMENODE);
        }
    }
    CTreeCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}
