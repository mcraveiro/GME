// Preference.cpp: implementation of the CPreference class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "objectinspector.h"
#include "ObjectInspectorCtl.h"

#include "Preference.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


// Definitions of static member variables

// LINE_STYLE_LIST (butt,arrow,diamond)
#define LINE_STYLE_LIST_SIZE 10
TCHAR *CPreference::m_szLineStyleList[]={_T("butt"), _T("arrow"), _T("diamond"), _T("apex"), _T("bullet"), _T("empty diamond"), _T("empty apex"), _T("empty bullet"), _T("left half arrow"), _T("right half arrow") };

// LINE_TYPE_LIST (solid,dash)
#define LINE_TYPE_LIST_SIZE 2
TCHAR *CPreference::m_szLineTypeList[]={_T("solid"), _T("dash")};

// BOOLEAN_LIST
#define BOOLEAN_LIST_SIZE 2
TCHAR *CPreference::m_szBooleanList[]={_T("true"), _T("false")};



//----------------------------------- Static  tables ------------------------------------

// Connection Preferences
/**** Type ************* Path In Registry ** Default Value ** Name to display ************ Help Text *******************************************************************************************************************************************************/
TCHAR *CPreference::m_szConnectionPreferenceTable[][5]=
{
	{_T("COLOR"),				_T("color"),			_T("0x000000"),		_T("Color"),								_T("Connection line color. Set this value to change the connection line color.")},
	{_T("COLOR"),				_T("nameColor"),		_T("0x000000"),		_T("Name color"),							_T("Connection label color. Set this value to change the connection label color.")},
	{_T("STRING"),				_T("help"),				_T(""),				_T("Help URL"),								_T("Sets this value to specify the URL containig the help information belonging to the connection.")},
	{_T("MULTISTRING"),		    _T("description"),		_T(""),				_T("Description"),							_T("Free format description of the model element for documentation purposes.")},
	{_T("LINE_STYLE_LIST"),		_T("srcStyle"),			_T("butt"),			_T("Source end style"),						_T("Sets this value to specify the source end style of the connection. Eg. Arrow.")},
	{_T("LINE_STYLE_LIST"),		_T("dstStyle"),			_T("butt"),			_T("Destination end style"),				_T("Sets this value to specify the destination end style of the connection. Eg. Arrow.")},
	{_T("LINE_TYPE_LIST"),		_T("lineType"),			_T("solid"),		_T("Line type"),							_T("Sets this value to specify the connection line type. Eg. Dashed.")},
	{_T("STRING"),				_T("labelFormatStr"),	_T(""),				_T("Label format string"),					_T("Sets this value to form a centered connection label from the connection attributes. Eg. %name%")},
	{_T("STRING"),				_T("srcLabel1"),		_T("<blank>"),		_T("Source primary attribute"),				_T("Displays an attribute as a label on the source side of the connection.")},
	{_T("STRING"),				_T("srcLabel2"),		_T("<blank>"),		_T("Source secondary attribute"),			_T("Displays an attribute as a label on the source side of the connection.")},
	{_T("STRING"),				_T("dstLabel1"),		_T("<blank>"),		_T("Destination primary attribute"),		_T("Displays an attribute as a label on the destination side of the connection.")},
	{_T("STRING"),				_T("dstLabel2"),		_T("<blank>"),		_T("Destination secondary attribute"),		_T("Displays an attribute as a label on the destination side of the connection.")},
	{_T("BOOLEAN_LIST"),		_T("isAutoRouted"),		_T("true"),			_T("Is auto routed"),						_T("Is the connection treated by the auto router.")}
};


// Atom Preferences
/**** Type ************* Path In Registry ** Default Value ** Name to display ************ Help Text *******************************************************************************************************************************************************/
TCHAR* CPreference::m_szAtomPreferenceTable[][5]=
{
	{_T("COLOR"),			_T("color"),			_T("0x000000"),		_T("Color"),						_T("Atom color. Set this value to change the atom line color.")},
	{_T("COLOR"),			_T("nameColor"),		_T("0x000000"),		_T("Name color"),					_T("Atom name label color. Set this value to change the atom name label color.")},
	{_T("COLOR"),			_T("fillColor"),		_T("0xFFFFFF"),		_T("Fill color"),					_T("Atom background fill color for the closed models.")},
	{_T("BOOLEAN_LIST"),	_T("gradientFill"),		_T("false"),		_T("Gradient fill enabled"),		_T("Is the gradient fill feature enabled.")},
	{_T("COLOR"),			_T("gradientColor"),	_T("0xc0c0c0"),		_T("Gradient (2nd fill) color"),	_T("Atom gradient background color (the other fill color).")},
	{_T("STRING"),			_T("gradientDirection"),_T("0"),			_T("Gradient direction (angle)"),	_T("Direction of the gradient fill.")},
	{_T("BOOLEAN_LIST"),	_T("itemShadowCast"),	_T("false"),		_T("Shadow casting enabled"),		_T("Is the shadow casting feature enabled.")},
	{_T("COLOR"),			_T("shadowColor"),		_T("0xc0c0c0"),		_T("Shadow color"),					_T("Atom shadow color.")},
	{_T("STRING"),			_T("shadowThickness"),	_T("9"),			_T("Shadow thickness/offset"),		_T("Thickness/offset of the shadow.")},
	{_T("STRING"),			_T("shadowDirection"),	_T("45"),			_T("Shadow direction (angle)"),		_T("Direction of the shadow (angle).")},
	{_T("BOOLEAN_LIST"),	_T("roundCornerRect"),	_T("false"),		_T("Round rectangle corner enabled"),_T("Is rounding of rectangle corner feature enabled.")},
	{_T("STRING"),			_T("roundCornerRadius"),_T("9"),			_T("Round rectangle corner radius"),_T("Radius of round rectangle corner.")},
	{_T("STRING"),			_T("help"),				_T(""),				_T("Help URL"),						_T("Sets this value to specify the URL containig the help information belonging to the atom.")},
	{_T("MULTISTRING"),		_T("description"),		_T(""),				_T("Description"),					_T("Free format description of the model element for documentation purposes.")},
	{_T("COMPASS_OPTION"),	_T("namePosition"),		_T("4"),			_T("Name Location"),				_T("Sets the name label location relative to the atom icon. Eg. North.")},
	{_T("STRING"),			_T("nameWrap"),			_T("0"),			_T("NameWrap number"),				_T("Sets the number of characters in a line of the name. If it is 0, then wrapping is disabled.")},
	{_T("BOOLEAN_LIST"),	_T("isNameEnabled"),	_T("true"),			_T("Name enabled"),					_T("Displays the name.")},
	{_T("COMPASS_CHECK"),	_T("autorouterPref"),	_T("neswNESW"),		_T("Auto Router Preference"),		_T("Sets the allowed stick point of the connections from and to this atom.")},
	{_T("BOOLEAN_LIST"),	_T("isHotspotEnabled"),	_T("true"),			_T("Hotspots enabled"),				_T("Enables the hotspot feature in connection mode.")},
	{_T("BOOLEAN_LIST"),	_T("isTypeShown"),		_T("false"),		_T("Type displayed"),				_T("Displays the type (subtype) if the atom is an Instance")},
	{_T("STRING"),			_T("decorator"),		_T(""),				_T("Decorator"),					_T("ProgID of an process COM component followed by optional parameters for installing  custom drawing code.\r\n Syntax: <ProgID> [<Param1=Value>...<ParamX=ValueX>]")},
	{_T("STRING"),			_T("icon"),				_T(""),				_T("Icon name"),					_T("Image(.bmp,.gif) file name to display the atom. Icon search paths can be set in File/Settings menu.")},
	{_T("STRING"),			_T("porticon"),			_T(""),				_T("Port icon name"),				_T("Specifies the image(.bmp,.gif) file name to display the atom if it is a port in a model.")},
	{_T("STRING"),			_T("subTypeIcon"),		_T(""),				_T("Subtype icon name"),			_T("Image(.bmp,.gif) file name to display the atom if it a subtype. Icon search paths can be set in File/Settings menu.")},
	{_T("STRING"),			_T("instanceIcon"),		_T(""),				_T("Instance icon name"),			_T("Image(.bmp,.gif) file name to display the atom if it an instance. Icon search paths can be set in File/Settings menu.")}
};


