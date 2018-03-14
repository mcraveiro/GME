#if !defined(AFX_REGISTRYBROWSERDLG_H__96478D7F_0A33_4570_B83E_BF205F5DA649__INCLUDED_)
#define AFX_REGISTRYBROWSERDLG_H__96478D7F_0A33_4570_B83E_BF205F5DA649__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SplitterBar.h"
#include "RegistryTree.h"
#include "RegBrwNode.h"
#include "RegistryPanel.h"
// RegistryBrowserDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRegistryBrowserDlg dialog

class CRegistryBrowserDlg : public CDialog
{
// Construction
public:
	void UpdateRegistry(void);
	void AddNodes(CRegBrwNode *parent, CComPtr<IMgaRegNodes> &regNodes);
	void FillNodes(void);
	CRegistryBrowserDlg(const CComPtr<IMgaObject> &object, CWnd* pParent = NULL);   // standard constructor
	~CRegistryBrowserDlg();

// Dialog Data
	//{{AFX_DATA(CRegistryBrowserDlg)
	enum { IDD = IDD_REGISTRY_BROWSER };
	CRegistryPanel	m_wndRegNode;
	CRegistryTree	m_wndRegistryTree;
	CString	m_objectKind;
	CString	m_objectName;
	CString	m_objectRole;
	CString	m_regnodePath;
	CString	m_regnodeStatus;
	CString	m_regnodeValue;
	//}}AFX_DATA
	CSplitterBar m_wndSplitterBar;
	CTypedPtrList<CPtrList, CRegBrwNode*>	m_nodes;
	CComPtr<IMgaObject> m_object;
	CMap<int, int, int, int> m_imageMap;



// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRegistryBrowserDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRegistryBrowserDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnChangeRegnodeValue();
	virtual void OnOK();
	//}}AFX_MSG

	CImageList	m_imageList;

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REGISTRYBROWSERDLG_H__96478D7F_0A33_4570_B83E_BF205F5DA649__INCLUDED_)
