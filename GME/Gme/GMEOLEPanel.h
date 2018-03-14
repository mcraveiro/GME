#if !defined(AFX_GMEOLEPANEL_H__03D69231_4001_4249_B5B9_B74C7A0E2157__INCLUDED_)
#define AFX_GMEOLEPANEL_H__03D69231_4001_4249_B5B9_B74C7A0E2157__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GMEOLEPanel.h : header file
//


/////////////////////////////////////////////////////////////////////////////
// CGMEOLEPanel command target

class CGMEOLEPanel : public CCmdTarget
{
	DECLARE_DYNCREATE(CGMEOLEPanel)

	CGMEOLEPanel();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:
	void SetParams(CPane *control, CString name, IDispatch* interf = NULL) {
		m_control = control;
		m_name = name;
		m_interf = interf;
	}

// Overrides
public:
	virtual void OnFinalRelease();

// Implementation
protected:
	CPane	*m_control;
	CString	m_name;
	IDispatch* m_interf;

	virtual ~CGMEOLEPanel();

	// Generated message map functions
	//{{AFX_MSG(CGMEOLEPanel)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	// OLE dispatch map functions
	afx_msg BSTR GetName();
	afx_msg void SetName(LPCTSTR lpszNewValue);
	afx_msg BOOL GetVisible();
	afx_msg void SetVisible(BOOL bNewValue);
	afx_msg IDispatch* GetInterface();
	afx_msg void SetInterface(IDispatch* intf);
	
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

	BEGIN_DUAL_INTERFACE_PART(Dual, IGMEOLEPanel)
		STDMETHOD(get_Name)(THIS_ BSTR* name);
		STDMETHOD(put_Visible)(THIS_ VARIANT_BOOL isVisible);
		STDMETHOD(get_Visible)(THIS_ VARIANT_BOOL* isVisible);
		STDMETHOD(get_Interface)(THIS_ IDispatch** isVisible);
	END_DUAL_INTERFACE_PART(Dual)

	//     add declaration of ISupportErrorInfo implementation
	//     to indicate we support the OLE Automation error object
	DECLARE_DUAL_ERRORINFO()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GMEOLEPANEL_H__03D69231_4001_4249_B5B9_B74C7A0E2157__INCLUDED_)
