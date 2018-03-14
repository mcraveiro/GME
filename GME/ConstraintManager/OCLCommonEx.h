//###############################################################################################################################################
//
//	Object Constraint Language Generic Manager
//	OCLCommonEx.h
//
//###############################################################################################################################################

#ifndef OCLCommonEx_h
#define OCLCommonEx_h

#include "StdAfx.h"
#include "OCLCommon.h"
#include "OCLException.h"
#include "OCLTree.h"
#include "OCLFeature.h"
#include "OCLType.h"
#include "mga.h"
#include "meta.h"

namespace OclCommonEx {

	enum ErrorImage
	{
		IMG_SYNTACTICAL_ERROR = 0,
		IMG_SEMANTICAL_ERROR = 1,
		IMG_VIOLATION_ERROR = 2,
		IMG_VIOLATION_WARNING = 3
	};

	typedef CComObjPtr<IMgaObject> SPObject;
	typedef std::vector<SPObject> ObjectVector;
	typedef CComObjPtr<IMgaFCO> SPFCO;
	typedef std::vector<SPFCO> FCOVector;
	typedef CComObjPtr<IMgaMetaBase> SPMeta;
	typedef std::vector<SPMeta> MetaBaseVector;
	typedef CComObjPtr<IMgaConnPoint> SPCP;
	typedef std::vector<SPCP> CPVector;
	typedef CComObjPtr<IMgaMetaPointerSpec> SPPointer;
	typedef std::vector<SPPointer> PointerVector;
	typedef CComObjPtr<IMgaMetaModel> SPModel;
	typedef std::map< std::string, SPModel> ModelMap;

//##############################################################################################################################################
//
//	F U N C T I O N S
//
//##############################################################################################################################################

	std::string Convert( const CString& strIn );

	CString Convert( const std::string& strIn );

	bool ParseCardinality( const CString& strCardinality );

	std::string UpperFirst( const std::string& strValue );
	std::string LowerFirst( const std::string& strValue );
	CString UpperFirst( const CString& strValue );
	CString LowerFirst( const CString& strValue );

	std::string GetConnPointRole( CComPtr<IMgaConnPoint> spCP );

	std::string GetObjectKind( CComPtr<IMgaObject> spObject );
	std::string GetObjectDisplayedName(CComPtr<IMgaObject>& spObject);

	std::string GetFCORole( CComPtr<IMgaFCO> spFCO );

	objtype_enum GetObjectType( CComPtr<IMgaObject> spObject );
	objtype_enum GetObjectType( CComPtr<IMgaMetaBase> spObject );

	std::string GetObjectName( CComPtr<IMgaObject> spObject );
	std::string GetObjectName( CComPtr<IMgaMetaBase> spObject );

	std::string ObjectTypeToString( objtype_enum eType );
	objtype_enum StringToObjectType( const std::string& strType );

	bool IsInLibrary( CComPtr<IMgaObject> spObject );
	StringVector GetLibraryPath( CComPtr<IMgaObject> spObject );

	std::string CreateType( const std::string& strKind, objtype_enum eType );
	bool ResolveType( const std::string& strFullName, std::string& strKind, objtype_enum& eType );

	void AddAssociation( OclMeta::TypeManager* pManager, OclMeta::Association* pAssociation, OclMeta::AssociationVector& vecAssociations, bool bCantBeSet );
	void AddAssociations( OclMeta::AssociationVector& vecAssociationsToAdd, OclMeta::AssociationVector& vecAssociations );

//##############################################################################################################################################
//
//	F U N C T I O N S
//
//##############################################################################################################################################

	bool AddObject( CComPtr<IMgaFCO> spFCO, FCOVector& vecFCOs );
	bool AddObject( CComPtr<IMgaObject> spObject, ObjectVector& vecObjects );
	bool ContainsObject( CComPtr<IMgaFCO> spFCO, FCOVector& vecFCOs );
	bool ContainsObject( CComPtr<IMgaObject> spObject, ObjectVector& vecObjects );

	// MetaModeling Time

	bool IsAbstract( CComPtr<IMgaFCO> spFCO );
	bool GetAllObjects( CComPtr<IMgaFCO>& spFCO, CComPtr<IMgaFCOs>& spRefs );
	void GetAssociationEnds( CComPtr<IMgaFCO> spFCO, const std::string& strRole, const std::string& strKind, FCOVector& vecFCOs );
	void GetAssociationEnds( CComPtr<IMgaFCO> spFCO, const std::string& strRole, const std::string& strKind, FCOVector& vecFCOs, FCOVector& vecConnections );
	void GetInheritances( CComPtr<IMgaFCO> spFCO, const std::string& strInheritanceType, bool bIsParent, FCOVector& vecFCOs );
	void GetEquivalences( CComPtr<IMgaFCO> spFCO, const std::string& strEquivalenceType, bool bToLeft, FCOVector& vecFCOs );

	// Modeling Time

	void GetMetaObjects( CComPtr<IMgaMetaProject> spProject, const std::string& strName, objtype_enum eType, MetaBaseVector& vecMetaBases );
	void GetDerivedObjects( CComPtr<IMgaProject> spProject, const std::string& strType, ObjectVector& vecObjects );
	void GetKindObjects( CComPtr<IMgaProject> spProject, const std::string& strKind, ObjectVector& vecObjects );
	void GetKindFolders( CComPtr<IMgaFolder> spFolder, const std::string& strKind, bool bKind, ObjectVector& vecObjects );

	std::string GetPointerRole( CComPtr<IMgaMetaPointerSpec> spPointer );
	ModelMap GetModels( CComPtr<IMgaMetaProject> spProject );
	StringVector GetPointerDescriptions( CComPtr<IMgaMetaPointerSpec> spPointer );
	StringVector TranslateDescriptionsToKinds( ModelMap& mapModels, const StringVector& vecDescriptions );

	std::string GetObjectPath( CComPtr<IMgaObject> spObject );
	std::string GetObjectDescription( CComPtr<IMgaObject> spObject );
	std::string GetConnectionPointDescription( CComPtr<IMgaConnPoint> spCP );




}; // namespace OclCommonEx

#endif // OCLCommonEx