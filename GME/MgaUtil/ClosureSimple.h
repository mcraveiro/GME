#pragma once
#include "afxwin.h"
#include "resource.h"

// CClosureSimple dialog

class CClosureSimple : public CDialog
{
	DECLARE_DYNAMIC(CClosureSimple)

public:
	CClosureSimple(CWnd* pParent = NULL);   // standard constructor
	virtual ~CClosureSimple();

// Dialog Data
	enum { IDD = IDD_CLOSURESIMPLECP };
	bool m_enableContainChkBox;
	bool m_enableFolderChkBox;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	CButton m_cont;
	CButton m_foldCont;
	BOOL m_bCont;
	BOOL m_bFoldCont;
	
	// exclude connections?
	BOOL m_bConn;
	// exclude references?
	BOOL m_bRef;
	// exclude atoms?
	BOOL m_bAtom;
	// exclude sets?
	BOOL m_bSet;
};
