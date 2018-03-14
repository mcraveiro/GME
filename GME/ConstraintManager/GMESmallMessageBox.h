//###############################################################################################################################################
//
//	Object Constraint Language Generic Manager
//	GMESmallMessageBox.h
//
//###############################################################################################################################################

#if !defined(AFX_GMESMALLMESSAGEBOX_H__96EDD0EF_D3E0_4332_86BE_226D5E933CF6__INCLUDED_)
#define AFX_GMESMALLMESSAGEBOX_H__96EDD0EF_D3E0_4332_86BE_226D5E933CF6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//##############################################################################################################################################
//
//	C L A S S : CSmallMessageDialog <<< + CDialog
//
//==============================================================================================================================================
//
//	D E S C R I P T I O N :
//
//##############################################################################################################################################

class CSmallMessageBox : public CDialog
{

// D I A L O G   E X T E N S I O N   B Y   I M P L E M E N T O R   B E G I N
// ============================================================

	public :
		CString		m_strMessage;
		bool		m_bCloseRequested;
		bool		m_bModeless;

	public :
		bool IncrementProgress();
		void DoModeless( int iRange );
		void UndoModeless();

// D I A L O G   E X T E N S I O N   B Y   I M P L E M E N T O R   E N D
// ============================================================



// Construction
public:
	CSmallMessageBox( CWnd* pParent = NULL );   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSmallMessageBox)
	enum { IDD = IDD_PROGRESS_DIALOG };
	CProgressCtrl	m_ctrlProgress;
	CStatic	m_lblMessage;
	CButton	m_btnOK;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSmallMessageBox)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSmallMessageBox)
	afx_msg void OnClickOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GMESMALLMESSAGEBOX_H__96EDD0EF_D3E0_4332_86BE_226D5E933CF6__INCLUDED_)
