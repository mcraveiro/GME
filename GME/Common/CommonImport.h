// Header file for missing bits from MIDL-generated .hs when using #import
#include "comdef.h"

typedef short metaid_type;
typedef long objid_type;
typedef short attrid_type;
typedef unsigned char valtype_type;
typedef unsigned char locking_type;
typedef short lockval_type;
typedef VARIANT guid_type;

typedef long metaref_type;

typedef BSTR ID_type;


// From InterfaceColl.h

template<class COLL>
struct TypeName_MgaColl2Elem {
	typedef IUnknown element_type;
	typedef IUnknown collection_type;
};
template<class ELEM>
struct TypeName_MgaElem2Coll {
	typedef IUnknown element_type;
	typedef IUnknown collection_type;
};


#define _MGA_COLLECTION_(NAME) _MGA_COLLECTION2_(NAME, NAME##s)
#define _MGA_COLLECTION2_(IMgaX, IMgaXs) \
struct IMgaX; struct IMgaXs; \
template<> struct ::TypeName_MgaColl2Elem<IMgaXs> \
  { typedef IMgaXs collection_type; typedef IMgaX element_type; }; \
template<> struct ::TypeName_MgaElem2Coll<IMgaX> \
 { typedef IMgaXs collection_type; typedef IMgaX element_type; };

_MGA_COLLECTION_(ICoreMetaObject)
_MGA_COLLECTION_(ICoreMetaAttribute)
_MGA_COLLECTION_(ICoreObject)
_MGA_COLLECTION_(ICoreAttribute)

_MGA_COLLECTION_(IMgaMetaAspect)
_MGA_COLLECTION_(IMgaMetaRole)
_MGA_COLLECTION_(IMgaMetaFCO)
_MGA_COLLECTION_(IMgaMetaAttribute)
_MGA_COLLECTION_(IMgaMetaFolder)
_MGA_COLLECTION_(IMgaMetaPointerSpec)
_MGA_COLLECTION_(IMgaMetaRegNode)
_MGA_COLLECTION_(IMgaConstraint)
_MGA_COLLECTION_(IMgaMetaPart)
_MGA_COLLECTION_(IMgaMetaModel)
_MGA_COLLECTION_(IMgaMetaConnJoint)
_MGA_COLLECTION_(IMgaMetaPointerItem)
_MGA_COLLECTION_(IMgaMetaEnumItem)
_MGA_COLLECTION_(IMgaFolder)
_MGA_COLLECTION_(IMgaFCO)
_MGA_COLLECTION_(IMgaAttribute)
_MGA_COLLECTION_(IMgaPart)
_MGA_COLLECTION_(IMgaConnPoint)
_MGA_COLLECTION_(IMgaRegNode)
_MGA_COLLECTION_(IMgaAddOn)
_MGA_COLLECTION2_(IMgaTerritory, IMgaTerritories)
_MGA_COLLECTION_(IMgaComponent)
_MGA_COLLECTION_(IMgaClient)
_MGA_COLLECTION_(IMgaObject)

#define TYPENAME_COLL2ELEM(COLL) typename ::TypeName_MgaColl2Elem<COLL>::element_type
#define TYPENAME_ELEM2COLL(ELEM) typename ::TypeName_MgaElem2Coll<ELEM>::collection_type


typedef unsigned long feature_code;
const unsigned long F_RESIZABLE	= 1<<0;
const unsigned long	F_MOUSEEVENTS =	1<<1;
const unsigned long	F_HASLABEL =	1<<2;
const unsigned long	F_HASSTATE = 1<<3;
const unsigned long	F_HASPORTS = 1<<4;
const unsigned long	F_ANIMATION = 1<<5;
const unsigned long	F_IMGPATH = 1<<6;
const unsigned long	F_RESIZEAFTERMOD = 1<<7;

const metaid_type METAID_NONE = 0;
const metaid_type METAID_ROOT = 1;

const attrid_type ATTRID_NONE = 0;
const attrid_type ATTRID_LOCK = 1;
const attrid_type ATTRID_NAME = 2;
const attrid_type ATTRID_FATHER = 3;
const attrid_type ATTRID_COLLECTION = 10000;

const objid_type OBJID_NONE = 0;
const objid_type OBJID_ROOT = 1;
const long RELID_BASE_MAX  = 0x7FFFFFF;	//assigned to non-derived ojects
const long RELID_VALUE_MAX = 0x7FFFFFFF;   // automatically assigned to deriveds

typedef GMEInterfaceVersion_enum GMEInterfaceVersion;
typedef long metaref_type;
#define ATTVAL_ENUM _attval_enum

