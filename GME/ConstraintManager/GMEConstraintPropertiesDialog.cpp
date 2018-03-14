//###############################################################################################################################################
//
//	Object Constraint Language Generic Manager
//	GMEConstraintPropertiesDialog.cpp
//
//###############################################################################################################################################

#include "StdAfx.h"
#include "OCLGMECMFacade.h"
#include "constraintmanager.h"
#include "GMEConstraintBrowserDialog.h"
#include "GMEConstraintPropertiesDialog.h"
#include "GMESyntacticSemanticDialog.h"
#include "OCLCommonEx.h"
#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

typedef std::map<int,int> EventMap;

namespace OclGmeCM
{

//##############################################################################################################################################
//
//	C L A S S : CConstraintPropertiesDialog <<< + CDialog
//
//##############################################################################################################################################

CConstraintPropertiesDialog::CConstraintPropertiesDialog( CConstraintBrowserDialog* pDialog, OclGme::SpConstraint spConstraint, CWnd* pParent )
	: CDialog(CConstraintPropertiesDialog::IDD, pParent), m_pDialog( pDialog ), m_spConstraintIn( spConstraint )
{
	//{{AFX_DATA_INIT(CConstraintPropertiesDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CConstraintPropertiesDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConstraintPropertiesDialog)
	DDX_Control(pDX, CRP_TABPAGES, m_tabPages);
	DDX_Control(pDX, CRP_BTNOK, m_btnOK);
	DDX_Control(pDX, CRP_BTNCANCEL, m_btnCancel);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConstraintPropertiesDialog, CDialog)
	//{{AFX_MSG_MAP(CConstraintPropertiesDialog)
	ON_BN_CLICKED(CRP_BTNCANCEL, OnClickCancel)
	ON_BN_CLICKED(CRP_BTNOK, OnClickOK)
	ON_NOTIFY(TCN_SELCHANGE, CRP_TABPAGES, OnSelectionChangeTab)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CConstraintPropertiesDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_tabPages.InsertItem(  0, _T("Identity") );
	m_tabPages.InsertItem(  1, _T("Expression") );
	m_tabPages.InsertItem(  2, _T("Events") );

	m_pageIdentity.Create( IDD_PROPERTIES_IDENTITY_PAGE, &m_tabPages );
	m_pageExpression.Create( IDD_PROPERTIES_EXPRESSION_PAGE, &m_tabPages );
	m_pageEvent.Create( IDD_PROPERTIES_EVENT_PAGE, &m_tabPages );

	PlacePage( m_pageIdentity );
	PlacePage( m_pageExpression );
	PlacePage( m_pageEvent );

	m_lstImages.Create( 16, 16, ILC_COLOR24 | ILC_MASK, 28, 28 );
	CBitmap bmp;
	bmp.LoadBitmap( IDB_TREEICONS );
	m_lstImages.Add( &bmp, RGB( 255, 0, 255 ) );

	std::string strName;
	std::string strContext;
	if ( m_spConstraintIn.Ptr() )
		m_spConstraintIn->GetContextAndName( strContext, strName );

	// Fill Identity Page

