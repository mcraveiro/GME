#ifndef MGA_COREXMLFILE_H
#define MGA_COREXMLFILE_H

#include "StdAfx.h"
#include "resource.h"
#include <fstream>//fstream.h
#include <list>
#include <map>
#include <hash_map>
#include <set>
#include <xercesc/dom/dom.hpp>
#include "..\Mga\MgaGeneric.h"
#include "CoreDictionaryAttributeValue.h"
#include "XmlBackEnd.h" // for EXTERN_C const CLSID CLSID_CoreXmlFile;
#include "OperOptions.h"
#include "MsgConsole.h"
#include "Transcoder.h"
#include "HiClient.h"

const metaobjidpair_type ROOT_ID_PAIR = {METAID_ROOT, OBJID_ROOT};

enum OpCode
{
	  ELEM_DELETED
	, ELEM_MOVED
	, ELEM_DERIVED
	, ELEM_REFERRED
	, ELEM_CONNECTED
	, ELEM_TAKESPARTINCONN
	, ELEM_REF_REDIRECTED
	, ELEM_TOTAL
};

class ParserLiterals
{
public:
	class Signer
	{
	public:
		static const XMLCh * users;
		static const XMLCh * user;
		static const XMLCh * name;
		static const XMLCh * since;
		static const XMLCh * until;
	};
	class Protector
	{
	public:
		static const XMLCh * item;
		static const XMLCh * when;
		static const XMLCh * oper;
		static const XMLCh * gd;
		static const XMLCh * objects;
	};
	class Main
	{
	public:
		static const XMLCh * deleted;
		static const XMLCh * metaId;
		static const XMLCh * id;
		static const XMLCh * parent;
	};

	static const XMLCh * newln;
	static const XMLCh * empty;
};

class HelperFiles
{
public:
	static const char * sessionFolderName;
	static const char * signFileName;
	static const char * protFileName;
	static const char * protFileExt;
};

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

struct XmlObjComp
{
	bool operator()( XmlObject * p1, XmlObject * p2) const;
};



typedef std::set<XmlObject*, XmlObjComp>     XmlObjSet; // ordered set, ensures xml files to look similar
typedef XmlObjSet::iterator                  XmlObjSetIter;
typedef std::vector<XmlObject*>              XmlObjVec;
typedef XmlObjVec::iterator                  XmlObjVecIter;
typedef std::map<metaid_type,attrid_type>    ParentMap;
typedef std::map<GUID, XmlObject*,GUID_less> GUIDToXmlObjectMap;
typedef GUIDToXmlObjectMap::iterator         GUIDToXmlObjectMapIter;


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
    virtual void         fromString  (const char * str, const wchar_t* strw)  {}
    virtual void         toString    (std::string& str) const {}
};

typedef std::map<attrid_type,XmlAttrBase*>   AttribMap;
typedef AttribMap::iterator                  AttribMapIter;

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
    virtual void         fromString  (const char * str, const wchar_t* strw);
    virtual void         toString    (std::string& str) const;
protected:
    long    m_value;

	friend class CCoreXmlFile;
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
    virtual void         fromString  (const char * str, const wchar_t* strw);
    virtual void         toString    (std::string& str) const;
protected:
    double  m_value;
};

////////////////////////////////////////////////////////////////////////////////
// XmlAttrDict class
////////////////////////////////////////////////////////////////////////////////

class XmlAttrDict: public XmlAttrBase
{
public:
	XmlAttrDict();

	virtual valtype_type getType() const { return VALTYPE_DICT; };
    virtual void fromVariant(VARIANT p);
	virtual void toVariant(VARIANT *p) const;
    virtual void fromString(const char * str, const wchar_t* strw);
    virtual void toString(std::string& str) const;
    CComPtr<ICoreDictionaryAttributeValue> m_value;
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
    virtual void         fromString  (const char * str, const wchar_t* strw);
    virtual void         toString    (std::string& str) const;
protected:
    std::string  m_value;
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
    virtual void         fromString  (const char * str, const wchar_t* strw);
    virtual void         toString    (std::string& str) const;
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
    virtual void         fromString  (const char * str, const wchar_t* strw);
    virtual void         toString    (std::string& str) const;
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
    enum AttrSet
    {
        ATTR_PRIMARY,
        ATTR_SECONDARY,
        ATTR_ALL        
    };

public:
                XmlObject           (ICoreMetaObject *metaobject, bool createAllAttributes );
               ~XmlObject           ();

