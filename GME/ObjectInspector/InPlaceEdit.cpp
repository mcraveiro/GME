// InPlaceEdit.cpp : implementation file
//
// Adapted by Chris Maunder <cmaunder@mail.com>
// Copyright (c) 1998-2002. All Rights Reserved.
//
// The code contained in this file is based on the original
// CInPlaceEdit from http://www.codeguru.com/listview/edit_subitems.shtml
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed unmodified by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name and all copyright 
// notices remains intact. 
//
// An email letting me know how you are using it would be nice as well. 
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability for any damage/loss of business that
// this product may cause.
//
// For use with CGridCtrl v2.10+
//
// History:
//         10 May 1998  Uses GVN_ notifications instead of LVN_,
//                      Sends notification messages to the parent, 
//                      instead of the parent's parent.
//         15 May 1998  There was a problem when editing with the in-place editor, 
//                      there arises a general protection fault in user.exe, with a 
//                      few qualifications:
//                         (1) This only happens with owner-drawn buttons;
//                         (2) This only happens in Win95
//                         (3) This only happens if the handler for the button does not 
//                             create a new window (even an AfxMessageBox will avoid the 
//                             crash)
//                         (4) This will not happen if Spy++ is running.
//                      PreTranslateMessage was added to route messages correctly.
//                      (Matt Weagle found and fixed this problem)
//         26 Jul 1998  Removed the ES_MULTILINE style - that fixed a few probs!
//          6 Aug 1998  Added nID to the constructor param list
//          6 Sep 1998  Space no longer clears selection when starting edit (Franco Bez)
//         10 Apr 1999  Enter, Tab and Esc key prob fixed (Koay Kah Hoe)
//                      Workaround for bizzare "shrinking window" problem in CE
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "objectinspector.h"
#include "InPlaceEdit.h"
#include "InspectorDefs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInPlaceEdit

CInPlaceEdit::CInPlaceEdit()
{
}

CInPlaceEdit::~CInPlaceEdit()
{
}


BEGIN_MESSAGE_MAP(CInPlaceEdit, CEdit)
	//{{AFX_MSG_MAP(CInPlaceEdit)
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInPlaceEdit message handlers

void CInPlaceEdit::OnKillFocus(CWnd* pNewWnd) 
{
	CEdit::OnKillFocus(pNewWnd);

	if(GetModify())
	{
		GetParent()->SendMessage(MSG_EDIT_END_OK,0,0);
	}

	ShowWindow(SW_HIDE);
}


LRESULT CInPlaceEdit::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if(message == WM_KEYDOWN)
	{
		switch(wParam)
		{
			case VK_ESCAPE:
				{
					SetModify(FALSE);
					GetParent()->SetFocus();	
					return 0;
				}break;
			case VK_RETURN:
				{
					if(!(GetStyle()&ES_MULTILINE))
			case VK_TAB:	// for JIRA GME-178
					{
						GetParent()->SetFocus();
						GetParent()->SendMessage(WM_KEYDOWN, VK_TAB, lParam);
					}
				}break;
			case'D':
				{
					if(::GetKeyState(VK_CONTROL) & 0x8000)
					{
						GetParent()->SendMessage(message,wParam,lParam);
						GetParent()->SetFocus();
					}
				}break;
			case 'A':
				{
					if(::GetKeyState(VK_CONTROL) & 0x8000)
					{
						SetSel(0, -1);
					}
				}break;
			default:
				;
		}	
	}	
	return CEdit::WindowProc(message, wParam, lParam);
}
