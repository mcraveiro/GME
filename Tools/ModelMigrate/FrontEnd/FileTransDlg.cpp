// FileTransDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CMgaXslt.h"
#include "ModelMigrate.h"
#include "FileTransDlg.h"
#include "RuleEditorDlg.h"
#include "Validator.h"
#include "OptionsDlg.h"
#include "AboutDlg.h"

#include <afxwin.h>
#include <comdef.h>
#include ".\filetransdlg.h"

#define DEF_APPEND_STR                              "_out"
#define DEF_OUTPUTDIR_STR                           ""
#define DEF_INTERMEDDIR_STR                         ""
/*static*/ CString FileTransDlg::m_strSection       = "Options";
/*static*/ CString FileTransDlg::m_strDirStringItem = "DefaultDir";
/*static*/ CString FileTransDlg::m_strScrStringItem = "DefaultScr";
/*static*/ CString FileTransDlg::m_strAppendStringItem          = "AppendedString";
/*static*/ CString FileTransDlg::m_strAppendStr                 = DEF_APPEND_STR;
/*static*/ CString FileTransDlg::m_strOutputDirStringItem       = "OutputDir";
/*static*/ CString FileTransDlg::m_strOutputDirStr              = DEF_OUTPUTDIR_STR;
/*static*/ CString FileTransDlg::m_strIntermedDirStringItem     = "IntermedDir";
/*static*/ CString FileTransDlg::m_strIntermedDirStr            = DEF_INTERMEDDIR_STR;
// FileTransDlg dialog

IMPLEMENT_DYNAMIC(FileTransDlg, CDialog)
FileTransDlg::FileTransDlg(CWnd* pParent /*=NULL*/)
	: CDialog(FileTransDlg::IDD, pParent)
	, m_cx( 0)
	, m_cy( 0)
	, m_dlgRuleEditor( 0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

FileTransDlg::~FileTransDlg()
{
	closeDlgRuleEditor(IDCANCEL);
	m_dlgRuleEditor = 0;
}

void FileTransDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LISTFILES, m_lstFiles);
	DDX_Control(pDX, IDC_EDIT4, m_log);
	DDX_Control(pDX, IDC_LISTSCRIPT, m_lstScript);
}


BEGIN_MESSAGE_MAP(FileTransDlg, CDialog)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedRuleEditor)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON6, OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTONCLEAR, OnBnClickedButtonclear)
	ON_BN_CLICKED(IDC_BUTTON8, OnBnClickedButton8)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_NOTIFY(LVN_KEYDOWN, IDC_LISTSCRIPT, OnLvnKeydownListscript)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LISTFILES, OnLvnKeydownListfiles)
END_MESSAGE_MAP()

// FileTransDlg message handlers

BOOL FileTransDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	//HACCEL hA = LoadAccelerators( 0, "IDR_MAINACCELERATOR");
	///BOOL res = TranslateAccelerator( GetSafeHwnd(), hA, 0);

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon


	m_lstFiles.m_parent = this;
	m_lstScript.m_parent = this;

	CWinApp* pApp = AfxGetApp();

	CString strV1, strV2, strV3, strV4, strV5 ;
	loadMyOptions( strV1, strV2, strV3, strV4, strV5);

	loadDir( strV1);
	CString conn( strV2);
	if( !strV2.IsEmpty())
	{
		int pos = conn.ReverseFind('\\');
				
		if( pos != -1)
			m_lstScript.addFile( conn.Right( conn.GetLength() - pos - 1), conn);
		else
			m_lstScript.addFile( conn, conn);
	}

	FileTransDlg::m_strOutputDirStr = strV4;
	FileTransDlg::m_strIntermedDirStr = strV5;

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

int FileTransDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rect;
	GetWindowRect( &rect);
	m_cy = rect.bottom;
	m_cx = rect.right;

	return 0;
}

