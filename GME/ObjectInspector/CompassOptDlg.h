#if !defined(AFX_COMPASSOPTDLG_H__1075F96D_6C41_4353_B962_5812365FC2EC__INCLUDED_)
#define AFX_COMPASSOPTDLG_H__1075F96D_6C41_4353_B962_5812365FC2EC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CompassOptDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCompassOptDlg dialog

class CCompassOptDlg : public CDialog
{
	bool m_bInited;
	bool m_bClosed;
	
// Construction
public:
	CCompassOptDlg(CWnd* pParent = NULL);   // standard constructor

// Attributes
protected :
	CPoint	m_ptRightTop;
	UINT	m_uCompassVal;

public :
	void SetParameters(const CPoint& ptRightTop, UINT uCompassVal);
	UINT GetCompassVal(void) const;

// Dialog Data
	//{{AFX_DATA(CCompassOptDlg)
	enum { IDD = IDD_COMPASS_OPTION };
	int		m_nRadio;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCompassOptDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CBitmap m_background;

	// Generated message map functions
	//{{AFX_MSG(CCompassOptDlg)
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnNcActivate(BOOL bActive);
	afx_msg void OnRadioCenter();
	afx_msg void OnDoubleclickedRadioCenter();
	afx_msg void OnRadioEast();
	afx_msg void OnDoubleclickedRadioEast();
	afx_msg void OnRadioNorth();
	afx_msg void OnDoubleclickedRadioNorth();
	afx_msg void OnRadioNortheast();
	afx_msg void OnDoubleclickedRadioNortheast();
	afx_msg void OnRadioNorthwest();
	afx_msg void OnDoubleclickedRadioNorthwest();
	afx_msg void OnRadioSouth();
	afx_msg void OnDoubleclickedRadioSouth();
	afx_msg void OnRadioSoutheast();
	afx_msg void OnDoubleclickedRadioSoutheast();
	afx_msg void OnRadioSouthwest();
	afx_msg void OnDoubleclickedRadioSouthwest();
	afx_msg void OnRadioWest();
	afx_msg void OnDoubleclickedRadioWest();
	afx_msg void OnPaint();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMPASSOPTDLG_H__1075F96D_6C41_4353_B962_5812365FC2EC__INCLUDED_)
