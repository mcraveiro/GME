#if !defined(AFX_INSPECTORLIST_H__93C5CFDD_9E01_4E3C_B924_3188772DCA2C__INCLUDED_)
#define AFX_INSPECTORLIST_H__93C5CFDD_9E01_4E3C_B924_3188772DCA2C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "InspectorDefs.h"
#include "InPlaceManager.h"
// InspectorList.h : header file
//



#include"ItemData.h"

typedef CArray<CListItem,CListItem&> CListItemArray;


class CInspectorListSettings
{
public:
	CInspectorListSettings():m_nDivider(INSP_DIVIDER_POSITION){};

	UINT m_nDivider;
};
/////////////////////////////////////////////////////////////////////////////
// CInspectorList window
class CInspectorList : public CListBox
{
	friend CInPlaceManager;
// Construction
public:
	CInspectorList(bool bCategories=true);
	bool AddItem(CListItem ListItem);
	bool InsertItem(CListItem ListItem, int nIndex);
	bool RemoveItem(int nIndex);
	void RemoveAll();

	void UpdateItems(CArray<CListItem,CListItem&> &ListItemArray);

	void GetSelItems(CArray<int,int>&IndexArray,CArray<CListItem,CListItem&> &ListItemArray);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInspectorList)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

	afx_msg void OnArrowClicked();
	afx_msg void OnEditorClicked();
	afx_msg LRESULT OnEditEndOK(WPARAM lParam, LPARAM wParam);
// Implementation
public:
	void GetItem(int nIndex, CListItem &ListItem);
	void RefreshState();
	void NotifyParent(UINT nSelItem);

	void DeleteByKey(void* dwKey);
	void DeleteByKey(void* dwKey, DWORD dwUserData);
	bool FindByKey(void*, CListItem&);
	bool FindByKey(void*, DWORD, CListItem&);

	bool OnRightSideClick(CPoint point);
	bool OnPlusMinusClick(CPoint point);
	void DoCollapseExpand(int);
	void ResetContent();
	CInspectorListSettings m_Settings;
	virtual ~CInspectorList();

	bool m_bCategories;

	// Generated message map functions
protected:
	CFont m_entryFont;
	CFont m_entryBoldFont;
	//{{AFX_MSG(CInspectorList)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSelChange();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnListContextResetToDefault();
	afx_msg void OnListContextCopy();
	afx_msg void OnOpenRefered();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	void UpdateItem(const CListItem& srcListItem, CListItem& dstListItem, int nIndex);
	void SetDefault();
	void SetHelp(int nIndex);
	bool SelectNextItem(BOOL reverse);	// JIRA GME-178

	CInPlaceManager m_InPlaceManager;

	CPoint m_ptOldTop;
	CPoint m_ptOldBottom;
	void InvertLine(CDC* pDC,CPoint ptSrc,CPoint ptDst);
	BOOL m_bIsDividerDrag;
	HCURSOR m_hCurSize;
	HCURSOR m_hCurArrow;

	CListItemArray m_ListItemArray;

	int m_ItemHeight;
	int m_ComboboxLineHeight;
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INSPECTORLIST_H__93C5CFDD_9E01_4E3C_B924_3188772DCA2C__INCLUDED_)
