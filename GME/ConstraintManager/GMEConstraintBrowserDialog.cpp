//###############################################################################################################################################
//
//	Object Constraint Language Generic Manager
//	GMEConstraintBrowserDialog.cpp
//
//###############################################################################################################################################

#include "stdafx.h"
#include "constraintmanager.h"
#include "GMEConstraintBrowserDialog.h"
#include "OCLGMECMFacade.h"
#include "GMEConstraintPropertiesDialog.h"
#include "GMESmallMessageBox.h"
#include "OCLObjectExGME.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define NILNAMESPACE ""
namespace OclGmeCM {

//##############################################################################################################################################
//
//	C L A S S : CConstraintBrowserDialog <<< + CDialog
//
//##############################################################################################################################################

CConstraintBrowserDialog::CConstraintBrowserDialog( Facade* pFacade, CComPtr<IMgaObject> spObject, CComPtr<IMgaObjects> spObjects, CWnd* pParent )
	: CDialog( CConstraintBrowserDialog::IDD, pParent ), m_pFacade( pFacade ), m_spObject( spObject ), m_spObjects( spObjects ), m_eKindActiveTree( CConstraintTreeCtrl::TK_CONSTRAINT ), m_bLoaded( false )
{
	//{{AFX_DATA_INIT(CConstraintBrowserDialog)
	//}}AFX_DATA_INIT
}

CConstraintBrowserDialog::~CConstraintBrowserDialog()
{
}

void CConstraintBrowserDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConstraintBrowserDialog)
	DDX_Control(pDX, BRW_BTNLOAD, m_btnLoad);
	DDX_Control(pDX, BRW_BTNAPPLY, m_btnApply);
	DDX_Control(pDX, BRW_TABENABLES, m_tabTrees);
	DDX_Control(pDX, BRW_BTNREMOVE, m_btnRemove);
	DDX_Control(pDX, BRW_BTNOK, m_btnOK);
	DDX_Control(pDX, BRW_BTNCHECK, m_btnCheck);
	DDX_Control(pDX, BRW_BTNCANCEL, m_btnCancel);
	DDX_Control(pDX, BRW_BTNADD, m_btnAdd);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConstraintBrowserDialog, CDialog)
	//{{AFX_MSG_MAP(CConstraintBrowserDialog)
	ON_BN_CLICKED(BRW_BTNADD, OnClickAdd)
	ON_BN_CLICKED(BRW_BTNCANCEL, OnClickCancel)
	ON_BN_CLICKED(BRW_BTNCHECK, OnClickCheck)
	ON_BN_CLICKED(BRW_BTNOK, OnClickOK)
	ON_BN_CLICKED(BRW_BTNREMOVE, OnClickRemove)
	ON_NOTIFY(TCN_SELCHANGE, BRW_TABENABLES, OnSelectionChangedTab)
	ON_BN_CLICKED(BRW_BTNAPPLY, OnClickApply)
	ON_BN_CLICKED(BRW_BTNLOAD, OnClickLoad)
	//}}AFX_MSG_MAP
	ON_NOTIFY( TVN_ITEMEXPANDING, BRW_TREEOBJECTS, OnItemExpanding )
	ON_NOTIFY( CTVN_CHECKCHANGED, BRW_TREEOBJECTS, OnCheckChanged )
	ON_NOTIFY( CTVN_CHECKCHANGEDREC, BRW_TREEOBJECTS, OnCheckChangedRecursive )
	ON_NOTIFY( CTVN_SHOWITEM, BRW_TREECONSTRAINTS, OnShowItem )
	ON_NOTIFY( TVN_SELCHANGED, BRW_TREECONSTRAINTS, OnSelectionChangedTreeConstraints )
	ON_NOTIFY( TVN_SELCHANGED, BRW_TREEOBJECTS, OnSelectionChangedTreeObjects )
END_MESSAGE_MAP()







BOOL CConstraintBrowserDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	CWaitCursor crsrWait;

	// Fill ImageLists

	m_lstObjectImages.Create( 16, 16, ILC_COLOR24 | ILC_MASK, 28, 28 );
	CBitmap bmp;
	bmp.LoadBitmap( IDB_TREEICONS );
	m_lstObjectImages.Add( &bmp, RGB( 255, 0, 255 ) );

	m_lstStateImages.Create( 16, 16, ILC_COLOR4 | ILC_MASK, 6, 6 );
	CBitmap bmp2;
	bmp2.LoadBitmap( IDB_CHECKICONS );
	m_lstStateImages.Add( &bmp2, RGB( 255, 255, 255 ) );

	m_lstCStateImages.Create( 16, 16, ILC_COLOR4 | ILC_MASK, 6, 6 );
	CBitmap bmp3;
	bmp3.LoadBitmap( IDB_CONSTRAINTSTATEICONS );
	m_lstCStateImages.Add( &bmp3, RGB( 255, 255, 255 ) );

	// Create Tabs

	m_tabTrees.InsertItem(  0, _T("Constraints") );
	m_tabTrees.InsertItem(  1, _T("Kinds and Types") );
	m_tabTrees.InsertItem(  2, _T("Settings") );

	m_pageSettings.Create( IDD_BROWSER_SETTINGS_PAGE, &m_tabTrees );
	PlaceWindow( m_pageSettings );

	// Create Constraint and Object Trees

	DWORD dwStyleTree = WS_VISIBLE | WS_TABSTOP | WS_CHILD | WS_BORDER | TVS_HASBUTTONS | TVS_LINESATROOT | TVS_HASLINES /*TVS_FULLROWSELECT*/ | TVS_DISABLEDRAGDROP | TVS_SHOWSELALWAYS | WS_EX_CLIENTEDGE;
	CRect rect;
	m_treeConstraints.Create( dwStyleTree, rect, &m_tabTrees, BRW_TREECONSTRAINTS );
	m_treeConstraints.SetImageList( &m_lstObjectImages, TVSIL_NORMAL );
	m_treeConstraints.SetImageList( &m_lstCStateImages, TVSIL_STATE );
	m_treeConstraints.SetKind( CConstraintTreeCtrl::TK_CONSTRAINT );
	PlaceWindow( m_treeConstraints, 10 );

	m_treeObjects.Create( dwStyleTree, rect, &m_tabTrees, BRW_TREEOBJECTS );
	m_treeObjects.SetImageList( &m_lstObjectImages, TVSIL_NORMAL );
	m_treeObjects.SetImageList( &m_lstStateImages, TVSIL_STATE );
	m_treeObjects.SetKind( CConstraintTreeCtrl::TK_OBJECT );
	PlaceWindow( m_treeObjects, 10 );

	// Set Tab specific things

	SetView( ( ! m_spObject.p && ! m_spObjects.p ) ? CConstraintTreeCtrl::TK_CONSTRAINT : CConstraintTreeCtrl::TK_OBJECT );
	m_tabTrees.SetCurSel( ( m_spObject.p || m_spObjects.p ) ? 1 : 0 );

	LoadSettings();
	LoadConstraints();
	LoadKinds();

	crsrWait.Restore();

	return TRUE;
}

