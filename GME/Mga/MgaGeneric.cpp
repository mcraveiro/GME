#include "stdafx.h"
 
void CreateCoreMetaProject(CComPtr<ICoreMetaProject> &project, bool v2 /*=false*/)
{
	ASSERT( project == NULL );

	COMTHROW( project.CoCreateInstance(L"Mga.CoreMetaProject") );

	COMTHROW( project->put_Token(PutInBstr("MgaProject")) );
	COMTHROW( project->put_Name(PutInBstr("MgaProject")) );

	GUID guid = {0x028F7444,0x2E51,0x11D3,{0xB3,0x6D,0x00,0x62,0x08,0x2D,0xF8,0x74}};
	COMTHROW( project->put_GUID(PutInVariant(guid)) );

// ------- Macros

	CComObjPtr<ICoreMetaObject> object;

#define CREATE_OBJECT(metaid, token, name) \
	object.Release(); \
	COMTHROW( project->AddObject(metaid, \
		PutInBstr(token), PutInBstr(name), PutOut(object)) );

#define CREATE_POINTER(metaid, token, name) \
	ASSERT(metaid == ATTRID_FATHER || metaid >= ATTRID_REF_MIN);\
	COMTHROW( object->AddAttribute((metaid), \
		PutInBstr(token), PutInBstr(name), VALTYPE_POINTER, NULL) );

#define CREATE_COLLECTION(metaid, token, name) \
	COMTHROW( object->AddAttribute((metaid) + ATTRID_COLLECTION, \
		PutInBstr(token), PutInBstr(name), VALTYPE_COLLECTION, NULL) );

#define CREATE_ATTRIBUTE(metaid, token, name, valtype) \
	ASSERT(metaid < ATTRID_REF_MIN);\
	COMTHROW( object->AddAttribute((metaid), \
		PutInBstr(token), PutInBstr(name), valtype, NULL) );

#define CLSID_PUSH(clsid) \
	{ CComVariant id; CopyTo(clsid, id); COMTHROW( object->AddClassID(id) ); }

//////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// COMMON ///////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

#define COMMON_DEF \
		CREATE_ATTRIBUTE(ATTRID_LOCK, "ObjLock", "Object Lock", VALTYPE_LOCK);

//ROOT
		CREATE_OBJECT(DTID_ROOT, "Root", "Template For RootObject");
		COMMON_DEF
		CREATE_COLLECTION(ATTRID_FATHER, "DummyFather", "Dummy Father");
		CREATE_POINTER(ATTRID_FATHER, "DummyChildren", "Dummy Children");

		CREATE_ATTRIBUTE(ATTRID_NAME, "Name", "Project Name",			VALTYPE_STRING);
		CREATE_ATTRIBUTE(ATTRID_CDATE, "CDate", "Create Date",				VALTYPE_DATE);
		CREATE_ATTRIBUTE(ATTRID_MDATE, "MDate", "Modification Date",		VALTYPE_DATE);
		CREATE_ATTRIBUTE(ATTRID_CREATOR, "Author", "Created by",			VALTYPE_STRING);
		CREATE_ATTRIBUTE(ATTRID_GUID, "GUID", "GUID",					    VALTYPE_BINARY);
		CREATE_ATTRIBUTE(ATTRID_VERSION, "Version", "Version",				VALTYPE_STRING);
		CREATE_ATTRIBUTE(ATTRID_PARGUID, "ParadigmGUID", "Paradigm GUID",	VALTYPE_BINARY);
		CREATE_ATTRIBUTE(ATTRID_PARVERSION, "ParadigmVersion", "Paradigm Version",	VALTYPE_STRING);
		CREATE_ATTRIBUTE(ATTRID_PARADIGM, "ExtParadigm", "External Paradigm",VALTYPE_STRING);
		CREATE_ATTRIBUTE(ATTRID_EXTDATA, "Comment", "Project Comments",			VALTYPE_STRING);
		CREATE_ATTRIBUTE(ATTRID_MGAVERSION, "MGA_Version", "MGA Version Code", VALTYPE_LONG);
		CREATE_COLLECTION(ATTRID_PARENT, "Children", "Child Objects");
//CONSTRAINT
		CREATE_OBJECT(DTID_CONSTRAINT, "Constraint", "Template For Constraint");
		COMMON_DEF

		CREATE_ATTRIBUTE(ATTRID_NAME, "Name", "Constraint Name",			VALTYPE_STRING);
		CREATE_ATTRIBUTE(ATTRID_CONSTROCL, "ConstraintOCL", "Constraint Text",VALTYPE_STRING);
		CREATE_ATTRIBUTE(ATTRID_CONSTRPRIORITY, "ConstrPriority", "Constraint Priority",VALTYPE_STRING);
		CREATE_POINTER(ATTRID_CONSTROWNER, "ConstraintOf", "Owner Kind");

// Old REGNODE format. Upgraded to new format in CoreBinFile
//REGNODE
//		CREATE_OBJECT(DTID_REGNODE, "RegNode", "Template For Registry Node");
//		COMMON_DEF
//
//		CREATE_ATTRIBUTE(ATTRID_NAME, "Name", "RegNode Name",		VALTYPE_STRING);
//		CREATE_ATTRIBUTE(ATTRID_REGFLAGS, "RegistryFlags", "Registry Flags", VALTYPE_LONG);
//		CREATE_POINTER(ATTRID_REGNOWNER, "RegNodeOf", "Parent Object/Regnode");
//		CREATE_COLLECTION(ATTRID_REGNOWNER, "RegNodes", "Registry Nodes");
//		CREATE_ATTRIBUTE(ATTRID_REGNODEVALUE, "RegNodeValue", "RegNode Value",VALTYPE_STRING);
//		CREATE_POINTER(ATTRID_XREF,	"FCOref", "Referenced FCO");

//////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// DATA ///////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

#define GUID_ATTRS_DEFS \
		{ CREATE_ATTRIBUTE(ATTRID_GUID1, "GUID1", "FCO GUID1",	VALTYPE_LONG);\
		CREATE_ATTRIBUTE(ATTRID_GUID2, "GUID2", "FCO GUID2",	VALTYPE_LONG);\
		CREATE_ATTRIBUTE(ATTRID_GUID3, "GUID3", "FCO GUID3",	VALTYPE_LONG);\
		CREATE_ATTRIBUTE(ATTRID_GUID4, "GUID4", "FCO GUID4",	VALTYPE_LONG);}

#define STATUS_ATTR_DEF CREATE_ATTRIBUTE(ATTRID_FILESTATUS, "fstate", "Multiuser File Status", VALTYPE_LONG);

//FOLDER
		CREATE_OBJECT(DTID_FOLDER, "Folder", "Template For Folder");
		COMMON_DEF

		CREATE_ATTRIBUTE(ATTRID_CDATE, "CDate", "Create Date",				VALTYPE_DATE);
		CREATE_ATTRIBUTE(ATTRID_MDATE, "MDate", "Modification Date",		VALTYPE_DATE);
		CREATE_ATTRIBUTE(ATTRID_NAME, "Name", "Folder Name",	VALTYPE_STRING);
		CREATE_ATTRIBUTE(ATTRID_META, "Meta", "Meta Identifier",VALTYPE_METAREF);
		CREATE_ATTRIBUTE(ATTRID_LASTRELID, "LastRelID", "Last Child RelID",	VALTYPE_LONG);
		CREATE_ATTRIBUTE(ATTRID_RELID, "RelID", "RelID",	VALTYPE_LONG);
		CREATE_POINTER(ATTRID_PARENT, "Parent", "Parent Object");
		CREATE_COLLECTION(ATTRID_PARENT, "Children", "Child Objects");
		CREATE_COLLECTION(ATTRID_CONSTROWNER, "Constraints", "Constraints");
		//CREATE_COLLECTION(ATTRID_REGNOWNER, "RegNodes", "Registry Nodes");
		CREATE_ATTRIBUTE(ATTRID_REGNODE, "RegNodes", "Registry Nodes", VALTYPE_DICT);
		CREATE_ATTRIBUTE(ATTRID_PERMISSIONS, "Permissions", "Permissions", VALTYPE_LONG);

		if( v2) GUID_ATTRS_DEFS;
		if( v2) STATUS_ATTR_DEF;
		CLSID_PUSH(  __uuidof(MgaO) );  
		
//FCO
#define FCO_DEF \
		COMMON_DEF\
		CREATE_ATTRIBUTE(ATTRID_NAME, "Name", "FCO Name",	VALTYPE_STRING);\
		CREATE_ATTRIBUTE(ATTRID_RELID, "RelID", "RelID",	VALTYPE_LONG);\
		CREATE_ATTRIBUTE(ATTRID_META, "Meta", "Meta Identifier",VALTYPE_METAREF);\
		CREATE_ATTRIBUTE(ATTRID_ROLEMETA, "RoleMeta", "Role Meta Identifier",VALTYPE_METAREF);\
		CREATE_POINTER(ATTRID_FCOPARENT, "Parent", "Parent Object");\
		CREATE_COLLECTION(ATTRID_CONSTROWNER, "Constraints", "Constraints");\
		CREATE_ATTRIBUTE(ATTRID_REGNODE, "RegNodes", "Registry Nodes", VALTYPE_DICT); \
		CREATE_COLLECTION(ATTRID_REFERENCE, "References", "Referenced by");\
		CREATE_COLLECTION(ATTRID_XREF,		"XReferences", "Cross refs");\
		CREATE_COLLECTION(ATTRID_ATTRPARENT,"Attributes", "Attributes");\
		CREATE_POINTER(ATTRID_DERIVED, "BaseType", "BaseType");\
		CREATE_COLLECTION(ATTRID_DERIVED, "SubTypes", "SubTypes/Instances"); \
		CREATE_ATTRIBUTE(ATTRID_PERMISSIONS, "Permissions", "Permissions", VALTYPE_LONG);

//MODEL
		CREATE_OBJECT(DTID_MODEL, "Model", "Template For Model");
		FCO_DEF
		
		CREATE_ATTRIBUTE(ATTRID_CDATE, "CDate", "Create Date",				VALTYPE_DATE);
		CREATE_ATTRIBUTE(ATTRID_MDATE, "MDate", "Modification Date",		VALTYPE_DATE);
		CREATE_COLLECTION(ATTRID_PARENT, "Children", "Child Objects");
		CREATE_ATTRIBUTE(ATTRID_LASTRELID, "LastRelID", "Last Child RelID",	VALTYPE_LONG);

		if( v2) GUID_ATTRS_DEFS;
		if( v2) STATUS_ATTR_DEF;

		CLSID_PUSH(  __uuidof(MgaO) );  

//ATOM		
		CREATE_OBJECT(DTID_ATOM, "Atom", "Template For Atom");
		FCO_DEF

		if( v2) GUID_ATTRS_DEFS;
		CLSID_PUSH(  __uuidof(MgaO) );  

//REFERENCE		
		CREATE_OBJECT(DTID_REFERENCE, "Reference", "Template For Reference");
		FCO_DEF

		CREATE_POINTER(ATTRID_REFERENCE, "Refers", "Referenced FCO");
		CREATE_COLLECTION(ATTRID_SEGREF, "Segments", "Connection Segments");
		CREATE_POINTER(ATTRID_MASTEROBJ, "MasterObj", "Master Object");
		CREATE_COLLECTION(ATTRID_MASTEROBJ, "MasterOf", "Master Of Objects");
		CREATE_ATTRIBUTE(ATTRID_REFASPECT, "RefAspect", "Ref Aspect", VALTYPE_METAREF);

		if( v2) GUID_ATTRS_DEFS;
		CLSID_PUSH(  __uuidof(MgaO) );  
		
//SET		
		CREATE_OBJECT(DTID_SET, "Set", "Template For Set");
		FCO_DEF


		CREATE_COLLECTION(ATTRID_SETMEMBER, "Members", "Set Members");

		if( v2) GUID_ATTRS_DEFS;
		CLSID_PUSH(  __uuidof(MgaO) );  


//SETNODE		
		CREATE_OBJECT(DTID_SETNODE, "SetNode", "Template For Set Member");
		COMMON_DEF

		CREATE_POINTER(ATTRID_SETMEMBER, "SetOwner", "Owner Set");
		CREATE_POINTER(ATTRID_XREF,		"FCOref", "Member FCO");
		CREATE_POINTER(ATTRID_MASTEROBJ, "MasterObj", "Master Object");
		CREATE_COLLECTION(ATTRID_MASTEROBJ, "MasterOf", "Master Of Objects");


//CONNECTION		
		CREATE_OBJECT(DTID_CONNECTION, "Connection", "Template For Connection");
		FCO_DEF
		
		CREATE_COLLECTION(ATTRID_CONNROLE, "ConnRoles", "ConnRoles");

		if( v2) GUID_ATTRS_DEFS;
		CLSID_PUSH(  __uuidof(MgaO) );  
		
//CONNECTION ROLE		
		CREATE_OBJECT(DTID_CONNROLE, "ConnRole", "Template for Connection Role");
		COMMON_DEF

		CREATE_ATTRIBUTE(ATTRID_NAME, "Name", "Connrole Name",VALTYPE_STRING);
		CREATE_POINTER(ATTRID_XREF,		"FCOref", "Connected FCO");
		CREATE_POINTER(ATTRID_CONNROLE, "CRoleParent", "Owner of ConnRole");
		CREATE_COLLECTION(ATTRID_CONNSEG, "ConnSegments", "Connection Segments");
		CREATE_POINTER(ATTRID_MASTEROBJ, "MasterObj", "Master Object");
		CREATE_COLLECTION(ATTRID_MASTEROBJ, "MasterOf", "Master Of Objects");

//CONNECTION ROLE SEGMENT
		CREATE_OBJECT(DTID_CONNROLESEG, "ConnRoleSeg", "Template for ConnRole Segment");
		COMMON_DEF

		CREATE_ATTRIBUTE(ATTRID_SEGORDNUM, "SegOrdNum", "Segment Order 1,2,3 ", VALTYPE_LONG);
		CREATE_POINTER(ATTRID_CONNSEG, "ConnSRole", "Owner Connection Role");
		CREATE_POINTER(ATTRID_SEGREF, "ReferenceObj", "Reference Involved");


// ATTRIBUTE
#define ATTR_DEF \
		COMMON_DEF\
		CREATE_ATTRIBUTE(ATTRID_META, "Meta", "Meta Identifier",VALTYPE_METAREF);\
		CREATE_POINTER(ATTRID_ATTRPARENT,"Owner", "Owner FCO");

//STRATTR
		CREATE_OBJECT(DTID_STRATTR, "StrAttr", "String Attribute");
		ATTR_DEF
		CREATE_ATTRIBUTE(ATTRID_STRATTR, "StrValue", "String Value",VALTYPE_STRING);

//INTATTR
		CREATE_OBJECT(DTID_INTATTR, "IntAttr", "LongInt Attribute");
		ATTR_DEF
		CREATE_ATTRIBUTE(ATTRID_INTATTR, "IntValue", "LongInt Value",VALTYPE_LONG);

//FLOATATTR
		CREATE_OBJECT(DTID_FLOATATTR, "FloatAttr", "Float Attribute");
		ATTR_DEF
		CREATE_ATTRIBUTE(ATTRID_FLOATATTR, "FloatValue", "Float Value",VALTYPE_REAL);

//BOOLATTR
		CREATE_OBJECT(DTID_BOOLATTR, "BoolAttr", "Bool Attribute");
		ATTR_DEF
		CREATE_ATTRIBUTE(ATTRID_BOOLATTR, "BoolValue", "Bool Value",VALTYPE_LONG);

//REFATTR
		CREATE_OBJECT(DTID_REFATTR, "RefAttr", "Reference Attribute");
		ATTR_DEF
		CREATE_POINTER(ATTRID_XREF,	"FCOref", "Referenced FCO");

}


