#pragma once
#include "resource.h"
#include "afxwin.h"

// CCredentialDlg dialog

class CCredentialDlg : public CDialog
{
	DECLARE_DYNAMIC(CCredentialDlg)

public:
	CCredentialDlg( bool p_userNameOnly, bool p_visibleMaySave, bool p_maySave, const std::string& p_uName, const char* p_realmStr, CWnd* pParent = NULL);   // standard constructor
	virtual ~CCredentialDlg();

	std::string name();
	std::string word();
	bool        maySave();

// Dialog Data
	enum { IDD = IDD_DIALOG1 };

protected:
	bool        m_userNameOnly;
	bool        m_visibleMaySave;
	bool        m_maySave;
	std::string m_suggestedName;
	const char* m_realmStrPtr;

	int         m_leftPad;
	int         m_rightPad;

	bool        m_resMaySave;
	std::string m_resName;
	std::string m_resWord;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnSize(UINT nType, int cx, int cy);

	CButton m_btnMaySave;
	CEdit   m_edtName;
	CEdit   m_edtWord;
	CEdit   m_edtRealm;
	CStatic m_msgAtTheTop;
};
