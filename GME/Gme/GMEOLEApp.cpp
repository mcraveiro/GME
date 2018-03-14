// GMEOLEApp.cpp : implementation file
//

#include "stdafx.h"
#include "gme.h"
#include "parser.h"
#include "GMEOLEApp.h"
#include "GMEOLEError.h"
#include "GMEApp.h"
#include "GMEView.h"
#include "GMEDoc.h"
#include "MainFrm.h"
#include "GMEVersion.h"
#include "GMEOLEColl.h"
#include "GMEOLEModel.h"
#include "GMEOLEPanel.h"
#include "GMEOLEIt.h"
#include "GMEEventLogger.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGMEOLEApp

IMPLEMENT_DYNCREATE(CGMEOLEApp, CCmdTarget)

CGMEOLEApp::CGMEOLEApp()
{
	EnableAutomation();
	
	// To keep the application running as long as an OLE automation 
	//	object is active, the constructor calls AfxOleLockApp.

	AfxOleLockApp();
}

CGMEOLEApp::~CGMEOLEApp()
{
	// To terminate the application when all objects created with
	// 	with OLE automation, the destructor calls AfxOleUnlockApp.

	AfxOleUnlockApp();
}


void CGMEOLEApp::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CCmdTarget::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(CGMEOLEApp, CCmdTarget)
	//{{AFX_MSG_MAP(CGMEOLEApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// comment by Zoltan Molnar: if you happen to enhance the public interface please make sure that 
// the dispIDs (found in gme.idl) correspond with the order of macros in the dispatch map below
// hint: always add to the tail of the DISPATCH_MAP
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
BEGIN_DISPATCH_MAP(CGMEOLEApp, CCmdTarget)
	//{{AFX_DISPATCH_MAP(CGMEOLEApp)
	DISP_PROPERTY_EX(CGMEOLEApp, "Visible", GetVisible, SetVisible, VT_BOOL)
	DISP_PROPERTY_EX(CGMEOLEApp, "Version", GetVersion, SetVersion, VT_BSTR)
	DISP_PROPERTY_EX(CGMEOLEApp, "VersionMajor", GetVersionMajor, SetVersionMajor, VT_I2)
	DISP_PROPERTY_EX(CGMEOLEApp, "VersionMinor", GetVersionMinor, SetVersionMinor, VT_I2)
	DISP_PROPERTY_EX(CGMEOLEApp, "VersionPatchLevel", GetVersionPatchLevel, SetVersionPatchLevel, VT_I2)
	DISP_PROPERTY_EX(CGMEOLEApp, "Models", GetModels, SetModels, VT_DISPATCH)
	DISP_PROPERTY_EX(CGMEOLEApp, "MgaProject", GetMgaProject, SetMgaProject, VT_DISPATCH)
	DISP_PROPERTY_EX(CGMEOLEApp, "Panels", GetPanels, SetPanels, VT_DISPATCH)
	DISP_PROPERTY_EX(CGMEOLEApp, "ConsoleContents", GetConsoleContents, SetConsoleContents, VT_BSTR)

	DISP_FUNCTION(CGMEOLEApp, "CreateProject", CreateProject, VT_EMPTY, VTS_BSTR VTS_BSTR)
	DISP_FUNCTION(CGMEOLEApp, "OpenProject", OpenProject, VT_EMPTY, VTS_BSTR)
	DISP_FUNCTION(CGMEOLEApp, "CreateProjectDialog", CreateProjectDialog, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CGMEOLEApp, "OpenProjectDialog", OpenProjectDialog, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CGMEOLEApp, "CloseProject", CloseProject, VT_EMPTY, VTS_BOOL)
	DISP_FUNCTION(CGMEOLEApp, "SaveProject", SaveProject, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CGMEOLEApp, "SaveProjectAs", SaveProjectAs, VT_EMPTY, VTS_BSTR)
	DISP_FUNCTION(CGMEOLEApp, "SaveProjectAsDialog", SaveProjectAsDialog, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CGMEOLEApp, "ExportProject", ExportProject, VT_EMPTY, VTS_BSTR)
	DISP_FUNCTION(CGMEOLEApp, "ImportProject", ImportProject, VT_EMPTY, VTS_BSTR)
	DISP_FUNCTION(CGMEOLEApp, "ConstraintsDialog", ConstraintsDialog, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CGMEOLEApp, "CheckAllConstraints", CheckAllConstraints, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CGMEOLEApp, "RegisterParagimsDialog", RegisterParagimsDialog, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CGMEOLEApp, "RegisterComponentsDialog", RegisterComponentsDialog, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CGMEOLEApp, "RunComponent", RunComponent, VT_EMPTY, VTS_BSTR)
	DISP_FUNCTION(CGMEOLEApp, "RunComponentDialog", RunComponentDialog, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CGMEOLEApp, "SettingsDialog", SettingsDialog, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CGMEOLEApp, "Undo", Undo, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CGMEOLEApp, "Redo", Redo, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CGMEOLEApp, "ClearUndoQueue", ClearUndoQueue, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CGMEOLEApp, "ProjectPropertiesDialog", ProjectPropertiesDialog, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CGMEOLEApp, "ShowHelpContents", ShowHelpContents, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CGMEOLEApp, "ShowAbout", ShowAbout, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CGMEOLEApp, "ShowFCO", ShowFCO, VT_EMPTY, VTS_DISPATCH VTS_BOOL)
	DISP_FUNCTION(CGMEOLEApp, "ConsoleMessage", ConsoleMessage, VT_EMPTY, VTS_BSTR VTS_I4)
	DISP_FUNCTION(CGMEOLEApp, "ConsoleClear", ConsoleClear, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CGMEOLEApp, "ConsoleNavigateTo", ConsoleNavigateTo, VT_EMPTY, VTS_BSTR)

	DISP_FUNCTION(CGMEOLEApp, "ChangeEditmode", ChangeEditmode, VT_EMPTY, VTS_I4)
	DISP_FUNCTION(CGMEOLEApp, "GridShow", GridShow, VT_EMPTY, VTS_BOOL)
	DISP_FUNCTION(CGMEOLEApp, "AttributepanelPage", AttributepanelPage, VT_EMPTY, VTS_I4)

	DISP_PROPERTY_EX(CGMEOLEApp, "OleIt", GetOleIt, SetOleIt, VT_DISPATCH)

	DISP_FUNCTION(CGMEOLEApp, "DisableComp", DisableComp, VT_EMPTY, VTS_BSTR VTS_BOOL)
	DISP_FUNCTION(CGMEOLEApp, "DisableCompForKinds", DisableCompForKinds, VT_EMPTY, VTS_BSTR VTS_BSTR)
	DISP_FUNCTION(CGMEOLEApp, "SetCompFiltering", SetCompFiltering, VT_EMPTY, VTS_BOOL)
	DISP_FUNCTION(CGMEOLEApp, "GetCompFiltering", GetCompFiltering, VT_BOOL, VTS_NONE)
	DISP_FUNCTION(CGMEOLEApp, "SetWorkingDirectory", SetWorkingDirectory, VT_EMPTY, VTS_BSTR)
	DISP_FUNCTION(CGMEOLEApp, "Exit", Exit, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CGMEOLEApp, "RefreshLib", RefreshLib, VT_I4, VTS_BSTR VTS_BOOL)

	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// if you modify the interface IGmeOleApp: please see comment above how to