// Model Preferences - item [3][2] is corrected in the constructor
/**** Type ************* Path In Registry ** Default Value ** Name to display ************ Help Text *******************************************************************************************************************************************************/
TCHAR* CPreference::m_szModelPreferenceTable[][5]=
{
	{_T("COLOR"),			_T("color"),			_T("0xc0c0c0"),		_T("Color"),								_T("Model color. Set this value to change the model line color.")},
	{_T("COLOR"),			_T("nameColor"),		_T("0x000000"),		_T("Name color"),							_T("Model name label color. Set this value to change the model name label color.")},
	{_T("COLOR"),			_T("borderColor"),		_T("0x000000"),		_T("Border color"),							_T("Model border color. Set this value to change the model border color.")},
	{_T("COLOR"),			_T("backgroundColor"),	NULL,/*->Const*/_T("Background color"),						_T("Window background color for the open models.")},
	{_T("COLOR"),			_T("fillColor"),		_T("0xFFFFFF"),		_T("Fill color"),							_T("Model background fill color for the closed models.")},
	{_T("BOOLEAN_LIST"),	_T("gradientFill"),		_T("false"),		_T("Gradient fill enabled"),				_T("Is the gradient fill feature enabled.")},
	{_T("COLOR"),			_T("gradientColor"),	_T("0xc0c0c0"),		_T("Gradient (2nd fill) color"),			_T("Model gradient background color for the closed models (the other fill color).")},
	{_T("STRING"),			_T("gradientDirection"),_T("0"),			_T("Gradient direction (angle)"),			_T("Direction of the gradient (angle).")},
	{_T("BOOLEAN_LIST"),	_T("itemShadowCast"),	_T("false"),		_T("Shadow casting enabled"),				_T("Is the shadow casting feature enabled.")},
	{_T("COLOR"),			_T("shadowColor"),		_T("0xc0c0c0"),		_T("Shadow color"),							_T("Model shadow color for the closed models.")},
	{_T("STRING"),			_T("shadowThickness"),	_T("9"),			_T("Shadow thickness/offset"),				_T("Thickness/offset of the shadow.")},
	{_T("STRING"),			_T("shadowDirection"),	_T("45"),			_T("Shadow direction (angle)"),				_T("Direction of the shadow (angle).")},
	{_T("BOOLEAN_LIST"),	_T("roundCornerRect"),	_T("false"),		_T("Round rectangle corner enabled"),		_T("Is rounding of rectangle corner feature enabled.")},
	{_T("STRING"),			_T("roundCornerRadius"),_T("9"),			_T("Round rectangle corner radius"),		_T("Radius of round rectangle corner.")},
	{_T("COLOR"),			_T("portColor"),		_T("0x000000"),		_T("Port name color"),						_T("Port name color for the displayed ports of this model.")},
	{_T("STRING"),			_T("help"),				_T(""),				_T("Help URL"),								_T("Sets this value to specify the URL containig the help information belonging to the model.")},
	{_T("MULTISTRING"),		_T("description"),		_T(""),				_T("Description"),							_T("Free format description of the model element for documentation purposes.")},
	{_T("COMPASS_OPTION"),	_T("namePosition"),		_T("4"),			_T("Name Location"),						_T("Sets the name label location relative to the atom icon. Eg. North.")},
	{_T("STRING"),			_T("nameWrap"),			_T("0"),			_T("NameWrap number"),						_T("Sets the number of characters in a line of the name. If it is 0, then wrapping is disabled.")},
	{_T("BOOLEAN_LIST"),	_T("isNameEnabled"),	_T("true"),			_T("Name enabled"),							_T("Displays the name.")},
	{_T("BOOLEAN_LIST"),	_T("isModelAutoRouted"),_T("true"),			_T("Is auto routed"),						_T("Are the models' connections treated by the auto router by default.")},
	{_T("COMPASS_CHECK"),	_T("autorouterPref"),	_T("neswNESW"),		_T("Auto Router Preference"),				_T("Sets the allowed stick point of the connections from and to this model.")},
	{_T("BOOLEAN_LIST"),	_T("isHotspotEnabled"),	_T("true"),			_T("Hotspots enabled"),						_T("Enables the hotspot feature in connection mode.")},
	{_T("BOOLEAN_LIST"),	_T("isTypeShown"),		_T("false"),		_T("Type displayed"),						_T("Displays the type (subtype) if the model is an Instance")},
	{_T("BOOLEAN_LIST"),	_T("isTypeInfoShown"),	_T("false"),		_T("Typeinfo displayed"),					_T("Displays T, S or I letter according to that the model is Type, Subtype or Instance")},
	{_T("STRING"),			_T("decorator"),		_T(""),				_T("Decorator"),							_T("ProgID of an process COM component followed by optional parameters for installing  custom drawing code.\r\n Syntax: <ProgID> [<Param1=Value>...<ParamX=ValueX>]")},
	{_T("STRING"),			_T("icon"),				_T(""),				_T("Icon name"),							_T("Image(.bmp,.gif) file name to display the model. Icon search paths can be set in File/Settings menu.")},
	{_T("STRING"),			_T("porticon"),			_T(""),				_T("Port icon name"),						_T("Specifies the image(.bmp,.gif) file name to display this model if it is a port in another model.")},
	{_T("STRING"),			_T("subTypeIcon"),		_T(""),				_T("Subtype icon name"),					_T("Image(.bmp,.gif) file name to display the model if it a subtype. Icon search paths can be set in File/Settings menu.")},
	{_T("STRING"),			_T("instanceIcon"),		_T(""),				_T("Instance icon name"),					_T("Image(.bmp,.gif) file name to display the model if it an instance. Icon search paths can be set in File/Settings menu.")},
	{_T("BOOLEAN_LIST"),	_T("portLabelInside"),	_T("true"),			_T("PortLabel shown inside container?"),	_T("Portname label may be shown either inside or outside the container.")},
	{_T("STRING"),			_T("portLabelLength"),	_T("3"),			_T("PortLabel Length"),						_T("Sets the text length shown when displaying portnames. If 0 total length is shown.")}
};


