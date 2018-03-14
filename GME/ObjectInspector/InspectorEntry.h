// InspectorEntry.h: interface for the CInspectorEntry class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INSPECTORENTRY_H__EC3A0721_9A2C_4BA0_9350_26E7ED5AFA7A__INCLUDED_)
#define AFX_INSPECTORENTRY_H__EC3A0721_9A2C_4BA0_9350_26E7ED5AFA7A__INCLUDED_

#pragma once

class CInspectorEntry;

typedef CTypedPtrList<CPtrList, CInspectorEntry *> CInspectorEntryList;

class CInspectorEntry  
{
public:
	virtual void DrawEntry(CDC *pDC, const CRect &rect);
	virtual int GetEntryHeight(CDC *pDC);
	virtual CString GetHelp();
	void InitFromParameter(const CString &name, const CString &val);
	void InitFromRegistry(const CString &name, const CComPtr<IMgaRegNode> &regnode);
	void InitFromAttribute(const CString &name, const CComPtr<IMgaAttribute> &attr);
	CString GetName()	{return m_name;};
	CInspectorEntry();
	virtual ~CInspectorEntry();

protected:
	CString m_help;
	virtual bool IsInitialized() { return ( (!m_ROval.IsEmpty()) || (m_regnode != NULL) || (m_attribute != NULL) ); }
	CString m_ROval;
	CComPtr<IMgaRegNode>	m_regnode;
	CComPtr<IMgaAttribute>	m_attribute;
	CString m_name;
};

#endif // !defined(AFX_INSPECTORENTRY_H__EC3A0721_9A2C_4BA0_9350_26E7ED5AFA7A__INCLUDED_)
