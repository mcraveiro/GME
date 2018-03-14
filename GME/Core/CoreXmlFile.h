#ifndef MGA_COREXMLFILE_H
#define MGA_COREXMLFILE_H

#include "resource.h"
#include <fstream.h>
#include <list>
#include <map>
#include <hash_map>
#include <set>
#include <xercesc/dom/dom.hpp>
#include "..\Mga\MgaGeneric.h"

const metaobjidpair_type ROOT_ID_PAIR = {METAID_ROOT, OBJID_ROOT};

struct GUID_less
{
	bool operator()(const GUID& a, const GUID& b) const
	{
        if( a.Data1 < b.Data1 )
            return true;
        else if( a.Data1 > b.Data1 )
            return false;
        else
        {
            DWORD * adw = (DWORD*)&a;
            DWORD * bdw = (DWORD*)&b;
            if( *(adw+1) < *(bdw+1) )
                return true;
            else if( *(adw+1) > *(bdw+1) )
                return false;
            else if( *(adw+2) < *(bdw+2) )
                return true;
            else if( *(adw+2) > *(bdw+2) )
                return false;
            else if( *(adw+3) < *(bdw+3) )
                return true;
            else 
                return false;
        }
	}
};

class XmlObject;

typedef set<XmlObject*>                 XmlObjSet;
typedef XmlObjSet::iterator             XmlObjSetIter;
typedef vector<XmlObject*>              XmlObjVec;
typedef XmlObjVec::iterator             XmlObjVecIter;
typedef map<metaid_type,attrid_type>    ParentMap;
typedef map<GUID, XmlObject*,GUID_less> GUIDToXmlObjectMap;
typedef GUIDToXmlObjectMap::iterator    GUIDToXmlObjectMapIter;


////////////////////////////////////////////////////////////////////////////////
// XmlAttrBase class
////////////////////////////////////////////////////////////////////////////////
class XmlAttrBase
{
public:
    static XmlAttrBase * create      (valtype_type type);

                         XmlAttrBase ();
    virtual             ~XmlAttrBase ();


    virtual valtype_type getType     () const            = 0;
    virtual void         fromVariant (VARIANT p)         {}
    virtual void         toVariant   (VARIANT *p) const  {}
    virtual void         fromString  (const char * str)  {}
    virtual void         toString    (string& str) const {}
};

typedef map<attrid_type,XmlAttrBase*>   AttribMap;
typedef AttribMap::iterator             AttribMapIter;


////////////////////////////////////////////////////////////////////////////////
// XmlAttrLong class
////////////////////////////////////////////////////////////////////////////////

class XmlAttrLong: public XmlAttrBase
{
public:
                         XmlAttrLong ();

    virtual valtype_type getType     () const;
    virtual void         fromVariant (VARIANT p);
	virtual void         toVariant   (VARIANT *p) const;
    virtual void         fromString  (const char * str);
    virtual void         toString    (string& str) const;
protected:
    long    m_value;
};

////////////////////////////////////////////////////////////////////////////////
// XmlAttrReal class
////////////////////////////////////////////////////////////////////////////////

class XmlAttrReal: public XmlAttrBase
{
public:
                         XmlAttrReal ();

    virtual valtype_type getType     () const;
    virtual void         fromVariant (VARIANT p);
	virtual void         toVariant   (VARIANT *p) const;
    virtual void         fromString  (const char * str);
    virtual void         toString    (string& str) const;
protected:
    double  m_value;
};

////////////////////////////////////////////////////////////////////////////////
// XmlAttrString class
////////////////////////////////////////////////////////////////////////////////

class XmlAttrString: public XmlAttrBase
{
public:
    virtual valtype_type getType     () const;
    virtual void         fromVariant (VARIANT p);
	virtual void         toVariant   (VARIANT *p) const;
    virtual void         fromString  (const char * str);
    virtual void         toString    (string& str) const;
protected:
    string  m_value;
};