// Reference (Proxy) Preferences
/**** Type ************* Path In Registry ** Default Value ** Name to display ************ Help Text *******************************************************************************************************************************************************/
TCHAR* CPreference::m_szProxyPreferenceTable[][5]=
{
	{_T("COLOR"),			_T("color"),			_T("0xc0c0c0"),		_T("Color"),								_T("Proxy color. Set this value to change the proxy line color.")},
	{_T("COLOR"),			_T("nameColor"),		_T("0x000000"),		_T("Name color"),							_T("Proxy name label color. Set this value to change the proxy name label color.")},
	{_T("COLOR"),			_T("fillColor"),		_T("0xFFFFFF"),		_T("Fill color"),							_T("Proxy background fill color for the closed models.")},
	{_T("BOOLEAN_LIST"),	_T("gradientFill"),		_T("false"),		_T("Gradient fill enabled"),				_T("Is the gradient fill feature enabled.")},
	{_T("COLOR"),			_T("gradientColor"),	_T("0xc0c0c0"),		_T("Gradient (2nd fill) color"),			_T("Proxy gradient background color for the closed models (the other fill color).")},
	{_T("STRING"),			_T("gradientDirection"),_T("0"),			_T("Gradient direction (angle)"),			_T("Direction of the gradient fill.")},
	{_T("BOOLEAN_LIST"),	_T("itemShadowCast"),	_T("false"),		_T("Shadow casting enabled"),				_T("Is the shadow casting feature enabled.")},
	{_T("COLOR"),			_T("shadowColor"),		_T("0xc0c0c0"),		_T("Shadow color"),							_T("Proxy shadow color.")},
	{_T("STRING"),			_T("shadowThickness"),	_T("9"),			_T("Shadow thickness/offset"),				_T("Thickness/offset of the shadow.")},
	{_T("STRING"),			_T("shadowDirection"),	_T("45"),			_T("Shadow direction (angle)"),				_T("Direction of the shadow (angle).")},
	{_T("BOOLEAN_LIST"),	_T("roundCornerRect"),	_T("false"),		_T("Round rectangle corner enabled"),		_T("Is rounding of rectangle corner feature enabled.")},
	{_T("STRING"),			_T("roundCornerRadius"),_T("9"),			_T("Round rectangle corner radius"),		_T("Radius of round rectangle corner.")},
	{_T("STRING"),			_T("help"),				_T(""),				_T("Help URL"),								_T("Sets this value to specify the URL containig the help information belonging to the proxy.")},
	{_T("MULTISTRING"),		_T("description"),		_T(""),				_T("Description"),							_T("Free format description of the model element for documentation purposes.")},
	{_T("COMPASS_OPTION"),	_T("namePosition"),		_T("4"),			_T("Name Location"),						_T("Sets the name label location relative to the proxy icon. Eg. North.")},
	{_T("STRING"),			_T("nameWrap"),			_T("0"),			_T("NameWrap number"),						_T("Sets the number of characters in a line of the name. If it is 0, then wrapping is disabled.")},
	{_T("BOOLEAN_LIST"),	_T("isNameEnabled"),	_T("true"),			_T("Name enabled"),							_T("Displays the name.")},
	{_T("COMPASS_CHECK"),	_T("autorouterPref"),	_T("neswNESW"),		_T("Auto Router Preference"),				_T("Sets the allowed stick point of the connections from and to this proxy.")},
	{_T("BOOLEAN_LIST"),	_T("isHotspotEnabled"),	_T("true"),			_T("Hotspots enabled"),						_T("Enables the hotspot feature in connection mode.")},
	{_T("COLOR"),			_T("borderColor"),		_T("0x000000"),		_T("Border color"),							_T("Proxy border color. Set this value to change the proxy border color.")},
	{_T("COLOR"),			_T("portColor"),		_T("0x000000"),		_T("Port name color"),						_T("Port name color for the displayed ports of this proxy.")},
	{_T("STRING"),			_T("decorator"),		_T(""),				_T("Decorator"),							_T("ProgID of an process COM component followed by optional parameters for installing  custom drawing code.\r\n Syntax: <ProgID> [<Param1=Value>...<ParamX=ValueX>]")},
	{_T("STRING"),			_T("icon"),				_T(""),				_T("Icon name"),							_T("Image(.bmp,.gif) file name to display the proxy. Icon search paths can be set in File/Settings menu.")},
	{_T("STRING"),			_T("porticon"),			_T(""),				_T("Port icon name"),						_T("Specifies the image(.bmp,.gif) file name to display the proxy if it is a port in a proxy.")},
	{_T("BOOLEAN_LIST"),	_T("portLabelInside"),	_T("true"),			_T("PortLabel shown inside container?"),	_T("Portname label may be shown either inside or outside the container.")},
	{_T("STRING"),			_T("portLabelLength"),	_T("3"),			_T("PortLabel Length"),						_T("Sets the text length shown when displaying portnames. If 0 total length is shown.")}
};


