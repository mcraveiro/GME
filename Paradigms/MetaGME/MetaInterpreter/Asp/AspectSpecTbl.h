#if !defined(AFX_ASPECTSPECTBL_H__62A172B9_B4D4_497D_B466_B7BB7BF98B7F__INCLUDED_)
#define AFX_ASPECTSPECTBL_H__62A172B9_B4D4_497D_B466_B7BB7BF98B7F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <AFXCMN.H>

// MyListCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMyListCtrl window

class CAspectSpecTbl : public CListCtrl
{
// Construction
public:
	CAspectSpecTbl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAspectSpecTbl)
	//}}AFX_VIRTUAL

// Implementation
public:
	CImageList m_checkImages;
	bool GetRow(int rowID, CString &role, CString& kindAspect, CString& isPrimary);
	void AddRow(int rowID, CString& role, CString& kindAspect, CString& isPrimary);
	virtual ~CAspectSpecTbl();
	int HitTestEx(CPoint &point, int *col) const;
	CEdit* EditSubLabel( int nItem, int nCol );
	CComboBox* ShowInPlaceList( int nItem, int nCol, CStringList &lstItems, int nSel );

	// Generated message map functions
protected:
	//{{AFX_MSG(CAspectSpecTbl)
	afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ASPECTSPECTBL_H__62A172B9_B4D4_497D_B466_B7BB7BF98B7F__INCLUDED_)
