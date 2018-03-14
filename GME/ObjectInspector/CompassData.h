// CompassData.h: interface for the CCompassData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMPASSDATA_H__3CBC0148_048C_4C29_B51C_226D2B5F16FA__INCLUDED_)
#define AFX_COMPASSDATA_H__3CBC0148_048C_4C29_B51C_226D2B5F16FA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

enum compass_enum
{
	CMPS_NORTH			= 0x001,
	CMPS_EAST			= 0x002,
	CMPS_SOUTH			= 0x004,
	CMPS_WEST			= 0x008,
	CMPS_NORTHEAST		= 0x010,
	CMPS_SOUTHEAST		= 0x020,
	CMPS_SOUTHWEST		= 0x040,
	CMPS_NORTHWEST		= 0x080,
	CMPS_CENTER			= 0x100,
};

class CCompassData  
{
public:
	static void ParseMgaCompassValueCheck(const CString& strValue,  UINT & uSmallCaseValue, UINT &uCapitalValue);
	static UINT ParseMgaCompassValueOption(const CString &strValue);
	static bool bIsSingle(UINT uData);
	static CString toString(UINT uData);
	static bool ParseCompassValue(CString strValue, UINT &uValue);

	static void toMgaStringOption(CString& strValue, UINT uData);
	static void toMgaStringCheck(CString& strValue, UINT uData, bool bIsCapital);
};

#endif // !defined(AFX_COMPASSDATA_H__3CBC0148_048C_4C29_B51C_226D2B5F16FA__INCLUDED_)
