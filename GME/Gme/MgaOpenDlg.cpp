// MgaOpenDlg.cpp : implementation file
//

#include "stdafx.h"
#include "GMEApp.h"
#include "MgaOpenDlg.h"
#include "NewXmlbackendProjDlg.h"
#include <afxdb.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMgaOpenDlg dialog


CMgaOpenDlg::CMgaOpenDlg(DialogTypes dType, CWnd* pParent /*=NULL*/)
	: CDialog(CMgaOpenDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMgaOpenDlg)
	m_radio = 0;
	//}}AFX_DATA_INIT

	switch (dType){
	case OpenDialog:
		title = _T("Open");
		filemsg = _T("Open project file");
        xmlfilemsg = _T("Open multi user project");
		flag_isopen = true;
		flag_back = false;
		flag_create = false;
		break;
	case SaveDialog:
		title = _T("Save");
		filemsg = _T("Save project file");
		xmlfilemsg = _T("Save multi user project");
		flag_isopen = false;
		flag_back = false;
		flag_create = false;
		break;
	case SaveAsDialog:
		title = _T("Save As");
		filemsg = _T("Save project file");
		xmlfilemsg = _T("Save multi user project");
		flag_isopen = false;
		flag_back = false;
		flag_create = false;
		break;
	case NewDialog:
		title = _T("New Project");
		filemsg = _T("Create project file");
        xmlfilemsg = _T("Create multi user project");
		flag_isopen = true;
		flag_back = true;
		flag_create = true;
		break;
	case ImportDialog:
		title = _T("Import to new project");
		filemsg = _T("Create project file");
		xmlfilemsg = _T("Create multi user project");
		flag_isopen = true;
		flag_back = false;
		flag_create = true;
		break;
	case ClearLocksDialog:
		title = _T("Select project");
		filemsg = _T("Open project file");
		xmlfilemsg = _T("Open multi user project");
		flag_isopen = true;
		flag_back = false;
		flag_create = false;
		break;
	}

	
}

static TCHAR mgafilter[] = _T("MGA Files (*.mga)|*.mga|Exported Files (*.xme;*.xml)|*.xme; *.xml|")
	_T("All files (*.*)|*.*||");

static TCHAR xmemgafilter[] = _T("GME Model Files (*.mga;*.xme;*.mgx)|*.mga; *.xme; *.mgx|MGA Files (*.mga)|*.mga|Exported Files (*.xme;*.xml)|*.xme; *.xml|")
	_T("Multi-user files (*.mgx)|*.mgx|All files (*.*)|*.*||");

static TCHAR mgaonlyfilter[] = _T("MGA Files (*.mga)|*.mga|All files (*.*)|*.*||");

static TCHAR metafilter[] = _T("MGA Meta Files (*.mta)|*.mta|XML Paradigm Files (*.xmp)|*.xmp|")
	_T("All files (*.*)|*.*||");


CString CMgaOpenDlg::AskMGAConnectionString()
{
	CString file, dir;
	if (theApp.isMgaProj() && theApp.mgaProject)
	{
		theApp.getMgaPaths(file, dir);
	}

	CString conn;
	CFileDialog dlg(flag_isopen ? TRUE : FALSE, _T("mga"), (file == _T("")) ? NULL : (LPCTSTR)file, 
			OFN_EXPLORER | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, mgaonlyfilter);

	if (dir != _T(""))
		dlg.GetOFN().lpstrInitialDir = dir;

	if( dlg.DoModal() == IDOK )	{
		conn = CString(_T("MGA=")) + dlg.GetPathName();
	}
	return conn;
}

