#ifndef SIMPLE_CLOSURE_H
#define SIMPLE_CLOSURE_H

#include "MakeClosure.h"

class CSimpleSelectClosure : public CMakeClosure
{
 public:
	CSimpleSelectClosure
			( IMgaFCOs *fcos 
			, IMgaFolders *fols
			, int containment
			, int folder_containment
			, bool excl_atom
			, bool excl_conn
			, bool excl_ref
			, bool excl_set
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
			, containment //cont
			, 0 //part_of_mod
			, folder_containment //fold_cont
			, 0 //part_of_fold
			, 0 //base_type
			, 0 //derived_type
			, LIB_CONT //into_libr
			, 3 //container_opt( 3)
			, 1 //direction_opt
			, -1 //all kinds
			, -1 //don't mark
			)
		, m_bExclAtom( excl_atom)
		, m_bExclConn( excl_conn)
		, m_bExclRef( excl_ref)
		, m_bExclSet( excl_set)
	{ }

	~CSimpleSelectClosure()
	{
	}

	/*virtual*/ bool isValidSelection();
	/*virtual*/ void procObj( IMgaObject* obj);
	/*virtual*/ void preProcess();
	/*virtual*/ void postProcess();
	bool removeExcludedKinds();
	void manageGuid( CComObjPtr<IMgaObject> obj);

 protected:
	bool m_bExclAtom;
	bool m_bExclConn;
	bool m_bExclRef;
	bool m_bExclSet;

};

struct ConnsAndDerivsLast
{
	bool operator()( const CComObjPtr<IMgaFCO>& p1, const CComObjPtr<IMgaFCO>& p2) const;
};

#endif // SIMPLE_CLOSURE_H
