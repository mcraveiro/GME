// GMEPanningWindow.h: interface for the CGMEPanningWindow class.
//
//////////////////////////////////////////////////////////////////////
//{{AFX_INCLUDES()
#include "PanningWindowWrapper.h"
//}}AFX_INCLUDES

#if !defined(AFX_GMEPANNINGWINDOW_H__EA4F9478_F8EB_4494_8D36_E31CF129200C__INCLUDED_)
#define AFX_GMEPANNINGWINDOW_H__EA4F9478_F8EB_4494_8D36_E31CF129200C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CGMEPanningWindow window

class CGMEPanningWindow : public CDockablePane
{
// Construction
public:
	CGMEPanningWindow();
	virtual ~CGMEPanningWindow() { theInstance = 0; };

	IDispatch * GetInterface() {
		LPUNKNOWN pUnk = m_PanningWindowWrapper.GetControlUnknown();

		// From there get the IDispatch interface of control.
		LPDISPATCH pDisp = NULL;
		pUnk->QueryInterface(IID_IDispatch, (LPVOID*)&pDisp);
		return pDisp;
	}

	// Dialog Data
	//{{AFX_DATA(CGMEPanningWindow)
	enum { IDD = IDD_PANNING_WINDOW_DIALOG };
	CPanningWindowWrapper	m_PanningWindowWrapper;
	//}}AFX_DATA

	static CGMEPanningWindow *theInstance;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGMEPanningWindow)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);

	// Generated message map functions
	//{{AFX_MSG(CGMEPanningWindow)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Implementation
public:
	void SetBitmapDC(HWND owner, HDC bdc, HBITMAP oldBmp, CRect& ori, CRect& rect, COLORREF& bkgrnd);
	void SetViewRect(CRect vrect);

	virtual void ShowPane(BOOL bShow, BOOL bDelay, BOOL bActivate/* = TRUE*/);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GMEPANNINGWINDOW_H__EA4F9478_F8EB_4494_8D36_E31CF129200C__INCLUDED_)
