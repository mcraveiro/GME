//###############################################################################################################################################
//
//	Object Constraint Language Generic Manager
//	GMEConstraintPropertiesDialog.h
//
//###############################################################################################################################################

#ifndef GMEConstraintPropertiesDialog_h
#define GMEConstraintPropertiesDialog_h

#include "GMEConstraintEx.h"

namespace OclGmeCM
{
class CConstraintBrowserDialog;

//##############################################################################################################################################
//
//	C L A S S : CIdentityPage <<< + CDialog
//
//==============================================================================================================================================
//
//	D E S C R I P T I O N :
//
//##############################################################################################################################################

class CIdentityPage : public CDialog
{
// Construction
public:
	CIdentityPage( CWnd* pParent = NULL );   // standard constructor

// Dialog Data
	//{{AFX_DATA(CIdentityPage)
	enum { IDD = IDD_PROPERTIES_IDENTITY_PAGE };
	CEdit	m_edtDefinition;
	CEdit			m_edtName;
	CEdit			m_edtDescription;
	CEdit			m_edtDefault;
	CComboBox	m_cmbPriority;
	CComboBox	m_cmbDepth;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIdentityPage)
	public:
	virtual BOOL PreTranslateMessage( MSG* pMsg );
	protected:
	virtual void DoDataExchange( CDataExchange* pDX );    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CIdentityPage)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//##############################################################################################################################################
//
//	C L A S S : CExpressionPage <<< + CDialog
//
//==============================================================================================================================================
//
//	D E S C R I P T I O N :
//
//##############################################################################################################################################

class CExpressionPage : public CDialog
{
// Construction
public:
	CExpressionPage(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CExpressionPage)
	enum { IDD = IDD_PROPERTIES_EXPRESSION_PAGE };
	CComboBoxEx	m_cmbContext;
	CEdit	m_edtExpression;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExpressionPage)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CExpressionPage)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//##############################################################################################################################################
//
//	C L A S S : CEventPage <<< + CDialog
//
//==============================================================================================================================================
//
//	D E S C R I P T I O N :
//
//##############################################################################################################################################

class CEventPage : public CDialog
{
// Construction
public:
	CEventPage(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEventPage)
	enum { IDD = IDD_PROPERTIES_EVENT_PAGE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEventPage)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEventPage)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//##############################################################################################################################################
//
//	C L A S S : CConstraintPropertiesDialog <<< + CDialog
//
//==============================================================================================================================================
//
//	D E S C R I P T I O N :
//
//##############################################################################################################################################

class CConstraintPropertiesDialog : public CDialog
{

// D I A L O G   E X T E N S I O N   B Y   I M P L E M E N T O R   B E G I N
// ============================================================

	private :
		CIdentityPage 				m_pageIdentity;
		CExpressionPage 				m_pageExpression;
		CEventPage 					m_pageEvent;

		OclGme::SpConstraint 			m_spConstraintIn;
		CConstraintBrowserDialog* 	m_pDialog;

		CImageList					m_lstImages;
	public :
		OclGme::SpConstraint 		m_spConstraintOut;

	private :
		std::map<int,int> GetEventMap() const;

	private :
		void FillContextCombo( objtype_enum eType );
		int InsertItem( objtype_enum eType, CString& strItem );
		CString GetItem( int iPos );
		int FindItem( const CString& strItem );
		void PlacePage( CDialog& page );

// D I A L O G   E X T E N S I O N   B Y   I M P L E M E N T O R   E N D
// ============================================================

// Construction
public:
	CConstraintPropertiesDialog( CConstraintBrowserDialog* pDialog, OclGme::SpConstraint spConstraint, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CConstraintPropertiesDialog)
	enum { IDD = IDD_PROPERTIES_DIALOG };
	CTabCtrl	m_tabPages;
	CButton	m_btnOK;
	CButton	m_btnCancel;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConstraintPropertiesDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CConstraintPropertiesDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnClickCancel();
	afx_msg void OnClickOK();
	afx_msg void OnSelectionChangeTab(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

}; // namespace OclGmeCM

#endif // GMEConstraintPropertiesDialog_h
