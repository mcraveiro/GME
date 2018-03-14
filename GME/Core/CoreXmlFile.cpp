#include "stdafx.h"
#include <stdio.h>
#include <io.h>
#include <stdlib.h>
#include "CoreXmlFile.h"
#include "CommonCollection.h"

using namespace XERCES_CPP_NAMESPACE;


void bin2string( const unsigned char * bytes, int len, string& str )
{
    char hex[3];
    str.clear();
    for( int i=0; i<len; ++i )
    {
        sprintf( hex, "%02x", bytes[i] );
        str += hex;
    }
}

void string2bin( const char * str, unsigned char * bytes )
{
    int l = strlen(str) / 2;
    for( int i=0; i<l; ++i )
    {
        unsigned char c;
        sscanf( str + i*2, "%02x", &c );
        bytes[i] = c;
    }
}

void guid2str( GUID guid, string& str )
{
    if( guid == GUID_NULL )
        str = "null";
    else
        bin2string( (unsigned char*)&guid, sizeof(GUID), str );
}

GUID str2guid( const char * str )
{
    int len = strlen(str);
    if( len == 0 || strcmp( str, "null" ) == 0 )
        return GUID_NULL;
    else
    {
        GUID guid;
        string2bin( str, (unsigned char*)&guid );
        return guid;
    }
}


////////////////////////////////////////////////////////////////////////////////
// XmlAttrBase class
////////////////////////////////////////////////////////////////////////////////

XmlAttrBase * XmlAttrBase::create(valtype_type valtype)
{
    ASSERT( valtype != VALTYPE_NONE );

	XmlAttrBase * xmlattr = NULL;

	switch(valtype)
	{
    case VALTYPE_LONG:
		xmlattr = new XmlAttrLong();
		break;

	case VALTYPE_STRING:
		xmlattr = new XmlAttrString();
		break;

	case VALTYPE_BINARY:
		xmlattr = new XmlAttrBinary();
		break;

	case VALTYPE_LOCK:
		xmlattr = new XmlAttrLock();
		break;

	case VALTYPE_POINTER:
		xmlattr = new XmlAttrPointer();
		break;

	case VALTYPE_COLLECTION:
		xmlattr = new XmlAttrCollection();
		break;

	case VALTYPE_REAL:
		xmlattr = new XmlAttrReal();
		break;

	default:
		HR_THROW(E_METAPROJECT);
	}

	if( xmlattr == NULL )
		HR_THROW(E_OUTOFMEMORY);

	return xmlattr;
}

XmlAttrBase::XmlAttrBase()
{ 
}
    
XmlAttrBase::~XmlAttrBase() 
{ 
}

////////////////////////////////////////////////////////////////////////////////
// XmlAttrLong class
////////////////////////////////////////////////////////////////////////////////

XmlAttrLong::XmlAttrLong()
{
    m_value = 0;
}

valtype_type XmlAttrLong::getType() const
{
    return VALTYPE_LONG;
}

void XmlAttrLong::fromVariant(VARIANT p)
{
    CopyTo(p, m_value);
}

void XmlAttrLong::toVariant(VARIANT *p) const
{
    CopyTo(m_value, p);
}

void XmlAttrLong::fromString(const char * str)
{
    if( str == NULL || strlen(str)==0 )
        m_value = 0;
    else
        m_value = atoi( str );
}

void XmlAttrLong::toString(string& str) const
{
    static char buf[100];
    sprintf( buf, "%d", m_value );
    str = buf;
}

////////////////////////////////////////////////////////////////////////////////
// XmlAttrReal class
////////////////////////////////////////////////////////////////////////////////

XmlAttrReal::XmlAttrReal()
{
    m_value = 0;
}

valtype_type XmlAttrReal::getType() const
{
    return VALTYPE_REAL;
}

void XmlAttrReal::fromVariant(VARIANT p)
{
    CopyTo(p, m_value);
}

void XmlAttrReal::toVariant(VARIANT *p) const
{
    CopyTo(m_value, p);
}

void XmlAttrReal::fromString(const char * str)
{
    if( str == NULL || strlen(str)==0 )
        m_value = 0;
    else
        m_value = atof( str );
}

void XmlAttrReal::toString(string& str) const
{
    static char buf[100];
    sprintf( buf, "%.6f", m_value );
    str = buf;
}

////////////////////////////////////////////////////////////////////////////////
// XmlAttrString class
////////////////////////////////////////////////////////////////////////////////

valtype_type XmlAttrString::getType() const
{
    return VALTYPE_STRING;
}

void XmlAttrString::fromVariant(VARIANT p)
{
    // TODO: test
    CopyTo(p, m_value);
}

void XmlAttrString::toVariant(VARIANT *p) const
{
    // TODO: test
    CopyTo(m_value, p);
}

void XmlAttrString::fromString(const char * str)
{
    if( str == NULL )
        m_value = "";
    else
        m_value = str;
}

void XmlAttrString::toString(string& str) const
{
    str = m_value;
}

////////////////////////////////////////////////////////////////////////////////
// XmlAttrBinary class
////////////////////////////////////////////////////////////////////////////////

valtype_type XmlAttrBinary::getType() const
{
    return VALTYPE_BINARY;
}

void XmlAttrBinary::fromVariant(VARIANT p)
{
    CopyTo(p, m_value);
}

void XmlAttrBinary::toVariant(VARIANT *p) const
{
    CopyTo(m_value, p);
}

void XmlAttrBinary::fromString(const char * str)
{   
    if( str == NULL || strlen(str) == 0 )
        m_value.clear();
    else
    {
        // TODO: optimize this code
        int             len  = strlen(str)/2;
        unsigned char * buff = new unsigned char[len];
        string2bin( str, buff );
        m_value.resize( len );
        for( int i=0; i<len; ++i )
            m_value [i] = buff[i];
        delete [] buff;
    }
}

