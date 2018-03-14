//###############################################################################################################################################
//
//	Object Constraint Language Generic Manager
//	GMESyntacticSemanticDialog.h
//
//###############################################################################################################################################

#if !defined(AFX_GMESYNTACTICSEMANTICDIALOG_H__317C600D_17A5_449C_9672_F87AF57491E9__INCLUDED_)
#define AFX_GMESYNTACTICSEMANTICDIALOG_H__317C600D_17A5_449C_9672_F87AF57491E9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GMEConstraintEx.h"

//##############################################################################################################################################
//
//	C L A S S : CSyntacticSemanticDialog <<< + CDialog
//
//==============================================================================================================================================
//
//	D E S C R I P T I O N :
//
//##############################################################################################################################################

class CSyntacticSemanticDialog : public CDialog
{

// D I A L O G   E X T E N S I O N   B Y   I M P L E M E N T O R   B E G I N
// ============================================================

private :
	CImageList							m_lstErrorImages;
	CImageList							m_lstConstraintImages;
	CImageList							m_lstLineImages;
	OclGme::ConstraintFunctionVector	m_vecConstraintFunctions;
	OclGme::ConstraintVector				m_vecConstraints;
	int										m_iSelected;
	int										m_iSelectedError;

public :
	void AddItem( const OclGme::SpConstraint& item );
	void AddItem( const OclGme::SpConstraintFunction& item );

private :
	void SelectConstraint( bool bNext );
	void SelectError( bool bNext );
	void GetConstraintRow( int iPos, int& eType, CString& strContext, CString& strName );
	void AddConstraintRow( int iPos );
	void DisplayConstraintItem();
	void DisplayErrorItem();
	void SortConstraints( int iColumn );
	static int CALLBACK CompareConstraint( LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort );

// D I A L O G   E X T E N S I O N   B Y   I M P L E M E N T O R   E N D
// ============================================================

// Construction
public:
	CSyntacticSemanticDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSyntacticSemanticDialog)
	enum { IDD = IDD_SYNTAX_SEMANTIC_DIALOG };
	CListCtrl	m_lstErrors;
	CListCtrl	m_lstConstraints;
	CListCtrl	m_lstExpression;
	CStatic		m_lblGeneralMessage;
	CStatic		m_imgLarge;
	CButton	m_btnClose;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSyntacticSemanticDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSyntacticSemanticDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnClickClose();
	afx_msg void OnKeyDownConstraints(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeyDownErrors(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickErrors(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickConstraintsColumn(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickConstraints(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GMESYNTACTICSEMANTICDIALOG_H__317C600D_17A5_449C_9672_F87AF57491E9__INCLUDED_)
