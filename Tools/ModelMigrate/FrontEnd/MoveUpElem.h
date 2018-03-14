#pragma once
#include "afxcmn.h"
#include <vector>
#include <string>

// MoveUpElem dialog

class MoveUpElem : public CDialog
{
	DECLARE_DYNAMIC(MoveUpElem)

public:
	MoveUpElem(CWnd* pParent = NULL);   // standard constructor
	virtual ~MoveUpElem();

// Dialog Data
	enum { IDD = IDD_DLG_PROPMOVEUP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	virtual void OnCancel();
	DECLARE_MESSAGE_MAP()

public:
	void init( const std::vector< std::string>& pars);
	CTreeCtrl m_tree;
	std::vector< std::string> getSequence();

protected:
	HTREEITEM lastChild();
	void addChild( const char * pLabel);
	void remChild();
	std::vector< std::string> getValues();
	std::vector< std::string> m_storage;
	void closeDlg( int pResult);

public:
	afx_msg void OnTvnEndlabeleditTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMReturnTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonInsertnode();
	afx_msg void OnBnClickedButtonDeletenode();
	virtual BOOL OnInitDialog();
};
