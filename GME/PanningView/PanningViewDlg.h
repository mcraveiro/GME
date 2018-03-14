#if !defined(AFX_PANNINGVIEWDLG_H__F45C3D88_53BE_4FD0_8446_372BBF753884__INCLUDED_)
#define AFX_PANNINGVIEWDLG_H__F45C3D88_53BE_4FD0_8446_372BBF753884__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PanningViewDlg.h : header file
//
#include "resource.h"
#include "PanningButton.h"

/////////////////////////////////////////////////////////////////////////////
// CPanningViewDlg dialog

class CPanningViewDlg : public CDialog
{
// Construction
public:
	CPanningViewDlg(CWnd* pParent = NULL);   // standard constructor
	void SetBitmapDC(HWND owner, HDC bdc, HBITMAP oldBmp, CRect& ori, CRect& rect, COLORREF& bkgrnd);
	void SetViewRect(CRect& vrect);
	BOOL OnInitDialog();


// Dialog Data
	//{{AFX_DATA(CPanningViewDlg)
	enum { IDD = IDD_PANNINGVIEWDLG };
	CPanningButton	m_pvbutton;
	//}}AFX_DATA
private:

public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPanningViewDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPanningViewDlg)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PANNINGVIEWDLG_H__F45C3D88_53BE_4FD0_8446_372BBF753884__INCLUDED_)
