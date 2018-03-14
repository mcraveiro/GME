#if !defined(AFX_COMPASSCHECKDLG_H__ED0FEEC5_330B_4633_8736_4F6639CBE469__INCLUDED_)
#define AFX_COMPASSCHECKDLG_H__ED0FEEC5_330B_4633_8736_4F6639CBE469__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CompassCheckDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCompassCheckDlg dialog

class CCompassCheckDlg : public CDialog
{
	bool m_bInited;
	bool m_bClosed;

// Construction
public:
	CCompassCheckDlg(CWnd* pParent = NULL);   // standard constructor

// Attributes
protected :
	CPoint m_ptRightTop;
	UINT m_uCompassVal;

public :
	void SetParameters(const CPoint& ptRightTop, UINT uCompassVal);
	UINT GetCompassVal(void) const;

// Dialog Data
	//{{AFX_DATA(CCompassCheckDlg)
	enum { IDD = IDD_DIALOG_COMPASS };
	BOOL	m_bEast;
	BOOL	m_bNorth;
	BOOL	m_bSouth;
	BOOL	m_bWest;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCompassCheckDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

    CBitmap m_background;

	// Generated message map functions
	//{{AFX_MSG(CCompassCheckDlg)
	afx_msg BOOL OnNcActivate(BOOL bActive);
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMPASSCHECKDLG_H__ED0FEEC5_330B_4633_8736_4F6639CBE469__INCLUDED_)
