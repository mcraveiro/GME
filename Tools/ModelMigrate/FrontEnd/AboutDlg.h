#pragma once
#include "afxwin.h"
#include "resource.h"

class AboutDlg : public CDialog
{
public:
	AboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};
