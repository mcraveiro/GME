#pragma once
#include "afxwin.h"


// CSvnLoginDlg dialog

class CSvnLoginDlg : public CDialog
{
	DECLARE_DYNAMIC(CSvnLoginDlg)

public:
	CSvnLoginDlg( int prefChoice, CWnd* pParent = NULL);   // standard constructor
	virtual ~CSvnLoginDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_SVNLOGIN };

public:
	bool wasAborted();
	void setSshOptionInitially();
	void disableSshOption();

protected:
	bool getDataFromCache( const std::string& p_inRealm, std::string& p_realm, std::string& p_uname);
	bool loadCacheFile   ( const char* p_fileNamePtr, std::string& p_realm, std::string& p_uname);

protected:
	const int m_prefChoice;
	bool m_aborted;
	bool m_sshDisabled;
	int  m_leftPad;
	int  m_rightPad;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedAbort();
	afx_msg void OnBnClickedRadioUspw();
	afx_msg void OnBnClickedRadioCached();
	afx_msg void OnBnClickedRadioPpk();
	afx_msg void OnBnClickedLoadFromCache();

	CString	m_password;
	CString	m_user;
	CString	m_database;
	CString	m_project;

	CButton m_radBtnLoginMethod;
	CButton m_btnLoadFromCache;
	CEdit m_userCtrl;
	CEdit m_passwordCtrl;
	int m_credRadioBtn;
};
