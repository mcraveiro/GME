#include "StdAfx.h"
#include "afxcmn.h"
#include "afxmt.h"
#include "resource.h"
#include "ProgressWindow.h"

#define WM_PROGRESSWINDOW_SENDLOGLINE (WM_APP+2001)
#define WM_PROGRESSWINDOW_CLOSE (WM_PROGRESSWINDOW_SENDLOGLINE + 1)


//////////////////////////////////////////////////////
// CProgressDialog dialog
class CProgressDialog : public CDialog
{
	DECLARE_DYNAMIC(CProgressDialog)

public:
	CProgressDialog(CWnd* pParent = NULL) :
		CDialog(CProgressDialog::IDD, pParent)
	{
	}

	virtual ~CProgressDialog() {}

	enum { IDD = IDD_DIALOG_PROGRESS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX)
	{
		CDialog::DoDataExchange(pDX);
		DDX_Control(pDX, IDC_EDIT_PROGRESS, m_log);
		DDX_Control(pDX, IDC_PROGRESS_BAR, m_bar);
	}

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog()
	{
		CDialog::OnInitDialog();
		//m_bar.SetMarquee(TRUE, 0);  // this needs UNICODE to be defined (why?)
		m_bar.ModifyStyle(0, PBS_MARQUEE);
		::SendMessage(m_bar.GetSafeHwnd(), PBM_SETMARQUEE, TRUE, 0);
		return TRUE;
	}

protected:
	CEdit m_log;
	CProgressCtrl m_bar;
public:
	afx_msg LRESULT OnSendLogLine(WPARAM wParam, LPARAM lParam)
	{
		LPCTSTR line = (LPCTSTR)lParam;

		CString logStr;
		m_log.GetWindowText(logStr);
		logStr += line;
		m_log.SetWindowText(logStr);

		m_log.LineScroll(m_log.GetLineCount());

		delete [] line; // dynamically allocated by the main thread

		return TRUE;
	}
};

IMPLEMENT_DYNAMIC(CProgressDialog, CDialog)

BEGIN_MESSAGE_MAP(CProgressDialog, CDialog)
	ON_MESSAGE(WM_PROGRESSWINDOW_SENDLOGLINE, OnSendLogLine)
END_MESSAGE_MAP()


//////////////////////////////////////////////////////
// CProgressThread
class CProgressThread : public CWinThread
{
	DECLARE_DYNCREATE(CProgressThread)

public:
	CProgressThread() {}
	virtual ~CProgressThread() {

	}


	virtual BOOL InitInstance() 
	{
		m_dlg.Create(CProgressDialog::IDD, CWnd::GetDesktopWindow());
		m_dlg.ShowWindow(SW_SHOW);
		initDone.SetEvent();
		return TRUE;
	}

	virtual int ExitInstance()
	{
		if (m_dlg.GetSafeHwnd()) {
			m_dlg.DestroyWindow();
		}
		return CWinThread::ExitInstance();
	}

	CEvent initDone;

protected:
	DECLARE_MESSAGE_MAP()

	CProgressDialog m_dlg;

public:
	afx_msg void OnSendLogLine(WPARAM wParam, LPARAM lParam)
	{
		if (m_dlg.GetSafeHwnd()) {
			m_dlg.PostMessage(WM_PROGRESSWINDOW_SENDLOGLINE, wParam, lParam);
		}
	}

	afx_msg void OnClose(WPARAM , LPARAM )
	{
		PostQuitMessage(0);
	}
};

IMPLEMENT_DYNCREATE(CProgressThread, CWinThread)

BEGIN_MESSAGE_MAP(CProgressThread, CWinThread)
	ON_THREAD_MESSAGE(WM_PROGRESSWINDOW_SENDLOGLINE, OnSendLogLine)
	ON_THREAD_MESSAGE(WM_PROGRESSWINDOW_CLOSE, OnClose)
END_MESSAGE_MAP()


//////////////////////////////////////////////////////
// Public functions

static CProgressThread* pThread = NULL;

void CloseProgressWindow()
{
	if (pThread == NULL) {
		return;
	}
	pThread->PostThreadMessage(WM_PROGRESSWINDOW_CLOSE, NULL, NULL);
	::WaitForSingleObject (pThread->m_hThread, INFINITE);
	delete pThread;
	pThread = NULL; // CProgressThread kills itself
}

void UpdateProgress(LPCTSTR msg)
{
	if (!pThread) {
		pThread = (CProgressThread*)AfxBeginThread (RUNTIME_CLASS (CProgressThread),
													THREAD_PRIORITY_NORMAL,
													0, CREATE_SUSPENDED);
		pThread->m_bAutoDelete = FALSE;
		pThread->ResumeThread();
		pThread->initDone.Lock();
	}
	return;
	LPCTSTR str = (LPCTSTR)msg;
	size_t len = _tcsclen(str) + 1;
	LPTSTR strnew = new TCHAR[len]; // will be freed in the progress thread
	if (strnew) {
		_tcscpy_s(strnew, len, str);
		pThread->PostThreadMessage(WM_PROGRESSWINDOW_SENDLOGLINE, NULL, (LPARAM)strnew);
	}
}