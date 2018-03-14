///////////////////////////////////////////////////////////////////////////////
// 
// CTreeCtrlEx - Multiple selection tree control for MFC
// 
// Bendik Engebretsen (c) 1997-1999
// bendik@techsoft.no
// http://www.techsoft.no/bendik/
//
//

#ifndef __TREECTRLEX_H_ACT_BRW
#define __TREECTRLEX_H_ACT_BRW
// this file is used in several other projects, 
// let's avoid complications by using different
// #defines in each case

#define TVGN_EX_ALL			0x000F

// Tihamer Levendovszky 12/07/2001
#include "StringArrayEx.h"

/////////////////////////////////////////////////////////////////////////////
// CTreeCtrlEx window

class CTreeCtrlEx : public CTreeCtrl
{
	DECLARE_DYNAMIC(CTreeCtrlEx)

// Construction
public:
	CTreeCtrlEx();
    virtual ~CTreeCtrlEx();
	BOOL Create(DWORD dwStyle, DWORD dwExStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

// Attributes
public:
	UINT GetSelectedCount() const;
	HTREEITEM GetNextItem(HTREEITEM hItem, UINT nCode);
	HTREEITEM GetFirstSelectedItem();
	HTREEITEM GetNextSelectedItem(HTREEITEM hItem);
	HTREEITEM GetPrevSelectedItem(HTREEITEM hItem);
	HTREEITEM ItemFromData(DWORD_PTR dwData, HTREEITEM hStartAtItem=NULL) const;

	BOOL SelectItemEx(HTREEITEM hItem, BOOL bSelect=TRUE);

	BOOL SelectItems(HTREEITEM hFromItem, HTREEITEM hToItem);
	void ClearSelection(BOOL bMultiOnly=FALSE);

	// Tihamer Levendovszky 12/07/2001
	int FindTextInItems(CString &strText, HTREEITEM hStartAtItem,
					CStringArrayEx &strResults, CArray<HTREEITEM,HTREEITEM>& hResults);
	BOOL CreateDragImageEx(CPoint ptDragPoint);
	BOOL CTreeCtrlEx::DeleteItem(HTREEITEM hItem);

protected:
	void SelectMultiple( HTREEITEM hClickedItem, UINT nFlags, CPoint point );

private:
	BOOL		m_bSelectPending;
	CPoint		m_ptClick;
	HTREEITEM	m_hClickedItem;
	HTREEITEM	m_hFirstSelectedItem;
	BOOL		m_bSelectionComplete;
	BOOL		m_bEditLabelPending;
	UINT_PTR		m_idTimer;
    //TCHAR *     m_pchTip ;
	//WCHAR *     m_pwchTip;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTreeCtrlEx)
	//}}AFX_VIRTUAL

// Implementation
public:
	void ScrollUp();
	void DeleteDragImageEx();
	CPoint m_ptHotSpot;
	CImageList m_CurrentDragImage;	

	// Generated message map functions
protected:
    //{{AFX_VIRTUAL(CTreeCtrlCh)
   virtual void PreSubclassWindow();
   //}}AFX_VIRTUAL

	//{{AFX_MSG(CTreeCtrlEx)
    //virtual int OnToolHitTest(CPoint point, TOOLINFO * pTI) const;
    //afx_msg BOOL OnToolTipText( UINT id, NMHDR * pNMHDR, LRESULT * pResult );
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnItemexpanding(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnSetfocus(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnKillfocus(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:
	CTreeCtrlEx(const CTreeCtrlEx&);
};


HTREEITEM GetTreeItemFromData(CTreeCtrl& treeCtrl, DWORD_PTR dwData, HTREEITEM hStartAtItem=NULL);

#endif
