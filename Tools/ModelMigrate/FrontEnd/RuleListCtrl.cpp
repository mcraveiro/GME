// RuleListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ModelMigrate.h"
#include "RuleListCtrl.h"
#include "NewRuleDlg.h"
#include "SimpleRepl.h"
#include "AttrNameDlg.h"
#include "AttrTypeChangeDlg.h"
#include "EnumAttrValueDlg.h"
#include "MoveUpElem.h"
#include "MoveDownElem.h"
#include "RemoveDlg.h"
#include "RemoveLocAttrDlg.h"
#include "StereotypeCastDlg.h"
#include "Script.h"
#include "Rule.h"
#include <fstream>
#include <algorithm>
#include "RuleEditorDlg.h"
// RuleListCtrl

#define EDIT_CMD               1501
#define DELESEL_CMD            1502
#define CLONE_CMD              1503
#define GENSEL_CMD             1504
#define GENALL_CMD             1505
#define MOVEUP_CMD             1506
#define MOVEDN_CMD             1507
#define ADD_CMD                1508
#define DELEALL_CMD            1509

// TODO: dependency with the other project in solution

IMPLEMENT_DYNAMIC(RuleListCtrl, CListCtrl)
RuleListCtrl::RuleListCtrl()
	: rowID( 0)
	, m_rules( 0)
{
}

RuleListCtrl::~RuleListCtrl()
{
	reset();
}


BEGIN_MESSAGE_MAP(RuleListCtrl, CListCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()


void RuleListCtrl::setParent( RuleEditorDlg* pDlg)
{ 
	m_parentDlg = pDlg; 
}

void RuleListCtrl::reset()
{
	m_rules.clear();
}


// RuleListCtrl message handlers


void RuleListCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	CListCtrl::OnLButtonDown(nFlags, point);
}

// HitTestEx	- Determine the row index and column index for a point
// Returns	- the row index or -1 if point is not over a row
// point	- point to be tested.
// col		- to hold the column index
int RuleListCtrl::HitTestEx(CPoint &point, int *col) const
{
	int colnum = 0;
	int row = HitTest( point, NULL );
	
	if( col ) *col = 0;

	// Make sure that the ListView is in LVS_REPORT
	if( (GetWindowLong(m_hWnd, GWL_STYLE) & LVS_TYPEMASK) != LVS_REPORT )
		return row;

	// Get the top and bottom row visible
	row = GetTopIndex();
	int bottom = row + GetCountPerPage();
	if( bottom > GetItemCount() )
		bottom = GetItemCount();
	
	// Get the number of columns
	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
	int nColumnCount = pHeader->GetItemCount();

	// Loop through the visible rows
	for( ;row <=bottom;row++)
	{
		// Get bounding rect of item and check whether point falls in it.
		CRect rect;
		GetItemRect( row, &rect, LVIR_BOUNDS );
		if( rect.PtInRect(point) )
		{
			// Now find the column
			for( colnum = 0; colnum < nColumnCount; colnum++ )
			{
				int colwidth = GetColumnWidth(colnum);
				if( point.x >= rect.left 
					&& point.x <= (rect.left + colwidth ) )
				{
					if( col ) *col = colnum;
					return row;
				}
				rect.left += colwidth;
			}
		}
	}
	return -1;
}

int RuleListCtrl::addRule( int chc, std::vector<std::string>& vec)
{
	// append a new rule to the storage
	Rule nR( (Rule::rule_type) chc, vec);
	m_rules.push_back( nR);

	return insertItem( nR);
}

