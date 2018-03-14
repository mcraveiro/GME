// InPlaceManager.cpp: implementation of the CInPlaceManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "InspectorList.h"
#include "objectinspector.h"
#include "InPlaceManager.h"
#include "CompassCheckDlg.h"
#include "CompassOptDlg.h"
#include "ComboBoxSelectDlg.h"
#include "ColourPopup.h"
#include "MgaUtil.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CInPlaceManager::CInPlaceManager(CInspectorList* pInspectorList)
{
	m_pInspectorList=pInspectorList;
}

CInPlaceManager::~CInPlaceManager()
{

}

void CInPlaceManager::ShowInPlace(CRect rectInPlace, int nIndex)
{

	HideAllInPlace();
	int nSelCount=m_pInspectorList->GetSelCount();
	if(nSelCount!=1)
	{
		return;
	}

	m_nCurrentIndex=nIndex;

	CListItem& ListItem=m_pInspectorList->m_ListItemArray.ElementAt(nIndex);

	ListItem.Value.toString();
	switch(ListItem.Value.dataType)
	{
	case ITEMDATA_NULL:
		{

		}break;

	case ITEMDATA_STRING:
		{
			if(ListItem.Value.cLineNum>1)
			{
				if(!ListItem.bIsReadOnly) {
					DisplayEditorButton(rectInPlace);
				}

				DisplayMultilineEdit(rectInPlace, ListItem.bIsReadOnly);
				CString strText; // zolmol modification
				INT_PTR uLim = ListItem.Value.stringVal.GetUpperBound(); // if empty uLim == -1
				for(INT_PTR i = 0; i <= uLim; i++)
				{
					strText+=ListItem.Value.stringVal[i];
					if( i != uLim)
						strText+=_T("\r\n");
				}
				// there is no newline at the end of the last line
				m_MultiEditCtrl.SetWindowText(strText);//WAS: strText.Left(strText.GetLength()-2)
				m_MultiEditCtrl.SetFocus();
				
			}
			else
			{
				DisplaySingleLineEdit(rectInPlace, ListItem.bIsReadOnly);

				m_SingleEditCtrl.SetWindowText(ListItem.Value.stringVal[0]);

				int nLength=ListItem.Value.stringVal[0].GetLength();
				m_SingleEditCtrl.SetSel(nLength,nLength);
			}

		}break;


	case ITEMDATA_FIXED_LIST:
		{
			if(!ListItem.bIsReadOnly) {
				DisplayArrowButton(rectInPlace);
			}

		}break;


	case ITEMDATA_INTEGER:
		{
			DisplaySingleLineEdit(rectInPlace, ListItem.bIsReadOnly);

			m_SingleEditCtrl.SetWindowText(ListItem.Value.stringVal[0]);
			int nLength=ListItem.Value.stringVal[0].GetLength();
			m_SingleEditCtrl.SetSel(nLength,nLength);


		}break;
	case ITEMDATA_DOUBLE:
		{
			DisplaySingleLineEdit(rectInPlace, ListItem.bIsReadOnly);

			m_SingleEditCtrl.SetWindowText(ListItem.Value.stringVal[0]);
			int nLength=ListItem.Value.stringVal[0].GetLength();
			m_SingleEditCtrl.SetSel(nLength,nLength);


		}break;
	case ITEMDATA_BOOLEAN:
		{
			if(!ListItem.bIsReadOnly) {
				DisplayArrowButton(rectInPlace);
			}
		}break;

	case ITEMDATA_COLOR	:
		{
			if(!ListItem.bIsReadOnly) {
				DisplayArrowButton(rectInPlace);
			}
		}break;

	case ITEMDATA_COMPASS:
		{
			if(!ListItem.bIsReadOnly) {
				DisplayArrowButton(rectInPlace);
			}
		}break;

	case ITEMDATA_COMPASS_EXCL:
		{
			if(!ListItem.bIsReadOnly) {
				DisplayArrowButton(rectInPlace);
			}
		}break;
	}
}