void XmlAttrBinary::toString(string& str) const
{
    bin2string( m_value.begin(), m_value.size(), str );
}

////////////////////////////////////////////////////////////////////////////////
// XmlAttrLock class
////////////////////////////////////////////////////////////////////////////////

XmlAttrLock::XmlAttrLock()
{
    m_value = LOCKING_NONE;
}

valtype_type XmlAttrLock::getType() const
{
    return VALTYPE_LOCK;
}

void XmlAttrLock::fromVariant(VARIANT p)
{
    CopyTo(p, m_value);
}

void XmlAttrLock::toVariant(VARIANT *p) const
{
    CopyTo(m_value, p);
}

void XmlAttrLock::fromString(const char * str)
{   
    if( str == NULL || strlen(str) == 0 )
        m_value = LOCKING_NONE;
    else
        m_value = atoi( str );
}

void XmlAttrLock::toString(string& str) const
{
    static char buf[100];
    sprintf( buf, "%d", m_value );
    str = buf;
}

////////////////////////////////////////////////////////////////////////////////
// XmlAttrPointer class
////////////////////////////////////////////////////////////////////////////////

XmlAttrPointer::XmlAttrPointer()
{
    m_parent = NULL;
}

valtype_type XmlAttrPointer::getType() const
{
    return VALTYPE_POINTER;
}

////////////////////////////////////////////////////////////////////////////////
// XmlAttrCollection class
////////////////////////////////////////////////////////////////////////////////

valtype_type XmlAttrCollection::getType() const
{
    return VALTYPE_COLLECTION;
}

////////////////////////////////////////////////////////////////////////////////
// XmlObject class
////////////////////////////////////////////////////////////////////////////////

XmlObject::XmlObject(ICoreMetaObject *metaobject, bool createAllAttributes)
{
    m_deleted         = false;
    m_modified        = false;
    m_loaded          = createAllAttributes;

    //m_readOnly        = false;

    createAttributes(metaobject,m_loaded);
    CoCreateGuid(&m_guid);
}

XmlObject::~XmlObject()
{
    for( AttribMapIter it = m_attributes.begin(); it != m_attributes.end(); ++it )
        delete it->second;
}

void XmlObject::createAttributes(ICoreMetaObject *metaobject, bool all)
{
    ASSERT( metaobject != NULL );

	CComObjPtr<ICoreMetaAttributes> metaattributes;
	COMTHROW( metaobject->get_Attributes(PutOut(metaattributes)) );
	ASSERT( metaattributes != NULL );

	typedef vector< CComObjPtr<ICoreMetaAttribute> > metaattributelist_type;
	metaattributelist_type metaattributelist;
	GetAll<ICoreMetaAttributes, ICoreMetaAttribute>(metaattributes, metaattributelist);

    for( metaattributelist_type::iterator i=metaattributelist.begin(); i!=metaattributelist.end(); ++i )
    {
		valtype_type valtype;
		COMTHROW( (*i)->get_ValueType(&valtype) );
        if( all || valtype==VALTYPE_POINTER || valtype==VALTYPE_COLLECTION || valtype==VALTYPE_LOCK)
        {
		    attrid_type attrId = ATTRID_NONE;
		    COMTHROW( (*i)->get_AttrID(&attrId) );
            ASSERT( attrId != ATTRID_NONE );
            if( m_attributes.find(attrId) == m_attributes.end() )
                m_attributes.insert( AttribMap::value_type(attrId,XmlAttrBase::create(valtype)) );
        }
	}
}

void XmlObject::deleteSecondaryAttribs()
{
    for( AttribMapIter it = m_attributes.begin(); it != m_attributes.end(); ++it )
    {
        valtype_type type =it->second->getType();
        if( type!=VALTYPE_LOCK && type!=VALTYPE_POINTER && type!=VALTYPE_COLLECTION )
        {
            delete it->second;
            m_attributes.erase(it);
        }
    }
}


////////////////////////////////////////////////////////////////////////////////
// CCoreXmlFile class
////////////////////////////////////////////////////////////////////////////////

CCoreXmlFile::CCoreXmlFile()
{
    m_opened               = false;
    m_inTransaction        = false;
    m_modified             = false;
    m_metaAttributeId      = ATTRID_NONE;
    m_metaAttributeValType = VALTYPE_NONE;
    m_openedObject         = NULL;

    fillParentMap();

    XMLPlatformUtils::Initialize();    
}

CCoreXmlFile::~CCoreXmlFile()
{
    clearAll();
}

STDMETHODIMP CCoreXmlFile::get_MetaProject(ICoreMetaProject **p)
{
    CHECK_OUT(p);
	CopyTo(m_metaProject, p);
	return S_OK;
}

STDMETHODIMP CCoreXmlFile::put_MetaProject(ICoreMetaProject *p)
{
    COMTRY
	{
		closeMetaProject();
		m_metaProject = p;
	}
	COMCATCH( closeMetaProject() )
}

STDMETHODIMP CCoreXmlFile::get_MetaObject(ICoreMetaObject **p)
{
    CHECK_OUT(p);
	CopyTo(m_metaObject, p);
	return S_OK;
}