void FileTransDlg::OnSizing(UINT fwSide, LPRECT pRect)
{
	const int desiredX = 300, desiredY = 300;
	if( pRect->bottom - pRect->top > desiredY
	 && pRect->right - pRect->left > desiredX)
		CDialog::OnSizing(fwSide, pRect);
	else
	{
		if( pRect->bottom - pRect->top < desiredY) pRect->bottom = pRect->top + desiredY;
		if( pRect->right - pRect->left < desiredX) pRect->right = pRect->left + desiredX;
	}
}

void FileTransDlg::OnSize(UINT nType, int cx, int cy)
{
	if( cx < 150) cx = 150; // these are not preventing the user to make the window to small
	if( cy < 150) cy = 150; // just prevent the controls to be shuffled when cx,cy are very close to 0

	CDialog::OnSize(nType, cx, cy);
	if( nType == SIZE_MINIMIZED) return;
	m_cx = cx;
	m_cy = cy;
	CRect rect, r0, r1, r2;
	int sy = 0;
	if( cx && cy && m_lstScript.GetSafeHwnd() && m_lstFiles.GetSafeHwnd() && m_log.GetSafeHwnd()) // if minimized cx == cy == 0
	{
		m_lstScript.GetWindowRect( &r0); ScreenToClient( &r0);
		m_lstFiles.GetWindowRect( &r1); ScreenToClient( &r1);
		m_log.GetWindowRect( &r2);   ScreenToClient( &r2);

		m_lstScript.SetWindowPos( 0, 0, 0, cx - 2*r0.left, r0.Height(), SWP_NOMOVE|SWP_NOZORDER|SWP_SHOWWINDOW);

		rect = r1;

		sy = (cy - r1.top)*3/5;

		rect.bottom = rect.top + sy;
		
		rect.right  = cx - rect.left; // maintain margin
		m_lstFiles.SetWindowPos( 0, 0, 0, rect.Width(), rect.Height(), SWP_NOMOVE|SWP_NOZORDER|SWP_SHOWWINDOW);
		rect.top = rect.bottom + r2.top - r1.bottom;
		rect.bottom = cy - 13;
		rect.right  = cx - rect.left; // maintain margin
		m_log.SetWindowPos( 0, rect.left, rect.top, rect.Width(), rect.Height(), SWP_NOZORDER|SWP_SHOWWINDOW);
	}
}

void FileTransDlg::OnBnClickedRuleEditor()
{
	ruleEditor();
}

void FileTransDlg::OnBnClickedButton2()
{
	selDir();
}


void FileTransDlg::OnBnClickedButton6()
{
	selFile();
}

void FileTransDlg::OnBnClickedButton3()
{
	selScript();
}

void FileTransDlg::OnBnClickedButton5()
{
	selScriptDir();
}

void FileTransDlg::OnBnClickedButton4()
{
	processAll();
}

void FileTransDlg::OnBnClickedButton8()
{
	processSel();
}

void FileTransDlg::options()
{
	OptionsDlg dlg;
	// load the current values from registry
	loadMyOptions( dlg.m_dir, dlg.m_scr, dlg.m_append, dlg.m_outputDir, dlg.m_intermediateFilesDir);

	if( dlg.DoModal() == IDOK)
	{
		// store the freshly modified options in static members
		FileTransDlg::m_strAppendStr = dlg.m_append;
		FileTransDlg::m_strOutputDirStr = dlg.m_outputDir;
		FileTransDlg::m_strIntermedDirStr = dlg.m_intermediateFilesDir;

		// save into registry
		saveMyOptions( dlg.m_dir, dlg.m_scr, dlg.m_append, dlg.m_outputDir, dlg.m_intermediateFilesDir);
	}
}

void FileTransDlg::ruleEditor()
{
	// stored by a member variable
	if( !m_dlgRuleEditor)
	{
		m_dlgRuleEditor = new RuleEditorDlg();
	}
	if( m_dlgRuleEditor->m_closed)
	{
		m_dlgRuleEditor->Create( RuleEditorDlg::IDD, this);
		m_dlgRuleEditor->ShowWindow( SW_SHOWNORMAL);
	}

	m_dlgRuleEditor->SetActiveWindow();
}

