#pragma once

// PanningViewPropPage.h : Declaration of the CPanningViewPropPage property page class.


// CPanningViewPropPage : See PanningViewPropPage.cpp for implementation.

class CPanningViewPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CPanningViewPropPage)
	DECLARE_OLECREATE_EX(CPanningViewPropPage)

// Constructor
public:
	CPanningViewPropPage();

// Dialog Data
	enum { IDD = IDD_PROPPAGE_PANNINGVIEW };

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	DECLARE_MESSAGE_MAP()
};

