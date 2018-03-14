
#ifndef MGA_COREMETADEFINES_H
#define MGA_COREMETADEFINES_H

// ------- generator

void CreateCoreMetaProject(CComPtr<ICoreMetaProject> &project);

// ------- VALTYPE

#define VALTYPE_METAREF					VALTYPE_LONG
#define VALTYPE_DATE					VALTYPE_STRING

// ------- MetaID

#define METAID_METAPROJECT				METAID_ROOT

// check the enum in the idl file
#define METAID_METABASE					200

#define METAID_METAMODEL				METAID_METABASE + OBJTYPE_MODEL
#define METAID_METAATOM					METAID_METABASE + OBJTYPE_ATOM
#define METAID_METAREFERENCE			METAID_METABASE + OBJTYPE_REFERENCE
#define METAID_METACONNECTION			METAID_METABASE + OBJTYPE_CONNECTION
#define METAID_METASET					METAID_METABASE + OBJTYPE_SET
#define METAID_METAFOLDER				METAID_METABASE + OBJTYPE_FOLDER
#define METAID_METAASPECT				METAID_METABASE + OBJTYPE_ASPECT
#define METAID_METAROLE					METAID_METABASE + OBJTYPE_ROLE
#define METAID_METAATTRIBUTE			METAID_METABASE + OBJTYPE_ATTRIBUTE
#define METAID_METAPART					METAID_METABASE + OBJTYPE_PART

#define METAID_METACONNJOINT			221
#define METAID_METAPOINTERSPEC			222
#define METAID_METAPOINTERITEM			223
#define METAID_METAFOLDERLINK			224
#define METAID_METAROOTOBJLINK			225
#define METAID_METAATTRLINK				226
#define METAID_METAREGNODE				227
#define METAID_METAENUMITEM				228
#define METAID_METACONSTRAINT			229

// ------- AttrID

//		ATTRID_NAME
//		ATTRID_LOCK
#define ATTRID_PARNAME					201
#define ATTRID_PARDISPNAME				202
#define ATTRID_GUID						203
#define ATTRID_VERSION					204
#define ATTRID_AUTHOR					205
#define ATTRID_COMMENT					206
#define ATTRID_CDATE					207
#define ATTRID_MDATE					208

#define ATTRID_METAREF					210
#define ATTRID_DISPNAME					211
#define ATTRID_CONSTRAINT_PTR			212
#define ATTRID_ALIASENABLED				213


#define ATTRID_DEFFOLDER_PTR			220
#define ATTRID_DEFFCO_PTR				221
#define ATTRID_DEFATTR_PTR				222

#define ATTRID_FOLDERLINK_PARENT_PTR	231
#define ATTRID_FOLDERLINK_CHILD_PTR		232
#define ATTRID_ROOTOBJLINK_FOLDER_PTR	233
#define ATTRID_ROOTOBJLINK_OBJECT_PTR	234
#define ATTRID_ATTRLINK_USEDIN_PTR		235
#define ATTRID_ATTRLINK_ATTR_PTR		236

#define ATTRID_KIND_PTR					241
#define ATTRID_ATTRIBUTES_COLL			242

#define ATTRID_ROLES_COLL				251
#define ATTRID_ASPECTS_COLL				252

#define ATTRID_PARTROLE_PTR				261
#define ATTRID_PARTASPECT_PTR			262
#define ATTRID_KINDASPECT				263

#define ATTRID_PARTDATA					271
#define ATTRID_VALUE					272
#define ATTRID_ATTVALTYPE				273
#define ATTRID_VIEWABLE					274

#define ATTRID_PTRSPECNAME				281
#define ATTRID_PTRITEMS_COLL			282
#define ATTRID_PTRITEMDESC				283
#define ATTRID_KINDNAME					284
#define ATTRID_ROLENAMES				285

#define ATTRID_CONNJOINTS_COLL			291
#define ATTRID_PTRSPECS_COLL			292
#define ATTRID_REGNODES_COLL			293
#define ATTRID_ENUMITEMS_COLL			294

#define ATTRID_CONSEXPR					300
#define ATTRID_CONSMASK					301
#define ATTRID_CONSDEPTH				302
#define ATTRID_CONSPRIORITY				303
#define ATTRID_CONSTYPE					304
#define ATTRID_CONSNAMESPC				305

#endif//MGA_COREMETADEFINES_H
