// Attribute.h: interface for the CAttribute class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ATTRIBUTE_H__895B04C9_5A11_4D55_ABCD_49E020BE4C5E__INCLUDED_)
#define AFX_ATTRIBUTE_H__895B04C9_5A11_4D55_ABCD_49E020BE4C5E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef CComPtr<IMgaMetaAttribute>MgaMetaAttributePtr;
#define ADAPT_META_ATTRIBUTE(x) CAdapt<MgaMetaAttributePtr>(x)
typedef CList<CAdapt<MgaMetaAttributePtr>,CAdapt<MgaMetaAttributePtr>&>CMgaMetaAttributePtrList;


class CAttribute  
{
	CMgaMetaAttributePtrList m_MetaAttributePtrList;
public:
	CAttribute();
	virtual ~CAttribute();
	void CreateList(const CMgaFCOPtrList& MgaFCOPtrList,CArray<CListItem,CListItem&> &ListItemArray);
	void CreateList(CComPtr<IMgaProject> ccpProject, CArray<CListItem,CListItem&> &ListItemArray);
	void WriteItemToMga(CListItem ListItem,const CMgaFCOPtrList& MgaFCOPtrList);
	void WriteItemToMga(CListItem ListItem, CComPtr<IMgaProject> ccpProject);
	
private:
	bool CreateListItem(CListItem& ListItem, const CComVariant& ccvtValue, bool bIsDirty, bool bIsDefault, POSITION dwKey);
	bool isViewable( const CComPtr<IMgaFCO>& r, const CComPtr<IMgaMetaAttribute>& a) const;
};

#endif // !defined(AFX_ATTRIBUTE_H__895B04C9_5A11_4D55_ABCD_49E020BE4C5E__INCLUDED_)
