// GMEPartBrowser.h: interface for the CGMEPartBrowser class.
//
//////////////////////////////////////////////////////////////////////
//{{AFX_INCLUDES()
#include "PartBrowserWrapper.h"
//}}AFX_INCLUDES

#if !defined(AFX_GMEPARTBROWSER_H__59BC1DA9_E2FB_41e7_A562_389C1D6458B7__INCLUDED_)
#define AFX_GMEPARTBROWSER_H__59BC1DA9_E2FB_41e7_A562_389C1D6458B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"
#include "GuiMeta.h"

/////////////////////////////////////////////////////////////////////////////
// CGMEPartBrowser window

class CGMEPartBrowser : public CDockablePane
{
// Construction
public:
	CGMEPartBrowser();
	virtual ~CGMEPartBrowser() { theInstance = 0; };

	IDispatch * GetInterface() {
		LPUNKNOWN pUnk = m_PartBrowserWrapper.GetControlUnknown();

		// From there get the IDispatch interface of control.
		LPDISPATCH pDisp = NULL;
		pUnk->QueryInterface(IID_IDispatch, (LPVOID*)&pDisp);
		return pDisp;
	}

	// Dialog Data
	//{{AFX_DATA(CGMEPartBrowser)
	enum { IDD = IDD_PART_BROWSER_DIALOG };
	CPartBrowserWrapper	m_PartBrowserWrapper;
	//}}AFX_DATA

	static CGMEPartBrowser *theInstance;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGMEPartBrowser)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);

	// Generated message map functions
	//{{AFX_MSG(CGMEPartBrowser)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnAspectChangedGmePartBrowserCtrl(LONG index);
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Implementation
public:
	void SetProject(CComPtr<IMgaProject>& mgaProject);
	void SetMetaModel(CGuiMetaModel* meta);
	void SetBgColor(COLORREF bgColor);
	void ChangeAspect(int ind);
	void CycleAspect();
	void RePaint(void);

protected:
	CGuiMetaModel*	guiMetaModel;
	CFont			m_font;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GMEPARTBROWSER_H__59BC1DA9_E2FB_41e7_A562_389C1D6458B7__INCLUDED_)
