// ScriptListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ModelMigrate.h"
#include "ScriptListCtrl.h"
#include "FileTransDlg.h"
#include <algorithm>

#define ADD_FILE_CMD                         2101
#define ADD_DIR_CMD                          2102
#define REMOVE_SEL_CMD                       2103
#define REMOVE_ALL_CMD                       2104
#define PROCESS_SEL_CMD                      2105
#define PROCESS_ALL_CMD                      2106
// ScriptListCtrl

IMPLEMENT_DYNAMIC(ScriptListCtrl, CListCtrl)
ScriptListCtrl::ScriptListCtrl()
	: m_rowID(0)
{
}

ScriptListCtrl::~ScriptListCtrl()
{
}


BEGIN_MESSAGE_MAP(ScriptListCtrl, CListCtrl)
	ON_WM_CREATE()
	ON_WM_DROPFILES()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()



// ScriptListCtrl message handlers


int ScriptListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

BOOL ScriptListCtrl::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style |= WS_CHILD|WS_VISIBLE|WS_BORDER|WS_TABSTOP|LVS_REPORT|LVS_NOSORTHEADER;//|LVS_SINGLESEL;
	return CListCtrl::PreCreateWindow(cs);
}

void ScriptListCtrl::addFile(const CString& file, const CString& path)
{	
	CString knd = file, ttt = path;
	LV_ITEM lvItem;
	lvItem.mask = LVIF_TEXT;
	lvItem.iItem = GetItemCount();
	lvItem.iSubItem = 0;
	lvItem.pszText = knd.GetBuffer(knd.GetLength());
	int index = InsertItem(&lvItem);
	
	lvItem.iSubItem = 1;
	lvItem.pszText = ttt.GetBuffer(ttt.GetLength());
	SetItem(&lvItem);

	//lvItem.iSubItem = 2;
	//lvItem.pszText = _T("");
	//SetItem(&lvItem);

	//lvItem.iSubItem = 3;
	//lvItem.pszText = _T("");
	//SetItem(&lvItem);


	SetItemData(index, ++m_rowID);
}

void ScriptListCtrl::PreSubclassWindow()
{
	int col0size = 2*GetStringWidth("FileName.xme");
	int col1size = 2*GetStringWidth("Full Path To File Is Very Long");
	//int col2size = 2*GetStringWidth("Status");
	//int col3size = 2*GetStringWidth("MessageWillBeVeryLong");

	InsertColumn(0, _T("Scripts to apply"), LVCFMT_LEFT,  col0size, -1);
	InsertColumn(1, _T("Full Path"), LVCFMT_LEFT, col1size, -1);
	//InsertColumn(2, _T("Status"), LVCFMT_LEFT, col2size, -1);
	//InsertColumn(3, _T(" "), LVCFMT_LEFT, col3size, -1);

	SetExtendedStyle( LVS_EX_FULLROWSELECT );

	CListCtrl::PreSubclassWindow();
}

void ScriptListCtrl::OnDropFiles(HDROP hDropInfo)
{
	UINT nFiles = DragQueryFile( hDropInfo, 0xFFFFFFFF, NULL, 0);
	if( nFiles < 1)
		AfxMessageBox( "No file dropped!");

	for( UINT iF = 0; iF < nFiles; ++iF)
	{
		TCHAR szFileName[_MAX_PATH];
		UINT res = DragQueryFile( hDropInfo, iF, szFileName, _MAX_PATH);
		if (res > 0)
		{
			CString conn( szFileName);
			int pos = conn.ReverseFind('\\');
			
			if( pos != -1)
				addFile( conn.Right( conn.GetLength() - pos - 1), szFileName);
			else
				addFile( szFileName, "");
		}
		else
			AfxMessageBox( "File name error!");
	}

	CListCtrl::OnDropFiles(hDropInfo);
}

std::vector<std::string> ScriptListCtrl::getSelFiles()
{
	std::vector< std::string> res;
	for( int i = 0; i < GetItemCount(); ++i)
	{
		CString knd;
		LV_ITEM lvItem;
		lvItem.mask = LVIF_TEXT | LVIF_STATE;
		lvItem.iItem = i;
		lvItem.iSubItem = 1;
		lvItem.pszText = knd.GetBuffer( 1024);
		lvItem.cchTextMax = 1024;
		lvItem.stateMask = LVIS_SELECTED;
		GetItem( &lvItem);

		// reset 'status'
		//lvItem.iSubItem = 2;
		//lvItem.pszText = "";
		//SetItem( &lvItem);

		if( (lvItem.state & LVIS_SELECTED) == LVIS_SELECTED)
			res.push_back( (LPCTSTR) knd );
	}

	return res;
}