void FileTransDlg::validatorGen()
{
	CFileDialog dlg( TRUE, 0, 0, 0,
		"Paradigms (*.xmp)|*.xmp|Parsed Paradigms (*.parsed)|*.parsed|All Files (*.*)|*.*||");
	if( dlg.DoModal() == IDOK)
	{
		CString fn = dlg.GetPathName();
		int pos = fn.Find( '.');
		CString defname = fn.Left( pos) + "_validator.xsl";
		CFileDialog sdlg( FALSE, ".xsl", defname, OFN_OVERWRITEPROMPT, 
			"XSLT Files (*.xsl;*.xslt)|*.xsl; *.xslt|All Files (*.*)|*.*||");

		if( sdlg.DoModal() == IDOK)
		{
			Validator::job( dlg.GetPathName(), sdlg.GetPathName());
		}
	}
}

void FileTransDlg::selFile()
{
	CFileDialog dlg( TRUE, "xme", 0, OFN_ALLOWMULTISELECT|OFN_EXPLORER,
		"XME Files (*.xme;*.xml)|*.xme; *.xml|All Files (*.*)|*.*||");
	if( dlg.DoModal() == IDOK)
	{
		//m_lstFiles.addFile( dlg.GetFileName(), dlg.GetPathName());
		POSITION pos = dlg.GetStartPosition();
		while( pos)
		{
			CString path = dlg.GetNextPathName( pos);
			CFileFind ff;
			if(ff.FindFile(path))
			{
				ff.FindNextFile();
				m_lstFiles.addFile( ff.GetFileName(), path);
			}
		}
	}

}

void FileTransDlg::selDir()
{
	CString d = FileTransDlg::getDirectory();
	if( !d.IsEmpty())
		loadDir( d);
}

void FileTransDlg::loadDir( CString pDir)
{
	CFileFind finder;
	
	BOOL bWorking = finder.FindFile( pDir + "\\*.xme");
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		m_lstFiles.addFile( finder.GetFileName(), finder.GetFilePath());
	}
}


