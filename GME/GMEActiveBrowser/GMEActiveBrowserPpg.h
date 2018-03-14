#if !defined(AFX_GMEACTIVEBROWSERPPG_H__ED990219_21C7_4BDE_AC84_CEA1FBD2D39E__INCLUDED_)
#define AFX_GMEACTIVEBROWSERPPG_H__ED990219_21C7_4BDE_AC84_CEA1FBD2D39E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// GMEActiveBrowserPpg.h : Declaration of the CGMEActiveBrowserPropPage property page class.

////////////////////////////////////////////////////////////////////////////
// CGMEActiveBrowserPropPage : See GMEActiveBrowserPpg.cpp.cpp for implementation.

class CGMEActiveBrowserPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CGMEActiveBrowserPropPage)
	DECLARE_OLECREATE_EX(CGMEActiveBrowserPropPage)

// Constructor
public:
	CGMEActiveBrowserPropPage();

// Dialog Data
	//{{AFX_DATA(CGMEActiveBrowserPropPage)
	enum { IDD = IDD_PROPPAGE_GMEACTIVEBROWSER };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	//{{AFX_MSG(CGMEActiveBrowserPropPage)
		// NOTE - ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GMEACTIVEBROWSERPPG_H__ED990219_21C7_4BDE_AC84_CEA1FBD2D39E__INCLUDED)