// keep in sync the DISPIDs (in gme.idl) and the order of DISP_FUNCTION macros
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

// Note: we add support for IID_IGMEOLEApp to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {81191A44-B898-4143-BF8B-CA7501FEC19A}
//static const IID_IGMEOLEApp = 
//{ 0x81191a44, 0xb898, 0x4143, { 0xbf, 0x8b, 0xca, 0x75, 0x1, 0xfe, 0xc1, 0x9a } };


BEGIN_INTERFACE_MAP(CGMEOLEApp, CCmdTarget)
	INTERFACE_PART(CGMEOLEApp, __uuidof(IGMEOLEApp), Dual)
	DUAL_ERRORINFO_PART(CGMEOLEApp)
END_INTERFACE_MAP()

// Note: we use an own IMPLEMENT_OLECREATE macro here in order to register a REGCLS_SINGLEUSE class factory
// For MDI OLE server's the default mechanism is REGCLS_MULTIPLEUSE (bMultiInstance == FALSE), but here we pass TRUE
// For more explanation see: "How to use single or multiple instances of an OLE object in MFC by using Visual C++" KB article:
// http://support.microsoft.com/kb/141154

// The skeleton for this macro snippet was copied from MFC 9.0's afxdisp.h instead of the KB article's example
// Then I set the bMultiInstance to TRUE
#define MY_IMPLEMENT_OLECREATE(class_name, external_name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
	COleObjectFactory class_name::factory(class_name::guid, \
		RUNTIME_CLASS(class_name), TRUE, _T(external_name)); \
	AFX_COMDAT const GUID class_name::guid = \
		{ l, w1, w2, { b1, b2, b3, b4, b5, b6, b7, b8 } }; \

// {C7DCCC2E-1642-4a40-8060-51A7B9FAE488}
MY_IMPLEMENT_OLECREATE(CGMEOLEApp, "GME.Application", 0xC7DCCC2E, 0x1642, 0x4a40, 0x80, 0x60, 0x51, 0xA7, 0xB9, 0xFA, 0xE4, 0x88)

/////////////////////////////////////////////////////////////////////////////
// CGMEOLEApp message handlers



BOOL CGMEOLEApp::GetVisible() 
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEApp::GetVisible()\r\n");
	
	// PRECONDITIONS: None

	return AfxGetMainWnd()->IsWindowVisible();
}

void CGMEOLEApp::SetVisible(BOOL bNewValue) 
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEApp::SetVisible()\r\n");
	
	// PRECONDITIONS: None

	AfxGetMainWnd()->ShowWindow(bNewValue ? SW_SHOW : SW_HIDE);
}

void CGMEOLEApp::CreateProject(LPCTSTR metaname, LPCTSTR connstr) 
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEApp::CreateProject() "+CString(metaname)+" "+CString(connstr)+"\r\n");
	
	PRECONDITION_NO_PROJECT

	theApp.CreateProject(metaname, connstr);
}

void CGMEOLEApp::OpenProject(LPCTSTR connstr) 
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEApp::OpenProject() "+CString(connstr)+"\r\n");
	
	PRECONDITION_NO_PROJECT

	theApp.OpenProject(connstr);
}

void CGMEOLEApp::CreateProjectDialog() 
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEApp::CreateProjectDialog()\r\n");
	
	PRECONDITION_NO_PROJECT

	theApp.OnFileNew();
}

void CGMEOLEApp::OpenProjectDialog() 
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEApp::OpenProjectDialog()\r\n");
	
	PRECONDITION_NO_PROJECT

	theApp.OnFileOpen();
}

void CGMEOLEApp::CloseProject(BOOL saveOnClose) 
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEApp::CloseProject()\r\n");
	
	PRECONDITION_ACTIVE_PROJECT

	if (saveOnClose == FALSE) {
		theApp.OnFileAbortProject();
	}
	else {
		theApp.OnFileCloseproject();
	}
}

