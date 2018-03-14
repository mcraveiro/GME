// SelConf.cpp : implementation file
//

#include "stdafx.h"
#include "SelConf.h"
#include "Any.h"
#include "FCO.h"
#include ".\selconf.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SelConf dialog
/*static*/ int SelConf::m_lastId = 0;
/*static*/ int SelConf::m_sortOrder = 1;

SelConf::SelConf( unsigned int no_of_configs, CWnd* pParent /*=NULL*/)
	: CDialog(SelConf::IDD, pParent),
		m_numberOfConfigs( no_of_configs)
{
	//{{AFX_DATA_INIT(SelConf)
	//}}AFX_DATA_INIT

}

SelConf::~SelConf()
{
	POSITION pos = m_entries.GetHeadPosition();
	while (pos) {
		delete m_entries.GetNext(pos);
	}
	m_entries.RemoveAll(); 
}

void SelConf::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SelConf)
	DDX_Control(pDX, IDC_COMBO1, m_config);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SelConf, CDialog)
	//{{AFX_MSG_MAP(SelConf)
	ON_CBN_EDITUPDATE(IDC_COMBO1, OnEditupdateCombo1)
	ON_CBN_CLOSEUP(IDC_COMBO1, OnCloseupCombo1)
	ON_BN_CLICKED(IDC_DESELECTALLBTN, OnBnClickedDeselectallbtn)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_SELECTALLBTN, OnBnClickedSelectallbtn)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SelConf message handlers

void SelConf::addConfigs( std::vector< std::string >& configs)
{
	ASSERT( configs.size() == m_numberOfConfigs);
	m_configStrings = configs;
}

int SelConf::findAmongConfigs( std::string& config_name)
{
	for( unsigned int i = 0; i < m_configStrings.size(); ++i)
	{
		if ( m_configStrings[i] == config_name)
			return (int) i;
	}
	return -1;
}


int SelConf::addEntry( const CString& role, char kind, int clique, bool in, const CString& repr, const void * ptr)
{
	entry *f = new entry;
	f->id = m_lastId;
	f->s = role;
	f->kind = kind;
	f->cliqueId = clique;
	f->val = in;
	f->resp = repr;
	f->ptr = ptr;

	m_entries.AddTail( f);
	return m_lastId++;
}

bool SelConf::getEntry( int row, CString& role, bool & in, CString& repr, const void * &ptr)
{
	bool retval = false;

	POSITION pos = m_entries.GetHeadPosition();
	while (pos) {
		entry	*e = m_entries.GetNext(pos);
		if (e->id == row) {
			role = e->s;
			in = e->val;
			ptr = e->ptr;
			repr = e->resp;
			retval = true;
		}
	}
	return retval;

}

BOOL SelConf::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CRect loc;
	this->GetClientRect(&loc);
	loc.DeflateRect(15, 50, 15, 50);

	m_table.Create(WS_CHILD|WS_VISIBLE|WS_BORDER/*|LVS_EDITLABELS*/|LVS_REPORT|LVS_SINGLESEL, loc, this, 1);
	m_table.m_parent = this;

	fillUpTable();

	unsigned int len = m_configStrings.size();
	for( unsigned int i = 0; i < len; ++i)
	{
		m_config.AddString( m_configStrings[i].c_str());
		m_config.SetItemData( i, i);
	}

	int nCount = m_config.GetCount();
	if (nCount > 0)
	{
    //m_config.SetCurSel(default_item_data);
		m_config.SelectString( -1, "Default");
		m_currSelConfigStr = "Default";
	}

	// TODO: Add extra initialization here
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void SelConf::OnOK() 
{
	// TODO: Add extra validation here
	if ( m_currSelConfigStr.empty() || m_currSelConfigStr.find_first_not_of(' ') == std::string::npos )
	{
		AfxMessageBox( "Please select a valid configuration name");
		return;
	}

	getDataFromTable();
	saveUserPref( false);

	/*CString temp_text;
	m_config.GetWindowText( temp_text);
	m_currSelConfigStr = (LPCTSTR) temp_text;

	int sel = m_config.GetCurSel();
	if ( sel != CB_ERR && sel < m_config.GetCount() && sel > 0)
		m_currSelConfigStr = m_configStrings[sel];*/

	CDialog::OnOK();
}

void SelConf::OnCloseupCombo1() 
{
	// TODO: Add your control notification handler code here
	int sel = m_config.GetCurSel();
	if ( sel != CB_ERR && sel < m_config.GetCount() && sel >= 0)
	{
		if( m_currSelConfigStr == m_configStrings[sel]) // upon first dropdown the else branch is selected anyway
			return;
		else
		{
			getDataFromTable();
			saveUserPref( true); // save user pref with the old config name
			m_currSelConfigStr = m_configStrings[sel]; // set the new config name
			refreshTable();
		}
	}
}