void CConstraintBrowserDialog::PlaceWindow( CWnd& window, int iGap )
{
	CRect rectTab;
	m_tabTrees.GetWindowRect( rectTab );
	m_tabTrees.ScreenToClient( rectTab );
	CRect rectItem;
	m_tabTrees.GetItemRect( 0, rectItem );

	CRect rect;
	window.GetWindowRect( rect );
	m_tabTrees.ScreenToClient( rect );

	int iXGap = ( iGap != -1 ) ? iGap : ( rectTab.Width() - rect.Width() ) / 2;
	int iYGap = ( iGap != -1 ) ? iGap : ( rectTab.Height() - rectItem.Height() - rect.Height() ) / 2;

	rect.left = iXGap;
	rect.right = rectTab.Width() - iXGap;
	rect.top = rectItem.Height() + iYGap;
	rect.bottom = rectTab.Height() - iYGap;

	window.MoveWindow( rect );
}

void CConstraintBrowserDialog::OnClickAdd()
{
	CConstraintPropertiesDialog dlgProperties( this, NULL );
	if ( dlgProperties.DoModal() == IDOK ) {
		InsertConstraint( dlgProperties.m_spConstraintOut );
		m_btnApply.EnableWindow( true );
		m_treeConstraints.SortItemChildren( TVI_ROOT );
	}
}

void CConstraintBrowserDialog::OnClickCancel()
{
	CDialog::OnCancel();
}

void CConstraintBrowserDialog::OnClickCheck()
{
	EvaluationRecordVector vecRecords;

	AfxGetApp()->DoWaitCursor( 1 );

	if ( m_eKindActiveTree == CConstraintTreeCtrl::TK_CONSTRAINT ) {
		for ( HTREEITEM hCItem = m_treeConstraints.GetRootItem(); hCItem != NULL ; hCItem = m_treeConstraints.GetNextSiblingItem( hCItem ) ) {
			BOOL bSelected = m_treeConstraints.GetItemState( hCItem, TVIS_SELECTED ) & TVIS_SELECTED;
			if ( bSelected ) {
				ConstraintMap::iterator it = m_mapConstraint.find( ( HTREEITEM ) m_treeConstraints.GetItemData( hCItem ) );
				OclGme::SpConstraint spConstraint = (*it).second;
				if ( spConstraint->IsValid() ) {
					OclCommonEx::ObjectVector vecObjects;
					OclCommonEx::GetKindObjects( m_pFacade->GetProject(), spConstraint->GetContextType().substr( 6 ), vecObjects );
					for ( unsigned int i = 0 ; i < vecObjects.size() ; i++ ) {
						OclGme::Constraint::EnableInfo eInfo = spConstraint->GetEnableInfo( vecObjects[ i ].p );
						if ( eInfo > OclGme::Constraint::CE_NONE && eInfo <= OclGme::Constraint::CE_ENABLED_READONLY ) {
							EvaluationRecord item;
							item.spObject = CREATE_GMEOBJECT( m_pFacade->GetTreeManager()->GetTypeManager(), vecObjects[ i ].p );
							item.spConstraint = spConstraint;
							vecRecords.push_back( item );
						}
					}
				}
			}
		}
	}
	else {
		for ( HandlerSet::iterator it = m_setSelecteds.begin() ; it != m_setSelecteds.end() ; ++it ) {
			EvaluationRecord item;

			HTREEITEM hParent = m_treeObjects.GetParentItem( *it );
			MapH2ID::iterator idit = m_mapH2ID.find( ( HTREEITEM ) m_treeObjects.GetItemData( hParent ) );
			std::string strOID = (*idit).second;
			CComPtr<IMgaObject> spObject;
			COMTHROW( m_pFacade->GetProject()->GetObjectByID( CComBSTR( OclCommonEx::Convert( (*idit).second ) ), &spObject ) );
			item.spObject = CREATE_GMEOBJECT( m_pFacade->GetTreeManager()->GetTypeManager(), spObject );

			std::string strCID = OclCommonEx::Convert( m_treeObjects.GetItemText( *it ) );
			for ( ConstraintMap::iterator cit = m_mapConstraint.begin() ; cit != m_mapConstraint.end() ; ++cit ) {
				if ( (*cit).second->GetFullName() == strCID ) {
					item.spConstraint = (*cit).second;
					break;
				}
			}

			OclGme::Constraint::EnableInfo eInfo = item.spConstraint->GetEnableInfo( spObject );
			if ( eInfo > OclGme::Constraint::CE_NONE && eInfo <= OclGme::Constraint::CE_ENABLED_READONLY )
				vecRecords.push_back( item );
		}
	}

	AfxGetApp()->DoWaitCursor( -1 );

	m_pFacade->EvaluateConstraints( vecRecords, true,  &m_gotoPunk);
	 // ?? !! gotoPunk
	if (m_gotoPunk)
	{
		ApplyChanges();
		CDialog::OnOK();
	}
}

void CConstraintBrowserDialog::OnClickOK()
{
	ApplyChanges();
	CDialog::OnOK();
}

void CConstraintBrowserDialog::OnClickApply()
{
	ApplyChanges();

	COMTHROW( m_pFacade->GetProject()->CommitTransaction() );
	CComPtr<IMgaTerritory> spTerritory;
	COMTHROW( m_pFacade->GetProject()->CreateTerritory( NULL, &spTerritory ) );
	COMTHROW( m_pFacade->GetProject()->BeginTransaction( spTerritory, TRANSACTION_GENERAL));
	m_btnApply.EnableWindow( false );
}


void CConstraintBrowserDialog::OnClickRemove()
{
	if ( m_eKindActiveTree == CConstraintTreeCtrl::TK_CONSTRAINT ) {
		for ( HTREEITEM hCItem = m_treeConstraints.GetRootItem(); hCItem != NULL ; hCItem = m_treeConstraints.GetNextSiblingItem( hCItem ) ) {
			BOOL bSelected = m_treeConstraints.GetItemState( hCItem, TVIS_SELECTED ) & TVIS_SELECTED;
			if ( bSelected ) {
				ConstraintMap::iterator it = m_mapConstraint.find( ( HTREEITEM ) m_treeConstraints.GetItemData( hCItem ) );
				RemoveConstraint( (*it).second );
			}
		}
	}
}

void CConstraintBrowserDialog::OnClickLoad()
{
	CWaitCursor crsrWait;

	m_bLoaded = true;
	for ( HTREEITEM hKItem = m_treeObjects.GetChildItem( TVI_ROOT ) ; hKItem != NULL ; hKItem = m_treeObjects.GetNextSiblingItem( hKItem ) )
		ExpandKind( OclCommonEx::Convert( m_treeObjects.GetItemText( hKItem ) ), hKItem );
	m_btnLoad.EnableWindow( false );

	crsrWait.Restore();
}