// Set Preferences
/**** Type ************* Path In Registry ** Default Value ** Name to display ************ Help Text *******************************************************************************************************************************************************/
TCHAR* CPreference::m_szSetPreferenceTable[][5]=
{
	{_T("COLOR"),			_T("color"),			_T("0x000000"),		_T("Color"),						_T("Set color. Set this value to change the set line color.")},
	{_T("COLOR"),			_T("nameColor"),		_T("0x000000"),		_T("Name color"),					_T("Set name label color. Set this value to change the set name label color.")},
	{_T("COLOR"),			_T("fillColor"),		_T("0xFFFFFF"),		_T("Fill color"),					_T("Set background fill color for the closed models.")},
	{_T("BOOLEAN_LIST"),	_T("gradientFill"),		_T("false"),		_T("Gradient fill enabled"),		_T("Is the gradient fill feature enabled.")},
	{_T("COLOR"),			_T("gradientColor"),	_T("0xc0c0c0"),		_T("Gradient (2nd fill) color"),	_T("Set gradient background color (the other fill color).")},
	{_T("STRING"),			_T("gradientDirection"),_T("0"),			_T("Gradient direction (angle)"),	_T("Direction of the gradient fill.")},
	{_T("BOOLEAN_LIST"),	_T("itemShadowCast"),	_T("false"),		_T("Shadow casting enabled"),		_T("Is the shadow casting feature enabled.")},
	{_T("COLOR"),			_T("shadowColor"),		_T("0xc0c0c0"),		_T("Shadow color"),					_T("Set shadow color.")},
	{_T("STRING"),			_T("shadowThickness"),	_T("9"),			_T("Shadow thickness/offset"),		_T("Thickness/offset of the shadow.")},
	{_T("STRING"),			_T("shadowDirection"),	_T("45"),			_T("Shadow direction (angle)"),		_T("Direction of the shadow (angle).")},
	{_T("BOOLEAN_LIST"),	_T("roundCornerRect"),	_T("false"),		_T("Round rectangle corner enabled"),_T("Is rounding of rectangle corner feature enabled.")},
	{_T("STRING"),			_T("roundCornerRadius"),_T("9"),			_T("Round rectangle corner radius"),_T("Radius of round rectangle corner.")},
	{_T("STRING"),			_T("help"),				_T(""),				_T("Help URL"),						_T("Sets this value to specify the URL containig the help information belonging to the set.")},
	{_T("MULTISTRING"),		_T("description"),		_T(""),				_T("Description"),					_T("Free format description of the model element for documentation purposes.")},
	{_T("COMPASS_OPTION"),	_T("namePosition"),		_T("4"),			_T("Name Location"),				_T("Sets the name label location relative to the set icon. Eg. North.")},
	{_T("STRING"),			_T("nameWrap"),			_T("0"),			_T("NameWrap number"),				_T("Sets the number of characters in a line of the name. If it is 0, then wrapping is disabled.")},
	{_T("BOOLEAN_LIST"),	_T("isNameEnabled"),	_T("true"),			_T("Name enabled"),					_T("Displays the name.")},
	{_T("COMPASS_CHECK"),	_T("autorouterPref"),	_T("neswNESW"),		_T("Auto Router Preference"),		_T("Sets the allowed stick point of the connections from and to this set.")},
	{_T("BOOLEAN_LIST"),	_T("isHotspotEnabled"),	_T("true"),			_T("Hotspots enabled"),				_T("Enables the hotspot feature in connection mode.")},
	{_T("BOOLEAN_LIST"),	_T("isTypeShown"),		_T("false"),		_T("Type displayed"),				_T("Displays the type (subtype) if the set is an Instance")},
	{_T("STRING"),			_T("decorator"),		_T(""),				_T("Decorator"),					_T("ProgID of an process COM component followed by optional parameters for installing  custom drawing code.\r\n Syntax: <ProgID> [<Param1=Value>...<ParamX=ValueX>]")},
	{_T("STRING"),			_T("icon"),				_T(""),				_T("Icon name"),					_T("Image(.bmp,.gif) file name to display the set. Icon search paths can be set in File/Settings menu.")},
	{_T("STRING"),			_T("porticon"),			_T(""),				_T("Port icon name"),				_T("Specifies the image(.bmp,.gif) file name to display the set if it is a port in a model.")},
	{_T("STRING"),			_T("subTypeIcon"),		_T(""),				_T("Subtype icon name"),			_T("Image(.bmp,.gif) file name to display the set if it a subtype. Icon search paths can be set in File/Settings menu.")},
	{_T("STRING"),			_T("instanceIcon"),		_T(""),				_T("Instance icon name"),			_T("Image(.bmp,.gif) file name to display the set if it an instance. Icon search paths can be set in File/Settings menu.")}
};
/***********************************************************************************************************************************************************************************************************************************************************/






//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPreference::CPreference()
{
	nConnTableLength = sizeof(m_szConnectionPreferenceTable)/sizeof(TCHAR*)/5;
	nAtomTableLength = sizeof(m_szAtomPreferenceTable)/sizeof(TCHAR*)/5;
	nModelTableLength= sizeof(m_szModelPreferenceTable)/sizeof(TCHAR*)/5;
	nProxyTableLength= sizeof(m_szProxyPreferenceTable)/sizeof(TCHAR*)/5;
	nSetTableLength=   sizeof(m_szSetPreferenceTable)/sizeof(TCHAR*)/5;

	// Get system background color
	COLORREF crSysBckg=::GetSysColor(COLOR_WINDOW);
	swprintf(m_szSysBckGrnd, sizeof(m_szSysBckGrnd)/sizeof(m_szSysBckGrnd[0]), _T("0x%02x%02x%02x"),GetRValue(crSysBckg), GetGValue(crSysBckg), GetBValue(crSysBckg));

	// Correcting table
	m_szModelPreferenceTable[3][2]=m_szSysBckGrnd;

	isAutRouterPrefsCategoryAdded = false;
}

CPreference::~CPreference()
{

}


// Finding the common preferences based on registry path
// which is assumed unique in the tables.

void CPreference::SetupPreferences(BYTE cFlags)
{
	m_strRegPathList.RemoveAll();

	if(cFlags<1 || cFlags>=32)
	{
		ASSERT((_T("Invalid cFlags parameter SetupPreferences(...)"),0));
		return;
	}

	// If there is a Connection
	if(cFlags&PREF_CONNECTION)
	{
		for(int i=0;i<nConnTableLength;i++)
		{
			bool bIsCommonRegPath=true;
			CString strCurrRegPath(m_szConnectionPreferenceTable[i][1]);

			if(cFlags&PREF_ATOM)
			{
				bIsCommonRegPath&=(FindRegPathInAtomTable(strCurrRegPath)!=-1);
			}
			if(cFlags&PREF_MODEL)
			{
				bIsCommonRegPath&=(FindRegPathInModelTable(strCurrRegPath)!=-1);
			}
			if(cFlags&PREF_PROXY)
			{
				bIsCommonRegPath&=(FindRegPathInProxyTable(strCurrRegPath)!=-1);
			}
			if(cFlags&PREF_SET)
			{
				bIsCommonRegPath&=(FindRegPathInSetTable(strCurrRegPath)!=-1);
			}

			// If it was a common preference
			if(bIsCommonRegPath)
			{
				m_strRegPathList.AddTail(strCurrRegPath);
			}
		}
	}
	// Atom
	else if(cFlags&PREF_ATOM)
	{
		for(int i=0;i<nAtomTableLength;i++)
		{
			bool bIsCommonRegPath=true;
			CString strCurrRegPath(m_szAtomPreferenceTable[i][1]);

			if(cFlags&PREF_MODEL)
			{
				bIsCommonRegPath&=(FindRegPathInModelTable(strCurrRegPath)!=-1);
			}
			if(cFlags&PREF_PROXY)
			{
				bIsCommonRegPath&=(FindRegPathInProxyTable(strCurrRegPath)!=-1);
			}
			if(cFlags&PREF_SET)
			{
				bIsCommonRegPath&=(FindRegPathInSetTable(strCurrRegPath)!=-1);
			}

			// If it was a common preference
			if(bIsCommonRegPath)
			{
				m_strRegPathList.AddTail(strCurrRegPath);
			}

		}
	}
	// Model
	else if(cFlags&PREF_MODEL)
	{
		for(int i=0;i<nModelTableLength;i++)
		{
			bool bIsCommonRegPath=true;
			CString strCurrRegPath(m_szModelPreferenceTable[i][1]);

			if(cFlags&PREF_PROXY)
			{
				bIsCommonRegPath&=(FindRegPathInProxyTable(strCurrRegPath)!=-1);
			}
			if(cFlags&PREF_SET)
			{
				bIsCommonRegPath&=(FindRegPathInSetTable(strCurrRegPath)!=-1);
			}

			// If it was a common preference
			if(bIsCommonRegPath)
			{
				m_strRegPathList.AddTail(strCurrRegPath);
			}
		}
	}
	//Proxy
	else if(cFlags&PREF_PROXY)
	{
		for(int i=0;i<nProxyTableLength;i++)
		{
			CString strCurrRegPath(m_szProxyPreferenceTable[i][1]);
			bool bIsCommonRegPath=true;

			if(cFlags&PREF_SET)
			{
				bIsCommonRegPath&=(FindRegPathInSetTable(strCurrRegPath)!=-1);
			}

			// If it was a common preference
			if(bIsCommonRegPath)
			{
				m_strRegPathList.AddTail(strCurrRegPath);
			}
		}
	}
	// Set
	else if(cFlags&PREF_SET)
	{
		for(int i=0;i<nSetTableLength;i++)
		{
			CString strCurrRegPath(m_szSetPreferenceTable[i][1]);
			m_strRegPathList.AddTail(strCurrRegPath);
		}
	}

	// ERROR: wrong cFlags parameter
	else
	{
		ASSERT((_T("Invalid cFlags parameter SetupPreferences(...)"),0));
	}

}