void RuleListCtrl::addNewItem( int pChoice)
{
	if( pChoice == Rule::KindNameChange
	 || pChoice == Rule::ParadigmGUIDChange
	 || pChoice == Rule::ParadigmNameChange
	 || pChoice == Rule::ParadigmVersionChange)
	{
		SimpleReplace sr;
		if( sr.DoModal() == IDOK)
		{
			std::vector<std::string> pars(2);
			pars[0] = sr.getPar1();
			pars[1] = sr.getPar2();
			int idx = addRule( pChoice, pars);
		}
	}
	else if( pChoice == Rule::AttrNameChange)
	{
		AttrNameDlg an;
		if( an.DoModal() == IDOK)
		{
			std::vector<std::string> pars(4);
			pars[0] = an.getPar1();
			pars[1] = an.getPar2();
			pars[2] = an.getPar3();
			pars[3] = an.getPar4();
			int idx = addRule( pChoice, pars);
		}
	}
	else if( pChoice == Rule::AttrTypeChange)
	{
		AttrTypeChangeDlg tc;
		if( tc.DoModal() == IDOK)
		{
			std::vector<std::string> pars(4);
			pars[0] = tc.getPar1();
			pars[1] = tc.getPar2();
			pars[2] = tc.getPar3();
			pars[3] = tc.getPar4();
			int idx = addRule( pChoice, pars);
		}
	}
	else if( pChoice == Rule::EnumAttrValueChange)
	{
		EnumAttrValueDlg ea;
		if( ea.DoModal() == IDOK)
		{
			std::vector<std::string> pars(5);
			pars[0] = ea.getPar1();
			pars[1] = ea.getPar2();
			pars[2] = ea.getPar3();
			pars[3] = ea.getPar4();
			pars[4] = ea.getPar5();
			int idx = addRule( pChoice, pars);
		}
	}
	else if( pChoice == Rule::MoveUpElem) 
	{
		MoveUpElem mu;
		if( mu.DoModal() == IDOK)
		{
			std::vector<std::string> pars = mu.getSequence();
			addRule( pChoice, pars);
		}
	}
	else if( pChoice == Rule::MoveDownElem)
	{
		MoveDownElem md;
		if( md.DoModal() == IDOK)
		{
			std::vector< std::string> pars = md.getSequence();
			addRule( pChoice, pars);
		}
	}
	else if( pChoice == Rule::RemoveKind
		||   pChoice == Rule::RemoveGlobalAttr)
	{
		RemoveDlg tc( pChoice == Rule::RemoveGlobalAttr);
		if( tc.DoModal() == IDOK)
		{
			std::vector<std::string> pars(1);
			pars[0] = tc.getPar1();
			int idx = addRule( pChoice, pars);
		}
	}
	else if( pChoice == Rule::RemoveLocalAttr)
	{
		RemoveLocalAttrDlg tc;
		if( tc.DoModal() == IDOK)
		{
			std::vector<std::string> pars(2);
			pars[0] = tc.getPar1();
			pars[1] = tc.getPar2();
			int idx = addRule( pChoice, pars);
		}
	}
	else if( pChoice == Rule::Atom2Model
		||   pChoice == Rule::Model2Atom)
	{
		StereotypeCastDlg sc( pChoice == Rule::Atom2Model);
		if( sc.DoModal() == IDOK)
		{
			std::vector<std::string> pars(1);
			pars[0] = sc.getPar1();
			int idx = addRule( pChoice, pars);
		}
	}
}