void CGMEOLEApp::SaveProject() 
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEApp::SaveProject()\r\n");

	PRECONDITION_ACTIVE_REAL_MGA_PROJECT

	theApp.OnFileSave();
}

void CGMEOLEApp::SaveProjectAs(LPCTSTR connstr) 
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEApp::SaveProjectAs() "+CString(connstr)+"\r\n");

	PRECONDITION_ACTIVE_REAL_MGA_PROJECT

	theApp.SaveProject(connstr);
}

void CGMEOLEApp::SaveProjectAsDialog() 
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEApp::SaveProjectAsDialog()\r\n");
	
	PRECONDITION_ACTIVE_REAL_MGA_PROJECT

	theApp.OnFileSaveAs();
}

void CGMEOLEApp::ExportProject(LPCTSTR connstr) 
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEApp::ExportProject() "+CString(connstr)+"\r\n");

	PRECONDITION_ACTIVE_PROJECT

	try
	{
		CComPtr<IMgaDumper> dumper;
		COMTHROW( dumper.CoCreateInstance(L"Mga.MgaDumper") );
		ASSERT( dumper != NULL );

		COMTHROW( dumper->DumpProject(theApp.mgaProject,PutInBstr(CString(connstr))) );

	}
	catch (hresult_exception &){

	}
}

void CGMEOLEApp::ImportProject(LPCTSTR connstr) 
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEApp::ImportProject() "+CString(connstr)+"\r\n");

	PRECONDITION_ACTIVE_PROJECT

	try
	{
		CComPtr<IMgaParser> parser;
		COMTHROW( parser.CoCreateInstance(L"Mga.MgaParser") );
		ASSERT( parser != NULL );

		if(theApp.mgaConstMgr) COMTHROW(theApp.mgaConstMgr->Enable(VARIANT_FALSE));

		IMgaParser2Ptr parser2 = (IMgaParser*)parser;
		if (parser2 && theApp.m_pMainWnd)
			COMTHROW(parser2->ParseProject2(theApp.mgaProject,PutInBstr(CString(connstr)), (ULONGLONG)(theApp.m_pMainWnd->GetSafeHwnd())));
		else
			COMTHROW(parser->ParseProject(theApp.mgaProject,PutInBstr(CString(connstr))) );

	}
	catch (hresult_exception &){

	}
	if (theApp.mgaConstMgr) COMTHROW(theApp.mgaConstMgr->Enable(VARIANT_TRUE));
}

void CGMEOLEApp::ConstraintsDialog() 
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEApp::ConstraintsDialog()\r\n");

	PRECONDITION_ACTIVE_CONSTMGR	

	theApp.OnFileDisplayConstraints();
}

void CGMEOLEApp::CheckAllConstraints() 
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEApp::CheckAllConstraints()\r\n");

	PRECONDITION_ACTIVE_CONSTMGR

	theApp.OnFileCheckall();
}


void CGMEOLEApp::RegisterParagimsDialog() 
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEApp::RegisterParagimsDialog()\r\n");

	// PRECONDITIONS: None

	theApp.OnFileRegparadigms();
}

void CGMEOLEApp::RegisterComponentsDialog() 
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEApp::RegisterComponentsDialog()\r\n");

	PRECONDITION_ACTIVE_PROJECT

	theApp.OnFileRegcomponents();
}


void CGMEOLEApp::RunComponent(LPCTSTR progID) 
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEApp::RunComponent() "+CString(progID)+"\r\n");

	PRECONDITION_ACTIVE_PROJECT

	CString compname(progID);
	theApp.RunComponent(compname);
}


void CGMEOLEApp::RunComponentDialog() 
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEApp::RunComponentDialog()\r\n");

	PRECONDITION_ACTIVE_PROJECT

	CString compname;
	theApp.RunComponent(compname);
}

void CGMEOLEApp::SettingsDialog() 
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEApp::SettingsDialog()\r\n");

	// PRECONDITIONS: None

	theApp.OnFileSettings();
}


void CGMEOLEApp::Undo() 
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEApp::Undo()\r\n");

	PRECONDITION_ACTIVE_PROJECT

	theApp.OnEditUndo();
}


void CGMEOLEApp::Redo() 
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEApp::Redo()\r\n");

	PRECONDITION_ACTIVE_PROJECT

	theApp.OnEditRedo();
}


void CGMEOLEApp::ClearUndoQueue() 
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEApp::ClearUndoQueue()\r\n");

	PRECONDITION_ACTIVE_PROJECT

	theApp.OnEditClearUndo();
}


void CGMEOLEApp::ProjectPropertiesDialog() 
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEApp::ProjectPropertiesDialog()\r\n");

	PRECONDITION_ACTIVE_PROJECT

	theApp.OnEditProjectproperties();
}

void CGMEOLEApp::ShowHelpContents() 
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEApp::ShowHelpContents()\r\n");

	// PRECONDITIONS: None

	theApp.OnHelpContents();
}

void CGMEOLEApp::ShowAbout() 
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEApp::ShowAbout()\r\n");

	// PRECONDITIONS: None

	theApp.OnAppAbout();
}

BSTR CGMEOLEApp::GetVersion() 
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEApp::GetVersion()\r\n");

	// PRECONDITIONS: None

	CString strResult(GME_VERSION_STR);
	return strResult.AllocSysString();
}

void CGMEOLEApp::SetVersion(LPCTSTR) 
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEApp::SetVersion()\r\n");

	SetNotSupported();
}