int CPreference::FindRegPathInConnectionTable(const CString &strPath)
{

	for(int i=0;i<nConnTableLength;i++)
	{
		CString strPathElement(m_szConnectionPreferenceTable[i][1]);
		if(strPath==strPathElement)
		{
			return i;
		}
	}

	return -1;
}

int CPreference::FindRegPathInAtomTable(const CString &strPath)
{
	for(int i=0;i<nAtomTableLength;i++)
	{
		CString strPathElement(m_szAtomPreferenceTable[i][1]);
		if(strPath==strPathElement)
		{
			return i;
		}
	}

	return -1;
}


int CPreference::FindRegPathInModelTable(const CString &strPath)
{
	for(int i=0;i<nModelTableLength;i++)
	{
		CString strPathElement(m_szModelPreferenceTable[i][1]);
		if(strPath==strPathElement)
		{
			return i;
		}
	}

	return -1;
}


int CPreference::FindRegPathInProxyTable(const CString &strPath)
{
	for(int i=0;i<nProxyTableLength;i++)
	{
		CString strPathElement(m_szProxyPreferenceTable[i][1]);
		if(strPath==strPathElement)
		{
			return i;
		}
	}
	return -1;
}


int CPreference::FindRegPathInSetTable(const CString &strPath)
{
	for(int i=0;i<nSetTableLength;i++)
	{
		CString strPathElement(m_szSetPreferenceTable[i][1]);
		if(strPath==strPathElement)
		{
			return i;
		}
	}

	return -1;
}


// Must be in transaction
void CPreference::CreateList(const CMgaFCOPtrList& MgaFCOPtrList)
{
	m_ListItemArray.RemoveAll();
	isAutRouterPrefsCategoryAdded = false;
	// If there is no FCO to show
	if(MgaFCOPtrList.IsEmpty())
	{
		return;
	}

	CListItem ListItem;
	ListItem.bIsContainer=true;
	ListItem.strName=_T("General Preferences");

	m_ListItemArray.Add(ListItem);

	BYTE cFlag=0;
	//CComPtr<IMgaFCO> ccpMgaFCO; // commented by zolmol7


	POSITION pos=MgaFCOPtrList.GetHeadPosition();

	// Iterating through the list determining the types of the FCOs
	while(pos)
	{
		CComPtr<IMgaFCO> ccpMgaFCO = MgaFCOPtrList.GetNext(pos); // zolmol7

		objtype_enum oeType;
		COMTHROW(ccpMgaFCO->get_ObjType(&oeType));


		switch(oeType)
		{
			case OBJTYPE_MODEL:
				cFlag|=PREF_MODEL;break;

			case OBJTYPE_ATOM:
				cFlag|=PREF_ATOM;break;

			case OBJTYPE_CONNECTION:
				cFlag|=PREF_CONNECTION;break;

			case OBJTYPE_REFERENCE:
				cFlag|=PREF_PROXY;break;

			case OBJTYPE_SET:
				cFlag|=PREF_SET;break;

			default: ASSERT((_T("Invalid MGA object type found creating preference list"),FALSE));break;
		}
	}

	// Creating preferences registry paths to m_strRegPathList
	SetupPreferences(cFlag);

	// Iterating through m_strRegPathList
	POSITION posPathList=m_strRegPathList.GetHeadPosition();
	// Because GetNext updates the position, we store the position of the current element
	POSITION posPathListCurr=posPathList;

	while(posPathList)
	{
		CString strCurrPath=m_strRegPathList.GetNext(posPathList);

		// Getting the registry entry for each objects
		POSITION posFCOs=MgaFCOPtrList.GetHeadPosition();

		// If there are multiple objects, they will be displayed based on the first one.
		bool bFirstRun=true;
		bool bSameValuefOrAll=true;
		bool bIsDefault=true;
		objtype_enum oeFirstType = OBJTYPE_ATOM;
		CString strFirstValue;

		while(posFCOs)
		{
			CComPtr<IMgaFCO> ccpMgaFCO = MgaFCOPtrList.GetNext(posFCOs);

			/*** Getting value belonging to the current registry path from MGA ***/
			CString strRegValue;
			int status = GetRegistryValue(ccpMgaFCO,strCurrPath,strRegValue);

			if (status == ATTSTATUS_INVALID || status == ATTSTATUS_UNDEFINED)
			{
				// get default value from table
				objtype_enum oeType;
				COMTHROW(ccpMgaFCO->get_ObjType(&oeType));
				GetDefaultValue(strCurrPath, oeType, strRegValue);
			}

			bIsDefault = (status != ATTSTATUS_HERE);

			if(bFirstRun)
			{
				strFirstValue=strRegValue;
				COMTHROW(ccpMgaFCO->get_ObjType(&oeFirstType));

				bFirstRun=false;
			}
			if(strFirstValue!=strRegValue)
			{
				bSameValuefOrAll=false;
			}
		}

		TableRowType pTableRow=GetTableRow(strCurrPath,oeFirstType);

		// Creating CListItem
		CArray<CListItem,CListItem&> ListItems;

		bool bIsMultiple=(MgaFCOPtrList.GetCount()>1);

		CComPtr<IMgaFCO> ccpFirstFCO=MgaFCOPtrList.GetHead();



		// Create list items - they may consist of more then one item (AutoRouter Preference)
		CreateListItem(ListItems, pTableRow,strFirstValue,bSameValuefOrAll,
						bIsDefault,bIsMultiple,ccpFirstFCO,posPathListCurr);

		SetCategories(strCurrPath,false);

		m_ListItemArray.Append(ListItems);

		SetCategories(strCurrPath,true);

		posPathListCurr=posPathList;
	}


}