void RuleListCtrl::showPropertyPage( int idx)
{
	DWORD chc = (DWORD) GetItemData( idx);

	if( chc == Rule::KindNameChange
	 || chc == Rule::ParadigmGUIDChange
	 || chc == Rule::ParadigmNameChange
	 || chc == Rule::ParadigmVersionChange)
	{
		SimpleReplace sr;
		ASSERT( idx < (int) m_rules.size());
		ASSERT( m_rules[idx].m_params.size() >= 2);
		sr.init( m_rules[idx].m_params[0].c_str(), m_rules[idx].m_params[1].c_str());

		if( sr.DoModal() == IDOK)
		{
			m_rules[idx].m_params[0] = sr.getPar1();
			m_rules[idx].m_params[1] = sr.getPar2();

			setItem( Rule::ruleSummary( m_rules[idx]), idx, 0);
		}
	}
	else if( chc == Rule::AttrNameChange)
	{
		AttrNameDlg an;
		ASSERT( idx < (int) m_rules.size());
		ASSERT( m_rules[idx].m_params.size() >= 4);
		an.init( m_rules[idx].m_params[0].c_str(), m_rules[idx].m_params[1].c_str(), m_rules[idx].m_params[2].c_str(), m_rules[idx].m_params[3].c_str());

		if( an.DoModal() == IDOK)
		{
			m_rules[idx].m_params[0] = an.getPar1();
			m_rules[idx].m_params[1] = an.getPar2();
			m_rules[idx].m_params[2] = an.getPar3();
			m_rules[idx].m_params[3] = an.getPar4();

			setItem( Rule::ruleSummary( m_rules[idx]), idx, 0);
		}
	}
	else if( chc == Rule::AttrTypeChange)
	{
		AttrTypeChangeDlg tc;
		ASSERT( idx < (int) m_rules.size());
		ASSERT( m_rules[idx].m_params.size() >= 4);
		tc.init( m_rules[idx].m_params[0].c_str(), m_rules[idx].m_params[1].c_str(), m_rules[idx].m_params[2].c_str(), m_rules[idx].m_params[3].c_str());
		if( tc.DoModal() == IDOK)
		{
			m_rules[idx].m_params[0] = tc.getPar1();
			m_rules[idx].m_params[1] = tc.getPar2();
			m_rules[idx].m_params[2] = tc.getPar3();
			m_rules[idx].m_params[3] = tc.getPar4();

			setItem( Rule::ruleSummary( m_rules[idx]), idx, 0);
		}
	}
	else if( chc == Rule::EnumAttrValueChange)
	{
		EnumAttrValueDlg ea;
		ASSERT( idx < (int) m_rules.size());
		ASSERT( m_rules[idx].m_params.size() >= 5);
		ea.init( m_rules[idx].m_params[0].c_str(), m_rules[idx].m_params[1].c_str(), m_rules[idx].m_params[2].c_str(), m_rules[idx].m_params[3].c_str(), m_rules[idx].m_params[4].c_str());

		if( ea.DoModal() == IDOK)
		{
			m_rules[idx].m_params[0] = ea.getPar1();
			m_rules[idx].m_params[1] = ea.getPar2();
			m_rules[idx].m_params[2] = ea.getPar3();
			m_rules[idx].m_params[3] = ea.getPar4();
			m_rules[idx].m_params[4] = ea.getPar5();

			setItem( Rule::ruleSummary( m_rules[idx]), idx, 0);
		}
	}
	else if( chc == Rule::MoveUpElem)
	{
		MoveUpElem mu;
		mu.init( m_rules[idx].m_params );
		if( mu.DoModal() == IDOK)
		{
			m_rules[idx].m_params = mu.getSequence();
			setItem( Rule::ruleSummary( m_rules[idx]), idx, 0);
		}
	}
	else if( chc == Rule::MoveDownElem)
	{
		MoveDownElem md;
		md.init( m_rules[idx].m_params);
		if( md.DoModal() == IDOK)
		{
			m_rules[idx].m_params = md.getSequence();
			setItem( Rule::ruleSummary( m_rules[idx]), idx, 0);
		}
	}
	else if( chc == Rule::RemoveKind
		||   chc == Rule::RemoveGlobalAttr)
	{
		RemoveDlg tc( chc == Rule::RemoveGlobalAttr);
		tc.init( m_rules[idx].m_params[0].c_str());
		if( tc.DoModal() == IDOK)
		{
			m_rules[idx].m_params[0] = tc.getPar1();
			setItem( Rule::ruleSummary( m_rules[idx]), idx, 0);
		}
	}
	else if( chc == Rule::RemoveLocalAttr)
	{
		RemoveLocalAttrDlg tc;
		tc.init( m_rules[idx].m_params[0].c_str(), m_rules[idx].m_params[1].c_str());
		if( tc.DoModal() == IDOK)
		{
			m_rules[idx].m_params[0] = tc.getPar1();
			m_rules[idx].m_params[1] = tc.getPar2();
			setItem( Rule::ruleSummary( m_rules[idx]), idx, 0);
		}
	}
	else if( chc == Rule::Atom2Model
		||   chc == Rule::Model2Atom)
	{
		StereotypeCastDlg sc( chc == Rule::Atom2Model);
		sc.init( m_rules[idx].m_params[0].c_str());
		if( sc.DoModal() == IDOK)
		{
			m_rules[idx].m_params[0] = sc.getPar1();
			setItem( Rule::ruleSummary( m_rules[idx]), idx, 0);
		}
	}
}

void RuleListCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	int index;
	CListCtrl::OnLButtonDblClk(nFlags, point);

	int colnum;
	if( ( index = HitTestEx( point, &colnum )) != -1 )
	{
		UINT flag = LVIS_FOCUSED;
		if( (GetItemState( index, flag ) & flag) == flag )
		{
			if(colnum==0 || colnum != 0)
			{
				showPropertyPage( index);
			}
		}
		else
			SetItemState( index, LVIS_SELECTED | LVIS_FOCUSED , 
					LVIS_SELECTED | LVIS_FOCUSED);
	}
}

void RuleListCtrl::add()
{
	NewRuleDlg nrd;
	if( nrd.DoModal() == IDOK)
	{
		addNewItem( nrd.m_choice);
	}
}

CString RuleListCtrl::genSelected( bool pSeparateFiles)
{
	std::vector< int> res; // vec constructed from the indices of the selected rows
	for( int i = 0; i < GetItemCount(); ++i)
	{
		CString knd;
		LV_ITEM lvItem;
		lvItem.mask = LVIF_TEXT | LVIF_STATE;
		lvItem.iItem = i;
		lvItem.iSubItem = 1;
		lvItem.pszText = knd.GetBuffer( 1024);
		lvItem.cchTextMax = 1024;
		lvItem.stateMask = LVIS_SELECTED;
		GetItem( &lvItem);

		if( (lvItem.state & LVIS_SELECTED) == LVIS_SELECTED)
			res.push_back( i );
	}

	CFileDialog dlg( FALSE, ".xsl", "script1", OFN_OVERWRITEPROMPT, 
		"XSLT Files (*.xsl;*.xslt)|*.xsl; *.xslt|All Files (*.*)|*.*||");

	if( dlg.DoModal() == IDOK)
	{
		if( pSeparateFiles)
		{
			if( genIntoSeparate( (LPCTSTR) dlg.GetPathName(), res))
				return dlg.GetPathName();
		}
		else
		{
			if( gen( (LPCTSTR) dlg.GetPathName(), res))
				return dlg.GetPathName();
		}
	}

	return "";
}


CString RuleListCtrl::genAll( bool pSeparateFiles)
{
	CFileDialog dlg( FALSE, ".xsl", "script1", OFN_OVERWRITEPROMPT, 
		"XSLT Files (*.xsl;*.xslt)|*.xsl; *.xslt|All Files (*.*)|*.*||");

	if( dlg.DoModal() == IDOK)
	{
		std::vector< int> res( GetItemCount()); // like all rows were selected
		for( unsigned int i = 0; i < res.size(); ++i) 
			res[i] = i;

		if( pSeparateFiles)
		{
			if( genIntoSeparate( (LPCTSTR) dlg.GetPathName(), res))
				return dlg.GetPathName();
		}
		else
		{
			if( gen( (LPCTSTR) dlg.GetPathName(), res))
				return dlg.GetPathName();
		}
	}

	return "";
}

