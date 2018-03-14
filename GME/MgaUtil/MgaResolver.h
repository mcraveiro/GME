// MgaResolver.h : Declaration of the CMgaResolver

#ifndef __MGARESOLVER_H_
#define __MGARESOLVER_H_

#include "resource.h"       // main symbols
#include "Gme.h"

#define RESOLV_ERR_COM_ERR 0x87640001 // COM function call error

#define RESOLV_ERR_get_FolderByStr    0x87640002 // function failed in resolving
#define RESOLV_ERR_get_AttrByStr      0x87640012 // function failed in resolving
#define RESOLV_ERR_get_KindByStr      0x87640022 // function failed in resolving
#define RESOLV_ERR_get_RoleByStr      0x87640032 // function failed in resolving
#define RESOLV_ERR_get_RoleByMeta     0x87640042 // function failed in resolving
#define RESOLV_ERR_get_ConnRoleByMeta 0x87640102 // function failed in resolving
#define RESOLV_ERR_get_RefRoleByMeta  0x87640112 // function failed in resolving
#define RESOLV_ERR_get_RolesByMetas   0x87640122 // function failed in resolving

/////////////////////////////////////////////////////////////////////////////

#define map_type_FolderByStr CTypedPtrList<CPtrList, Cmap_elmt_type_FolderByStr*>
#define map_type_KindByStr CTypedPtrList<CPtrList, Cmap_elmt_type_KindByStr*>
#define map_type_RoleByStr CTypedPtrList<CPtrList, Cmap_elmt_type_RoleByStr*>
#define map_type_AttrByStr CTypedPtrList<CPtrList, Cmap_elmt_type_AttrByStr*>
#define map_type_RoleByPathStr CTypedPtrList<CPtrList, Cmap_elmt_type_RoleByPathStr*>

/////////////////////////////////////////////////////////////////////////////

class Cmap_elmt_type_FolderByStr {

public:
	// from:
	CComPtr<IMgaMetaFolder> m_parent;
	CString m_kind;
	// to:
	CComPtr<IMgaMetaFolder> p;
public:
	Cmap_elmt_type_FolderByStr(IMgaMetaFolder * immf, CString _m_kind) : 
	  m_kind(_m_kind), m_parent(immf) {}

	bool operator==(const Cmap_elmt_type_FolderByStr & c2) const {
		if	(
			(m_parent == c2.m_parent)
			&&
			(m_kind == c2.m_kind)
			)
			return true;
		else
			return false;
	}
};



class Cmap_elmt_type_KindByStr {

public:
	// from:
	CComPtr<IMgaMetaFolder> m_parent;
	CString m_kind;
	objtype_enum m_objtype;
	// to:
	CComPtr<IMgaMetaFCO> p;
public:
	Cmap_elmt_type_KindByStr(IMgaMetaFolder * immf, CString _m_kind, objtype_enum _m_objtype) : 
	  m_kind(_m_kind), m_parent(immf), m_objtype(_m_objtype) {}

	bool operator==(const Cmap_elmt_type_KindByStr & c2) const {
		if	(
			(m_parent == c2.m_parent)
			&&
			(m_kind == c2.m_kind)
			&&
			(m_objtype == c2.m_objtype)
			)
			return true;
		else
			return false;
	}

};

class Cmap_elmt_type_RoleByStr {

public:
	// from:
	CComPtr<IMgaMetaModel> m_parent;
	CString m_kind;
	objtype_enum m_objtype;
	CString m_role;
	CString m_aspect;
	// to:
	CComPtr<IMgaMetaRole> p;
public:
	Cmap_elmt_type_RoleByStr(IMgaMetaModel * immfco, CString _m_kind,
		objtype_enum _objtype, CString role, CString aspect) : 
	  m_kind(_m_kind), m_parent(immfco), m_objtype(_objtype),
	  m_role(role), m_aspect(aspect) {}

	bool operator==(const Cmap_elmt_type_RoleByStr & c2) const {
		if	(
			(m_parent == c2.m_parent)
			&&
			(m_kind == c2.m_kind)
			&&
			(m_objtype == c2.m_objtype)
			&&
			(m_role == c2.m_role)
			&&
			(m_aspect == c2.m_aspect)
			)
			return true;
		else
			return false;
	}
	
};