void CPreference::SetCategories( const CString& strPath, bool bIsAfter)
{

	CString strName, strToolTip;

	if(!bIsAfter)
	{
		if(strPath==_T("srcStyle"))
		{
			strName=_T("Connection Preferences");
		}
		else if((strPath==_T("autorouterPref") || strPath==_T("isModelAutoRouted")) && !isAutRouterPrefsCategoryAdded)
		{
			strName=_T("Auto Router Preferences");
			isAutRouterPrefsCategoryAdded = true;
		}
		else
		{
			return;
		}
	}
	else
	{
		return;
	}

	CListItem ListItem;
	ListItem.bIsContainer=true;
	ListItem.strName=strName;
	ListItem.strToolTip=strToolTip;

	m_ListItemArray.Add(ListItem);


}


// Getting value belonging to the specified registry path from MGA.
// If strRegValue is empty the registry value does not exist.
int CPreference::GetRegistryValue(CComPtr<IMgaFCO>&ccpMgaFCO,CString strPath, CString &strRegValue)
{
	CComBSTR bstrRegPath(strPath);

	// Getting regnode
	CComPtr<IMgaRegNode> ccpMgaRegNode;
	COMTHROW(ccpMgaFCO->get_RegistryNode(bstrRegPath,&ccpMgaRegNode));

	// Getting regnode status
	long lRegNodeStatus;
	COMTHROW(ccpMgaRegNode->get_Status(&lRegNodeStatus));


	//Status of definition: 0: this node, -1: in meta, >=1: inherited
	if(lRegNodeStatus == ATTSTATUS_HERE)
	{
		/* Getting the value of the registry node */

		CComBSTR bstrRegValue;
		COMTHROW(ccpMgaRegNode->get_Value(&bstrRegValue));

		strRegValue=bstrRegValue;

	}
	else if(lRegNodeStatus == ATTSTATUS_METADEFAULT)
	{
		/* Getting value from meta */

		// Getting Meta
		CComPtr<IMgaMetaFCO> ccpMetaFCO;
		COMTHROW(ccpMgaFCO->get_Meta(&ccpMetaFCO));

		// Getting registry value from meta
		CComBSTR bstrRegValue;
		COMTHROW(ccpMetaFCO->get_RegistryValue(bstrRegPath,&bstrRegValue));

		strRegValue=bstrRegValue;
	}
	else if(lRegNodeStatus >= ATTSTATUS_IN_ARCHETYPE1)
	{
		/* Getting inherited value */

		// Getting ancestor FCO
		CComPtr<IMgaFCO> ccpAncestorFCO;
		COMTHROW(ccpMgaFCO->get_DerivedFrom(&ccpAncestorFCO));

		// Getting registry value from the ancestor
		CComBSTR bstrRegValue;
		COMTHROW(ccpAncestorFCO->get_RegistryValue(bstrRegPath,&bstrRegValue));

		strRegValue=bstrRegValue;
	}
	else if(lRegNodeStatus == ATTSTATUS_INVALID)  //  - It does happen.
	{
		strRegValue=_T("");
	}
	else if (lRegNodeStatus == ATTSTATUS_UNDEFINED)
	{
	}
	else
	{
		ASSERT((_T("Undocumented(and undesired) MGA feature"),false));
	}
	return lRegNodeStatus;
}




void CPreference::GetDefaultValue(CString strCurrPath, objtype_enum oeType, CString& strDefValue)
{
	switch(oeType)
	{
		case OBJTYPE_MODEL:
			{
				int nIndex=FindRegPathInModelTable(strCurrPath);
				ASSERT(nIndex!=-1);
				strDefValue=m_szModelPreferenceTable[nIndex][2];
			}
			break;

		case OBJTYPE_ATOM:
			{
				int nIndex=FindRegPathInAtomTable(strCurrPath);
				ASSERT(nIndex!=-1);
				strDefValue=m_szAtomPreferenceTable[nIndex][2];
			}
			break;

		case OBJTYPE_CONNECTION:
			{
				int nIndex=FindRegPathInConnectionTable(strCurrPath);
				ASSERT(nIndex!=-1);
				strDefValue=m_szConnectionPreferenceTable[nIndex][2];
			}
			break;

		case OBJTYPE_REFERENCE:
			{
				int nIndex=FindRegPathInProxyTable(strCurrPath);
				ASSERT(nIndex!=-1);
				strDefValue=m_szProxyPreferenceTable[nIndex][2];
			}
			break;

		case OBJTYPE_SET:
			{
				int nIndex=FindRegPathInSetTable(strCurrPath);
				ASSERT(nIndex!=-1);
				strDefValue=m_szSetPreferenceTable[nIndex][2];
			}
			break;

		default: ASSERT((_T("Invalid MGA object type found creating preference list"),FALSE));break;
	}
}