void CConstraintBrowserDialog::OnSelectionChangedTab( NMHDR* pNMHDR, LRESULT* pResult )
{
	if ( m_tabTrees.GetCurSel() == 0 )
		SetView( CConstraintTreeCtrl::TK_CONSTRAINT );
	else
		if ( m_tabTrees.GetCurSel() == 1 )
			SetView( CConstraintTreeCtrl::TK_OBJECT );
		else
			SetView( CConstraintTreeCtrl::TK_UNKNOWN );
	*pResult = 0;
}

void CConstraintBrowserDialog::OnItemExpanding( NMHDR* pNMHDR, LRESULT* pResult )
{
	NM_TREEVIEW* pNMTV = (NM_TREEVIEW*) pNMHDR;
	if ( m_eKindActiveTree == CConstraintTreeCtrl::TK_OBJECT ) {
		int iImNum, iImNumS;
		m_treeObjects.GetItemImage( pNMTV->itemNew.hItem, iImNum, iImNum );
		if ( iImNum > 8 ) {
			iImNumS = m_treeObjects.GETSTATE( pNMTV->itemNew.hItem );
			if ( iImNumS == NS_UNKNOWN ) {
				MapH2ID::iterator it = m_mapH2ID.find( (HTREEITEM) m_treeObjects.GetItemData( pNMTV->itemNew.hItem ) );
				ExpandKind( (*it).second, (*it).first );
			}
		}
	}
	*pResult = 0;
}

void CConstraintBrowserDialog::OnCheckChanged( NMHDR* pNMHDR, LRESULT* pResult )
{
	NM_TREEVIEW* pNMTV = (NM_TREEVIEW*)pNMHDR;
	if ( m_eKindActiveTree == CConstraintTreeCtrl::TK_OBJECT ) {
		int iImNumS = m_treeObjects.GETSTATE( pNMTV->itemNew.hItem );
		int iImNewS = ( iImNumS == NS_UNCHECKED || iImNumS == NS_UNCHECKED_INHERITED || iImNumS == NS_MISCELLANEOUS ) ? NS_CHECKED : NS_UNCHECKED;
		SetNewState( pNMTV->itemNew.hItem, iImNewS, false );
	}
	m_btnApply.EnableWindow( true );
}

void CConstraintBrowserDialog::OnCheckChangedRecursive( NMHDR* pNMHDR, LRESULT* pResult )
{
	NM_TREEVIEW* pNMTV = (NM_TREEVIEW*)pNMHDR;
	if ( m_eKindActiveTree == CConstraintTreeCtrl::TK_OBJECT ) {
		int iImNumS = m_treeObjects.GETSTATE( pNMTV->itemNew.hItem );
		int iImNewS = ( iImNumS == NS_UNCHECKED || iImNumS == NS_UNCHECKED_INHERITED || iImNumS == NS_MISCELLANEOUS ) ? NS_CHECKED : NS_UNCHECKED;
		SetNewState( pNMTV->itemNew.hItem, iImNewS, true );
	}
	m_btnApply.EnableWindow( true );
}

void CConstraintBrowserDialog::OnShowItem( NMHDR* pNMHDR, LRESULT* pResult )
{
	NM_TREEVIEW* pNMTV = (NM_TREEVIEW*) pNMHDR;
	ConstraintMap::iterator it = m_mapConstraint.find( ( HTREEITEM ) m_treeConstraints.GetItemData( pNMTV->itemNew.hItem ) );
	if ( it != m_mapConstraint.end() ) {
		CConstraintPropertiesDialog dlgProperties( this, (*it).second );
		if ( dlgProperties.DoModal() == IDOK && (*it).second->GetLocation() == OclGme::ConstraintBase::CL_PROJECT ) {
			RemoveConstraint( (*it).second, false );
			InsertConstraint( dlgProperties.m_spConstraintOut );
			m_btnApply.EnableWindow( true );
			m_treeConstraints.SortItemChildren( TVI_ROOT );
		}
	}
}

void CConstraintBrowserDialog::OnSelectionChangedTreeConstraints(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_TREEVIEW* pNMTV = (NM_TREEVIEW*) pNMHDR;
	if ( m_eKindActiveTree == CConstraintTreeCtrl::TK_CONSTRAINT ) {
		bool bRemoveEnabled = true;
		bool bCheckEnabled = false;
		for ( HTREEITEM hCItem = m_treeConstraints.GetRootItem(); hCItem != NULL ; hCItem = m_treeConstraints.GetNextSiblingItem( hCItem ) ) {
			BOOL bSelected = m_treeConstraints.GetItemState( hCItem, TVIS_SELECTED ) & TVIS_SELECTED;
			int iImNumS = m_treeConstraints.GETSTATE( hCItem );
			if ( bSelected ) {
				if ( iImNumS == 1 )
					bCheckEnabled = true;
				int iImNum;
				m_treeConstraints.GetItemImage( hCItem, iImNum, iImNum );
				if ( iImNum == 0 || iImNum == 1 || iImNum == 4 || iImNum == 5 ) {
					bRemoveEnabled = false;
					break;
				}
			}
		}
		m_btnCheck.EnableWindow( bCheckEnabled );
		m_btnRemove.EnableWindow( bRemoveEnabled && bCheckEnabled );
	}
	*pResult = 0;
}