short CGMEOLEApp::GetVersionMajor() 
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEApp::GetVersionMajor()\r\n");

	// PRECONDITIONS: None

	return GME_VERSION_MAJOR;
}

void CGMEOLEApp::SetVersionMajor(short) 
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEApp::SetVersionMajor()\r\n");

	SetNotSupported();
}

short CGMEOLEApp::GetVersionMinor() 
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEApp::GetVersionMinor()\r\n");

	// PRECONDITIONS: None

	return GME_VERSION_MINOR;
}

void CGMEOLEApp::SetVersionMinor(short) 
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEApp::SetVersionMinor()\r\n");

	SetNotSupported();
}

short CGMEOLEApp::GetVersionPatchLevel() 
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEApp::GetVersionPatchLevel()\r\n");

	// PRECONDITIONS: None

	return GME_VERSION_PLEVEL;
}

void CGMEOLEApp::SetVersionPatchLevel(short) 
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEApp::SetVersionPatchLevel()\r\n");

	SetNotSupported();
}


void CGMEOLEApp::ShowFCO(LPDISPATCH mgaFCO, BOOL inParent) 
{
	try {
		CGMEEventLogger::LogGMEEvent("CGMEOLEApp::ShowFCO()\r\n");

		PRECONDITION_ACTIVE_PROJECT

		// JIRA GME-219, CGMEView::OnInitialUpdate would throw this anyway
		long status;
		COMTHROW(theApp.mgaProject->get_ProjectStatus(&status));
		bool inTrans = (status & 0x08L) != 0;
		if (inTrans)
			COMTHROW(E_MGA_ALREADY_IN_TRANSACTION);

		if (CGMEDoc::theInstance) {
			CGMEDoc::theInstance->ShowObject(mgaFCO, inParent);
		}
	} catch (const hresult_exception& e) {
		AfxThrowOleException(e.hr);
	}
}

LPDISPATCH CGMEOLEApp::GetModels() 
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEApp::GetModels()\r\n");

	PRECONDITION_ACTIVE_PROJECT

	CGMEOLEColl* coll = new CGMEOLEColl();

	if (CGMEDoc::theInstance) {
		POSITION pos = CGMEDoc::theInstance->GetFirstViewPosition();
		while (pos) {
			CGMEView* view = (CGMEView*)(CGMEDoc::theInstance->GetNextView(pos));
			ASSERT(view);
			CGMEOLEModel* model = new CGMEOLEModel();
			model->SetView(view);
			coll->Add(model->GetIDispatch(FALSE));
			model->GetIDispatch(FALSE)->Release();	// We do not hold reference, hopefully the collection will hold one
		}
	}

	return coll->GetIDispatch(FALSE);
}

void CGMEOLEApp::SetModels(LPDISPATCH) 
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEApp::SetModels()\r\n");

	SetNotSupported();
}

void CGMEOLEApp::ChangeEditmode(long mode)
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEApp::ChangeEditmode()\r\n");
	if (CGMEDoc::theInstance) 
		CGMEDoc::theInstance->SetMode(mode);
}

void CGMEOLEApp::GridShow(BOOL show)
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEApp::GridShow()\r\n");
	CMDIChildWnd *pChild  = CMainFrame::theInstance->MDIGetActive();
	if (pChild) 
	{
#if !defined (ACTIVEXGMEVIEW)
		CGMEView *view = (CGMEView*)pChild->GetActiveView();
		if (view) 
			view->ShowGrid(show==TRUE);
#endif
	}
}

void CGMEOLEApp::AttributepanelPage(long page)
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEApp::AttributepanelPage()\r\n");
	CGMEObjectInspector::theInstance->ShowPanel(page);
}


LPDISPATCH CGMEOLEApp::GetMgaProject() 
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEApp::GetMgaProject()\r\n");

	// PRECONDITIONS: None

	LPDISPATCH ret = nullptr;
	if (theApp.mgaProject)
	{
		theApp.mgaProject->QueryInterface(IID_IDispatch, (void**)&ret);
	}
	return ret;
}

void CGMEOLEApp::SetMgaProject(LPDISPATCH) 
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEApp::SetMgaProject()\r\n");

	SetNotSupported();
}

LPDISPATCH CGMEOLEApp::GetOleIt() 
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEApp::GetOleIt()\r\n");

	// PRECONDITIONS: None

	CGMEOLEIt* opened = new CGMEOLEIt();
	LPDISPATCH rr = opened->GetIDispatch( FALSE);

	return rr;
}

void CGMEOLEApp::SetOleIt(LPDISPATCH) 
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEApp::SetOleIt()\r\n");

	SetNotSupported();
}

