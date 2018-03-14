// SearchDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Search.h"
#include "SearchDlg.h"
#include "SearchCtl.h"
#include "Input.h"
#include "SearchAlg.h"
#include <string>
#include <sstream>
#include <stack>

#include "..\Gme\GMEOLEData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


BEGIN_MESSAGE_MAP(AutocompleteComboBox, CComboBox)
	ON_CONTROL_REFLECT(CBN_EDITCHANGE, OnEditChange)
END_MESSAGE_MAP()

LRESULT AutocompleteComboBox::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if (message == WM_KEYDOWN && wParam == VK_DOWN)
	{ // FIXME this doesn't work
		this->ShowDropDown();
		return 0;
	}

	return __super::WindowProc(message, wParam, lParam);
}

void AutocompleteComboBox::OnEditChange()
{
	CString str;
	GetWindowTextW(str);
	DWORD sel = GetEditSel();
	if (m_previous.GetLength() >= str.GetLength() || wcsncmp(m_previous, str, m_previous.GetLength()) != 0)
	{
		m_previous = str;
		return;
	}
	auto substr_eq = [&str](const wchar_t* str2) { return wcsncmp(str, str2, str.GetLength()) == 0; };
	ResetContent();
	auto it = std::find_if(m_options.begin(), m_options.end(), substr_eq);
	auto first = it;
	for (; it != m_options.end() && substr_eq(*it); ++it)
	{
		InsertString(-1, *it);
	}
	if (first != m_options.end())
	{
		SetWindowTextW(*first);
		SetEditSel(sel, -1);
	}
	else
	{
		SetWindowTextW(str);
		SetEditSel(sel, sel);
	}
	m_previous = str;
}


/////////////////////////////////////////////////////////////////////////////
// CSearchDlg dialog


CSearchDlg::CSearchDlg()
: m_scopedCtrlEnabled( FALSE)
, m_edtNameSecond(_T(""))
, m_edtRoleNameSecond(_T(""))
, m_edtKindNameSecond(_T(""))
, m_edtAttributeSecond(_T(""))
, m_radioScope(0)
, m_searchResults(FALSE)
, m_radioLogical(0)
, m_edtNameFirst( _T(""))
, m_edtKindNameFirst( _T(""))
, m_edtRoleNameFirst( _T(""))
, m_edtAttributeFirst( _T(""))
, m_edtAttrValue ( _T(""))
, m_chkAtom(TRUE)
, m_chkFullWord(FALSE)
, m_chkMod(TRUE)
, m_chkRef (TRUE)
, m_chkSet(TRUE)
, m_chkConnection(TRUE)
, m_chkLocate(FALSE)
, m_chkMatchCase(TRUE)

, m_chkSplSearch(FALSE)
{
}

void CSearchDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CSearchDlg)
    DDX_Control(pDX, IDC_CHECK_CASESENS, m_chkMatchCaseCtrl);
    DDX_Control(pDX, IDC_CHECKFULLWORD, m_chkFullWordCtrl);
    DDX_Control(pDX, IDC_EDITNAME, m_edtNameCtrlFirst);
    DDX_Control(pDX, IDC_EDITKIND, m_edtKindNameCtrlFirst);
    DDX_Control(pDX, IDC_EDITROLE, m_edtRoleNameCtrlFirst);
    DDX_Control(pDX, IDC_EDITATTRNAME, m_edtAttributeCtrlFirst);
    DDX_Control(pDX, IDC_CHECKREF, m_chkRefCtrl);
    DDX_Control(pDX, IDC_PROGRESSSEARCH, m_pgsSearch);
    DDX_Control(pDX, IDC_CHECKATM, m_chkAtomCtrl);
    DDX_Control(pDX, IDC_CHECKSET, m_chkSetCtrl);
    DDX_Control(pDX, IDC_CHECKMOD, m_chkModCtrl);
    DDX_Control(pDX, IDC_BUTTON_GO, m_btnGO);
    DDX_Control(pDX, IDC_LISTRESULTS, m_lstResults);
    DDX_Text(pDX, IDC_EDITNAME, m_edtNameFirst);
    DDX_Text(pDX, IDC_EDITKIND, m_edtKindNameFirst);
    DDX_Text(pDX, IDC_EDITROLE, m_edtRoleNameFirst);
    DDX_Text(pDX, IDC_EDITATTRNAME, m_edtAttributeFirst);
    DDX_Check(pDX, IDC_CHECKATM, m_chkAtom);
    DDX_Check(pDX, IDC_CHECKFULLWORD, m_chkFullWord);
    DDX_Check(pDX, IDC_CHECKMOD, m_chkMod);
    DDX_Check(pDX, IDC_CHECKREF, m_chkRef);
    DDX_Check(pDX, IDC_CHECKSET, m_chkSet);
    DDX_Check(pDX, IDC_CHECK_CASESENS, m_chkMatchCase);
    DDX_Text(pDX, IDC_EDITNAME2, m_edtNameSecond);
    DDX_Text(pDX, IDC_EDITROLE2, m_edtRoleNameSecond);
    DDX_Text(pDX, IDC_EDITKIND2, m_edtKindNameSecond);
    DDX_Text(pDX, IDC_EDITATTRVALU2, m_edtAttributeSecond);
    DDX_Control(pDX, IDC_RADIOAND, m_logicalGrp);
    DDX_Radio(pDX, IDC_ENTIRESCOPE2, m_radioScope);
    DDX_Check(pDX, IDC_CHECK_RESULTS, m_searchResults);
    DDX_Radio(pDX, IDC_RADIOAND, m_radioLogical);
    DDX_Control(pDX, IDC_EDITNAME2, m_edtNameCtrlSecond);
    DDX_Control(pDX, IDC_EDITROLE2, m_edtRoleNameCtrlSecond);
    DDX_Control(pDX, IDC_EDITKIND2, m_edtKindNameCtrlSecond);
    DDX_Control(pDX, IDC_EDITATTRVALU2, m_edtAttributeCtrlSecond);
    DDX_Control(pDX, IDC_TREE_SEARCH_HISTORY, m_treeSearchHistory);
    DDX_Check(pDX, IDC_CHECKCON, m_chkConnection);
    DDX_Check(pDX, IDC_CHECKSPLSEARCH, m_chkSplSearch);
    DDX_Control(pDX, IDC_STATICREF, m_stcRefCtrl);
    DDX_Control(pDX, IDC_CHECKCON, m_chkConnCtrl);
}

