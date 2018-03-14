#pragma once


#include "PanningViewDlg.h"


// PanningViewCtrl.h : Declaration of the CPanningViewCtrl ActiveX Control class.


// CPanningViewCtrl : See PanningViewCtrl.cpp for implementation.

class CPanningViewCtrl : public COleControl
{
	DECLARE_DYNCREATE(CPanningViewCtrl)

// Constructor
public:
	CPanningViewCtrl();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPanningViewCtrl)
	public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	~CPanningViewCtrl();

	DECLARE_OLECREATE_EX(CPanningViewCtrl)    // Class factory and guid
	DECLARE_OLETYPELIB(CPanningViewCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CPanningViewCtrl)     // Property page IDs
	DECLARE_OLECTLTYPE(CPanningViewCtrl)		// Type name and misc status

	CPanningViewDlg	m_panningViewDlg;

// Message maps
	//{{AFX_MSG(CPanningViewCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dispatch maps
	//{{AFX_DISPATCH(CPanningViewCtrl)
	afx_msg void SetBitmapDC(ULONGLONG ownerWnd, ULONGLONG bDC, ULONGLONG oldBmp,
							 LONG orix, LONG oriy, LONG oriw, LONG orih,
							 LONG rx, LONG ry, LONG rw, LONG rh,
							 OLE_COLOR bkgrnd);
	afx_msg void SetViewRect(LONG vrx, LONG vry, LONG vrw, LONG vrh);
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()

	afx_msg void AboutBox();

// Event maps
	//{{AFX_EVENT(CPanningViewCtrl)
	//}}AFX_EVENT
	DECLARE_EVENT_MAP()

// Dispatch and event IDs
public:
	enum {
		//{{AFX_DISP_ID(CPanningViewCtrl)
		dispidSetBitmapDC = 1L,
		dispidSetViewRect = 2L
		//}}AFX_DISP_ID
	};
};