LPDISPATCH CGMEOLEApp::GetPanels() 
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEApp::GetPanels()\r\n");

	// PRECONDITIONS: None

	CGMEOLEColl* coll = new CGMEOLEColl();

	CGMEOLEPanel* panel = new CGMEOLEPanel();
	panel->SetParams( &CMainFrame::theInstance->m_browser, _T("Browser"), CMainFrame::theInstance->m_browser.GetInterface());
	coll->Add(panel->GetIDispatch(FALSE));
	panel->GetIDispatch(FALSE)->Release();	// We do not hold reference, hopefully the collection will hold one

	panel = new CGMEOLEPanel();
	panel->SetParams( &CMainFrame::theInstance->m_objectInspector, _T("Attribute Panel"), CMainFrame::theInstance->m_objectInspector.GetInterface());
	coll->Add(panel->GetIDispatch(FALSE));
	panel->GetIDispatch(FALSE)->Release();	// We do not hold reference, hopefully the collection will hold one

	panel = new CGMEOLEPanel();
	panel->SetParams( &CMainFrame::theInstance->m_console, _T("Console"), CMainFrame::theInstance->m_console.GetInterface());
	coll->Add(panel->GetIDispatch(FALSE));
	panel->GetIDispatch(FALSE)->Release();	// We do not hold reference, hopefully the collection will hold one

	panel = new CGMEOLEPanel();
	panel->SetParams( &CMainFrame::theInstance->m_partBrowser, _T("Part Browser"), CMainFrame::theInstance->m_partBrowser.GetInterface());
	coll->Add(panel->GetIDispatch(FALSE));
	panel->GetIDispatch(FALSE)->Release();	// We do not hold reference, hopefully the collection will hold one

	// terge 
	panel = new CGMEOLEPanel();
	panel->SetParams( &CMainFrame::theInstance->m_panningWindow, _T("Panning Window"), CMainFrame::theInstance->m_panningWindow.GetInterface());
	coll->Add(panel->GetIDispatch(FALSE));
	panel->GetIDispatch(FALSE)->Release();	// We do not hold reference, hopefully the collection will hold one

	panel = new CGMEOLEPanel();
	panel->SetParams( &CMainFrame::theInstance->m_wndStatusBar, _T("Status Bar"));
	coll->Add(panel->GetIDispatch(FALSE));
	panel->GetIDispatch(FALSE)->Release();	// We do not hold reference, hopefully the collection will hold one

	panel = new CGMEOLEPanel();
	panel->SetParams( &CMainFrame::theInstance->m_wndToolBarMain, _T("Main Toolbar"));
	coll->Add(panel->GetIDispatch(FALSE));
	panel->GetIDispatch(FALSE)->Release();	// We do not hold reference, hopefully the collection will hold one

	panel = new CGMEOLEPanel();
	panel->SetParams( &CMainFrame::theInstance->m_wndToolBarModeling, _T("Modeling Toolbar"));
	coll->Add(panel->GetIDispatch(FALSE));
	panel->GetIDispatch(FALSE)->Release();	// We do not hold reference, hopefully the collection will hold one

	panel = new CGMEOLEPanel();
	panel->SetParams( &CMainFrame::theInstance->m_wndToolBarWins, _T("Window Toolbar"));
	coll->Add(panel->GetIDispatch(FALSE));
	panel->GetIDispatch(FALSE)->Release();	// We do not hold reference, hopefully the collection will hold one

	panel = new CGMEOLEPanel();
	panel->SetParams( &CMainFrame::theInstance->m_wndComponentBar, _T("Component Toolbar"));
	coll->Add(panel->GetIDispatch(FALSE));
	panel->GetIDispatch(FALSE)->Release();	// We do not hold reference, hopefully the collection will hold one

	return coll->GetIDispatch(FALSE);
}

void CGMEOLEApp::SetPanels(LPDISPATCH) 
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEApp::SetPanels()\r\n");

	SetNotSupported();
}

BSTR CGMEOLEApp::GetConsoleContents() 
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEApp::GetConsoleContents()\r\n");
	
	// PRECONDITIONS: None

	CString str = CMainFrame::theInstance->m_console.GetContents();
	
	return str.AllocSysString();
}

void CGMEOLEApp::SetConsoleContents(LPCTSTR contents) 
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEApp::SetConsoleContents("+CString(contents)+")\r\n");
	
	// PRECONDITIONS: None

	CMainFrame::theInstance->m_console.SetContents(contents);
}

void CGMEOLEApp::ConsoleMessage(LPCTSTR msg, long type)
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEApp::ConsoleMessage("+CString(msg)+")\r\n");
	
	// PRECONDITIONS: None

	CMainFrame::theInstance->m_console.Message(msg, (short)type);
}

void CGMEOLEApp::ConsoleClear()
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEApp::ConsoleClear()\r\n");
	
	// PRECONDITIONS: None

	CMainFrame::theInstance->m_console.Clear();
}

void CGMEOLEApp::ConsoleNavigateTo(LPCTSTR url)
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEApp::ConsoleNavigateTo("+CString(url)+")\r\n");
	
	// PRECONDITIONS: None

	CMainFrame::theInstance->m_console.NavigateTo(url);
}

void CGMEOLEApp::DisableComp(LPCTSTR pCompName, BOOL pDisable)
{
	// this method allows user added components to be disabled
	// thus preventing their launch at inappropriate time
	// it is not equivalent with "Toggle", since that removes
	// a component from the possible list of interpreters/plugins
	// and that state is saved and used across several GME sessions
	// in contrast this disable mechanism is not saves between
	// separate GME sessions
	CGMEEventLogger::LogGMEEvent("CGMEOLEApp::DisableComp()\r\n");
	theApp.DisableComp( pCompName, pDisable == TRUE);
}

void CGMEOLEApp::DisableCompForKinds( LPCTSTR pCompName, LPCTSTR pKindNameSeq)
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEApp::DisableCompForKinds()\r\n");
	theApp.DisableCompForKinds( pCompName, pKindNameSeq);
}

void CGMEOLEApp::SetCompFiltering( BOOL pOn)
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEApp::SetCompFiltering()\r\n");
	return theApp.SetCompFiltering( pOn == TRUE);
}

BOOL CGMEOLEApp::GetCompFiltering()
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEApp::GetCompFiltering()\r\n");
	return theApp.GetCompFiltering();
}

void CGMEOLEApp::SetWorkingDirectory( LPCTSTR pPath)
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEApp::SetWorkingDirectory()\r\n");
	bool rv = theApp.SetWorkingDirectory( pPath);
	if( !rv) throw CString("Error happened");
}