void CInPlaceManager::DisplayMultilineEdit(CRect rectBound, bool readOnly)
{

	rectBound.bottom--;
	rectBound.bottom--;

	if(!::IsWindow(m_MultiEditCtrl.GetSafeHwnd()))
	{
		m_MultiEditCtrl.Create(ES_WANTRETURN|ES_AUTOVSCROLL |WS_VSCROLL|ES_MULTILINE | ES_AUTOHSCROLL | WS_CHILD | WS_VISIBLE |ES_LEFT,rectBound,m_pInspectorList,IDC_EDITBOX_MULTILINE);


		m_MultiEditCtrl.SetFont(&m_pInspectorList->m_entryFont);
		m_MultiEditCtrl.SetFocus();
	}
	else
	{
		if(!m_MultiEditCtrl.IsWindowVisible())
		{
			m_MultiEditCtrl.MoveWindow(rectBound.left,rectBound.top,rectBound.Width(),rectBound.Height());
			m_MultiEditCtrl.ShowWindow(SW_SHOW);
			// m_MultiEditCtrl.SetFocus();
		}
		else
		{
			m_MultiEditCtrl.SetFocus();
			m_MultiEditCtrl.Invalidate();
		}
	}
	m_MultiEditCtrl.SetReadOnly(readOnly ? TRUE : FALSE);
}


void CInPlaceManager::DisplaySingleLineEdit(CRect rectBound, bool readOnly)
{

	rectBound.bottom--;
	rectBound.bottom--;

	if(!::IsWindow(m_SingleEditCtrl.GetSafeHwnd()))
	{
		m_SingleEditCtrl.Create(WS_CHILD| WS_VISIBLE|ES_AUTOHSCROLL|ES_LEFT,rectBound,m_pInspectorList,IDC_EDITBOX_SINGLELINE);
		
		
		m_SingleEditCtrl.SetFont(&m_pInspectorList->m_entryFont);
		m_SingleEditCtrl.SetFocus();

	}
	else
	{
		if(!m_SingleEditCtrl.IsWindowVisible())
		{
			m_SingleEditCtrl.SetWindowText(_T(""));
			m_SingleEditCtrl.MoveWindow(rectBound.left,rectBound.top,rectBound.Width(),rectBound.Height());
			m_SingleEditCtrl.ShowWindow(SW_SHOW);
			m_SingleEditCtrl.SetFocus();
		}
		else
		{
			m_SingleEditCtrl.SetFocus();
			m_SingleEditCtrl.Invalidate();
		}
	}
	m_SingleEditCtrl.SetReadOnly(readOnly ? TRUE : FALSE);
}

void CInPlaceManager::DisplayColorCombo(CRect rectBound, bool rightSideClick)
{
	m_pInspectorList->ClientToScreen(rectBound);

	CListItem &ListItem= m_pInspectorList->m_ListItemArray.ElementAt(m_nCurrentIndex);

	CColourPopup dlg(m_pInspectorList);
	dlg.SetParameters(rectBound,				// rect to display popup
					  ListItem.Value.colorVal,	// Selected colour
					  rightSideClick,			// summoned by arrow button click or right side click
					  _T("Default"),			// "Default" text area
					  _T("More Colors.."));		// Custom Text

	if(dlg.DoModal()==IDOK)
	{
		if (dlg.m_nCurrentSel == DEFAULT_BOX_VALUE)
		{
			ListItem.RestoreDefault();
		}
		else
		{
			ListItem.Value.SetColorValue(dlg.GetSelectedColor());
			ListItem.SetDirty();
		}

		m_pInspectorList->NotifyParent(m_nCurrentIndex);

		m_pInspectorList->Invalidate();
	}

	m_pInspectorList->SetFocus();
}





void CInPlaceManager::HideAllInPlace()
{
	HideEdit();
	HideArrowButton();
	HideEditorButton();
}


void CInPlaceManager::HideEdit()
{
	if(::IsWindow(m_SingleEditCtrl.GetSafeHwnd()))
	{
		m_SingleEditCtrl.ShowWindow(SW_HIDE);
	}

	if(::IsWindow(m_MultiEditCtrl.GetSafeHwnd()))
	{
		m_MultiEditCtrl.ShowWindow(SW_HIDE);
	}
}


void CInPlaceManager::DisplayArrowButton(CRect rectBound)
{
	rectBound.top--;
	rectBound.bottom--;
	rectBound.top--;
	rectBound.bottom--;

	rectBound.left=rectBound.right-rectBound.Height();
	if(!::IsWindow(m_ArrowButton.GetSafeHwnd()) )
	{
		m_ArrowButton.Create(_T("ArrowButton"),WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,rectBound,m_pInspectorList,IDC_ARROW_BUTTON);
	}
	else
	{
		m_ArrowButton.MoveWindow(rectBound.left,rectBound.top,rectBound.Width(),rectBound.Height());
		m_ArrowButton.ShowWindow(SW_SHOW);
	}

	m_ArrowButton.Invalidate();
}