STDMETHODIMP CCoreXmlFile::put_MetaObject(ICoreMetaObject *p)
{
    if( m_metaProject == NULL )
		COMRETURN(E_INVALID_USAGE);

	if( m_metaObject == p )
		return S_OK;

	COMTRY
	{
		if( p != NULL )
		{
            // check the metaproject of the object, it must be the same as the metaproject
            // of the storage
			CComObjPtr<ICoreMetaProject> t;
			COMTHROW( p->get_Project(PutOut(t)) );
			if( !IsEqualObject(m_metaProject, t) )
				HR_THROW(E_SAMEPROJECT);
		}

		closeMetaObject();
		m_metaObject = p;
		if( m_metaObject != NULL )
			openMetaObject();
	}
	COMCATCH( closeMetaObject() )
}

STDMETHODIMP CCoreXmlFile::get_MetaID(metaid_type *p)
{
    CHECK_OUT(p);
	*p = m_metaObjectId;
	return S_OK;
}

STDMETHODIMP CCoreXmlFile::put_MetaID(metaid_type metaid)
{
    if( m_metaProject == NULL )
		COMRETURN(E_INVALID_USAGE);

	COMTRY
	{
		if( metaid != METAID_NONE )
		{
			CComObjPtr<ICoreMetaObject> p;
			COMTHROW( m_metaProject->get_Object(metaid, PutOut(p)) );
			ASSERT( p != NULL );

			if( m_metaObject != p )
			{
				closeMetaObject();
				MoveTo(p, m_metaObject);
				openMetaObject();
			}
		}
		else
			closeMetaObject();
	}
	COMCATCH( closeMetaObject() )
}

STDMETHODIMP CCoreXmlFile::get_MetaAttribute(ICoreMetaAttribute **p)
{
	CHECK_OUT(p);
	CopyTo(m_metaAttribute, p);
	return S_OK;
}

STDMETHODIMP CCoreXmlFile::put_MetaAttribute(ICoreMetaAttribute *p)
{
    if( m_metaObject == NULL )
		COMRETURN(E_INVALID_USAGE);
	ASSERT( m_metaProject != NULL );

	if( m_metaAttribute == p )
		return S_OK;

	COMTRY
	{
		if( m_metaAttribute != NULL )
		{
            // the metaobject of the given metaattribute must be the same as the metaobject of 
            // the storage
			CComObjPtr<ICoreMetaObject> t;
			COMTHROW( m_metaAttribute->get_Object(PutOut(t)) );
			if( !IsEqualObject(m_metaObject, t) )
			{
				m_metaAttribute = NULL;
				return E_INVALIDARG;
			}
		}

		closeMetaAttribute();
		m_metaAttribute = p;
		if( m_metaAttribute != NULL )
			openMetaAttribute();
	}
	COMCATCH( closeMetaAttribute() )
}

STDMETHODIMP CCoreXmlFile::get_AttrID(attrid_type *p)
{
	CHECK_OUT(p);
	if( m_metaAttribute )
		return m_metaAttribute->get_AttrID(p);
	*p = 0;
	return S_OK;
}

STDMETHODIMP CCoreXmlFile::put_AttrID(attrid_type attrid)
{
    if( m_metaObject == NULL )
		COMRETURN(E_INVALID_USAGE);
	ASSERT( m_metaProject != NULL );

	COMTRY
	{
		if( attrid != ATTRID_NONE )
		{
			CComObjPtr<ICoreMetaAttribute> p;
			COMTHROW( m_metaObject->get_Attribute(attrid, PutOut(p)) );
			ASSERT( p != NULL );

			if( m_metaAttribute != p )
			{
				closeMetaAttribute();
				m_metaAttribute = p;
				openMetaAttribute();
			}
		}
		else
			closeMetaAttribute();
	}
	COMCATCH( closeMetaAttribute() )
}

STDMETHODIMP CCoreXmlFile::get_AttributeValue(VARIANT *p)
{
    CHECK_OUT(p);

	if( m_openedObject == NULL || !m_inTransaction )
		COMRETURN(E_INVALID_USAGE);
	
	COMTRY
	{
        AttribMapIter it = m_openedObject->m_attributes.find( m_metaAttributeId );
        if( m_metaAttributeValType == VALTYPE_POINTER )
        {
            getPointer( (XmlAttrPointer*)it->second, p );
        }
        else if( m_metaAttributeValType == VALTYPE_COLLECTION )
        {
            getCollection( (XmlAttrCollection*)it->second, p );
        }
        else if( m_metaAttributeValType == VALTYPE_LOCK )
        {
            it->second->toVariant(p);
        }
        else
        {
            if( !m_openedObject->m_loaded )
            {
                // attributes are not in memory!!
            }
            else
            {
                it->second->toVariant(p);
            }
        }
	}
	COMCATCH(;)
}

STDMETHODIMP CCoreXmlFile::put_AttributeValue(VARIANT p)
{
    if( m_openedObject == NULL || !m_inTransaction )
		COMRETURN(E_INVALID_USAGE);

    // TODO: return with specific error code
    //if( m_openedObject->m_readOnly )
      //  COMRETURN(E_INVALID_USAGE);
	
	COMTRY
	{
        AttribMapIter it = m_openedObject->m_attributes.find( m_metaAttributeId );
        if( m_metaAttributeValType == VALTYPE_POINTER )
        {
            setPointer( (XmlAttrPointer*)it->second, p );
        }
        else if( m_metaAttributeValType == VALTYPE_COLLECTION )
        {
            ASSERT( false );
        }
        else if( m_metaAttributeValType == VALTYPE_LOCK )
        {
            it->second->fromVariant(p);
        }
        else
        {
            if( !m_openedObject->m_loaded )
            {
                // attributes are not in memory!!
            }
            else
            {
                it->second->fromVariant(p);
            }
        }

        m_openedObject->m_modified = true;
	}
	COMCATCH(;)
}