void CPreference::CreateListItem(CArray<CListItem,CListItem&> &ListItems,
								 TableRowType pTableRow, const CString &strValue,
								 bool bSameValueForAll, bool bIsDefault,
								 bool bIsMultiple, const CComPtr<IMgaFCO>&ccpMgaFCO,
								 POSITION nKey)
{

	ListItems.RemoveAll();

	CListItem ListItem;
	ListItem.strName=(*pTableRow)[3];
	ListItem.strToolTip=(*pTableRow)[4];
	ListItem.bIsDefault=bIsDefault;
	ListItem.bIsDifferentValue=!bSameValueForAll;
	ListItem.dwKeyValue = nKey;
	ListItem.dwUserData=0;

	CString strPrefName=(*pTableRow)[1];

	if(strPrefName==_T("isAutoRouted"))
	{
		ListItem.dwUserData=AUTOROUTER_STATE_CONN;
	}
	else if(strPrefName==_T("isModelAutoRouted"))
	{
		ListItem.dwUserData=AUTOROUTER_STATE_MODEL;
	}

	CString strEntryType=(*pTableRow)[0];

	if(strEntryType==_T("COLOR"))
	{
		// Change value format from MGA 0xrrggbb to Windows 0xbbggrr
		CString strNewVal;

		TCHAR* endptr;
		long l = _tcstol(strValue, &endptr, 16);
		if(*endptr == _T('\0'))
		{
			strNewVal.Format(_T("0x%02x%02x%02x"),GetRValue(l), GetGValue(l), GetBValue(l));
		}
		else
		{
			// Invalid color format
			ASSERT(false);
		}
		ListItem.Value.dataType=ITEMDATA_COLOR;
		ListItem.Value.stringVal.SetAtGrow(0,strNewVal);
		VERIFY(ListItem.Value.Validate());

		ListItem.DefValue.dataType=ITEMDATA_COLOR;
		ListItem.DefValue.stringVal.SetAtGrow(0,(*pTableRow)[2]);
		VERIFY(ListItem.DefValue.Validate());
		ListItems.Add(ListItem);
	}

	else if(strEntryType==_T("STRING"))
	{
		ListItem.Value.SetStringValue(strValue);
		ListItem.DefValue.SetStringValue((*pTableRow)[2]);
		ListItems.Add(ListItem);
	}

	else if(strEntryType==_T("MULTISTRING"))
	{
		CStringArray strValueArray;
		str_split(strValue, strValueArray);
		ListItem.Value.SetStringValue(strValueArray,3);

		CString strDefValue = (*pTableRow)[2];
		CStringArray strDefValueArray;
		str_split(strDefValue, strDefValueArray);
		ListItem.DefValue.SetStringValue(strDefValueArray,3);
		
		ListItems.Add(ListItem);
	}

	else if(strEntryType==_T("COMPASS_OPTION"))
	{
		UINT uCompassValue= CCompassData::ParseMgaCompassValueOption(strValue);
		ListItem.Value.SetCompassExclValue(uCompassValue);

		uCompassValue=CCompassData::ParseMgaCompassValueOption((*pTableRow)[2]);
		ListItem.DefValue.SetCompassExclValue(uCompassValue);
		ListItems.Add(ListItem);
	}

	else if(strEntryType==_T("COMPASS_CHECK"))
	{
		// Dst: small case; Src: capital
		UINT uSrcVal,uDstVal,uSrcDefVal,uDstDefVal;
		CCompassData::ParseMgaCompassValueCheck(strValue,uDstVal,uSrcVal);
		CCompassData::ParseMgaCompassValueCheck((*pTableRow)[2],uDstDefVal,uSrcDefVal);

		CListItem ListItemSrc=ListItem;

		// Setting source
		ListItemSrc.Value.SetCompassValue(uSrcVal);
		ListItemSrc.DefValue.SetCompassValue(uSrcDefVal);
		ListItemSrc.strName+=_T(" (Source)");
		ListItemSrc.strToolTip+=_T(" (Source side)");
		ListItemSrc.dwUserData=AUTOROUTER_SOURCE;

		ListItems.Add(ListItemSrc);

		// Setting destination
		ListItem.Value.SetCompassValue(uDstVal);
		ListItem.DefValue.SetCompassValue(uDstDefVal);
		ListItem.strName+=_T(" (Destination)");
		ListItem.strToolTip+=_T(" (Destination side)");
		ListItem.dwUserData=AUTOROUTER_DESTINATION;
		ListItems.Add(ListItem);

		// Creating _T("Miscellaneous Preferences") category
		CListItem ListItemMisc;
		ListItemMisc.bIsContainer=true;
		ListItemMisc.strName=_T("Miscellaneous Preferences");
		ListItems.Add(ListItemMisc);

	}

	else if(strEntryType==_T("LINE_STYLE_LIST"))
	{
		CString strDefValue=(*pTableRow)[2];
		CStringArray strArr;
		int nDefIndex=-1, nIndex=-1;

		for(int i=0;i<LINE_STYLE_LIST_SIZE;i++)
		{
			CString strTemp=m_szLineStyleList[i];
			if(strTemp==strValue)
			{
				nIndex=i;
			}

			if(strTemp==strDefValue)
			{
				nDefIndex=i;
			}
			strArr.Add(strTemp);
		}

		ASSERT(nIndex!=-1);		// Values is not in the list
		ASSERT(nDefIndex!=-1);	// Value in the table as default is not in the list

		ListItem.Value.SetListValue(strArr,nIndex);
		ListItem.DefValue.SetListValue(strArr,nDefIndex);
		ListItems.Add(ListItem);

	}
	else if(strEntryType==_T("LINE_TYPE_LIST"))
	{
		CString strDefValue=(*pTableRow)[2];
		CStringArray strArr;
		int nDefIndex=-1, nIndex=-1;

		for(int i=0;i<LINE_TYPE_LIST_SIZE;i++)
		{
			CString strTemp=m_szLineTypeList[i];
			if(strTemp==strValue)
			{
				nIndex=i;
			}

			if(strTemp==strDefValue)
			{
				nDefIndex=i;
			}
			strArr.Add(strTemp);
		}

		ASSERT(nIndex!=-1);		// Values is not in the list
		ASSERT(nDefIndex!=-1);	// Value in the table as default is not in the list

		ListItem.Value.SetListValue(strArr,nIndex);
		ListItem.DefValue.SetListValue(strArr,nDefIndex);
		ListItems.Add(ListItem);

	}

	else if(strEntryType==_T("BOOLEAN_LIST"))
	{
		CString strDefValue=(*pTableRow)[2];
		CStringArray strArr;
		int nDefIndex=-1, nIndex=-1;

		for(int i=0;i<BOOLEAN_LIST_SIZE;i++)
		{
			CString strTemp=m_szBooleanList[i];
			if(strTemp==strValue)
			{
				nIndex=i;
			}

			if(strTemp==strDefValue)
			{
				nDefIndex=i;
			}
			strArr.Add(strTemp);
		}
		if (nIndex < 0) {
			nIndex = nDefIndex;
		}

		ASSERT(nIndex!=-1);		// Values is not in the list
		ASSERT(nDefIndex!=-1);	// Value in the table as default is not in the list

		ListItem.Value.SetListValue(strArr,nIndex);
		ListItem.DefValue.SetListValue(strArr,nDefIndex);
		ListItems.Add(ListItem);

	}


	else if(strEntryType==_T("ATTR_LIST"))
	{
		if(!bIsMultiple)
		{

			CStringArray strArr;
			CStringArray strLabelArr;

			// Get default value and set it as the first element
			CString strTemp=(*pTableRow)[2];
			strArr.Add(_T(""));
			strLabelArr.Add(strTemp);
			int nDefIndex=0;

			CComPtr<IMgaMetaFCO> ccpMetaFCO;

			COMTHROW(ccpMgaFCO->get_Meta(&ccpMetaFCO));

			CComPtr<IMgaMetaAttributes> ccpMetaAttributes;
			COMTHROW(ccpMetaFCO->get_Attributes(&ccpMetaAttributes));

			int nIndex=-1;
			if(strValue.IsEmpty())
			{
				nIndex=0;
			}

			int i=1;
			MGACOLL_ITERATE(IMgaMetaAttribute, ccpMetaAttributes)
			{
				CComBSTR bstrName;
				COMTHROW(MGACOLL_ITER->get_Name(&bstrName));
				strTemp=bstrName;
				strLabelArr.Add(strTemp);

				strTemp=_T("%") + strTemp + _T("%");
				strArr.Add(strTemp);


				if(strTemp==strValue)
				{
					nIndex=i;
				}

				i++;
			}MGACOLL_ITERATE_END;

//			ASSERT(nIndex!=-1);		// Value is not among the attributes
			if( nIndex < 0 )
				nIndex = 0;

			ListItem.Value.SetListValue(strLabelArr,strArr,nIndex);
			ListItem.DefValue.SetListValue(strLabelArr,strArr,nDefIndex);
		}
		ListItems.Add(ListItem);
	}

	else
	{
		// If you created a new type in the tables, add a piece of converter code above
		// for it.
		ASSERT((_T("Unknown preference type has been specified in the preference tables."),0));
	}


}