    bool        isContainer         ();    
        
protected:
    void        createAttributes        (ICoreMetaObject *metaobject, int attrSet );
    void        deleteSecondaryAttribs  ();

protected:
    metaid_type        m_metaid;
    GUID               m_guid;
    int                m_index;            // index in the m_objects array
    AttribMap          m_attributes;
    bool               m_loaded;           // true if all attributes are loded (otherwise only pointer, collections and locks stored)
    bool               m_deleted;
    bool               m_modified;
    CTime              m_lastWriteTime;

    friend class CCoreXmlFile;
    friend struct XmlObjComp;
};

struct UnresolvedPointer
{
    XmlObject   * m_object;             // whose attribute is this
    attrid_type   m_attrib;
    GUID          m_pointedObjGuid;
};

typedef std::vector<UnresolvedPointer>       UnresolvedPointerVec;
typedef UnresolvedPointerVec::iterator  UnresolvedPointerVecIt;

struct UndoItem
{
    UndoItem( XmlObject * object, attrid_type attrId, CComVariant value )
    {
        m_object = object;
        m_value  = value;
        m_attrId = attrId;
    };

    attrid_type   m_attrId;
    XmlObject *   m_object;
    CComVariant   m_value;
};

typedef std::map<XmlAttrBase*, UndoItem>  UndoMap;
typedef UndoMap::iterator            UndoMapIter;


class  SignFileEntry
{
public:
	SignFileEntry( const std::string& p_username, const CTime& p_time) : m_username( p_username), m_time( p_time) { }
	std::string      m_username;
	CTime            m_time;
};

class LoggedIn
{
public:
	LoggedIn( const std::string& p_nm, char flag) : m_nm( p_nm), m_fl( flag) { }
	std::string      m_nm;
	char             m_fl;
	bool             operator==( const LoggedIn& peer) const { return m_nm == peer.m_nm; }
};

class SignFileData
{
public:
	SignFileData( const std::string& pn, const std::string& ps, const std::string& pu) : m_user( pn), m_since( ps), m_until( pu) { }
	std::string              m_user;
	std::string              m_since;
	std::string              m_until;
	bool         operator==( const SignFileData& peer) const { return m_user == peer.m_user; }
};


class ProtectEntry
{
public:
	ProtectEntry( GUID p_gd, OpCode p_op, CTime p_time);
	GUID              m_guid;
	OpCode            m_op;
	CTime             m_time;
};

class PublicStorage
{
protected:
	std::string                       m_fileName;
	std::string                       m_localFileName;
	std::vector< ProtectEntry >       m_list;
	CCoreXmlFile*                     m_parent;

	const char *                      m_ccsItem;
private:
	void acquireSVN              ( const char * obj);
	void releaseSVN              ( const char * obj);

protected:
	void                   acquireFile ();
	void                   releaseFile ();
	inline std::string     userName    ();
	inline bool            isSV        ();

public:
	PublicStorage               ();
	void setParent              ( CCoreXmlFile* p_parent);
	void init                   ( const std::string& p_initialContent);
};

class SignManager : public PublicStorage
{
	void in_or_off              ( bool in);
	bool anybodyElseHolding     ();

	void update                 ( bool p_in, const SignFileEntry& p_entry);

public:
	typedef std::vector< SignFileData> SignFileDataVec;

	void setParent              ( CCoreXmlFile* p_parent); // hides parent's setParent
	void in                     () { in_or_off( true); }
	void off                    () { in_or_off( false); }
	SignFileDataVec getUserData ();

};


class ProtectList : public PublicStorage
{
	std::vector< ProtectEntry >       m_list;

	void writeProtList        ();
	void writeProtLisp        ();
	void purgeProtList        ( CTime& p_lastSyncTime);
	void clearProtList        ();
	bool needed               ();

public:
	void setParent            ( CCoreXmlFile* p_parent); // hides parent's setParent
	void addEntry             ( const ProtectEntry& p_pe);

	static std::string getProtListFileName( const std::string& p_username);

	void onCommited           ();
	void onAborted            ();
	void onLoad               ();
};

class ProtFileEntry
{
public:
	ProtFileEntry( const std::string& p_guid, OpCode p_op, CTime& p_time) : m_guid( p_guid), m_op( p_op), m_time( p_time) { }
	std::string              m_guid;
	OpCode                   m_op;
	CTime                    m_time;
};

