// GMEObjectInspector.h: interface for the CGMEObjectInspector class.
//
//////////////////////////////////////////////////////////////////////
//{{AFX_INCLUDES()
#include "objectinspector.h"
//}}AFX_INCLUDES

#if !defined(AFX_GMEOBJECTINSPECTOR_H__25FF15A3_1A69_43B6_83DC_1A3F3E327E9B__INCLUDED_)
#define AFX_GMEOBJECTINSPECTOR_H__25FF15A3_1A69_43B6_83DC_1A3F3E327E9B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CGMEObjectInspector : public CDockablePane  
{
public:

	CGMEObjectInspector();
	virtual ~CGMEObjectInspector(){theInstance = 0;};

	IDispatch * GetInterface() {
		LPUNKNOWN pUnk = m_ObjectInspector.GetControlUnknown();

		// From there get the IDispatch interface of control.
		LPDISPATCH pDisp = NULL;
		pUnk->QueryInterface(IID_IDispatch, (LPVOID*)&pDisp);
		return pDisp;
	}

	// Dialog Data
	//{{AFX_DATA(CGMEObjectInspector)
	enum { IDD = IDD_OBJECT_INSPECTOR_DIALOG };
	CObjectInspector	m_ObjectInspector;
	//}}AFX_DATA

	static CGMEObjectInspector *theInstance;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGMEObjectInspector)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL



	// Generated message map functions
	//{{AFX_MSG(CGMEObjectInspector)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnRootFolderNameChangedGmeObjectInspectorCtrl();
	afx_msg void OnConvertPathToCustomGmeObjectInspectorCtrl(LPUNKNOWN pMgaObject);
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void SetObjects(LPUNKNOWN pUnknown);
	LPUNKNOWN GetObjects();
	void SetProject(CComPtr<IMgaProject>& mgaProject);
	void CloseProject();
	void ShowPanel(long panelID);
	void CyclePanel(VARIANT_BOOL frwd);

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GMEOBJECTINSPECTOR_H__25FF15A3_1A69_43B6_83DC_1A3F3E327E9B__INCLUDED_)