void CInPlaceManager::DisplayEditorButton(CRect rectBound)
{
	// Get External Editor prefs and display editor button if needed
	CComPtr<IMgaRegistrar> registrar;
	try {
		COMTHROW( registrar.CoCreateInstance(OLESTR("MGA.MgaRegistrar")) );
		ASSERT( registrar != NULL );

		VARIANT_BOOL extenable;
		COMTHROW( registrar->get_ExternalEditorEnabled(REGACCESS_USER, &extenable) );
		if (extenable == VARIANT_FALSE) {
			return;
		}
	} 
	catch (hresult_exception &) {
		return;
	}


	CBitmap bm;
	bm.LoadMappedBitmap(IDB_BITMAP_EDITOR);

	CSize bmSize;
	BITMAP bmStruct;
	if (bm.GetBitmap(&bmStruct)) {
		bmSize.cx = bmStruct.bmWidth;
		bmSize.cy = bmStruct.bmHeight;
	}

	CRect rect;
	rect.top = rectBound.top;
	rect.bottom = rect.top + bmSize.cy;
	rect.right = rectBound.left - 2;		// Shift it
	rect.left = rect.right - bmSize.cx;

	if(!::IsWindow(m_EditorButton.GetSafeHwnd()) )
	{
		m_EditorButton.Create(_T("EditorButton"),WS_CHILD|WS_VISIBLE|BS_BITMAP,rect,m_pInspectorList,IDC_EDITOR_BUTTON);
		m_EditorButton.SetBitmap(bm);
		bm.Detach();

	}
	else
	{
		m_EditorButton.MoveWindow(rect.left,rect.top,rect.Width(),rect.Height());
		m_EditorButton.ShowWindow(SW_SHOW);
	}

	m_EditorButton.Invalidate();
}


void CInPlaceManager::HideArrowButton()
{
	if(::IsWindowVisible(m_ArrowButton.GetSafeHwnd()) )
	{
		m_ArrowButton.ShowWindow(SW_HIDE);
	}
}

void CInPlaceManager::HideEditorButton()
{
	if(::IsWindowVisible(m_EditorButton.GetSafeHwnd()) )
	{
		m_EditorButton.ShowWindow(SW_HIDE);
	}
}