void SelConf::OnEditupdateCombo1() 
{
	// TODO: Add your control notification handler code here
	CString temp_text;
	m_config.GetWindowText( temp_text);
	m_currSelConfigStr = (LPCTSTR) temp_text;	
}


void SelConf::refreshTable()
{
	std::string regPath = "/Configs/" + m_currSelConfigStr, regVal;
	int k = findAmongConfigs (m_currSelConfigStr);
	if ( k != -1)
	{
		m_table.DeleteAllItems();

		POSITION pos = m_entries.GetHeadPosition();
		while (pos)
		{
			entry	*e = m_entries.GetNext(pos);
			Any * any_ptr = const_cast<Any *>( static_cast<const Any *>(e->ptr));
			if ( any_ptr)
			{
					regVal = any_ptr->getMyRegistry()->getValueByPath( regPath);
					if ( regVal == "false")
						e->val = false;
					else
						e->val = true;//m_table.addRow(e->id, e->s, e->cliqueId, true);
			}
			/*else
				m_table.addRow(e->id, e->s, -1, true);*/
		}
		fillUpTable();
		UpdateWindow();
	}
}

/*
Called with true usually
			 with false finally (OnOk)
*/
void SelConf::saveUserPref( bool check)
{
	int k = findAmongConfigs( m_currSelConfigStr);
	if ( check && k == -1) return;
	
	std::string regVal, regPath = "/Configs/" + m_currSelConfigStr;
	POSITION pos = m_entries.GetHeadPosition();
	while (pos)
	{
		entry	*e = m_entries.GetNext(pos);
		Any * any_ptr = const_cast<Any *>( static_cast<const Any *>(e->ptr));
		if ( any_ptr)
		{
			if ( !check) // if called from OnOk
			{
				any_ptr->toBeEx( e->val);
				FCO * fco_ptr = dynamic_cast<FCO*>( any_ptr);
				if( any_ptr->isFCO() && fco_ptr)
				{
					// stores into the ExtedAnc var the selected responsible
					fco_ptr->setExtedAnc( fco_ptr->findRspPtr( (LPCTSTR) e->resp));
				}
			}
			if (e->val)	regVal = "true"; else regVal = "false";
			any_ptr->getMyRegistry()->setValueByPath( regPath, regVal);
			any_ptr->getMyRegistry()->setValueByPath( regPath + "/Resp", (LPCTSTR) e->resp);
		}
	}
}

void SelConf::fillUpTable()
{
	POSITION pos = m_entries.GetHeadPosition();
	while (pos) {
		entry	*e = m_entries.GetNext(pos);
		m_table.addRow(e->id, e->s, e->kind, e->cliqueId, e->val, e->resp);
	}
}

void SelConf::getDataFromTable()
{
	POSITION pos = m_entries.GetHeadPosition();
	while (pos) {
		entry	*e = m_entries.GetNext(pos);
		m_table.getRow( e->id, e->s, e->val, e->resp);
	}
}

// appends those class names to list which are samekind extended ancestors
// of the fco identified by 'changed'
bool SelConf::addPossibleAncestors( CStringList& list, int changed)
{
	POSITION pos = m_entries.GetHeadPosition();
	entry	*e = 0;
	while (pos) {
		e = m_entries.GetNext(pos);
		if ( e->id == changed)
			break;
	}

	if ( e && e->id == changed)
	{
		Any * any_ptr = const_cast<Any *>( static_cast<const Any *>(e->ptr));
		if ( any_ptr && any_ptr->isFCO())
		{
			FCO * fco_ptr = dynamic_cast<FCO*>( any_ptr);
			if( fco_ptr)
			{
				std::vector<FCO *> ancestor;
				bool found_same_kind_extd_anc = false; // notifies that extended ancestors of the same kind have been found
				fco_ptr->getIntAncestors( ancestor);
				
				for( unsigned int i = 0; i < ancestor.size(); ++i)
				{
					// check whether ancestor[i] is to be extended
					POSITION pos_d = m_entries.GetHeadPosition();
					entry	*e_d = 0;
					while (pos_d) {
						e_d = m_entries.GetNext(pos_d);
						if ( e_d->ptr == ancestor[i])
						{
							CString role, repr; bool extended = false;
							if( m_table.getRow( e_d->id, role, extended, repr))
							{
								if( extended                                          // it is extended
								  && fco_ptr->getMyKind() == ancestor[i]->getMyKind() // it is the same kind that fco_ptr
								  )
								{
									list.AddTail( ancestor[i]->getLStrictName().c_str());    // put into the option
									found_same_kind_extd_anc = true;
								}
							}
							break; // end the cycle
						}
					}
				}
				
				return found_same_kind_extd_anc; // will notify the user if additional options have been added at all
			}
		}
	}
	return false;
}

