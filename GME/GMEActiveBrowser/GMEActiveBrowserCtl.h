#if !defined(AFX_GMEACTIVEBROWSERCTL_H__F16A65B4_8221_4ABF_B2D6_463802025C5F__INCLUDED_)
#define AFX_GMEACTIVEBROWSERCTL_H__F16A65B4_8221_4ABF_B2D6_463802025C5F__INCLUDED_

#include "ActiveBrowserPropertyFrame.h"
#include "MgaObjectEventList.h"	// Added by ClassView

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// GMEActiveBrowserCtl.h : Declaration of the CGMEActiveBrowserCtrl ActiveX Control class.

/////////////////////////////////////////////////////////////////////////////
// CGMEActiveBrowserCtrl : See GMEActiveBrowserCtl.cpp for implementation.


class CGMEActiveBrowserCtrl : public COleControl
{
	DECLARE_DYNCREATE(CGMEActiveBrowserCtrl)

	CActiveBrowserPropertyFrame* m_pPropFrame;
	CGMEActiveBrowserApp* m_pApp;
// Constructor
public:
	CGMEActiveBrowserCtrl();

	// Event handlers for the MGA layer
	void OnMgaObjectEvent(IMgaObject * obj, unsigned long eventmask, VARIANT v);
	void OnMgaGlobalEvent(globalevent_enum event);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGMEActiveBrowserCtrl)
	public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();
	virtual DWORD GetControlFlags();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Event maps
	//{{AFX_EVENT(CGMEActiveBrowserCtrl)
	void FireDblClickMgaObject(LPUNKNOWN pMgaObject)
		{FireEvent(eventidDblClickMgaObject,EVENT_PARAM(VTS_UNKNOWN), pMgaObject);}
	void FireShowAttributes(LPUNKNOWN pMgaObject)
		{FireEvent(eventidShowAttributes,EVENT_PARAM(VTS_UNKNOWN), pMgaObject);}
	void FireShowPreferences(LPUNKNOWN pMgaObject)
		{FireEvent(eventidShowPreferences,EVENT_PARAM(VTS_UNKNOWN), pMgaObject);}
	void FireShowProperties()
		{FireEvent(eventidShowProperties,EVENT_PARAM(VTS_NONE));}
	void FireClickMgaObject(LPUNKNOWN pMgaObject)
		{FireEvent(eventidClickMgaObject,EVENT_PARAM(VTS_UNKNOWN), pMgaObject);}
	void FireShowInParentMgaObject(LPUNKNOWN pMgaObject)
		{FireEvent(eventidShowInParentMgaObject,EVENT_PARAM(VTS_UNKNOWN), pMgaObject);}
	void FireRootFolderNameChanged()
		{FireEvent(eventidRootFolderNameChanged, EVENT_PARAM(VTS_NONE)); }
	void FireShowFindDlg()
		{FireEvent(eventidShowFindDlg, EVENT_PARAM(VTS_NONE)); }
	void FireShowObjInspDlg()
		{FireEvent(eventidShowObjInspDlg, EVENT_PARAM(VTS_NONE)); }
	void FireCycleObjInsp(VARIANT_BOOL pFrwd)
		{FireEvent(eventidCycleObjInsp, EVENT_PARAM(VTS_BOOL), pFrwd); }
	//}}AFX_EVENT
	DECLARE_EVENT_MAP()


// Implementation
protected:
	~CGMEActiveBrowserCtrl();

	DECLARE_OLECREATE_EX(CGMEActiveBrowserCtrl)    // Class factory and guid
	DECLARE_OLETYPELIB(CGMEActiveBrowserCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CGMEActiveBrowserCtrl)     // Property page IDs
	DECLARE_OLECTLTYPE(CGMEActiveBrowserCtrl)		// Type name and misc status

// Message maps
	//{{AFX_MSG(CGMEActiveBrowserCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dispatch maps
	//{{AFX_DISPATCH(CGMEActiveBrowserCtrl)
	afx_msg short SetCurrentProject(LPUNKNOWN pMgaProject);
	afx_msg short CloseCurrentProject();
	afx_msg void RefreshAll();
	afx_msg LPUNKNOWN GetSelectedMgaObjects();
	afx_msg void OpenProject(LPCTSTR pProjectName);

	afx_msg void FocusItem(BSTR Id);
	afx_msg void OpenItem(BSTR Id);
	afx_msg void OpenSubtree(BSTR Id);
	afx_msg void CloseSubtree(BSTR Id);
	afx_msg void Up();
	afx_msg void Down();
	afx_msg void ChangePropPage(browser_page page);
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()



	// Native COM interfaces - peter
	DECLARE_INTERFACE_MAP()
public:
	BEGIN_INTERFACE_PART(EventSink, IMgaEventSink)
		STDMETHOD(GlobalEvent(globalevent_enum event));
		STDMETHOD(ObjectEvent(IMgaObject * obj, unsigned long eventmask, VARIANT v));
	END_INTERFACE_PART(EventSink)
// Dispatch and event IDs
public:
	enum {
	//{{AFX_DISP_ID(CGMEActiveBrowserCtrl)
	dispidSetCurrentProject = 1L,
	dispidCloseCurrentProject = 2L,
	dispidRefreshAll = 3L,
	dispidGetSelectedMgaObjects = 4L,
	dispidOpenProject = 5L,
	dispidFocusItem = 6L,
	dispidOpenItem = 7L,
	dispidOpenSubtree = 8L,
	dispidCloseSubtree = 9L,
	dispidUp = 10L,
	dispidDown = 11L,
	dispidChangePropPage = 12L,
	eventidDblClickMgaObject = 1L,
	eventidShowAttributes = 2L,
	eventidShowPreferences = 3L,
	eventidShowProperties = 4L,
	eventidClickMgaObject = 5L,
	eventidShowInParentMgaObject = 6L,
	eventidRootFolderNameChanged = 7L,
	eventidShowFindDlg = 8L,
	eventidShowObjInspDlg = 9L,
	eventidCycleObjInsp = 10L
	//}}AFX_DISP_ID
	};
private:
	void PropagateMgaMessages();
	CMgaObjectEventList m_MgaObjectEventList;
	int CreateActiveBrowser();
public:
	void HighlightItem(IUnknown* item, int highlight);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GMEACTIVEBROWSERCTL_H__F16A65B4_8221_4ABF_B2D6_463802025C5F__INCLUDED)