void CInPlaceManager::OnClickEditorButton()
{
	CListItem& ListItem=m_pInspectorList->m_ListItemArray.ElementAt(m_nCurrentIndex);
	ASSERT(ListItem.Value.dataType == ITEMDATA_STRING);
	ASSERT(ListItem.Value.cLineNum > 1);

	// Create temporary file
	TCHAR szTempPath[MAX_PATH];
	if (::GetTempPath(MAX_PATH, szTempPath) == 0) {
		return;
	}

	TCHAR szTempFileName[MAX_PATH];
	if (::GetTempFileName(szTempPath, _T("GME"), 0, szTempFileName) == 0) {
		ASSERT(("Unable to get temporary filename.", false));
		return;
	}
	
	CString szAppPath;
	bool content_specific_editor = false;

	if( !ListItem.strContentType.IsEmpty())
	{
		CString extension;
		bool content_type_valid = true;
		if( ListItem.strContentType.GetAt(0) == _T('.')) // starts with '.' -> interpret it as extension
			extension = ListItem.strContentType;
		else // interpret it as MIME type (Content-Type), and lookup the extension corresponding to it
			content_type_valid = CInPlaceManager::findInfoInMimeDB( ListItem.strContentType, szAppPath, extension);
	
		if( content_type_valid) // remove the file created above by GetTempFileName
			CFile::Remove( szTempFileName);

		content_specific_editor = true;
		// if enough space, append the extension to the filename
		if( content_type_valid && _tcslen( szTempFileName) + extension.GetLength() <= MAX_PATH - 1) // there is enough room for appending the extension
			_tcscat( szTempFileName, extension);
		else if( content_type_valid && extension.GetLength() == 4) // replace tailing .tmp with extension
			_tcsncpy( szTempFileName + _tcslen( szTempFileName) - 4, extension, 4);
		else // can't use the content_specific_editor because the extension is not appended
			content_specific_editor = false;

		// we could use findCommand (implemented at bottom of file) to locate the command
		// assigned for open/edit verbs, but we choose to rely on the ShellExecute call
		//if( content_type_valid)
		//{
		//	content_specific_editor = findCommand( extension, szAppPath);
		//	if( szAppPath.Find( "%1") != -1) // if "%1" found in command string replace it with tempfilename
		//		szAppPath.Replace( "%1", szTempFileName); 
		//}
	}

	CStdioFile tempFile;
	if (tempFile.Open(szTempFileName, CFile::modeCreate | CFile::modeWrite | CFile::typeText) == 0) {
		ASSERT(("Unable to create temporary file.", false));
		return;
	}

	// write attribute value to the temporary file
	for(int i=0;i<=ListItem.Value.stringVal.GetUpperBound();i++)
	{
		CString strLine = ListItem.Value.stringVal[i];
		strLine.TrimRight(_T("\r\n"));
		tempFile.WriteString(strLine + _T("\n"));
	}

	tempFile.Close();

	BOOL launched = FALSE;
	if( content_specific_editor)
	{
		if( !szAppPath.IsEmpty()) // GMEEditor value found in MimeDB
		{
			CString szCommandLine = _T(" ");
			szCommandLine += szTempFileName;
			int nCommandLineLength = szCommandLine.GetLength();

			// startup info for the redactor's process is taken similar to that
			// of invoking application
			STARTUPINFO startUpInfo;
			PROCESS_INFORMATION processInfo;
			::GetStartupInfo(&startUpInfo);

			// start pref'd editor with the file name as the command line parameter
			launched = ::CreateProcess(szAppPath,szCommandLine.GetBuffer(nCommandLineLength),
				NULL,NULL,FALSE,0,NULL,NULL,&startUpInfo,&processInfo);
			szCommandLine.ReleaseBuffer();
			CloseHandle(processInfo.hProcess);
			CloseHandle(processInfo.hThread);
		}

		if( !launched)
		{
			// success codes are strictly greater than 32
			int retcode = (int) ShellExecute( (HWND) m_EditorButton, _T("edit"), szTempFileName, 0, 0, SW_SHOWNORMAL);
			if( retcode == SE_ERR_NOASSOC) // failed because no such verb (edit) exists for this extension
				retcode = (int) ShellExecute( (HWND) m_EditorButton, _T("open"), szTempFileName, 0, 0, SW_SHOWNORMAL);

			launched = retcode > 32;
			// if it was not launched successfully, use the specified editor
		}
	}

	if( !launched) // if not found a content specific one, or if it failed to launch
	{
		// Get External Editor prefs and build command line for the editor application
		CComPtr<IMgaRegistrar> registrar;
		try {
			COMTHROW( registrar.CoCreateInstance(OLESTR("MGA.MgaRegistrar")) );
			ASSERT( registrar != NULL );
			COMTHROW( registrar->get_ExternalEditor(REGACCESS_USER, PutOut(szAppPath)) );
		} 
		catch (hresult_exception &) {
		}

		CString szCommandLine = szAppPath + _T(" ");
		szCommandLine += szTempFileName;
		int nCommandLineLength = szCommandLine.GetLength();

		// startup info for the redactor's process is taken similar to that
		// of invoking application
		STARTUPINFO startUpInfo;
		PROCESS_INFORMATION processInfo;
		::GetStartupInfo(&startUpInfo);

		// start notepad.exe with the XML file name as the command line parameter

		launched = ::CreateProcess(NULL, szCommandLine.GetBuffer(nCommandLineLength),
			NULL,NULL,FALSE,0,NULL,NULL,&startUpInfo,&processInfo);
		szCommandLine.ReleaseBuffer();
		CloseHandle(processInfo.hProcess);
		CloseHandle(processInfo.hThread);
	}

	// DWORD h = ::WaitForSingleObject(processInfo.hProcess, INFINITE);
	if (launched)
	{
		m_EditorButton.MessageBox(_T("Click OK to finish."), _T("Using External Text Editor"), MB_ICONINFORMATION);
	}

	// open temporary file
	if (tempFile.Open(szTempFileName, CFile::modeRead | CFile::typeText) == 0) {
		ASSERT(("Unable to open temporary file.", false));
		return;
	}

	// read attribute value from the temporary file
	ListItem.Value.stringVal.RemoveAll();
	CString strLine;
	while (tempFile.ReadString(strLine) == TRUE) {
		strLine.TrimRight(_T("\r\n"));
		ListItem.Value.stringVal.Add(strLine);
	}

	tempFile.Close();

	ListItem.SetDirty();

	m_pInspectorList->NotifyParent(m_nCurrentIndex);

	m_pInspectorList->SetFocus();


	
}