BEGIN_MESSAGE_MAP(CSearchDlg, CDialog)
    //{{AFX_MSG_MAP(CSearchDlg)
    ON_BN_CLICKED(IDC_BUTTON_GO, OnButtonGo)
    ON_NOTIFY(NM_CLICK, IDC_LISTRESULTS, OnClickListResults)
    ON_NOTIFY(LVN_ITEMCHANGED, IDC_LISTRESULTS, OnClickListResults)
    ON_NOTIFY(NM_DBLCLK, IDC_LISTRESULTS, OnDblclkListResults)
    ON_NOTIFY(LVN_KEYDOWN, IDC_LISTRESULTS, OnKeyDownListResults)
    //}}AFX_MSG_MAP
    ON_WM_SIZE()
    ON_WM_SIZING()
    ON_WM_VSCROLL()
    ON_WM_HSCROLL()
    ON_WM_KEYDOWN()
    ON_NOTIFY(NM_DBLCLK, IDC_TREE_SEARCH_HISTORY, &CSearchDlg::OnNMDblclkTreeSearchHistory)
    ON_BN_CLICKED(IDC_CHECKSPLSEARCH, &CSearchDlg::OnCheckSplSearch)
    ON_NOTIFY(LVN_COLUMNCLICK, IDC_LISTRESULTS, &CSearchDlg::OnLvnColumnclickListresults)
    ON_BN_CLICKED(IDC_BUTTON_CLEAR, &CSearchDlg::OnBnClickedButtonClear)
END_MESSAGE_MAP()

BOOL CSearchDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    //initialize sort array
    ascending[0]=ascending[1]=ascending[2]=ascending[3]=true;

    //column headers
    m_lstResults.InsertColumn(1, _T("Object"), LVCFMT_LEFT, 100, 0);
    m_lstResults.InsertColumn(2, _T("Path"), LVCFMT_LEFT, 210, 1);
    m_lstResults.InsertColumn(3, _T("Kind"), LVCFMT_LEFT, 95, 2);
   // m_lstResults.InsertColumn(4, _T("Value"), LVCFMT_LEFT, 210, 3);
    m_lstResults.SetExtendedStyle(m_lstResults.GetExtendedStyle()|LVS_EX_FULLROWSELECT);

    specialSearchFCO = NULL;

    m_pgsSearch.SetRange(1,16000);
    m_pgsSearch.SetStep(1);

	InsertTextToControl(CString(L"_id="), m_edtAttributeCtrlFirst);
	InsertTextToControl(CString(L"_guid="), m_edtAttributeCtrlFirst);
	// InsertTextToControl(CString(L"_abspath="), m_edtAttributeCtrlFirst);

    //load search history from registry
    LoadSearchHistory();

	dropTarget.Register(this);

	return true;
}

/////////////////////////////////////////////////////////////////////////////
// CSearchDlg message handlers


/////////////////////////////////////////////////////////////////////////////
//  These functions should be implemented to work with CSearchCtl

// Must remove all search result
void CSearchDlg::RemoveAll()
{
    results = NULL;
    COMTHROW(results.CoCreateInstance(L"Mga.MgaFCOs"));
    DisplayResults();
	results = NULL;
}

// Must remove results belong to zombie objects
void CSearchDlg::RemoveZombies()
{
	if (results == NULL)
		return;

    CSearchCtrl *TheCtrl = GetCtrl();
    TheCtrl->BeginTransaction();

	bool zombieFound = ::RemoveZombies<IMgaFCO, IMgaFCOs>(results);

    if(zombieFound) //only redraw if we had to remove a zombie
    {
        DisplayResults();
    }

    if(specialSearchFCO != NULL)
    {	//test if specialSearchFCO became a zombie
        long oStatus;
        COMTHROW(specialSearchFCO->get_Status(&oStatus));
        if(oStatus != OBJECT_EXISTS)
        {
            specialSearchFCO = NULL;
            m_stcRefCtrl.SetWindowText(_T("NULL References"));
			m_stcRefCtrl.ShowWindow(TRUE);
        }
    }

    TheCtrl->CommitTransaction();
}

// Enable search 
//enables all the  controls in the search dialog
void CSearchDlg::EnableSearch()
{
    //Iterate through all the child controls and enable it
    HWND hwnd = ::GetWindow( GetSafeHwnd(),
        GW_CHILD);

    while( hwnd )
    {
        ::EnableWindow(hwnd,true);
        // Get the next window. Use it.
        hwnd = ::GetWindow( hwnd, GW_HWNDNEXT |GW_HWNDFIRST);
    }
}

//disables all the  controls in the search dialog
void CSearchDlg::DisableSearch()
{

    //Iterate through all the child controls and disable it
    HWND hwnd = ::GetWindow( GetSafeHwnd(),
        GW_CHILD |GW_HWNDFIRST);
    while( hwnd )
    {
        ::EnableWindow(hwnd,false);
        // Get the next window. Use it.
        hwnd = ::GetWindow( hwnd, GW_HWNDNEXT );
    }
}


void CSearchDlg::OnButtonGo() 
{
	// FIXME: why do neither ON_NOTIFY(LVN_ITEMACTIVATE nor NM_RETURN work?
	HWND h = ::GetFocus();
	while (h) {
		if (h == GetDlgItem(IDC_LISTRESULTS)->GetSafeHwnd()) {
			return itemDblClicked();
		}
		h = ::GetParent(h);
	}
    if(CWnd::UpdateData(TRUE))
    {
        if (m_searchResults)
        {
			if (results != nullptr) // TODO: should probably EnableWindow(FALSE) the Search button instead
			{
				SearchResults();
				CreateSearchHistory();
			}
            return;
        }

        CSearchCtrl *TheCtrl = GetCtrl();
        CComPtr<IMgaProject> ccpProject = TheCtrl->GetProject();

        //show progress on especially long searches
        m_pgsSearch.ShowWindow(SW_RESTORE);
        RemoveAll();
        //		AfxMessageBox(_T("Searching"));

        TheCtrl->BeginTransaction();
        CComPtr<IMgaObjects> ccpObjectsInTerr = TheCtrl->PutInMyTerritory( TheCtrl->GetScopedL());

        //the CInput class is a legacy from the Search Add-on
        //it has functionality for local searching, 
        //which is not used here. (hence the NULL's and 0's below)
        CComPtr<IMgaFolder> rootInput;
        COMTHROW(ccpProject->get_RootFolder(&rootInput));
        CInput inp;
        try
        {
            //this might throw error related to regular expression
            inp.GetInput(m_edtNameFirst,m_edtRoleNameFirst,m_edtKindNameFirst,m_edtAttributeFirst,m_edtNameSecond,m_edtRoleNameSecond,m_edtKindNameSecond,m_edtAttributeSecond,m_edtAttrValue,
            m_chkMod,m_chkAtom,m_chkRef,m_chkSet,m_chkConnection,m_chkSplSearch,m_chkFullWord,NULL,0,m_chkMatchCase,m_radioScope,m_radioLogical);

			if (results == NULL)
				COMTHROW(results.CoCreateInstance(L"Mga.MgaFCOs"));
			CSearch searchGME(inp);
			searchGME.Search(rootInput, ccpObjectsInTerr, specialSearchFCO,results,&m_pgsSearch);
			DisplayResults();

		}
        catch (std::tr1::regex_error& err)
        {
            m_pgsSearch.ShowWindow(SW_HIDE);
            m_pgsSearch.SetPos(1);
            AfxMessageBox(_T("Make sure your input is correct. If you are using")
                _T(" regular expression elements\n like *, + etc make sure you are")
                _T(" following proper syntax"));
            TheCtrl->AbortTransaction();

            return;
        }

        m_pgsSearch.ShowWindow(SW_HIDE);
        m_pgsSearch.SetPos(1);

        TheCtrl->CommitTransaction();
    }
    CreateSearchHistory();
    SaveSearchHistory();

}