void FileTransDlg::process( const std::vector< std::string>& pToRun)
{
	std::vector< std::string> scripts = m_lstScript.getAllFiles();
	if( scripts.size() == 0 || scripts[0].empty()) { appendLn( "Invalid or no entries in script list!"); return; }
	bool multi_step = scripts.size() > 1;

	// we test wether output files can be created safely (without overwriting input files)
	bool do_append_filenames = false;
	if( FileTransDlg::m_strAppendStr.IsEmpty()                  // no appended string
	&& ( FileTransDlg::m_strOutputDirStr.IsEmpty()            // and no valid target dir
		|| !directoryExists( FileTransDlg::m_strOutputDirStr)))
	{
		appendLn( "Invalid options found, output filename must be different from input filename, see Options menu!");
		appendLn( CString( "Using the input directory as the target directory, filenames will be appended with \"") + DEF_APPEND_STR + "\"!");
		FileTransDlg::m_strAppendStr = DEF_APPEND_STR;
	}

	CString xsltScript, f_In, f_Out;

	appendLn( "");
	appendLn( "Processing started on " + CTime::GetCurrentTime().Format("%b %d, %H:%M:%S"));

	std::vector<std::string> ok;
	std::vector<std::string> fail;
	for( unsigned int i = 0; i < pToRun.size(); ++i)
	{
		CString err;
		f_Out = f_In = pToRun[i].c_str();

		// f_Out to be suffixed with the m_strAppendStr (no extension yet)
		int dpos = f_Out.ReverseFind('.');
		if( dpos == -1) f_Out += FileTransDlg::m_strAppendStr;
		else f_Out = f_Out.Left( dpos) + FileTransDlg::m_strAppendStr;

		if(!multi_step)
		{
			xsltScript = scripts[0].c_str();
			f_Out = FileTransDlg::placeIntoTargetDir( f_Out + ".xme");

			apply( xsltScript, f_In, f_Out, err);

			// analyze error
			if( !err.IsEmpty())
			{
				fail.push_back( pToRun[i]); // f_In
				//appendLn( f_In + " transformation failed: " + err);
				appendLn( "Transformation failed. Error msg: " + err + " [Infile: " + f_In + "]" );
			}
			else
			{
				ok.push_back( (LPCTSTR) f_Out);
				//appendLn( f_In + " transformation OK.");
				appendLn( "Transformation OK. Infile: " + f_In + ". Outfile: " + f_Out);
			}
		}
		else // multi step processing
		{
			CString f_OutPattern = FileTransDlg::placeIntoIntermedDir( f_Out);
			CString f_OutFinal = FileTransDlg::placeIntoTargetDir( f_Out + ".xme");

			bool any_error = false;
			appendLn( "Start processing " + f_In);

			// do the multi step processing
			for( unsigned int j = 0; err.IsEmpty() && j < scripts.size(); ++j)
			{
				err.Empty();				

				if( j+1 != scripts.size()) // the intermediate files look like: a_out01.xme
				{
					CString j_str;
					j_str.Format( "%.2d", j+1);
					f_Out = f_OutPattern + j_str + ".xme";
				}
				else // the last step
					f_Out = f_OutFinal;

				xsltScript = scripts[j].c_str();

				// invoke step i
				apply( xsltScript, f_In, f_Out, err);

				CString j_str;
				j_str.Format( "%d", j+1);
				if( !err.IsEmpty())
				{
					//fail.push_back( pToRun[i]); // the original file is marked
					any_error = true;
					//appendLn( f_In + " step " + j_str + " of transformation failed: " + err);
					appendLn( "Transformation step " + j_str + " failed. Error msg: " + err + " [Infile: " + f_In + "]" );
				}
				else
				{
					//ok.push_back( (LPCTSTR) f_Out);
					//appendLn( f_In + " step " + j_str + " of transformation OK.");
					appendLn( "Transformation step " + j_str + " OK. Infile: " + f_In + ". Outfile: " + f_Out);
				}

				f_In = f_Out;
			}

			if( any_error) 
			{
				fail.push_back( pToRun[i]); // not == with p_In
				appendLn( "");
			}
			else
			{
				ok.push_back( (LPCTSTR) f_Out);
				appendLn( "Succesfully transformed " + CString( pToRun[i].c_str()) + " to " + f_Out);
				appendLn( "");
			}
		}
	}

	// mark the failed file list with 'failed'
	m_lstFiles.mark( fail);

	appendLn( CString( '=', 80));
	for( unsigned int i = 0; i < ok.size(); ++i)
	{
		if( i == 0) appendLn( "Successful artifacts:");
		appendLn( ok[i].c_str());
	}
	appendLn( CString( '-', 160));
	for( unsigned int i = 0; i < fail.size(); ++i)
	{
		if( i == 0) appendLn( "Failed tranformations:");
		appendLn( fail[i].c_str());
	}
}

void FileTransDlg::processSel()
{
	process( m_lstFiles.getSelFiles());
}

void FileTransDlg::processAll()
{
	process( m_lstFiles.getAllFiles());
}

void FileTransDlg::selScript()
{
	CFileDialog dlg( TRUE, "xsl", 0, OFN_ALLOWMULTISELECT|OFN_EXPLORER,
		"XSLT Files (*.xsl;*.xslt)|*.xsl; *.xslt|All Files (*.*)|*.*||");
	if( dlg.DoModal() == IDOK)
	{
		//m_lstScript.addFile( dlg.GetFileName(), dlg.GetPathName());
		POSITION pos = dlg.GetStartPosition();
		while( pos)
		{
			CString path = dlg.GetNextPathName( pos);
			CFileFind ff;
			if(ff.FindFile(path))
			{
				ff.FindNextFile();
				m_lstScript.addFile( ff.GetFileName(), path);
			}
		}
	}
}

void FileTransDlg::selScriptDir()
{
	CString d = FileTransDlg::getDirectory( "Load .xsl and .xslt files from directory");
	if( !d.IsEmpty())
		loadScriptDir( d);
}

void FileTransDlg::loadScriptDir( CString pDir)
{
	CFileFind finder;
	
	BOOL bWorking = finder.FindFile( pDir + "\\*.xsl");
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		m_lstScript.addFile( finder.GetFileName(), finder.GetFilePath());
	}

	bWorking = finder.FindFile( pDir + "\\*.xslt");
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		m_lstScript.addFile( finder.GetFileName(), finder.GetFilePath());
	}
}

