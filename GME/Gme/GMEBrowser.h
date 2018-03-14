//{{AFX_INCLUDES()
#include "gmeactivebrowser.h"
//}}AFX_INCLUDES
#if !defined(AFX_GMEBROWSER_H__7157C233_E649_11D3_9206_00104B98EAD9__INCLUDED_)
#define AFX_GMEBROWSER_H__7157C233_E649_11D3_9206_00104B98EAD9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GMEBrowser.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGMEBrowser dialog

class CGMEBrowser : public CDockablePane
{
// Construction
public:
	CGMEBrowser(CWnd* pParent = NULL);   // standard constructor
	virtual ~CGMEBrowser() { theInstance = 0; }

	bool GetSelectedItem(CComPtr<IMgaObject> &item);
	bool GetSelectedItems(CComPtr<IMgaObjects> &items);
	void SetProject(CComPtr<IMgaProject>& mgaProject);
	void CloseProject();
	void RefreshAll();
	void ShowObject(LPUNKNOWN selected);
	void ShowObjectInParent( LPUNKNOWN selected);
	void ShowFindDlg();
	void ShowObjInsp();
	void CycleObjInsp( VARIANT_BOOL frwd);
	void FocusItem(BSTR Id);
	void ShowAttrPref(bool isAttr, LPUNKNOWN selected);
	void SetCurrObject(LPUNKNOWN pMgaObject);
	IDispatch * GetInterface() 
	{
		LPUNKNOWN pUnk = m_GMEActiveBrowser.GetControlUnknown();

		// From there get the IDispatch interface of control.
		LPDISPATCH pDisp = NULL;
		pUnk->QueryInterface(IID_IDispatch, (LPVOID*)&pDisp);
		return pDisp;
	};

// Dialog Data
	//{{AFX_DATA(CGMEBrowser)
	enum { IDD = IDD_BROWSER_DIALOG };
	CGMEActiveBrowser	m_GMEActiveBrowser;
	//}}AFX_DATA

	static CGMEBrowser *theInstance;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGMEBrowser)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL


// Implementation
protected:
	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);

	// Generated message map functions
	//{{AFX_MSG(CGMEBrowser)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDblClickMgaObjectActivebrowserCtlr1(LPUNKNOWN selected);
	afx_msg void OnCRonObjectActivebrowserCtlr1(LPUNKNOWN selected);
	afx_msg void OnShowAttributesActivebrowserctrl1(LPUNKNOWN object);
	afx_msg void OnShowPreferencesActivebrowserctrl1(LPUNKNOWN object);
	afx_msg void OnDblClickMgaObjectGmeActiveBrowserCtrl(LPUNKNOWN pMgaObject);
	afx_msg void OnShowAttributesGmeActiveBrowserCtrl(LPUNKNOWN pMgaObject);
	afx_msg void OnShowPreferencesGmeActiveBrowserCtrl(LPUNKNOWN pMgaObject);
	afx_msg void OnShowPropertiesGmeActiveBrowserCtrl();
	afx_msg void OnClickMgaObjectGmeActiveBrowserCtrl(LPUNKNOWN pMgaObject);
	afx_msg void OnShowInParentMgaObjectGmeActiveBrowserCtrl(LPUNKNOWN pMgaObject);
	afx_msg void OnShowFindDlgGmeActiveBrowserCtrl();
	afx_msg void OnShowObjInspDlgGmeActiveBrowserCtrl();
	afx_msg void OnCycleObjsInspGmeActiveBrowserCtrl(VARIANT_BOOL frwd);
	afx_msg void OnRootFolderNameChangedGmeActiveBrowserCtrl();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GMEBROWSER_H__7157C233_E649_11D3_9206_00104B98EAD9__INCLUDED_)