	if ( m_spConstraintIn.Ptr() ) {
		m_pageIdentity.m_edtName.SetWindowText( OclCommonEx::Convert( strName ) );
		m_pageIdentity.m_edtName.SetReadOnly( true );
		m_pageIdentity.m_edtDescription.SetWindowText( OclCommonEx::Convert( m_spConstraintIn->GetMessage() ) );
		m_pageIdentity.m_edtDescription.SetReadOnly( m_spConstraintIn->GetLocation() != OclGme::ConstraintBase::CL_PROJECT );
		m_pageIdentity.m_edtDefault.SetReadOnly( m_spConstraintIn->GetLocation() != OclGme::ConstraintBase::CL_PROJECT );
		m_pageIdentity.m_cmbPriority.SetCurSel( m_spConstraintIn->GetPriority() - 1 );
		m_pageIdentity.m_cmbPriority.EnableWindow( m_spConstraintIn->GetLocation() == OclGme::ConstraintBase::CL_PROJECT );
		m_pageIdentity.m_cmbDepth.SetCurSel( m_spConstraintIn->GetDepth() );
		m_pageIdentity.m_cmbDepth.EnableWindow( m_spConstraintIn->GetLocation() == OclGme::ConstraintBase::CL_PROJECT );
		switch ( m_spConstraintIn->GetLocation() ) {
			case OclGme::ConstraintBase::CL_PROJECT : {
				CString strProject;
				COMTHROW( m_pDialog->m_pFacade->GetProject()->get_Name( PutOut( strProject ) ) );
				m_pageIdentity.m_edtDefinition.SetWindowText( L"Project: " + strProject );
				break;
			}
			case OclGme::ConstraintBase::CL_META : {
				CString strMProject;
				COMTHROW( m_pDialog->m_pFacade->GetMetaProject()->get_Name( PutOut( strMProject ) ) );
				m_pageIdentity.m_edtDefinition.SetWindowText( L"Meta: " + strMProject );
				break;
			}
			default : {
				StringVector vecLibPath = m_spConstraintIn->GetLibraryPath();
				CString strPath;
				for ( unsigned int i = 0 ; i < vecLibPath.size() ; i++ ) {
					if ( i > 0 )
						strPath += "/";
					strPath += OclCommonEx::Convert( vecLibPath[ i ] );
				}
				m_pageIdentity.m_edtDefinition.SetWindowText( L"Library: " + strPath );
				break;
			}
		}
	}
	else {
		m_pageIdentity.m_cmbPriority.SetCurSel( 0 );
		m_pageIdentity.m_cmbDepth.SetCurSel( 1 );
		CString strProject;
		COMTHROW( m_pDialog->m_pFacade->GetProject()->get_Name( PutOut( strProject ) ) );
		m_pageIdentity.m_edtDefinition.SetWindowText( L"Project: " + strProject );
	}

	// Fill Expression Page

	m_pageExpression.m_cmbContext.SetImageList( &m_lstImages );
	FillContextCombo( OBJTYPE_FOLDER );
	FillContextCombo( OBJTYPE_MODEL );
	FillContextCombo( OBJTYPE_ATOM );
	FillContextCombo( OBJTYPE_REFERENCE );
	FillContextCombo( OBJTYPE_CONNECTION );
	FillContextCombo( OBJTYPE_SET );
	if ( m_pageExpression.m_cmbContext.GetCount() != 0 )
		m_pageExpression.m_cmbContext.SetCurSel( 0 );

	GetItem( 1 );

	if ( m_spConstraintIn.Ptr() ) {
		m_pageExpression.m_cmbContext.EnableWindow( false );
		int iPos = FindItem( OclCommonEx::Convert( strContext ) );
		if ( iPos == -1 ) {
			iPos = InsertItem( OBJTYPE_NULL, L"Invalid context: " + OclCommonEx::Convert( strContext ) );
			m_btnOK.EnableWindow( m_spConstraintIn->GetLocation() == OclGme::ConstraintBase::CL_PROJECT );
		}
		m_pageExpression.m_cmbContext.SetCurSel( iPos );

		CString strExpression = OclCommonEx::Convert( m_spConstraintIn->GetExpression() );
		strExpression.Replace( _T("\r"), _T("") );
		strExpression.Replace( _T("\n"), _T("\r\n") );
		strExpression.Replace( _T("\t"), _T("    ") );
		m_pageExpression.m_edtExpression.SetWindowText( strExpression );
		m_pageExpression.m_edtExpression.SetReadOnly( m_spConstraintIn->GetLocation() != OclGme::ConstraintBase::CL_PROJECT );
	}

	// Fill Event Page

	if ( m_spConstraintIn.Ptr() ) {
		EventMap eMap = GetEventMap();
		for ( EventMap::iterator i = eMap.begin() ; i != eMap.end() ; ++i ) {
			( ( CButton* ) m_pageEvent.GetDlgItem( (*i).second ) )->SetCheck( ( m_spConstraintIn->GetEventMask() & (*i).first ) ? 1 : 0 );
			m_pageEvent.GetDlgItem( (*i).second )->EnableWindow( m_spConstraintIn->GetLocation() == OclGme::ConstraintBase::CL_PROJECT );
		}
	}

	m_btnCancel.EnableWindow( ! ( m_spConstraintIn.Ptr() && m_spConstraintIn->GetLocation() != OclGme::ConstraintBase::CL_PROJECT ) );

	return TRUE;
}

