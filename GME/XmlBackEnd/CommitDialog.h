#pragma once
#include "afxwin.h"


// CCommitDialog dialog

class CCommitDialog : public CDialog
{
	DECLARE_DYNAMIC(CCommitDialog)

public:
	CCommitDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCommitDialog();

// Dialog Data
	enum { IDD = IDD_DIALOG_COMMIT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()
public:
	
	CEdit m_commentEdit;
	CString m_comment;
};
