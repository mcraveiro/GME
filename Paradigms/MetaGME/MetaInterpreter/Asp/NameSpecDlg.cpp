// AspectSpecDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NameSpecDlg.h"
#include "Any.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// NameSpecDlg

//IMPLEMENT_DYNAMIC(NameSpecDlg, CPropertySheet)

NameSpecDlg::NameSpecDlg(CWnd* pParent /*=NULL*/)
	: CDialog( NameSpecDlg::IDD, pParent)
	, m_lastID( 0)
{
	//{{AFX_DATA_INIT(NameSpecDlg)
	//}}AFX_DATA_INIT
}

NameSpecDlg::~NameSpecDlg()
{
}


BEGIN_MESSAGE_MAP(NameSpecDlg, CDialog)
	//{{AFX_MSG_MAP(NameSpecDlg)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// NameSpecDlg message handlers



void NameSpecDlg::GetEntry(int rowID, CString& name, CString& dispname, CString& kind)
{
	m_nmlist.GetRow(rowID, name, dispname, kind);
}

void uniqueNames( const CString& to_insert, CStringList& names)
{
	bool found = false;
	POSITION pos;
	pos = names.GetHeadPosition();
	while ( pos && !found)
	{
		if( to_insert.Compare(names.GetNext( pos )) == 0)
		{
			found = true;
		}
	}
	if( !found)
		names.InsertAfter( names.GetTailPosition(), to_insert);
}

void NameSpecDlg::GetNames( int num, CString& curr, CStringList& names)
{
	LARGE_MAP::iterator it = m_map.begin();
	for( int k = 0; k != num && it != m_map.end(); ++it, ++k);

	names.AddHead( curr);
	if( it != m_map.end()) // found
	{
		std::set< BON::FCO>::iterator jt = it->second.begin();
		for( ; jt != it->second.end(); ++jt)
		{
			CString onelem = (*jt)->getName().c_str();
			uniqueNames( onelem, names);

		}
	}
}

void NameSpecDlg::GetDispNames( int num, CString& currdispname, CStringList& dispnames)
{
	LARGE_MAP::iterator it = m_map.begin();
	for( int k = 0; k != num && it != m_map.end(); ++it, ++k);

	dispnames.AddHead( currdispname);
	if( it != m_map.end()) // found
	{
		std::set< BON::FCO>::iterator jt = it->second.begin();
		for( ; jt != it->second.end(); ++jt)
		{
			CString onelem;
			BON::Attribute attr = (*jt)->getAttribute( Any::DisplayedName_str);
			if( attr )
			{
				onelem = attr->getStringValue().c_str();
				uniqueNames( onelem, dispnames);
			}
		}
	}
	uniqueNames( "", dispnames); // thus allowing always the empty disp name possibility
}


BOOL NameSpecDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CRect loc;
	this->GetClientRect(&loc);
	loc.DeflateRect(10, 3*10, 10, 4*10);
	m_nmlist.Create(WS_CHILD|WS_VISIBLE|WS_BORDER/*|LVS_EDITLABELS*/|LVS_REPORT|LVS_SINGLESEL, loc, this, 1);
	
	DEFNAMES_MAP::iterator it = m_dn.begin();
	for( ; it != m_dn.end(); ++it)
	{
		BON::FCO ff( it->first);
		CString defname     = it->second.first.c_str();
		CString defdispname = it->second.second.c_str();
		CString kind = ff->getObjectMeta().name().c_str();


		//t
		//kind = CString(ff->getName().c_str()) + ":" + kind;
		//et

		m_nmlist.AddRow( m_lastID++, defname, defdispname, kind);
	}

	return TRUE;
}


void NameSpecDlg::OnOK() 
{
	DEFNAMES_MAP::iterator it = m_dn.begin();
	for( int k = 0; k < m_lastID && it != m_dn.end(); ++k, ++it)
	{
		CString name, dispname, kind; 
		GetEntry( k, name, dispname, kind);
		BON::FCO fco = it->first;
		std::string newsel_name = (LPCTSTR) name;
		std::string newsel_dispname = (LPCTSTR) dispname;
		m_result[ fco] = make_pair( newsel_name, newsel_dispname);

#ifdef _DEBUG
		//check
		std::set< BON::FCO>::iterator jt = m_map[ fco].begin();
		for( ; jt != m_map[ fco].end() && (*jt)->getName() != newsel_name; ++jt);

		if( jt == m_map[ fco].end() && newsel_name != m_dn[ fco].first)
			ASSERT(0); // if not found among equivs and is not the default name
#endif
	}

	CDialog::OnOK();
}
