#if !defined(AFX_GMEOLEASPECT_H__338DD18C_3C58_4C65_9202_2B5E00CF740A__INCLUDED_)
#define AFX_GMEOLEASPECT_H__338DD18C_3C58_4C65_9202_2B5E00CF740A__INCLUDED_

#include "GuiMeta.h"
#include "GMEView.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GMEOLEAspect.h : header file
//



/////////////////////////////////////////////////////////////////////////////
// CGMEOLEAspect command target

class CGMEOLEAspect : public CCmdTarget
{
	DECLARE_DYNCREATE(CGMEOLEAspect)

	CGMEOLEAspect();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:
	void SetGuiParams(CGMEView *view, CGuiMetaAspect *guiAspect) { m_view = view; m_guiAspect = guiAspect; }

// Overrides
public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

// Implementation
protected:
	CGuiMetaAspect*	m_guiAspect;
	CGMEView*		m_view;
	virtual ~CGMEOLEAspect();

	bool AmIValid();

	// Generated message map functions
	//{{AFX_MSG(CGMEOLEAspect)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	// OLE dispatch map functions
	afx_msg LPDISPATCH GetMgaAspect();
	afx_msg void SetMgaAspect(LPDISPATCH newValue);
	afx_msg BOOL GetActive();
	afx_msg void SetActive(BOOL bNewValue);
	afx_msg BOOL GetValid();
	afx_msg void SetValid(BOOL bNewValue);

	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

	BEGIN_DUAL_INTERFACE_PART(Dual, IGMEOLEAspect)
		STDMETHOD(get_MgaAspect)(THIS_ IMgaMetaAspect** aspect);
		STDMETHOD(put_Active)(THIS_ VARIANT_BOOL isActive);
		STDMETHOD(get_Active)(THIS_ VARIANT_BOOL* isActive);
		STDMETHOD(get_Valid)(THIS_ VARIANT_BOOL* isValid);
	END_DUAL_INTERFACE_PART(Dual)

	//     add declaration of ISupportErrorInfo implementation
	//     to indicate we support the OLE Automation error object
	DECLARE_DUAL_ERRORINFO()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GMEOLEASPECT_H__338DD18C_3C58_4C65_9202_2B5E00CF740A__INCLUDED_)
