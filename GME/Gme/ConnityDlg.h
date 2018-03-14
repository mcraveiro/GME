#pragma once
//#include "afxwin.h"
#include "GMEStd.h"
#include "GuiObject.h"
#include "afxwin.h"


// CConnityDlg dialog, as in Connectivity

class CConnityDlg : public CDialog
{
	DECLARE_DYNAMIC(CConnityDlg)

	CGuiConnectionList* m_listC;

	CGuiConnection*     m_selectedC;

	bool                m_reverse;

	static int          m_leftPad;
	static int          m_rightPad;
	static int          m_lastCX;
	static int          m_lastCY;
	static int          m_smallestCX;
	static int          m_smallestCY;

public:
	CConnityDlg( bool p_reverse, CWnd* pParent = NULL);   // standard constructor
	virtual ~CConnityDlg();

	void setList( CGuiConnectionList& p_list);

	CGuiConnection*    getSelectedC();

// Dialog Data
	enum { IDD = IDD_CONNECTIVITYDIALOG };

	virtual BOOL OnInitDialog();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()
	CListBox m_options;
	CButton m_ok;
	CButton m_ca;

public:
	afx_msg void OnLbnDblclkList1();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();

};
