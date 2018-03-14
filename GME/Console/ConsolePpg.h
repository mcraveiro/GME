#if !defined(AFX_CONSOLEPPG_H__C9DDB515_3A8D_436A_9D77_B16D256C638D__INCLUDED_)
#define AFX_CONSOLEPPG_H__C9DDB515_3A8D_436A_9D77_B16D256C638D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// ConsolePpg.h : Declaration of the CConsolePropPage property page class.

////////////////////////////////////////////////////////////////////////////
// CConsolePropPage : See ConsolePpg.cpp.cpp for implementation.

class CConsolePropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CConsolePropPage)
	DECLARE_OLECREATE_EX(CConsolePropPage)

// Constructor
public:
	CConsolePropPage();

// Dialog Data
	//{{AFX_DATA(CConsolePropPage)
	enum { IDD = IDD_PROPPAGE_CONSOLE };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	//{{AFX_MSG(CConsolePropPage)
		// NOTE - ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONSOLEPPG_H__C9DDB515_3A8D_436A_9D77_B16D256C638D__INCLUDED)
