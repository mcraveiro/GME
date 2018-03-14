// AggregateOptions.cpp: implementation of the CAggregateOptions class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "gmeactivebrowser.h"
#include "AggregateOptions.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAggregateOptions::CAggregateOptions()
{
	// Defaults for the first run or in case of registry problems
	m_bDispModel=TRUE;
	m_bDispAtom=TRUE;
	m_bDispReference=TRUE;
	m_bDispSet=TRUE;
	m_bDispConnections=FALSE;

	m_soSortOptions=SORT_BYNAME;
	m_bStoreTreeInRegistry=FALSE;

	// This is not set by the user interface only stored in the registry
	m_bIsDynamicLoading=TRUE;

	LoadFromRegistry();
}

CAggregateOptions::~CAggregateOptions()
{
	SaveToRegistry();
}

void CAggregateOptions::SaveToRegistry()
{
		CGMEActiveBrowserApp* pApp=(CGMEActiveBrowserApp*)AfxGetApp();
		pApp->WriteProfileInt(_T("Settings"),_T("Display Models"),m_bDispModel);
		pApp->WriteProfileInt(_T("Settings"),_T("Display Atoms"),m_bDispAtom);
		pApp->WriteProfileInt(_T("Settings"),_T("Display References"),m_bDispReference);
		pApp->WriteProfileInt(_T("Settings"),_T("Display Sets"),m_bDispSet);
		pApp->WriteProfileInt(_T("Settings"),_T("Display Connections"),m_bDispConnections);

		pApp->WriteProfileInt(_T("Settings"),_T("Sort Option"),m_soSortOptions);
		pApp->WriteProfileInt(_T("Settings"),_T("Store Tree In Registry"),m_bStoreTreeInRegistry);

		pApp->WriteProfileInt(_T("Settings"),_T("Dynamic Loading"),m_bIsDynamicLoading);
}

void CAggregateOptions::LoadFromRegistry()
{
		#define NO_VALUE 1000

		CGMEActiveBrowserApp* pApp=(CGMEActiveBrowserApp*)AfxGetApp();
		
		int nValue=pApp->GetProfileInt(_T("Settings"),_T("Display Models"),NO_VALUE);
		if(nValue!=NO_VALUE)
		{
			m_bDispModel=nValue;
		}

		nValue=pApp->GetProfileInt(_T("Settings"),_T("Display Atoms"),NO_VALUE);
		if(nValue!=NO_VALUE)
		{
			m_bDispAtom=nValue;
		}

		nValue=pApp->GetProfileInt(_T("Settings"),_T("Display Connections"),NO_VALUE);
		if(nValue!=NO_VALUE)
		{
			m_bDispConnections=nValue;
		}

		nValue=pApp->GetProfileInt(_T("Settings"),_T("Display References"),NO_VALUE);
		if(nValue!=NO_VALUE)
		{
			m_bDispReference=nValue;
		}

		nValue=pApp->GetProfileInt(_T("Settings"),_T("Display Sets"),NO_VALUE);
		if(nValue!=NO_VALUE)
		{
			m_bDispSet=nValue;
		}

		nValue=pApp->GetProfileInt(_T("Settings"),_T("Store Tree In Registry"),NO_VALUE);
		if(nValue!=NO_VALUE)
		{
			m_bStoreTreeInRegistry=nValue;
		}

		nValue=pApp->GetProfileInt(_T("Settings"),_T("Dynamic Loading"),NO_VALUE);
		if(nValue!=NO_VALUE)
		{
			m_bIsDynamicLoading=nValue;
		}


		nValue=pApp->GetProfileInt(_T("Settings"),_T("Sort Option"),NO_VALUE);
		if(nValue!=NO_VALUE)
		{
			m_soSortOptions=(eSortOptions)nValue;
		}

}

BOOL CAggregateOptions::IsDisplayedType(objtype_enum otType)
{

	switch(otType)
	{
		case OBJTYPE_FOLDER: return TRUE;
		case OBJTYPE_MODEL: return m_bDispModel;
		case OBJTYPE_ATOM: return m_bDispAtom;
		case OBJTYPE_CONNECTION: return m_bDispConnections;
		case OBJTYPE_REFERENCE: return m_bDispReference;
		case OBJTYPE_SET: return  m_bDispSet;
		default: return FALSE;
	}
}
