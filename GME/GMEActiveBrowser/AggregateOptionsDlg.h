#if !defined(AFX_AGGREGATEOPTIONSDLG_H__DF07E3D2_2E00_4356_81C6_48902597DFDE__INCLUDED_)
#define AFX_AGGREGATEOPTIONSDLG_H__DF07E3D2_2E00_4356_81C6_48902597DFDE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AggregateOptionsDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAggregateOptionsDlg dialog

class CAggregateOptionsDlg : public CDialog
{
// Construction
public:
	BOOL m_bIsResortNeeded;
	BOOL m_bIsRefreshNeeded;
	CAggregateOptionsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAggregateOptionsDlg)
	enum { IDD = IDD_OPTIONS_DIALOG };
	BOOL	m_bDisplayAtoms;
	BOOL	m_bDisplayConnections;
	BOOL	m_bDisplayModels;
	BOOL	m_bDisplayReferences;
	BOOL	m_bDisplaySets;
	int		m_SortOptions;
	BOOL	m_bStoreTreeInRegistry;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAggregateOptionsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAggregateOptionsDlg)
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AGGREGATEOPTIONSDLG_H__DF07E3D2_2E00_4356_81C6_48902597DFDE__INCLUDED_)
