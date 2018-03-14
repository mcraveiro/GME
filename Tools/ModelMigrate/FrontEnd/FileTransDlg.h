#pragma once
#include "afxcmn.h"
#include "FileListCtrl.h"
#include "ScriptListCtrl.h"
#include "RuleEditorDlg.h"
#include "Log.h"
#include "afxwin.h"

// FileTransDlg dialog

class FileTransDlg : public CDialog
{
	DECLARE_DYNAMIC(FileTransDlg)

public:
	FileTransDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~FileTransDlg();

// Dialog Data
	enum { IDD = IDD_DLG_FILETRANS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
public:
	FileListCtrl m_lstFiles;
	ScriptListCtrl m_lstScript;
	Log m_log;//CEdit m_log;
	HICON m_hIcon;

	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg void OnBnClickedRuleEditor();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton6();
	afx_msg void OnBnClickedButton8();
	afx_msg void OnBnClickedButtonclear();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg void OnLvnKeydownListscript(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnKeydownListfiles(NMHDR *pNMHDR, LRESULT *pResult);

protected:
	int m_cx;
	int m_cy;

	RuleEditorDlg * m_dlgRuleEditor;
	void closeDlgRuleEditor( int nRes);

	void loadDir( CString pDir);
	void loadScriptDir( CString pDir);

	void loadMyOptions( CString& pStrVal1, CString& pStrVal2, CString& pStrVal3, CString& pStrVal4, CString& pStrVal5);
	void saveMyOptions( const CString& pStrVal1, const CString& pStrVal2, const CString& pStrVal3, const CString& pStrVal4, const CString& pStrVal5);

	void process( const std::vector< std::string>& pToRun);
	void apply( CString pScrF, CString pInF, CString pOutF, CString& pErrMsg);

	void appendLog( CString t);
	void appendLn( CString t);
	void clearLog();
	void onReturn();

public:
	void selFile();
	void selDir();
	void processSel();
	void processAll();

	// misc
	void ruleEditor();
	void validatorGen();
	void options();
	void quit();

	// script related methods
	void selScript();
	void selScriptDir();

protected: // static strings
	static CString m_strSection;
	static CString m_strDirStringItem;
	static CString m_strScrStringItem;
	static CString m_strAppendStringItem;
	static CString m_strAppendStr;
	static CString m_strOutputDirStringItem;
	static CString m_strOutputDirStr;
	static CString m_strIntermedDirStringItem;
	static CString m_strIntermedDirStr;

public:    // public static methods
	static CString getDirectory( const CString& text = "Load .xme files from directory");
	static bool    directoryExists( const CString& dir);
	static bool    fileExists( const CString& file);
	static CString placeIntoTargetDir( const CString& file);
	static CString placeIntoIntermedDir( const CString& file);
};
