#ifndef _GMESTD_INCLUDED_
#define _GMESTD_INCLUDED_

#include "afxcoll.h"
#include "afxtempl.h"

#include "meta.h"
#define IMgaContainer IMgaObject
#include "mga.h"
#include "..\Annotator\AnnotationDefs.h"
#include "..\..\SDK\Decoratorlib\DecoratorDefs.h"
#include "CommonSmart.h"

/*
	ATTENTION !

	There is a similar file in Annotator, MgaDecorators, SDK/DecoratorLib (DecoratorStd.h) and in GMEView (GMEViewStd.h)
	Modify/extend this file carefully !
*/
typedef enum { GME_EDIT_MODE, GME_AUTOCONNECT_MODE, GME_DISCONNECT_MODE, GME_SET_MODE, GME_ZOOM_MODE, GME_VISUAL_MODE, GME_SHORTAUTOCONNECT_MODE, GME_SHORTDISCONNECT_MODE } GMEModeCode;
typedef enum { GME_CMD_CONTEXT } GMECommandCode;
typedef enum { GME_LEFT_SIDE, GME_TOP_SIDE, GME_RIGHT_SIDE, GME_BOTTOM_SIDE, GME_SIDE_NUM } GMESizeCode;
typedef enum { GME_RIGHT_DIRECTION = 0, GME_LEFT_DIRECTION, GME_DOWN_DIRECTION, GME_UP_DIRECTION, GME_DIRECTION_NUM } GMEDirectionCode;
typedef enum { GME_NAME_FONT = 0, GME_PORTNAME_FONT, GME_CONNLABEL_FONT, GME_FONT_KIND_NUM } GMEFontKind;
typedef enum { GME_LINE_SOLID = 0, GME_LINE_DASH, GME_LINE_CUSTOMIZED } GMEConnLineType;

#define	GME_BACKGROUND_COLOR		RGB(0xff, 0xff, 0xff)
#define	GME_BORDER_COLOR			RGB(0x00, 0x00, 0x00)
#define	GME_MODEL_COLOR				RGB(0xc0, 0xc0, 0xc0)
#define	GME_NAME_COLOR				RGB(0x00, 0x00, 0x00)
#define	GME_PORTNAME_COLOR			RGB(0x60, 0x60, 0x60)
#define	GME_CONNECTION_COLOR 		RGB(0x00, 0x00, 0x00)
#define	GME_GREYEDOUT_COLOR 		RGB(0xd0, 0xd0, 0xd0)
#define	GME_GRID_COLOR				RGB(0xd0, 0xd0, 0xd0)
#define	GME_BLACK_COLOR				RGB(0x00, 0x00, 0x00)
#define	GME_WHITE_COLOR				RGB(0xff, 0xff, 0xff)
#define	GME_RED_COLOR				RGB(0xff, 0x00, 0x00)
#define	GME_DARKRED_COLOR			RGB(0x80, 0x00, 0x00)
#define	GME_GREY_COLOR				RGB(0xc0, 0xc0, 0xc0)
#define GME_GRAYED_OUT_COLOR		RGB(0xde, 0xde, 0xde)

#define GME_GRID_SIZE				7
#define GME_PORT_SIZE				11
#define GME_PORT_OFFSET				3
#define GME_PORT_BORDER_OFFSET		10
#define GME_GRID_BORDER_OFFSET		3
#define GME_MODEL_WIDTH				113
#define GME_MODEL_HEIGHT			70
#define GME_3D_BORDER_SIZE			3

#define GRIDBITS
#ifdef GRIDBITS
#define GME_MAX_GRID_DIM			4*2048
#else
#define GME_MAX_GRID_DIM			1024
#endif

#define GME_BUTT_END				0
#define GME_ARROW_END				1
#define GME_DIAMOND_END				2
#define GME_APEX_END				3
#define GME_BULLET_END				4
#define GME_EMPTYDIAMOND_END		5
#define GME_EMPTYAPEX_END			6
#define GME_EMPTYBULLET_END			7
#define GME_HALFARROWLEFT_END       8
#define GME_HALFARROWRIGHT_END      9


#define GME_HOTSPOT_RADIUS			10
#define GME_HOTSPOT_VISUAL_RADIUS	3
#define GME_CONNSELECT_WIDTH		2


#define GME_DYN_MENU_MINID			0x1001
#define DECORATOR_CTX_MENU_MINID	0x4001
#define DECORATOR_CTX_MENU_MAXID	0x4101

//#define GME_MAX_ZOOM_PERCENT		300
//#define GME_MIN_ZOOM_PERCENT		10

#define EXTENT_ERROR_CORR	1.1 // ?? masks an error of extent calculation

#define ZOOM_WIDTH	-1
#define ZOOM_HEIGHT	-2
#define ZOOM_ALL	-3
#define ZOOM_MIN	5
#define ZOOM_MAX	500
#define ZOOM_NO		100

//#define GME_ZOOM_LEVEL_0			GME_MIN_ZOOM_PERCENT
//#define GME_ZOOM_LEVEL_1			25
//#define GME_ZOOM_LEVEL_2			50
//#define GME_ZOOM_LEVEL_3			75
//#define GME_ZOOM_LEVEL_4			100
//#define GME_ZOOM_LEVEL_5			150
//#define GME_ZOOM_LEVEL_6			200
//#define GME_ZOOM_LEVEL_7			GME_MAX_ZOOM_PERCENT
//#define GME_ZOOM_LEVEL_NUM			8
//#define GME_ZOOM_LEVEL_MED			4

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

#define GME_START_NORTH				0			// Must match AutoRouter preferences stuff
#define GME_START_EAST				1
#define GME_START_SOUTH				2
#define GME_START_WEST				3
#define GME_END_NORTH				4
#define GME_END_EAST				5
#define GME_END_SOUTH				6
#define GME_END_WEST				7
#define GME_AR_NUM					8

