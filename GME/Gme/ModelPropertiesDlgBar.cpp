// ModelPropertiesDlgBar.cpp : implementation file
//

#include "stdafx.h"
#include "GMEApp.h"
#include "ModelPropertiesDlgBar.h"
#include "GMEstd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CModelPropertiesDlgBar dialog

IMPLEMENT_DYNCREATE(CModelPropertiesDlgBar, CPaneDialog)


CModelPropertiesDlgBar::CModelPropertiesDlgBar()
: CPaneDialog()
{
}

BEGIN_MESSAGE_MAP(CModelPropertiesDlgBar, CPaneDialog)
	ON_CBN_SELENDOK(IDC_ZOOMS, OnSelEnd)
	ON_COMMAND(IDOK, OnOK)
END_MESSAGE_MAP()

void CModelPropertiesDlgBar::ShowType()
{
	int size = (int)(16.0 * GetDC()->GetDeviceCaps(LOGPIXELSY) / 96.0);
	GetDlgItem(IDC_TYPEMARK)->MoveWindow(6, 8, size, size);
	GetDlgItem(IDC_TYPEMARK)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_INSTANCEMARK)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_BASETYPE_LABEL)->SetWindowText(_T("Base:"));
}

void CModelPropertiesDlgBar::ShowInstance()
{
	GetDlgItem(IDC_TYPEMARK)->ShowWindow(SW_HIDE);
	int size = (int)(16.0 * GetDC()->GetDeviceCaps(LOGPIXELSY) / 96.0);
	GetDlgItem(IDC_INSTANCEMARK)->MoveWindow(6, 8, size, size);
	GetDlgItem(IDC_INSTANCEMARK)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_BASETYPE_LABEL)->SetWindowText(_T("Type:"));
}

/////////////////////////////////////////////////////////////////////////////
// CModelPropertiesDlgBar message handlers

void CModelPropertiesDlgBar::writeNumToEdit(CEdit *edit, int kk)
{
	if (!edit)
	{
		CWnd* zoom = GetDlgItem(IDC_ZOOMS);
		edit = (CEdit*)(zoom->GetDlgItem(IDC_TYPENAME));
	}
	char buff[100];
	edit->SetSel(0, -1);
	_itoa(kk, buff, 10);
	CString str = buff;
	str += _T("%");
	edit->ReplaceSel(str);
}

void CModelPropertiesDlgBar::OnOK()
{
	// enter pressed on which item
	CWnd* fwin = GetFocus();
	if (!fwin)
		return;
	CWnd* zoom = GetDlgItem(IDC_ZOOMS);
	if (!zoom)
		return;
	CEdit* edit = (CEdit*)(zoom->GetDlgItem(IDC_TYPENAME));
	if (!edit)
		return;
	if (fwin->m_hWnd == edit->m_hWnd)
	{
		CString buff = CEditGetLine(*edit, 0);
		int kk = _ttoi(buff);
		kk = abs(kk);
		if (!kk)
			kk = 100;
		// send message to zoom with (userdefined=true, kk)
		((CFrameWnd*)GetParent())->GetActiveView()->PostMessage(WM_USER_ZOOM, (WPARAM)TRUE, (LPARAM)kk);
		writeNumToEdit(edit, kk);
	}
	((CFrameWnd*)GetParent())->GetActiveView()->SetFocus();
}


void CModelPropertiesDlgBar::OnSelEnd()
{
	// item chosen from list -> zoom 
	CComboBox* zoom = (CComboBox*)GetDlgItem(IDC_ZOOMS);
	int item = zoom->GetCurSel();
	if (item != CB_ERR)
	{
		int kk = m_zoomlist[item];
		// send message to zoom with (userdefined=false, kk)
		((CFrameWnd*)GetParent())->GetActiveView()->PostMessage(WM_USER_ZOOM, (WPARAM)FALSE, (LPARAM)kk);
	}
	((CFrameWnd*)GetParent())->GetActiveView()->SetFocus();
}

void CModelPropertiesDlgBar::SetZoomList(int *list)
{
	// supposed it is sorted
	if (!list)
		return;
	int i;
	for (i=0; list[i] && i<MAX_ZOOM-1; i++)
	{
		m_zoomlist[i] = list[i];
	}
	m_zoomlist[i] = 0;
	CComboBox* zoom = (CComboBox*)GetDlgItem(IDC_ZOOMS);
	for (int k=0; m_zoomlist[k]; k++)
	{
		CString str;
		if (m_zoomlist[k] > 0)
		{
			TCHAR buff[100];
			_itot(m_zoomlist[k], buff, 10);
			str = buff;
			str += _T("%");
		}
		else
		{
			switch (m_zoomlist[k])
			{
			case ZOOM_WIDTH:
				str = _T("Fit Width");
				break;
			case ZOOM_HEIGHT:
				str = _T("Fit Height");
				break;
			case ZOOM_ALL:
				str = _T("Fit All");
				break;
			}
		}
		zoom->AddString(str);
	}
}

void CModelPropertiesDlgBar::SetZoomVal(int kk)
{
	// only to the edit box
	if (kk <= 0)
		return;
	writeNumToEdit(NULL, kk);
}

void CModelPropertiesDlgBar::NextZoomVal(int &kk)
{
	// related to the current one in the edit box 
	CComboBox* zoom = (CComboBox*)GetDlgItem(IDC_ZOOMS);
	if (!zoom)
		return;
	CEdit* edit = (CEdit*)(zoom->GetDlgItem(IDC_TYPENAME));
	if (!edit)
		return;
	CString buff = CEditGetLine(*edit, 0);
	kk = _ttoi(buff);
	if (!kk)
		kk = 100;
	int i;
	for (i=0; m_zoomlist[i]  &&  m_zoomlist[i]<=kk; i++);
	if (m_zoomlist[i])
	{
		kk = m_zoomlist[i];
		writeNumToEdit(edit, kk);
	}
}

void CModelPropertiesDlgBar::PrevZoomVal(int &kk)
{
	// related to the current one in the edit box 
	CComboBox* zoom = (CComboBox*)GetDlgItem(IDC_ZOOMS);
	if (!zoom)
		return;
	CEdit* edit = (CEdit*)(zoom->GetDlgItem(IDC_TYPENAME));
	if (!edit)
		return;
	CString buff = CEditGetLine(*edit, 0);
	kk = _ttoi(buff);
	if (!kk)
		kk = 100;
	int i;
	for (i=0; m_zoomlist[i]  &&  m_zoomlist[i]<kk; i++);
	if (m_zoomlist[i]  &&  i)
	{
		kk = m_zoomlist[i-1];
		writeNumToEdit(edit, kk);
	}
}


