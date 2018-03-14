#pragma once
#include "afxwin.h"


// RemoveLocalAttrDlg dialog

class RemoveLocalAttrDlg : public CDialog
{
	DECLARE_DYNAMIC(RemoveLocalAttrDlg)

public:
	RemoveLocalAttrDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~RemoveLocalAttrDlg();

// Dialog Data
	enum { IDD = IDD_DLG_PROPREMOVELOCALATTR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	void init( const CString& par1, const CString& par2);
	const CString& getPar1() const;
	const CString& getPar2() const;

protected:
	CString m_attr;
	CString m_owner;
};