STDMETHODIMP CCoreXmlFile::OpenObject(objid_type objid)
{
    if( m_metaObject == NULL || !m_inTransaction )
		COMRETURN(E_INVALID_USAGE);
	   
    if( m_openedObject == (XmlObject*)objid )
		return S_OK;

    metaobjidpair_type idpair;
	idpair.metaid = m_metaObjectId;
	idpair.objid  = objid;

	COMTRY
	{
        m_openedObject = objectFromObjId(idpair);
	}
	COMCATCH(;)
}

STDMETHODIMP CCoreXmlFile::CreateObject(objid_type *objid)
{
    CHECK_OUT(objid);

	if( m_metaObject == NULL || !m_inTransaction )
		COMRETURN(E_INVALID_USAGE);

    ASSERT( m_metaObjectId != METAID_ROOT );

	COMTRY
	{				
        // create and add new object
        XmlObject * obj = new XmlObject(m_metaObject);
        obj->m_metaid   = m_metaObjectId;
        addObject( obj );

        m_modified = true;
        m_openedObject = obj;
		*objid = (long)obj;
	}
	COMCATCH(;)
}

STDMETHODIMP CCoreXmlFile::CloseObject()
{
    m_openedObject = NULL;
	return S_OK;
}

STDMETHODIMP CCoreXmlFile::LockObject()
{
	return S_OK;
}

STDMETHODIMP CCoreXmlFile::DeleteObject()
{
	return S_OK;
}

STDMETHODIMP CCoreXmlFile::OpenProject(BSTR connection, VARIANT_BOOL *ro_mode) 
{
    if( m_opened || m_metaProject == NULL )
        COMRETURN(E_INVALID_USAGE);

    setFolderPathOnOpen( connection );

    readAll();

    m_opened   = true;
    m_modified = false;

    //readBinaryCache();


    //UnresolvedPointerVec pointers;
    //readXMLFile("c:\\temp\\gmetest\\xml3\\test1\\d6b6c42a6d4ea8409ef6cd583eefa54c.xml", pointers );



    return S_OK;
}

STDMETHODIMP CCoreXmlFile::CreateProject(BSTR connection)
{
    if( m_opened || m_metaProject == NULL )
		COMRETURN(E_INVALID_USAGE);
    
    // create project folder
    setFolderPathOnCreate(connection);
    if( CreateDirectory(m_folderPath.c_str(),NULL) == 0 )
        COMRETURN(E_FILEOPEN);

    // clear data structures
    clearAll();

    // query the metaobject for the root
    CComObjPtr<ICoreMetaObject> mo;
	COMTHROW( m_metaProject->get_Object(METAID_ROOT, PutOut(mo)) );
	ASSERT( mo != NULL );

    // create the root
    m_root = new XmlObject(mo);
    m_root->m_metaid = METAID_ROOT;
    addObject( m_root );
    
    m_opened   = true;
    m_modified = true;
    
    return S_OK;
}

STDMETHODIMP CCoreXmlFile::SaveProject(BSTR connection, VARIANT_BOOL keepoldname = VARIANT_TRUE) 
{
    return S_OK;	
}

STDMETHODIMP CCoreXmlFile::CloseProject( VARIANT_BOOL abort)
{
    if( !m_opened || m_metaProject == NULL )
		COMRETURN(E_INVALID_USAGE);

	COMTRY
	{
		/*if( abort == VARIANT_FALSE && modified && !(filename == ".")) 
			SaveProject(NULL);*/

		//CancelProject();

        //writeBinaryCache();
        //writeAll();
        clearAll();

	}
	COMCATCH(;)

	return S_OK;
}

STDMETHODIMP CCoreXmlFile::DeleteProject()
{ 
    return E_NOTIMPL;
}

STDMETHODIMP CCoreXmlFile::BeginTransaction()
{	
    if( !m_opened || m_inTransaction )
		COMRETURN(E_INVALID_USAGE);
	m_inTransaction = true;
	return S_OK;
}

STDMETHODIMP CCoreXmlFile::CommitTransaction()
{
    if( !m_inTransaction )
		COMRETURN(E_INVALID_USAGE);

	ASSERT( m_opened );

	CloseObject();	
	m_inTransaction = false;


	return S_OK;
}

STDMETHODIMP CCoreXmlFile::AbortTransaction()
{
	if( !m_inTransaction )
		COMRETURN(E_INVALID_USAGE);

	ASSERT( m_opened );

	CloseObject();	
	m_inTransaction = false;
	return S_OK;
}

STDMETHODIMP CCoreXmlFile::get_StorageType(long *p)
{
    CHECK_OUT(p);
	*p = 0;
	return S_OK;
}

void CCoreXmlFile::fillParentMap()
{
    m_parentMap.clear();

    m_parentMap.insert( ParentMap::value_type( DTID_CONSTRAINT , ATTRID_CONSTROWNER ) );
    m_parentMap.insert( ParentMap::value_type( DTID_REGNODE    , ATTRID_REGNOWNER   ) );
    m_parentMap.insert( ParentMap::value_type( DTID_FOLDER     , ATTRID_PARENT      ) );
    m_parentMap.insert( ParentMap::value_type( DTID_MODEL      , ATTRID_PARENT      ) );
    m_parentMap.insert( ParentMap::value_type( DTID_ATOM       , ATTRID_PARENT      ) );
    m_parentMap.insert( ParentMap::value_type( DTID_REFERENCE  , ATTRID_PARENT      ) );
    m_parentMap.insert( ParentMap::value_type( DTID_SET        , ATTRID_PARENT      ) );
    m_parentMap.insert( ParentMap::value_type( DTID_SETNODE    , ATTRID_SETMEMBER   ) );
    m_parentMap.insert( ParentMap::value_type( DTID_CONNECTION , ATTRID_PARENT      ) );
    m_parentMap.insert( ParentMap::value_type( DTID_CONNROLE   , ATTRID_CONNROLE    ) );
    m_parentMap.insert( ParentMap::value_type( DTID_CONNROLESEG, ATTRID_CONNSEG     ) );
    m_parentMap.insert( ParentMap::value_type( DTID_STRATTR    , ATTRID_ATTRPARENT  ) );
    m_parentMap.insert( ParentMap::value_type( DTID_INTATTR    , ATTRID_ATTRPARENT  ) );
    m_parentMap.insert( ParentMap::value_type( DTID_FLOATATTR  , ATTRID_ATTRPARENT  ) );
    m_parentMap.insert( ParentMap::value_type( DTID_BOOLATTR   , ATTRID_ATTRPARENT  ) );
    m_parentMap.insert( ParentMap::value_type( DTID_REFATTR    , ATTRID_ATTRPARENT  ) );
}

