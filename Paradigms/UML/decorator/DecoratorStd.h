//
// 12/06/2002 - modified by Ed Willink, Thales Research and Technology (abstract classes)
//
#ifndef DECORATOR_STD_H
#define DECORATOR_STD_H


typedef enum { GME_LEFT_SIDE, GME_TOP_SIDE, GME_RIGHT_SIDE, GME_BOTTOM_SIDE, GME_SIDE_NUM } GMESizeCode;
typedef enum { GME_RIGHT_DIRECTION = 0, GME_LEFT_DIRECTION, GME_DOWN_DIRECTION, GME_UP_DIRECTION, GME_DIRECTION_NUM } GMEDirectionCode;
typedef enum { GME_NAME_FONT = 0, GME_PORTNAME_FONT, GME_CONNLABEL_FONT, GME_ABSTRACT_FONT, GME_FONT_KIND_NUM } GMEFontKind;
typedef enum { DSAtom, DSSet, DSNullRef, DSRefToNull, DSAtomPort, DSModelPort, DSRefPort, DSSetPort, DSBMPNum, DSModel, DSModelInstance, DSModelRef } DefaultShape;

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

#define GME_NORTH					0			// Must match PopupPropDlg radiobutton stuff!
#define GME_NORTHEAST				1
#define GME_EAST					2
#define GME_SOUTHEAST				3
#define GME_SOUTH					4
#define GME_SOUTHWEST				5
#define GME_WEST					6
#define GME_NORTHWEST				7
#define GME_CENTER					8
#define GME_NAMELOC_NUM				9

#define GME_NAME_DISPLAY_LNG		64

#define COLOR_PREF					"color"
#define NAME_COLOR_PREF				"nameColor"

#define  UML_STEREOTYPE_ATTR		"Stereotype"
#define  UML_ABSTRACT_ATTR			"IsAbstract"
#define  UML_ATTRIBUTES_ATTR		"Attributes"
#define  UML_ATTRIBUTE_SEP			" : "
#define  UML_INHERITANCE_NAME		"Inheritance"
#define  UML_INHERITANCE_PATTERN_NAME		"InheritancePattern"
#define  UML_CONSTRAINT_NAME		"Constraint"
#define  UML_CONSTRAINT_DEFINITION_NAME "ConstraintDefinition"
#define  UML_CONNECTOR_NAME			"Connector"
#define  UML_CONNECTOR_PATTERN_NAME			"ConnectorPattern"

#define  UML_DECORATOR_MARGINX		6
#define  UML_DECORATOR_MARGINY		4
#define  UML_DECORATOR_GAPY			0
#define  UML_DECORATOR_MINATTRSIZE	13

#define  UML_INHERITANCE_WIDTH		24
#define  UML_INHERITANCE_HEIGHT		(0.866 * UML_INHERITANCE_WIDTH)
#define  UML_CONNECTOR_WIDTH		6
#define  UML_CONNECTOR_HEIGHT		UML_CONNECTOR_WIDTH
#define  UML_CONSTRAINT_THICKNESS	1.0/9.0
#define  UML_CONSTRAINT_WIDTH		31
#define  UML_CONSTRAINT_HEIGHT		UML_CONSTRAINT_WIDTH

#define  UML_GREY_COLOR				RGB(0xc0, 0xc0, 0xc0)
#define  UML_WHITE_COLOR			RGB(0xFF, 0xFF, 0xFF)
#define  UML_TRANSPARENT_COLOR		RGB(0xFF, 0xFF, 0xFF)


#endif	//DECORATOR_STD_H