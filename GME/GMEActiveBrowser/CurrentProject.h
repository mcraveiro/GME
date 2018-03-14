// CurrentProject.h: interface for the CCurrentProject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CURRENTPROJECT_H__AFB185BF_306A_4BF6_9D54_4B9F886AD219__INCLUDED_)
#define AFX_CURRENTPROJECT_H__AFB185BF_306A_4BF6_9D54_4B9F886AD219__INCLUDED_


#include "MgaContext.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000




enum eCurrentProjectState {CURRENT_PROJECT_OPEN,CURRENT_PROJECT_CLOSED};
class CCurrentProject  
{
public:
	eCurrentProjectState GetCurrentProjectState();
	void CloseCurrentProject();
	short CreateCurrentProject(LPUNKNOWN pMgaProject);
	CMgaContext m_MgaContext;
	CCurrentProject();
	virtual ~CCurrentProject();

private:
	
	eCurrentProjectState m_ProjectState;
};

#endif // !defined(AFX_CURRENTPROJECT_H__AFB185BF_306A_4BF6_9D54_4B9F886AD219__INCLUDED_)