void CConstraintPropertiesDialog::PlacePage( CDialog& dlgPage )
{
	CRect rectTab; 
	m_tabPages.GetWindowRect( rectTab ); 
	m_tabPages.ScreenToClient( rectTab );
	CRect rectItem;
	m_tabPages.GetItemRect( 0, rectItem );
	
	CRect rect;
	dlgPage.GetWindowRect( rect ); 
	m_tabPages.ScreenToClient( rect );

	int iXGap = ( rectTab.Width() - rect.Width() ) / 2;
	int iYGap = ( rectTab.Height() - rectItem.Height() - rect.Height() ) / 2;
	
	rect.left = iXGap;
	rect.right = rectTab.Width() - iXGap;
	rect.top = rectItem.Height() + iYGap;
	rect.bottom = rectTab.Height() - iYGap;

	dlgPage.MoveWindow( rect );
}

void CConstraintPropertiesDialog::FillContextCombo( objtype_enum eType )
{
	OclCommonEx::MetaBaseVector vecMetas;
	OclCommonEx::GetMetaObjects( m_pDialog->m_pFacade->GetMetaProject(), "", eType, vecMetas );

	StringVector vecKinds;
	for ( unsigned int i = 0 ; i < vecMetas.size() ; i++ )
		vecKinds.push_back( OclCommonEx::GetObjectName( vecMetas[ i ].p ) );
	std::sort( vecKinds.begin(), vecKinds.end() );

	for ( unsigned int i = 0 ; i < vecKinds.size() ; i++ )
		InsertItem( eType, L"meta::" + OclCommonEx::Convert( vecKinds[ i ] ) );
}

int CConstraintPropertiesDialog::InsertItem( objtype_enum eType, CString& strItem )
{
	COMBOBOXEXITEM cbi;
	cbi.mask = CBEIF_INDENT | CBEIF_TEXT;
	if ( eType != OBJTYPE_NULL )
		cbi.mask |= CBEIF_IMAGE | CBEIF_OVERLAY | CBEIF_SELECTEDIMAGE;
	cbi.iItem = m_pageExpression.m_cmbContext.GetCount();
	cbi.pszText = strItem.GetBuffer( strItem.GetLength() );
	cbi.cchTextMax = strItem.GetLength();
	if ( eType != OBJTYPE_NULL ) {
		cbi.iImage = ( eType != OBJTYPE_FOLDER ) ? eType + 10 : ( strItem ==  _T("meta::RootFolder") ) ? 9 : 10;
		cbi.iSelectedImage = cbi.iImage;
		cbi.iOverlay = cbi.iImage;
	}
	cbi.iIndent = I_INDENTCALLBACK;
	int iResult = m_pageExpression.m_cmbContext.InsertItem( &cbi );
	strItem.ReleaseBuffer();
	return iResult;
}

CString CConstraintPropertiesDialog::GetItem( int iPos )
{
	CString strResult;
	COMBOBOXEXITEM cbi;
	cbi.mask = CBEIF_TEXT;
	cbi.iItem = iPos;
	cbi.pszText = strResult.GetBuffer( 255 );
	cbi.cchTextMax = 255;

	m_pageExpression.m_cmbContext.GetItem( &cbi );
	strResult.ReleaseBuffer();
	return strResult;
}

int CConstraintPropertiesDialog::FindItem( const CString& strItem )
{
	for ( int i = 0 ; i < m_pageExpression.m_cmbContext.GetCount() ; i++ )
		if ( GetItem( i ) == strItem )
			return i;
	return -1;
}

void CConstraintPropertiesDialog::OnClickCancel()
{
	CDialog::OnCancel();
}

