#if !defined(AFX_ASPECTSPECDLG_H__A771B477_7ECF_41F9_8FCD_1557C770B87C__INCLUDED_)
#define AFX_ASPECTSPECDLG_H__A771B477_7ECF_41F9_8FCD_1557C770B87C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxtempl.h>
#include <AFXDLGS.H>
#include "resource.h"
#include "NameSpecTbl.h"
#include "map"
#include "string"
#include "BONImpl.h"
#include "BON.h"

// AspectSpecDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// NameSpecDlg

//class NameSpecDlg : public CPropertySheet
class NameSpecDlg : public CDialog
{
	//DECLARE_DYNAMIC(NameSpecDlg)

// Dialog Data
	//{{AFX_DATA(NameSpecDlg)
	enum { IDD = IDD_EQUIVDIALOG };
	//}}AFX_DATA

// Construction
public:
	NameSpecDlg( CWnd* pParent = NULL);   // standard constructor

// Attributes
public:

	NameSpecTbl	m_nmlist;
	int	m_lastID;
	typedef std::map< BON::FCO, std::set< BON::FCO> > LARGE_MAP;
	typedef std::map< BON::FCO, std::pair<std::string, std::string> > DEFNAMES_MAP;

	LARGE_MAP m_map;//contains the equivalent fcos (which are non-proxy)
	DEFNAMES_MAP m_dn;	// the default values , or initial values shown when the dialog pops up
	DEFNAMES_MAP m_result;	// the result

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(NameSpecDlg)
	public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
public:
	void GetEntry(int entryNum, CString& name, CString& dispname, CString& kind);

	void GetNames(int num, CString& curname, CStringList& names);
	void GetDispNames( int num, CString& curdispname, CStringList& dispnames);

	virtual ~NameSpecDlg();

	// Generated message map functions
protected:
	//{{AFX_MSG(NameSpecDlg)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ASPECTSPECDLG_H__A771B477_7ECF_41F9_8FCD_1557C770B87C__INCLUDED_)
