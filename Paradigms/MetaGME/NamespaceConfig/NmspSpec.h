#pragma once
#include "afxwin.h"
#include "resource.h"

#include "MetaGMEVisitor.h"
#include "afxcmn.h"

// NmspSpec dialog

class NmspSpec : public CDialog
{
	DECLARE_DYNAMIC(NmspSpec)

public:
	NmspSpec(CWnd* pParent = NULL);   // standard constructor
	virtual ~NmspSpec();

// Dialog Data
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

protected:
	void initElems();
	void insert( int pIdx, MetaBON::MetaGMEVisitor::VectorOfPairs::const_iterator it);

	void updateEditField( int pSel = -1);
	void updateItemInfo( int pIdx, CString pTxt);
	bool loadItemInfo( int pIdx, CString& pTxt);

	MetaBON::MetaGMEVisitor::VectorOfPairs      m_data;

public:
	void setData( MetaBON::MetaGMEVisitor::VectorOfPairs& pData);
	void getData( MetaBON::MetaGMEVisitor::VectorOfPairs& rData);

public:
	CEdit m_edName;
	CButton m_bEnabled;
	CButton m_bSave;
	CTreeCtrl m_tree;
	HTREEITEM m_rootElem;
	bool    m_edChanged;

	virtual BOOL OnInitDialog();

	afx_msg void OnEnKillfocusEdit1();
	afx_msg void OnEnChangeEdit1();

	afx_msg void OnNMClickTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMSetfocusTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedSave();
	afx_msg void OnEnSetfocusEdit1();
};