void CConstraintPropertiesDialog::OnClickOK()
{
	if ( m_spConstraintIn.Ptr() && m_spConstraintIn->GetLocation() != OclGme::ConstraintBase::CL_PROJECT ) {
		CDialog::OnCancel();
		return;
	}

	// Retrieve Identity Page

	CString strName;
	m_pageIdentity.m_edtName.GetWindowText( strName );
	if ( strName.IsEmpty() ) {
		AfxMessageBox( _T("Constraint cannot be untitled.") );
		return;
	}

	CString strDescription;
	m_pageIdentity.m_edtDescription.GetWindowText( strDescription );
	if ( strDescription.IsEmpty() ) {
		AfxMessageBox( _T("Constraint must have description.") );
		return;
	}

	long lPriority = m_pageIdentity.m_cmbPriority.GetCurSel() + 1;

	constraint_depth_enum eDepth = ( constraint_depth_enum ) m_pageIdentity.m_cmbDepth.GetCurSel();

	// Retrieve Expression page

	CString strContext = GetItem( m_pageExpression.m_cmbContext.GetCurSel() );

	CString strExpression;
	m_pageExpression.m_edtExpression.GetWindowText( strExpression );
	if ( strExpression.IsEmpty() ) {
		AfxMessageBox( _T("Constraint must have expression.") );
		return;
	}

	// Retrieve Event page

	unsigned long ulEventMask = 0;
	EventMap eMap = GetEventMap();
	for ( EventMap::iterator i = eMap.begin() ; i != eMap.end() ; ++i ) {
		if ( ( ( CButton* ) m_pageEvent.GetDlgItem( (*i).second ) )->GetCheck() == 1 )
			ulEventMask |= (*i).first;
	}

	// Check whether Constraint is unique

	ConstraintVectorMap::iterator it = m_pDialog->m_mapConstraints.find( OclCommonEx::Convert( strContext ) );
	if ( it != m_pDialog->m_mapConstraints.end() ) {
		OclGme::ConstraintVector vecConstraints = (*it).second;
		for ( unsigned int i = 0 ; i < vecConstraints.size() ; i++ )
			if ( vecConstraints[ i ]->GetName() == OclCommonEx::Convert( strName ) && vecConstraints[ i ].Ptr() != m_spConstraintIn.Ptr() ) {
				AfxMessageBox( _T("Context [ ") + strContext + _T(" ] already has a constraint called ") + strName + _T(".") );
				return;
			}
	}

	OclGme::Constraint* pConstraint = new OclGme::Constraint( OclCommonEx::Convert( strName ), OclCommonEx::Convert( strContext ), OclCommonEx::Convert( strExpression ), OclCommonEx::Convert( strDescription ), ulEventMask,  lPriority, eDepth );
	OclGme::SpConstraint spConstraint( pConstraint );
	spConstraint->Register( m_pDialog->m_pFacade->GetTreeManager() );
	Ocl::Constraint::State eState = spConstraint->Parse();
	if ( eState == Ocl::Constraint::CS_PARSE_SUCCEEDED ) {
		OclTree::TypeContextStack context;
		context.AddVariable( "project", TypeSeq( 1, "gme::Project" ) );
		eState = spConstraint->Check( context );
		if ( eState == Ocl::Constraint::CS_CHECK_SUCCEEDED ) {
			spConstraint->SetDependencyResult( OclMeta::DependencySet() );
			m_spConstraintOut = spConstraint;
			CDialog::OnOK();
			return;
		}
	}

	CSyntacticSemanticDialog dlgErrors( this );
	dlgErrors.AddItem( spConstraint );
	dlgErrors.DoModal();
}

void CConstraintPropertiesDialog::OnSelectionChangeTab(NMHDR* pNMHDR, LRESULT* pResult)
{
	m_pageIdentity.ShowWindow( ( m_tabPages.GetCurSel() == 0 ) ? SW_SHOW : SW_HIDE );
	m_pageExpression.ShowWindow( ( m_tabPages.GetCurSel() == 1 ) ? SW_SHOW : SW_HIDE );
	m_pageEvent.ShowWindow( ( m_tabPages.GetCurSel() == 2 ) ? SW_SHOW : SW_HIDE );
	*pResult = 0;
}

