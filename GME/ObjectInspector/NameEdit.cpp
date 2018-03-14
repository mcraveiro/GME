// NameEdit.cpp : implementation file
//

#include "stdafx.h"
#include "objectinspector.h"
#include "InspectorDefs.h"
#include "NameEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNameEdit

CNameEdit::CNameEdit()
{
}

CNameEdit::~CNameEdit()
{
}


BEGIN_MESSAGE_MAP(CNameEdit, CEdit)
	//{{AFX_MSG_MAP(CNameEdit)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNameEdit message handlers

LRESULT CNameEdit::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{

	if(message == WM_KEYDOWN)
	{
		if(::IsWindowEnabled(GetSafeHwnd()))
		{
			switch(wParam)
			{
				case VK_ESCAPE:
					{
						GetParent()->SendMessage(MSG_NAME_EDIT_END_CANCEL);
						GetParent()->SetFocus();					
					}break;
				case VK_RETURN:
					{
						GetParent()->SendMessage(MSG_NAME_EDIT_END_OK);
						GetParent()->SetFocus();
						
					}break;
				default:
					;
			}	
		}
	}	
	
	return CEdit::WindowProc(message, wParam, lParam);
}


