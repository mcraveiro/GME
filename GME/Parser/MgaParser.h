
#ifndef MGA_MGAPARSER_H_
#define MGA_MGAPARSER_H_

#ifndef MGA_GENPARSER_H
#include "GenParser.h"
#endif

#include "resource.h"
#include "CommonVersionInfo.h"
#include <hash_map>
#include <list>
#include <vector>
#include <map>
#include <exception>
#include "MgaUtil.h"
#include "Gme.h"

class CompareCComObj
{
 public:
	 bool operator()( const CComObjPtr<IMgaFCO>& p1, const CComObjPtr<IMgaFCO>& p2) const
	 {
		 // FIXME: comparing IMgaFCO* is not defined
		 return p1.p < p2.p;
	 }
};

// --------------------------- CMgaParser

class ATL_NO_VTABLE CMgaParser : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CMgaParser, &__uuidof(MgaParser)>,
	public ISupportErrorInfoImpl3<&__uuidof(IMgaParser), &__uuidof(IMgaParser2), &__uuidof(IMgaParser3)>,
	public IDispatchImpl<IMgaParser3, &__uuidof(IMgaParser3), &__uuidof(__MGAParserLib), 1, 1>,
	public IGMEVersionInfoImpl,
	public CGenParser
{
public:
	CMgaParser()
		: m_maintainGuids( false)
	{ 
		m_resolveDerFuncPtr = &CMgaParser::ResolveDerivation;
	}

DECLARE_REGISTRY_RESOURCEID(IDR_MGAPARSER)
DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CMgaParser)
	COM_INTERFACE_ENTRY2(IMgaParser, IMgaParser3)
	COM_INTERFACE_ENTRY2(IDispatch, IMgaParser3)
	COM_INTERFACE_ENTRY(IMgaParser2)
	COM_INTERFACE_ENTRY2(IMgaParser3, IMgaParser3)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY_IID(__uuidof(IGMEVersionInfo), IGMEVersionInfoImpl)
END_COM_MAP()

// ------- Methods

public:
	STDMETHOD(GetXMLInfo)(BSTR filename, BSTR *paradigm, BSTR* parversion, VARIANT *parguid, BSTR* basename, BSTR* version);
	STDMETHOD(GetXMLParadigm)(BSTR filename, BSTR *paradigm);
	STDMETHOD(GetClipXMLInfo)(BSTR filename, IMgaObject *target, VARIANT_BOOL *isacceptable, BSTR* p_acckind, BSTR* p_version);
	STDMETHOD(ParseFCOs)(IMgaObject *p, BSTR filename);
	STDMETHOD(ParseFCOs2)(IMgaObject *p, BSTR filename, ULONGLONG hwndParent);
	STDMETHOD(ParseClos1)(IMgaObject *p, BSTR filename); // basic
	STDMETHOD(ParseClos4)(IMgaObject *p, BSTR filename, int options); // smart
	STDMETHOD(ParseProject)(IMgaProject *p, BSTR filename);
	STDMETHOD(ParseProject2)(IMgaProject *p, BSTR filename, ULONGLONG hwndParent);

	std::tstring GetClipXMLInfo2();

	void CloseAll();

// ------- Passes

public:
	virtual void startElement(const XMLCh* const name, AttributeList& attributes);
    virtual void endElement(const XMLCh* const name);
	void fireStartFunction(const std::tstring& name, const attributes_type& attributes);
	void fireEndFunction(const std::tstring& name);

	struct range_type
	{
		counter_type begin;
		counter_type end;

		element_type previous;
	};

	std::list<range_type> ranges;

	int skip_element_level;
	class pass_exception : public std::exception
	{
		std::wstring w;
	public:
		pass_exception() { }
		pass_exception(std::wstring what) : w(what) { }
		virtual const char *what() const {
			return static_cast<const char*>(_bstr_t(w.c_str()));
		}
		virtual const wchar_t *wwhat() const {
			return w.c_str();
		}
	};

	typedef struct librecord {
		std::tstring libname;
		CComPtr<IMgaFolder> f;
	} librecord;

	typedef std::vector<librecord> librecords;
	librecords libstodo;
	CComVariant projectguid;
	_bstr_t	projectversion;

// ------- Logging

public:

// ------- Attributes

