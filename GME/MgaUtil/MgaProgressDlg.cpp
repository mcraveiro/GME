
#include "stdafx.h"
#include "MgaProgressDlg.h"

EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)

// --------------------------- CMgaProgressDlg

CMgaProgressDlg::CMgaProgressDlg() 
{
}

CMgaProgressDlg::~CMgaProgressDlg()
{
	if( dlg.m_hWnd != NULL )
		dlg.DestroyWindow();
}

STDMETHODIMP CMgaProgressDlg::SetTitle(BSTR t)
{
	COMTRY
	{
		title = t;

		if( dlg.m_hWnd != NULL )
			dlg.SetWindowText(title);
	}
	COMCATCH(;)
}

int nIDs[2] = {IDC_STATIC1, IDC_STATIC2};

STDMETHODIMP CMgaProgressDlg::SetLine(int line, BSTR msg)
{
	if( line < 0 || line >= 2 )
		COMRETURN(E_INVALIDARG);

	COMTRY
	{
		lines[line] = msg;

		if( dlg.m_hWnd != NULL )
			dlg.GetDlgItem(nIDs[line])->SetWindowText(lines[line]);
	}
	COMCATCH(;)
}

STDMETHODIMP CMgaProgressDlg::StartProgressDialog(HWND hwndParent)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	CWnd *win = hwndParent == NULL ? AfxGetMainWnd() : CWnd::FromHandle(hwndParent);
	// n.b. dlg.Create doesn't load comctl32 6.0
	dlg.Attach(CreateDialogW(HINST_THISCOMPONENT, MAKEINTRESOURCE(IDD_PROGRESSDLG), hwndParent, NULL));
	//dlg.Create(IDD_PROGRESSDLG, win);
	ASSERT( dlg.m_hWnd != NULL );

	dlg.SetWindowText(title);
	dlg.GetDlgItem(nIDs[0])->SetWindowText(lines[0]);
	dlg.GetDlgItem(nIDs[1])->SetWindowText(lines[1]);

	// center the dialog
	if (win->GetSafeHwnd() != NULL)
	{
		CRect rcOwner, rcDlg, rc;
		dlg.GetWindowRect(rcDlg);
		win->GetWindowRect(rcOwner);
		CopyRect(&rc, &rcOwner); 

		OffsetRect(&rcDlg, -rcDlg.left, -rcDlg.top); 
		OffsetRect(&rc, -rc.left, -rc.top); 
		OffsetRect(&rc, -rcDlg.right, -rcDlg.bottom); 

		SetWindowPos(dlg.GetSafeHwnd(), HWND_TOP, rcOwner.left + (rc.right / 2), rcOwner.top + (rc.bottom / 2), 0, 0, SWP_NOSIZE); 
	}

	dlg.ShowWindow(SW_SHOWNORMAL);
	dlg.UpdateWindow();

	if (hwndParent)
		dlg.GetParent()->EnableWindow(FALSE);

	return S_OK;
}

STDMETHODIMP CMgaProgressDlg::SetProgress(long completed, long total)
{
	if( dlg.m_hWnd == NULL )
		COMRETURN(E_INVALID_USAGE);

	CProgressCtrl *ctrl = (CProgressCtrl*)dlg.GetDlgItem(IDC_PROGRESS1);
	ASSERT( ctrl != NULL );

	if (total == 0) {
		if (!(ctrl->GetStyle() & PBS_MARQUEE)) {
			ctrl->ModifyStyle(0, PBS_MARQUEE);
			ctrl->SetMarquee(TRUE, 30);
		}
	} else {
		if ((ctrl->GetStyle() & PBS_MARQUEE)) {
			ctrl->ModifyStyle(0, PBS_MARQUEE, 0, 0);
			ctrl->SetMarquee(FALSE, 30);
		}
	}
	if (!(ctrl->GetStyle() & PBS_MARQUEE)) {
		ctrl->SetRange32(0, total);
		ctrl->SetPos(completed);
	}

	//dlg.UpdateWindow();
	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) > 0)
	{
		if (msg.message == WM_QUIT) {
			PostQuitMessage((int)msg.wParam);
			return S_OK;
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return S_OK;
}

STDMETHODIMP CMgaProgressDlg::HasUserCancelled(VARIANT_BOOL *p)
{
	if( dlg.m_hWnd == NULL )
		COMRETURN(E_INVALID_USAGE);

//	*p = dlg.cancelPressed ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP CMgaProgressDlg::StopProgressDialog()
{
	if( dlg.m_hWnd == NULL )
		COMRETURN(E_INVALID_USAGE);

	HWND hwndParent = ::GetParent(dlg.GetSafeHwnd());
	if (hwndParent)
		::EnableWindow(hwndParent, TRUE);

	HWND hwnd = dlg.m_hWnd;
	dlg.Detach();
	ASSERT( dlg.m_hWnd == NULL );
	VERIFY(::DestroyWindow(hwnd));

	return S_OK;
}
