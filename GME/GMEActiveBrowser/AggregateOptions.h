// AggregateOptions.h: interface for the CAggregateOptions class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AGGREGATEOPTIONS_H__F55D03CE_8EDB_422E_905B_67B46C9E5895__INCLUDED_)
#define AFX_AGGREGATEOPTIONS_H__F55D03CE_8EDB_422E_905B_67B46C9E5895__INCLUDED_

#pragma once

enum eSortOptions{SORT_BYNAME,SORT_BYTYPE,SORT_BYCREATION};

class CAggregateOptions  
{
public:
	BOOL m_bIsDynamicLoading;
	BOOL IsDisplayedType(objtype_enum otType);
	void LoadFromRegistry();
	void SaveToRegistry();
	BOOL m_bDispModel;
	BOOL m_bDispAtom;
	BOOL m_bDispReference;
	BOOL m_bDispSet;
	BOOL m_bDispConnections;
	BOOL m_bStoreTreeInRegistry;

	eSortOptions m_soSortOptions;
	CAggregateOptions();
	virtual ~CAggregateOptions();

};

#endif // !defined(AFX_AGGREGATEOPTIONS_H__F55D03CE_8EDB_422E_905B_67B46C9E5895__INCLUDED_)
