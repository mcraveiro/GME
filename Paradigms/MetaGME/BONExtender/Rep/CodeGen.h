#ifndef CODEGEN_H
#define CODEGEN_H

#include "FCO.h"
#include "FolderRep.h"
#include "ModelRep.h"
#include "SetRep.h"
#include "ReferenceRep.h"
#include "ConnectionRep.h"
#include "Method.h"

//#include "vector"

class CodeGen
{

public:
	static std::string fill( int i = 1);
	static std::string indent( int i = 0);


	/**
	 * Visitor related methods
	 */

	static std::string folderKidsTemplate();
	static std::string modelKidsTemplate();
	static Method acceptMethod( Any * any, bool pWithTraversalOfKids, bool pSpecialized, bool pRetValBool);
	//obsolete static Method folderAcceptMethod( Any * cont, bool pWithTraversalOfKids, bool pSpecialized);
	//static Method modelAcceptMethod( Any * cont, bool pWithTraversalOfKids, bool pSpecialized);
	
	/**
	 * Folder related generators
	 */
	static void folderGetter( const std::string& retval_folderkind, const std::string& sub_name, const std::string& method_name, bool extended, Any * cont, Method& m);
	static void folderGetterGeneric( const std::string& retval_folderkind, const std::string& sub_name, const std::string& method_name, bool extended, Any * cont, Method& m);

	static void dumpFoldGetter( FolderRep* sub, FolderRep *fold);

	static void kindGetter1( const std::string& retval_kind, const std::string& kind, const std::string& method_name, Any * cont, Method& m);
	static void kindGetter2( const std::string& retval_kind, const std::string& kind, const std::vector< std::string>& kind_vec, const std::string& method_name, Any * cont, Method& m);	
	static void kindGetter3( const std::string& retval_kind, const std::string& kind, const std::string& method_name, Any * cont, Method& m);

	static void kindGetter1Generic( const std::string& retval_kind, const std::string& kind, const std::string& method_name, Any * cont, Method& m);
	static void kindGetter2Generic( const std::string& retval_kind, const std::string& kind, const std::vector< std::string>& kind_vec, const std::string& method_name, Any * cont, Method& m);	
	static void kindGetter3Generic( const std::string& retval_kind, const std::string& kind, const std::string& method_name, Any * cont, Method& m);

	static void dumpKindGetter( FCO* fco, FolderRep * cont);


	/**
	 * Model related generators
	 */
	static void roleGetter1( const std::string& retval_kind, const std::string& inquire, const std::string& method_name, const std::string& fco_name, Any * cont, Method& m);
	static void roleGetter1Generic( const std::string& retval_kind, const std::string& inquire, const std::string& method_name, const std::string& fco_name, Any * cont, Method& m);

	static void roleGetter2( const std::string& retval_kind, const std::string& inquire, const std::string& method_name, const std::string& fco_name, Any * cont, Method& m);
	static void roleGetter2Generic( const std::string& retval_kind, const std::string& inquire, const std::string& method_name, const std::string& fco_name, Any * cont, Method& m);

	static void roleGetter3( const std::string& retval_kind, const std::string& method_name, const std::string& fco_name, const std::string& role_name, const std::vector< std::string > & roles, const std::string& dummy_str, Any * cont, Method& m);
	static void roleGetter3Generic( const std::string& retval_kind, const std::string& method_name, const std::string& fco_name,  const std::string& role_name, const std::vector< std::string > & roles, const std::string& dummy_str, Any * cont, Method& m);

	static void roleGetter4( const std::string& retval_kind, const std::string& method_name, const std::string& fco_name, const std::string& desc_k_name, const std::string& role_name, const std::string& nmsp, Any * cont, Method& m);
	static void roleGetter4Generic( const std::string& retval_kind, const std::string& method_name, const std::string& fco_name, const std::string& desc_k_name, const std::string& role_name, const std::string& nmsp, Any * cont, Method& m);

	static void dumpRoleGetter( FCO* fco, RoleRep * role, ModelRep *);

	/**
	 * Set related generators
	 */
	
	static Method dumpSetGetter( SetRep * cont, const FCO * fco, const std::string& comm_kind, bool aggreg = false, bool dummy_par = false);
	static Method dumpSetGetterGeneric( SetRep * cont, const FCO * fco, const std::string& comm_kind, bool aggreg = false, bool dummy_par = false);

	/**
	 * Reference related generators
	 */
	static Method dumpRefGetter( ReferenceRep * cont, FCO * fco, const std::string& comm_kind);

	
	
	/**
	 * Connection related generators
	 */
	
	// get<In, Out>ConnectionLinks
	static Method dumpGetInConnectionLinks( FCO *fco, ConnectionRep * conn);
	static Method dumpGetOutConnectionLinks( FCO *fco, ConnectionRep * conn);
	static Method dumpGetBothConnectionLinks( FCO *fco, ConnectionRep * conn);
	
	static Method dumpGetInConnectionLinksGeneric( FCO *fco, ConnectionRep * conn);
	static Method dumpGetOutConnectionLinksGeneric( FCO *fco, ConnectionRep * conn);
	static Method dumpGetBothConnectionLinksGeneric( FCO *fco, ConnectionRep * conn);

	// get<In, Out>ConnectionEnd
	static Method dumpGetInConnectionEnd( FCO *fco, FCO* peer, ConnectionRep * conn, bool refport);
	static Method dumpGetOutConnectionEnd( FCO *fco, FCO* peer, ConnectionRep * conn, bool refport);
	static Method dumpGetBothConnectionEnd( FCO *fco, FCO* peer, ConnectionRep * conn, bool refport);
	
	static Method dumpGetInConnectionEndGeneric( FCO *fco, FCO* peer, ConnectionRep * conn, bool refport);
	static Method dumpGetOutConnectionEndGeneric( FCO *fco, FCO* peer, ConnectionRep * conn, bool refport);
	static Method dumpGetBothConnectionEndGeneric( FCO *fco, FCO* peer, ConnectionRep * conn, bool refport);

	//get<Src, Dst>
	static Method dumpGetSrc(FCO *fco, ConnectionRep * conn, bool refport);
	static Method dumpGetDst(FCO *fco, ConnectionRep * conn, bool refport);
};

#endif // CODEGEN_H