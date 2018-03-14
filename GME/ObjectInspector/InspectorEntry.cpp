// InspectorEntry.cpp: implementation of the CInspectorEntry class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "objectinspector.h"
#include "InspectorEntry.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CInspectorEntry::CInspectorEntry()
{
	m_name.Empty();
	m_ROval.Empty();
}

CInspectorEntry::~CInspectorEntry()
{

}

void CInspectorEntry::InitFromParameter(const CString &name, const CString &val)
{
	m_name = name;
	m_ROval = val;
}

void CInspectorEntry::InitFromRegistry(const CString &name, const CComPtr<IMgaRegNode> &regnode)
{
	m_name = name;
	m_regnode = regnode;
}

void CInspectorEntry::InitFromAttribute(const CString &name, const CComPtr<IMgaAttribute> &attr)
{
	m_name = name;
	m_attribute = attr;
}

CString CInspectorEntry::GetHelp()
{
	return m_help;
}

int CInspectorEntry::GetEntryHeight(CDC *pDC)
{
	return 14;
}

void CInspectorEntry::DrawEntry(CDC *pDC, const CRect &rect)
{

}
