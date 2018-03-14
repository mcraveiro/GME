#if !defined(AFX_OBJECTINSPECTORCTL_H__746A87ED_6895_4A5D_84B2_A822210554BD__INCLUDED_)
#define AFX_OBJECTINSPECTORCTL_H__746A87ED_6895_4A5D_84B2_A822210554BD__INCLUDED_


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "InspectorDefs.h"
#include "InspectorDlg.h"

#include "..\GmeActiveBrowser\MgaObjectEventList.h"
// ObjectInspectorCtl.h : Declaration of the CObjectInspectorCtrl ActiveX Control class.

/////////////////////////////////////////////////////////////////////////////
// CObjectInspectorCtrl : See ObjectInspectorCtl.cpp for implementation.

typedef CComPtr<IMgaFCO> MgaFCOPtr;
#define ADAPT_FCO(x) CAdapt<MgaFCOPtr>(x)
typedef CList< CAdapt< MgaFCOPtr >, CAdapt< MgaFCOPtr > & > CMgaFCOPtrList;

typedef CComPtr<IMgaFolder>MgaFolderPtr;
#define ADAPT_FOLDER(x) CAdapt<MgaFolderPtr>(x)
typedef CList< CAdapt< MgaFolderPtr >, CAdapt< MgaFolderPtr > & > CMgaFolderPtrList;



#include "Attribute.h"	// Added by ClassView
#include "Preference.h"	// Added by ClassView
#include "Property.h"


class CObjectInspectorCtrl : public COleControl
{
	DECLARE_DYNCREATE(CObjectInspectorCtrl)

// Constructor
public:
	CObjectInspectorCtrl();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CObjectInspectorCtrl)
	public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	~CObjectInspectorCtrl();

	DECLARE_OLECREATE_EX(CObjectInspectorCtrl)    // Class factory and guid
	DECLARE_OLETYPELIB(CObjectInspectorCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CObjectInspectorCtrl)     // Property page IDs
	DECLARE_OLECTLTYPE(CObjectInspectorCtrl)		// Type name and misc status

	CInspectorDlg	m_inspectorDlg;

	CComPtr<IMgaTerritory>	m_territory;
	CComPtr<IMgaProject>	m_project;
	CComPtr<IMgaObjects>	m_objects;

	CMgaFCOPtrList m_FCOList;
	CMgaFolderPtrList m_FolderList;

	bool ignoreNextEvents;

// Message maps
	//{{AFX_MSG(CObjectInspectorCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dispatch maps
	//{{AFX_DISPATCH(CObjectInspectorCtrl)
	afx_msg LPUNKNOWN GetMgaProject();
	afx_msg void SetMgaProject(LPUNKNOWN newValue);
	afx_msg LPUNKNOWN GetMgaObjects();
	afx_msg void SetMgaObjects(LPUNKNOWN newValue);
	afx_msg void ShowPanel(long panelID);
	afx_msg void EditName();
	afx_msg void CyclePanel(VARIANT_BOOL frwd);
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()

	afx_msg void AboutBox();

	void OnMgaObjectEvent(IMgaObject * obj, unsigned long eventmask);
	void OnMgaGlobalEvent(globalevent_enum event);
	void RemoveZombies();

public:
// Event maps
	//{{AFX_EVENT(CObjectInspectorCtrl)
	void FireRootFolderNameChanged()
		{FireEvent(eventidRootFolderNameChanged, EVENT_PARAM(VTS_NONE)); }
	void FireConvertPathToCustom(LPUNKNOWN pMgaObject)
		{FireEvent(eventidConvertPathToCustom, EVENT_PARAM(VTS_UNKNOWN), pMgaObject);}
	//}}AFX_EVENT
	DECLARE_EVENT_MAP()

// Native COM interfaces - peter
	DECLARE_INTERFACE_MAP()

	BEGIN_INTERFACE_PART(EventSink, IMgaEventSink)
		STDMETHOD(GlobalEvent(globalevent_enum event));
		STDMETHOD(ObjectEvent(IMgaObject * obj, unsigned long eventmask, VARIANT v));
	END_INTERFACE_PART(EventSink)

private:
	template<typename F>
	void WriteToMga(F f);

// Dispatch and event IDs
public:
	void WriteNameToMga( const CString& strName);
	void UndoRedo( bool undo = true);

	void DetachFromArchetype(CListItem ListItem);
	void WriteAttributeItemToMga(CListItem ListItem);
	void WritePreferenceItemToMga(CListItem ListItem,bool bIsForKind);
	void RefreshReferencePanel();
	void RefreshAttributePanel();
	void RefreshPropertyPanel();
	void OpenRefered();

	CAttribute m_Attribute;
	CPreference m_Preference;
	CProperty m_Property;

	void RefreshPanels();
	bool IsInitialized(bool withObject = false) const;
	void IgnoreNextEvents(void);

	enum {
	//{{AFX_DISP_ID(CObjectInspectorCtrl)
	dispidMgaProject = 1L,
	dispidMgaObject = 2L,
	dispidShowPanel = 3L,
	dispidEditName = 4L,
	dispidCyclePanel = 5L,
	eventidRootFolderNameChanged = 1L,
	eventidConvertPathToCustom = 2L
	//}}AFX_DISP_ID
	};
private:
	void RefreshName();
	CMgaObjectEventList m_MgaObjectEventList;
	void PropagateMgaMessages();

	static CComPtr<IGMEOLEApp> get_GME(IMgaProjectPtr& p_mgaproject)
	{
		CComPtr<IGMEOLEApp> gme;
		if (p_mgaproject) {
			CComBSTR bstrName("GME.Application");
			CComPtr<IMgaClient> pClient;
			HRESULT hr = p_mgaproject->GetClientByName(bstrName, &pClient);
			if (SUCCEEDED(hr) && pClient) {
				CComPtr<IDispatch> pDispatch;
				hr = pClient->get_OLEServer(&pDispatch);
				if (SUCCEEDED(hr) && pDispatch) {
					hr = pDispatch.QueryInterface(&gme);
					if (FAILED(hr)) {
						gme = NULL;
					}
				}
			}
		}
		return gme;
	}};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OBJECTINSPECTORCTL_H__746A87ED_6895_4A5D_84B2_A822210554BD__INCLUDED)