void CInPlaceManager::OnClickArrowButton(bool rightSideClick)
{

	CRect rectArrow;

	m_ArrowButton.GetWindowRect(rectArrow);
	m_pInspectorList->ScreenToClient(rectArrow);
	CRect rectWnd(m_pInspectorList->m_Settings.m_nDivider,rectArrow.bottom-1,rectArrow.right,rectArrow.bottom-1);

	CListItem& ListItem=m_pInspectorList->m_ListItemArray.ElementAt(m_nCurrentIndex);


	switch(ListItem.Value.dataType)
	{
	case ITEMDATA_COMPASS:
		{
			rectWnd.bottom+=100;
			DisplayCompassCheck(rectWnd);
		}break;

	case ITEMDATA_COMPASS_EXCL:
		{
			rectWnd.bottom+=100;
			DisplayCompassOpt(rectWnd);
		}break;

	case ITEMDATA_BOOLEAN:
		{
			rectWnd.bottom += 2 * m_pInspectorList->m_ComboboxLineHeight + 2;
			DisplayCombo(rectWnd);
		}break;
	case ITEMDATA_FIXED_LIST:
		{
			rectWnd.bottom += min((long)ListItem.Value.stringVal.GetSize(), 8) 
				* m_pInspectorList->m_ComboboxLineHeight + 2;
			DisplayCombo(rectWnd);
		}break;

	case ITEMDATA_COLOR:
		{
			DisplayColorCombo(rectWnd, rightSideClick);
		}break;

	default:
		{
			// This should not happen here
			ASSERT(0);
		}
	}

	if(::IsWindowVisible(m_ArrowButton.GetSafeHwnd()) )
		m_ArrowButton.Invalidate();
}


void CInPlaceManager::DisplayCompassCheck(CRect rectBound)
{
	CCompassCheckDlg dlg(m_pInspectorList);

	CListItem& ListItem=m_pInspectorList->m_ListItemArray.ElementAt(m_nCurrentIndex);
	UINT uCompassVal=ListItem.Value.compassVal;
	CPoint ptDlgTopRight(rectBound.right,rectBound.top);
	m_pInspectorList->ClientToScreen(&ptDlgTopRight);
	dlg.SetParameters(ptDlgTopRight, uCompassVal);

	if(dlg.DoModal()==IDOK)
	{
		ListItem.Value.SetCompassValue(dlg.GetCompassVal());

		ListItem.SetDirty();

		m_pInspectorList->NotifyParent(m_nCurrentIndex);
	}

	m_pInspectorList->SetFocus();
}

void CInPlaceManager::DisplayCompassOpt(CRect rectBound)
{
	CCompassOptDlg dlg(m_pInspectorList);

	CListItem& ListItem=m_pInspectorList->m_ListItemArray.ElementAt(m_nCurrentIndex);
	UINT uCompassVal=ListItem.Value.compassVal;
	CPoint ptDlgTopRight(rectBound.right,rectBound.top);
	m_pInspectorList->ClientToScreen(&ptDlgTopRight);
	dlg.SetParameters(ptDlgTopRight, uCompassVal);

	if(dlg.DoModal()==IDOK)
	{
		ListItem.Value.SetCompassExclValue(dlg.GetCompassVal());
		ListItem.SetDirty();

		m_pInspectorList->NotifyParent(m_nCurrentIndex);
	}

	m_pInspectorList->SetFocus();
}


void CInPlaceManager::DisplayCombo(CRect rectBound)
{
	CComboBoxSelectDlg dlg(m_pInspectorList, m_pInspectorList->m_ComboboxLineHeight);

	CListItem& ListItem = m_pInspectorList->m_ListItemArray.ElementAt( m_nCurrentIndex );
	m_pInspectorList->ClientToScreen( &rectBound );
	dlg.SetParameters(rectBound, &ListItem, &m_pInspectorList->m_entryFont);

	if ( dlg.DoModal() == IDOK ) {
		ListItem.SetDirty();
		m_pInspectorList->NotifyParent( m_nCurrentIndex );
	}

	m_pInspectorList->SetFocus();
}