void FileTransDlg::apply(CString xsltFilename, CString inputXmlFilename, CString outputXmlFilename, CString& pErrMsg)
{
	_bstr_t err;
	CXslt::doNativeXslt(xsltFilename, inputXmlFilename, outputXmlFilename, err);
	pErrMsg = static_cast<const TCHAR*>(err);
}

void FileTransDlg::loadMyOptions( CString& pStrVal1, CString& pStrVal2, CString& pStrVal3, CString& pStrVal4, CString& pStrVal5)
{
	CWinApp* pApp = AfxGetApp();

	pStrVal1 = pApp->GetProfileString( m_strSection, m_strDirStringItem);
	pStrVal2 = pApp->GetProfileString( m_strSection, m_strScrStringItem);
	pStrVal3 = pApp->GetProfileString( m_strSection, m_strAppendStringItem);
	pStrVal4 = pApp->GetProfileString( m_strSection, m_strOutputDirStringItem);
	pStrVal5 = pApp->GetProfileString( m_strSection, m_strIntermedDirStringItem);
}

void FileTransDlg::saveMyOptions( const CString& pStrVal1, const CString& pStrVal2, const CString& pStrVal3, const CString& pStrVal4, const CString& pStrVal5)
{
	CWinApp* pApp = AfxGetApp();

	pApp->WriteProfileString( m_strSection, m_strDirStringItem, pStrVal1);
	pApp->WriteProfileString( m_strSection, m_strScrStringItem, pStrVal2);
	pApp->WriteProfileString( m_strSection, m_strAppendStringItem, pStrVal3);
	pApp->WriteProfileString( m_strSection, m_strOutputDirStringItem, pStrVal4);
	pApp->WriteProfileString( m_strSection, m_strIntermedDirStringItem, pStrVal5);
}

CString FileTransDlg::getDirectory( const CString& text)
{
	BROWSEINFO bi;
	TCHAR szDir[MAX_PATH];
	CString Dir = "";
	LPITEMIDLIST pidl;
	LPMALLOC pMalloc;	
	OleInitialize(NULL);
	if (SUCCEEDED(SHGetMalloc(&pMalloc)))
	{
		ZeroMemory(&bi,sizeof(bi));
		bi.hwndOwner = NULL;
		bi.pszDisplayName = 0;
		bi.lpszTitle = text;
		bi.pidlRoot = 0;
		bi.ulFlags = BIF_BROWSEINCLUDEFILES | BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT | BIF_EDITBOX | BIF_VALIDATE ;
		bi.lpfn = NULL;
		//Displays a dialog box that enables the user to select a shell folder
		pidl = SHBrowseForFolder(&bi) ;
		if (pidl)
		{
			// Converts an item identifier list to a file system path
			if (!SHGetPathFromIDList(pidl,szDir))
			{
				pMalloc->Free(pidl) ;
				pMalloc->Release() ;
			}
			pMalloc->Free(pidl);
			pMalloc->Release();
		}
		else
		{	// Selected 'Not to save'
			return "";
		}
		Dir = szDir;
		//Dir += "\\";
	}
	// end of getting directory info 
	return Dir;
}

void FileTransDlg::closeDlgRuleEditor( int nRes)
{
	if( m_dlgRuleEditor)
	{
		if( m_dlgRuleEditor->GetSafeHwnd())
		{
			m_dlgRuleEditor->EndDialog( nRes);
			m_dlgRuleEditor->DestroyWindow();
		}
		delete m_dlgRuleEditor;
		m_dlgRuleEditor = 0;
	}
}

void FileTransDlg::OnOK()
{
	//IDOK is used for other purposes
	//closeDlgRuleEditor( IDOK);
}

void FileTransDlg::OnCancel()
{
	closeDlgRuleEditor( IDCANCEL);

	EndDialog(IDCANCEL);
	DestroyWindow();
}

