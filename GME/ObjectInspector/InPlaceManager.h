// InPlaceManager.h: interface for the CInPlaceManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INPLACEMANAGER_H__35788679_EF59_416E_8C00_D294B83FBA07__INCLUDED_)
#define AFX_INPLACEMANAGER_H__35788679_EF59_416E_8C00_D294B83FBA07__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ArrowButton.h"
#include "HooverListbox.h"
#include "InPlaceEdit.h"

class CInspectorList;
class CInPlaceManager
{
public:
	void OnEditEnd();

	bool OnRightItemClick(int nIndex,CRect rectInPlace);
	void OnEditSingleLineEnd();
	void OnEditMultiLineEnd();
	void OnClickArrowButton(bool rightSideClick);
	void OnClickEditorButton();

	void ShowInPlace(CRect rectInPlace, int nIndex);
	void HideAllInPlace();


	CInPlaceManager(CInspectorList* pInspectorList);
	virtual ~CInPlaceManager();
	CInspectorList *m_pInspectorList;

	CArrowButton m_ArrowButton;
	CButton	m_EditorButton;
	CInPlaceEdit m_SingleEditCtrl;
	CInPlaceEdit m_MultiEditCtrl;
	int  m_nLastSelEditIndex;

private:
	void DisplayCombo(CRect rectBound);
	void DisplayCompassCheck(CRect rectBound);
	void DisplayCompassOpt(CRect rectBound);
	int  m_nCurrentIndex;

	void DisplayArrowButton(CRect rectBound);
	void DisplayEditorButton(CRect rectBound);
	void DisplayMultilineEdit(CRect rectBound, bool readOnly);
	void DisplaySingleLineEdit(CRect rectBound, bool readOnly);
	void DisplayColorCombo(CRect rectBound, bool rightSideClick);

	void HideArrowButton();
	void HideEditorButton();
	void HideEdit();

	static bool findInfoInMimeDB( const CString& pContentType, CString& pPrefApp, CString& pExtension);
	//implemented, but not used
	//static bool findCommand( const CString& pExtension, CString& pEditCommand);


};

#endif // !defined(AFX_INPLACEMANAGER_H__35788679_EF59_416E_8C00_D294B83FBA07__INCLUDED_)
