#if !defined(AFX_TESTWINDOW_H__A7B87C94_3DCC_4768_82F6_C913E586FFE5__INCLUDED_)
#define AFX_TESTWINDOW_H__A7B87C94_3DCC_4768_82F6_C913E586FFE5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TestWindow.h : header file
//

#include "ScrollZoomView.h"
#include <string>

/////////////////////////////////////////////////////////////////////////////
// CTestWindow window

class CTestWindow : public CWnd
{
// Construction
public:
	CTestWindow(const CString& titleStr, const CRect& pos, CScrollZoomView* parent);
	virtual ~CTestWindow();

// Attributes
protected:
	CFont				txtMetricFont;
	COLORREF			backgroundColor;
	COLORREF			canvasBgndColor;
	CScrollZoomView*	parentWindowPtr;
	CString				title;
	CRect				position;
	bool				created;

public:

// Implementation
	CString	GetTitle(void) const { return title; };
	void	CreateWindowSpecial(const CString& wndClassStr);
	void	DestroyWindowSpecial(void);
	void	MoveWindowSpecial(int deltax, int deltay);
	bool	IsIntersectRect(const CRect& rect);

	// Generated message map functions
protected:
	//{{AFX_MSG(CTestWindow)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTWINDOW_H__A7B87C94_3DCC_4768_82F6_C913E586FFE5__INCLUDED_)
