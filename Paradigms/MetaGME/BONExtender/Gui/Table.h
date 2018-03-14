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

class SelConf;

class CTable : public CListCtrl
{
// Construction
public:
	CTable();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTable)
	//}}AFX_VIRTUAL
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual BOOL PreCreateWindow(CREATESTRUCT &cs);

// Implementation
public:
	SelConf * m_parent;

	void addRow(int rowID, CString& role, char kind, int clique, bool isPrimary, CString& repr);
	bool getRow(int rowID, CString& role, bool& extend, CString& repr);
	virtual ~CTable();
	int HitTestEx(CPoint &point, int *col) const;
	CEdit* EditSubLabel( int nItem, int nCol );
	CComboBox* ShowInPlaceList( int nItem, int nCol, CStringList &lstItems, int nSel );

protected:
	static LPCTSTR MakeShortString(CDC* pDC, LPCTSTR lpszLong, int nColumnLen, int nOffset);
	int m_cxClient;

	// Generated message map functions
protected:
	//{{AFX_MSG(CTable)
	afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnSort(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ASPECTSPECTBL_H__62A172B9_B4D4_497D_B466_B7BB7BF98B7F__INCLUDED_)
