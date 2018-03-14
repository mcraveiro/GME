//{{AFX_INCLUDES()
#include "console.h"
//}}AFX_INCLUDES

#pragma once
// GMEConsole.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGMEConsole dialog

class CGMEConsole : public CDockablePane
{
// Construction
public:
	CGMEConsole(CWnd* pParent = NULL);   // standard constructor
	virtual ~CGMEConsole(){theInstance = 0;};

	void Clear();
	void Message(CString str, short type);
	CString GetContents();
	void SetContents(const CString& contents);
	void SetGMEApp(IDispatch *idp);
	void SetGMEProj(IDispatch *idp);
	void NavigateTo(CString url);

// Dialog Data
	//{{AFX_DATA(CGMEConsole)
	enum { IDD = IDD_CONSOLE_DIALOG };
	CConsole	m_Console;
	//}}AFX_DATA

	static CGMEConsole *theInstance;
	
	IDispatch * GetInterface()
	{
		LPUNKNOWN pUnk = m_Console.GetControlUnknown();

		// From there get the IDispatch interface of control.
		LPDISPATCH pDisp = NULL;
		pUnk->QueryInterface(IID_IDispatch, (LPVOID*)&pDisp);
		return pDisp;
	};

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGMEConsole)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGMEConsole)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClickMGAIDConsoleCtrl(LPCTSTR objid);
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

virtual BOOL PreTranslateMessage(MSG* pMsg)
{
	__int64 ret = 0;
	m_Console.InvokeHelper(0x43576E64 /* magic from ConsoleCtl.cpp */, DISPATCH_PROPERTYGET, VT_I8, (void*)&ret, 0);
	CWnd* cwnd = (CWnd*)(void*)ret;
	return cwnd->PreTranslateMessage(pMsg);
}

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

