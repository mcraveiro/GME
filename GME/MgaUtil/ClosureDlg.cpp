// ClosureDlg.cpp : implementation file
//

#include "stdafx.h"
#include "gme.h"
#include "ClosureDlg.h"
#include "ClosureKindSel.h"
#include <afxcmn.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CClosureDlg dialog


CClosureDlg::CClosureDlg(CWnd* pParent /*=NULL*/)
	//: CPropertyPage(CClosureDlg::IDD)
	: CDialog(CClosureDlg::IDD, pParent)
	, m_activeFolderContainment( true)
{
	//{{AFX_DATA_INIT(CClosureDlg)
	m_bConnection = FALSE;
	m_bContainment = FALSE;
	m_bFolderContainment = FALSE;
	m_bRefersTo = FALSE;
	m_bSetMember = FALSE;
	m_bMemberOfSets = FALSE;
	m_bReferredBy = FALSE;
	m_bBaseTypes = FALSE;
	m_bDerivedTypes = FALSE;
	m_bPartOfModels = FALSE;
	m_bPartOfFolders = FALSE;
	m_strOutputFile = _T("");
	m_outputOption = -1;
	m_direction = -1;
	m_wrappingOption = -1;
	m_libraryHandling = -1;
	m_includeAllKindsRadio = -1;
	m_bRefPortConnection = FALSE;
	m_bMarkObjs = FALSE;
	//}}AFX_DATA_INIT
	m_iMarkVal = -1;
}


void CClosureDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CClosureDlg)
	DDX_Control(pDX, IDC_FILTER, m_ctrlFilter);
	DDX_Control(pDX, IDC_CHECKREFPORTCONNECTION, m_ctrlRefPortConnection);
	DDX_Control(pDX, IDC_INCLUDEALL, m_ctrlIncludeAll);
	DDX_Control(pDX, IDC_INCLUDESOME, m_ctrlIncludeSome);
	DDX_Control(pDX, IDC_WRAPMINIPROJ, m_ctrlRadio1);
	DDX_Control(pDX, IDC_WRAPAUTO, m_ctrlRadio2);
	DDX_Control(pDX, IDC_WRAPASIS, m_ctrlRadio3);
	DDX_Control(pDX, IDC_LIBR1, m_ctrlLibRadio1);
	DDX_Control(pDX, IDC_LIBR2, m_ctrlLibRadio2);
	DDX_Control(pDX, IDC_LIBR3, m_ctrlLibRadio3);
	DDX_Control(pDX, IDC_CHECKPARTOFMODEL, m_ctrlPartOfModels);
	DDX_Control(pDX, IDC_CHECKPARTOFFOLDERS, m_ctrlPartOfFolders);
	DDX_Control(pDX, IDC_CHECKCONTAINMENT, m_ctrlContainment);
	DDX_Control(pDX, IDOK, m_ctrlOk);
	DDX_Control(pDX, IDC_BROWSE, m_ctrlBrowse);
	DDX_Control(pDX, IDC_EDIT1, m_ctrlOutputFile);
	DDX_Control(pDX, IDC_CHECKFOLDERCONTAINMENT, m_ctrlFolderContainment);
	DDX_Check(pDX, IDC_CHECKCONNECTION, m_bConnection);
	DDX_Check(pDX, IDC_CHECKCONTAINMENT, m_bContainment);
	DDX_Check(pDX, IDC_CHECKFOLDERCONTAINMENT, m_bFolderContainment);
	DDX_Check(pDX, IDC_CHECKREFERSTO, m_bRefersTo);
	DDX_Check(pDX, IDC_CHECKSETMEMBER, m_bSetMember);
	DDX_Check(pDX, IDC_CHECKMEMBEROF, m_bMemberOfSets);
	DDX_Check(pDX, IDC_CHECKREFERREDBY, m_bReferredBy);
	DDX_Check(pDX, IDC_CHECKBASETYPES, m_bBaseTypes);
	DDX_Check(pDX, IDC_CHECKSUBTYPES, m_bDerivedTypes);
	DDX_Check(pDX, IDC_CHECKPARTOFMODEL, m_bPartOfModels);
	DDX_Check(pDX, IDC_CHECKPARTOFFOLDERS, m_bPartOfFolders);
	DDX_Text(pDX, IDC_EDIT1, m_strOutputFile);
	DDX_Radio(pDX, IDC_OUTPUTTOCLIPBOARD, m_outputOption);
	DDX_Radio(pDX, IDC_DIRECTIONUP, m_direction);
	DDX_Radio(pDX, IDC_WRAPMINIPROJ, m_wrappingOption);
	DDX_Radio(pDX, IDC_LIBR1, m_libraryHandling);
	DDX_Radio(pDX, IDC_INCLUDEALL, m_includeAllKindsRadio);
	DDX_Check(pDX, IDC_CHECKREFPORTCONNECTION, m_bRefPortConnection);
	DDX_Check(pDX, IDC_CHECKMARK, m_bMarkObjs);
	DDX_Control(pDX, IDC_EDITMARK, m_ctrlMarkVal);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CClosureDlg, CDialog)
	//{{AFX_MSG_MAP(CClosureDlg)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_BN_CLICKED(IDC_OUTPUTTOCLIPBOARD, OnOutputtoclipboard)
	ON_BN_CLICKED(IDC_OUTPUTTOFILE, OnOutputtofile)
	ON_BN_CLICKED(IDC_DIRECTIONUP, OnDirectionup)
	ON_BN_CLICKED(IDC_DIRECTIONDN, OnDirectiondn)
	ON_BN_CLICKED(IDC_CHECKCONNECTION, OnCheckconnection)
	ON_BN_CLICKED(IDC_CHECKFOLDERCONTAINMENT, OnCheckfoldercontainment)
	ON_BN_CLICKED(IDC_CHECKCONTAINMENT, OnCheckcontainment)
	ON_BN_CLICKED(IDC_FILTER, OnFilter)
	ON_BN_CLICKED(IDC_INCLUDESOME, OnIncludeSome)
	ON_BN_CLICKED(IDC_INCLUDEALL, OnIncludeAll)
	ON_BN_CLICKED(IDC_WRAPMINIPROJ, OnWrapMiniProj)
	ON_BN_CLICKED(IDC_WRAPAUTO, OnWrapAuto)
	ON_BN_CLICKED(IDC_WRAPASIS, OnWrapAsIs)
	ON_BN_CLICKED(IDC_CHECKMARK, OnMarkClicked)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClosureDlg message handlers