void CConstraintBrowserDialog::OnSelectionChangedTreeObjects(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_TREEVIEW* pNMTV = (NM_TREEVIEW*) pNMHDR;
	if ( m_eKindActiveTree == CConstraintTreeCtrl::TK_OBJECT ) {
		for ( HandlerSet::iterator it = m_setClicked.begin() ; it != m_setClicked.end() ; ++it ) {
			BOOL bSelected = m_treeObjects.GetItemState( *it, TVIS_SELECTED ) & TVIS_SELECTED;
			if ( ! bSelected ) {
				SelectItem( *it, false );
				m_setClicked.erase( *it );
			}
		}
		if ( pNMTV->itemNew.hItem ) {
			BOOL bSelected = m_treeObjects.GetItemState( pNMTV->itemNew.hItem, TVIS_SELECTED ) & TVIS_SELECTED;
			SelectItem( pNMTV->itemNew.hItem, ( bSelected ) ? true : false );
			m_setClicked.insert( pNMTV->itemNew.hItem );
		}
		m_btnCheck.EnableWindow( ! m_setSelecteds.empty() );
	}
	*pResult = 0;
}


	void CConstraintBrowserDialog::SelectItem( HTREEITEM hItem, bool bSelect )
	{
		int iImNum;
		m_treeObjects.GetItemImage( hItem, iImNum, iImNum );

		// Object Selection

		if ( iImNum > 8 ) {
			CString strKind = m_treeObjects.GetItemText( hItem );
			ExpandKind( OclCommonEx::Convert( m_treeObjects.GetItemText( hItem ) ), hItem );
			for ( HTREEITEM hKItem = m_treeObjects.GetRootItem(); hKItem != NULL ; hKItem = m_treeObjects.GetNextSiblingItem( hKItem ) ) {
				if ( m_treeObjects.GetItemText( hKItem ) == strKind ) {
					hItem = hKItem;
					break;
				}
			}
		}
		if ( iImNum > 5 ) {
			for ( HTREEITEM hCItem = m_treeObjects.GetChildItem( hItem ); hCItem != NULL ; hCItem = m_treeObjects.GetNextSiblingItem( hCItem ) ) {
				int iImNumC;
				m_treeObjects.GetItemImage( hCItem, iImNumC, iImNumC );
				if ( iImNumC < 6 )
					SelectItem( hCItem, bSelect );
			}
			return;
		}

		// Constraint Selection

		HTREEITEM hParent = m_treeObjects.GetParentItem( hItem );
		m_treeObjects.GetItemImage( hParent, iImNum, iImNum );
		if ( iImNum < 9 )
			if ( bSelect )
				m_setSelecteds.insert( hItem );
			else
				m_setSelecteds.erase( hItem );
		for ( HTREEITEM hOItem = m_treeObjects.GetChildItem( hParent ); hOItem != NULL ; hOItem = m_treeObjects.GetNextSiblingItem( hOItem ) ) {
			int iImNumO;
			m_treeObjects.GetItemImage( hOItem, iImNumO, iImNumO );
			if ( iImNumO > 5 )
				SelectItem( FindConstraintNode( hOItem, OclCommonEx::Convert( m_treeObjects.GetItemText( hItem ) ) ), bSelect );
		}
	}

	void CConstraintBrowserDialog::ApplyChanges()
	{
		m_pFacade->m_vecUserConstraints.clear();
		m_pFacade->m_vecMetaConstraints.clear();

		for ( ConstraintMap::iterator i = m_mapConstraint.begin() ; i != m_mapConstraint.end() ; ++i ) {
			if ( (*i).second->GetLocation() != OclGme::ConstraintBase::CL_META )
				m_pFacade->m_vecUserConstraints.push_back( (*i).second );
			else
				m_pFacade->m_vecMetaConstraints.push_back( (*i).second );
		}

		EvaluationInfo info;

		info.bEnabledSCLogical = m_pageSettings.m_chkEnableLogical.GetCheck() == 1;
		info.bEnabledSCIterator = m_pageSettings.m_chkEnableIterator.GetCheck() == 1;
		info.bEnabledTracking = m_pageSettings.m_chkEnableTracking.GetCheck() == 1;

		if ( ( (CButton*) m_pageSettings.GetDlgItem( BSEP_RDVIOLFIRST ) )->GetCheck() == 1 )
			info.iViolationCount = 1;
		else if ( ( (CButton*) m_pageSettings.GetDlgItem( BSEP_RDLEVELFIRST ) )->GetCheck() == 1 )
			info.iViolationCount = -1;
		else if ( ( (CButton*) m_pageSettings.GetDlgItem( BSEP_RDDEFAULT ) )->GetCheck() == 1 )
			info.iViolationCount = -2;
		else {
			CString strCount;
			m_pageSettings.m_edtViolationCount.GetWindowText( strCount );
			info.iViolationCount = _ttoi( strCount );
		}

		if ( ( (CButton*) m_pageSettings.GetDlgItem( BSEP_RDZERODEPTH ) )->GetCheck() == 1 )
			info.iModelDepth = 0;
		else if ( ( (CButton*) m_pageSettings.GetDlgItem( BSEP_RDONEDEPTH ) )->GetCheck() == 1 )
			info.iModelDepth = 1;
		else
			info.iModelDepth = -1;

		m_pFacade->SetEvaluationInfo( info );
	}

	void CConstraintBrowserDialog::LoadSettings()
	{
		EvaluationInfo info = m_pFacade->GetEvaluationInfo();

		m_pageSettings.m_chkEnableLogical.SetCheck( info.bEnabledSCLogical );
		m_pageSettings.m_chkEnableIterator.SetCheck( info.bEnabledSCIterator );
		m_pageSettings.m_chkEnableTracking.SetCheck( info.bEnabledTracking );

		if ( info.iViolationCount > 1 ) {
			( (CButton*) m_pageSettings.GetDlgItem( BSEP_RDVIOLCOUNT ) )->SetCheck( 1 );
			m_pageSettings.m_edtViolationCount.SetReadOnly( false );
			CString strCount;
			strCount.Format( _T("%d"), info.iViolationCount );
			m_pageSettings.m_edtViolationCount.SetWindowText( strCount );
		}
		else {
			m_pageSettings.m_edtViolationCount.SetWindowText( _T("2") );
			switch( info.iViolationCount ) {
				case 1 : ( (CButton*) m_pageSettings.GetDlgItem( BSEP_RDVIOLFIRST ) )->SetCheck( 1 ); break;
				case -1 : ( (CButton*) m_pageSettings.GetDlgItem( BSEP_RDLEVELFIRST ) )->SetCheck( 1 ); break;
				default : ( (CButton*) m_pageSettings.GetDlgItem( BSEP_RDDEFAULT ) )->SetCheck( 1 ); break;
			}
		}

		switch ( info.iModelDepth ) {
			case 0 : ( (CButton*) m_pageSettings.GetDlgItem( BSEP_RDZERODEPTH ) )->SetCheck( 1 ); break;
			case 1 : ( (CButton*) m_pageSettings.GetDlgItem( BSEP_RDONEDEPTH ) )->SetCheck( 1 ); break;
			default : ( (CButton*) m_pageSettings.GetDlgItem( BSEP_RDANYDEPTH ) )->SetCheck( 1 ); break;
		}
	}

	void CConstraintBrowserDialog::LoadConstraints()
	{
		for ( unsigned int i = 0 ; i < m_pFacade->m_vecMetaConstraints.size() ; i++ )
			AddConstraint( m_pFacade->m_vecMetaConstraints[ i ] );
		for ( unsigned int i = 0 ; i < m_pFacade->m_vecUserConstraints.size() ; i++ )
			AddConstraint( m_pFacade->m_vecUserConstraints[ i ] );

		m_treeConstraints.SortItemChildren( TVI_ROOT );
	}

	void CConstraintBrowserDialog::LoadKinds()
	{
		OclCommonEx::MetaBaseVector vecMetas;
		OclCommonEx::GetMetaObjects( m_pFacade->GetMetaProject(), "", OBJTYPE_NULL, vecMetas );
		UINT uiMask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_STATE | TVIF_TEXT;
		int iImNumS = NS_UNKNOWN;
		int iImNum;
		CComPtr<IMgaMetaFolder> spRootFolder;
		COMTHROW( m_pFacade->GetMetaProject()->get_RootFolder( &spRootFolder ) );
		for ( unsigned int i = 0 ; i < vecMetas.size() ; i++ ) {
			std::string strKind = OclCommonEx::GetObjectName( vecMetas[ i ].p );
			objtype_enum eType = OclCommonEx::GetObjectType( vecMetas[ i ].p );
			if ( eType == OBJTYPE_FOLDER )
				iImNum = ( vecMetas[ i ].p == spRootFolder.p ) ? 9 : 10;
			else
				iImNum = eType + 10;
			HTREEITEM hItem = m_treeObjects.InsertItem( uiMask, OclCommonEx::Convert( strKind ), iImNum, iImNum, iImNumS << 12 , TVIS_STATEIMAGEMASK, NULL, TVI_ROOT, TVI_LAST );
			m_treeObjects.SetItemData( hItem, (DWORD_PTR) hItem );
			m_mapH2ID.insert( MapH2ID::value_type( hItem, strKind ) );
			m_mapID2H.insert( MapID2H::value_type( strKind, hItem ) );
		}

		m_treeObjects.SortItemChildren( TVI_ROOT );
	}

	void CConstraintBrowserDialog::AddConstraint( OclGme::SpConstraint spConstraint )
	{
		// Set State

		int iImNumS;
		switch ( spConstraint->GetState() ) {
			case Ocl::Constraint::CS_CTX_PARSE_FAILED :
			case Ocl::Constraint::CS_PARSE_FAILED :
				iImNumS = CSIMG_SYNTAX_ERROR;
				break;
			case Ocl::Constraint::CS_CTX_CHECK_FAILED :
			case Ocl::Constraint::CS_CHECK_FAILED : {
				iImNumS = CSIMG_SEMANTIC_ERROR;
				std::string strType = spConstraint->GetFullName();
				auto iPos = strType.rfind( ":" );
				strType = strType.substr( 0, iPos - 1 );
				try {
					m_pFacade->GetTreeManager()->GetTypeManager()->GetType( strType, NILNAMESPACE );
				}
				catch ( ... ) {
					iImNumS = CSIMG_CONTEXT_ERROR;
				}
				break;
			}
			case Ocl::Constraint::CS_CHECK_DEPENDENCY_FAILED :
				iImNumS = CSIMG_DEPENDENCY_ERROR;
				break;
			case Ocl::Constraint::CS_CHECK_DEPENDENCY_SUCCEEDED :
			case Ocl::Constraint::CS_EVAL_FAILED :
			case Ocl::Constraint::CS_EVAL_SUCCEEDED :
				iImNumS = CSIMG_OK;
				break;
			default :
				iImNumS = CSIMG_CONTEXT_ERROR;
		}

		// Insert Constraint

		UINT uiMask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_STATE | TVIF_TEXT;
		int iImNum = spConstraint->GetType();
		HTREEITEM hItem = m_treeConstraints.InsertItem( uiMask, OclCommonEx::Convert( spConstraint->GetFullName() ), iImNum, iImNum, iImNumS << 12 , TVIS_STATEIMAGEMASK, NULL, TVI_ROOT, TVI_LAST );
		m_treeConstraints.SetItemData( hItem, (DWORD_PTR) hItem );
		if ( iImNumS == CSIMG_OK ) {
			ConstraintVectorMap::iterator it = m_mapConstraints.find( spConstraint->GetContextType() );
			if ( it == m_mapConstraints.end() )
				m_mapConstraints.insert( ConstraintVectorMap::value_type( spConstraint->GetContextType(), OclGme::ConstraintVector( 1, spConstraint ) ) );
			else
				m_mapConstraints[ (*it).first ].push_back( spConstraint );
		}
		m_mapConstraint.insert( ConstraintMap::value_type( hItem, spConstraint ) );
	}

	void CConstraintBrowserDialog::ExpandKind( const std::string& strKind, HTREEITEM hItem )
	{
		if ( m_treeObjects.GETSTATE( hItem ) != NS_UNKNOWN )
			return;

		// Create ConstraintVector

		int iRootIm;
		m_treeObjects.GetItemImage( hItem, iRootIm, iRootIm );

		OclGme::ConstraintVector vecConstraints;
		ConstraintVectorMap::iterator it =  m_mapConstraints.find( "meta::" + strKind );
		if ( it != m_mapConstraints.end() )
			vecConstraints = (*it).second;

		// Add Constraints As Children

		for ( unsigned int i = 0 ; i < vecConstraints.size() ; i++ ) {
			UINT uiMask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_STATE | TVIF_TEXT;
			int iImNum = vecConstraints[ i ]->GetType();
			int iImNumS = ( vecConstraints[ i ]->GetLocation() != OclGme::ConstraintBase::CL_PROJECT && vecConstraints[ i ]->GetPriority() == 1 ) ? NS_CHECKED_DISABLED : NS_CHECKED;
			HTREEITEM hCItem = m_treeObjects.InsertItem( uiMask, OclCommonEx::Convert( vecConstraints[ i ]->GetFullName() ), iImNum, iImNum, iImNumS << 12 , TVIS_STATEIMAGEMASK, NULL, hItem, TVI_LAST );
			m_treeObjects.SetItemData( hCItem, (DWORD_PTR)hCItem );
		}
		m_treeObjects.SETSTATE( hItem, (int) ( ( vecConstraints.size() == 0 ) ? NS_UNCHECKED_DISABLED : NS_CHECKED ) );

		OclCommonEx::ObjectVector vecObjects;
		OclCommonEx::GetKindObjects( m_pFacade->GetProject(), strKind, vecObjects );
		for ( unsigned int i = 0 ; i < vecObjects.size() ; i++ ) {
			CComQIPtr<IMgaFCO> spFCO = vecObjects[ i ].p;
			if ( spFCO.p ) {
				CComPtr<IMgaFCO> spArche;
				COMTHROW( spFCO->get_ArcheType( &spArche ) );
				if ( ! spArche.p )
					LoadObject( spFCO.p, hItem, vecConstraints );
			}
			else
				LoadObject( vecObjects[ i ].p, hItem, vecConstraints );
		}
		m_treeObjects.SortItemChildren( hItem );
	}

	void CConstraintBrowserDialog::LoadObject( CComPtr<IMgaObject> spObject, HTREEITEM hParent, const OclGme::ConstraintVector& vecConstraints )
	{
		// Determine icon

		std::string strName = OclCommonEx::GetObjectName( spObject );
		CComQIPtr<IMgaFCO> spFCO = spObject;
		CComQIPtr<IMgaFolder> spFolder = spObject;
		int iImNum = 6;
		if ( spFCO.p ) {
			VARIANT_BOOL vbInstance;
			COMTHROW( spFCO->get_IsInstance( &vbInstance ) );
			if ( vbInstance )
				iImNum = 8;
			else {
				CComPtr<IMgaFCO> spArche;
				COMTHROW( spFCO->get_ArcheType( &spArche ) );
				if ( spArche.p )
					iImNum = 7;
			}
		}
		else {
			CComPtr<IMgaFolder> spRoot;
			COMTHROW( m_pFacade->GetProject()->get_RootFolder( &spRoot ) );
			iImNum = ( spObject.p == spRoot.p ) ? 9 : 10;
		}

		// Insert TreeItem

		CString strID;
		COMTHROW( spObject->get_ID( PutOut( strID ) ) );
		int iImNumS = NS_UNKNOWN;
		UINT uiMask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_STATE | TVIF_TEXT;
		HTREEITEM hItem = m_treeObjects.InsertItem( uiMask, OclCommonEx::Convert( strName ), iImNum, iImNum, iImNumS << 12 , TVIS_STATEIMAGEMASK, NULL, hParent, TVI_LAST );
		m_treeObjects.SetItemData( hItem, (DWORD_PTR)hItem );
		m_mapH2ID.insert( MapH2ID::value_type( hItem, OclCommonEx::Convert( strID ) ) );
		m_mapID2H.insert( MapID2H::value_type( OclCommonEx::Convert( strID ), hItem ) );

		for ( unsigned int i = 0 ; i < vecConstraints.size() ; i++ ) {

			// Add Constraint

			int iImCNum = vecConstraints[ i ]->GetType();
			int iImCNumS = NS_UNKNOWN;
			HTREEITEM hCItem = m_treeObjects.InsertItem( uiMask, OclCommonEx::Convert( vecConstraints[ i ]->GetFullName() ), iImCNum, iImCNum, iImCNumS << 12 , TVIS_STATEIMAGEMASK, NULL, hItem, TVI_LAST );
			m_treeObjects.SetItemData( hCItem, (DWORD_PTR)hCItem );

			// Determine state

			switch ( vecConstraints[ i ]->GetEnableInfo( spObject ) ) {
				case OclGme::Constraint::CE_NONE :
				case OclGme::Constraint::CE_DISABLED_READONLY :
					iImCNumS = NS_UNCHECKED_DISABLED; break;
				case OclGme::Constraint::CE_ENABLED_READONLY :
					iImCNumS = NS_CHECKED_DISABLED; break;
				case OclGme::Constraint::CE_ENABLED_INHERITED :
					iImCNumS = NS_CHECKED_INHERITED; break;
				case OclGme::Constraint::CE_DISABLED_INHERITED :
					iImCNumS = NS_UNCHECKED_INHERITED; break;
				case OclGme::Constraint::CE_ENABLED :
					iImCNumS = NS_CHECKED; break;
				case OclGme::Constraint::CE_DISABLED :
					iImCNumS = NS_UNCHECKED; break;
			}

			// Refreshing Root State and Set This State

			m_treeObjects.SETSTATE( hCItem, iImCNumS );
		}
		RefreshRootState( hItem );

		if ( vecConstraints.size() == 0 )
			m_treeObjects.SETSTATE( hItem, NS_UNCHECKED_DISABLED );

		// Load Derived Objects

		if ( spFCO.p && iImNum != 8 ) {
			CComPtr<IMgaFCOs> spDeriveds;
			COMTHROW( spFCO->get_DerivedObjects( &spDeriveds ) );
			MGACOLL_ITERATE( IMgaFCO, spDeriveds ) {
				LoadObject( MGACOLL_ITER.p, hItem, vecConstraints );
			} MGACOLL_ITERATE_END;
		}

		m_treeObjects.SortItemChildren( hItem );
	}

	void CConstraintBrowserDialog::RefreshRootState( HTREEITEM hParent )
	{
		int iCount = 0;
		int iChecked = 0;
		int iDisabled = 0;
		for ( HTREEITEM hCItem = m_treeObjects.GetChildItem( hParent ) ; hCItem != NULL ; hCItem = m_treeObjects.GetNextSiblingItem( hCItem ) ) {
			int iImNumC;
			m_treeObjects.GetItemImage( hCItem, iImNumC, iImNumC );
			if ( iImNumC < 6 ) {
				int iImNumS = m_treeObjects.GETSTATE( hCItem );
				iCount++;
				if ( iImNumS == NS_CHECKED || iImNumS == NS_CHECKED_INHERITED || iImNumS == NS_CHECKED_DISABLED )
					iChecked++;
				if ( iImNumS == NS_CHECKED_DISABLED || iImNumS == NS_UNCHECKED_DISABLED )
					iDisabled++;
			}
		}
		if ( iCount == 0 )
			m_treeObjects.SETSTATE( hParent, (int) NS_UNCHECKED_DISABLED );
		else if ( iCount == iDisabled )
			if ( iChecked == 0 )
				m_treeObjects.SETSTATE( hParent, (int) NS_UNCHECKED_DISABLED );
			else
				m_treeObjects.SETSTATE( hParent, (int) NS_CHECKED_DISABLED );
		else if ( iCount == iChecked )
			m_treeObjects.SETSTATE( hParent, (int) NS_CHECKED );
		else if ( iChecked == 0 )
			m_treeObjects.SETSTATE( hParent, (int) NS_UNCHECKED );
		else
			m_treeObjects.SETSTATE( hParent, (int) NS_MISCELLANEOUS );
	}

	int CConstraintBrowserDialog::GetAncestorState( HTREEITEM hItem, const std::string& strCID )
	{
		HTREEITEM hParent = m_treeObjects.GetParentItem( hItem ); // Assume Instance
		hParent = m_treeObjects.GetParentItem( hParent ); // Then Its Type
		if ( ! hParent )
			return NS_ERROR;
		HTREEITEM hCItem = FindConstraintNode( hParent, strCID );
		if ( hCItem )
			return m_treeObjects.GETSTATE( hCItem );
		return NS_ERROR;
	}

	void CConstraintBrowserDialog::SetNewState( HTREEITEM hItem, int iNewS, bool bInheritance )
	{
		int iImNum;
		m_treeObjects.GetItemImage( hItem, iImNum, iImNum );

		// If not Constraint TreeNode

		if ( iImNum > 5 ) {
			for ( HTREEITEM hCItem = m_treeObjects.GetChildItem( hItem ) ; hCItem != NULL ; hCItem = m_treeObjects.GetNextSiblingItem( hCItem ) ) {
				int iImNumC;
				m_treeObjects.GetItemImage( hCItem, iImNumC, iImNumC );
				if ( iImNumC > 0 && iImNumC < 6 )
					SetNewState( hCItem, iNewS, bInheritance );
				else
					if ( bInheritance )
						SetNewState( hCItem, iNewS, bInheritance );
			}
			return;
		}

		// If Constraint TreeNode

		if ( m_treeObjects.GETSTATE( hItem ) == NS_UNCHECKED_DISABLED || m_treeObjects.GETSTATE( hItem ) == NS_CHECKED_DISABLED )
			return;

		// Obtain parent

		HTREEITEM hParent = m_treeObjects.GetParentItem( hItem );
		int iImNumP;
		m_treeObjects.GetItemImage( hParent, iImNumP, iImNumP );

		// determine the new state

		CString strCID = m_treeObjects.GetItemText( hItem );
		int iAncestor = GetAncestorState( hItem, OclCommonEx::Convert( strCID ) );
		int iRealNewS;
		if ( iNewS == NS_CHECKED )
			iRealNewS = ( ( iAncestor == NS_CHECKED || iAncestor == NS_CHECKED_INHERITED ) && iImNumP != 6 ) ? NS_CHECKED_INHERITED : NS_CHECKED;
		else
			iRealNewS = ( ( iAncestor == NS_UNCHECKED || iAncestor == NS_UNCHECKED_INHERITED ) && iImNumP != 6 ) ? NS_UNCHECKED_INHERITED : NS_UNCHECKED;

		// if parent is Object and not kind, perform the change in registry

		if ( iImNumP > 5 && iImNumP < 9 ) {
			MapH2ID::iterator it = m_mapH2ID.find( (HTREEITEM) m_treeObjects.GetItemData( hParent ) );
			CComPtr<IMgaObject> spObject;
			COMTHROW( m_pFacade->GetProject()->GetObjectByID( CComBSTR( OclCommonEx::Convert( (*it).second ) ), &spObject ) );
			CComQIPtr<IMgaRegNode> spRegNode;
			CComQIPtr<IMgaFCO> spFCO = spObject;
			if ( spFCO.p )
				COMTHROW( spFCO->get_RegistryNode( CComBSTR( L"ConstraintEnabling/" + strCID ), &spRegNode ) );
			else {
				CComQIPtr<IMgaFolder> spFolder = spObject;
				COMTHROW( spFolder->get_RegistryNode( CComBSTR( L"ConstraintEnabling/" + strCID ), &spRegNode ) );
			}
			if ( iRealNewS == NS_CHECKED_INHERITED || iRealNewS == NS_UNCHECKED_INHERITED && iImNumP != 6 )
				COMTHROW( spRegNode->RemoveTree() );
			else
				COMTHROW( spRegNode->put_Value( CComBSTR( ( iRealNewS == NS_CHECKED || iRealNewS == NS_CHECKED_INHERITED ) ? "yes" : "no" ) ) );
		}

		// Refresh Root and Set State in Tree

		m_treeObjects.SETSTATE( hItem, iRealNewS );
		RefreshRootState( hParent );

		// Refresh All Inherited

		for ( HTREEITEM hOItem = m_treeObjects.GetChildItem( hParent ) ; hOItem != NULL ; hOItem = m_treeObjects.GetNextSiblingItem( hOItem ) ) {
			int iImNumO;
			m_treeObjects.GetItemImage( hOItem, iImNumO, iImNumO );
			if ( iImNumO > 5 ) {
				HTREEITEM hCItem = FindConstraintNode( hOItem, OclCommonEx::Convert( strCID ) );
				if ( hCItem ) {
					if ( ! bInheritance ) {
						iNewS = m_treeObjects.GETSTATE( hCItem );
						if ( iNewS == NS_CHECKED_INHERITED )
							iNewS = NS_CHECKED;
						else if ( iNewS == NS_UNCHECKED_INHERITED )
							iNewS = NS_UNCHECKED;
					}
					SetNewState( hCItem, iNewS, bInheritance );
				}
			}
		}
	}

	HTREEITEM CConstraintBrowserDialog::FindConstraintNode( HTREEITEM hItem, const std::string& strCID )
	{
		for ( HTREEITEM hCItem = m_treeObjects.GetChildItem( hItem ) ; hCItem != NULL ; hCItem = m_treeObjects.GetNextSiblingItem( hCItem ) ) {
			int iImNumC;
			m_treeObjects.GetItemImage( hCItem, iImNumC, iImNumC );
			if ( iImNumC < 6 ) {
				CString strID = m_treeObjects.GetItemText( hCItem );
				if ( strCID == OclCommonEx::Convert( strID ) )
					return hCItem;
			}
		}
		return NULL;
	}

	void CConstraintBrowserDialog::SetView( CConstraintTreeCtrl::Kind eKind )
	{
		if ( m_eKindActiveTree == CConstraintTreeCtrl::TK_OBJECT && m_treeObjects.IsWindowVisible() )
			m_treeObjects.ClearSelection();
		if ( m_eKindActiveTree == CConstraintTreeCtrl::TK_CONSTRAINT && m_treeConstraints.IsWindowVisible() )
			m_treeConstraints.ClearSelection();

		m_eKindActiveTree = eKind;
		m_btnRemove.EnableWindow( false );
		m_btnAdd.EnableWindow( eKind == CConstraintTreeCtrl::TK_CONSTRAINT );

		m_treeConstraints.ShowWindow( ( eKind == CConstraintTreeCtrl::TK_CONSTRAINT ) ? SW_SHOW : SW_HIDE );
		m_treeObjects.ShowWindow( ( eKind == CConstraintTreeCtrl::TK_OBJECT ) ? SW_SHOW : SW_HIDE );
		m_pageSettings.ShowWindow( ( eKind == CConstraintTreeCtrl::TK_UNKNOWN ) ? SW_SHOW : SW_HIDE );

		m_btnLoad.EnableWindow( eKind == CConstraintTreeCtrl::TK_OBJECT && ! m_bLoaded );

		m_btnCheck.EnableWindow( eKind != CConstraintTreeCtrl::TK_UNKNOWN );
	}

	void CConstraintBrowserDialog::RefreshKind( const std::string& strKind )
	{
		MapID2H::iterator it = m_mapID2H.find( strKind );
		m_treeObjects.DeleteItem( (*it).second );
		m_mapID2H.erase( it );

		OclCommonEx::MetaBaseVector vecMetas;
		OclCommonEx::GetMetaObjects( m_pFacade->GetMetaProject(), strKind, OBJTYPE_NULL, vecMetas );
		UINT uiMask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_STATE | TVIF_TEXT | TVIF_CHILDREN;
		int iImNumS = NS_UNKNOWN;
		int iImNum;
		CComPtr<IMgaMetaFolder> spRootFolder;
		COMTHROW( m_pFacade->GetMetaProject()->get_RootFolder( &spRootFolder ) );
		objtype_enum eType = OclCommonEx::GetObjectType( vecMetas[ 0 ].p );
		if ( eType == OBJTYPE_FOLDER )
			iImNum = ( vecMetas[ 0 ].p == spRootFolder.p ) ? 9 : 10;
		else
			iImNum = eType + 10;
		HTREEITEM hItem = m_treeObjects.InsertItem( uiMask, OclCommonEx::Convert( strKind ), iImNum, iImNum, iImNumS << 12 , TVIS_STATEIMAGEMASK, NULL, TVI_ROOT, TVI_LAST );
		m_treeObjects.SetItemData( hItem, (DWORD_PTR)hItem );
		m_mapH2ID.insert( MapH2ID::value_type( hItem, strKind ) );
		m_mapID2H.insert( MapID2H::value_type( strKind, hItem ) );

		m_treeObjects.SortItemChildren( TVI_ROOT );

		ExpandKind( strKind, hItem );
	}

	void CConstraintBrowserDialog::InsertConstraint( OclGme::SpConstraint spConstraint )
	{
		// Add Constraint to Tree, and Context keyed map

		AddConstraint( spConstraint );

		// Write it to registry

		CComPtr<IMgaFolder> spRootFolder;
		COMTHROW( m_pFacade->GetProject()->get_RootFolder( &spRootFolder ) );
		CComPtr<IMgaRegNode> spRegNode;
		COMTHROW( spRootFolder->get_RegistryNode( CComBSTR( L"ConstraintDefinitions/" + OclCommonEx::Convert( spConstraint->GetFullName() ) ), &spRegNode ) );
		spConstraint->Write( spRegNode );

		// Remove the kind from Object Tree, and refresh it

		RefreshKind( spConstraint->GetContextType().substr( 6 ) );
	}

	void CConstraintBrowserDialog::RemoveConstraint( OclGme::SpConstraint spConstraint, bool bRemoveEnableInfo )
	{
		// Remove it from constraint map and constraint tree

		for ( HTREEITEM hCItem = m_treeConstraints.GetChildItem( TVI_ROOT ) ; hCItem != NULL ; hCItem = m_treeObjects.GetNextSiblingItem( hCItem ) ) {
			CString strCID = m_treeConstraints.GetItemText( hCItem );
			if ( OclCommonEx::Convert( strCID ) == spConstraint->GetFullName() ) {
				HTREEITEM hRealItem = ( HTREEITEM ) m_treeConstraints.GetItemData( hCItem );
				m_treeConstraints.DeleteItem( hCItem );
				m_mapConstraint.erase( hRealItem );
				break;
			}
		}

		// Remove it from context map

		std::string strType;
		if ( spConstraint->IsValid() )
			strType = spConstraint->GetContextType();
		else {
			strType = spConstraint->GetFullName();
			auto iPos = strType.rfind( ":" );
			strType = strType.substr( 0, iPos - 1 );
		}

		if ( spConstraint->IsValid() ) {
			ConstraintVectorMap::iterator it = m_mapConstraints.find( strType );
			for ( OclGme::ConstraintVector::iterator vit = (*it).second.begin() ; vit != (*it).second.end() ; ++vit ) {
				if ( (*vit)->GetFullName() == spConstraint->GetFullName() ) {
					(*it).second.erase( vit );
					break;
				}
			}
		}

		// Remove it from Registry

		CComPtr<IMgaFolder> spRootFolder;
		COMTHROW( m_pFacade->GetProject()->get_RootFolder( &spRootFolder ) );
		CComPtr<IMgaRegNode> spRegNode;
		COMTHROW( spRootFolder->get_RegistryNode( CComBSTR( L"ConstraintDefinitions/" + OclCommonEx::Convert( spConstraint->GetFullName() ) ), &spRegNode ) );
		COMTHROW( spRegNode->RemoveTree() );

		// Remove Enabling flags from objects' registry

		if ( bRemoveEnableInfo ) {
			OclCommonEx::ObjectVector vecObjects;
			OclCommonEx::GetKindObjects( m_pFacade->GetProject(), strType.substr( 6 ), vecObjects );
			for ( unsigned int i = 0 ; i < vecObjects.size() ; i++ ) {
				if ( OclCommonEx::GetLibraryPath( vecObjects[ i ].p ).empty() ) {
					CComPtr<IMgaRegNode> spRegNode;
					CComQIPtr<IMgaFolder> spFolder = vecObjects[ i ].p;
					if ( spFolder.p )
						COMTHROW( spFolder->get_RegistryNode( CComBSTR( OclCommonEx::Convert( "ConstraintEnabling/" + spConstraint->GetFullName() ) ), &spRegNode ) );
					else {
						CComQIPtr<IMgaFCO> spFCO = vecObjects[ i ].p;
						COMTHROW( spFCO->get_RegistryNode( CComBSTR( OclCommonEx::Convert( "ConstraintEnabling/" + spConstraint->GetFullName() ) ), &spRegNode ) );
					}
					COMTHROW( spRegNode->RemoveTree() );
				}
			}

			// Refresh Kind
		}

		RefreshKind( strType.substr( 6 ) );
	}

