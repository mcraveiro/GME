#if !defined(AFX_DIALOGLIST_H__5D30C6D3_4070_11D4_9A91_00902721DB65__INCLUDED_)
#define AFX_DIALOGLIST_H__5D30C6D3_4070_11D4_9A91_00902721DB65__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogList.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogList dialog

class CDialogList : public CDialog
{
// Construction
public:
	typedef enum { CHKTEXT_NONE, CHKTEXT_ONETIMER, CHKTEXT_STICKY } dialogmode;
	CDialogList ( CString       _caption
                , dialogmode    _mode=CHKTEXT_NONE
                , bool          hasIgnore = false
                , CWnd*         pParent = NULL
                );

// Dialog Data
	//{{AFX_DATA(CDialogList)
	enum { IDD = IDD_DIALOG_LIST };
	CButton	m_btnIgnore;
	CButton	mcb_check_once;
	CListBox	m_list;
	CString	m_sz_listselection;
	BOOL	mb_check_once;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogList)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogList)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnButtonIgnore();
	afx_msg void OnDblclkList();
	afx_msg void OnChangeList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


public:
	CStringList m_sz_prelist;
	int mn_selection_index;
	CString m_caption;
	CString m_checkboxtext;

private:
	dialogmode cb_mode;
    bool       m_hasIgnore;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGLIST_H__5D30C6D3_4070_11D4_9A91_00902721DB65__INCLUDED_)