// recursively build a path of names from the rootFCO to "named" used in the original call
void CSearchDlg::BuildExtendedName(IMgaFCO *named, CString &extName)
{
    if (named != NULL)
    {
        CComPtr<IMgaModel> parent;
        COMTHROW( named->get_ParentModel(&parent));
        if (parent != NULL)
        {
            BuildExtendedName(parent, extName);
            extName += _T(" : ");
        }
        else //parent is a Folder
        {
            CComPtr<IMgaFolder> parentFolder = NULL;
            COMTHROW( named->get_ParentFolder(&parentFolder));
            BuildExtendedName(parentFolder, extName);
            extName += _T(" : ");
        }
        CBstr bstr;
        COMTHROW( named->get_Name(bstr));
        extName += bstr;
    }

}

// Get the parent folders' names, too
void CSearchDlg::BuildExtendedName(IMgaFolder *named, CString &extName)
{
    if (named != NULL)
    {
        CComPtr<IMgaFolder> parent;
        COMTHROW( named->get_ParentFolder(&parent));
        if (parent != NULL)
        {
            BuildExtendedName(parent, extName);
            extName += _T(" : ");
        }
        CBstr bstr;
        COMTHROW( named->get_Name(bstr));
        extName += bstr;
    }
}

//display all found items at once
void CSearchDlg::DisplayResults()
{
    int count = 0;
    CString name;
    CString path, kind;
   
    if(!m_chkSplSearch) //only want to wipe this out on a regular search
	{
		specialSearchFCO = NULL;
		m_stcRefCtrl.SetWindowText(_T("NULL References"));
		m_stcRefCtrl.ShowWindow(TRUE);
	}


    //Hide the window so it doesn't waste time redrawing each time we add an item
    m_lstResults.SetRedraw(FALSE);
    m_lstResults.DeleteAllItems();

    MGACOLL_ITERATE(IMgaFCO, results)
    {
        path = _T("");
        name = _T("");
        kind = _T("");
        
        CBstr bstr;
        COMTHROW( MGACOLL_ITER->get_Name(bstr) );
        name += bstr;

        BuildExtendedName(MGACOLL_ITER.p, path);

        //get the KindName, instead of displaying the type such as Model, Atom etc
        //the kind name will be shown
        CBstr bstrKindName;
        CComPtr<IMgaMetaFCO> cmeta;
        COMTHROW( MGACOLL_ITER->get_Meta(&cmeta) );
        COMTHROW( cmeta->get_Name(bstrKindName) );

        //set the kind to kind name
        kind = bstrKindName;

        //use LVITEM to insert item into list control, LVITEM is needed so that
        //lParam can be supplide which will be used during sort
        //Object
        LVITEM lvItem;
        lvItem.lParam = count;
        lvItem.iItem = count;
        lvItem.iSubItem = 0;
        lvItem.pszText=name.GetBuffer();
        lvItem.mask = LVIF_PARAM | LVIF_TEXT;
        m_lstResults.InsertItem(&lvItem);
        
        //don't supply lParam for subitems, if you do that the subitems won't get
        //displayed properly
        //path
        lvItem.mask = LVIF_TEXT;
        lvItem.iSubItem = 1;
        lvItem.pszText = path.GetBuffer();
        m_lstResults.SetItem(&lvItem);
 
        //kind
        lvItem.iSubItem = 2;
        lvItem.pszText = kind.GetBuffer();
        m_lstResults.SetItem(&lvItem);
      
        count++;

        //let the user know the searcher is still alive for especially long result lists
        //(before, when this function didn't hide the m_lstResults control while adding stuff, 
        //it would take much more time than the actual searching for long result lists)
        m_pgsSearch.StepIt();

    } MGACOLL_ITERATE_END;

    if (count==0)
    {
        m_lstResults.InsertItem(count, _T(""));
        m_lstResults.SetItemText(count, 1, _T("No Matching Results"));
    }

    //Now that everything is added, allow the display to redraw
    m_lstResults.SetRedraw(TRUE);
}

void CSearchDlg::OnClickListResults(NMHDR* pNMHDR, LRESULT* pResult) 
{
    this->itemClicked(); *pResult = 0;
}

void CSearchDlg::OnDblclkListResults(NMHDR* pNMHDR, LRESULT* pResult) 
{
    this->itemDblClicked(); *pResult = 0;
}

void CSearchDlg::OnKeyDownListResults(NMHDR* pNMHDR, LRESULT* pResult)
{
	LV_KEYDOWN* pLVKeyDown = (LV_KEYDOWN*)pNMHDR;
	*pResult = 0;

	long count = 0;
    if (results)
		COMTHROW(results->get_Count(&count));

	if (count > 0 && pLVKeyDown->wVKey == VK_DELETE)
    {
        CWnd::UpdateData(TRUE);

        CSearchCtrl *TheCtrl = GetCtrl();
		try 
		{
			TheCtrl->BeginTransaction(TRANSACTION_GENERAL);
		}
		catch (const hresult_exception&)
		{
			return;
		}
        try
        {
			ForEachSelectedFCO([&](CComPtr<IMgaFCO> selectedFCO) {
				COMTHROW(selectedFCO->DestroyObject());
			});
            TheCtrl->CommitTransaction();
			// RemoveZombies
        }
        catch(...)
        {
            TheCtrl->AbortTransaction();
        }

    }

}


