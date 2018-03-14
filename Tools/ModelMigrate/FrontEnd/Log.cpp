// Log.cpp : implementation file
//

#include "stdafx.h"
#include "ModelMigrate.h"
#include "Log.h"
#include ".\log.h"


// Log
#define CLEAR_CMD		3001

IMPLEMENT_DYNAMIC(Log, CEdit)
Log::Log()
{
}

Log::~Log()
{
}


BEGIN_MESSAGE_MAP(Log, CEdit)
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()



// Log message handlers

void Log::OnRButtonDown(UINT nFlags, CPoint point)
{
	CMenu m;
	m.CreatePopupMenu();
	m.AppendMenu( MF_ENABLED|MF_STRING, CLEAR_CMD,     "Clear");
	//m.AppendMenu( MF_ENABLED|MF_SEPARATOR, 0,             "");

	ClientToScreen( &point);
	m.TrackPopupMenu( TPM_LEFTALIGN |TPM_RIGHTBUTTON, point.x, point.y, this);

	// base impl not called
	//CEdit::OnRButtonDown(nFlags, point);
}

BOOL Log::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if( wParam == CLEAR_CMD)
	{
		doClear();
	}

	return CEdit::OnCommand(wParam, lParam);
}

void Log::doClear()
{
	SetWindowText("");
}