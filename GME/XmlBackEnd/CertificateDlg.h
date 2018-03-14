#pragma once
#include "resource.h"
#include "afxwin.h"

// CCertificateDlg dialog

class CCertificateDlg : public CDialog
{
	DECLARE_DYNAMIC(CCertificateDlg)

public:
	CCertificateDlg( const std::string& p_text, bool p_permAcceptEnabled, CWnd* pParent = NULL);   // standard constructor
	virtual ~CCertificateDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG2 };
	typedef enum Response
	{
		PermanentAccept
		, TemoraryAccept
		, Reject
	};


	Response getResp();

protected:
	int         m_leftPad;
	int         m_rightPad;

	Response    m_response;
	std::string m_certDetails;
	bool        m_permAcceptEnabled;

	CEdit       m_textBox;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
