// SvnLoginDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "XmlBackEnd.h"
#include ".\SvnLoginDlg.h"
#include <io.h>
#include <fstream>

// SvnLoginDlg dialog

IMPLEMENT_DYNAMIC(CSvnLoginDlg, CDialog)
CSvnLoginDlg::CSvnLoginDlg( int p_prefChoice, CWnd* pParent /*=NULL*/)
	: CDialog(CSvnLoginDlg::IDD, pParent)
	, m_prefChoice( p_prefChoice)
	, m_aborted( false)
	, m_sshDisabled( false)
	, m_leftPad( 20)
	, m_rightPad( 20)
	, m_user(_T(""))
	, m_password(_T(""))
	, m_database(_T(""))
	, m_project(_T(""))
	, m_credRadioBtn(0)
{
}

CSvnLoginDlg::~CSvnLoginDlg()
{
}

void CSvnLoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_USER, m_user);
	DDX_Text(pDX, IDC_EDIT_PWD, m_password);
	DDX_Text(pDX, IDC_STATIC_DATABASE, m_database);
	DDX_Text(pDX, IDC_STATIC_PROJECT, m_project);
	DDX_Control(pDX, IDC_RADIO_CACHED, m_radBtnLoginMethod);
	DDX_Control(pDX, IDC_EDIT_USER, m_userCtrl);
	DDX_Control(pDX, IDC_EDIT_PWD, m_passwordCtrl);
	DDX_Control(pDX, IDC_LOADFROMCACHE, m_btnLoadFromCache);
	DDX_Radio(pDX, IDC_RADIO_CACHED, m_credRadioBtn);
}


BEGIN_MESSAGE_MAP(CSvnLoginDlg, CDialog)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_ABORT, OnBnClickedAbort)
	ON_BN_CLICKED(IDC_RADIO_USPW, OnBnClickedRadioUspw)
	ON_BN_CLICKED(IDC_RADIO_CACHED, OnBnClickedRadioCached)
	ON_BN_CLICKED(IDC_RADIO_PPK, OnBnClickedRadioPpk)
	ON_BN_CLICKED(IDC_LOADFROMCACHE, OnBnClickedLoadFromCache)
END_MESSAGE_MAP()


// CSvnLoginDlg message handlers

void CSvnLoginDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	CWnd * db = GetDlgItem( IDC_STATIC_DATABASE);
	CWnd * pj = GetDlgItem( IDC_STATIC_PROJECT);
	CRect db_rect, pj_rect;
	if( db && pj && db->GetSafeHwnd() && pj->GetSafeHwnd()) {
		db->GetWindowRect( &db_rect);
		pj->GetWindowRect( &pj_rect);
		ScreenToClient( &db_rect);
		ScreenToClient( &pj_rect);
		int width = cx - m_leftPad - m_rightPad;

		db->SetWindowPos( NULL, m_leftPad, db_rect.top, width > 20? width: 20, db_rect.Height(), SWP_NOZORDER);
		pj->SetWindowPos( NULL, m_leftPad, pj_rect.top, width > 20? width: 20, pj_rect.Height(), SWP_NOZORDER);
	}
}

BOOL CSvnLoginDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	m_credRadioBtn = m_prefChoice;

	// turn off ppk in all cases except svn+ssh
	CWnd* ppk = GetDlgItem( IDC_RADIO_PPK);
	if( ppk) ppk->EnableWindow( !m_sshDisabled); // enable also if no url to decide upon

	m_passwordCtrl.EnableWindow( m_credRadioBtn == 1);//either 0 or 2, m_radBtnLoginMethod.GetCheck() != BST_CHECKED);
	m_btnLoadFromCache.EnableWindow( m_credRadioBtn == 0);

	// wm_size related calculations
	CWnd * db = GetDlgItem( IDC_STATIC_DATABASE);
	CRect db_rect;
	if( db && db->GetSafeHwnd()) {
		db->GetWindowRect( &db_rect);
		ScreenToClient( &db_rect);

		CRect win_rect;
		GetWindowRect( &win_rect);
		ScreenToClient( &win_rect);

		m_leftPad = db_rect.left - win_rect.left;
		m_rightPad = win_rect.right - db_rect.right;
	}

	UpdateData(FALSE); // send data modifications to controls

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSvnLoginDlg::OnBnClickedRadioUspw()
{
	m_passwordCtrl.EnableWindow( TRUE);
	m_btnLoadFromCache.EnableWindow( FALSE);
}

void CSvnLoginDlg::OnBnClickedRadioCached()
{
	m_passwordCtrl.EnableWindow( FALSE);
	m_btnLoadFromCache.EnableWindow( TRUE);
}

void CSvnLoginDlg::OnBnClickedRadioPpk()
{
	m_passwordCtrl.EnableWindow( FALSE);
	m_btnLoadFromCache.EnableWindow( FALSE);
}

void CSvnLoginDlg::OnBnClickedAbort()
{
	m_aborted = true;
	EndDialog( IDCANCEL);
	//throw hresult_exception( E_FAIL);
}

