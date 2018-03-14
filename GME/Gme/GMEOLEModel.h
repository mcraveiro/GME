#if !defined(AFX_GMEOLEMODEL_H__AFDFA91A_CACB_40E6_A254_6C835A980383__INCLUDED_)
#define AFX_GMEOLEMODEL_H__AFDFA91A_CACB_40E6_A254_6C835A980383__INCLUDED_

#include "GMEView.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GMEOLEModel.h : header file
//



/////////////////////////////////////////////////////////////////////////////
// CGMEOLEModel command target

class CGMEOLEModel : public CCmdTarget
{
	DECLARE_DYNCREATE(CGMEOLEModel)

public:
	CGMEOLEModel();           // constructor used by dynamic creation
	

// Attributes
public:

// Operations
public:
	void SetView(CGMEView *view) { m_view = view; }

// Overrides
public:
	virtual void OnFinalRelease();

// Implementation
protected:
	CGMEView* m_view;
	virtual ~CGMEOLEModel();

	bool AmIValid();

	// Generated message map functions
	//{{AFX_MSG(CGMEOLEModel)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	// OLE dispatch map functions
	afx_msg BOOL GetActive();
	afx_msg void SetActive(BOOL bNewValue);
	afx_msg LPDISPATCH GetAspects();
	afx_msg void SetAspects(LPDISPATCH newValue);
	afx_msg LPDISPATCH GetMgaModel();
	afx_msg void SetMgaModel(LPDISPATCH newValue);
	afx_msg BOOL GetValid();
	afx_msg void SetValid(BOOL bNewValue);
	afx_msg void Print();
	afx_msg void PrintDialog();
	afx_msg void DumpWindowsMetaFile(LPCTSTR filePath);
	afx_msg void CheckConstraints();
	afx_msg void RunComponent(LPCTSTR appID);
	afx_msg void RunComponentDialog();
	afx_msg void Close();
	afx_msg void GrayOutFCO(BOOL bGray, BOOL bNeighbours, LPDISPATCH mgaFCOs);
	afx_msg void GrayOutHide();
	afx_msg void ShowSetMembers(LPDISPATCH mgaFCO);
	afx_msg void HideSetMembers();
	afx_msg void Zoom(long percent);
	afx_msg void ZoomTo(LPDISPATCH mgaFCOs);
	afx_msg void Scroll(long bar, long scroll);
	afx_msg HRESULT DumpModelGeometryXML(LPCTSTR filePath);
	afx_msg CString GetCurrentAspect(void);
	afx_msg void SetCurrentAspect(const CString& aspectName);

	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

	BEGIN_DUAL_INTERFACE_PART(Dual, IGMEOLEModel)
		STDMETHOD(put_Active)(THIS_ VARIANT_BOOL isActive);
		STDMETHOD(get_Active)(THIS_ VARIANT_BOOL* isActive);
		STDMETHOD(get_Aspects)(THIS_ IGMEOLEColl** coll);
		STDMETHOD(get_MgaModel)(THIS_ IMgaModel** model);
		STDMETHOD(get_Valid)(THIS_ VARIANT_BOOL* isValid);
		
		STDMETHOD(Print)(THIS);
		STDMETHOD(PrintDialog)(THIS);
		STDMETHOD(DumpWindowsMetaFile)(THIS_ BSTR filePath);
		STDMETHOD(CheckConstraints)(THIS);
		STDMETHOD(RunComponent)(THIS_ BSTR appID);
		STDMETHOD(RunComponentDialog)(THIS);
		STDMETHOD(Close)(THIS);
		STDMETHOD(GrayOutFCO)(THIS_ VARIANT_BOOL bGray, VARIANT_BOOL bNeighbours, IMgaFCOs* mgaFCO);
		STDMETHOD(GrayOutHide)(THIS);
		STDMETHOD(ShowSetMembers)(THIS_ IMgaFCO* mgaFCO);
		STDMETHOD(HideSetMembers)(THIS);
		STDMETHOD(Zoom)(THIS_ DWORD percent);
		STDMETHOD(ZoomTo)(THIS_ IMgaFCOs* mgaFCOs);
		STDMETHOD(Scroll)(THIS_ bar_enum bar, scroll_enum scroll);
		STDMETHOD(DumpModelGeometryXML)(THIS_ BSTR filePath);
		STDMETHOD(GetCurrentAspect)(THIS_ BSTR* aspectName);
		STDMETHOD(SetCurrentAspect)(THIS_ BSTR aspectName);
	END_DUAL_INTERFACE_PART(Dual)

	
	//     add declaration of ISupportErrorInfo implementation
	//     to indicate we support the OLE Automation error object
	DECLARE_DUAL_ERRORINFO()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GMEOLEMODEL_H__AFDFA91A_CACB_40E6_A254_6C835A980383__INCLUDED_)
