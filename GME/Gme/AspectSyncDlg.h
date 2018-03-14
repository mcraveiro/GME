#if !defined(AFX_ASPECTSYNCDLG_H__EACECE4D_D198_4672_B945_EA41FB9024A7__INCLUDED_)
#define AFX_ASPECTSYNCDLG_H__EACECE4D_D198_4672_B945_EA41FB9024A7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AspectSyncDlg.h : header file
//

#include "gmeapp.h"
#include "gmestd.h"
#include "SyncObjsListCtrl.h"
#include "SyncAspListCtrl.h"



/////////////////////////////////////////////////////////////////////////////
// CAspectSyncDlg dialog

class CAspectSyncDlg : public CDialog
{
// Construction
public:
	CGuiMetaAspectList m_allAspects;
	CGuiObjectList m_allObjects;
	CGuiObjectList m_movingObjects;
	CGuiMetaAspect* m_srcAspect;
	CGuiMetaAspectList m_dstAspects;
	CAspectSyncDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAspectSyncDlg)
	enum { IDD = IDD_SYNCHRONIZE_DIALOG };
	CSyncAspListCtrl	m_dstAspectList;
	CSyncObjsListCtrl	m_syncObjsList;
	CComboBox	m_srcAspectCombo;
	BOOL	m_priorityForSrcVisible;
	BOOL	m_priorityForSelected;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAspectSyncDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAspectSyncDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ASPECTSYNCDLG_H__EACECE4D_D198_4672_B945_EA41FB9024A7__INCLUDED_)
