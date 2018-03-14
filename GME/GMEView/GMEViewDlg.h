#if !defined(AFX_GMEVIEWDLG_H__F859CB75_586D_4c49_AACE_69B3B7500DFB__INCLUDED_)
#define AFX_GMEVIEWDLG_H__F859CB75_586D_4c49_AACE_69B3B7500DFB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GMEViewDlg.h : header file
//

#include "Resource.h"
#include "ModelPropertiesDlgBar.h"
#include "ScrollZoomView.h"
#include "GMEViewDropTarget.h"

#include <vector>
#include "TestWindow.h"

/////////////////////////////////////////////////////////////////////////////
// CGMEViewDlg dialog

class CGMEViewDropTarget;


class CGMEViewDlg : public CScrollZoomView
{
	DECLARE_DYNCREATE(CGMEViewDlg)

// Construction
public:
	CGMEViewDlg();   // standard constructor
	virtual ~CGMEViewDlg();

public:
// Dialog Data
	//{{AFX_DATA(CGMEViewDlg)
	enum { IDD = IDD_GMEVIEW_DIALOG };
	//}}AFX_DATA

	CComPtr<IMgaProject>		mgaProject;
	CComPtr<IMgaMetaModel>		mgaMetaModel;
	CComPtr<IMgaModel>			mgaModel;
	int							currentAspect;
	CString						strAspect;
	CString						regPath;

	COLORREF					canvasBgndColor;

	CString						defZoomLev;
	int							m_zoomVal;
	CRect						modelExtent;
	bool						modelExtentCalculated;

	static int					instanceCount;
	static bool					offScreenCreated;
	static CDC*					offScreen;
	static CBitmap*				ofsbmp;

	CFont						txtMetricFont;

	static CString				strTestWndClass;
	std::vector<CTestWindow*>	testWindows;
	std::vector<CTestWindow*>	displayedWindows;
	bool						inWindowDragging;
	CPoint						lastDragPoint;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGMEViewDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

public:
	void						SetCurrentProject(CComPtr<IMgaProject> project);
	void						SetMetaModel(CComPtr<IMgaMetaModel> meta);
	void						SetModel(CComPtr<IMgaModel> model);
	void						ChangeAspect(int aspect);
	void						Invalidate(void);
	void						SetScroll(void);

// Implementation
protected:
	void						CreateOffScreen(CDC* dc);
	int							RangedRand(int range_min, int range_max);
	void						CreateTestWindows(void);
	void						DestroyTestWindows(void);
	std::vector<CTestWindow*>	GetWindowsInRect(const CRect& rect);

	// Generated message map functions
	//{{AFX_MSG(CGMEViewDlg)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMove(int x, int y);
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
	afx_msg void OnDraw(CDC* pDC);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnSelchangeAspectTab(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void SendAspectChange(long index);
	void SendZoomChange(long index);
	void SendWriteStatusZoom(long zoomVal);

public:
	CGMEViewDropTarget m_DropTarget;

	BOOL DoDrop(eDragOperation doDragOp, COleDataObject* pDataObject, CPoint point);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GMEVIEWDLG_H__F859CB75_586D_4c49_AACE_69B3B7500DFB__INCLUDED_)