int RuleListCtrl::gen( const std::string& pOutputFileName, const std::vector<int>& pSelectVec)
{
	std::fstream f;
	f.open( pOutputFileName.c_str(), std::ios_base::out|std::ios_base::binary); // write out as is (not to mess with additional CR-s)
	ASSERT( f.is_open());
	if( !f.is_open())
	{
		AfxMessageBox( "Could not create target file!");
		return 0;
	}

	std::string h_part = Script::getXSLFromResource( "Header.xsl");
	if( !h_part.empty())
	{
		f.write( h_part.c_str(), (std::streamsize) h_part.length());
	}

	f << "\r\n<!-- ******************************************* rules ******************************************* -->";
	f << "\r\n";

	for( unsigned int i = 0; i < m_rules.size(); ++i)
	{
		if( std::find( pSelectVec.begin(), pSelectVec.end(), i) == pSelectVec.end()) // not found
			continue;

		f << "\r\n";
		f << "<!-- ~~~~~~~~~~~~ rule nb=\"" << i << "\" ~~~~~~~~~~~~ -->";
		f << "\r\n";

		m_rules[i].gen( f );

		f << "\r\n";
		f << "<!-- ~~~~~~~~~~~~ /rule ~~~~~~~~~~~~~~~~~~~ -->";
		f << "\r\n";
		f << "\r\n";
	}

	f << "<!-- ******************************************* /rules ******************************************* -->";
	f << "\r\n\r\n\r\n";

	std::string t_part = Script::getXSLFromResource( "Tail.xsl");
	if( !t_part.empty())
	{
		f.write( t_part.c_str(), (std::streamsize) t_part.length());
	}

	f.close();

	return 1;
}

int RuleListCtrl::genIntoSeparate( const std::string& pOutputFileName, const std::vector<int>& pSelectVec)
{
	std::string h_part = Script::getXSLFromResource( "Header.xsl");
	if( h_part.empty())
	{
		AfxMessageBox( "Could not load header part from resources!"); 
		return 0;
	}

	std::string t_part = Script::getXSLFromResource( "Tail.xsl");
	if( t_part.empty())
	{
		AfxMessageBox( "Could not load tail part from resources!");
		return 0;
	}

	size_t pos_of_dot = pOutputFileName.rfind( ".xsl");
	if( pos_of_dot == 0) 
	{ 
		AfxMessageBox( "Wrong file name given"); 
		return 0; 
	}
	else if( pos_of_dot == std::string::npos) pos_of_dot = pOutputFileName.length();

	int rule_nmb = 0;
	for( unsigned int i = 0; i < m_rules.size(); ++i)
	{
		if( std::find( pSelectVec.begin(), pSelectVec.end(), i) == pSelectVec.end()) // not found
			continue;

		++rule_nmb;

		// will use the file name given without extension
		std::string output_file_name( pOutputFileName.substr(0, pos_of_dot));
		char buff[16];
		sprintf_s( buff, "%0.3i.xsl", rule_nmb);
		output_file_name += buff; // append something like: "001.xsl"

		std::fstream f;
		f.open( output_file_name.c_str(), std::ios_base::out|std::ios_base::binary); // write out as is (not to mess with additional CR-s)
		ASSERT( f.is_open());
		if( !f.is_open())
		{
			std::string msg = "Could not create target file: ";
			msg += output_file_name;
			AfxMessageBox( msg.c_str());
			return 0;
		}

		f.write( h_part.c_str(), (std::streamsize) h_part.length());

		f << "\r\n<!-- ******************************************* 1 rule ******************************************* -->";
		f << "\r\n";


		f << "\r\n";
		f << "<!-- ~~~~~~~~~~~~ rule nb=\"" << rule_nmb << "\" ~~~~~~~~~~~~ -->";
		f << "\r\n";

		m_rules[i].gen( f );

		f << "\r\n";
		f << "<!-- ~~~~~~~~~~~~ /rule ~~~~~~~~~~~~~~~~~~~ -->";
		f << "\r\n";
		f << "\r\n";

		f << "<!-- ******************************************* /1 rule ******************************************* -->";
		f << "\r\n\r\n\r\n";
		
		f.write( t_part.c_str(), (std::streamsize) t_part.length());

		f.close();
	}

	return 1;
}


void RuleListCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
	CListCtrl::OnRButtonDown(nFlags, point);
	ClientToScreen( &point);

	createPopupMenu( point);
}

void RuleListCtrl::showPopupMenu()
{
	// find the focused item to determine the position of the popup menu
	int idx = -1;
	for( int i = 0; idx == -1 && i < GetItemCount(); ++i)
	{
		UINT flag = LVIS_FOCUSED;// | LVIS_SELECTED;
		if( (GetItemState( i, flag ) & flag) == flag )
		{
			idx = i;
		}
	}

	CPoint point;
	if( idx != -1) 
		GetItemPosition( idx, &point);
	else
	{
		// no item found, use the top-left corner of the empty listcontrol
		CRect rect;
		GetClientRect(rect);
		point = CPoint( rect.left, rect.top);
	}
	point.Offset( 10, 10);

	ClientToScreen( &point);
	createPopupMenu( point);
}

void RuleListCtrl::createPopupMenu( CPoint point)
{
	CMenu m;
	m.CreatePopupMenu();
	m.AppendMenu( MF_ENABLED|MF_STRING, EDIT_CMD,     "Edit");
	m.AppendMenu( MF_ENABLED|MF_STRING, ADD_CMD,      "Add");
	m.AppendMenu( MF_ENABLED|MF_STRING, CLONE_CMD,    "Clone");
	m.AppendMenu( MF_ENABLED|MF_SEPARATOR, 0,             "");
	m.AppendMenu( MF_ENABLED|MF_STRING, DELEALL_CMD,  "Delete All");
	m.AppendMenu( MF_ENABLED|MF_STRING, DELESEL_CMD,  "Delete Selected");
	m.AppendMenu( MF_ENABLED|MF_SEPARATOR, 0,             "");
	m.AppendMenu( MF_ENABLED|MF_STRING, MOVEUP_CMD,   "Move Up");
	m.AppendMenu( MF_ENABLED|MF_STRING, MOVEDN_CMD,   "Move Down");
	m.AppendMenu( MF_ENABLED|MF_SEPARATOR, 0,             "");
	m.AppendMenu( MF_ENABLED|MF_STRING, GENALL_CMD,  "Generate from All");
	m.AppendMenu( MF_ENABLED|MF_STRING, GENSEL_CMD,  "Generate from Selected");
	m.TrackPopupMenu( TPM_LEFTALIGN |TPM_RIGHTBUTTON, point.x, point.y, this);
}

BOOL RuleListCtrl::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch( wParam)
	{
	case ADD_CMD: onAdd();break;
	case EDIT_CMD: onEdit();break;
	case DELEALL_CMD: onDeleAll();break;
	case DELESEL_CMD: onDeleSel();break;
	case CLONE_CMD: onClone();break;
	case MOVEUP_CMD: onMoveUp();break;
	case MOVEDN_CMD: onMoveDown();break;
	case GENALL_CMD: onGenAll();break;
	case GENSEL_CMD: onGenSel();break;
	};

	return CListCtrl::OnCommand(wParam, lParam);
}

void RuleListCtrl::onEdit()
{
	int index = getFirstFocSel();
	if( index != -1) showPropertyPage( index);
}

void RuleListCtrl::onClone()
{
	int index = getFirstFocSel();
	if( index != -1)
	{
		CString sum, typ;
		getItem( sum, index, 0);
		getItem( typ, index, 1);
		addRule( m_rules[index].m_type, m_rules[index].m_params);
	}
}

void RuleListCtrl::onDeleAll()
{
	DeleteAllItems();
	m_rules.clear();
}

void RuleListCtrl::onDeleSel()
{
	for( int i = GetItemCount(); i > 0 ; --i)
	{
		UINT sel = GetItemState( i-1, LVIS_SELECTED);
		if( sel & LVIS_SELECTED)
		{
			DeleteItem( i-1);

			int index = i - 1;
			// delete elem an index in m_rules
			RULE_VEC_ITERATOR ri = m_rules.begin();
			while( ri != m_rules.end() && index) 
				{ ++ri; --index; }
			if( ri != m_rules.end()) // valid iterator 
				m_rules.erase( ri); 
		}
	}
}