CString CMgaOpenDlg::AskConnectionString(bool allowXme, bool openFileDialog)
{
	CString conn;

	try
	{
		if (openFileDialog || (DoModal() == IDOK && !pressed_back) )
		{
			if (openFileDialog || m_radio == 0)
			{
				const TCHAR* filter;
				if (allowXme == false && openFileDialog == false)
				{
					filter = mgaonlyfilter;
				}
				else
				{
					filter = allowXme ? xmemgafilter : mgafilter;
				}
				CFileDialog dlg(openFileDialog, NULL, fileNameHint.IsEmpty() ? NULL : (LPCTSTR)fileNameHint,
					OFN_EXPLORER | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT |
					(flag_create ? 0 : OFN_FILEMUSTEXIST), filter);
				if (!folderPathHint.IsEmpty())
					dlg.m_ofn.lpstrInitialDir = folderPathHint.GetBuffer(_MAX_PATH);
				if (allowXme == false && openFileDialog == false)
					dlg.m_ofn.lpstrDefExt = L"mga";

				if( dlg.DoModal() == IDOK )
				{
					CString ext = dlg.GetFileExt();
					ext.MakeLower();

					if (allowXme == false && openFileDialog == false)
					{
						return CString(L"MGA=") + dlg.GetPathName();
					}
					if( ext == _T("mga") || ext == _T("mta") )
						conn = CString(_T("MGA=")) + dlg.GetPathName();
                    else if( ext == _T("mgx") )
						conn = CString(_T("MGX=")) + dlg.GetPathName();
					else if( (ext == _T("xml")) || (ext == _T("xme")) || (ext == _T("xmp")) )
						conn = CString(_T("XML=")) + dlg.GetPathName();
					else if( ext == _T("") )
					{
						switch( dlg.m_ofn.nFilterIndex )
						{
						case 3:
						case 1:
							conn = CString(_T("MGA=")) + dlg.GetPathName() + _T(".mga");
							break;

						case 2:
							conn = CString(_T("XML=")) + dlg.GetPathName() + _T(".xme");
							break;
						}
					}
					else
					{
						switch( dlg.m_ofn.nFilterIndex )
						{
						case 3:
						case 1:
							conn = CString(_T("MGA=")) + dlg.GetPathName();
							break;

						case 2:
							conn = CString(_T("XML=")) + dlg.GetPathName();
							break;
						}
					}
				}
			}
            else if( m_radio == 1 )
            {
                if( flag_create )
                {
                    // create new multiuser project
                    CNewXmlbackendProjDlg dlg;
                    if( dlg.DoModal() == IDOK )
                        conn = dlg.m_connectionString;
                }
                else
                {
					// KMS: FIXME: I think this is dead code
                    // open existing multiuser project
                    BROWSEINFO bi;

                    TCHAR szDisplayName[MAX_PATH];
                    TCHAR szPath[MAX_PATH];

	                bi.hwndOwner      = m_hWnd;
	                bi.pidlRoot       = NULL;
	                bi.lpszTitle      = _T("Select the project location.");
	                bi.pszDisplayName = szDisplayName;
	                bi.ulFlags        = BIF_RETURNONLYFSDIRS;
	                bi.lpfn           = NULL;
	                bi.lParam         = 0;

                    LPITEMIDLIST idlist = SHBrowseForFolder(&bi);

                    if( idlist )
                    {
                        if( SHGetPathFromIDList(idlist, szPath) ) 
                        {
                            conn = _T("MGX=\"");
                            conn += szPath;
                            conn += _T("\"");
                        }
                    }
                }
            }
		}
	}
	catch(...)
	{
	}

	return conn;
}

CString CMgaOpenDlg::PruneConnectionString(const CString& conn)
{
	CString ret;

	int p = 0;
	while( p < conn.GetLength() )
	{
		int q = conn.Find(';', p);
		if( q < 0 )
			q = conn.GetLength();

		CString part((const TCHAR*)conn + p, q-p);

		int r = part.Find('=');
		if( r < 0 )
			r = part.GetLength();

		CString key((const TCHAR*)part, r);

		if( key == _T("UID") ||
			key == _T("PWD") ||
			key == _T("USER") ||
			key == _T("PASSWORD") )
		{
			if( !ret.IsEmpty() )
				ret += _T(";");

			ret += part;
		}

		p = q+1;
	}

	return ret;
}

CString CMgaOpenDlg::FilterInvalidCharacters(const CString& path, bool isPath)
{
	CString str = path;
	// are there invalid characters in the suggested file name?
	if (str.FindOneOf( _T("\\/:*?\"<>|")) != -1)
	{
		TCHAR nc = _T('_');
		int rpl = 0;
		if (!isPath) {
			rpl += str.Replace(_T('\\'), nc);
			rpl += str.Replace(_T('/'), nc);
			rpl += str.Replace(_T(':'), nc);
		}
		rpl += str.Replace(_T('*'), nc);
		rpl += str.Replace(_T('?'), nc);
		rpl += str.Replace(_T('"'), nc);
		rpl += str.Replace(_T('<'), nc);
		rpl += str.Replace(_T('>'), nc);
		rpl += str.Replace(_T('|'), nc);
		// rpl chars replaced in total
	}
	return str;
}

void CMgaOpenDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMgaOpenDlg)
	DDX_Radio(pDX, IDC_RADIO2, m_radio);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMgaOpenDlg, CDialog)
	//{{AFX_MSG_MAP(CMgaOpenDlg)
	ON_BN_CLICKED(IDC_BACK, OnBack)
	ON_BN_CLICKED(IDC_NEXT, OnNext)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMgaOpenDlg message handlers


BOOL CMgaOpenDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	GetDlgItem(IDC_BACK)->EnableWindow(flag_back);
	SetWindowText(title);
	GetDlgItem(IDC_RADIO2)->SetWindowText(filemsg);
    GetDlgItem(IDC_RADIO4)->SetWindowText(xmlfilemsg);
#if defined(_M_X64)
	GetDlgItem(IDC_RADIO4)->EnableWindow(false);
#endif

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMgaOpenDlg::OnBack() 
{
	// TODO: Add your control notification handler code here

	pressed_back = true;
	CDialog::OnOK();
}


void CMgaOpenDlg::OnNext() 
{
	// TODO: Add your control notification handler code here

	pressed_back = false;
	CDialog::OnOK();
}

void CMgaOpenDlg::SetFileNameHint(const CString& hint)
{
	fileNameHint = FilterInvalidCharacters(hint);
}

void CMgaOpenDlg::SetFolderPathHint(const CString& hint)
{
	folderPathHint = FilterInvalidCharacters(hint, true);
}