#define GME_ANIM_EVENT_ID			74
#define	GME_ANIM_INTERVAL			250			// in milliseconds

#define GME_AUTOSAVE_EVENT_ID		75


#define GME_NAME_DISPLAY_LNG		64

#define HELP_PREF					"help"
#define COLOR_PREF					"color"
#define CONN_SRC_END_STYLE_PREF		"srcStyle"
#define CONN_DST_END_STYLE_PREF		"dstStyle"
#define CONN_LINE_TYPE_PREF			"lineType"
#define MODEL_BACKGROUND_COLOR_PREF	"backgroundColor"
#define MODEL_BORDER_COLOR_PREF		"borderColor"
#define PORT_NAME_COLOR_PREF		"portColor"
#define NAME_COLOR_PREF				"nameColor"
#define CONN_LABEL_FORMATSTR_PREF	"labelFormatStr"
#define CONN_SRC_LABEL1_PREF		"srcLabel1"
#define CONN_SRC_LABEL2_PREF		"srcLabel2"
#define CONN_DST_LABEL1_PREF		"dstLabel1"
#define CONN_DST_LABEL2_PREF		"dstLabel2"
#define NAME_POS_PREF				"namePosition"
#define AUTOROUTER_PREF				"autorouterPref"
#define ICON_PREF					"icon"
#define PORT_ICON_PREF				"porticon"
#define DECORATOR_PREF				"decorator"
#define HOTSPOT_PREF				"isHotspotEnabled"
#define HOTSPOT_FALSE				"false"
#define CUSTOMCONNECTIONDATA		"customConnectionData"
#define CONNECTIONAUTOROUTING		"isAutoRouted"
#define MODELAUTOROUTING			"isModelAutoRouted"

#define GME_AUTOSAVE_EXTENSION		".bak"


#define	GME_CONN_SRC_LABEL1			0
#define	GME_CONN_SRC_LABEL2			1
#define	GME_CONN_DST_LABEL1			2
#define	GME_CONN_DST_LABEL2			3
#define GME_CONN_MAIN_LABEL			4
#define GME_CONN_LABEL_NUM			5

#define GME_DEFAULT_DECORATOR		"MGA.BoxDecorator"

class CGuiMetaProject;
class CGuiMetaBase;
class CGuiMetaFolder;
class CGuiMetaAttribute;
class CGuiMetaFco;
class CGuiMetaModel;
class CGuiMetaAspect;

class CGuiBase;
class CGuiAspect;
class CGuiAnnotator;
class CGuiFco;
class CGuiConnection;
class CGuiConnectionLabel;
class CGuiConnectionLabelSet;
class CGuiObject;
class CGuiCompound;
class CGuiModel;
class CGuiCompoundReference;
class CGuiSimpleReference;
class CGuiPort;

class CDynMenu;
class CDynMenuItem;

class CGuiDepCtrlDesc;
class CGuiPreference;
class CGuiPreferenceSet;

class CGMEView;

class CPendingRequest;

typedef CList<CComObjPtr<IMgaFCO>, CComObjPtr<IMgaFCO> >				CMgaFcoList;
typedef CList<CComObjPtr<IMgaAttribute>, CComObjPtr<IMgaAttribute> >	CMgaAttributeList;

typedef CTypedPtrMap<CMapPtrToPtr, void *, CGuiMetaBase *>				CGuiMetaBaseTable;
typedef CTypedPtrList<CPtrList, CGuiMetaBase *>							CGuiMetaBaseList;

typedef CTypedPtrList<CPtrList, CGMEView*>								CViewList;

typedef CTypedPtrList<CPtrList, CGuiMetaAspect*>						CGuiMetaAspectList;
typedef CTypedPtrList<CPtrList, CGuiMetaAttribute*>						CGuiMetaAttributeList;
typedef CTypedPtrList<CPtrList, CGuiMetaFco*>							CGuiMetaFcoList;

typedef CTypedPtrList<CPtrList, CGuiFco*> 								CGuiFcoList;
typedef CTypedPtrList<CPtrList, CGuiObject*> 							CGuiObjectList;
typedef CTypedPtrList<CPtrList, CGuiAspect*>							CGuiAspectList;
typedef CTypedPtrList<CPtrList, CGuiPort*> 								CGuiPortList;
typedef CTypedPtrList<CPtrList, CGuiAnnotator*>							CGuiAnnotatorList;
typedef CTypedPtrList<CPtrList, CGuiConnection*> 						CGuiConnectionList;

typedef CTypedPtrList<CPtrList, CGuiDepCtrlDesc*>	 					CGuiDepCtrlDescList;

typedef CTypedPtrMap<CMapStringToPtr, CString, CGuiPreference *>		CGuiPreferenceTable;
typedef CTypedPtrList<CPtrList, CGuiPreference*>	 					CGuiPreferenceList;

typedef CTypedPtrList<CPtrList, CRect *>								CRectList;
typedef CTypedPtrList<CPtrList, CButton *>								CButtonList;
typedef CTypedPtrList<CPtrList, CFont *>								CFontList;

typedef CTypedPtrList<CPtrList, CPendingRequest *>						CPendingRequestList;

typedef CList<CPoint, CPoint&>											CPointList;

typedef CTypedPtrMap<CMapPtrToPtr, void *, CPen *>						CPenTable;
typedef CTypedPtrMap<CMapPtrToPtr, void *, CBrush *>					CBrushTable;

typedef CList<bool, bool &>												CBoolList;

#define chSTR(x) #x
#define chSTR2(x) chSTR(x)

#endif // whole file