void FileTransDlg::quit()
{
	closeDlgRuleEditor( IDCANCEL);
	EndDialog( IDCANCEL);
	DestroyWindow();
}

void FileTransDlg::appendLn( CString t)
{
	appendLog( t);
	appendLog( "\r\n");
}

void FileTransDlg::appendLog( CString t)
{
	CString old;
	m_log.GetWindowText( old);

	old.Append( t);

	m_log.SetWindowText( old);

	m_log.LineScroll( m_log.GetLineCount());
}

void FileTransDlg::clearLog()
{
	m_log.SetWindowText("");
}

bool FileTransDlg::directoryExists( const CString& dir)
{
	bool res = false;
	CFileStatus stat;
	if( !dir.IsEmpty()                   // not-empty string
		&& CFile::GetStatus( dir, stat)) // status fetched succesfully
	{
		if( (stat.m_attribute & CFile::directory) == CFile::directory // it is a directory
			&& (stat.m_attribute & CFile::readOnly)  == 0) // not read only
		{
			res = true;
		}
	}

	return res;
}

bool FileTransDlg::fileExists( const CString& file)
{
	bool res = false;
	CFileStatus stat;
	if( !file.IsEmpty()                   // not-empty string
		&& CFile::GetStatus( file, stat)) // status fetched succesfully
	{
		if( (stat.m_attribute & CFile::directory) == 0     // it is NOT a directory
			&& (stat.m_attribute & CFile::readOnly)  == 0) // not read only
		{
			res = true;
		}
	}

	return res;
}

void FileTransDlg::OnBnClickedButtonclear()
{
	clearLog();
}


void FileTransDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		AboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

void FileTransDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

BOOL FileTransDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch( wParam) {
		case IDOK: onReturn();                                    break; 
		case ID_SCRIPTS_ADD: selScript();                         break;
		case ID_SCRIPTS_ADDDIRECTORY: selScriptDir();             break;
		case ID_SCRIPTS_REMOVEALL: m_lstScript.removeAll();       break;
		case ID_SCRIPTS_REMOVESELECTED: m_lstScript.removeSel();  break;
		case ID_FILES_ADDFILE: selFile();                         break;
		case ID_FILES_ADDDIRECTORY: selDir();                     break;
		case ID_FILES_REMOVEALL: m_lstFiles.removeAll();          break;
		case ID_FILES_REMOVESELECTED: m_lstFiles.removeSel();     break;
		case ID_RULES_EDITRULES: ruleEditor();                    break;
		case ID_RULES_VALIDATE: validatorGen();                   break;
		case ID_PROCESSING_PROCESSALL: processAll();              break;
		case ID_PROCESSING_PROCESSSELECTED: processSel();         break;
		case ID_MISC_OPTIONS: options();                          break;
		case ID_MISC_CLEARLOG: clearLog();                        break;
		case ID_FILES_EXIT:    quit();                            break;
	};

	if( wParam == ID_FILES_EXIT) return TRUE;
	return CDialog::OnCommand(wParam, lParam);
}

void FileTransDlg::onReturn()
{
	CWnd* child = this->GetFocus();
	if( !child) return;
	if( child == GetDlgItem(IDC_LISTSCRIPT))
	{ ; } // do nothing
	else if( child == GetDlgItem(IDC_LISTFILES))
		processSel();
}