void CCoreXmlFile::closeMetaProject()
{
    closeMetaObject();
    CloseProject();
	m_metaProject = NULL;
}

void CCoreXmlFile::openMetaObject()
{
	ASSERT( m_metaObject != NULL );
	COMTHROW( m_metaObject->get_MetaID(&m_metaObjectId) );
}

void CCoreXmlFile::closeMetaObject()
{
	CloseObject();
	closeMetaAttribute();
	m_metaObject   = NULL;
	m_metaObjectId = METAID_NONE;
}

void CCoreXmlFile::openMetaAttribute()
{
	ASSERT( m_metaAttribute != NULL );
	COMTHROW( m_metaAttribute->get_AttrID(&m_metaAttributeId) );
    COMTHROW( m_metaAttribute->get_ValueType(&m_metaAttributeValType) );

	if( m_metaAttributeId == ATTRID_NONE )
		HR_THROW(E_METAPROJECT);
}

void CCoreXmlFile::closeMetaAttribute()
{
	m_metaAttribute        = NULL;
	m_metaAttributeId      = ATTRID_NONE;
    m_metaAttributeValType = VALTYPE_NONE;
}

void CCoreXmlFile::setFolderPathOnCreate(BSTR connection)
{
    string conn;

    CopyTo(connection, conn);

    if( !(string(conn, 0, 4) == "MGX=") )
	    HR_THROW(E_INVALID_USAGE);

    conn.erase(0, 4);
    m_folderPath = conn;

    char drive[_MAX_DRIVE];
    char dir[_MAX_DIR];
    char fname[_MAX_FNAME];
    char ext[_MAX_EXT];

    _splitpath( conn.c_str(), drive, dir, fname, ext );

    m_cacheFileName = m_folderPath;
    m_cacheFileName += "\\";
    m_cacheFileName += fname;
    m_cacheFileName += ".mgx";
}

void CCoreXmlFile::setFolderPathOnOpen(BSTR connection)
{
    string conn;

    CopyTo(connection, conn);

    if( !(string(conn, 0, 4) == "MGX=") )
	    HR_THROW(E_INVALID_USAGE);

    conn.erase(0, 4);
    
    char drive[_MAX_DRIVE];
    char dir[_MAX_DIR];
    char fname[_MAX_FNAME];
    char ext[_MAX_EXT];

    _splitpath( conn.c_str(), drive, dir, fname, ext );

    m_folderPath = drive;
    m_folderPath += "\\";
    m_folderPath += dir;
    m_folderPath.resize(m_folderPath.size()-1); // remove the ending "\"

    m_cacheFileName = m_folderPath;
    m_cacheFileName += "\\";
    m_cacheFileName += fname;
    m_cacheFileName += ".mgx";
}











void CCoreXmlFile::clearAll()
{
    for( XmlObjVecIter i=m_objects.begin(); i!=m_objects.end(); ++i )       
        delete (*i);
    m_objects.clear();
    m_openedObject = NULL;
    m_root = NULL;
}

void CCoreXmlFile::addObject(XmlObject * obj)
{
    obj->m_index = m_objects.size();
    m_objects.push_back( obj );
    m_objectsByGUID.insert( GUIDToXmlObjectMap::value_type( obj->m_guid, obj ) );
}
    
void CCoreXmlFile::removeObject(XmlObject * obj)
{
    // remove pointers
    for(AttribMapIter it=obj->m_attributes.begin(); it!=obj->m_attributes.end(); ++it)
    {
        if( it->second->getType() == VALTYPE_POINTER )
        {
            // remove it from parent object's collection
            XmlAttrPointer * pointer = (XmlAttrPointer*)it->second;
            if( pointer->m_parent != NULL )
            {
                AttribMapIter it2 = pointer->m_parent->m_attributes.find( it->first + ATTRID_COLLECTION );
                ASSERT( it2 != pointer->m_parent->m_attributes.end() );
                ASSERT( it2->second->getType() == VALTYPE_COLLECTION );
                ((XmlAttrCollection*)it2->second)->m_children.erase(obj);
            }
        }
        else if( it->second->getType() == VALTYPE_COLLECTION )
        {
            XmlAttrCollection * collection = (XmlAttrCollection*)it->second;
            for( XmlObjSetIter it2=collection->m_children.begin(); it2!=collection->m_children.end(); ++it2 )
            {
                // set parent of this child to NULL
                XmlObject * obj2 = (*it2);
                AttribMapIter it3 = obj2->m_attributes.find( it->first - ATTRID_COLLECTION );
                ASSERT( it3 != obj2->m_attributes.end() );
                ASSERT( it3->second->getType() == VALTYPE_POINTER );
                ((XmlAttrPointer *)(it3->second))->m_parent = NULL;
            }
        }
    }

    obj->m_deleted = true;
}