////////////////////////////////////////////////////////////////////////////////
// XmlAttrBinary class
////////////////////////////////////////////////////////////////////////////////

class XmlAttrBinary: public XmlAttrBase
{
public:
    virtual valtype_type getType     () const;
    virtual void         fromVariant (VARIANT p);
	virtual void         toVariant   (VARIANT *p) const;
    virtual void         fromString  (const char * str);
    virtual void         toString    (string& str) const;
protected:
    bindata m_value;
};

////////////////////////////////////////////////////////////////////////////////
// XmlAttrLock class
////////////////////////////////////////////////////////////////////////////////

class XmlAttrLock: public XmlAttrBase
{
public:
                         XmlAttrLock ();

    virtual valtype_type getType     () const;
    virtual void         fromVariant (VARIANT p);
	virtual void         toVariant   (VARIANT *p) const;
    virtual void         fromString  (const char * str);
    virtual void         toString    (string& str) const;
protected:
    lockval_type    m_value;
};

////////////////////////////////////////////////////////////////////////////////
// XmlAttrPointer class
////////////////////////////////////////////////////////////////////////////////

class XmlAttrPointer: public XmlAttrBase
{
public:
                         XmlAttrPointer ();

    virtual valtype_type getType        () const;
protected:
    XmlObject * m_parent;

    friend class CCoreXmlFile;
};

////////////////////////////////////////////////////////////////////////////////
// XmlAttrCollection class
////////////////////////////////////////////////////////////////////////////////

class XmlAttrCollection: public XmlAttrBase
{
public:
    virtual valtype_type getType     () const;
protected:
    XmlObjSet   m_children;

    friend class CCoreXmlFile;
};


////////////////////////////////////////////////////////////////////////////////
// XmlObject class
////////////////////////////////////////////////////////////////////////////////

class XmlObject
{
public:
                XmlObject           (ICoreMetaObject *metaobject, bool createAllAttributes=true);
               ~XmlObject           ();

protected:
    void        createAttributes        (ICoreMetaObject *metaobject, bool all=false);
    void        deleteSecondaryAttribs  ();

protected:
    long               m_metaid;
    GUID               m_guid;
    int                m_index;            // index in the m_objects array
    AttribMap          m_attributes;
    bool               m_loaded;           // true if all attributes are loded (otherwise only pointer, collections and locks stored)

    bool               m_deleted;
    //bool               m_readOnly;
    bool               m_modified;

    //GUID               m_fileLevelObject;  // point to the ascendent which stores this, or GUID_NULL if this is a file level object

    friend class CCoreXmlFile;
};

struct UnresolvedPointer
{
    XmlObject   * m_object;             // whose attribute is this
    attrid_type   m_attrib;
    GUID          m_pointedObjGuid;
};

typedef vector<UnresolvedPointer>       UnresolvedPointerVec;
typedef UnresolvedPointerVec::iterator  UnresolvedPointerVecIt;