void CSearchDlg::OnSize(UINT nType, int cx, int cy)
{
    CRect dialogRect;
    GetWindowRect( & dialogRect); ScreenToClient(&dialogRect);

    CDialog::OnSize(nType, cx, cy);
    const int bottomMargin = 10;
    const int rightMargin = 10;
    if( cx >= 0 && cy >= 0 && m_lstResults.GetSafeHwnd()&& m_treeSearchHistory.GetSafeHwnd())
    {
        int bottomPos = (cy - bottomMargin) > 0 ? (cy - bottomMargin) : 0;

        int rightPos = (cx - rightMargin) > 0? (cx - rightMargin) : 0;
        
        CRect rectResultsList; m_lstResults.GetWindowRect( &rectResultsList); ScreenToClient( &rectResultsList);
        CRect rectPrevSearches;m_treeSearchHistory.GetWindowRect(rectPrevSearches); ScreenToClient(rectPrevSearches);
        
        //set search history tree to fixed width, it  will move to reflect
        //resizing but would have fixed width
        rectPrevSearches.bottom = bottomPos;
        rectPrevSearches.left = rightPos - rectPrevSearches.Width();
        rectPrevSearches.right =  rightPos;

        m_treeSearchHistory.SetWindowPos(NULL, rectPrevSearches.left, rectPrevSearches.top, rectPrevSearches.Width(), 
        rectPrevSearches.Height(), SWP_NOZORDER|SWP_SHOWWINDOW);

        //set the title to appropriate position
        //because search history changed its position the title "Previous Searches"
        //also needs to be moved
        CRect rectPrevSearchesTitle; 
        CWnd *prevSrchTitle = GetDlgItem (IDC_PREVSRCH_TITLE);
        prevSrchTitle->GetWindowRect(&rectPrevSearchesTitle);ScreenToClient(&rectPrevSearchesTitle);

        int width = rectPrevSearchesTitle.Width();
        rectPrevSearchesTitle.left = rectPrevSearches.left;
        rectPrevSearchesTitle.right = rectPrevSearchesTitle.left + width;
        prevSrchTitle->SetWindowPos(NULL, rectPrevSearchesTitle.left, rectPrevSearchesTitle.top, rectPrevSearchesTitle.Width(),
            rectPrevSearchesTitle.Height(), SWP_NOZORDER | SWP_SHOWWINDOW);

        //compute width for search results based on search history's location
        //search results list will be resized to take all the width after search history
        // is placed
        rectResultsList.bottom = bottomPos;
        rectResultsList.right = rectPrevSearches.left - 10;

        m_lstResults.SetWindowPos( NULL, rectResultsList.left, rectResultsList.top, rectResultsList.Width() , rectResultsList.Height(), SWP_NOMOVE|SWP_NOZORDER|SWP_SHOWWINDOW);
    }
}

void CSearchDlg::OnSizing(UINT fwSide, LPRECT pRect)
{
    CDialog::OnSizing(fwSide, pRect);
    // GMESearch::OnSizing() responsible for preventing shrinking the window too much
}

void CSearchDlg::clickGo()
{
    this->OnButtonGo();
}

void CSearchDlg::itemClicked()
{
    long count = 0;
    if( results) COMTHROW(results->get_Count(&count));

    if(count > 0 ) //don't want to grab stuff out of an empty list
    {

        CWnd::UpdateData(TRUE);

        CSearchCtrl *TheCtrl = GetCtrl();
        try
        {
            TheCtrl->BeginTransaction();

            int selected = m_lstResults.GetSelectionMark() + 1; //IMgaFCOs 1 based, GetSelectionMark 0 based
            DWORD_PTR lParam = m_lstResults.GetItemData(selected - 1);
            CComPtr<IMgaFCO> selectedFCO;
            // selected might be 0 because GetSelectionMark might have returned -1
            if( selected >= 1 && selected <= count)
            {
                COMTHROW(results->get_Item(lParam+1, &selectedFCO)); // crashed probably when called with 0

                //want to be able to search for references to this object in the special search
                CBstr bstr;
                CString name;
                COMTHROW( selectedFCO->get_Name(bstr));
                name = CString( bstr) + _T(" References");
                m_stcRefCtrl.SetWindowText(name);
				m_stcRefCtrl.ShowWindow(TRUE);
                specialSearchFCO = selectedFCO;
            }

			CComPtr<IMgaObjects> selectedObjects;
			COMTHROW(selectedObjects.CoCreateInstance(L"Mga.MgaObjects", 0, CLSCTX_INPROC));
			ForEachSelectedFCO([&](CComPtr<IMgaFCO> selected) {
				CComQIPtr<IMgaObject> selectedObject(selected);
				selectedObjects->Append(selectedObject);
			});
			
            TheCtrl->CommitTransaction();
            TheCtrl->ClickOnObject(selectedObjects);
        }
        catch( ...)
        {
            TheCtrl->AbortTransaction();
        }
    }
}

void CSearchDlg::itemDblClicked()
{
    long count = 0;
    if( results) COMTHROW(results->get_Count(&count));


    if(count > 0 ) //don't want to grab stuff out of an empty list
    {
        CWnd::UpdateData(TRUE);

        CSearchCtrl *TheCtrl = GetCtrl();
        try
        {
            // non-model children of Folders can't be shown, only located
            bool must_do_locate = false;

            TheCtrl->BeginTransaction();

            int selected = m_lstResults.GetSelectionMark() + 1; //IMgaFCOs 1 based, GetSelectionMark 0 based
            //LVITEM lvItem;
            DWORD_PTR lParam = m_lstResults.GetItemData(selected-1);
           // m_lstResults.GetItem(&lvItem);
            CComPtr<IMgaFCO> selectedFCO;
            // selected might be 0 because GeSelectionMark might have returned -1
            if( selected >= 1 && selected <= count)
            {
                COMTHROW(results->get_Item(lParam+1,&selectedFCO)); // crashed probably when called with 0
            }

            //CComPtr<IMgaObject> selectedObject = (IMgaObject *)(selectedFCO.p); // WAS this the scapegoat?
            CComQIPtr<IMgaObject> selectedObject( selectedFCO);
            TheCtrl->CommitTransaction();
            if( selectedObject)
            {
                TheCtrl->LocateMgaObject(selectedObject);
            }
        }
        catch(...)
        {
            TheCtrl->AbortTransaction();
        }

    }
}

//save search history in registry
void CSearchDlg::SaveSearchHistory()
{
    CRegKey key;
    std::wstringstream stream;

    //delete the key first and save new one
    if(key.Open(HKEY_CURRENT_USER,_T("Software\\GME"),KEY_ALL_ACCESS)==ERROR_SUCCESS)
    {
        key.DeleteSubKey(_T("Search"));
    }
    key.Create(HKEY_CURRENT_USER,_T("Software\\GME\\Search"));
    HTREEITEM hItem=m_treeSearchHistory.GetRootItem();
    int i=0;
    while(hItem && i<10)
    {
        //the search history are stored as search0, search1 etc 
        //form key string
        stream<<_T("search")<<i;

        //set the key and the value
        key.SetStringValue(stream.str().c_str(), m_treeSearchHistory.GetItemText(hItem));

        //clear the string stream
        stream.str(_T(""));

        //get next item to be saved
        hItem=m_treeSearchHistory.GetNextSiblingItem(hItem);
        i++;

    }
    key.Close();
}

