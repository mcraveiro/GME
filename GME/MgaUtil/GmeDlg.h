#if !defined(AFX_GMEDLG_H__AD1F4233_4627_11D4_B3F7_005004D38590__INCLUDED_)
#define AFX_GMEDLG_H__AD1F4233_4627_11D4_B3F7_005004D38590__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GmeDlg.h : header file
//

#include "Resource.h"

// copied from GmeStd.h, keep in sync with that!
#define ZOOM_WIDTH	-1
#define ZOOM_HEIGHT	-2
#define ZOOM_ALL	-3
#define ZOOM_MIN	5
#define ZOOM_MAX	500
#define ZOOM_NO		100
#define MAX_ZOOM	100  // the number of zoom levels

/////////////////////////////////////////////////////////////////////////////
// CGmeDlg dialog

class CGmeDlg : public CDialog
{
// Construction
public:
	CGmeDlg(CWnd* pParent = NULL);   // standard constructor
	CString	m_isave;
	CString	m_sysisave;
	CString	m_scriptEngine;

// Dialog Data
	//{{AFX_DATA(CGmeDlg)
	enum { IDD = IDD_GMEDLG };
	CListCtrl	m_ScriptEgines;
	CString	m_iconpath;
	CString	m_sysiconpath;
	BOOL	m_multipleview;
	BOOL	m_enablelogging;
	CString	m_autosave_dir;
	BOOL	m_autosave_enabled;
	int		m_autosave_freq;
	int		m_autosave_dest;
	BOOL	m_ext_enable;
	CString	m_ext_editor;
	BOOL	m_useAutoRouting;
	BOOL	m_labelavoidance;
	BOOL	m_sendOverObj;
	BOOL	m_timeStamps;
	BOOL	m_navigationHistory;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGmeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGmeDlg)
	afx_msg void OnAddIconPath();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnAddsysiconpath();
	afx_msg void OnAutosaveDirButton();
	afx_msg void OnAutosaveEnabled();
	afx_msg void OnAutosaveSameDir();
	afx_msg void OnAutosaveDedicatedDir();
	afx_msg void OnExtButton();
	afx_msg void OnExtEnable();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void AutosaveControlManager();
	void ExtControlManager();
	static CString getDirectory( const CString& text);
protected:
	CComObjPtr<IMgaRegistrar> registrar;

private:
	void fillScripEngineList();

	int m_zoomList[MAX_ZOOM];

	static const TCHAR* m_strZWidth;   // "Fit Width";
	static const TCHAR* m_strZHeight;  // "Fit Height";
	static const TCHAR* m_strZAll;     // "Fit All";

	void fillZoomComboBox(int *list);
	CString getZoomValueFromReg();
	CString getZoomValue();
	void setZoomValue(CString& val);

	static const TCHAR* m_strFmtStrg;
	static const TCHAR* m_strFmtStrf;
	static const TCHAR* m_strFmtStre;
	static const TCHAR* m_strFmtStrE;
	static const TCHAR* m_strFmtStrg2;
	static const TCHAR* m_strFmtStrf2;

	void fillFmtStrComboBox();
	CString getFmtStrFromReg();
	CString getFmtStrValue();

	void fillUndoComboBox();
	CString getUndoQueueSizeFromReg();
	CString getUndoQueueSizeValue();

	static const TCHAR* m_strESStrD;
	static const TCHAR* m_strESStrHS;
	static const TCHAR* m_strESStrHQ;

	void fillEdgeSmoothModeComboBox();
	CString getEdgeSmoothModeStrFromReg();
	edgesmoothmode_enum getEdgeSmoothModeValue();

	static const TCHAR* m_strFSStrSD;
	static const TCHAR* m_strFSStrSBPPGF;
	static const TCHAR* m_strFSStrSBPP;
	static const TCHAR* m_strFSStrAAGF;
	static const TCHAR* m_strFSStrAA;
	static const TCHAR* m_strFSStrCTGF;

	void fillFontSmoothModeComboBox();
	CString getFontSmoothModeStrFromReg();
	fontsmoothmode_enum getFontSmoothModeValue();

public:
	afx_msg void OnCbnSelchangeZooms();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GMEDLG_H__AD1F4233_4627_11D4_B3F7_005004D38590__INCLUDED_)