void CCoreXmlFile::setPointer(XmlObject * obj, attrid_type attribId, XmlObject * parent)
{
    ASSERT( obj!=NULL );

    AttribMapIter it = obj->m_attributes.find(attribId);
    ASSERT( it!=obj->m_attributes.end() );
    
    XmlAttrPointer * attr = (XmlAttrPointer*)it->second;
    
    // remove item from old parent's list
    if( attr->m_parent != NULL )
    {
        // find collection attribute of parent
        AttribMapIter it2 = attr->m_parent->m_attributes.find( attribId + ATTRID_COLLECTION );
        ASSERT( it2 != attr->m_parent->m_attributes.end() );
        ASSERT( it2->second->getType() == VALTYPE_COLLECTION );
        
        // remove this form the list
        ((XmlAttrCollection *)it2->second)->m_children.erase(obj);
    }

    // set pointer attribute
    attr->m_parent = parent;

    // add item to new parent's list
    if( parent != NULL )
    {
        AttribMapIter it3 = parent->m_attributes.find( attribId + ATTRID_COLLECTION );
        ASSERT( it3 != parent->m_attributes.end() );
        ASSERT( it3->second->getType() == VALTYPE_COLLECTION );
        ((XmlAttrCollection *)it3->second)->m_children.insert(obj);
    }
}

void CCoreXmlFile::setPointer(XmlAttrPointer * attr, VARIANT p)
{   
    metaobjidpair_type idpair;
    CopyTo(p, idpair);
    XmlObject * parent = objectFromObjId(idpair);
    setPointer( m_openedObject, m_metaAttributeId, parent );
}
    
void CCoreXmlFile::updateCollections()
{
    for( XmlObjVecIter it=m_objects.begin(); it!=m_objects.end(); ++it )
    {   
        XmlObject * obj = (*it);
        for( AttribMapIter j=obj->m_attributes.begin(); j!=obj->m_attributes.end(); ++j)
        {
            if( j->second->getType() == VALTYPE_POINTER )
            {
                XmlAttrPointer * pointer = (XmlAttrPointer*)j->second;
                if( pointer->m_parent != NULL )
                {
                    AttribMapIter k = pointer->m_parent->m_attributes.find( j->first + ATTRID_COLLECTION );
                    ASSERT( k != pointer->m_parent->m_attributes.end() );
                    ((XmlAttrCollection*)k->second)->m_children.insert( obj );
                }
            }
        }
    }
}

void CCoreXmlFile::resolvePointers(UnresolvedPointerVec& pointers)
{
    for( UnresolvedPointerVecIt it=pointers.begin(); it!=pointers.end(); ++it )
    {
        if( it->m_pointedObjGuid == GUID_NULL )
        {
            setPointer( it->m_object, it->m_attrib, NULL );
        }
        else
        {
            GUIDToXmlObjectMapIter it2 = m_objectsByGUID.find( it->m_pointedObjGuid );
            if( it2 != m_objectsByGUID.end() )
            {
                setPointer( it->m_object, it->m_attrib, it2->second );
            }
            else
            {
                // TODO: invalid pointer, what to do?
            }
        }
    }
}

void CCoreXmlFile::getPointer(XmlAttrPointer * attr, VARIANT * p)
{
    ASSERT( attr!= NULL );
    metaobjidpair_type id;
    objIdFromObject( attr->m_parent, id );
    CopyTo(id, p);
}

void CCoreXmlFile::getCollection(XmlAttrCollection * attr, VARIANT * p)
{
    ASSERT( attr!= NULL );
    vector<metaobjidpair_type> idpairs;
    for( XmlObjSetIter it = attr->m_children.begin(); it != attr->m_children.end(); ++it )
    {
        metaobjidpair_type id;
        objIdFromObject( *it, id );
        idpairs.push_back( id );
    }
    CopyTo(idpairs, p);
}

XmlObject * CCoreXmlFile::objectFromObjId(metaobjidpair_type idpair)
{
    if( idpair.metaid == METAID_NONE && idpair.objid == OBJID_NONE )
        return NULL;

    if( idpair.metaid == METAID_ROOT )
        return m_root;
    else
        return (XmlObject*)idpair.objid;
}

void CCoreXmlFile::objIdFromObject(XmlObject * obj, metaobjidpair_type& idpair)
{
    if( obj == NULL )
    {
        idpair.metaid = METAID_NONE;
        idpair.objid  = OBJID_NONE;
    }
    else
    {
        idpair.metaid = obj->m_metaid;
        if( idpair.metaid == METAID_ROOT )
            idpair.objid = 1;
        else
            idpair.objid = (long)obj;
    }
}

void CCoreXmlFile::writeBinaryCache()
{
    FILE * f = fopen( m_cacheFileName.c_str(), "wb" );
    if( f==NULL )
        HR_THROW(E_FILEOPEN);

    XmlObjVecIter i;

    // write out GUIDs and metaids
    int n = m_objects.size();
    fwrite( &n, sizeof(n), 1, f );
    for( i = m_objects.begin(); i != m_objects.end(); ++i )
    {
        fwrite( &((*i)->m_metaid), sizeof(long), 1, f );
        fwrite( &((*i)->m_guid), sizeof(GUID), 1, f );
    }

    // write out pointers
    int x;
    for( i=m_objects.begin(); i!=m_objects.end(); ++i )
    {   
        XmlObject * obj = (*i);
        for( AttribMapIter j=obj->m_attributes.begin(); j!=obj->m_attributes.end(); ++j)
        {
            if( j->second->getType() == VALTYPE_POINTER )
            {
                XmlAttrPointer * pointer = (XmlAttrPointer*)j->second;
                if( pointer->m_parent == NULL )
                    x = 0;
                else
                    x = pointer->m_parent->m_index;
                fwrite( &x, sizeof(x), 1, f );
            }
        }
    }

    fclose(f);
}