EventMap CConstraintPropertiesDialog::GetEventMap() const
{
	EventMap eMap;

	eMap.insert( EventMap::value_type( OBJEVENT_RELATION, PEVP_CHKCHANGEASSOC ) );
	eMap.insert( EventMap::value_type( OBJEVENT_ATTR, PEVP_CHKCHANGEATTRIB ) );
	eMap.insert( EventMap::value_type( OBJEVENT_PROPERTIES, PEVP_CHKCHANGEPROP ) );
	eMap.insert( EventMap::value_type( OBJEVENT_CLOSEMODEL, PEVP_CHKCLOSEMODEL ) );
	eMap.insert( EventMap::value_type( OBJEVENT_CONNECTED, PEVP_CHKCONNECT ) );
	eMap.insert( EventMap::value_type( OBJEVENT_CREATED, PEVP_CHKCREATE ) );
	eMap.insert( EventMap::value_type( OBJEVENT_DESTROYED, PEVP_CHKDELETE ) );
	eMap.insert( EventMap::value_type( OBJEVENT_SUBT_INST, PEVP_CHKDERIVE ) );
	eMap.insert( EventMap::value_type( OBJEVENT_DISCONNECTED, PEVP_CHKDISCONNECT ) );
	eMap.insert( EventMap::value_type( OBJEVENT_SETEXCLUDED, PEVP_CHKEXCLUDESET ) );
	eMap.insert( EventMap::value_type( OBJEVENT_SETINCLUDED, PEVP_CHKINCLUDESET ) );
	eMap.insert( EventMap::value_type( OBJEVENT_LOSTCHILD, PEVP_CHKLOSTCHILD ) );
	eMap.insert( EventMap::value_type( OBJEVENT_PARENT, PEVP_CHKMOVE ) );
	eMap.insert( EventMap::value_type( OBJEVENT_NEWCHILD, PEVP_CHKNEWCHILD ) );
	eMap.insert( EventMap::value_type( OBJEVENT_REFERENCED, PEVP_CHKREFER ) );
	eMap.insert( EventMap::value_type( OBJEVENT_REFRELEASED, PEVP_CHKUNREFER ) );

	return eMap;
}

//##############################################################################################################################################
//
//	C L A S S : CIdentityPage <<< + CDialog
//
//##############################################################################################################################################

CIdentityPage::CIdentityPage(CWnd* pParent /*=NULL*/)
	: CDialog(CIdentityPage::IDD, pParent)
{
	//{{AFX_DATA_INIT(CIdentityPage)
	//}}AFX_DATA_INIT
}

void CIdentityPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIdentityPage)
	DDX_Control(pDX, PIDP_EDTDEFINITION, m_edtDefinition);
	DDX_Control(pDX, PIDP_EDTNAME, m_edtName);
	DDX_Control(pDX, PIDP_EDTDESCRIPTION, m_edtDescription);
	DDX_Control(pDX, PIDP_EDTDEFAULT, m_edtDefault);
	DDX_Control(pDX, PIDP_CMBPRIORITY, m_cmbPriority);
	DDX_Control(pDX, PIDP_CMBDEPTH, m_cmbDepth);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CIdentityPage, CDialog)
	//{{AFX_MSG_MAP(CIdentityPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CIdentityPage::PreTranslateMessage(MSG* pMsg)
{
	return ( pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE ) ? GetParent()->PreTranslateMessage( pMsg ) : CDialog::PreTranslateMessage( pMsg );
}

//##############################################################################################################################################
//
//	C L A S S : CExpressionPage <<< + CDialog
//
//##############################################################################################################################################

CExpressionPage::CExpressionPage(CWnd* pParent /*=NULL*/)
	: CDialog(CExpressionPage::IDD, pParent)
{
	//{{AFX_DATA_INIT(CExpressionPage)
	//}}AFX_DATA_INIT
}


void CExpressionPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CExpressionPage)
	DDX_Control(pDX, PEXP_CMBCONTEXT, m_cmbContext);
	DDX_Control(pDX, PEXP_EDTEXPRESSION, m_edtExpression);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CExpressionPage, CDialog)
	//{{AFX_MSG_MAP(CExpressionPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CExpressionPage::PreTranslateMessage(MSG* pMsg)
{
	return ( pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE ) ? GetParent()->PreTranslateMessage( pMsg ) : CDialog::PreTranslateMessage( pMsg );
}

//##############################################################################################################################################
//
//	C L A S S : CEventPage <<< + CDialog
//
//##############################################################################################################################################

CEventPage::CEventPage(CWnd* pParent /*=NULL*/)
	: CDialog(CEventPage::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEventPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEventPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEventPage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CEventPage, CDialog)
	//{{AFX_MSG_MAP(CEventPage)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CEventPage::PreTranslateMessage(MSG* pMsg)
{
	return ( pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE ) ? GetParent()->PreTranslateMessage( pMsg ) : CDialog::PreTranslateMessage( pMsg );
}

}; // namespace OclGmeCM