void CSearchDlg::LoadSearchHistory()
{
    CRegKey key;
    std::wstring strKeyName;
    std::wstringstream stream(strKeyName);
    ULONG length;
    TCHAR strValue[1000];
    //CString strValue;

    int i=0;
    if(key.Open(HKEY_CURRENT_USER,_T("Software\\GME\\Search"),KEY_ALL_ACCESS)==ERROR_SUCCESS)
    {
        while(i<10)
        {
            //form the key
            strKeyName=_T("search");
            stream<<int(i);

            //buffer size
            length=sizeof(strValue)/sizeof(strValue[0]);

            strKeyName+=stream.str();

            //query the key
            if(key.QueryStringValue(strKeyName.c_str(),strValue,&length)!=ERROR_SUCCESS)
                break;

            //insert it in the tree control and combo boxes
            InsertHistory(strValue);
            stream.str(_T(""));
            i++;
        }
    }
}


//create search history text and insert it intt the tree control
//called after the user clicks the search button and results are available

void CSearchDlg::CreateSearchHistory()
{
    //variable to store if criteria is empty
    bool firstCriteriaEmpty=false,secondCriteriaEmpty=false;

    InsertTextToControl(m_edtNameFirst,m_edtNameCtrlFirst);
    InsertTextToControl(m_edtRoleNameFirst,m_edtKindNameCtrlFirst);
    InsertTextToControl(m_edtKindNameFirst,m_edtKindNameCtrlFirst);
    InsertTextToControl(m_edtAttributeFirst,m_edtAttributeCtrlFirst);

    InsertTextToControl(m_edtNameSecond,m_edtNameCtrlSecond);
    InsertTextToControl(m_edtRoleNameSecond,m_edtRoleNameCtrlSecond);
    InsertTextToControl(m_edtKindNameFirst,m_edtKindNameCtrlSecond);
    InsertTextToControl(m_edtAttributeSecond,m_edtAttributeCtrlSecond);

    //read the root item
    CString strFirstElement=m_treeSearchHistory.GetItemText(m_treeSearchHistory.GetRootItem());
    //insert an dummy item to the tree once the processing of search text is complete
    //it will be renamed to appropriate one
    HTREEITEM hItem=m_treeSearchHistory.InsertItem(_T("Dummy"),NULL,TVI_FIRST);

    CString strSearch;

    if(m_edtNameFirst!=_T("") || m_edtKindNameFirst!=_T("") || m_edtRoleNameFirst!=_T("") || m_edtAttributeFirst!=_T(""))
    {
        HTREEITEM hFirstCriteria=m_treeSearchHistory.InsertItem(_T("First Search Criteria"),hItem,TVI_FIRST);
        //First criteria
        PrepareHistoryString(_T("Name"),m_edtNameFirst,hFirstCriteria,strSearch);
        PrepareHistoryString(_T("Role"),m_edtRoleNameFirst,hFirstCriteria,strSearch);
        PrepareHistoryString(_T("Kind"),m_edtKindNameFirst,hFirstCriteria,strSearch);
        PrepareHistoryString(_T("Attribute"),m_edtAttributeFirst,hFirstCriteria,strSearch);
    }
    else
    {
        firstCriteriaEmpty=true;
    }


    //search criteria separator

    //strSearch.Append(_T(":"));
    if(m_edtNameSecond!=_T("") || m_edtKindNameSecond!=_T("") || m_edtRoleNameSecond!=_T("") || m_edtAttributeSecond!=_T(""))
    {
        HTREEITEM hSecondCriteria=m_treeSearchHistory.InsertItem(_T("Second Search Criteria"),hItem,TVI_LAST);

        //second criteria
        PrepareHistoryString(_T("Second Name"),m_edtNameSecond,hSecondCriteria,strSearch);
        PrepareHistoryString(_T("Second Role"),m_edtRoleNameSecond,hSecondCriteria,strSearch);
        PrepareHistoryString(_T("Second Kind"),m_edtKindNameSecond,hSecondCriteria,strSearch);
        PrepareHistoryString(_T("Second Attribute"),m_edtAttributeSecond,hSecondCriteria,strSearch);
    }
    else
    {
        secondCriteriaEmpty=true;
    }

    //others
    HTREEITEM hOtherCriteria=m_treeSearchHistory.InsertItem(_T("Others"),hItem,TVI_LAST);
    PrepareHistoryString(_T("Logical"),m_radioLogical,hOtherCriteria,strSearch);
    PrepareHistoryString(_T("Model"),m_chkMod,hOtherCriteria,strSearch);
    PrepareHistoryString(_T("Atom"),m_chkAtom,hOtherCriteria,strSearch);
    PrepareHistoryString(_T("Set"),m_chkSet,hOtherCriteria,strSearch);
    PrepareHistoryString(_T("Reference"),m_chkRef,hOtherCriteria,strSearch);
    PrepareHistoryString(_T("Connection"),m_chkConnection,hOtherCriteria,strSearch);
    PrepareHistoryString(_T("Special"),m_chkSplSearch,hOtherCriteria,strSearch);
    PrepareHistoryString(_T("Case"),m_chkMatchCase,hOtherCriteria,strSearch);
    PrepareHistoryString(_T("WholeWord"),m_chkFullWord,hOtherCriteria,strSearch);
    PrepareHistoryString(_T("Scope"),m_radioScope,hOtherCriteria,strSearch);
    PrepareHistoryString(_T("Results"),m_searchResults,hOtherCriteria,strSearch);

    m_treeSearchHistory.SetItemText(hItem,strSearch);

    //Get first element from the previous history or check if both the criteria
    //are empty, if they are same the history is not added
    if(strFirstElement==strSearch || (firstCriteriaEmpty && secondCriteriaEmpty))
    {
        m_treeSearchHistory.DeleteItem(hItem);
        return;
    }

    //for now it allows keeping unlimited history while the program is running
    //once the program is reloaded only 10 history items are allowed
}

//this is used for inserting history text to combo boxes
//it first checks if the first element in the combobox is same
//as the element to be inserted, if not it adds to the combo comtrol

void CSearchDlg::InsertTextToControl(CString& strNewTerm,CComboBox& control)
{
    CString strTerm;
    int n=control.GetLBTextLen(0);
    n=n==-1?1:n;
    control.GetLBText(0,strTerm.GetBuffer(n));
    if (strNewTerm!=_T("") && strNewTerm!=strTerm) control.InsertString(0,strNewTerm);
}

