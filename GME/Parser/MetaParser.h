
#ifndef MGA_MgaMetaParser_H
#define MGA_MgaMetaParser_H

#ifndef MGA_GENPARSER_H
#include "GenParser.h"
#endif

#include "resource.h"
#include "CommonVersionInfo.h"

// --------------------------- CMgaMetaParser

class ATL_NO_VTABLE CMgaMetaParser : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CMgaMetaParser, &__uuidof(MgaMetaParser)>,
	public ISupportErrorInfoImpl<&__uuidof(IMgaMetaParser)>,
	public IDispatchImpl<IMgaMetaParser, &__uuidof(IMgaMetaParser), &__uuidof(__MGAParserLib), 1, 1>,
	public IGMEVersionInfoImpl,
	public CGenParser
{
	enum pass_type{FIRST_PASS,SECOND_PASS};
	pass_type currentPass;
	bool foundconstraints;
	bool explicitguid;
public:
	CMgaMetaParser();
	~CMgaMetaParser();

DECLARE_REGISTRY_RESOURCEID(IDR_MgaMetaParser)
DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CMgaMetaParser)
	COM_INTERFACE_ENTRY(IMgaMetaParser)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY_IID(__uuidof(IGMEVersionInfo), IGMEVersionInfoImpl)
END_COM_MAP()

// ------- Methods

public:
	STDMETHOD(Parse)(BSTR filename, BSTR connection);
	
	HRESULT CloseAll();

// ------- Attributes

public:
	static const std::tstring GetNextToken(std::tstring::const_iterator &i, 
		std::tstring::const_iterator &e, std::tstring::const_iterator end);

	typedef CGenParserFunc<CMgaMetaParser> elementfunc;

	static elementfunc elementfuncs_firstpass[];
	static elementfunc elementfuncs_secondpass[];

	CComObjPtr<IMgaMetaProject> metaproject;

// ------- Element Handlers

public:
	void StartParadigm(const attributes_type &attributes);
	void EndComment();
	void EndAuthor();
	void EndDispName();
	void StartFolder(const attributes_type &attributes);
	void StartFolder2(const attributes_type &attributes);
	void StartAtom(const attributes_type &attributes);
	void StartModel(const attributes_type &attributes);
	void StartConnection(const attributes_type &attributes);
	void StartReference(const attributes_type &attributes);
	void StartSet(const attributes_type &attributes);
	void StartAttrDef(const attributes_type &attributes);
	void StartFCO2(const attributes_type &attributes);
	void StartRole2(const attributes_type &attributes);
	void StartAspect2(const attributes_type &attributes);
	void StartPart2(const attributes_type &attributes);
	void StartRegNode(const attributes_type &attributes);
	void StartConnJoint(const attributes_type &attributes);
	void StartPointerSpec(const attributes_type &attributes);
	void StartPointerItem(const attributes_type &attributes);
	void StartEnumItem(const attributes_type &attributes);
	void StartConstraint(const attributes_type &attributes);
	void EndConstraint();
	void fireStartFunction(const std::tstring& name, const attributes_type& attributes);
	void fireEndFunction(const std::tstring& name);
};

#endif//MGA_MgaMetaParser_H
