// CurrentProject.cpp: implementation of the CCurrentProject class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "gmeactivebrowser.h"
#include "GMEActiveBrowserCtl.h"
#include "CurrentProject.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCurrentProject::CCurrentProject():m_ProjectState(CURRENT_PROJECT_CLOSED)
{
	
}

CCurrentProject::~CCurrentProject()
{

}

short CCurrentProject::CreateCurrentProject(LPUNKNOWN pMgaProject)
{
	ASSERT(m_ProjectState==CURRENT_PROJECT_CLOSED);

	
	CGMEActiveBrowserApp* pApp;
	pApp=(CGMEActiveBrowserApp*)AfxGetApp();

	CGMEActiveBrowserCtrl* pCtrl=pApp->GetCtrl();
		
	m_MgaContext.CreateContext(pCtrl->m_xEventSink,pMgaProject);
	m_ProjectState=CURRENT_PROJECT_OPEN;
	

	// Notifying the views
	pCtrl->m_pPropFrame->OnOpenProject();


	return 0;
}

void CCurrentProject::CloseCurrentProject()
{

	if(m_ProjectState==CURRENT_PROJECT_CLOSED) return;
	
	// Cleaning up UI		
	CGMEActiveBrowserApp* pApp;
	pApp=(CGMEActiveBrowserApp*)AfxGetApp();
	CGMEActiveBrowserCtrl* pCtrl=pApp->GetCtrl();
	pCtrl->m_pPropFrame->OnCloseProject();

	m_MgaContext.CloseContext();


	m_ProjectState=CURRENT_PROJECT_CLOSED;
}


eCurrentProjectState CCurrentProject::GetCurrentProjectState()
{
	return m_ProjectState;
}