void FileTransDlg::OnLvnKeydownListscript(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);
	NMLVKEYDOWN &s = *pLVKeyDow;

	bool cntrl = GetKeyState( VK_CONTROL) < 0;
	bool shft  = GetKeyState( VK_SHIFT) < 0;
	bool alt   = GetKeyState( VK_MENU) < 0; // alt key

	if( s.wVKey == VK_APPS) m_lstScript.showPopupMenu();
	if( s.wVKey == VK_F3 || s.wVKey == VK_INSERT && !cntrl) // F3, INS - Add Script
		SendMessage( WM_COMMAND, ID_SCRIPTS_ADD, 0);

	if( s.wVKey == VK_F4 && !alt || s.wVKey == VK_INSERT && cntrl) // F4, CTRL INS - Add Script Dir (but not ALT+F4)
		SendMessage( WM_COMMAND, ID_SCRIPTS_ADDDIRECTORY, 0);

	if( s.wVKey == VK_DELETE && !cntrl)
		SendMessage( WM_COMMAND, ID_SCRIPTS_REMOVESELECTED, 0);  // DEL - Remove Sel
	if( s.wVKey == VK_DELETE && cntrl)
		SendMessage( WM_COMMAND, ID_SCRIPTS_REMOVEALL, 0);       // CTRL DEL - Remove All
	

	if(( s.wVKey == IDOK || s.wVKey == VK_RETURN || s.wVKey == VK_F5) && !cntrl) // F5 - Run Sel
		SendMessage( WM_COMMAND, ID_PROCESSING_PROCESSSELECTED, 0);
	if(( s.wVKey == IDOK || s.wVKey == VK_RETURN || s.wVKey == VK_F5) && cntrl) // CTRL F5 - Run All
		SendMessage( WM_COMMAND, ID_PROCESSING_PROCESSALL, 0);


	*pResult = 0;
}

void FileTransDlg::OnLvnKeydownListfiles(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);
	NMLVKEYDOWN &s = *pLVKeyDow;

	bool cntrl = GetKeyState( VK_CONTROL) < 0;
	bool shft =  GetKeyState( VK_SHIFT) < 0;
	if( s.wVKey == VK_APPS) m_lstFiles.showPopupMenu();
	
	if( s.wVKey == VK_F6 || s.wVKey == VK_INSERT && !cntrl) // F6, INS - Add File
		SendMessage( WM_COMMAND, ID_FILES_ADDFILE, 0);

	if( s.wVKey == VK_F7 || s.wVKey == VK_INSERT && cntrl) // F7, CTRL INS - Add Dir
		SendMessage( WM_COMMAND, ID_FILES_ADDDIRECTORY, 0);

	if( s.wVKey == VK_DELETE && !cntrl)                    // DEL - Remove Sel
		SendMessage( WM_COMMAND, ID_FILES_REMOVESELECTED, 0);
	if( s.wVKey == VK_DELETE && cntrl)                     // CTRL DEL - Remove All
		SendMessage( WM_COMMAND, ID_FILES_REMOVEALL, 0);

	if(( s.wVKey == IDOK || s.wVKey == VK_RETURN || s.wVKey == VK_F5) && !cntrl) // F5 - Run Sel
		SendMessage( WM_COMMAND, ID_PROCESSING_PROCESSSELECTED, 0);
	if(( s.wVKey == IDOK || s.wVKey == VK_RETURN || s.wVKey == VK_F5) && cntrl) // CTRL F5 - Run All
		SendMessage( WM_COMMAND, ID_PROCESSING_PROCESSALL, 0);

	*pResult = 0;
}

/*static*/ CString FileTransDlg::placeIntoTargetDir( const CString& pFile)
{
	CString file = pFile;	
	// change the directory part of the path if specified and exists
	if( directoryExists( FileTransDlg::m_strOutputDirStr))    // status fetched succesfully
	{
		int dpos = file.ReverseFind('\\');
		if( dpos == -1) // not found '\\'
		{
			file.Insert( 0, '\\');
			file.Insert( 0, FileTransDlg::m_strOutputDirStr);
		}
		else
		{
			file = FileTransDlg::m_strOutputDirStr + '\\' + file.Right( file.GetLength() - dpos - 1);
		}
	}
	return file;
}

/*static*/ CString FileTransDlg::placeIntoIntermedDir( const CString& pFile)
{
	CString file = pFile;	
	// change the directory part of the path to the intermediate directory if specified
	if( directoryExists( FileTransDlg::m_strIntermedDirStr))    // status fetched succesfully
	{
		int dpos = file.ReverseFind('\\');
		if( dpos == -1) // not found '\\'
		{
			file.Insert( 0, '\\');
			file.Insert( 0, FileTransDlg::m_strIntermedDirStr);
		}
		else
		{
			file = FileTransDlg::m_strIntermedDirStr + '\\' + file.Right( file.GetLength() - dpos - 1);
		}
	}
	return file;
}
