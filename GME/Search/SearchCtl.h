#if !defined(AFX_SEARCHCTL_H__E14E5717_51B5_4F31_87EF_1512A4E4D109__INCLUDED_)
#define AFX_SEARCHCTL_H__E14E5717_51B5_4F31_87EF_1512A4E4D109__INCLUDED_


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SearchDefs.h"
#include "SearchDlg.h"


// SearchCtl.h : Declaration of the CSearchCtrl ActiveX Control class.

/////////////////////////////////////////////////////////////////////////////
// CSearchCtrl : See SearchCtl.cpp for implementation.

class CSearchCtrl : public COleControl
{
	DECLARE_DYNCREATE(CSearchCtrl)

// Constructor
public:
	CSearchCtrl();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSearchCtrl)
	public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// These functions can be used from the dialog
public: //there is a "protected:" in the AFX section above..
	CComPtr<IMgaProject> GetProject() { return m_project; }
	CComPtr<IMgaObjects> GetScopedL() { return m_MgaObjs; }
	void BeginTransaction(transactiontype_enum=TRANSACTION_READ_ONLY);
	void CommitTransaction();
	void AbortTransaction();
	CComPtr<IMgaTerritory> Terr();
	CComPtr<IMgaObjects> PutInMyTerritory(CComPtr<IMgaObjects> &in_coll);
	void ClickOnObject(CComPtr<IMgaObject> object);
	void ClickOnObject(CComPtr<IMgaObjects> object);
	void DblClickOnObject(CComPtr<IMgaObject> object);
	void WantToBeClosed();
	void LocateMgaObject(CComPtr<IMgaObject> object);
    virtual BOOL PreTranslateMessage(MSG *pMsg);
	

// Implementation
protected:
	~CSearchCtrl();

	DECLARE_OLECREATE_EX(CSearchCtrl)    // Class factory and guid
	DECLARE_OLETYPELIB(CSearchCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CSearchCtrl)     // Property page IDs
	DECLARE_OLECTLTYPE(CSearchCtrl)		// Type name and misc status

	CSearchDlg	m_searchDlg;
	CComPtr<IMgaTerritory>	m_territory;
	CComPtr<IMgaProject>	m_project;
	CComPtr<IMgaObjects>    m_MgaObjs;

// Message maps
	//{{AFX_MSG(CSearchCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dispatch maps
	//{{AFX_DISPATCH(CSearchCtrl)
	afx_msg LPUNKNOWN GetMgaProject();
	afx_msg void SetMgaProject(LPUNKNOWN newValue);
	afx_msg void OpenProject(LPCTSTR connStr);
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()

	afx_msg void AboutBox();

// MGA Event handlers
	void OnMgaObjectEvent(IMgaObject * obj, unsigned long eventmask);
	void OnMgaGlobalEvent(globalevent_enum event);

// Event maps
	//{{AFX_EVENT(CSearchCtrl)
	void FireClickMgaObject(LPUNKNOWN mgaObject)
		{FireEvent(eventidClickMgaObject,EVENT_PARAM(VTS_UNKNOWN), mgaObject);}
	void FireDblClickMgaObject(LPUNKNOWN mgaObject)
		{FireEvent(eventidDblClickMgaObject,EVENT_PARAM(VTS_UNKNOWN), mgaObject);}
	void FireWantToBeClosed()
		{FireEvent(eventidWantToBeClosed,EVENT_PARAM(VTS_NONE));}
    void FireLocateMgaObject(LPCTSTR mgaObjectId)
		{FireEvent(eventidLocateMgaObject, EVENT_PARAM(VTS_BSTR), mgaObjectId);}


	//}}AFX_EVENT
	DECLARE_EVENT_MAP()

// Native COM interfaces - peter
	DECLARE_INTERFACE_MAP()

	BEGIN_INTERFACE_PART(EventSink, IMgaEventSink)
		STDMETHOD(GlobalEvent(globalevent_enum event));
		STDMETHOD(ObjectEvent(IMgaObject * obj, unsigned long eventmask, VARIANT v));
	END_INTERFACE_PART(EventSink)

// Dispatch and event IDs
public:
	enum {	
	//{{AFX_DISP_ID(CSearchCtrl)
	dispidMgaProject = 1L,
	dispidOpenProject = 2L,
	dispidSelMgaObjects = 3L,
	eventidClickMgaObject = 1L,
	eventidDblClickMgaObject = 2L,
	eventidWantToBeClosed = 3L,
	eventidLocateMgaObject = 4L,
	//}}AFX_DISP_ID
	};

private:

	int	m_transactionCnt;
	bool m_inEventTransactionMode;
protected:
	void SelMgaObjects(IUnknown* p_selMgaObjs);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SEARCHCTL_H__E14E5717_51B5_4F31_87EF_1512A4E4D109__INCLUDED)
