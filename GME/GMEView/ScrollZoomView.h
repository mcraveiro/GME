/*

  Custom ScrollView class by Peter

*/

#if !defined(AFX_SROLLZOOMVIEW_H__D62F2EF7_6999_4D3A_AE87_336E50002FF2__INCLUDED_)
#define AFX_SROLLZOOMVIEW_H__D62F2EF7_6999_4D3A_AE87_336E50002FF2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SrollZoomView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// class CScrollZoomView supports simple scrolling and scaling

class CScrollZoomView : public CView
{
	DECLARE_DYNAMIC(CScrollZoomView)

// Constructors
protected:
	CScrollZoomView();

public:
	static AFX_DATA const SIZE sizeDefault;
		// used to specify default calculated page and line sizes

	// in logical units - call one of the following Set routines
	void SetScaleToFitSize(SIZE sizeTotal);
	void SetScrollSizes(int nMapMode, SIZE sizeTotal, int scalePercent,
						const SIZE& sizePage = sizeDefault,
						const SIZE& sizeLine = sizeDefault);

// Attributes
public:
	CPoint GetScrollPosition() const;		// upper corner of scrolling
	CSize GetTotalSize() const				{ return m_totalLog; }				// logical size

	// for device units
	CPoint GetDeviceScrollPosition() const;
	void GetDeviceScrollSizes(int& nMapMode, SIZE& sizeTotal,
			SIZE& sizePage, SIZE& sizeLine) const;

// Operations
public:
	void ScrollToPosition(POINT pt);	// set upper left position
	void FillOutsideRect(CDC* pDC, CBrush* pBrush);
	void ResizeParentToFit(BOOL bShrinkOnly = TRUE);
	BOOL DoMouseWheel(UINT fFlags, short zDelta, CPoint point);

	void notifyPanning(CPoint pt);

// Implementation
protected:
	bool m_zoomScroll;
	CPoint m_zoomP;
	bool m_noHscroll;	// hack terge
	bool m_noVscroll;	// hack terge

	int m_nMapMode;
	CSize m_totalLog;			// total size in logical units (no rounding)
	CSize m_totalDev;			// total size in device units
	CSize m_pageDev;			// per page scroll size in device units
	CSize m_lineDev;			// per line scroll size in device units
	int	  m_scalePercent;		// Modified by Peter

	BOOL m_bCenter;				// Center output if larger than total size
	BOOL m_bInsideUpdate;		// internal state for OnSize callback
	void CenterOnPoint(CPoint ptCenter);
	void ScrollToDevicePosition(POINT ptDev);	// explicit scrolling no checking

protected:
	virtual void OnDraw(CDC* pDC) = 0;			// pass on pure virtual

	void UpdateBars();			// adjust scrollbars etc
	BOOL GetTrueClientSize(CSize& size, CSize& sizeSb);
		// size with no bars
	void GetScrollBarSizes(CSize& sizeSb);
	void GetScrollBarState(CSize sizeClient, CSize& needSb,
		CSize& sizeRange, CPoint& ptMove, BOOL bInsideClient);

public:
	virtual ~CScrollZoomView();
#ifdef _DEBUG
	virtual void Dump(CDumpContext&) const;
	virtual void AssertValid() const;
#endif //_DEBUG
	virtual void CalcWindowRect(LPRECT lpClientRect,
		UINT nAdjustType = adjustBorder);
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);

	// scrolling implementation support for OLE
	virtual BOOL OnScroll(UINT nScrollCode, UINT nPos, BOOL bDoScroll = TRUE);
	virtual BOOL OnScrollBy(CSize sizeScroll, BOOL bDoScroll = TRUE);

	//{{AFX_MSG(CScrollZoomView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT fFlags, short zDelta, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SROLLZOOMVIEW_H__D62F2EF7_6999_4D3A_AE87_336E50002FF2__INCLUDED_)