public:
	typedef CGenParserFunc<CMgaParser> elementfunc;

	enum FUNC_TABLE_STATE{MGA,MGA_INFO,BC_MGA,SC_MGA, CLIP_MGA_INFO};
	FUNC_TABLE_STATE funcTableState;

	static elementfunc elementfuncs_mga[];
	static elementfunc elementfuncs_mgainfo[];
	static elementfunc elementfuncs_bcmga[];
	static elementfunc elementfuncs_scmga[];
	static elementfunc elementfuncs_clipmgainfo[];

	CComObjPtr<IMgaProject> project;
	CComObjPtr<IMgaTerritory> territory;
	CComObjPtr<IMgaResolver> resolver;
	CComObjPtr<IMgaProgressDlg> progress;

	CComObjPtr<IMgaObject> m_target;

	BSTR *closversion;
	BSTR *closacckind;
	BSTR *infoparname;
	BSTR *infoparversion;
	VARIANT *infoparguid;
	BSTR *infoprojname;
	BSTR *infoversion;

	long project_prefs, project_prefs_orig;

	std::tstring constraint_name;
	std::tstring constraint_value;

	counter_type max_counter;
	int pass_count;

// ------- Lookup

public:
	void LookupByID(const std::tstring &id, CComObjPtr<IMgaObject> &object);
	void LookupByID(const std::tstring &id, CComObjPtr<IMgaFCO> &fco);
	// return true if successful
	bool RegisterLookup(const std::tstring &id, IMgaObject *object);
	void RegisterLookup(const attributes_type &attributes, IMgaObject *object);

	//void RegisterReadOnlyStatus( const attributes_type &attributes);
	bool GetIntendedReadOnlyFlag( bool *p_isReadOnly);

	typedef stdext::hash_map<std::tstring, CComBstrObj> id_lookup_type;
	typedef id_lookup_type::iterator id_lookup_iterator;

	id_lookup_type id_lookup;

	typedef std::list< bool > readonly_stack_type;
	readonly_stack_type readonly_stack;

// ------- Derivation

public:
	struct deriv_type
	{
		CComObjPtr<IMgaFCO> from;
		VARIANT_BOOL isinstance;
		bool isprimary;
	};

	typedef void (CMgaParser::*ResolverFuncType) (const attributes_type &, deriv_type &);
	ResolverFuncType m_resolveDerFuncPtr;

	void ResolveDerivation(const attributes_type &attributes, deriv_type &deriv);
	void ResolveClosure1Derivation(const attributes_type &attributes, deriv_type &deriv);
	void ResolveClosure4Derivation(const attributes_type &attributes, deriv_type &deriv);

	bool findPlaceForElem( const attributes_type &attributes, deriv_type& deriv, CComObjPtr<IMgaObject>& place);
	bool findPlaceForElem( const attributes_type &attributes, deriv_type& deriv, CComObjPtr<IMgaFolder>& place_fld, CComObjPtr<IMgaModel>& place_mdl);

	bool parseConnection( CComObjPtr<IMgaObject> prev, const attributes_type &attributes, CComObjPtr<IMgaFCO>& pFoundSrcObj, CComObjPtr<IMgaFCO>& pFoundDstObj, CComObjPtr<IMgaFCOs>& pFoundSrcRefChain, CComObjPtr<IMgaFCOs>& pFoundDstRefChain);
	bool findConnectionEnd( CComObjPtr<IMgaObject> prev, const std::tstring& isbound, const std::tstring& role, const std::tstring& targetGUID, const std::tstring& target, const std::tstring& refchainGUID, const std::tstring& refchain, CComObjPtr<IMgaFCO>& pFoundObj, CComObjPtr<IMgaFCOs>& pFoundRefChain);
	bool findObject( const CComObjPtr<IMgaModel>& prev, const attributes_type& attributes, CComObjPtr<IMgaFCO>& obj, const char type);
	bool findObject( const CComObjPtr<IMgaFolder>& prev, const attributes_type& attributes, CComObjPtr<IMgaFCO>& obj, const char type);
	bool findObjectIn( const CComObjPtr<IMgaObject>& parent, const CComObjPtrVector<IMgaFCO>& vec, const attributes_type& attributes, CComObjPtr<IMgaFCO>& obj, const char type);
	bool findFolderIn( const CComObjPtr<IMgaFolder>& prev, const attributes_type& attributes, CComObjPtr<IMgaFolder>& fol);
	void findObjOnRelPath( CComObjPtr<IMgaObject> obj_rel_to, const std::tstring& relpath , CComObjPtr<IMgaObject>& obj, const std::tstring& text);
	void findObjOnAbsPath( CComObjPtr<IMgaProject> project, const std::tstring& abspath , CComObjPtr<IMgaObject>& obj, const std::tstring& text);
	void findFCOWithRelPathAndGUID( CComObjPtr<IMgaObject> obj_rel_to, const std::tstring& relpath, const std::tstring& guid, CComObjPtr<IMgaFCO>& pFoundFco);
	void findFCOWithGUIDInTree( CComObjPtr<IMgaObject> pParent, const std::tstring& guid, CComObjPtr<IMgaFCO>& pFoundFCO);
	void findFCOWithGUID( CComObjPtr<IMgaObject> prev, const std::tstring& guid, CComObjPtr<IMgaFCO>& pFoundFCO);
	void stepUpInHierarchy( CComObjPtr<IMgaObject>& pCurrentObj, const std::tstring& originalPath, CComObjPtr<IMgaObject>& pNewParent, std::tstring& remainingPath);

	static bool isNullRef( CComObjPtr<IMgaFCO>& ref);
	static bool isEmptySet( CComObjPtr<IMgaFCO>& set);
	bool isNeedFor2ndStep();
	void tryToFindMissedReferreds();
	void tryToFindMissedSetMembers();
	std::map< CComObjPtr<IMgaFCO>, std::tstring, CompareCComObj > m_notFoundReferredObject;
	std::map< CComObjPtr<IMgaFCO>, std::vector< std::tstring >, CompareCComObj > m_notFoundSetMembers;
	std::tstring m_clVerStr;

	bool manual_relid_mode;
	long relid;

	void preparerelid(const attributes_type &attributes);
	void assignrelid(IMgaObject *obj);	

	bool m_maintainGuids; // if parser should parse and set (into MGA layer) the found guids
