#if !defined(AFX_PANNINGBUTTON_H__B7B65E67_B459_4C91_A578_16AF27271A47__INCLUDED_)
#define AFX_PANNINGBUTTON_H__B7B65E67_B459_4C91_A578_16AF27271A47__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PanningButton.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPanningButton window

class CPanningButton : public CWnd
{
// Construction
public:
	CPanningButton();
	bool Create();
	void SetBitmapDC(HWND owner, HDC bdc, HBITMAP oldBmp, CRect& ori, CRect& rect, COLORREF& bkgrnd);
	void SetViewRect(CRect vrect);

private:
	HCURSOR		m_arrowh;
	HCURSOR		m_sizeallh;
	COLORREF	m_backgorund;

	bool	m_inMove;
	CPoint	m_moveStartPoint;
	CRect	m_moveStartRect;

	HWND	m_owner;
	HDC		m_bitmapDC;
	HBITMAP	m_oldBmp;
	CRect	m_bitmapSizeOri;
	CRect	m_bitmapSizeStored;
	CRect	m_bitmapOnScreen;

	CRect	m_viewRectOnStored;

	CRect	m_viewRectOnScreen;
	CRect	m_viewRectInMove;

	double m_storedToScreenRx;
	double m_storedToScreenRy;
	double m_oriToStoredRx;
	double m_oriToStoredRy;

	void BmpToScreen(CRect clientr);
	void ViewRectToScreen(CRect& vRect);
	void DeleteDeviceContext(HDC bDC, HBITMAP oldBmp);

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPanningButton)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPanningButton();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPanningButton)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PANNINGBUTTON_H__B7B65E67_B459_4C91_A578_16AF27271A47__INCLUDED_)
