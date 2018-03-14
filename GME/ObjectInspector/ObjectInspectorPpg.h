#if !defined(AFX_OBJECTINSPECTORPPG_H__026F8A77_ABCA_43CE_8B48_0D5351C58818__INCLUDED_)
#define AFX_OBJECTINSPECTORPPG_H__026F8A77_ABCA_43CE_8B48_0D5351C58818__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// ObjectInspectorPpg.h : Declaration of the CObjectInspectorPropPage property page class.

////////////////////////////////////////////////////////////////////////////
// CObjectInspectorPropPage : See ObjectInspectorPpg.cpp.cpp for implementation.

class CObjectInspectorPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CObjectInspectorPropPage)
	DECLARE_OLECREATE_EX(CObjectInspectorPropPage)

// Constructor
public:
	CObjectInspectorPropPage();

// Dialog Data
	//{{AFX_DATA(CObjectInspectorPropPage)
	enum { IDD = IDD_PROPPAGE_OBJECTINSPECTOR };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	//{{AFX_MSG(CObjectInspectorPropPage)
		// NOTE - ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OBJECTINSPECTORPPG_H__026F8A77_ABCA_43CE_8B48_0D5351C58818__INCLUDED)