class Cmap_elmt_type_AttrByStr {

public:
	// from:
	CComPtr<IMgaMetaFCO> m_parent;
	CString m_kind;
	// to:
	CComPtr<IMgaMetaAttribute> p;
public:
	Cmap_elmt_type_AttrByStr(IMgaMetaFCO * immfco, CString _m_kind) : 
	  m_kind(_m_kind), m_parent(immfco) {}

	bool operator==(const Cmap_elmt_type_AttrByStr & c2) const {
		if	(
			(m_parent == c2.m_parent)
			&&
			(m_kind == c2.m_kind)
			)
			return true;
		else
			return false;
	}
};

class Cmap_elmt_type_RoleByPathStr { 
	// from: 
public:
	CString m_path;
	objtype_enum m_objtype;
	CComPtr<IMgaMetaBase> m_parent; 
	// to:
	CComPtr<IMgaMetaRole> p;
public:
	Cmap_elmt_type_RoleByPathStr(CString _m_path, IMgaMetaBase *parent, objtype_enum _objtype) : 
			m_path(_m_path), m_parent(parent), m_objtype(_objtype) {}

	bool operator==(const Cmap_elmt_type_RoleByPathStr & c2) const {
			return m_path == c2.m_path && 
				m_objtype == c2.m_objtype &&
				m_parent == c2.m_parent;
	}
};


/////////////////////////////////////////////////////////////////////////////
// CMgaResolver
class ATL_NO_VTABLE CMgaResolver : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CMgaResolver, &__uuidof(MgaResolver)>,
	public IDispatchImpl<IMgaResolver, &__uuidof(IMgaResolver), &__uuidof(__MGAUtilLib)>,
	public ISupportErrorInfoImpl<&__uuidof(IMgaResolver)>
{
public:
	CMgaResolver() : mb_is_interactive(true), mb_use_sticky(false)
		, mb_name_alteration_sticky( false)
		, m_alter_option( CH_NONE)
	{
	}

	~CMgaResolver() { this->Clear(); }

DECLARE_REGISTRY_RESOURCEID(IDR_MGARESOLVER)
DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CMgaResolver)
	COM_INTERFACE_ENTRY(IMgaResolver)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()

public:
	// enums
	typedef enum { CH_NONE, CH_PREFIX, CH_TRUNCATE, CH_MIGRATE } alter_mode_enum;

private:
	// namespace related methods
	bool isAlterationSet() const;
	void setAlteration( alter_mode_enum v);

	CComBSTR prefixIt(CComBSTR pIn);
	CComBSTR truncIt(CComBSTR pIn);

	// namespace related vars
	alter_mode_enum      m_alter_option;
	CComBSTR             m_prefix;
	CComBSTR             m_trunc;
	bool                 mb_name_alteration_sticky;

	bool mb_is_interactive, mb_use_sticky;
	void clear_mappings();

	map_type_FolderByStr map_FolderByStr;
	map_type_KindByStr   map_KindByStr;
	map_type_RoleByStr   map_RoleByStr;
	map_type_AttrByStr   map_AttrByStr;
	map_type_RoleByPathStr  map_RoleByPathStr;

	bool map_get_FolderByStr(IMgaMetaFolder *&parent, BSTR kind, IMgaMetaFolder **p);
	bool map_get_AttrByStr(IMgaMetaFCO *&parent, BSTR kind, IMgaMetaAttribute **p);
	bool map_get_KindByStr(IMgaMetaFolder *&parent, BSTR kind, objtype_enum objtype, IMgaMetaFCO **p);
	bool map_get_RoleByStr(IMgaMetaModel *&parent, BSTR kind, objtype_enum objtype,
		BSTR &role, BSTR &aspect, IMgaMetaRole **p);
	bool map_get_RoleByPathStr(BSTR path, IMgaMetaBase *parent, objtype_enum objtype, IMgaMetaRole **p);

	bool map_put_FolderByStr(IMgaMetaFolder *&parent , BSTR kind, IMgaMetaFolder *p);
	bool map_put_AttrByStr(IMgaMetaFCO *&parent, BSTR kind, IMgaMetaAttribute *p);
	bool map_put_KindByStr(IMgaMetaFolder *&parent, BSTR kind, objtype_enum objtype, IMgaMetaFCO *p);
	bool map_put_RoleByStr(IMgaMetaModel *&parent, BSTR kind, objtype_enum objtype,
		BSTR &role, BSTR &aspect, IMgaMetaRole *p);
	bool map_put_RoleByPathStr(BSTR path, IMgaMetaBase *parent, objtype_enum objtype, IMgaMetaRole *p);

	CComPtr<IGMEOLEApp>	get_GME(CComPtr<IMgaProject> project);