class ATL_NO_VTABLE CCoreXmlFile : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<ICoreStorage, &IID_ICoreStorage, &LIBID_CORELib>,
	public ISupportErrorInfoImpl<&IID_ICoreStorage>,
	public CComCoClass<CCoreXmlFile, &CLSID_CoreBinFile>
{
public:
	CCoreXmlFile();
	~CCoreXmlFile();

BEGIN_COM_MAP(CCoreXmlFile)
	COM_INTERFACE_ENTRY(ICoreStorage)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()

DECLARE_REGISTRY_RESOURCEID(IDR_COREXMLFILE)

public:
	STDMETHODIMP get_MetaProject        (ICoreMetaProject **p);
	STDMETHODIMP put_MetaProject        (ICoreMetaProject *p);
	STDMETHODIMP get_MetaObject         (ICoreMetaObject **p);
	STDMETHODIMP put_MetaObject         (ICoreMetaObject *p);
	STDMETHODIMP get_MetaID             (metaid_type *p);
	STDMETHODIMP put_MetaID             (metaid_type p);
	STDMETHODIMP get_MetaAttribute      (ICoreMetaAttribute **p);
	STDMETHODIMP put_MetaAttribute      (ICoreMetaAttribute *p);
	STDMETHODIMP get_AttrID             (attrid_type *p);
	STDMETHODIMP put_AttrID             (attrid_type p);
	STDMETHODIMP get_AttributeValue     (VARIANT *p);
	STDMETHODIMP put_AttributeValue     (VARIANT p);
	STDMETHODIMP OpenObject             (objid_type objid);
	STDMETHODIMP CreateObject           (objid_type *objid);
	STDMETHODIMP CloseObject            ();
	STDMETHODIMP DeleteObject           ();
	STDMETHODIMP LockObject             ();
	STDMETHODIMP OpenProject            (BSTR connection, VARIANT_BOOL *ro_mode);
	STDMETHODIMP CreateProject          (BSTR connection);
    STDMETHODIMP SaveProject            (BSTR connection, VARIANT_BOOL keepoldname);
	STDMETHODIMP CloseProject           (VARIANT_BOOL abort = VARIANT_FALSE);
	STDMETHODIMP DeleteProject          ();
	STDMETHODIMP BeginTransaction       ();
	STDMETHODIMP CommitTransaction      ();
	STDMETHODIMP AbortTransaction       ();
	STDMETHODIMP get_StorageType        (long *p);

protected:
    void         fillParentMap          ();

    void         closeMetaProject       ();
    void         openMetaObject         ();
    void         closeMetaObject        ();
    void         openMetaAttribute      ();
    void         closeMetaAttribute     ();

    void         setFolderPathOnCreate  (BSTR connection);
    void         setFolderPathOnOpen    (BSTR connection);

    // graph operatrions
    void         clearAll               ();

    void         addObject              (XmlObject * obj);

    void         removeObject           (XmlObject * obj);

    void         setPointer             (XmlObject * obj, attrid_type attribId, XmlObject * pointed);

    void         setPointer             (XmlAttrPointer * attr, VARIANT p);
    
    void         updateCollections      ();

    void         resolvePointers        (UnresolvedPointerVec& pointers);

    // getting connections
    void         getPointer             (XmlAttrPointer * attr, VARIANT * p);

    void         getCollection          (XmlAttrCollection * attr, VARIANT * p);

    // object pointer and id conversion
    XmlObject *  objectFromObjId        (metaobjidpair_type idpair);

    void         objIdFromObject        (XmlObject * obj, metaobjidpair_type& idpair);
    
    // serialization
    void         writeBinaryCache       ();

    void         readBinaryCache        ();

    void         writeAll               ();

    void         writeXMLFile           (XmlObject * container);

    void         writeObject            (XmlObject * obj, FILE * file, bool container, const char * prefix);

    void         readXMLFile            (const char * fileName, UnresolvedPointerVec& pointers );

    void         readObject             (XERCES_CPP_NAMESPACE::DOMElement * e, UnresolvedPointerVec& pointers, XmlObject * parent);

    void         readAll                ();



    void         incrementalUpdate      ();
    

protected:
    // meta project
    CComObjPtr<ICoreMetaProject>    m_metaProject;

    // meta object
    CComObjPtr<ICoreMetaObject>     m_metaObject;
    metaid_type                     m_metaObjectId;

    // meta attribute
    CComObjPtr<ICoreMetaAttribute>  m_metaAttribute;
	attrid_type                     m_metaAttributeId;
    valtype_type                    m_metaAttributeValType;

    ParentMap                       m_parentMap;
    
    // project
    bool                            m_opened;
    bool                            m_modified;
    string                          m_folderPath;   // absolute path of the folder contains the project files
    string                          m_cacheFileName;
    bool                            m_inTransaction;

    // objects
    XmlObjVec                       m_objects;
    GUIDToXmlObjectMap              m_objectsByGUID;
    XmlObject                     * m_openedObject;
    XmlObject                     * m_root;
};

#endif//MGA_COREXMLFILE_H
