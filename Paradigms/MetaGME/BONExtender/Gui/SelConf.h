#if !defined(AFX_SELCONF_H__C00FC7AA_4D0A_4DC6_946B_BFFF99DD2CB3__INCLUDED_)
#define AFX_SELCONF_H__C00FC7AA_4D0A_4DC6_946B_BFFF99DD2CB3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SelConf.h : header file
//
#include "resource.h"
#include "Table.h"
#include <AFXCMN.H>
#include <afxtempl.h>
#include "string"
#include "vector"
#include "map"

typedef struct
{
	int id;
	CString s;
	char kind;
	int cliqueId;
	bool val;
	CString resp;
	const void * ptr;
} entry;

typedef CTypedPtrList<CPtrList, entry*> CEntryList;
/////////////////////////////////////////////////////////////////////////////
// SelConf dialog

class SelConf : public CDialog
{
// Construction
public:
	SelConf( unsigned int configs, CWnd* pParent = NULL);   // standard constructor
	~SelConf();

// Dialog Data
	//{{AFX_DATA(SelConf)
	enum { IDD = IDD_DIALOG1 };
	CComboBox	m_config;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SelConf)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	int addEntry( const CString& role, char kind, int clique, bool in, const CString& repr, const void * ptr);
	bool getEntry( int row, CString& role, bool& in, CString& repr, const void * &ptr);

	void addConfigs( std::vector< std::string >& configs);
	int findAmongConfigs( std::string& config_name);

	std::string m_currSelConfigStr;

	void getDescsAncs( bool which, DWORD_PTR changed, std::vector< int > & res);
	bool addPossibleAncestors( CStringList& list, int changed);
	
	static int m_sortOrder;
	static bool getEntries( LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort, entry& e, entry& f);
	static int CALLBACK MyNameCompare(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
	static int CALLBACK MyKindCompare(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
	static int CALLBACK MyExtdCompare(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
	static int CALLBACK MyReprCompare(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

protected:
	const int m_numberOfConfigs;
	static int m_lastId;
	CEntryList m_entries;

	void selectAll( bool pVal);
	void fillUpTable(); // copies data to
	void getDataFromTable(); // copies data from m_table
	CTable m_table;

	void saveUserPref( bool check);
	void refreshTable();
	
	std::vector< std::string > m_configStrings;
	// Generated message map functions
	//{{AFX_MSG(SelConf)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnEditupdateCombo1();
	afx_msg void OnCloseupCombo1();
	afx_msg void OnBnClickedDeselectallbtn();
	afx_msg void OnBnClickedSelectallbtn();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELCONF_H__C00FC7AA_4D0A_4DC6_946B_BFFF99DD2CB3__INCLUDED_)
