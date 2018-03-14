#if !defined(AFX_SEARCHPPG_H__FDFF0CA0_A1DB_4E67_956D_E61C7A96C684__INCLUDED_)
#define AFX_SEARCHPPG_H__FDFF0CA0_A1DB_4E67_956D_E61C7A96C684__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// SearchPpg.h : Declaration of the CSearchPropPage property page class.

////////////////////////////////////////////////////////////////////////////
// CSearchPropPage : See SearchPpg.cpp.cpp for implementation.

class CSearchPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CSearchPropPage)
	DECLARE_OLECREATE_EX(CSearchPropPage)

// Constructor
public:
	CSearchPropPage();

// Dialog Data
	//{{AFX_DATA(CSearchPropPage)
	enum { IDD = IDD_PROPPAGE_SEARCH };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	//{{AFX_MSG(CSearchPropPage)
		// NOTE - ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SEARCHPPG_H__FDFF0CA0_A1DB_4E67_956D_E61C7A96C684__INCLUDED)