class ATL_NO_VTABLE CCoreXmlFile : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<ICoreStorage, &IID_ICoreStorage, &LIBID_XMLBACKENDLib>,
	public ISupportErrorInfoImpl<&IID_ICoreStorage>,
	public CComCoClass<CCoreXmlFile, &CLSID_CoreXmlFile>
{
public:
	enum SourceControl
	{
		SC_NONE,
		SC_SUBVERSION
	};

	enum CheckOutSate
	{
		CS_NOT_CHECKEDOUT,
		CS_CURRENT_USER,
		CS_OTHER_USER
	};

	enum FileStatusBitList
	{
		//  FS_UTD = 0x0
		FS_LOCAL = 0x1
		, FS_OTHER = 0x2
		, FS_MODIFIEDBYOTHERS = 0x100
		, FS_NOTYETSAVED      = 0x200

	};

public:
	CCoreXmlFile();
	~CCoreXmlFile();

	BEGIN_COM_MAP(CCoreXmlFile)
		COM_INTERFACE_ENTRY(ICoreStorage)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY(ISupportErrorInfo)
	END_COM_MAP()

	DECLARE_REGISTRY_RESOURCEID(IDR_CoreXmlFile)

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

	void         closeMetaProject       () /*throw()*/;

	void         openMetaObject         ();

	void         closeMetaObject        () /*throw()*/;

	void         openMetaAttribute      ();

	void         closeMetaAttribute     () /*throw()*/;

	void         parseConnectionString  ( BSTR connection );

	void         svnSshHandling         ();
	std::string  svnSshMangling         ( const std::string& p_url);
	std::string  userNameFromSvnSshUrl  ();
	bool         isUrlSvnSsh            ();

	void         svnOptions             ();

	void         setFileNames           ( bool reset = false);

	void         getContainerFileName   (XmlObject * obj, std::string& str, bool fullpath=true);

	void         getContainerName       (XmlObject * obj, std::string& name, std::string& type);

	// graph operatrions
	void         clearAll               ();

	void         addObject              (XmlObject * obj);

	void         deleteObject           (XmlObject * obj);

	void         setPointer             (XmlObject * obj, attrid_type attribId, XmlObject * pointed);

	XmlObject *  setPointer             (VARIANT p);

	void         updateCollections      ();

	void         resolvePointers        (UnresolvedPointerVec& pointers);

	void         resetSourceControlForAll();
	void         resetSourceControlInfo (XmlObject * obj);
	void         resetSourceControlStat (XmlObject * obj, bool freshObj);

	void         resetSettings          ();

	// getting connections
	void         getPointer             (XmlAttrPointer * attr, VARIANT * p);

	void         getCollection          (XmlAttrCollection * attr, VARIANT * p);

	XmlObject *  getContainer           (XmlObject * obj);

	void         getContainedObjects    (XmlObject * obj, XmlObjVec& vec);

	// check out
	void         getCheckOutContainers  (XmlObjSet& objects, XmlObjSet& containers, bool thorough = false);

	void         getDependentContainers (XmlObject * obj, XmlObjSet& containers, XmlObjSet& processedObjects, bool thorough = false);

	void         getMyDepObj            ( XmlObject * obj, XmlObjSet& containers, XmlObjSet& processedObjects, bool thorough);

	void         getMyDepObjConts       ( XmlObjSet& objects, XmlObjSet& containers, bool thorough);

	void         getAllTheWayDown       ( XmlObject * obj, XmlObjSet& containers);

	void         getAllUpAndDown        ( XmlObjSet& objects, XmlObjSet& containers);

	void         getBasesOfObj          ( XmlObject * obj, XmlObjSet& containers);

	void         getBasesOf             ( XmlObjSet& objects, XmlObjSet& containers);

	void         getDeriveds            ( XmlObject * obj, XmlObjSet& containers);

	void         applySmallChange       ( XmlObjSet& p_conts);

	bool         checkOutFiles          (XmlObjSet& containers);

	// object pointer and id conversion
	XmlObject *  objectFromObjId        (metaobjidpair_type idpair);

	void         objIdFromObject        (XmlObject * obj, metaobjidpair_type& idpair);

	// serialization
	void         writeBinaryCache       ();

	bool         readBinaryCache        ();

	void         timestampOfCache       ( FILETIME* p_fTime);

	void         createProjectFile      ();

	void         readProjectFile        ();

	void         writeAll               ();

	void         writeXMLFile           (XmlObject * container);

	void         writeObject            (XmlObject * obj, Transcoder& file, bool container, const char * prefix, CTime lastWriteTime );

	void         applyLastWrTime        (XmlObject * obj, bool container, CTime lastWriteTime );

	void         timeSync               (const char * fileName, XmlObject * container);

	void         fullReadContainer      (XmlObject * container);

	void         readXMLFile            (const char * fileName, UnresolvedPointerVec& pointers, bool fullLoad );

	void         readObject             (XERCES_CPP_NAMESPACE::DOMElement * e, UnresolvedPointerVec& pointers, XmlObject * parent, bool fullLoad, CTime lastWriteTime );

	void         readAll                ( bool fullLoad );
	void         loadFrom               ( const std::string& p_dir, UnresolvedPointerVec& p_pointers, bool p_fullLoad);
	void         loadDirs               ( const std::string& p_dir, UnresolvedPointerVec& p_pointers, bool p_fullLoad);

	void         getLatestAndLoad       ();

	// source control
	bool         getUserCredentialInfo  ( int p_svnText, bool p_requireLogin);

	void         createProjectFolder    ();
	int          createHashedFolders    ();
	void         commitHashedFolders    ();
	void         socoAdd                ( const std::string& p_path, bool p_recursive);
	void         socoCommit             ( const std::string& p_path, const std::string& p_comment, bool p_initial);

	void         createNonversioned      ();

	void         getSourceSafePath      (XmlObject * obj, std::string& str);

	bool         isContainerReadOnly    (XmlObject * obj);

	bool         isContinerCheckedOut   (XmlObject * obj);

	void         checkOutContainer      (XmlObject * obj);
	void         rollBackTheCheckOutContainer( XmlObject * obj);

	void         addToSourceControl     (XmlObject * container, bool p_fileExisted);

	void         getLatestVersion       ();

	void         checkInAll             ();

	void         checkInAll             ( bool keepCheckedOut );

	void         showUsedFiles          ( XmlObjSet& containers, bool p_latentMessage = false );

	void         setSourceControlNodes  ( XmlObject * container, long lInfo, long lStat);

	void         updateSourceControlInfo( XmlObject * container );
	void         updateSourceControlInfo();

	void         whoControlsThis        ( XmlObject * container = 0);

	void         dumpSourceControlInfo  ();

	bool         filesModifiedByOthers  ();

	bool         filesModifiedByOthersV3( XmlObjSet& readOnlyFiles, XmlObjSet& latentFiles );

	bool         fileModifiedByOthers   ( XmlObject * obj );
	//bool         fileModifiedByOthers   ( const std::string& p_file, const CTime& p_myTime);

	bool         makeSureFileExistsInVerSys( const std::string& p_fname, const std::string& p_initialcontent, bool p_needsLock = true);

	// SVN section:
	void         getSVLastCommiter      ( XmlObject * obj, std::string& user);
	void         getSVCurrentOwner      ( XmlObject * obj, std::string& user, bool& newfile);
	bool         isCheckedOutByElseSVN  ( const std::string& p_file);
	//void         checkOutSVN            ( const std::string& p_file);
	bool         isVersionedInSVN       ( const std::string& p_file, bool p_isADir = false, bool p_suppressErrorMsg = false);
	bool         infoSVN                ( const std::string& p_url, bool p_recursive, std::string& p_resultMsg, std::string& p_author, std::string& p_owner);

	bool         lockablePropertySVN    ( const std::string& p_file);
	bool         applyLockSVN           ( const std::string& p_file); // throws hresult_exception if failed to lock
	bool         removeLockSVN          ( const std::string& p_file);
	bool         mkdirSVN               ( const std::string& p_url, const std::string& p_newDirName, const std::string& p_localPath);
	bool         addSVN                 ( const std::string& p_file, bool p_recursive = false);
    void         findAllRwObjs          ( const std::string& p_folderPath, std::vector< std::string>& p_rw_file_vec);
	bool         bulkCommitSVN          ( const std::string& p_dir, const std::string& p_comment, bool p_noUnlock  = false);
	bool         commitSVN              ( const std::string& p_dirOrFile, const std::string& p_comment, bool p_initialCommit = false, bool p_noUnlock = false);
	bool         updateSVN              ( const std::string& p_dirOrFile);
	void         svnSetup               ( bool createOrOpen);
	void         testSubversion         ();
	void         createSubversionClientImpl();
	void         createSubversionedFolder();

	std::string                 m_svnUrl;
	bool                        m_hashFileNames;
	int                         m_hashVal;
	bool                        m_hashInfoFound;
	bool                        m_svnShowCmdLineWindows;
	bool                        m_svnRedirectOutput;

	std::auto_ptr<HiClient> m_svn;

	bool		 m_needsSessionRefresh;
	void         protect                ( XmlObject * obj, OpCode oc);

	bool         findOnProtectedLists   ( GUID p_gd, std::string& p_scapegoatUser);
	void         refreshSessionFolder   ();

	/*
	std::string  refreshProtectionFile  ( const std::string& p_username);
	bool         refreshSignFile        ();
	bool         refreshOneFile         ( const std::string& p_filename);
	*/
	bool         findInFile             ( const std::string& p_filename, const std::string& p_gd);

	
	void         replaceUserName        ( const std::string& p_username);
	std::string  userName               ();
	inline bool  isSV                   ();

	bool                        userFilter             ( CTimeSpan& p_elapsed);

	std::vector< LoggedIn>      allusers               ();
	std::vector< LoggedIn>      getUsersFromSignFile   ();
	CTime                       lastSyncTimeForAllUsers();
	CTime                       findEarliestLogin      ( int p_nbOfDays, int p_nbOfHours, int p_nbOfMinutes);

	bool         specialUserInquiryHandled( VARIANT p);
public:
	// these can be used by other classes too
	MsgConsole                  m_console;
	void         sendMsg                ( const std::string&, int mtype );
	std::string  makelink               ( XmlObject * ptr);

/*	inline bool  isFileReadOnly         ( const std::string& p_file);
	inline bool  isFileReadOnly2        ( const std::string& p_file, bool* p_fileExisted);
	inline bool  isFileReadWrite        ( const std::string& p_file);
	inline bool  isFile                 ( const std::string& p_file);
	inline bool  isDir                  ( const std::string& p_file);
	inline bool  fileExist              ( const std::string& p_file);
*/
protected: // parser opts

	XERCES_CPP_NAMESPACE::DOMDocument*
	             enclosedParse          ( const std::string& p_fileName, XERCES_CPP_NAMESPACE::DOMLSParser* p_parser, bool *p_success);

	void         newDOMObjs( XERCES_CPP_NAMESPACE::DOMImplementationLS** p_domImpl, std::auto_ptr<XERCES_CPP_NAMESPACE::DOMLSParser>& p_domParser, std::auto_ptr<XERCES_CPP_NAMESPACE::DOMErrorHandler>& p_domErrHandler);

public: // parser creation for other tasks than the individual xml file scannning
	DOMLSParser*  getFreshParser          ( const std::string& p_whoIsTheUser, DOMImplementationLS ** p_ptrRetDomImpl = 0);

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
	std::string                     m_parentFolderPath;   // absolute path of the folder contains the project folder
	std::string                     m_folderPath;   // absolute path of the folder contains the project files
	std::string                     m_contentPath;
	static const char *             m_contentConst;
	std::string                     m_cacheFileName;
	std::string                     m_projectFileName;
	std::string                     m_projectName;
	bool                            m_inTransaction;
	bool                            m_savedOnce;

	// source control info
	int                             m_sourceControl;

	std::string                     m_vssParentPath;
	std::string                     m_vssPath;
	std::string                     m_vssUser;
	std::string                     m_vssPassword;
	// objects
	XmlObjVec                       m_objects;
	GUIDToXmlObjectMap              m_objectsByGUID;
	XmlObject                     * m_openedObject;
	XmlObject                     * m_root;
	bool                            m_trivialChanges;
	bool                            m_fullLockNeeded;


#ifdef _DEBUG
	std::string                     mylog;
#endif

	// transaction handling
	UndoMap                         m_undoMap;
	XmlObjSet                       m_modifiedObjects;
	XmlObjVec                       m_createdObjects;
	XmlObjSet                       m_deletedObjects;
	//XmlObjSet                       m_checkOutContainers;

	// protection list
	ProtectList                     m_protectList;
	SignManager                     m_signer;

	OperatingOptions                m_userOpts;
	std::vector< XmlObject*>        m_deldObjs;

	friend PublicStorage;
	friend SignManager;
	friend ProtectList;
};


#endif//MGA_COREXMLFILE_H
