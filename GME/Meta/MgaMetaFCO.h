
#ifndef MGA_MGAMETAFCO_H
#define MGA_MGAMETAFCO_H

#ifndef MGA_MGAMETABASE_H
#include "MgaMetaBase.h"
#endif

// --------------------------- CMgaMetaFCO

class ATL_NO_VTABLE CMgaMetaFCO :
	public CMgaMetaBase
{
public:
	static HRESULT get_AttributeByName(IMgaMetaFCO *me, BSTR name, IMgaMetaAttribute **p);
	static HRESULT get_AttributeByRef(IMgaMetaFCO *me, metaref_type mref, IMgaMetaAttribute **p);

	static HRESULT get_DefinedAttributeByName(IMgaMetaFCO *me, BSTR name, 
		VARIANT_BOOL inscope, IMgaMetaAttribute **p);

	static void Traverse(CMgaMetaProject *metaproject, CCoreObjectPtr &me);
	static HRESULT get_AliasingEnabled(IMgaMetaFCO *me, VARIANT_BOOL *pVal);
	static HRESULT put_AliasingEnabled(IMgaMetaFCO *me, VARIANT_BOOL newVal);
};

// --------------------------- DECLARE

#define DECLARE_MGAMETAFCO() \
DECLARE_MGAMETABASE() \
\
public: \
	STDMETHOD(get_DefinedIn)(IMgaMetaBase **p) \
	{ return ComGetPointerValue(GetUnknown(), ATTRID_DEFFCO_PTR, p); } \
\
	STDMETHOD(get_DefinedAttributes)(IMgaMetaAttributes **p) \
	{ return ComGetSortedCollValue<IMgaMetaAttribute>(GetUnknown(), ATTRID_DEFATTR_PTR, p); } \
\
	STDMETHOD(get_DefinedAttributeByName)(BSTR name, VARIANT_BOOL inscope, IMgaMetaAttribute **p) \
	{ return CMgaMetaFCO::get_DefinedAttributeByName(this, name, inscope, p); } \
\
	STDMETHOD(GetDefinedAttributeByNameDisp)(BSTR name, VARIANT_BOOL inscope, IMgaMetaAttribute **p) \
	{ return get_DefinedAttributeByName( name, inscope, p); } \
\
	STDMETHOD(get_UsedInRoles)(IMgaMetaRoles **p) \
	{ return ComGetCollectionValue<IMgaMetaRole>(GetUnknown(), ATTRID_KIND_PTR, p); } \
\
	STDMETHOD(get_UsedInFolders)(IMgaMetaFolders **p) \
	{ return ComGetLinkCollectionValue<IMgaMetaFolder>(GetUnknown(), \
		ATTRID_ROOTOBJLINK_OBJECT_PTR, ATTRID_ROOTOBJLINK_FOLDER_PTR, p); } \
\
	STDMETHOD(get_Attributes)(IMgaMetaAttributes **p) \
	{ return ComGetSortedLinkCollValue<IMgaMetaAttribute>(GetUnknown(), \
		ATTRID_ATTRLINK_USEDIN_PTR, ATTRID_ATTRLINK_ATTR_PTR, p); } \
\
	STDMETHOD(get_AttributeByName)(BSTR name, IMgaMetaAttribute **p) \
	{ return CMgaMetaFCO::get_AttributeByName(this, name, p); } \
\
	STDMETHOD(GetAttributeByNameDisp)(BSTR name, IMgaMetaAttribute **p) \
	{ return get_AttributeByName( name, p); } \
\
	STDMETHOD(get_AttributeByRef)(metaref_type mref, IMgaMetaAttribute **p) \
	{ return CMgaMetaFCO::get_AttributeByRef(this, mref, p); } \
\
	STDMETHOD(GetAttributeByRefDisp)(metaref_type mref, IMgaMetaAttribute **p) \
	{ return get_AttributeByRef( mref, p); } \
\
	STDMETHOD(DefineAttribute)(IMgaMetaAttribute **p) \
	{ return ComDefineBase(this, METAID_METAATTRIBUTE, ATTRID_DEFATTR_PTR, p); } \
\
	STDMETHOD(AddAttribute)(IMgaMetaAttribute *p) \
	{ return ComAddLink(this, METAID_METAATTRLINK, ATTRID_ATTRLINK_USEDIN_PTR, \
		ATTRID_ATTRLINK_ATTR_PTR, p); } \
\
	STDMETHOD(RemoveAttribute)(IMgaMetaAttribute *p) \
	{ return ComRemoveLink(this, METAID_METAATTRLINK, ATTRID_ATTRLINK_USEDIN_PTR, \
		ATTRID_ATTRLINK_ATTR_PTR, p); }\
\
	STDMETHOD(get_AliasingEnabled)(VARIANT_BOOL *pVal) \
	{ return CMgaMetaFCO::get_AliasingEnabled(this, pVal); } \
\
	STDMETHOD(put_AliasingEnabled)(VARIANT_BOOL newVal) \
	{ return CMgaMetaFCO::put_AliasingEnabled(this, newVal); } 

#endif//MGA_MGAMETAFCO_H
