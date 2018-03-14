#if !defined(AFX_OPTIONS_H__D33EAC1F_01D6_4270_93F8_069D3BAFE231__INCLUDED_)
#define AFX_OPTIONS_H__D33EAC1F_01D6_4270_93F8_069D3BAFE231__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"
#include "BON.h"
#include <afxcmn.h>
/////////////////////////////////////////////////////////////////////////////
// Options dialog

class Options : public CDialog
{
// Construction
public:
	Options(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(Options)
	enum { IDD = IDD_OPTIONS };
	CSpinButtonCtrl	m_spin;
	CString	m_strRegCont;
	CString	m_strFolCont;
	CString	m_strConnect;
	BOOL	m_dontask;
	CString	m_strPriority;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Options)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	void PASCAL DDV_HexNumber(CDataExchange* pDX, const CString& nmb);

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(Options)
	virtual BOOL OnInitDialog();
	afx_msg void OnButton1();
	afx_msg void OnButton2();
	afx_msg void OnButton3();
	afx_msg void OnDontAskClick();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void setDefs( int regCont, int folCont, int conn, int prior);
	void getResults( int* v1, int* v2, int* v3, int* vp, bool* ask);

	static bool fetchOpts( const BON::Project& proj, int* v1, int* v2, int* v3, int* vp);
	static void saveOpts( const BON::Project& proj, int v1, int v2, int v3, int vp, bool dont_ask);

	static const std::string OptionsRegistrySubNode_str;
	static const std::string RegContMask_str;
	static const std::string FolContMask_str;
	static const std::string ConnectMask_str;
	static const std::string Priority_str;
	static const std::string DontAsk_str;

protected:
	int m_valRegContMask;// redundancy...
	int m_valFolContMask;
	int m_valConnectMask;
	int m_valPriority;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONS_H__D33EAC1F_01D6_4270_93F8_069D3BAFE231__INCLUDED_)
