// ChildFrm.h : interface of the CChildFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHILDFRM_H__BD235B51_BD3F_11D3_91E2_00104B98EAD9__INCLUDED_)
#define AFX_CHILDFRM_H__BD235B51_BD3F_11D3_91E2_00104B98EAD9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ModelPropertiesDlgBar.h"

class CChildFrame : public CMDIChildWndEx
{
	DECLARE_DYNCREATE(CChildFrame)
public:
	CChildFrame();

// Attributes
protected:
	CString m_title;
	CString m_appTitle;
	bool sendEvent;
	CView* view;

// Operations
public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChildFrame)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL


// Implementation
public:
	virtual ~CChildFrame();

	CString GetTitle(void) const;
	void SetTitle(const CString& title);
	CString GetAppTitle(void) const;
	void SetAppTitle(const CString& appTitle);
	void SetSendEvent(bool sendEventParam);
	void SetView(CView* viewParam);
	afx_msg void OnMDIActivate(BOOL bActivate,
		CWnd* pActivateWnd, CWnd* pDeactivateWnd);

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:
	CModelPropertiesDlgBar propBar;
// Generated message map functions
protected:
	//{{AFX_MSG(CChildFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnUpdateFrameTitle(BOOL bAddToTitle);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void RegisterTaskbarTab(CMDIChildWndEx* pWndBefore = NULL) { } // disable each tab showing up on the taskbar in windows 7

	afx_msg void PostNcDestroy() {
		delete this;
	}

	BOOL Create(LPCTSTR lpszClassName,
		LPCTSTR lpszWindowName, DWORD dwStyle,
		const RECT& rect, CMDIFrameWnd* pParentWnd,
		CCreateContext* pContext)
	{
	/* n.b. modifying style fixes a menu leak:
	Function tag: 0x49 (LoadMenuW)
	Cleanup tag: 0x6a (DestroyMenu)
	0x75a1a655      USER32!xxxLoadSysMenu+0x1a
	0x75a1a607      USER32!CreateMDIChild+0xa3
	0x75a1a2bf      USER32!_CreateWindowEx+0xfe
	0x75a08a5c      USER32!CreateWindowExW+0x33
	0x10009b12      LeakTrap_vc71!detail::MyCreateWindowExW+0x42
	0x75a1c9d0      USER32!MDIClientWndProcWorker+0x42c
	0x75a18585      USER32!MDIClientWndProcW+0x29
	0x75a062fa      USER32!InternalCallWinProc+0x23
	0x75a06d3a      USER32!UserCallWinProcCheckWow+0x109
	0x75a10d27      USER32!CallWindowProcAorW+0xab
	0x75a10d4d      USER32!CallWindowProcW+0x1b
	0x5b363b50      mfc100u!CWnd::DefWindowProcW+0x44
	0x5b364bc4      mfc100u!CWnd::WindowProc+0x3b
	0x5b362fcc      mfc100u!AfxCallWndProc+0xb5
	0x5b363258      mfc100u!AfxWndProc+0x37
	0x5b259faf      mfc100u!AfxWndProcBase+0x56
	0x75a062fa      USER32!InternalCallWinProc+0x23
	0x75a06d3a      USER32!UserCallWinProcCheckWow+0x109
	0x75a0965e      USER32!SendMessageWorker+0x581
	0x75a096c5      USER32!SendMessageW+0x7f
	0x5b3780a8      mfc100u!CMDIChildWnd::Create+0x103
	0x5b3781e4      mfc100u!CMDIChildWnd::LoadFrame+0xb9
	0x5b31a4b9      mfc100u!CDocTemplate::CreateNewFrame+0x60
	0x4e2cfc        GME!CMainFrame::CreateNewView+0x5c
	*/
		return __super::Create(lpszClassName, lpszWindowName, dwStyle & ~WS_SYSMENU, rect, pParentWnd, pContext);
	}
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHILDFRM_H__BD235B51_BD3F_11D3_91E2_00104B98EAD9__INCLUDED_)