// ------- Element Handlers
	
	public:
	void StartProject(const attributes_type &attributes);
	void EndName();
	void EndComment();
	void EndAuthor();
	void EndValue();
	void StartFolder(const attributes_type &attributes);
	void StartModel(const attributes_type &attributes);
	void StartAtom(const attributes_type &attributes);
	void StartRegNode(const attributes_type &attributes);
	void StartAttribute(const attributes_type &attributes);
	void StartConnection(const attributes_type &attributes);
	void StartConnPoint(const attributes_type &attributes);
	void EndConstraint();
	void StartReference(const attributes_type &attributes);
	void StartSet(const attributes_type &attributes);
	void StartClipboard(const attributes_type &attributes);
	void EndObject();

	void StartProjectInfo(const attributes_type &attributes);
	void EndNameInfo();

	void StartBCConnPoint(const attributes_type &attributes);
	void StartBCReference(const attributes_type &attributes);
	void StartBCSet(const attributes_type &attributes);
	void StartBCClipboard(const attributes_type &attributes);

	void StartSCFolder(const attributes_type &attributes);
	void StartSCConnection(const attributes_type &attributes);
	void StartSCConnPoint(const attributes_type &attributes);
	void StartSCReference(const attributes_type &attributes);
	void StartSCSet(const attributes_type &attributes);
	void StartSCAtom(const attributes_type &attributes);
	void StartSCModel(const attributes_type &attributes);
	void msgSC( CComBSTR& msg, msgtype_enum type);

	void EndSCConnection();
	void EndSCName();
	void EndSCValue();
	void EndSCConstraint();
	void StartSCRegNode(const attributes_type &attributes);
	void StartSCAttribute(const attributes_type &attributes);
	void EndSCRegNode();
	bool m_mergeAllowed;


	void StartClipboardInfo(const attributes_type &attributes);

	STDMETHOD(get_Resolver)(IMgaResolver **MgaResolver) {
		if (this->resolver)
		{
			resolver->AddRef();
		}
		*MgaResolver = this->resolver;
		return S_OK;
	}
    STDMETHOD(put_Resolver)(IMgaResolver *MgaResolver)
	{
		if (this->resolver)
		{
			this->resolver.Release();
		}
		this->resolver = MgaResolver;
		return S_OK;
	}

	protected:
		static CComPtr<IGMEOLEApp>	get_GME(CComObjPtr<IMgaProject> project);
		static void					clear_GME( CComPtr<IGMEOLEApp>& p_GME);
		CComPtr<IGMEOLEApp> m_GME;

};

#endif //__MGAPARSER_H_