//prepares history string it is of the form
//<name>=<search term/text>
//combined with ',' for a seacrh criteria, search criterias are separated by ':'
//it also adds child to the tree control
void CSearchDlg::PrepareHistoryString(const CString &strCriteriaName,CString & strSearchValue,HTREEITEM hParent,CString &strSearch)
{
    CString strNameValue;
    if(strSearchValue.Trim()==_T(""))
        return;
    if(strSearch.GetLength()>0)
        strSearch.Append(_T(", "));

    strNameValue.Append(strCriteriaName);
    strNameValue.Append(_T("="));
    strNameValue.Append(_T("\""));
    strNameValue.Append(strSearchValue);
    strNameValue.Append(_T("\""));

    strSearch.Append(strNameValue);

    m_treeSearchHistory.InsertItem(strNameValue,hParent,NULL);
}

void CSearchDlg::PrepareHistoryString(const CString &strCriteriaName,int & strSearchValue,HTREEITEM hParent,CString &strSearch)
{
    TCHAR buffer[10];
    _itot(strSearchValue,buffer,10);
    PrepareHistoryString(strCriteriaName,CString(buffer),hParent,strSearch);

}


void CSearchDlg::InsertHistory(CString strHistory)
{
    //insert at the top of combobox

    // For each combobox control check if the current text is not empty
    //and is current text equals previous search text.
    //if above consitions are true don't insert into combo history
    //otherwise insert. This doesn't stop from combo box containing same texts and multiple places
    //becaseu it doesn't check whole list of texts
    //if more than 10 elments exists in the combo box delete it as well

    CString readValue;

    HTREEITEM hItem=m_treeSearchHistory.InsertItem(_T("Dummy"),NULL,TVI_LAST);
    HTREEITEM hFirstCriteria=m_treeSearchHistory.InsertItem(_T("First Search Criteria"),hItem,TVI_FIRST);
    HTREEITEM hSecondCriteria=m_treeSearchHistory.InsertItem(_T("Second Search Criteria"),hItem,TVI_LAST);
    HTREEITEM hOtherCriteria=m_treeSearchHistory.InsertItem(_T("Other"),hItem,TVI_LAST);

    CString strSearch;

    ReadHistoryValue(_T("Name"),strHistory,readValue);
    InsertTextToControl(readValue,m_edtNameCtrlFirst);
    PrepareHistoryString(CString(_T("Name")),readValue,hFirstCriteria,strSearch);

    ReadHistoryValue(_T("Role"),strHistory,readValue);
    InsertTextToControl(readValue,m_edtRoleNameCtrlFirst);
    PrepareHistoryString(CString(_T("Role")),readValue,hFirstCriteria,strSearch);

    ReadHistoryValue(_T("Kind"),strHistory,readValue);
    InsertTextToControl(readValue,m_edtKindNameCtrlFirst);
    PrepareHistoryString(CString(_T("Kind")),readValue,hFirstCriteria,strSearch);

    ReadHistoryValue(_T("Attribute"),strHistory,readValue);
    InsertTextToControl(readValue,m_edtAttributeCtrlFirst);
    PrepareHistoryString(CString(_T("Attribute")),readValue,hFirstCriteria,strSearch);

    //if no first search criteria items are there delete it
    int length=strSearch.GetLength();
    if(length==0)
        m_treeSearchHistory.DeleteItem(hFirstCriteria);

    //search criteria separator
    //strSearch.Append(_T(":"));

    ReadHistoryValue(_T("Second Name"),strHistory,readValue);
    InsertTextToControl(readValue,m_edtNameCtrlSecond);
    PrepareHistoryString(CString(_T("Second Name")),readValue,hSecondCriteria,strSearch);

    ReadHistoryValue(_T("Second Role"),strHistory,readValue);
    InsertTextToControl(readValue,m_edtRoleNameCtrlSecond);
    PrepareHistoryString(CString(_T("Second Role")),readValue,hSecondCriteria,strSearch);

    ReadHistoryValue(_T("Second Kind"),strHistory,readValue);
    InsertTextToControl(readValue,m_edtKindNameCtrlSecond);
    PrepareHistoryString(CString(_T("Second Kind")),readValue,hSecondCriteria,strSearch);

    ReadHistoryValue(_T("Second Attribute"),strHistory,readValue);
    InsertTextToControl(readValue,m_edtAttributeCtrlSecond);
    PrepareHistoryString(CString(_T("Second Attribute")),readValue,hSecondCriteria,strSearch);

    //check if second search criteria has been added
    length=strSearch.GetLength()-length;
    if(length==0)
         m_treeSearchHistory.DeleteItem(hSecondCriteria);


    ReadHistoryValue(_T("Logical"),strHistory,m_radioLogical);
    PrepareHistoryString(_T("Logical"),m_radioLogical,hOtherCriteria,strSearch);

    ReadHistoryValue(_T("Model"),strHistory,m_chkMod);
    PrepareHistoryString(_T("Model"),m_chkMod,hOtherCriteria,strSearch);

    ReadHistoryValue(_T("Atom"),strHistory,m_chkAtom);
    PrepareHistoryString(_T("Atom"),m_chkAtom,hOtherCriteria,strSearch);

    ReadHistoryValue(_T("Set"),strHistory,m_chkSet);
    PrepareHistoryString(_T("Set"),m_chkSet,hOtherCriteria,strSearch);

    ReadHistoryValue(_T("Reference"),strHistory,m_chkRef);
    PrepareHistoryString(_T("Reference"),m_chkRef,hOtherCriteria,strSearch);

    ReadHistoryValue(_T("Connection"),strHistory,m_chkConnection);
    PrepareHistoryString(_T("Connection"),m_chkConnection,hOtherCriteria,strSearch);

    ReadHistoryValue(_T("Special"),strHistory,m_chkSplSearch);
    PrepareHistoryString(_T("Special"),m_chkSplSearch,hOtherCriteria,strSearch);

    ReadHistoryValue(_T("Case"),strHistory,m_chkMatchCase);
    PrepareHistoryString(_T("Case"),m_chkMatchCase,hOtherCriteria,strSearch);

    ReadHistoryValue(_T("WholeWord"),strHistory,m_chkFullWord);
    PrepareHistoryString(_T("WholeWord"),m_chkFullWord,hOtherCriteria,strSearch);

    ReadHistoryValue(_T("Scope"),strHistory,m_radioScope);
    PrepareHistoryString(_T("Scope"),m_radioScope,hOtherCriteria,strSearch);

    ReadHistoryValue(_T("Results"),strHistory,m_searchResults);
    PrepareHistoryString(_T("Results"),m_searchResults,hOtherCriteria,strSearch);

    m_treeSearchHistory.SetItemText(hItem,strSearch);

    UpdateData(FALSE); 
}