BOOL CClosureDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_wrappingOption = 1;	// automatic
	m_outputOption = 0;		// clipboard
	m_direction = 1;		// downward
	m_libraryHandling = 0;	// stubs
	m_includeAllKindsRadio = 0;	// all kind preferred over the "some kinds" only
	m_includeFolders = 1;	// def values for the kind selector dialog
	m_includeModels = 1;
	m_includeAtoms = 1;
	m_includeSets = 0;
	m_includeReferences = 0;


	// initially use the value of the activeFolderCntm since the direction is down
	m_ctrlFolderContainment.EnableWindow( m_activeFolderContainment);

	// direction is down
	m_ctrlPartOfFolders.EnableWindow( FALSE);
	m_ctrlPartOfModels.EnableWindow( FALSE);

	m_strOutputFile = _T("Closure.xme");

	m_ctrlOutputFile.EnableWindow( m_outputOption);
	m_ctrlBrowse.EnableWindow( m_outputOption);

	m_ctrlIncludeAll.EnableWindow(m_bContainment || m_bFolderContainment);
	m_ctrlIncludeSome.EnableWindow(m_bContainment || m_bFolderContainment);
	m_ctrlFilter.EnableWindow( m_includeAllKindsRadio == 0?FALSE:TRUE);

	m_ctrlRefPortConnection.EnableWindow( m_bConnection);

	m_ctrlMarkVal.EnableWindow( m_bMarkObjs);
	if( m_iMarkVal != -1)
	{
		TCHAR buf[12];
		_stprintf_s( buf, _T("%i"), m_iMarkVal);
		m_ctrlMarkVal.SetWindowText( buf);
	}

	//m_bConnection = TRUE;
	//m_bContainment = TRUE;
	//m_bFolderContainment = m_activeFolderContainment;

	//m_bRefersTo = TRUE;
	//m_bReferredBy = TRUE;

	//m_bSetMember = TRUE;
	//m_bMemberOfSets = TRUE;

	//m_bPartOfModels = FALSE;
	//m_bPartOfFolders = FALSE;

	//m_bDigIntoLibraries = TRUE;
	
	//m_bBaseTypes = TRUE;
	//m_bDerivedTypes = TRUE;

	UpdateData( FALSE);

	return TRUE;
}

void CClosureDlg::OnOK() 
{
	TCHAR buf[12];
	m_ctrlMarkVal.GetWindowText( buf, 11);

	if( 1 != _stscanf( buf, _T("%i"), &m_iMarkVal))
		m_iMarkVal = -1;

	CDialog::OnOK();
}


void CClosureDlg::OnBrowse() 
{
	UpdateData( TRUE);
	CWaitCursor wait;
	CFileDialog dlg(FALSE, _T("xme"), _T("Closure.xme"),
		OFN_EXPLORER | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("GME Exported Files (*.xme;*.xml)|*.xme; *.xml|All Files (*.*)|*.*||"));
	if( dlg.DoModal() == IDOK) 
	{
		m_strOutputFile = dlg.GetPathName();
		this->UpdateData( FALSE);
	}
}

