#if(0) // this class is not used for smart copy anymore
#ifndef KIND_CLOSURE_H
#define KIND_CLOSURE_H

#include "MakeClosure.h"

class CKindSelectClosure : public CMakeClosure
{
 public:
	CKindSelectClosure
			( IMgaFCOs *fcos 
			, IMgaFolders *fols
			, int refs = 1
			, int conns = 1
			, int sets = 1
			)
		: CMakeClosure
			( fcos
			, fols
			, 0 //ref
			, 0 //refd 
			, 0 //conn
			, 0 //refportconn
			, 0 //set_mem
			, 0 //mem_of_set
			, 1 //cont
			, 0 //part_of_mod
			, 1 //fold_cont
			, 0 //part_of_fold
			, 0 //base_type
			, 0 //derived_type
			, LIB_STUBS //into_libr
			, 3 //container_opt( 3)
			, 1 //direction_opt
			, -1 //all kinds
			, -1 //don't mark
			)
		, m_bRefs( refs == 1)
		, m_bConns( conns == 1)
		, m_bSets( sets == 1)
		, m_peculiarFolds()
		, m_peculiarFcos()
	{ }

	~CKindSelectClosure()
	{
		m_peculiarFolds.clear();
		m_peculiarFcos.clear();
	}

	/*void getSpecFCOs( IMgaFCOs **spec_fcos);
	void getSpecFolders( IMgaFolders **spec_folds);*/

	/*virtual*/ void procObj( IMgaObject* obj);
	/*virtual*/ bool isValidSelection();
	/*virtual*/ bool postValidateSelection();
	/*virtual*/ void preProcess();
	/*virtual*/ void postProcess();
	std::string m_iniSelPath;
	std::string m_accKind; // to store the special container kind the closure has been initiated from
 protected:
	bool m_bRefs;
	bool m_bConns;
	bool m_bSets;
	
	CComObjPtrVector<IMgaFolder> m_peculiarFolds;
	CComObjPtrVector<IMgaFCO> m_peculiarFcos; 
};


class CContainerSelectSomeKindClosure : public CKindSelectClosure
{
 public:
	CContainerSelectSomeKindClosure
			( IMgaFCOs *fcos 
			, IMgaFolders *fols
			, int refs = 1
			, int conns = 1
			, int sets = 1
			)
		: CKindSelectClosure
			( fcos
			, fols
			, refs
			, conns
			, sets
			)
	{ }

	/*virtual*/ bool isValidSelection();
	/*virtual*/ void postProcess();
 protected:

};

struct ConnsLast
{
	bool operator()( const CComObjPtr<IMgaFCO>& p1, const CComObjPtr<IMgaFCO>& p2) const;
};

#endif // KIND_CLOSURE_H
#endif
