#ifndef DECORATOR_STD_H
#define DECORATOR_STD_H


#define	GME_BACKGROUND_COLOR		RGB(0xff, 0xff, 0xff)
#define	GME_BORDER_COLOR			RGB(0x00, 0x00, 0x00)
#define	GME_MODEL_COLOR				RGB(0xc0, 0xc0, 0xc0)
#define	GME_NAME_COLOR				RGB(0x00, 0x00, 0x00)
#define	GME_PORTNAME_COLOR			RGB(0x60, 0x60, 0x60)
#define	GME_CONNECTION_COLOR 		RGB(0x00, 0x00, 0x00)
#define	GME_GRID_COLOR				RGB(0xd0, 0xd0, 0xd0)
#define	GME_BLACK_COLOR				RGB(0x00, 0x00, 0x00)
#define	GME_WHITE_COLOR				RGB(0xff, 0xff, 0xff)
#define	GME_GREY_COLOR				RGB(0xc0, 0xc0, 0xc0)
#define GME_GRAYED_OUT_COLOR		RGB(0xde, 0xde, 0xde)

#define GME_GRID_SIZE				7
#define GME_PORT_SIZE				11
#define GME_PORT_OFFSET				3
#define GME_PORT_BORDER_OFFSET		10
#define GME_GRID_BORDER_OFFSET		3
#define GME_MODEL_WIDTH				113
#define GME_LEFTPORT_MARGIN			100
#define GME_MODEL_HEIGHT			70
//#define GME_3D_BORDER_SIZE			3

#define HELP_PREF					_T("help")
#define COLOR_PREF					_T("color")
#define CONN_LINE_TYPE_PREF			_T("lineType")
#define MODEL_BACKGROUND_COLOR_PREF	_T("backgroundColor")
#define MODEL_BORDER_COLOR_PREF		_T("borderColor")
#define PORT_NAME_COLOR_PREF		_T("portColor")
#define NAME_COLOR_PREF				_T("nameColor")
#define NAME_POS_PREF				_T("namePosition")
#define ICON_PREF					_T("icon")
#define PORTICON_PREF				_T("porticon")

#define	 META_DEFAULT_SHOWABSTRACT		true
#define	 META_DEFAULT_SHOWATTRIBUTES	true

#define  META_ATTRIBUTE_SEP			_T(" : ")

#define	 META_DECORATOR_MARGINX		6
#define	 META_DECORATOR_MARGINY		4
#define	 META_DECORATOR_GAPY		0
#define  META_DECORATOR_MINATTRSIZE	13
#define  META_INHERITANCE_WIDTH		21
									// sin 60
#define  META_INHERITANCE_RATIO		0.86602540378443864676372317075294
#define	 META_INHERITANCE_HEIGHT	(META_INHERITANCE_RATIO * META_INHERITANCE_WIDTH)
#define	 META_INHERITANCE_RADIUS	(META_INHERITANCE_HEIGHT / 4)
#define  META_EQUIVALENCE_WIDTH		(2 * META_INHERITANCE_HEIGHT)
#define	 META_EQUIVALENCE_HEIGHT	META_INHERITANCE_WIDTH
#define	 META_CONNECTOR_RAIDUS		4
#define  META_CONSTRAINT_THICKNESS	1.0/9.0
#define  META_CONSTRAINT_WIDTH		31
#define  META_CONSTRAINT_HEIGHT		META_CONSTRAINT_WIDTH

#define	 META_CLASS_BGCOLOR			RGB(0xFF,0xFF,0xFF)
#define	 META_TRANSPARENT_COLOR		RGB(0xFF,0xFF,0xFF)

typedef enum {NULLSHAPE, CLASS, CLASSPROXY, CONNECTOR, CONSTRAINT, EQUIVALENCE, INHERITANCE, INTINHERITANCE, IMPINHERITANCE, CONSTRAINTFUNC } ShapeCode;

#define  META_ABSTRACT_ATTR			_T("IsAbstract")

#define	 META_ATOM_KIND				_T("Atom")
#define	 META_ATOMPROXY_KIND		_T("AtomProxy")
#define	 META_MODEL_KIND			_T("Model")
#define	 META_MODELPROXY_KIND		_T("ModelProxy")
#define	 META_REFERENCE_KIND		_T("Reference")
#define	 META_REFERENCEPROXY_KIND	_T("ReferenceProxy")
#define	 META_SET_KIND				_T("Set")
#define	 META_SETPROXY_KIND			_T("SetProxy")
#define	 META_CONNECTION_KIND		_T("Connection")
#define	 META_CONNECTIONPROXY_KIND	_T("ConnectionProxy")
#define	 META_FCO_KIND				_T("FCO")
#define	 META_FCOPROXY_KIND			_T("FCOProxy")

#define	 META_FOLDER_KIND			_T("Folder")
#define	 META_FOLDERPROXY_KIND		_T("FolderProxy")

#define	 META_ASPECT_KIND			_T("Aspect")
#define	 META_ASPECTPROXY_KIND		_T("AspectProxy")

#define	 META_BOOLEANATTR_KIND		_T("BooleanAttribute")
#define	 META_ENUMATTR_KIND			_T("EnumAttribute")
#define	 META_FIELDATTR_KIND		_T("FieldAttribute")

#define	 META_CONSTRAINT_KIND		_T("Constraint")
#define	 META_CONSTRAINTFUNC_KIND	_T("ConstraintFunc")

#define	 META_CONNECTOR_KIND		_T("Connector")

#define	 META_EQUIVALENCE_KIND		_T("Equivalence")
#define	 META_SAMEFOLDER_KIND		_T("SameFolder")
#define	 META_SAMEASPECT_KIND		_T("SameAspect")


#define	 META_INHERITANCE_KIND		_T("Inheritance")
#define	 META_IMPINHERITANCE_KIND	_T("ImplementationInheritance")
#define	 META_INTINHERITANCE_KIND	_T("InterfaceInheritance")

#define  META_BOOLEANATTR_LABEL		_T("bool")
#define  META_ENUMATTR_LABEL		_T("enum")
#define  META_FIELDATTR_LABEL		_T("field")

#define  META_PARAM_SHOWATTRIBUTES	_T("showattributes")
#define  META_PARAM_SHOWABSTRACT	_T("showabstract")
#define  META_PARAM_VAL_TRUE		_T("true")
#define  META_PARAM_VAL_FALSE		_T("false")

#define  E_METADECORATOR_KINDNOTSUPPORTED	0x80737050
#define  E_METADECORATOR_INVALID_PARAMVALUE	0x80737051


#endif	//DECORATOR_STD_H