// IMgaResolver
public:

	STDMETHOD(Clear)();
	STDMETHOD(put_IsInteractive)(VARIANT_BOOL p);
	STDMETHOD(get_IsInteractive)(VARIANT_BOOL *p);

	STDMETHOD(put_IsStickyEnabled)(VARIANT_BOOL p);
	STDMETHOD(get_IsStickyEnabled)(VARIANT_BOOL *p);

	STDMETHOD(getUserOptions)();

	STDMETHOD(get_FolderByStr)(IMgaFolder *parent, BSTR kind, IMgaMetaFolder **p);
	STDMETHOD(GetFolderByStrDisp)(IMgaFolder *parent, BSTR kind, IMgaMetaFolder **p) { return get_FolderByStr( parent, kind, p); }
	STDMETHOD(get_KindByStr)(IMgaFolder *parent, BSTR kind, objtype_enum objtype, IMgaMetaFCO **p);
	STDMETHOD(GetKindByStrDisp)(IMgaFolder *parent, BSTR kind, objtype_enum objtype, IMgaMetaFCO **p) { return get_KindByStr( parent, kind, objtype, p); }
	STDMETHOD(get_RoleByStr)(IMgaModel *parent, BSTR kind, objtype_enum objtype,
		BSTR role, BSTR aspect, IMgaMetaRole **p);
	STDMETHOD(GetRoleByStrDisp)(IMgaModel *parent, BSTR kind, objtype_enum objtype,
		BSTR role, BSTR aspect, IMgaMetaRole **p) { return get_RoleByStr( parent, kind, objtype, role, aspect, p); }
	STDMETHOD(get_AttrByStr)(IMgaFCO *parent, BSTR kind, IMgaMetaAttribute **p);
	STDMETHOD(GetAttrByStrDisp)(IMgaFCO *parent, BSTR kind, IMgaMetaAttribute **p) { return get_AttrByStr( parent, kind, p); }

	STDMETHOD(get_RoleByMeta)(IMgaModel *parent, IMgaMetaFCO *kind, objtype_enum objtype,
		IMgaMetaRole *role, IMgaMetaAspect *aspect, IMgaMetaRole **p);

	STDMETHOD(GetRoleByMetaDisp)(IMgaModel *parent, IMgaMetaFCO *kind, objtype_enum objtype,
		IMgaMetaRole *role, IMgaMetaAspect *aspect, IMgaMetaRole **p) { return get_RoleByMeta( parent, kind, objtype, role, aspect, p); }

	STDMETHOD(get_ConnRoleByMeta)(IMgaModel *parent, IMgaMetaAspect *aspect, 
		IMgaFCO *src, IMgaFCO *src_port, IMgaFCO *dst, IMgaFCO *dst_port, IMgaMetaRole **p);
	STDMETHOD(GetConnRoleByMetaDisp)(IMgaModel *parent, IMgaMetaAspect *aspect, 
		IMgaFCO *src, IMgaFCO *src_port, IMgaFCO *dst, IMgaFCO *dst_port, IMgaMetaRole **p) { return get_ConnRoleByMeta( parent, aspect, src, src_port, dst, dst_port, p); }
	STDMETHOD(get_RefRoleByMeta)(IMgaModel *parent, IMgaMetaAspect *aspect, 
		IMgaFCO *src, IMgaMetaRole **p);
	STDMETHOD(GetRefRoleByMetaDisp)(IMgaModel *parent, IMgaMetaAspect *aspect, 
		IMgaFCO *src, IMgaMetaRole **p) { return get_RefRoleByMeta( parent, aspect, src, p); }

	STDMETHOD(get_RolesByMetas)(IMgaModel *dst_parent, IMgaFCOs *src, 
		IMgaMetaAspect * aspect, BSTR sz_aspect, IMgaMetaRoles ** p);

	STDMETHOD(GetRolesByMetasDisp)(IMgaModel *dst_parent, IMgaFCOs *src, 
		IMgaMetaAspect * aspect, BSTR sz_aspect, IMgaMetaRoles ** p) { return get_RolesByMetas( dst_parent, src, aspect, sz_aspect, p); }

};

#endif //__MGARESOLVER_H_
