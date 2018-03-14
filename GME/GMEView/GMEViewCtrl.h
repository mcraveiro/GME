#if !defined(AFX_GMEVIEWCTRL_H__1D56CE4E_1840_4991_A5EB_6F6DA3CEEABB__INCLUDED_)
#define AFX_GMEVIEWCTRL_H__1D56CE4E_1840_4991_A5EB_6F6DA3CEEABB__INCLUDED_


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "GMEViewFrame.h"
#include "..\GmeActiveBrowser\MgaObjectEventList.h"

// GMEViewCtrl.h : Declaration of the CGMEViewCtrl ActiveX Control class.


// CGMEViewCtrl : See GMEViewCtrl.cpp for implementation.

class CGMEViewCtrl : public COleControl
{
	DECLARE_DYNCREATE(CGMEViewCtrl)

// Constructor
public:
	CGMEViewCtrl();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGMEViewCtrl)
	public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	~CGMEViewCtrl();

	DECLARE_OLECREATE_EX(CGMEViewCtrl)    // Class factory and guid
	DECLARE_OLETYPELIB(CGMEViewCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CGMEViewCtrl)     // Property page IDs
	DECLARE_OLECTLTYPE(CGMEViewCtrl)		// Type name and misc status

	CGMEViewFrame*	m_gmeViewFrame;

	// Just for temporary store until GMEViewDlg is created
	CComPtr<IMgaProject>	mgaProject;
	CComPtr<IMgaMetaModel>	mgaMetaModel;
	CComPtr<IMgaModel>		mgaModel;
	int						aspect;

// Message maps
	//{{AFX_MSG(CGMEViewCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dispatch maps
	//{{AFX_DISPATCH(CGMEViewCtrl)
	afx_msg void SetCurrentProject(LPUNKNOWN project);
	afx_msg void SetMetaModel(LPUNKNOWN meta);
	afx_msg void SetModel(LPUNKNOWN model);
	afx_msg void ChangeAspect(LONG index);
	afx_msg void CycleAspect(void);
	afx_msg void Invalidate(void);
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()

	afx_msg void AboutBox();

	void OnMgaObjectEvent(IMgaObject* obj, unsigned long eventmask);
	void OnMgaGlobalEvent(globalevent_enum event);

// Event maps
	//{{AFX_EVENT(CGMEViewCtrl)
	void FireAspectChanged(LONG index)
		{FireEvent(eventidAspectChanged, EVENT_PARAM(VTS_I4), index); }
	void FireZoomChanged(LONG index)
		{FireEvent(eventidZoomChanged, EVENT_PARAM(VTS_I4), index); }
	void FireWriteStatusZoom(LONG zoomVal)
		{FireEvent(eventidWriteStatusZoom, EVENT_PARAM(VTS_I4), zoomVal); }
	//}}AFX_EVENT
	DECLARE_EVENT_MAP()

// Native COM interfaces - peter
	DECLARE_INTERFACE_MAP()

	BEGIN_INTERFACE_PART(EventSink, IMgaEventSink)
		STDMETHOD(GlobalEvent(globalevent_enum event));
		STDMETHOD(ObjectEvent(IMgaObject* obj, unsigned long eventmask, VARIANT v));
	END_INTERFACE_PART(EventSink)

// Dispatch and event IDs
public:
	enum {
		//{{AFX_DISP_ID(CGMEViewCtrl)
		dispidCycleAspect = 5L,
		dispidChangeAspect = 4L,
		dispidInvalidate = 6L,
		dispidSetCurrentProject = 1L,
		dispidSetMetaModel = 2L,
		dispidSetModel = 3L,
		eventidAspectChanged = 1L,
		eventidWriteStatusZoom = 3L,
		eventidZoomChanged = 2L
		//}}AFX_DISP_ID
	};

	void SendAspectChanged(LONG index);
	void SendZoomChanged(LONG index);
	void SendWriteStatusZoom(LONG zoomVal);

protected:
	CMgaObjectEventList m_MgaObjectEventList;
	void PropagateMgaMessages();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GMEVIEWCTRL_H__1D56CE4E_1840_4991_A5EB_6F6DA3CEEABB__INCLUDED_)
