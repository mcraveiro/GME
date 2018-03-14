#include "stdafx.h"

#include "GMEVersion.h"
#include "GMEApp.h"
/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	CStatic	m_bmpAbout;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	afx_msg void OnAboutPicture();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeRichedit21();
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Control(pDX, IDC_BMPABOUT, m_bmpAbout);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	ON_BN_CLICKED(IDC_BMPABOUT, OnAboutPicture)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CGMEApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

void CAboutDlg::OnAboutPicture() 
{
	// TODO: Add your control notification handler code here
	CDLLInfoDlg dllDlg;
	dllDlg.DoModal();		
}

BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CStatic* version = (CStatic*)GetWindow(GW_CHILD);
	ASSERT(version);

#if defined(_M_IX86)
	version->SetWindowTextW(CString(GME_VERSION_STR) + " x86");
#elif defined(_M_X64)
	version->SetWindowTextW(CString(GME_VERSION_STR) + " x64");
#else
#error Unknown arch
#endif

	CRect rectAbout;
	GetClientRect( rectAbout );
	CBitmap bmpAbout;
	bmpAbout.LoadBitmap( IDB_ABOUT );
	BITMAP bmpStruct;
	bmpAbout.GetBitmap( &bmpStruct );
	m_bmpAbout.MoveWindow( ( rectAbout.Width() - bmpStruct.bmWidth )  / 2, rectAbout.TopLeft().y + 10, bmpStruct.bmWidth, bmpStruct.bmHeight );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}



/////////////////////////////////////////////////////////////////////////////
// CDLLInfoDlg dialog


CDLLInfoDlg::CDLLInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDLLInfoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDLLInfoDlg)
	//}}AFX_DATA_INIT
}


void CDLLInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDLLInfoDlg)
	DDX_Control(pDX, IDC_DLL_LIST, m_DLLList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDLLInfoDlg, CDialog)
	//{{AFX_MSG_MAP(CDLLInfoDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDLLInfoDlg message handlers
#include <Tlhelp32.h>

static BOOL EnumModules(HMODULE *modules, DWORD modsize, LPDWORD needed)
{
	
      OSVERSIONINFO  osver ;
      HINSTANCE      hInstLib ;
      HANDLE         hSnapShot ;
      MODULEENTRY32  moduleentry ;
      HANDLE         hProcess ;
	  BOOL			 bFlag;
      

	  hProcess = ::GetCurrentProcess();
	  if (!hProcess)
		  return FALSE;
	  	
      // ToolHelp Function Pointers.
      HANDLE (WINAPI *lpfCreateToolhelp32Snapshot)(DWORD,DWORD) ;
      BOOL (WINAPI *lpfModule32First)(HANDLE,LPMODULEENTRY32) ;
      BOOL (WINAPI *lpfModule32Next)(HANDLE,LPMODULEENTRY32) ;

      // PSAPI Function Pointers.
      BOOL (WINAPI *lpfEnumProcessModules)( HANDLE, HMODULE *,
         DWORD, LPDWORD );
      	
      osver.dwOSVersionInfoSize = sizeof( osver ) ;
      if( !GetVersionEx( &osver ) )
      {
         return FALSE ;
      }

	
      // If Windows NT:
      if( osver.dwPlatformId == VER_PLATFORM_WIN32_NT )
      {
         hInstLib = LoadLibraryA( "PSAPI.DLL" ) ;
         if( hInstLib == NULL )
            return FALSE ;

         // Get procedure addresses.
         lpfEnumProcessModules = (BOOL(WINAPI *)(HANDLE, HMODULE *,
            DWORD, LPDWORD)) GetProcAddress( hInstLib,
            "EnumProcessModules" ) ;
         if(!lpfEnumProcessModules) {
			 FreeLibrary( hInstLib );
			 return FALSE ;
         }
         if( !lpfEnumProcessModules( hProcess, modules, modsize, needed ) ) {
			 FreeLibrary( hInstLib ) ;
			 return FALSE;
         }
         FreeLibrary( hInstLib ) ;
	  }
	  else if( osver.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS ) {
		  hInstLib = LoadLibraryA( "Kernel32.DLL" ) ;
		  if( hInstLib == NULL )
			  return FALSE ;		
		  
         lpfCreateToolhelp32Snapshot=
            (HANDLE(WINAPI *)(DWORD,DWORD))
            GetProcAddress( hInstLib,
            "CreateToolhelp32Snapshot" ) ;
         lpfModule32First=
            (BOOL(WINAPI *)(HANDLE,LPMODULEENTRY32))
            GetProcAddress( hInstLib, "Module32First" ) ;
         lpfModule32Next=
            (BOOL(WINAPI *)(HANDLE,LPMODULEENTRY32))
            GetProcAddress( hInstLib, "Module32Next" ) ;
         if( lpfModule32Next == NULL ||
            lpfModule32First == NULL ||
            lpfCreateToolhelp32Snapshot == NULL )
         {
            FreeLibrary( hInstLib ) ;
            return FALSE ;
         }
		 hSnapShot = lpfCreateToolhelp32Snapshot(
            TH32CS_SNAPMODULE, 0 ) ;
         if( hSnapShot == INVALID_HANDLE_VALUE )
         {
            FreeLibrary( hInstLib ) ;
            return FALSE ;
         }
		 moduleentry.dwSize = sizeof(MODULEENTRY32);
		 bFlag = lpfModule32First( hSnapShot, &moduleentry );
		 unsigned int cntMod = 0;
		 while (bFlag) {
			 if ( (modsize/sizeof(HMODULE)) > cntMod) {
				modules[cntMod++] = moduleentry.hModule;
			 }
			 bFlag = lpfModule32Next( hSnapShot, &moduleentry );
		 }
		 *needed = cntMod*sizeof(HMODULE);
		 FreeLibrary( hInstLib ) ;
	  }
	  else {
		  return FALSE;
	  }
	  return TRUE;
		 

}

BOOL CDLLInfoDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CRect rect;
	m_DLLList.GetClientRect(&rect);
	m_DLLList.InsertColumn(0, _T("Dynamic Library Path"), LVCFMT_LEFT, rect.Size().cx-::GetSystemMetrics(SM_CXVSCROLL));
	// TODO: Add extra initialization here
	HMODULE modules[1024];
	DWORD	needed;
	bool	success = true;

	if (EnumModules(modules, sizeof(modules), &needed)) {
		if (needed > sizeof(modules)) {
			success = false;			// Not so nice...
		}
		for (unsigned int i=0; i< ((needed/sizeof(HMODULE))-1); i++) {
			TCHAR moduleName[255];
			if (::GetModuleFileName(modules[i], moduleName, (sizeof(moduleName)/sizeof(TCHAR)) )) {
				m_DLLList.InsertItem(i, moduleName);
			}
			else {
				success = false;
			}
		}
	}
	else {
		success = false;
	}
	
	if (!success) {
		AfxMessageBox(_T("Error in EnumProcessModules()."));
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
void CAboutDlg::OnEnChangeRichedit21()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}