void CClosureDlg::OnOutputtoclipboard() 
{
	UpdateData( TRUE);
	
	m_ctrlOutputFile.EnableWindow( m_outputOption);
	m_ctrlBrowse.EnableWindow( m_outputOption);
}

void CClosureDlg::OnOutputtofile() 
{
	OnOutputtoclipboard();
}

void CClosureDlg::OnDirectionup() 
{
	UpdateData( TRUE);
	// if the closure direction is up: the folder containment should be allowed
	m_ctrlFolderContainment.EnableWindow( FALSE);	
	m_ctrlContainment.EnableWindow( FALSE);
	m_ctrlPartOfFolders.EnableWindow( TRUE);
	m_ctrlPartOfModels.EnableWindow( TRUE);

	m_ctrlIncludeAll.EnableWindow( FALSE);
	m_ctrlIncludeSome.EnableWindow( FALSE);
	m_ctrlFilter.EnableWindow( FALSE);

	UpdateData( FALSE);
}


void CClosureDlg::OnDirectiondn() 
{
	UpdateData( TRUE);
	// if the closure direction is down: the folder containment should be set
	// according to the variable which indicates if there is any folder involved
	m_ctrlFolderContainment.EnableWindow( m_activeFolderContainment);
	m_ctrlContainment.EnableWindow( TRUE);
	m_ctrlPartOfFolders.EnableWindow( FALSE);
	m_ctrlPartOfModels.EnableWindow( FALSE);

	m_ctrlIncludeAll.EnableWindow(m_bContainment || m_bFolderContainment);
	m_ctrlIncludeSome.EnableWindow(m_bContainment || m_bFolderContainment);
	m_ctrlFilter.EnableWindow( m_includeAllKindsRadio == 0?FALSE:TRUE);
}

void CClosureDlg::OnCheckconnection() 
{
	UpdateData( TRUE);

	m_bRefPortConnection = m_bConnection?m_bRefPortConnection:FALSE;

	m_ctrlRefPortConnection.EnableWindow( m_bConnection);

	UpdateData( FALSE);
}

void CClosureDlg::OnCheckfoldercontainment() 
{
	OnCheckcontainment();
}

void CClosureDlg::OnCheckcontainment() 
{
	UpdateData( TRUE);
	
	m_ctrlIncludeAll.EnableWindow(m_bContainment || m_bFolderContainment);
	m_ctrlIncludeSome.EnableWindow(m_bContainment || m_bFolderContainment);
	m_ctrlFilter.EnableWindow( m_includeAllKindsRadio == 0?FALSE:TRUE);
}

void CClosureDlg::OnFilter() 
{
	CClosureKindSel sel( 
		  m_includeFolders
		, m_includeModels
		, m_includeAtoms
		, m_includeSets
		, m_includeReferences
		, 0);

	if( sel.DoModal() == IDOK)
	{
		m_includeFolders = sel.m_folder == TRUE;
		m_includeModels = sel.m_model == TRUE;
		m_includeAtoms = sel.m_atom == TRUE;
		m_includeSets = sel.m_set == TRUE;
		m_includeReferences = sel.m_ref == TRUE;
	}
}

void CClosureDlg::OnIncludeAll() 
{
	m_ctrlFilter.EnableWindow( FALSE);
}

void CClosureDlg::OnIncludeSome() 
{
	m_ctrlFilter.EnableWindow( TRUE);
}


void CClosureDlg::OnWrapMiniProj() 
{
	UpdateData( TRUE);
	m_ctrlLibRadio1.EnableWindow( m_wrappingOption != 0);
	m_ctrlLibRadio2.EnableWindow( m_wrappingOption != 0);
	m_ctrlLibRadio3.EnableWindow( TRUE);
	if( m_wrappingOption == 0) m_libraryHandling = 2;
	UpdateData( FALSE);	
}

void CClosureDlg::OnWrapAuto() 
{
	OnWrapMiniProj();
}

void CClosureDlg::OnWrapAsIs() 
{
	OnWrapMiniProj();
}

long CClosureDlg::calcKindMask()
{
	long kind_map = 0;//important to set it to 0
	if( m_includeAllKindsRadio == 0)
		kind_map = -1;//0xffffffff;
	else
		kind_map = kind_map
			| (m_includeFolders?0x1:0)
			| (m_includeModels?0x2:0)
			| (m_includeAtoms?0x4:0)
			| (m_includeSets?0x8:0)
			| (m_includeReferences?0x10:0);

	return kind_map;
}

void CClosureDlg::OnMarkClicked()
{
	UpdateData( TRUE);
	m_ctrlMarkVal.EnableWindow( m_bMarkObjs);
	UpdateData( FALSE);	

}
