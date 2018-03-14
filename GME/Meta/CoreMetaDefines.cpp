
#include "stdafx.h"

void CreateCoreMetaProject(CComPtr<ICoreMetaProject> &project)
{
	ASSERT( project == NULL );

	COMTHROW( project.CoCreateInstance(OLESTR("MGA.CoreMetaProject")) );

	COMTHROW( project->put_Token(PutInBstr("MgaMetaProject")) );
	COMTHROW( project->put_Name(PutInBstr("MgaMetaProject")) );

	::GUID guid = {0x028F7AA4,0x2E51,0x11D3,{0xB3,0x17,0x00,0x62,0x08,0x2D,0xF8,0x85}};
	COMTHROW( project->put_GUID(PutInVariant(guid)) );

// ------- Macros

	CComObjPtr<ICoreMetaObject> object;

#define CREATE_OBJECT(metaid, token, name) \
	object.Release(); \
	COMTHROW( project->AddObject(metaid, \
		PutInBstr(token), PutInBstr(name), PutOut(object)) );

#define CREATE_POINTER(metaid, token, name) \
	COMTHROW( object->AddAttribute((metaid), \
		PutInBstr(token), PutInBstr(name), VALTYPE_POINTER, NULL) );

#define CREATE_COLLECTION(metaid, token, name) \
	COMTHROW( object->AddAttribute((metaid) + ATTRID_COLLECTION, \
		PutInBstr(token), PutInBstr(name), VALTYPE_COLLECTION, NULL) );

#define CREATE_ATTRIBUTE(metaid, token, name, valtype) \
	COMTHROW( object->AddAttribute((metaid), \
		PutInBstr(token), PutInBstr(name), valtype, NULL) );

#define CLSID_PUSH(clsid) \
	{ CComVariant id; CopyTo(clsid, id); COMTHROW( object->AddClassID(id) ); }

// ------- Common

#define DECLARE_COMMON() \
	CREATE_ATTRIBUTE(ATTRID_LOCK, "ObjLock", "Object Lock", VALTYPE_LOCK);

// ------- Base

#define DECLARE_BASE() \
	DECLARE_COMMON(); \
	CREATE_ATTRIBUTE(ATTRID_NAME, "Name", "Name", VALTYPE_STRING); \
	CREATE_ATTRIBUTE(ATTRID_METAREF, "MetaRef", "Meta ID",VALTYPE_METAREF); \
	CREATE_ATTRIBUTE(ATTRID_DISPNAME, "DispName", "Displayed Name", VALTYPE_STRING); \
	CREATE_COLLECTION(ATTRID_REGNODES_COLL, "RegNodes", "Registry Nodes"); \
	CREATE_COLLECTION(ATTRID_CONSTRAINT_PTR, "Constraints", "Constraints");

// ------- Folder

#define DECLARE_FOLDER() \
	DECLARE_BASE(); \
	CREATE_POINTER(ATTRID_DEFFOLDER_PTR, "FolderParent", "Defined In"); \
	CREATE_COLLECTION(ATTRID_DEFFOLDER_PTR, "DefinedFolders", "Defined Folders"); \
	CREATE_COLLECTION(ATTRID_DEFFCO_PTR, "DefinedFCOs", "Defined FCOs"); \
	CREATE_COLLECTION(ATTRID_DEFATTR_PTR, "DefinedAttributes", "Defined Attributes"); \
	CREATE_COLLECTION(ATTRID_FOLDERLINK_PARENT_PTR, "LegalFolders", "Legal Child Folders"); \
	CREATE_COLLECTION(ATTRID_FOLDERLINK_CHILD_PTR, "LegalParents", "Used In Folders"); \
	CREATE_COLLECTION(ATTRID_ROOTOBJLINK_FOLDER_PTR, "ObjectLinks", "Legal Root Objects");

	CREATE_OBJECT(METAID_METAFOLDER, "MetaFolder", "Meta Folder");
	DECLARE_FOLDER();
	CLSID_PUSH(__uuidof(MgaMetaFolder));

	CREATE_OBJECT(METAID_METAFOLDERLINK, "MetaFolderLink", "Meta Folder Link");
	DECLARE_COMMON();
	CREATE_POINTER(ATTRID_FOLDERLINK_PARENT_PTR, "Parent", "Parent");
	CREATE_POINTER(ATTRID_FOLDERLINK_CHILD_PTR, "Child", "Child");

	CREATE_OBJECT(METAID_METAROOTOBJLINK, "MetaRootObjLink", "Meta Root Object Link");
	DECLARE_COMMON();
	CREATE_POINTER(ATTRID_ROOTOBJLINK_FOLDER_PTR, "Folder", "Folder");
	CREATE_POINTER(ATTRID_ROOTOBJLINK_OBJECT_PTR, "Object", "Object");

// ------- Project

	CREATE_OBJECT(METAID_METAPROJECT, "MetaProject", "Meta Project");
	DECLARE_FOLDER();
	CREATE_ATTRIBUTE(ATTRID_PARNAME, "ParName", "Paradigm Name", VALTYPE_STRING);
	CREATE_ATTRIBUTE(ATTRID_PARDISPNAME, "ParDispName", "Displayed Name", VALTYPE_STRING); \
	CREATE_ATTRIBUTE(ATTRID_GUID, "ParGUID", "Paradigm GUID", VALTYPE_BINARY);
	CREATE_ATTRIBUTE(ATTRID_VERSION, "Version", "Paradigm Version", VALTYPE_STRING);
	CREATE_ATTRIBUTE(ATTRID_AUTHOR, "Author", "Paradigm Author", VALTYPE_STRING);
	CREATE_ATTRIBUTE(ATTRID_COMMENT, "Comment", "Comment", VALTYPE_STRING);
	CREATE_ATTRIBUTE(ATTRID_CDATE, "CDate", "Create Date", VALTYPE_DATE);
	CREATE_ATTRIBUTE(ATTRID_MDATE, "MDate", "Modification Date", VALTYPE_DATE);
	CLSID_PUSH(__uuidof(MgaMetaFolder));

// ------- FCO

#define DECLARE_FCO() \
	DECLARE_BASE() \
	CREATE_POINTER(ATTRID_DEFFCO_PTR, "ObjectParent", "Defined In"); \
	CREATE_COLLECTION(ATTRID_DEFATTR_PTR, "DefinedAttributes", "Defined Attributes"); \
	CREATE_COLLECTION(ATTRID_KIND_PTR, "UsedInRoles", "Used In Roles"); \
	CREATE_COLLECTION(ATTRID_ROOTOBJLINK_OBJECT_PTR, "FolderLinks", "Used In Folders"); \
	CREATE_COLLECTION(ATTRID_ATTRLINK_USEDIN_PTR, "Attributes", "Attributes");\
/*	CREATE_ATTRIBUTE(ATTRID_ALIASENABLED, "AliasEnab", "Aliases Enabled", VALTYPE_LONG) */ ;

// ------- Model

	CREATE_OBJECT(METAID_METAMODEL, "MetaModel", "Meta Model");
	DECLARE_FCO();
	CREATE_COLLECTION(ATTRID_DEFFCO_PTR, "DefinedFCOs", "Defined FCOs");
	CREATE_COLLECTION(ATTRID_ROLES_COLL, "Roles", "Roles");
	CREATE_COLLECTION(ATTRID_ASPECTS_COLL, "Aspects", "Aspects");
	CLSID_PUSH(__uuidof(MgaMetaModel));

// ------- Aspect

	CREATE_OBJECT(METAID_METAASPECT, "MetaAspect", "Meta Aspect");
	DECLARE_BASE();
	CREATE_POINTER(ATTRID_ASPECTS_COLL, "Model", "Model");
	CREATE_COLLECTION(ATTRID_PARTASPECT_PTR, "AspectParts", "Parts");
	CREATE_COLLECTION(ATTRID_ATTRLINK_USEDIN_PTR, "Attributes", "Attributes");
	CLSID_PUSH(__uuidof(MgaMetaAspect));

// ------- Role

	CREATE_OBJECT(METAID_METAROLE, "MetaRole", "Meta Role");
	DECLARE_BASE();
	CREATE_POINTER(ATTRID_ROLES_COLL, "ParentModel", "Parent Model");
	CREATE_COLLECTION(ATTRID_PARTROLE_PTR, "RoleParts", "Parts");
	CREATE_POINTER(ATTRID_KIND_PTR, "Kind", "Kind");
	CLSID_PUSH(__uuidof(MgaMetaRole));

// ------- Part

	CREATE_OBJECT(METAID_METAPART, "MetaPart", "Meta Part");
	DECLARE_BASE();
	CREATE_POINTER(ATTRID_PARTROLE_PTR, "Role", "Role");
	CREATE_POINTER(ATTRID_PARTASPECT_PTR, "Aspect", "Aspect");
	CREATE_ATTRIBUTE(ATTRID_PARTDATA, "Data", "Part Data", VALTYPE_LONG);
	CREATE_ATTRIBUTE(ATTRID_KINDASPECT, "KindAspect", "Kind Aspect", VALTYPE_STRING);
	CLSID_PUSH(__uuidof(MgaMetaPart));

// ------- Atom

	CREATE_OBJECT(METAID_METAATOM, "MetaAtom", "Meta Atom");
	DECLARE_FCO();
	CLSID_PUSH(__uuidof(MgaMetaAtom));

// ------- Attribute

	CREATE_OBJECT(METAID_METAATTRIBUTE, "MetaAttribute", "Meta Attribute");
	DECLARE_BASE();
	CREATE_POINTER(ATTRID_DEFATTR_PTR, "AttrParent", "Defined In");
	CREATE_COLLECTION(ATTRID_ATTRLINK_ATTR_PTR, "AttrLinks", "Used In");
	CREATE_ATTRIBUTE(ATTRID_VALUE, "Value", "Default Value", VALTYPE_STRING);
	CREATE_ATTRIBUTE(ATTRID_ATTVALTYPE, "ValType", "Value Type", VALTYPE_LONG);
	CREATE_ATTRIBUTE(ATTRID_VIEWABLE, "Viewable", "Viewable", VALTYPE_LONG);
	CREATE_COLLECTION(ATTRID_ENUMITEMS_COLL, "EnumItems", "Enum Items");
	CLSID_PUSH(__uuidof(MgaMetaAttribute));

	CREATE_OBJECT(METAID_METAATTRLINK, "MetaAttrLink", "Meta Attribute Link");
	DECLARE_COMMON();
	CREATE_POINTER(ATTRID_ATTRLINK_USEDIN_PTR, "AttrUsedIn", "Used In");
	CREATE_POINTER(ATTRID_ATTRLINK_ATTR_PTR, "Attribute", "Attribute");

// ------- PointerSpec

#define DECLARE_POINTERSPEC() \
	CREATE_ATTRIBUTE(ATTRID_PTRSPECNAME, "SpecName", "Pointer Spec Name", VALTYPE_STRING); \
	CREATE_COLLECTION(ATTRID_PTRITEMS_COLL, "Items", "Pointer Items");

	CREATE_OBJECT(METAID_METAPOINTERSPEC, "MetaPointerSpec", "Meta Pointer Specification");
	DECLARE_COMMON();
	DECLARE_POINTERSPEC();
	CREATE_POINTER(ATTRID_PTRSPECS_COLL, "SpecParent", "Parent");
	CLSID_PUSH(__uuidof(MgaMetaPointerSpec));

// ------- PointerItem

	CREATE_OBJECT(METAID_METAPOINTERITEM, "MetaPointerItem", "Meta Pointer Item");
	DECLARE_COMMON();
	CREATE_POINTER(ATTRID_PTRITEMS_COLL, "ItemParent", "Parent");
	CREATE_ATTRIBUTE(ATTRID_PTRITEMDESC, "Desc", "Description", VALTYPE_STRING);
	CLSID_PUSH(__uuidof(MgaMetaPointerItem));

// ------- Reference

	CREATE_OBJECT(METAID_METAREFERENCE, "MetaReference", "Meta Reference");
	DECLARE_FCO();
	DECLARE_POINTERSPEC();
	CLSID_PUSH(__uuidof(MgaMetaReference));
	CLSID_PUSH(__uuidof(MgaMetaPointerSpec));

// ------- Set

	CREATE_OBJECT(METAID_METASET, "MetaSet", "Meta Set");
	DECLARE_FCO();
	DECLARE_POINTERSPEC();
	CLSID_PUSH(__uuidof(MgaMetaSet));
	CLSID_PUSH(__uuidof(MgaMetaPointerSpec));

// ------- Connection

	CREATE_OBJECT(METAID_METACONNECTION, "MetaConnection", "Meta Connection");
	DECLARE_FCO();
	CREATE_COLLECTION(ATTRID_CONNJOINTS_COLL, "Joints", "Connection Joints");
	CLSID_PUSH(__uuidof(MgaMetaConnection));

// ------- ConnJoint

	CREATE_OBJECT(METAID_METACONNJOINT, "MetaConnJoint", "Meta Connection Joint");
	DECLARE_COMMON();
	CREATE_POINTER(ATTRID_CONNJOINTS_COLL, "JointParent", "Parent");
	CREATE_COLLECTION(ATTRID_PTRSPECS_COLL, "Specs", "Pointer Specifications");
	CLSID_PUSH(__uuidof(MgaMetaConnJoint));

// ------- RegNode

	CREATE_OBJECT(METAID_METAREGNODE, "MetaRegNode", "Meta Registry Node");
	DECLARE_COMMON();
	CREATE_ATTRIBUTE(ATTRID_NAME, "Name", "Name", VALTYPE_STRING);
	CREATE_ATTRIBUTE(ATTRID_VALUE, "Value", "Value", VALTYPE_STRING);
	CREATE_POINTER(ATTRID_REGNODES_COLL, "RegNodeParent", "Parent");
	CREATE_COLLECTION(ATTRID_REGNODES_COLL, "RegNodes", "Registry Nodes");
	CLSID_PUSH(__uuidof(MgaMetaRegNode));

// ------- EnumItem

	CREATE_OBJECT(METAID_METAENUMITEM, "MetaEnumItem", "Meta Registry Node");
	DECLARE_COMMON();
	CREATE_ATTRIBUTE(ATTRID_NAME, "Name", "Displayed Name", VALTYPE_STRING);
	CREATE_ATTRIBUTE(ATTRID_VALUE, "Value", "Value", VALTYPE_STRING);
	CREATE_POINTER(ATTRID_ENUMITEMS_COLL, "EnumItemParent", "Parent");
	CLSID_PUSH(__uuidof(MgaMetaEnumItem));

// ------- Constraint

	CREATE_OBJECT(METAID_METACONSTRAINT, "MetaConstraint", "Meta Constraint");
	DECLARE_COMMON();
	CREATE_POINTER(ATTRID_CONSTRAINT_PTR, "ConstraintParent", "Constraint Parent");
	CREATE_ATTRIBUTE(ATTRID_NAME, "Name", "Name", VALTYPE_STRING);
	CREATE_ATTRIBUTE(ATTRID_DISPNAME, "DispName", "Displayed Name", VALTYPE_STRING);
	CREATE_ATTRIBUTE(ATTRID_CONSEXPR, "Expression", "Constraint Expression", VALTYPE_STRING);
	CREATE_ATTRIBUTE(ATTRID_CONSMASK, "EventMask", "Constraint Event Mask", VALTYPE_LONG);
	CREATE_ATTRIBUTE(ATTRID_CONSDEPTH, "Depth", "Constraint Depth", VALTYPE_LONG);
	CREATE_ATTRIBUTE(ATTRID_CONSPRIORITY, "Priority", "Constraint Priority", VALTYPE_LONG);
	CREATE_ATTRIBUTE(ATTRID_CONSTYPE, "Type", "Constraint Type", VALTYPE_LONG);
	CREATE_ATTRIBUTE(ATTRID_CONSNAMESPC, "DesiredNamespace", "DesiredNamespace", VALTYPE_STRING);
	CLSID_PUSH(__uuidof(MgaMetaConstraint));

}
