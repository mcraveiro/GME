#pragma once
#include <vector>
#include <string>
#include <ostream>

class Rule //: public CObject
{
public: // typedefs
	typedef enum       // keep in sync with rule_templates
	{
		KindNameChange,
		AttrNameChange,
		AttrTypeChange,
		EnumAttrValueChange,
		MoveUpElem,
		MoveDownElem,
		RemoveKind,
		RemoveGlobalAttr,
		RemoveLocalAttr,
		Atom2Model,
		Model2Atom,
		ParadigmGUIDChange,
		ParadigmNameChange,
		ParadigmVersionChange
	} rule_type;


	static const int rule_nmb = 14;
	static const char * rule_templates[ rule_nmb];// keep in sync 
	//{ "TKindNameChange.xsl"
	//, "TAttrNameChange.xsl"
	//, "TAttrTypeChange.xsl"
	//, "TEnumAttrValueChange.xsl"
	//, "TMoveUpElem.xsl"
	//, "TMoveDownElem.xsl"
	//, "TRemoveKind.xsl"
	//, "TRemoveGlobalAttr.xsl"
	//, "TRemoveLocalAttr.xsl"
	//, "TAtom2Model.xsl"
	//, "TModel2Atom.xsl"
	//, "TParadigmGUIDChange.xsl"
	//, "TParadigmNameChange.xsl"
	//, "TParadigmVersionChange.xsl"
	//};

	static CString ruleTypeStr( int i);
	static CString ruleSummary( const Rule& pRule);
	static CString ruleSummary( Rule::rule_type pTyp, const std::vector<std::string>& pParams);

public:
	//DECLARE_SERIAL( Rule)
	void Serialize( CArchive& ar);

	Rule(void); // needed for SERIAL
	Rule( rule_type rt, std::vector<std::string>& vec);
	~Rule(void);

	void gen( std::ostream& pFstream);
	//std::string getSummary();

	rule_type m_type;
	std::vector< std::string> m_params;
	std::string m_name;

//protected:
	Rule( const Rule&);
	const Rule& operator=( const Rule&);
};
