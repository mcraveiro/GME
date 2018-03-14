#if !defined(AFX_EVENTSDIAL_H__6AEC715F_5884_4DB8_AB81_DE6C33BF3BF4__INCLUDED_)
#define AFX_EVENTSDIAL_H__6AEC715F_5884_4DB8_AB81_DE6C33BF3BF4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxcmn.h>
#include "resource.h"


/////////////////////////////////////////////////////////////////////////////
// EventsDial dialog

class EventsDial : public CDialog
{
// Construction
public:
	EventsDial(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(EventsDial)
	enum { IDD = IDD_EVENTSDLG };
	BOOL	m_chkCreated;
	BOOL	m_chkDestroyed;
	BOOL	m_chkAttr;
	BOOL	m_chkNewchild;
	BOOL	m_chkRelation;
	BOOL	m_chkProperties;
	BOOL	m_chkSubtinst;
	BOOL	m_chkParent;
	BOOL	m_chkLostchild;
	BOOL	m_chkReferenced;
	BOOL	m_chkConnected;
	BOOL	m_chkIncludeInSet;
	BOOL	m_chkDisconnect;
	BOOL	m_chkUnrefer;
	BOOL	m_chkExcludeFromSet;
	BOOL	m_chkCloseModel;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(EventsDial)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(EventsDial)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	static int m_valCreated;
	static int m_valDestroyed;
	static int m_valAttr;
	static int m_valRegistry;
	static int m_valNewchild;
	static int m_valRelation;
	static int m_valProperties;
	static int m_valSubtinst;
	static int m_valBase;
	static int m_valPosition;
	static int m_valParent;
	static int m_valLostchild;
	static int m_valReferenced;
	static int m_valConnected;
	static int m_valSetIncluded;
	static int m_valRefReleased;
	static int m_valDisconnected;
	static int m_valSetExcluded;
	static int m_valUserBits;
	static int m_valCloseModel;

	// the sum of all values
	int m_sumAll;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EVENTSDIAL_H__6AEC715F_5884_4DB8_AB81_DE6C33BF3BF4__INCLUDED_)
