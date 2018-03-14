#pragma once

// GMEViewPropPage.h : Declaration of the CGMEViewPropPage property page class.


// CGMEViewPropPage : See GMEViewPropPage.cpp for implementation.

class CGMEViewPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CGMEViewPropPage)
	DECLARE_OLECREATE_EX(CGMEViewPropPage)

// Constructor
public:
	CGMEViewPropPage();

// Dialog Data
	enum { IDD = IDD_PROPPAGE_GMEVIEW };

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	DECLARE_MESSAGE_MAP()
};

