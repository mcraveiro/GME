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

class NameSpecTbl : public CListCtrl
{
// Construction
public:
	NameSpecTbl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(NameSpecTbl)
	//}}AFX_VIRTUAL

// Implementation
public:
	CImageList m_checkImages;
	bool GetRow( int rowID, CString& name, CString& disp_name, CString& kind);
	void AddRow( int rowID, CString& name, CString& disp_name, CString& kind);

	virtual ~NameSpecTbl();
	virtual BOOL PreCreateWindow( CREATESTRUCT &cs);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	int HitTestEx(CPoint &point, int *col) const;
	CEdit* EditSubLabel( int nItem, int nCol );
	CComboBox* ShowInPlaceList( int nItem, int nCol, CStringList &lstItems, int nSel );

	static LPCTSTR MakeShortString(CDC* pDC, LPCTSTR lpszLong, int nColumnLen, int nOffset);


	// Generated message map functions
protected:
	//{{AFX_MSG(NameSpecTbl)
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