// which: true if the user selected NO, false if the user selected YES
void SelConf::getDescsAncs( bool which, DWORD_PTR changed, std::vector< int > & res)
{
	POSITION pos = m_entries.GetHeadPosition();
	entry	*e = 0;
	while (pos) {
		e = m_entries.GetNext(pos);
		if ( e->id == changed)
			break;
	}

	if ( e && e->id == changed)
	{
		Any * any_ptr = const_cast<Any *>( static_cast<const Any *>(e->ptr));
		if ( any_ptr && any_ptr->isFCO())
		{
			FCO * fco_ptr = dynamic_cast<FCO*>( any_ptr);
			if( fco_ptr)
			{
				std::vector<FCO *> family;
				if ( which) // means 'no' is selected
				{
					/*if ( fco_ptr->getMyKind() == Any::FCO_REP || !fco_ptr->hasParentOfSameKind()) //if fco_rep is selected for 'no' means no other elements in the hierarchy needed to be extended or if has _not_ the same kind of ancestor means doesn't inherit from a same kind object (inherits from FCO)
						fco_ptr->getAllInMyHierarchy( family);
					else*/
						family = fco_ptr->getAllDescendants();
				}
				else // means 'yes'
				{
					family = fco_ptr->getAllAncestors(); // since fco_rep may exist among the ancestors and that will become yes, we have to make yes all fcoreps and the first nonfcorep leaves
					//fco_ptr->getRootDescsUntilNonFcoRep( family); // get more dependants
				}

				for( unsigned int i = 0; i < family.size(); ++i)
				{
					FCO * dep_ptr = family[i]; // dependant
					POSITION pos_d = m_entries.GetHeadPosition();
					entry	*e_d = 0;
					while (pos_d) {
						e_d = m_entries.GetNext(pos_d);
						if ( e_d->ptr == dep_ptr)
						{
							res.push_back( e_d->id);
							break;
						}
					}
				}
			}
		}
	}
}

/*static*/ bool SelConf::getEntries( LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort, entry& e, entry& f)
{
	SelConf * myDlg = reinterpret_cast<SelConf *>((void*)lParamSort);
	bool b1 = myDlg->getEntry( lParam1, e.s, e.val, e.resp, e.ptr); 
	bool b2 = myDlg->getEntry( lParam2, f.s, f.val, f.resp, f.ptr); 
	
	return b1 && b2;
}

/*static*/ int CALLBACK SelConf::MyNameCompare(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	entry e, f;
	if( !getEntries( lParam1, lParam2, lParamSort, e, f)) return 0;

	return m_sortOrder * e.s.Compare( f.s);
}

/*static*/ int CALLBACK SelConf::MyKindCompare(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	entry e, f;
	if( !getEntries( lParam1, lParam2, lParamSort, e, f)) return 0;

	Any * any_ptr1 = reinterpret_cast<Any *>((void*)e.ptr);
	Any * any_ptr2 = reinterpret_cast<Any *>((void*)f.ptr);

	std::string ks1 = any_ptr1->getMyKindStr();
	std::string ks2 = any_ptr2->getMyKindStr();

	return m_sortOrder * ks1.compare( ks2);
}

/*static*/ int CALLBACK SelConf::MyExtdCompare(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	entry e, f;
	if( !getEntries( lParam1, lParam2, lParamSort, e, f)) return 0;

	if( e.val == f.val) return 0;
	else return m_sortOrder * (int)e.val < m_sortOrder * (int)f.val;
}

/*static*/ int CALLBACK SelConf::MyReprCompare(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	entry e, f;
	if( !getEntries( lParam1, lParam2, lParamSort, e, f)) return 0;

	return m_sortOrder * e.resp.Compare( f.resp);
}


void SelConf::selectAll( bool pVal)
{
	m_table.DeleteAllItems();

	POSITION pos = m_entries.GetHeadPosition();
	while( pos) {
		entry *e = m_entries.GetNext(pos);
		e->val = pVal;
		e->resp = "";
	}
	fillUpTable();
}

void SelConf::OnBnClickedDeselectallbtn()
{
	selectAll( false);
}

void SelConf::OnBnClickedSelectallbtn()
{
	selectAll( true);
}