TableRowType CPreference::GetTableRow(CString strCurrPath, objtype_enum oeType)
{
	switch(oeType)
	{
		case OBJTYPE_MODEL:
			{
				int nIndex=FindRegPathInModelTable(strCurrPath);
				ASSERT(nIndex!=-1);
				return &m_szModelPreferenceTable[nIndex];
			}
			break;

		case OBJTYPE_ATOM:
			{
				int nIndex=FindRegPathInAtomTable(strCurrPath);
				ASSERT(nIndex!=-1);
				return &m_szAtomPreferenceTable[nIndex];
			}
			break;

		case OBJTYPE_CONNECTION:
			{
				int nIndex=FindRegPathInConnectionTable(strCurrPath);
				ASSERT(nIndex!=-1);
				return &m_szConnectionPreferenceTable[nIndex];
			}
			break;

		case OBJTYPE_REFERENCE:
			{
				int nIndex=FindRegPathInProxyTable(strCurrPath);
				ASSERT(nIndex!=-1);
				return &m_szProxyPreferenceTable[nIndex];
			}
			break;

		case OBJTYPE_SET:
			{
				int nIndex=FindRegPathInSetTable(strCurrPath);
				ASSERT(nIndex!=-1);
				return &m_szSetPreferenceTable[nIndex];
			}
			break;

		default:
			{
				ASSERT((_T("Invalid MGA object type found creating preference list"),FALSE));break;
			}
	}

	return NULL;
}

void CPreference::Write2Mga(const CString& strValue,const CComPtr<IMgaFCO>& ccpMgaFCO,const CListItem& ListItem, const CString& strRegPath, bool bIsForKind)
{


	// Converting path and value in BSTR format for MGA
	CComBSTR bstrPath(strRegPath);
	CComBSTR bstrValue(strValue);


	if(ListItem.bIsDefault)
	{
		// Getting regnode
		CComPtr<IMgaRegNode> ccpMgaRegNode;
		COMTHROW(ccpMgaFCO->get_RegistryNode(bstrPath,&ccpMgaRegNode));

		// Delete the registry entries
		COMTHROW(ccpMgaRegNode->RemoveTree());
	}
	else
	{
		if(bIsForKind)
		{
			// Delete current value - meta will be displayed
			// Getting regnode
			CComPtr<IMgaRegNode> ccpMgaRegNode;
			COMTHROW(ccpMgaFCO->get_RegistryNode(bstrPath,&ccpMgaRegNode));

			// Hacked by Paka in order to always do something in the project !!!
			// If there is nothing in the Project (NOT the MetaProject) to be commited, then the user gets confused
			// because the Project does not change and nothing has to be refreshed
			 COMTHROW( ccpMgaRegNode->put_Value( CComBSTR( _T("Dummy") ) ) );
			// Hacked part end

			// Delete the registry entries
			COMTHROW(ccpMgaRegNode->RemoveTree());
				// For _T("kind") we set registry values in the meta
			CComPtr<IMgaMetaFCO> ccpMetaFCO;
			COMTHROW(ccpMgaFCO->get_Meta(&ccpMetaFCO));

			COMTHROW(ccpMetaFCO->put_RegistryValue(bstrPath,bstrValue));
		}
		else
		{
			// For a specific object we modify the registry value
			COMTHROW(ccpMgaFCO->put_RegistryValue(bstrPath,bstrValue));

		}
	}
}


void CPreference::ListValue2MgaValue(const CListItem& ListItem, CString& strValue)
{

	switch(ListItem.Value.dataType)
	{
		case ITEMDATA_COLOR	:
		{
			strValue.Format(_T("0x%02x%02x%02x"),
							GetRValue(ListItem.Value.colorVal),
							GetGValue(ListItem.Value.colorVal),
							GetBValue(ListItem.Value.colorVal));
		}break;

		case ITEMDATA_COMPASS_EXCL:
		{
			CCompassData::toMgaStringOption(strValue,ListItem.Value.compassVal);
		}break;

		case ITEMDATA_COMPASS:
		{
			strValue=ListItem.Value.stringVal[0];
		}break;

		case ITEMDATA_FIXED_LIST:
		{
			if (ListItem.Value.stringListNames.GetSize() > 0) {
				strValue=ListItem.Value.stringListNames.GetAt(ListItem.Value.listVal);
			}
			else {
				ListItem.Value.toString(strValue);
			}
		}break;

		default:
		{
			ListItem.Value.toString(strValue);
		}break;
	}
}

void CPreference::WriteItemToMga(CListItem ListItem,const CMgaFCOPtrList& MgaFCOPtrList, bool bIsForKind)
{
	if(ListItem.bIsContainer)return;

	CString strValue;
	ListValue2MgaValue(ListItem,strValue);

	CString strRegPath=m_strRegPathList.GetAt((POSITION)ListItem.dwKeyValue);

	POSITION pos=MgaFCOPtrList.GetHeadPosition();

	while(pos)
	{
		CComPtr<IMgaFCO> ccpMgaFCO=MgaFCOPtrList.GetNext(pos);

		Write2Mga(strValue,ccpMgaFCO,ListItem,strRegPath,bIsForKind);


	}
}

void CPreference::IssuePossiblePathConversion(CListItem ListItem, const CMgaFCOPtrList& MgaFCOPtrList,
											  CObjectInspectorCtrl* pParent)
{
	if (ListItem.bIsContainer)
		return;

	CString strValue;
	ListValue2MgaValue(ListItem,strValue);

	POSITION pos = MgaFCOPtrList.GetHeadPosition();

	while(pos)
	{
		CComPtr<IMgaFCO> ccpMgaFCO = MgaFCOPtrList.GetNext(pos);

		// Firing the event
		if ((ListItem.dwUserData == AUTOROUTER_STATE_CONN ||
			 ListItem.dwUserData == AUTOROUTER_STATE_MODEL) &&
			 strValue == _T("false"))
		{
			if (ListItem.dwUserData == AUTOROUTER_STATE_CONN)
				pParent->IgnoreNextEvents();
			pParent->FireConvertPathToCustom(ccpMgaFCO);
		}
	}
}
