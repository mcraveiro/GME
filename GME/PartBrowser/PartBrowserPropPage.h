#pragma once

// PartBrowserPropPage.h : Declaration of the CPartBrowserPropPage property page class.


// CPartBrowserPropPage : See PartBrowserPropPage.cpp for implementation.

class CPartBrowserPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CPartBrowserPropPage)
	DECLARE_OLECREATE_EX(CPartBrowserPropPage)

// Constructor
public:
	CPartBrowserPropPage();

// Dialog Data
	enum { IDD = IDD_PROPPAGE_PARTBROWSER };

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	DECLARE_MESSAGE_MAP()
};