//parse the search string to read a particular value
//first it looks for the criteria name
//when found takes the string after that as the value
//the quotes and commas are removed, 2 search criterias are differentiated by ':'
void CSearchDlg::ReadHistoryValue(const CString &strCriteriaName, CString &strHistory, CString &strValue)
{
    //clear the output string
    strValue=_T("");
    CString strTemp(strCriteriaName);
    strTemp.Append(_T("="));
    int indexText,indexComma;
    indexText=strHistory.Find(strTemp);
    if(indexText==-1) return;
    indexComma=strHistory.Find(_T(","),indexText);
    indexComma=indexComma==-1?strHistory.GetLength():indexComma;
    int start=indexText + strTemp.GetLength()+1;
    strValue=strHistory.Mid(start,indexComma-start-1);

}

void CSearchDlg::ReadHistoryValue(const CString &strCriteriaName,CString &strHistory, int &value)
{
    CString strValue;
    ReadHistoryValue(strCriteriaName,strHistory,strValue);
    value=_ttoi(strValue.GetBuffer());
}

void CSearchDlg::SearchResults()
{
	if (results == nullptr)
		return;
    CSearchCtrl *TheCtrl = GetCtrl();
    CComPtr<IMgaProject> ccpProject = TheCtrl->GetProject();
    m_pgsSearch.ShowWindow(SW_RESTORE);
    TheCtrl->BeginTransaction();

    CComPtr<IMgaObjects> ccpObjectsInTerr = TheCtrl->PutInMyTerritory( TheCtrl->GetScopedL());
    CComPtr<IMgaFCOs> new_results;
    COMTHROW(new_results.CoCreateInstance(L"Mga.MgaFCOs"));;
    CInput inp;
    inp.GetInput(m_edtNameFirst,m_edtRoleNameFirst,m_edtKindNameFirst,m_edtAttributeFirst,m_edtNameSecond,m_edtRoleNameSecond,m_edtKindNameSecond,m_edtAttributeSecond,m_edtAttrValue,
        m_chkMod,m_chkAtom,m_chkRef,m_chkSet,m_chkConnection,m_chkSplSearch,m_chkFullWord,NULL,0,m_chkMatchCase,m_radioScope,m_radioLogical
        );

    CSearch searchGME(inp);
    searchGME.SearchResults(results,new_results,&m_pgsSearch);
    results=new_results;
    m_pgsSearch.ShowWindow(SW_HIDE);
    m_pgsSearch.SetPos(1);
    DisplayResults();
    TheCtrl->CommitTransaction();
}

CSearchDlg::~CSearchDlg()
{
    specialSearchFCO = NULL;
}

void CSearchDlg::OnNMDblclkTreeSearchHistory(NMHDR *pNMHDR, LRESULT *pResult)
{
    CString strSearchText;
    HTREEITEM hItem=m_treeSearchHistory.GetSelectedItem();
    HTREEITEM hParent=m_treeSearchHistory.GetParentItem(hItem);
    HTREEITEM hGrandParent=m_treeSearchHistory.GetParentItem(hParent);

    if(hGrandParent)
        strSearchText=m_treeSearchHistory.GetItemText(hGrandParent);
    else if(hParent)
        strSearchText=m_treeSearchHistory.GetItemText(hParent);
    else
        strSearchText=m_treeSearchHistory.GetItemText(hItem);

    //first search criteria
    ReadHistoryValue(_T("Name"),strSearchText,m_edtNameFirst);
    ReadHistoryValue(_T("Role"),strSearchText,m_edtRoleNameFirst);
    ReadHistoryValue(_T("Kind"),strSearchText,m_edtKindNameFirst);
    ReadHistoryValue(_T("Attribute"),strSearchText,m_edtAttributeFirst);

    //second search criteria
    ReadHistoryValue(_T("Second Name"),strSearchText,m_edtNameSecond);
    ReadHistoryValue(_T("Second Role"),strSearchText,m_edtRoleNameSecond);
    ReadHistoryValue(_T("Second Kind"),strSearchText,m_edtKindNameSecond);
    ReadHistoryValue(_T("Second Attribute"),strSearchText,m_edtAttributeSecond);

    //others ReadHistoryValue(_T("Logical"),strHistory,m_radioLogical);
    ReadHistoryValue(_T("Model"),strSearchText,m_chkMod);
    ReadHistoryValue(_T("Atom"),strSearchText,m_chkAtom);
    ReadHistoryValue(_T("Set"),strSearchText,m_chkSet);
    ReadHistoryValue(_T("Reference"),strSearchText,m_chkRef);
    ReadHistoryValue(_T("Connection"),strSearchText,m_chkConnection);
    ReadHistoryValue(_T("Special"),strSearchText,m_chkSplSearch);
    ReadHistoryValue(_T("Case"),strSearchText,m_chkMatchCase);
    ReadHistoryValue(_T("WholeWord"),strSearchText,m_chkFullWord);
    ReadHistoryValue(_T("Scope"),strSearchText,m_radioScope);
    ReadHistoryValue(_T("Results"),strSearchText,m_searchResults);

    UpdateData(FALSE);  
     // if special search is on disable search for others than reference
    if(m_chkSplSearch)
    {
        OnCheckSplSearch();
    }
    else
    {
        m_chkModCtrl.EnableWindow(TRUE);
		m_chkAtomCtrl.EnableWindow(TRUE);
		m_chkSetCtrl.EnableWindow(TRUE);
		m_chkRefCtrl.EnableWindow(TRUE);
        m_chkConnCtrl.EnableWindow(TRUE);
    }
    *pResult = 0;
}

void CSearchDlg::OnCheckSplSearch()
{
    CWnd::UpdateData(TRUE);

	if(m_chkSplSearch)
	{
		m_chkMod = FALSE;
		m_chkAtom = FALSE;
		m_chkSet = FALSE;
        m_chkConnection = FALSE;
		m_chkRef = TRUE;

		m_chkModCtrl.EnableWindow(FALSE);
		m_chkAtomCtrl.EnableWindow(FALSE);
		m_chkSetCtrl.EnableWindow(FALSE);
		m_chkRefCtrl.EnableWindow(FALSE);
        m_chkConnCtrl.EnableWindow(FALSE);
		CWnd::UpdateData(FALSE);
	}
	else
	{
		m_chkMod = TRUE;
		m_chkAtom = TRUE;
		m_chkSet = TRUE;
		m_chkRef = TRUE;
        m_chkConnection=TRUE;

		m_chkModCtrl.EnableWindow(TRUE);
		m_chkAtomCtrl.EnableWindow(TRUE);
		m_chkSetCtrl.EnableWindow(TRUE);
		m_chkRefCtrl.EnableWindow(TRUE);
        m_chkConnCtrl.EnableWindow(TRUE);
		CWnd::UpdateData(FALSE);

		//reset special reference search to NULL
		specialSearchFCO = NULL;
		m_stcRefCtrl.SetWindowText(_T("NULL References"));
		m_stcRefCtrl.ShowWindow(TRUE);
		CWnd::UpdateData(TRUE);
	}
}