void CInPlaceManager::OnEditMultiLineEnd()
{
	CListItem &ListItem= m_pInspectorList->m_ListItemArray.ElementAt(m_nCurrentIndex);

	ASSERT(ListItem.Value.dataType==ITEMDATA_STRING);

	ASSERT(ListItem.Value.cLineNum>1);

	int nLines=m_MultiEditCtrl.GetLineCount();
	ASSERT( nLines); // nLines >= 1 always


	CString strLine;
	ListItem.Value.stringVal.RemoveAll();
	int nLineLength=m_MultiEditCtrl.GetWindowTextLength();
	// To Zoltan's comment:
	// http://groups.google.com/group/microsoft.public.vc.mfc/browse_thread/thread/f75d48b8655dde0e
	// There are two errors with CEdit GetLine
	// 1.: GetLine cannot return less than two characters, so if there's just one character, we should still supply
	//     a buffer for two characters (in ANSI: two bytes)
	// 2.: If the edit control is empty, the GetLine call returns two null terminators
	//     (this caused the phenomena Zoltan encountered)
	// ****
	// zolmol: avoid trimming in case when the field is cleared: nLineLength = 0 and nLines = 1 
	//         (corrupted strLine inserted into ListItem caused memory damage when released)
	if( nLineLength > 0 )
	{
		for(int i=0;i<nLines;i++)
		{
			strLine = CEditGetLine(m_MultiEditCtrl, i);
			strLine.TrimRight(_T("\r\n"));
			ListItem.Value.stringVal.Add(strLine);
		}
	}

	ListItem.SetDirty();

	m_pInspectorList->NotifyParent(m_nCurrentIndex);

	m_pInspectorList->Invalidate();
}

void CInPlaceManager::OnEditSingleLineEnd()
{
	CListItem &ListItem= m_pInspectorList->m_ListItemArray.ElementAt(m_nCurrentIndex);


	CString strText;
	m_SingleEditCtrl.GetWindowText(strText);

	switch(ListItem.Value.dataType)
	{
		case ITEMDATA_STRING:
			{
				ListItem.Value.SetStringValue(strText);

				if(!ListItem.Value.Validate()) {
					m_pInspectorList->MessageBox(_T("Invalid (non ASCII) string data: ")+strText,_T("Object Inspector"),MB_ICONERROR);
					m_pInspectorList->SetFocus();
				} 
				else {
					ListItem.SetDirty();
					m_pInspectorList->NotifyParent(m_nCurrentIndex);

					m_pInspectorList->Invalidate();
				}
			}break;

		case ITEMDATA_INTEGER:
		{
			ListItem.Value.stringVal[0]=strText;

			if(!ListItem.Value.Validate())
			{
				m_pInspectorList->MessageBox(_T("Invalid integer data: ")+strText,_T("Object Inspector"),MB_ICONERROR);
				// Restoring correct value
				ListItem.Value.toString();
				m_pInspectorList->SetFocus();
			}
			else
			{
				ListItem.SetDirty();

				m_pInspectorList->NotifyParent(m_nCurrentIndex);
			}

		}break;

		case ITEMDATA_DOUBLE:
		{
			ListItem.Value.stringVal[0]=strText;

			if(!ListItem.Value.Validate())
			{
				m_pInspectorList->MessageBox(_T("Invalid floating point data: ")+strText,_T("Object Inspector"),MB_ICONERROR);
				// Restoring correct value
				ListItem.Value.toString();
				m_pInspectorList->SetFocus();
			}
			else
			{
				ListItem.SetDirty();

				m_pInspectorList->NotifyParent(m_nCurrentIndex);
			}

		}break;

		default:
		{
			ASSERT(0); // Should not happen
		}
	}


}


bool CInPlaceManager::OnRightItemClick(int nIndex, CRect rectInPlace)
{
	// User request: color and direction selection popups and other popup should pop up not only by the
	//				 arrow click, but the right side line click too
	if(::IsWindowVisible(m_ArrowButton.GetSafeHwnd()) )
	{
		if (nIndex == m_nCurrentIndex) {
			OnClickArrowButton(true);
			return true;
		}
	}

	return false;
}


void CInPlaceManager::OnEditEnd()
{
	CListItem &ListItem= m_pInspectorList->m_ListItemArray.ElementAt(m_nCurrentIndex);
	if(ListItem.Value.dataType==ITEMDATA_STRING&& ListItem.Value.cLineNum>1)
	{
		OnEditMultiLineEnd();
	}
	else
	{
		OnEditSingleLineEnd();
	}
}