void RuleListCtrl::onMoveUp()
{
	int index = getFirstFocSel();
	if( index != -1 && index > 0)
	{
		CString sumI, typI, sumJ, typJ;
		getItem( sumI, index, 0);
		getItem( typI, index, 1);
		getItem( sumJ, index-1, 0);
		getItem( typJ, index-1, 1);
		
		// swap the rule vector
		Rule rl = m_rules[ index];
		m_rules[ index] = m_rules[ index - 1];
		m_rules[ index - 1] = rl; // simpler than an iter_swap

		// swap the items in the list ctrl
		setItem( (LPCTSTR) sumJ, index, 0);
		setItem( (LPCTSTR) typJ, index, 1);
		setItem( (LPCTSTR) sumI, index - 1, 0);
		setItem( (LPCTSTR) typI, index - 1, 1);

		DWORD data = (DWORD) GetItemData( index);
		SetItemData( index, (DWORD) GetItemData( index - 1));
		SetItemData( index - 1, data);

		// set the focus and selection to the same item (in the upper line)
		setFocSel( index - 1);
	}
}

void RuleListCtrl::onMoveDown()
{
	int index = getFirstFocSel();
	if( index != -1 && index < GetItemCount() - 1)
	{
		CString sumI, typI, sumJ, typJ;
		getItem( sumI, index, 0);
		getItem( typI, index, 1);
		getItem( sumJ, index+1, 0);
		getItem( typJ, index+1, 1);
		
		// swap the rule vector
		Rule rl = m_rules[ index];
		m_rules[ index] = m_rules[ index + 1];
		m_rules[ index + 1] = rl; // simpler than an iter_swap

		// swap the items in the list ctrl
		setItem( (LPCTSTR) sumJ, index, 0);
		setItem( (LPCTSTR) typJ, index, 1);
		setItem( (LPCTSTR) sumI, index + 1, 0);
		setItem( (LPCTSTR) typI, index + 1, 1);

		DWORD data = (DWORD) GetItemData( index);
		SetItemData( index, (DWORD) GetItemData( index + 1));
		SetItemData( index + 1, data);

		// set the focus and selection to the same item (in the lower line)
		setFocSel( index + 1);
	}
}

void RuleListCtrl::onGenAll()
{
	if( m_parentDlg) m_parentDlg->OnBnClickedGenerate();
}

void RuleListCtrl::onGenSel()
{
	if( m_parentDlg) m_parentDlg->OnBnClickedGeneratesel();
}

void RuleListCtrl::onAdd()
{
	add();
}

int RuleListCtrl::getFirstFocSel()
{
	int index = -1;
	UINT flag = LVIS_FOCUSED | LVIS_SELECTED;
	for( int i = 0; index == -1 && i < GetItemCount(); ++i)
	{
		if( (GetItemState( i, flag) & flag) == flag) index = i;
	}

	return index;
}

void RuleListCtrl::setFocSel( int index)
{
	UINT flag = LVIS_FOCUSED | LVIS_SELECTED;
	for( int i = 0; i < GetItemCount(); ++i)
		if( i == index) SetItemState( i, flag, flag);
		else            SetItemState( i, 0   , flag);
}

void RuleListCtrl::PreSubclassWindow()
{
	int col0size = 2*GetStringWidth("KindNameChangeIsSo");
	int col1size = 2*GetStringWidth("TypeOfIsAlso");
	//int col2size = 2*GetStringWidth("Apply");
	//int col3size = 2*GetStringWidth("Priority");

	InsertColumn(0, _T("Rule"), LVCFMT_LEFT,  col0size, -1);
	InsertColumn(1, _T("Type"), LVCFMT_LEFT, col1size, -1);
	//InsertColumn(2, _T("Apply"), LVCFMT_LEFT, col2size, -1);
	//InsertColumn(3, _T("Priority"), LVCFMT_LEFT, col3size, -1);

	SetExtendedStyle( LVS_EX_FULLROWSELECT );

	CListCtrl::PreSubclassWindow();
}