void CSearchDlg::OnLvnColumnclickListresults(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
    
    //create the sort param that needs to be passed to sort function
    SortParam param(&m_lstResults,pNMLV->iSubItem,ascending[pNMLV->iSubItem]);
   
    //sort the items
    ListView_SortItemsEx(m_lstResults.GetSafeHwnd(), ListItemCompareProc, (LPARAM) &param);

   ascending[pNMLV->iSubItem] = !ascending[pNMLV->iSubItem];
    *pResult = 0;
}

void CSearchDlg::OnBnClickedButtonClear()
{
    m_treeSearchHistory.DeleteAllItems();
    SaveSearchHistory();
}

DROPEFFECT CSearchDropTarget::OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	CSearchDlg *ctrl = STATIC_DOWNCAST(CSearchDlg, pWnd);
	CSearchCtrl *TheCtrl = ctrl->GetCtrl();
	if (CGMEDataSource::IsGmeNativeDataAvailable(pDataObject, TheCtrl->GetProject()))
	{
		return DROPEFFECT_LINK;
	}
	return DROPEFFECT_NONE;
}

void CSearchDropTarget::OnDragLeave(CWnd* pWnd)
{
}

DROPEFFECT CSearchDropTarget::OnDragOver(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	CSearchDlg *ctrl = STATIC_DOWNCAST(CSearchDlg, pWnd);
	CSearchCtrl *TheCtrl = ctrl->GetCtrl();
	if (CGMEDataSource::IsGmeNativeDataAvailable(pDataObject, TheCtrl->GetProject()))
	{
		return DROPEFFECT_LINK;
	}
	return DROPEFFECT_NONE;
}

BOOL CSearchDropTarget::OnDrop(CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point)
{
	COMTRY {
	CSearchDlg *ctrl = STATIC_DOWNCAST(CSearchDlg, pWnd);
	CSearchCtrl *TheCtrl = ctrl->GetCtrl();
	if (CGMEDataSource::IsGmeNativeDataAvailable(pDataObject, TheCtrl->GetProject()))
	{
		
		CComPtr<IDataObject> p = pDataObject->GetIDataObject(FALSE);
		CComPtr<IMgaDataSource> pt;
		COMTHROW(p.QueryInterface(&pt));

		CComPtr<IUnknown> unk;
		COMTHROW(pt->get_Data(&unk));

		CComPtr<IMgaFCOs> fcos;
		// CComPtr<IMgaMetaRole> metaRole; TODO: put Kind or Role based on partbrowser drag-n-drop
		if (SUCCEEDED(unk.QueryInterface(&fcos)))
		{
			CComPtr<IMgaFCO> fco;
			long count = 0;
			if (SUCCEEDED(fcos->get_Count(&count)) && count > 0)
			{
				COMTHROW(fcos->get_Item(1, &fco));
			}
			if (fco == nullptr)
			{
				return FALSE;
			}

			CWnd *dropTarget = ctrl->ChildWindowFromPoint(point);
			CWnd *editKind = STATIC_DOWNCAST(CButton, ctrl->GetDlgItem(IDC_EDITKIND));
			CWnd *editRole = STATIC_DOWNCAST(CButton, ctrl->GetDlgItem(IDC_EDITROLE));
			CWnd *editName = STATIC_DOWNCAST(CButton, ctrl->GetDlgItem(IDC_EDITNAME));
			if (dropTarget->GetSafeHwnd() == editKind->GetSafeHwnd())
			{
				TheCtrl->BeginTransaction();
				try
				{
					CComPtr<IMgaMetaFCO> meta;
					COMTHROW(fco->get_Meta(&meta));
					_bstr_t kind;
					COMTHROW(meta->get_Name(kind.GetAddress()));
					editKind->SetWindowTextW(static_cast<const wchar_t*>(kind));
					TheCtrl->CommitTransaction();
				}
				catch (hresult_exception)
				{
					TheCtrl->AbortTransaction();
					throw;
				}
			}
			else if (dropTarget->GetSafeHwnd() == editRole->GetSafeHwnd())
			{
				try
				{
					TheCtrl->BeginTransaction();
					CComPtr<IMgaMetaRole> meta;
					COMTHROW(fco->get_MetaRole(&meta));
					_bstr_t rolename;
					COMTHROW(meta->get_Name(rolename.GetAddress()));
					editRole->SetWindowTextW(static_cast<const wchar_t*>(rolename));
					TheCtrl->CommitTransaction();
				}
				catch (hresult_exception)
				{
					TheCtrl->AbortTransaction();
					throw;
				}
			}
			else if (dropTarget->GetSafeHwnd() == editName->GetSafeHwnd())
			{
				TheCtrl->BeginTransaction();
				try
				{
					_bstr_t name;
					COMTHROW(fco->get_Name(name.GetAddress()));
					editName->SetWindowTextW(static_cast<const wchar_t*>(name));
					TheCtrl->CommitTransaction();
				}
				catch (hresult_exception)
				{
					TheCtrl->AbortTransaction();
					throw;
				}
			}
			else
			{
				ctrl->results = NULL;
				COMTHROW(ctrl->results.CoCreateInstance(L"Mga.MgaFCOs"));
				COMTHROW(ctrl->results->Append(fco));
				TheCtrl->BeginTransaction();
				try
				{
					ctrl->DisplayResults();
					TheCtrl->CommitTransaction();
					ctrl->m_lstResults.SetFocus();
					int ind = ctrl->m_lstResults.GetTopIndex();
					VERIFY(ctrl->m_lstResults.SetItemState(ind, LVIS_SELECTED, LVIS_SELECTED));
					ctrl->m_lstResults.SetSelectionMark(ind);
					ctrl->itemClicked();
					CButton *special_check = STATIC_DOWNCAST(CButton, ctrl->GetDlgItem(IDC_CHECKSPLSEARCH));
					special_check->SetCheck(TRUE);
					ctrl->OnCheckSplSearch();
				}
				catch (hresult_exception)
				{
					TheCtrl->AbortTransaction();
					throw;
				}
			}

			return TRUE;
		}
	}
	return FALSE;
	} COMCATCH(return FALSE;)
}