bool CSvnLoginDlg::wasAborted()
{
	return m_aborted;
}

void CSvnLoginDlg::disableSshOption()
{
	m_sshDisabled = true;
}

bool CSvnLoginDlg::getDataFromCache( const std::string& p_inRealm, std::string& p_realm, std::string& p_uname)
{
// analyzing files as "C:\Documents and Settings\zolmol\Application Data\Subversion\auth\svn.simple\f061142afd1f61e24960515448599a09" 
// with sample content below
	const char* svn_sim_path = "\\Subversion\\auth\\svn.simple";
	char        app_data_path[ _MAX_PATH];
	if( !SHGetSpecialFolderPath( NULL, app_data_path, CSIDL_APPDATA, true)) //most likely C:\Documents and Settings\<username>\Application Data
		return false;

	if( 1)
	{
		char                 buf[_MAX_PATH];
		_finddata_t          fileInfo;

		sprintf( buf, "%s%s\\*.", app_data_path, svn_sim_path);

		long searchHandle = _findfirst( buf, &fileInfo );
		long ret = searchHandle;
		while( ret != -1 )
		{
			sprintf( buf, "%s%s\\%s", app_data_path, svn_sim_path, fileInfo.name );

			std::string _realm;
			std::string _uname;
			if( loadCacheFile( buf, _realm, _uname))
			{
				std::string::size_type co_pos = _realm.find( ':');
				std::string::size_type rb_pos = _realm.find( '>');
				if( co_pos != std::string::npos && rb_pos != std::string::npos && co_pos < rb_pos)
					rb_pos = co_pos; // colon found indicating port number, so will cut right there

				if( rb_pos != std::string::npos && _realm[0] == '<')
				{
					_realm = _realm.substr( 1, rb_pos - 1);
					if( p_inRealm.substr( 0, _realm.length()) == _realm) // found
					{
						p_realm = _realm;
						p_uname = _uname;
						return true;
					}
				}
			}
			
			ret = _findnext( searchHandle, &fileInfo );
		}
		_findclose( searchHandle );
	}

	return false;
}

bool CSvnLoginDlg::loadCacheFile( const char* p_fileNamePtr, std::string& p_realm, std::string& p_uname)
{
// sample content of a cache file:
//...
//K 15 
//svn:realmstring
//V 68
//<https://svn.isis.vanderbilt.edu:443> testrepo username and password
//K 8
//username
//V 7
//zolmol2
//END 

	std::string value_realmstring;
	std::string value_username;
	bool        collecting_realmstring = false;
	bool        collecting_username    = false;
	bool        found_realmstring      = false;
	bool        found_username         = false;

	enum TypeOfLine
	{
		KEY_INDICATOR
		,VAL_INDICATOR
		,KEY_STRING
		,VAL_STRING
	};

	TypeOfLine t_of_ln = VAL_STRING;
	TypeOfLine t_of_prev_ln;
	{
		std::ifstream datafile;
		datafile.open( p_fileNamePtr, std::ios_base::in);
		if( datafile.is_open())
		{
			char buff[1024];
			while( datafile.getline( buff, 1024))
			{
				std::string line( buff);
				if( line.empty())
					continue;

				t_of_prev_ln = t_of_ln;
				
				if( line.substr( 0, 2) == "K ")
					t_of_ln = KEY_INDICATOR;
				else if( line.substr( 0, 2) == "V ")
					t_of_ln = VAL_INDICATOR;
				else if( t_of_prev_ln == KEY_INDICATOR)
					t_of_ln = KEY_STRING;
				else if( t_of_prev_ln == VAL_INDICATOR)
					t_of_ln = VAL_STRING;

				if( t_of_ln == KEY_STRING && 0 == line.find( "svn:realmstring"))
				{
					collecting_realmstring = true;
				}
				else if( t_of_ln == KEY_STRING && 0 == line.find( "username"))
				{
					collecting_username = true;
				}

				if( t_of_ln == VAL_STRING)
				{
					if( collecting_realmstring)
						value_realmstring = line, collecting_realmstring = false, found_realmstring = true;
					if( collecting_username)
						value_username = line, collecting_username = false, found_username = true;
				}
			}
		}
		datafile.close();
	}

	if( found_realmstring && found_username && !value_realmstring.empty() && !value_username.empty())
	{
		p_realm = value_realmstring;
		p_uname = value_username;
		return true;
	}

	return false;
}

void CSvnLoginDlg::OnBnClickedLoadFromCache()
{
	std::string uname;
	if( getDataFromCache( (LPCTSTR) m_database, std::string(), uname))
	{
		m_user = uname.c_str();
		UpdateData( FALSE);
		m_userCtrl.SetFocus(); // set the focus back to the username to allow a quick 'Enter' to be hit
		m_userCtrl.SetSel( 0, m_userCtrl.LineLength());
	}
	else
		AfxMessageBox( "Cached credentials were not found for this server!");
}