/*static*/ bool CInPlaceManager::findInfoInMimeDB( const CString& pContentType, CString& pPrefApp, CString& pExtension)
{
	// mime types (content types) are enumerated under:
	const TCHAR MIME_DB[] = _T("MIME\\Database\\Content Type\\");
	const TCHAR EXTENSION_VALUE[] = _T("Extension");
	const TCHAR GMEEDITOR_VALUE[] = _T("GMEEditor"); // a developer might introduce here a GME specific value (by an installer)

	CRegKey rk;
	LONG res = rk.Open( HKEY_CLASSES_ROOT, MIME_DB + pContentType, KEY_READ);

	if( res != ERROR_SUCCESS) // such a mime type not found
		return false;

	// an Extension value exists hopefully
	ULONG buff_len = 32; // we don't expect wider extensions than 32
	CString ext(_T(' '), buff_len);
	TCHAR* buff = ext.GetBufferSetLength( buff_len);
	res = rk.QueryStringValue( EXTENSION_VALUE, buff, &buff_len);
	ext.ReleaseBufferSetLength( buff_len);

	if( res != ERROR_SUCCESS)
		ext.Empty(); // will not return succesfully

	buff_len = MAX_PATH;
	CString pref_app(_T(' '), buff_len);
	buff = pref_app.GetBufferSetLength( buff_len);
	res = rk.QueryStringValue( GMEEDITOR_VALUE, buff, &buff_len);
	pref_app.ReleaseBufferSetLength( buff_len);
	rk.Close();

	if( res != ERROR_SUCCESS)
		pref_app.Empty(); // this is not an error

	pPrefApp = pref_app;
	pExtension = ext;
	return !pExtension.IsEmpty();
}

// finds the Open/Edit verb defined for the file class (extension)
// format of returned string pCommand
// "C:\Program Files\Windows Media Player\wmplayer.exe" "%L"
// "C:\Program Files\Microsoft Visual Studio .NET 2003\Common7\IDE\devenv.exe" "%1"
// "C:\Program Files\Opera8\Opera.exe" "%1"
// "C:\Program Files\apps\Microsoft Office\Office10\msohtmed.exe" %1
//
///*static*/ bool CInPlaceManager::findCommand( const CString& pExtension, CString& pCommand)
//{
//	if( pExtension.IsEmpty() || pExtension.GetAt(0) != '.') return false;
//
//	CRegKey rk;
//	LONG res;
//	res = rk.Open( HKEY_CLASSES_ROOT, pExtension , KEY_READ);
//	if( res != ERROR_SUCCESS) // such an extension not found
//		return false;
//
//	// for each extension a file class is created typically: .xls -> Excel.Sheet.8
//	ULONG buff_len = 256; // it is enough long for a file class name
//	CString file_class( ' ', buff_len); 
//	TCHAR * buff = file_class.GetBufferSetLength( buff_len);
//	res = rk.QueryStringValue( 0, buff, &buff_len);
//	file_class.ReleaseBufferSetLength( buff_len);
//
//	rk.Close();
//	if( res != ERROR_SUCCESS)
//		return false;
//
//	file_class = buff;
//
//	res = rk.Open(HKEY_CLASSES_ROOT, file_class , KEY_READ);
//	if( res != ERROR_SUCCESS) return false;
//
//	CRegKey sk;
//	const TCHAR SHELL_EDIT_COMMAND[] = _T("SHELL\\EDIT\\COMMAND");
//	const TCHAR SHELL_OPEN_COMMAND[] = _T("SHELL\\OPEN\\COMMAND");
//
//	res = sk.Open( rk, SHELL_EDIT_COMMAND, KEY_READ);
//	if( res != ERROR_SUCCESS)
//	{
//		res = sk.Open( rk, SHELL_OPEN_COMMAND, KEY_READ); // this is the most common
//		if( res != ERROR_SUCCESS)
//			return false;
//	}
//
//	
//	ULONG cmd_len = MAX_PATH;
//	TCHAR * cmd = pCommand.GetBufferSetLength( cmd_len);
//	res = sk.QueryStringValue( 0, cmd, &cmd_len);
//	pCommand.ReleaseBufferSetLength( cmd_len);
//	sk.Close();
//
//	if( res != ERROR_SUCCESS)
//		return false;
//
//	return !pCommand.IsEmpty(); // return true only if something valuable found
//}