int RuleListCtrl::insertItem( const Rule& pRule)
{
	CString sum = Rule::ruleSummary( pRule);
	CString typ = Rule::ruleTypeStr( (int) pRule.m_type);

	// append an item to the control
	LV_ITEM lvItem;
	lvItem.mask = LVIF_TEXT;
	lvItem.iItem = GetItemCount();
	lvItem.iSubItem = 0;
	lvItem.pszText = sum.GetBuffer(sum.GetLength());
	int index = InsertItem(&lvItem);
	
	lvItem.iSubItem = 1;
	lvItem.pszText = typ.GetBuffer(typ.GetLength());
	SetItem(&lvItem);

	//bool apply = true;
	//lvItem.iSubItem = 2;
	//lvItem.pszText = apply ? _T("yes"): _T("no");
	//SetItem(&lvItem);

	//lvItem.iSubItem = 3;
	//lvItem.pszText = _T("1");
	//SetItem(&lvItem);


	DWORD data = pRule.m_type;
	SetItemData(index, data);

	return index;
}

void RuleListCtrl::getItem( CString& pStr, int idx, int subIdx)
{
	LV_ITEM lvItem;
	lvItem.mask = LVIF_TEXT;
	lvItem.iItem = idx;
	lvItem.iSubItem = subIdx;
	lvItem.pszText = pStr.GetBuffer( 256);
	lvItem.cchTextMax = 256;

	GetItem( &lvItem); 
}

void RuleListCtrl::setItem( CString pStr, int idx, int subIdx)
{
	LV_ITEM lvItem;
	lvItem.mask = LVIF_TEXT;
	lvItem.iItem = idx;
	lvItem.iSubItem = subIdx;
	lvItem.pszText = pStr.GetBuffer( pStr.GetLength());
	SetItem(&lvItem);
}

bool RuleListCtrl::erasable()
{
	return m_rules.empty() && GetItemCount() <= 0;
}

void RuleListCtrl::fileNew()
{
	m_rules.clear();
	this->DeleteAllItems();
}

void RuleListCtrl::fileLoad()
{
	CFileDialog dlg( TRUE, ".mm", "", 0, 
		"ModelMigrate Files (*.mm)|*.mm|All Files (*.*)|*.*||");

	if( dlg.DoModal() == IDOK)
	{
		CFile f( (LPCTSTR) dlg.GetPathName(), CFile::modeRead);
		CArchive ar( &f, CArchive::load);
		unsigned int loaded_size = 0;
		ar >> loaded_size;
		unsigned int size_before = (unsigned int) m_rules.size();
		m_rules.resize( size_before + loaded_size); // enlarge it to the new size
		for( unsigned int i = 0; i < loaded_size; ++i)
			m_rules[size_before + i].Serialize( ar);

		ar.Close();
		f.Close();

		for( unsigned int i = size_before; i < m_rules.size(); ++i)
		{
			insertItem( m_rules[i]);
		}
	}
}

void RuleListCtrl::fileSave()
{
	CFileDialog dlg( FALSE, ".mm", "ruleset", OFN_OVERWRITEPROMPT, 
		"ModelMigrate Files (*.mm)|*.mm|All Files (*.*)|*.*||");

	if( dlg.DoModal() == IDOK)
	{
		CFile f( (LPCTSTR) dlg.GetPathName(), CFile::modeCreate | CFile::modeWrite);
		CArchive ar( &f, CArchive::store);
		ar << (unsigned int) m_rules.size();
		for( unsigned int i = 0; i < m_rules.size(); ++i)
			m_rules[i].Serialize( ar);

		ar.Close();
		f.Close();
	}
}