void CCoreXmlFile::readBinaryCache()
{    
    FILE * f = fopen( m_cacheFileName.c_str(), "rb" );
    if( f==NULL )
        HR_THROW(E_FILEOPEN);

    clearAll();

    // read GUIDs and metaids and create objects
    int n,i;
    fread( &n, sizeof(n), 1, f );
    for( i=0; i<n; ++i )
    {
        long metaid;
        GUID guid;

        fread( &metaid, sizeof(metaid), 1, f );
        fread( &guid, sizeof(GUID), 1, f );

        CComObjPtr<ICoreMetaObject> metaobject;
        COMTHROW( m_metaProject->get_Object(metaid, PutOut(metaobject)) );

        XmlObject * obj = new XmlObject(metaobject);
        obj->m_metaid = metaid;
        obj->m_guid   = guid;
        addObject(obj);
    }

    // read and set pointers
    int x;
    for( XmlObjVecIter it=m_objects.begin(); it!=m_objects.end(); ++it )
    {   
        XmlObject * obj = (*it);
        for( AttribMapIter j=obj->m_attributes.begin(); j!=obj->m_attributes.end(); ++j)
        {
            if( j->second->getType() == VALTYPE_POINTER )
            {
                XmlAttrPointer * pointer = (XmlAttrPointer*)j->second;

                fread( &x, sizeof(x), 1, f );
                if( x == 0 )
                    pointer->m_parent = NULL;
                else
                    pointer->m_parent = m_objects[x];
            }
        }
    }

    fclose(f);

    updateCollections();
}

void CCoreXmlFile::writeAll()
{
    for( XmlObjVecIter it=m_objects.begin(); it!=m_objects.end(); ++it )
    {
        XmlObject * obj = (*it);

        if( obj->m_metaid == DTID_MODEL || obj->m_metaid == DTID_FOLDER 
            || obj->m_metaid == METAID_ROOT )
        {
            writeXMLFile( obj );
        }
    }
}

void CCoreXmlFile::writeXMLFile(XmlObject * container)
{
    // container must be model or folder or the root
    if( container->m_metaid != DTID_MODEL && container->m_metaid != DTID_FOLDER 
        && container->m_metaid != METAID_ROOT )
        HR_THROW(E_INVALID_USAGE);

    // set file name
    char fileName[MAX_PATH];
    string guidStr;
    guid2str(container->m_guid,guidStr);
    sprintf( fileName, "%s\\%s.xml", m_folderPath.c_str(), guidStr.c_str() );

    // open file
    FILE * file = fopen( fileName, "w" );
    if( file == NULL )
        HR_THROW(E_FILEOPEN);

    // write objects recursively
    writeObject( container, file, true, "" );
    
    fclose(file);
}

void CCoreXmlFile::writeObject(XmlObject * obj, FILE * file, bool container, const char * prefix)
{        
    string                          str;
    CComObjPtr<ICoreMetaObject>     metaobject;
    CComBSTR                        metaToken;    

    COMTHROW( m_metaProject->get_Object( obj->m_metaid, PutOut(metaobject) ) );
    COMTHROW( metaobject->get_Token( &metaToken ) );
    guid2str( obj->m_guid, str );

    fprintf(file, "%s<%S MetaId=\"%d\" Id=\"%s\"", prefix, metaToken, obj->m_metaid, str.c_str() );

    // write pointers, attributes
    AttribMapIter it;
    for( it=obj->m_attributes.begin(); it!=obj->m_attributes.end(); ++it )
    {
        XmlAttrBase                    * attr = it->second;
        CComObjPtr<ICoreMetaAttribute>   metaAttrib;
        CComBSTR                         attribToken;
        string                           attrVal;

        COMTHROW( metaobject->get_Attribute( it->first, PutOut(metaAttrib) ) );
        metaAttrib->get_Token( &attribToken );

        if( attr->getType() == VALTYPE_POINTER  )
        {
            ParentMap::iterator it2 = m_parentMap.find( obj->m_metaid );
            if( it2==m_parentMap.end() || it2->second!=it->first || container )
            {
                XmlAttrPointer * pointer = (XmlAttrPointer*)attr;
                if( pointer->m_parent != NULL )
                    guid2str( pointer->m_parent->m_guid, attrVal );
            }
        }
        else if( attr->getType() != VALTYPE_COLLECTION && attr->getType() != VALTYPE_LOCK )
        {
            XmlAttrBase * attr = it->second;
            attr->toString(attrVal);
        }

        if( !attrVal.empty() )
            fprintf(file, " %S=\"%s\"", attribToken, attrVal.c_str() );
    }
    fprintf(file, ">\n" );

    // write out children
    // child is written if it is not the root, a model or a folder
    // and the parent of the child is a us according to m_parentMap
    string newPrefix = prefix;
    newPrefix += "\t";
    for( it=obj->m_attributes.begin(); it!=obj->m_attributes.end(); ++it )
    {
        XmlAttrBase * attr = it->second;
        if( attr->getType() == VALTYPE_COLLECTION )
        {
            XmlAttrCollection * coll = (XmlAttrCollection*)attr;
            for( XmlObjSetIter it2=coll->m_children.begin(); it2!=coll->m_children.end(); ++it2 )
            {
                XmlObject * obj2 = (*it2);
                if( obj2!=NULL && obj2->m_metaid!=DTID_MODEL && obj2->m_metaid!=DTID_FOLDER                  
                    && obj2->m_metaid != METAID_ROOT )
                {                    
                    ParentMap::iterator it3 = m_parentMap.find( obj2->m_metaid );
                    ASSERT( it3 != m_parentMap.end() );
                    if( it3->second + ATTRID_COLLECTION == it->first )
                        writeObject( obj2, file, false, newPrefix.c_str() );
                }
            }
        }
    }
    
    fprintf(file, "%s</%S>\n",prefix, metaToken);
}