//##############################################################################################################################################
//
//	C L A S S : CConstraintBrowserDialog <<< + CDialog
//
//##############################################################################################################################################

CSettingsPage::CSettingsPage( CWnd* pParent /*=NULL*/ )
	: CDialog( CSettingsPage::IDD, pParent )
{
	//{{AFX_DATA_INIT(CSettingsPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CSettingsPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSettingsPage)
	DDX_Control(pDX, BSEP_EDTVIOLCOUNT, m_edtViolationCount);
	DDX_Control(pDX, BSEP_CHKTRACKING, m_chkEnableTracking);
	DDX_Control(pDX, BSEP_CHKSC_LOGICAL, m_chkEnableLogical);
	DDX_Control(pDX, BSEP_CHKSC_ITERATOR, m_chkEnableIterator);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSettingsPage, CDialog)
	//{{AFX_MSG_MAP(CSettingsPage)
	ON_BN_CLICKED(BSEP_RDVIOLFIRST, OnRadioButtonClicked)
	ON_EN_CHANGE(BSEP_EDTVIOLCOUNT, OnViolationCountChanged)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(BSEP_RDVIOLCOUNT, OnRadioButtonClicked)
	ON_BN_CLICKED(BSEP_RDLEVELFIRST, OnRadioButtonClicked)
	ON_BN_CLICKED(BSEP_RDDEFAULT, OnRadioButtonClicked)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSettingsPage message handlers

BOOL CSettingsPage::PreTranslateMessage(MSG* pMsg)
{
	if ( pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE )
		return GetParent()->PreTranslateMessage( pMsg );
	return CDialog::PreTranslateMessage( pMsg );
}

void CSettingsPage::OnRadioButtonClicked()
{
	m_edtViolationCount.SetReadOnly( ! ( (CButton*) GetDlgItem( BSEP_RDVIOLCOUNT ) )->GetCheck() );
}

void CSettingsPage::OnViolationCountChanged()
{
	CString strCount;
	m_edtViolationCount.GetWindowText( strCount );
	if ( strCount.IsEmpty() )
		m_edtViolationCount.SetWindowText( _T("2") );
	else {
		int iCount = _ttoi( strCount );
		if ( iCount == 0 )
			m_edtViolationCount.SetWindowText( _T("2") );
		if ( iCount > 999 )
			m_edtViolationCount.SetWindowText( _T("999") );
	}
}

}; // namespace OclGmeCM