std::vector<std::string> ScriptListCtrl::getAllFiles()
{
	std::vector< std::string> res;
	for( int i = 0; i < GetItemCount(); ++i)
	{
		CString knd;
		LV_ITEM lvItem;
		lvItem.mask = LVIF_TEXT;
		lvItem.iItem = i;
		lvItem.iSubItem = 1;
		lvItem.pszText = knd.GetBuffer( 1024);
		lvItem.cchTextMax = 1024;
		GetItem( &lvItem); 

		//lvItem.iSubItem = 2;
		//lvItem.pszText = "";
		//SetItem( &lvItem);

		res.push_back( (LPCTSTR) knd );
	}

	return res;
}

void ScriptListCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	bool sent = false;
	CListCtrl::OnLButtonDblClk(nFlags, point);
}


void ScriptListCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
	CListCtrl::OnRButtonDown(nFlags, point);

	ClientToScreen( &point);
	createPopupMenu( point);
}

void ScriptListCtrl::showPopupMenu()
{
	// find the focused item to determine the position of the popup menu
	int idx = -1;
	for( int i = 0; idx == -1 && i < GetItemCount(); ++i)
	{
		UINT flag = LVIS_FOCUSED;// | LVIS_SELECTED;
		if( (GetItemState( i, flag ) & flag) == flag )
		{
			idx = i;
		}
	}

	CPoint point;
	if( idx != -1) 
		GetItemPosition( idx, &point);
	else
	{
		// no item found, use the top-left corner of the empty listcontrol
		CRect rect;
		GetClientRect(rect);
		point = CPoint( rect.left, rect.top);
	}
	point.Offset( 10, 10);

	ClientToScreen( &point);
	createPopupMenu( point);
}

void ScriptListCtrl::createPopupMenu( CPoint pnt)
{
	CMenu m;
	m.CreatePopupMenu();
	m.AppendMenu( MF_ENABLED|MF_STRING, ADD_FILE_CMD,     "Add Script...");
	m.AppendMenu( MF_ENABLED|MF_STRING, ADD_DIR_CMD,      "Add Directory...");
	m.AppendMenu( MF_ENABLED|MF_SEPARATOR, 0,             "");
	m.AppendMenu( MF_ENABLED|MF_STRING, REMOVE_ALL_CMD,   "Remove All");
	m.AppendMenu( MF_ENABLED|MF_STRING, REMOVE_SEL_CMD,   "Remove Selected");
	//m.AppendMenu( MF_ENABLED|MF_SEPARATOR, 0,             "");
	//m.AppendMenu( MF_ENABLED|MF_STRING, PROCESS_ALL_CMD,  "Process All");
	//m.AppendMenu( MF_ENABLED|MF_STRING, PROCESS_SEL_CMD,  "Process Selected");
	m.TrackPopupMenu( TPM_LEFTALIGN |TPM_RIGHTBUTTON, pnt.x, pnt.y, this);
}

BOOL ScriptListCtrl::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch( wParam)
	{
	case ADD_FILE_CMD: m_parent->selScript();break;
	case ADD_DIR_CMD:  m_parent->selScriptDir();break;
	case REMOVE_ALL_CMD: removeAll();break;
	case REMOVE_SEL_CMD: removeSel();break;
	//case PROCESS_SEL_CMD: m_parent->processSel();break;
	//case PROCESS_ALL_CMD: m_parent->processAll();break;
	};
	return CListCtrl::OnCommand(wParam, lParam);
}

void ScriptListCtrl::mark( std::vector< std::string>& vecs)
{
}

void ScriptListCtrl::removeAll()
{
	DeleteAllItems();
}

void ScriptListCtrl::removeSel()
{
	for( int i = GetItemCount(); i > 0 ; --i)
	{
		UINT sel = GetItemState( i-1, LVIS_SELECTED);
		if( sel & LVIS_SELECTED)
		{
			DeleteItem( i-1);
		}
	}
}