void CCoreXmlFile::readXMLFile( const char * fileName, UnresolvedPointerVec& pointers )
{
    DOMBuilder * parser = NULL;

    try
    {    
        DOMImplementationLS * domimpl = DOMImplementationRegistry::getDOMImplementation(NULL);
        ASSERT( domimpl != NULL );

        parser = domimpl->createDOMBuilder( DOMImplementationLS::MODE_SYNCHRONOUS, NULL );
        ASSERT( parser != NULL );

        DOMDocument * doc = parser->parseURI( fileName );
    
        readObject( doc->getDocumentElement(), pointers, NULL );

        delete parser;
    }
    catch(...)
    {
        if( parser != NULL )
            delete parser;

        HR_THROW(E_FILEOPEN);
    }
}

void CCoreXmlFile::readObject(DOMElement * e, UnresolvedPointerVec& pointers, XmlObject * parent)
{    
    // get metaid, and id
    char * metaidStr  = XMLString::transcode(e->getAttribute(XMLString::transcode("MetaId")));
    char * objGUIDStr = XMLString::transcode(e->getAttribute(XMLString::transcode("Id" )));
    long   metaid     = atoi( metaidStr );
    GUID   guid       = str2guid( objGUIDStr );
    delete metaidStr;
    delete objGUIDStr;

    // get meta object
    CComObjPtr<ICoreMetaObject> metaobject;
    COMTHROW( m_metaProject->get_Object( metaid, PutOut(metaobject) ) );

    // find or create object
    XmlObject * obj = NULL;
    GUIDToXmlObjectMapIter it = m_objectsByGUID.find( guid );
    if( it != m_objectsByGUID.end() )
    {
        obj = it->second;
        if( !obj->m_loaded )
        {
            obj->createAttributes(metaobject,true);
            obj->m_loaded = true;
        }
    }
    else
    {
        obj = new XmlObject(metaobject,true);
        obj->m_metaid = metaid;
        obj->m_guid   = guid;
        addObject( obj );
        if( metaid == METAID_ROOT )
            m_root = obj;
    }
    
    // read attributes
    AttribMapIter it2;
    for( it2 = obj->m_attributes.begin(); it2 != obj->m_attributes.end(); ++it2 )
    {        
        CComObjPtr<ICoreMetaAttribute>  metaAttrib;
        CComBSTR                        attribToken;

        COMTHROW( metaobject->get_Attribute( it2->first, PutOut(metaAttrib) ) );
        COMTHROW( metaAttrib->get_Token( &attribToken ));        

        char * attrVal = XMLString::transcode(e->getAttribute(attribToken));

        XmlAttrBase * attr = it2->second;
        if( attr->getType() == VALTYPE_POINTER )
        {
            UnresolvedPointer p;
            p.m_object = obj;
            p.m_attrib = it2->first;          
            if( attrVal==NULL || strlen(attrVal)==0 )
                p.m_pointedObjGuid = GUID_NULL;
            else
                p.m_pointedObjGuid = str2guid( attrVal );
            pointers.push_back( p );
        }
        else if( attr->getType() != VALTYPE_LOCK && attr->getType() != VALTYPE_COLLECTION )
        {
            it2->second->fromString(attrVal);
        }

        delete attrVal;
    }

    // implicit parent pointer
    if( parent != NULL )
    {
        ParentMap::iterator it3 = m_parentMap.find( obj->m_metaid );
        UnresolvedPointer p;
        p.m_object = obj;
        p.m_attrib = it3->second;
        p.m_pointedObjGuid = parent->m_guid;
        pointers.push_back(p);
    }        
  
    // read children
    DOMNodeList * children = e->getChildNodes();
    for( int i=0; i<children->getLength(); ++i )
    {
        DOMNode * node = children->item(i);
        if( node->getNodeType() == DOMNode::ELEMENT_NODE )        
            readObject( (DOMElement*)node, pointers, obj );
    }
}

void CCoreXmlFile::readAll()
{
    UnresolvedPointerVec pointers;
    char                 buf[_MAX_PATH];
    _finddata_t          fileInfo;

    clearAll();
    
    sprintf( buf, "%s\\*.xml", m_folderPath.c_str() );
    long searchHandle = _findfirst( buf, &fileInfo );
    long ret = searchHandle;
    while( ret != -1 )
    {
        sprintf( buf, "%s\\%s", m_folderPath.c_str(), fileInfo.name );
        readXMLFile( buf, pointers );
        ret = _findnext( searchHandle, &fileInfo );
    }
    _findclose( searchHandle );

    resolvePointers( pointers );
}

void CCoreXmlFile::incrementalUpdate()
{
    // vegig kell menni a fileokon es megnezni az uj es a modosult fileokat

    // minden file-ra meghivjuk a readXMLFile ami beolvassa az objektumokat a filebol, 
    // torli a mar nem letezo objektumokat beolvassa az atributumokat
    // (lehet hogy csak a pointereket kene beolvasni!)
    // a feloldatlan pointereket egy listaba gyujtjuk
    // objektum torlesnel figyelni kell hogy a pointereket is update-eljuk

    // a felodatlan pointereket a vegen feloldjuk
    

}

