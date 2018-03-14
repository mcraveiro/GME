#if !defined(AFX_PARTBROWSERCTRL_H__6E43458B_2522_4805_9C82_4ABE0A87F7BC__INCLUDED_)
#define AFX_PARTBROWSERCTRL_H__6E43458B_2522_4805_9C82_4ABE0A87F7BC__INCLUDED_


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "PartBrowserDlg.h"
#include "..\GMEActiveBrowser\MgaObjectEventList.h"

// PartBrowserCtrl.h : Declaration of the CPartBrowserCtrl ActiveX Control class.


// CPartBrowserCtrl : See PartBrowserCtrl.cpp for implementation.

class CPartBrowserCtrl : public COleControl
{
	DECLARE_DYNCREATE(CPartBrowserCtrl)

// Constructor
public:
	CPartBrowserCtrl();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPartBrowserCtrl)
	public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	~CPartBrowserCtrl();

	DECLARE_OLECREATE_EX(CPartBrowserCtrl)    // Class factory and guid
	DECLARE_OLETYPELIB(CPartBrowserCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CPartBrowserCtrl)     // Property page IDs
	DECLARE_OLECTLTYPE(CPartBrowserCtrl)		// Type name and misc status

	CPartBrowserDlg	m_partBrowserDlg;

// Message maps
	//{{AFX_MSG(CPartBrowserCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dispatch maps
	//{{AFX_DISPATCH(CPartBrowserCtrl)
	afx_msg void SetCurrentProject(LPUNKNOWN project);
	afx_msg void SetMetaModel(LPUNKNOWN meta);
	afx_msg void SetBgColor(OLE_COLOR oleBgColor);
	afx_msg void ChangeAspect(LONG index);
	afx_msg void CycleAspect(void);
	afx_msg void RePaint(void);
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()

	afx_msg void AboutBox();

	void OnMgaObjectEvent(IMgaObject* obj, unsigned long eventmask);
	void OnMgaGlobalEvent(globalevent_enum event);

// Event maps
	//{{AFX_EVENT(CPartBrowserCtrl)
	void FireAspectChanged(LONG index)
		{FireEvent(eventidAspectChanged, EVENT_PARAM(VTS_I4), index); }
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
		//{{AFX_DISP_ID(CPartBrowserCtrl)
		dispidCycleAspect = 5L,
		dispidChangeAspect = 4L,
		dispidRePaint = 6L,
		dispidSetBgColor = 3L,
		dispidSetCurrentProject = 1L,
		dispidSetMetaModel = 2L,
		eventidAspectChanged = 1L
		//}}AFX_DISP_ID
	};

	void SendAspectChanged(LONG index);

protected:
	CMgaObjectEventList m_MgaObjectEventList;
	void PropagateMgaMessages();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PARTBROWSERCTRL_H__6E43458B_2522_4805_9C82_4ABE0A87F7BC__INCLUDED_)
