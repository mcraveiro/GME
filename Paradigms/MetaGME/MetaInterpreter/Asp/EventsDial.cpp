// EventsDial.cpp : implementation file
//

#include "stdafx.h"
#include "EventsDial.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*static*/ int EventsDial::m_valCreated			= 0x80000000;
/*static*/ int EventsDial::m_valDestroyed		= 0x40000000;
/*static*/ int EventsDial::m_valAttr			= 0x00000001;
/*static*/ int EventsDial::m_valRegistry		= 0x00000002;//not exposed to the user
/*static*/ int EventsDial::m_valNewchild		= 0x00000004;
/*static*/ int EventsDial::m_valRelation		= 0x00000008;//ref pointer, set member, conn endpoint change
/*static*/ int EventsDial::m_valProperties		= 0x00000010;
/*static*/ int EventsDial::m_valSubtinst		= 0x00000020;
/*static*/ int EventsDial::m_valBase			= 0x00000040;//obsolete (based on mga.idl)
/*static*/ int EventsDial::m_valPosition		= 0x00000080;//obsolete (based on mga.idl)
/*static*/ int EventsDial::m_valParent			= 0x00000100;
/*static*/ int EventsDial::m_valLostchild		= 0x00000200;
/*static*/ int EventsDial::m_valReferenced		= 0x00000400;
/*static*/ int EventsDial::m_valConnected		= 0x00000800;
/*static*/ int EventsDial::m_valSetIncluded		= 0x00001000;
/*static*/ int EventsDial::m_valRefReleased		= 0x00002000;
/*static*/ int EventsDial::m_valDisconnected	= 0x00004000;
/*static*/ int EventsDial::m_valSetExcluded		= 0x00008000;
/*static*/ int EventsDial::m_valUserBits		= 0x00FF0000;//not exposed to the user
/*static*/ int EventsDial::m_valCloseModel		= 0x00800000;

/////////////////////////////////////////////////////////////////////////////
// EventsDial dialog

EventsDial::EventsDial(CWnd* pParent /*=NULL*/)
	: CDialog(EventsDial::IDD, pParent)
	, m_sumAll( 0)

{
	//{{AFX_DATA_INIT(EventsDial)
	m_chkCreated = FALSE;
	m_chkDestroyed = FALSE;
	m_chkAttr = FALSE;
	m_chkNewchild = FALSE;
	m_chkRelation = FALSE;
	m_chkProperties = FALSE;
	m_chkSubtinst = FALSE;
	m_chkParent = FALSE;
	m_chkLostchild = FALSE;
	m_chkReferenced = FALSE;
	m_chkConnected = FALSE;
	m_chkIncludeInSet = FALSE;
	m_chkDisconnect = FALSE;
	m_chkUnrefer = FALSE;
	m_chkExcludeFromSet = FALSE;
	m_chkCloseModel = FALSE;
	//}}AFX_DATA_INIT
}


void EventsDial::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(EventsDial)
	DDX_Check(pDX, IDC_CHECK1, m_chkCreated);
	DDX_Check(pDX, IDC_CHECK2, m_chkDestroyed);
	DDX_Check(pDX, IDC_CHECK3, m_chkAttr);
	DDX_Check(pDX, IDC_CHECK5, m_chkNewchild);
	DDX_Check(pDX, IDC_CHECK6, m_chkRelation);
	DDX_Check(pDX, IDC_CHECK7, m_chkProperties);
	DDX_Check(pDX, IDC_CHECK8, m_chkSubtinst);
	DDX_Check(pDX, IDC_CHECK11, m_chkParent);
	DDX_Check(pDX, IDC_CHECK12, m_chkLostchild);
	DDX_Check(pDX, IDC_CHECK13, m_chkReferenced);
	DDX_Check(pDX, IDC_CHECK14, m_chkConnected);
	DDX_Check(pDX, IDC_CHECK16, m_chkIncludeInSet);
	DDX_Check(pDX, IDC_CHECK17, m_chkDisconnect);
	DDX_Check(pDX, IDC_CHECK18, m_chkUnrefer);
	DDX_Check(pDX, IDC_CHECK19, m_chkExcludeFromSet);
	DDX_Check(pDX, IDC_CHECK15, m_chkCloseModel);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(EventsDial, CDialog)
	//{{AFX_MSG_MAP(EventsDial)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// EventsDial message handlers

BOOL EventsDial::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_chkCreated		= ( m_sumAll & m_valCreated) != 0;
	m_chkDestroyed		= ( m_sumAll & m_valDestroyed) != 0;
	m_chkAttr			= ( m_sumAll & m_valAttr) != 0;
	//m_chkRegistry		= ( m_sumAll & m_valRegistry) != 0;
	m_chkNewchild		= ( m_sumAll & m_valNewchild) != 0;
	m_chkRelation		= ( m_sumAll & m_valRelation) != 0;
	m_chkProperties		= ( m_sumAll & m_valProperties) != 0;
	m_chkSubtinst		= ( m_sumAll & m_valSubtinst) != 0;
	//m_chkBase			= ( m_sumAll & m_valBase) != 0;
	//m_chkPosition		= ( m_sumAll & m_valPosition) != 0;
	m_chkParent			= ( m_sumAll & m_valParent) != 0;
	m_chkLostchild		= ( m_sumAll & m_valLostchild) != 0;
	m_chkReferenced		= ( m_sumAll & m_valReferenced) != 0;
	m_chkConnected		= ( m_sumAll & m_valConnected) != 0;
	m_chkIncludeInSet	= ( m_sumAll & m_valSetIncluded ) != 0;
	m_chkUnrefer		= ( m_sumAll & m_valRefReleased ) != 0;
	m_chkDisconnect		= ( m_sumAll & m_valDisconnected ) != 0;
	m_chkExcludeFromSet	= ( m_sumAll & m_valSetExcluded ) != 0;
	m_chkCloseModel		= ( m_sumAll & m_valCloseModel) != 0;

	UpdateData( FALSE);

	return TRUE;
}

void EventsDial::OnOK() 
{
	UpdateData( TRUE);

	m_sumAll = 0;

	if( m_chkCreated)			m_sumAll |= m_valCreated;
	if( m_chkDestroyed)			m_sumAll |= m_valDestroyed;
	if( m_chkAttr)				m_sumAll |= m_valAttr;
	//if( m_chkRegistry)		m_sumAll |= m_valRegistry;
	if( m_chkNewchild)			m_sumAll |= m_valNewchild;
	if( m_chkRelation)			m_sumAll |= m_valRelation;
	if( m_chkProperties)		m_sumAll |= m_valProperties;
	if( m_chkSubtinst)			m_sumAll |= m_valSubtinst;
	//if( m_chkBase)			m_sumAll |= m_valBase;
	//if( m_chkPosition)		m_sumAll |= m_valPosition;
	if( m_chkParent)			m_sumAll |= m_valParent;
	if( m_chkLostchild)			m_sumAll |= m_valLostchild;
	if( m_chkReferenced)		m_sumAll |= m_valReferenced;
	if( m_chkConnected)			m_sumAll |= m_valConnected;
	if( m_chkIncludeInSet)		m_sumAll |= m_valSetIncluded;
	if( m_chkUnrefer)			m_sumAll |= m_valRefReleased;
	if( m_chkDisconnect)		m_sumAll |= m_valDisconnected;
	if( m_chkExcludeFromSet)	m_sumAll |= m_valSetExcluded;
	if( m_chkCloseModel)		m_sumAll |= m_valCloseModel;
	
	CDialog::OnOK();
}
