
// svngui.h : main header file for the svngui application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols

#include "SVNClient.h"


// CsvnguiApp:
// See svngui.cpp for the implementation of this class
//

class CsvnguiApp : public CWinAppEx
{
public:
	CsvnguiApp();


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	// void Log(LPCSTR message);
	void Log(LPCTSTR message);

// Implementation
	BOOL  m_bHiColorIcons;
	CSVNClient svn;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CsvnguiApp theApp;
