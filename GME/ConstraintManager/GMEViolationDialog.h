//###############################################################################################################################################
//
//	Object Constraint Language Generic Manager
//	GMEViolationDialog.h
//
//###############################################################################################################################################

#if !defined(AFX_GMEVIOLATIONDIALOG_H__6D74A53B_13E1_40AD_AD4A_C4D71881F0F7__INCLUDED_)
#define AFX_GMEVIOLATIONDIALOG_H__6D74A53B_13E1_40AD_AD4A_C4D71881F0F7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GMEConstraintEx.h"
#include "ConstraintManager.h"
#include "Gme.h"

namespace OclGmeCM {
	struct EvaluationRecord;
	#include <string>


//##############################################################################################################################################
//
//	C L A S S : CViolationDialog <<< + CDialog
//
//==============================================================================================================================================
//
//	D E S C R I P T I O N :
//
//##############################################################################################################################################

class CViolationDialog : public CDialog
{

// D I A L O G   E X T E N S I O N   B Y   I M P L E M E N T O R   B E G I N
// ============================================================


private :
	CComPtr<IMgaProject>			m_spProject;
	CComPtr<IGMEOLEApp>				m_oleapp;

	CImageList						m_lstErrorImages;
	CImageList						m_lstConstraintImages;
	CImageList						m_lstLineImages;
	std::vector<EvaluationRecord>	m_vecRecords;
	bool							m_bAbort;
	bool							m_bOK;
	bool							m_bExpanded;
	int								m_iSelected;
	int 							m_iSelectedConstraint;
	int								m_iSelectedError;
	int								m_iSelectedVariable;
	CComPtr<IUnknown> m_gotopunk;

public :
	void GetGotoPunk(IUnknown ** ppunk) 
	{
		*ppunk=NULL; 
		if (m_gotopunk) 
			m_gotopunk.CopyTo(ppunk); 
	}
	void AddItem( const EvaluationRecord& item );
	void EnableAbort();
	void EnableOK();
	bool IsExpanded() const;

private :
	void addFunctions();
	void SelectConstraint( bool bNext );
	void SelectError( bool bNext );
	void DisplayErrorItem();
	void GotoObject();
	void DisplayConstraintItem();
	void RefreshWidgets( bool bNewConstraint );
	void SortConstraints( int iColumn );
	static int CALLBACK CompareConstraint( LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort );

// D I A L O G   E X T E N S I O N   B Y   I M P L E M E N T O R   E N D
// ============================================================


// Construction
public:
	CViolationDialog( bool bExpanded, CWnd* pParent, IMgaProject *project);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CViolationDialog)
	enum { IDD = IDD_VIOLATION_DIALOG };
	CStatic	m_lblConstraint;
	CButton	m_frmMessage;
	CButton	m_btnPrevious;
	CButton	m_btnNext;
	CButton	m_btnExpand;
	CTabCtrl	m_tabPages;
	CStatic		m_lblGeneralMessage;
	CListCtrl	m_lstObjects;
	CButton	m_btnAbort;
	CListCtrl	m_lstExpression;
	CListCtrl	m_lstErrors;
	CListCtrl	m_lstConstraints;
	CStatic		m_imgLarge;
	CButton	m_btnClose;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViolationDialog)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CViolationDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnClickClose();
	afx_msg void OnKeyDownConstraints(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickAbort();
	afx_msg void OnClickConstraints(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickErrors(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickVariable(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeyDownErrors(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelectionChangedTab(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickExpand();
	afx_msg void OnClickNext();
	afx_msg void OnClickPrevious();
	afx_msg void OnClickConstraintsColumn(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

}; // namespace OclGmeCM

#endif // !defined(AFX_GMEVIOLATIONDIALOG_H__6D74A53B_13E1_40AD_AD4A_C4D71881F0F7__INCLUDED_)
