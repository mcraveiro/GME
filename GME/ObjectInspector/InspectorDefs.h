#ifndef INSPECTOR_DEFS_H
#define INSPECTOR_DEFS_H


// Comment this out if you do not want to see the messages in the debug window
#define DEBUG_EVENTS


#ifdef DEBUG_EVENTS
 #define EVENT_TRACE(x) TRACE(x)
#else
 #define EVENT_TRACE(x)
#endif 



#define INSP_INITIAL_SIZEX	200	
#define INSP_INITIAL_SIZEY	320
#define INSP_MIN_SIZEX		80	
#define INSP_MIN_SIZEY		100

#define INSP_LEFT_MARGIN		16	
#define INSP_DIVIDER_POSITION	120
#define INSP_MOUSE_RADIUS		1
#define INSP_PLUS_MINUS_BUTTON_SIZE		9


#define INSP_ENTRY_FONT_FACE	_T("MS Sans Serif")
#define INSP_ENTRY_FONT_PTSIZE	80

typedef enum { INSP_ATTR_PANEL=0, INSP_PREF_PANEL, INSP_PROP_PANEL, INSP_PANEL_NUM } InspPanelID;


#define INSP_COLORBOX_SIZE				13
#define INSP_COLORBOX_MARGIN			 2

#define AUTOROUTER_SOURCE		0x01
#define AUTOROUTER_DESTINATION	0x02
#define AUTOROUTER_STATE_CONN	0x03
#define AUTOROUTER_STATE_MODEL	0x04


// Resource IDs
#define IDC_ARROW_BUTTON				301
#define IDC_COMBO_LISTBOX				302
#define IDC_EDITBOX_MULTILINE			303
#define IDC_EDITBOX_SINGLELINE			304
#define IDC_EDITOR_BUTTON				305

// Hoover List box message
#define HLB_SELENDOK     WM_APP + 1020 

// InPlace Edit message
#define MSG_EDIT_END_OK	 WM_APP + 1021


// Inspector List Messages
#define LBN_ON_ITEM_CHANGED	 WM_APP + 1022
#define LBN_ON_OPEN_REFERED	 WM_APP + 1025


// NameEdit Messages
#define MSG_NAME_EDIT_END_OK		 WM_APP + 1023
#define MSG_NAME_EDIT_END_CANCEL	 WM_APP + 1024

// Contains a number 
#define ATTRIBUTE_MULTILINE_PREFERENCE_REGISTRY_PATH "multiLine"
#define ATTRIBUTE_HELP_REGISTRY_PATH "help"
#define ATTRIBUTE_CONTENTTYPE_REGISTRY_PATH "content-type"


#endif	//INSPECTOR_DEFS_H