void CGMEOLEApp::Exit()
{
	theApp.CloseProject(false);
	// n.b. Post here, since this call could be out-of-proc, and it won't succeed 
	// if TerminateProcess is called
	CMainFrame::theInstance->PostMessage(WM_CLOSE, 0, 0);
}

long CGMEOLEApp::RefreshLib( LPCTSTR pLibName, BOOL pUngroup)
{
	return 0;
}

DELEGATE_DUAL_INTERFACE(CGMEOLEApp, Dual)

// Implement ISupportErrorInfo to indicate we support the
// OLE Automation error handler.
IMPLEMENT_DUAL_ERRORINFO(CGMEOLEApp, __uuidof(IGMEOLEApp))

STDMETHODIMP CGMEOLEApp::XDual::put_Visible( VARIANT_BOOL isVisible)
{
	METHOD_PROLOGUE(CGMEOLEApp, Dual)

	TRY_DUAL(__uuidof(IGMEOLEApp))
	{
		pThis->SetVisible(isVisible);
		return NOERROR;
	}
	CATCH_ALL_DUAL
}


STDMETHODIMP CGMEOLEApp::XDual::get_Visible( VARIANT_BOOL* isVisible)
{
	METHOD_PROLOGUE(CGMEOLEApp, Dual)

	TRY_DUAL(__uuidof(IGMEOLEApp))
	{
		*isVisible = (pThis->GetVisible() == FALSE) ? VARIANT_FALSE : VARIANT_TRUE;
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEApp::XDual::get_Version( BSTR* versionStr)
{
	METHOD_PROLOGUE(CGMEOLEApp, Dual)

	TRY_DUAL(__uuidof(IGMEOLEApp))
	{
		// CString str = pThis->GetVersion();
		// str.SetSysString(versionStr);
		*versionStr = pThis->GetVersion();
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEApp::XDual::get_VersionMajor( short* n)
{
	METHOD_PROLOGUE(CGMEOLEApp, Dual)

	TRY_DUAL(__uuidof(IGMEOLEApp))
	{
		*n = pThis->GetVersionMajor();
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEApp::XDual::get_VersionMinor( short* n)
{
	METHOD_PROLOGUE(CGMEOLEApp, Dual)

	TRY_DUAL(__uuidof(IGMEOLEApp))
	{
		*n = pThis->GetVersionMinor();
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEApp::XDual::get_VersionPatchLevel( short* n)
{
	METHOD_PROLOGUE(CGMEOLEApp, Dual)

	TRY_DUAL(__uuidof(IGMEOLEApp))
	{
		*n = pThis->GetVersionPatchLevel();
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEApp::XDual::get_Models( IGMEOLEColl** coll)
{
	METHOD_PROLOGUE(CGMEOLEApp, Dual)

	TRY_DUAL(__uuidof(IGMEOLEApp))
	{
		LPDISPATCH lpDisp = pThis->GetModels();
		lpDisp->QueryInterface(__uuidof(IGMEOLEColl), (LPVOID*)coll);
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEApp::XDual::get_MgaProject( IMgaProject** project)
{
	METHOD_PROLOGUE(CGMEOLEApp, Dual)

	TRY_DUAL(__uuidof(IGMEOLEApp))
	{
		LPDISPATCH lpDisp = pThis->GetMgaProject();
		lpDisp->QueryInterface(__uuidof(IMgaProject), (LPVOID*)project);
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEApp::XDual::get_OleIt( IGMEOLEIt** model)
{
	METHOD_PROLOGUE(CGMEOLEApp, Dual)

	TRY_DUAL(__uuidof(IGMEOLEApp))
	{
		LPDISPATCH lpDisp = pThis->GetOleIt();
		lpDisp->QueryInterface(__uuidof(IGMEOLEIt), (LPVOID*)model);
		lpDisp->Release();
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEApp::XDual::get_Panels( IGMEOLEColl** coll)
{
	METHOD_PROLOGUE(CGMEOLEApp, Dual)

	TRY_DUAL(__uuidof(IGMEOLEApp))
	{
		LPDISPATCH lpDisp = pThis->GetPanels();
		lpDisp->QueryInterface(__uuidof(IGMEOLEColl), (LPVOID*)coll);
		lpDisp->Release();
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEApp::XDual::get_ConsoleContents( BSTR* contents)
{
	METHOD_PROLOGUE(CGMEOLEApp, Dual)

	TRY_DUAL(__uuidof(IGMEOLEApp))
	{
		*contents = pThis->GetConsoleContents();
		return NOERROR;
	}
	CATCH_ALL_DUAL
}


STDMETHODIMP CGMEOLEApp::XDual::put_ConsoleContents( BSTR contents)
{
	METHOD_PROLOGUE(CGMEOLEApp, Dual)

	TRY_DUAL(__uuidof(IGMEOLEApp))
	{
		pThis->SetConsoleContents(CString(contents));
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEApp::XDual::CreateProject( BSTR metaname, BSTR connstr)
{
	METHOD_PROLOGUE(CGMEOLEApp, Dual)

	TRY_DUAL(__uuidof(IGMEOLEApp))
	{
		pThis->CreateProject(CString(metaname), CString(connstr));
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEApp::XDual::OpenProject( BSTR connstr)
{
	METHOD_PROLOGUE(CGMEOLEApp, Dual)

	TRY_DUAL(__uuidof(IGMEOLEApp))
	{
		pThis->OpenProject(CString(connstr));
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEApp::XDual::CreateProjectDialog()
{
	METHOD_PROLOGUE(CGMEOLEApp, Dual)

	TRY_DUAL(__uuidof(IGMEOLEApp))
	{
		pThis->CreateProjectDialog();
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEApp::XDual::OpenProjectDialog()
{
	METHOD_PROLOGUE(CGMEOLEApp, Dual)

	TRY_DUAL(__uuidof(IGMEOLEApp))
	{
		pThis->OpenProjectDialog();
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEApp::XDual::CloseProject( VARIANT_BOOL saveOnClose)
{
	METHOD_PROLOGUE(CGMEOLEApp, Dual)

	TRY_DUAL(__uuidof(IGMEOLEApp))
	{
		pThis->CloseProject(saveOnClose);
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEApp::XDual::SaveProject()
{
	METHOD_PROLOGUE(CGMEOLEApp, Dual)

	TRY_DUAL(__uuidof(IGMEOLEApp))
	{
		pThis->SaveProject();
		return NOERROR;
	}
	CATCH_ALL_DUAL
}


STDMETHODIMP CGMEOLEApp::XDual::SaveProjectAs( BSTR connstr)
{
	METHOD_PROLOGUE(CGMEOLEApp, Dual)

	TRY_DUAL(__uuidof(IGMEOLEApp))
	{
		pThis->SaveProjectAs(CString(connstr));
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEApp::XDual::SaveProjectAsDialog()
{
	METHOD_PROLOGUE(CGMEOLEApp, Dual)

	TRY_DUAL(__uuidof(IGMEOLEApp))
	{
		pThis->SaveProjectAsDialog();
		return NOERROR;
	}
	CATCH_ALL_DUAL
}


STDMETHODIMP CGMEOLEApp::XDual::ExportProject( BSTR connstr)
{
	METHOD_PROLOGUE(CGMEOLEApp, Dual)

	TRY_DUAL(__uuidof(IGMEOLEApp))
	{
		pThis->ExportProject(CString(connstr));
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEApp::XDual::ImportProject( BSTR connstr)
{
	METHOD_PROLOGUE(CGMEOLEApp, Dual)

	TRY_DUAL(__uuidof(IGMEOLEApp))
	{
		pThis->ImportProject(CString(connstr));
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEApp::XDual::ConstraintsDialog()
{
	METHOD_PROLOGUE(CGMEOLEApp, Dual)

	TRY_DUAL(__uuidof(IGMEOLEApp))
	{
		pThis->ConstraintsDialog();
		return NOERROR;
	}
	CATCH_ALL_DUAL
}


STDMETHODIMP CGMEOLEApp::XDual::CheckAllConstraints()
{
	METHOD_PROLOGUE(CGMEOLEApp, Dual)

	TRY_DUAL(__uuidof(IGMEOLEApp))
	{
		pThis->CheckAllConstraints();
		return NOERROR;
	}
	CATCH_ALL_DUAL
}


STDMETHODIMP CGMEOLEApp::XDual::RegisterParagimsDialog()
{
	METHOD_PROLOGUE(CGMEOLEApp, Dual)

	TRY_DUAL(__uuidof(IGMEOLEApp))
	{
		pThis->RegisterParagimsDialog();
		return NOERROR;
	}
	CATCH_ALL_DUAL
}


STDMETHODIMP CGMEOLEApp::XDual::RegisterComponentsDialog()
{
	METHOD_PROLOGUE(CGMEOLEApp, Dual)

	TRY_DUAL(__uuidof(IGMEOLEApp))
	{
		pThis->RegisterComponentsDialog();
		return NOERROR;
	}
	CATCH_ALL_DUAL
}


STDMETHODIMP CGMEOLEApp::XDual::RunComponent( BSTR progID)
{
	METHOD_PROLOGUE(CGMEOLEApp, Dual)

	TRY_DUAL(__uuidof(IGMEOLEApp))
	{
		pThis->RunComponent(CString(progID));
		return NOERROR;
	}
	CATCH_ALL_DUAL
}


STDMETHODIMP CGMEOLEApp::XDual::RunComponentDialog()
{
	METHOD_PROLOGUE(CGMEOLEApp, Dual)

	TRY_DUAL(__uuidof(IGMEOLEApp))
	{
		pThis->RunComponentDialog();
		return NOERROR;
	}
	CATCH_ALL_DUAL
}


STDMETHODIMP CGMEOLEApp::XDual::SettingsDialog()
{
	METHOD_PROLOGUE(CGMEOLEApp, Dual)

	TRY_DUAL(__uuidof(IGMEOLEApp))
	{
		pThis->SettingsDialog();
		return NOERROR;
	}
	CATCH_ALL_DUAL
}


STDMETHODIMP CGMEOLEApp::XDual::Undo()
{
	METHOD_PROLOGUE(CGMEOLEApp, Dual)

	TRY_DUAL(__uuidof(IGMEOLEApp))
	{
		pThis->Undo();
		return NOERROR;
	}
	CATCH_ALL_DUAL
}


STDMETHODIMP CGMEOLEApp::XDual::Redo()
{
	METHOD_PROLOGUE(CGMEOLEApp, Dual)

	TRY_DUAL(__uuidof(IGMEOLEApp))
	{
		pThis->Redo();
		return NOERROR;
	}
	CATCH_ALL_DUAL
}


STDMETHODIMP CGMEOLEApp::XDual::ClearUndoQueue()
{
	METHOD_PROLOGUE(CGMEOLEApp, Dual)

	TRY_DUAL(__uuidof(IGMEOLEApp))
	{
		pThis->ClearUndoQueue();
		return NOERROR;
	}
	CATCH_ALL_DUAL
}


STDMETHODIMP CGMEOLEApp::XDual::ProjectPropertiesDialog()
{
	METHOD_PROLOGUE(CGMEOLEApp, Dual)

	TRY_DUAL(__uuidof(IGMEOLEApp))
	{
		pThis->ProjectPropertiesDialog();
		return NOERROR;
	}
	CATCH_ALL_DUAL
}


STDMETHODIMP CGMEOLEApp::XDual::ShowHelpContents()
{
	METHOD_PROLOGUE(CGMEOLEApp, Dual)

	TRY_DUAL(__uuidof(IGMEOLEApp))
	{
		pThis->ShowHelpContents();
		return NOERROR;
	}
	CATCH_ALL_DUAL
}


STDMETHODIMP CGMEOLEApp::XDual::ShowAbout()
{
	METHOD_PROLOGUE(CGMEOLEApp, Dual)

	TRY_DUAL(__uuidof(IGMEOLEApp))
	{
		pThis->ShowAbout();
		return NOERROR;
	}
	CATCH_ALL_DUAL
}


STDMETHODIMP CGMEOLEApp::XDual::ShowFCO( IMgaFCO* mgaFCO, VARIANT_BOOL inParent)
{
	METHOD_PROLOGUE(CGMEOLEApp, Dual)

	TRY_DUAL(__uuidof(IGMEOLEApp))
	{
		LPDISPATCH lpDisp = NULL;
		mgaFCO->QueryInterface(IID_IDispatch, (LPVOID*)&lpDisp);
		pThis->ShowFCO(lpDisp, inParent);
		lpDisp->Release();
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEApp::XDual::ConsoleMessage(BSTR msg, msgtype_enum type)
{
	METHOD_PROLOGUE(CGMEOLEApp, Dual)

	TRY_DUAL(__uuidof(IGMEOLEApp))
	{
		pThis->ConsoleMessage(CString(msg), type);
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEApp::XDual::ConsoleClear()
{
	METHOD_PROLOGUE(CGMEOLEApp, Dual)

	TRY_DUAL(__uuidof(IGMEOLEApp))
	{
		pThis->ConsoleClear();
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEApp::XDual::ConsoleNavigateTo(BSTR url)
{
	METHOD_PROLOGUE(CGMEOLEApp, Dual)

	TRY_DUAL(__uuidof(IGMEOLEApp))
	{
		pThis->ConsoleNavigateTo(CString(url));
		return NOERROR;
	}
	CATCH_ALL_DUAL
}


STDMETHODIMP CGMEOLEApp::XDual::ChangeEditmode(editmode_enum mode)
{
	METHOD_PROLOGUE(CGMEOLEApp, Dual)

	TRY_DUAL(__uuidof(IGMEOLEApp))
	{
		pThis->ChangeEditmode((long)mode);
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEApp::XDual::GridShow(VARIANT_BOOL show)
{
	METHOD_PROLOGUE(CGMEOLEApp, Dual)

	TRY_DUAL(__uuidof(IGMEOLEApp))
	{
		pThis->GridShow(show);
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEApp::XDual::AttributepanelPage(attribpanel_page page)
{
	METHOD_PROLOGUE(CGMEOLEApp, Dual)

	TRY_DUAL(__uuidof(IGMEOLEApp))
	{
		pThis->AttributepanelPage((long)page);
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEApp::XDual::DisableComp(BSTR pCompName, VARIANT_BOOL pHide)
{
	METHOD_PROLOGUE(CGMEOLEApp, Dual)

	TRY_DUAL(__uuidof(IGMEOLEApp))
	{
		pThis->DisableComp( CString( pCompName), pHide);
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEApp::XDual::DisableCompForKinds(BSTR pCompName, BSTR pKindSequence)
{
	METHOD_PROLOGUE(CGMEOLEApp, Dual)

	TRY_DUAL(__uuidof(IGMEOLEApp))
	{
		pThis->DisableCompForKinds( CString( pCompName), CString( pKindSequence));
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEApp::XDual::SetCompFiltering( VARIANT_BOOL pOn)
{
	METHOD_PROLOGUE(CGMEOLEApp, Dual)

	TRY_DUAL(__uuidof(IGMEOLEApp))
	{
		pThis->SetCompFiltering( pOn);
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEApp::XDual::GetCompFiltering( VARIANT_BOOL* pOn)
{
	METHOD_PROLOGUE(CGMEOLEApp, Dual)

	TRY_DUAL(__uuidof(IGMEOLEApp))
	{
		*pOn = (pThis->GetCompFiltering())?VARIANT_TRUE:VARIANT_FALSE;
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEApp::XDual::SetWorkingDirectory(BSTR pPath)
{
	METHOD_PROLOGUE(CGMEOLEApp, Dual)

	TRY_DUAL(__uuidof(IGMEOLEApp))
	{
		pThis->SetWorkingDirectory( CString( pPath));
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEApp::XDual::Exit()
{
	METHOD_PROLOGUE(CGMEOLEApp, Dual)

	TRY_DUAL(__uuidof(IGMEOLEApp))
	{
		pThis->Exit();
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEApp::XDual::RefreshLib( BSTR pLibName, VARIANT_BOOL pUngroup, long* pNumOfErrors)
{
	METHOD_PROLOGUE(CGMEOLEApp, Dual)

	TRY_DUAL(__uuidof(IGMEOLEApp))
	{
		long res = pThis->RefreshLib( CString( pLibName), pUngroup);
		*pNumOfErrors = res;
		return NOERROR;
	}
	CATCH_ALL_DUAL
}
