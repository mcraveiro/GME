// ItemData.h: interface for the CItemData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ITEMDATA_H__10E7D725_9DA8_4F76_A3E3_9FB83D386870__INCLUDED_)
#define AFX_ITEMDATA_H__10E7D725_9DA8_4F76_A3E3_9FB83D386870__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CompassData.h"

void str_split( CString str, CStringArray &returnArray);

enum itemdata_enum
{
	ITEMDATA_NULL			= 0,
	ITEMDATA_STRING			= 1,
	ITEMDATA_INTEGER		= 2,
	ITEMDATA_DOUBLE			= 3,
	ITEMDATA_BOOLEAN		= 4,
	ITEMDATA_FIXED_LIST		= 9,
	ITEMDATA_COLOR			=10,
	ITEMDATA_COMPASS		=11,
	ITEMDATA_COMPASS_EXCL	=12

};

class CInPlaceManager;
class CItemData  
{
	friend class CListItem;
	friend class CInPlaceManager;
	friend class CInspectorList;
	void CommonInit();
public:
	static const TCHAR * m_defFMTSTR;
	static CString      m_fmtStr;
	static void         getRealFmtString();

	void toString();
	bool toString(CString &)const;
	bool Validate();

	CItemData();
	virtual ~CItemData();

	// Constructors
	// Copy Const
	CItemData(CItemData&srcItemData);


	CItemData(int i){CommonInit();SetIntValue(i);};
	CItemData(double d){CommonInit();SetDoubleValue(d);};
	CItemData(bool b){CommonInit();SetBoolValue(b);};
	CItemData(COLORREF c){CommonInit();SetColorValue(c);};
	CItemData(UINT cmps){CommonInit();SetCompassValue(cmps);};
	CItemData(CStringArray& strArr, BYTE cDispLineNum){CommonInit();SetStringValue(strArr,cDispLineNum);};	
	CItemData(CString& str){SetStringValue(str);};
	CItemData(CStringArray& strArr,CStringArray& strArrNames, UINT nSelection){CommonInit();SetListValue(strArr, strArrNames,nSelection);};	
	CItemData(CStringArray& strArr, UINT nSelection){CommonInit();SetListValue(strArr,nSelection);};

	// Set functions: set the value and the data type
	void SetIntValue(int i);
	void SetDoubleValue(double d);
	void SetBoolValue(bool b);
	void SetColorValue(COLORREF c);
	void SetCompassValue(UINT cmps);
	void SetCompassExclValue(UINT cmps);
	void SetStringValue(CStringArray& strArr,BYTE cDispLineNum);
	void SetStringValue(CString str);
	void SetListValue(CStringArray& strArr,CStringArray& strArrNames, UINT nSelection);
	void SetListValue(CStringArray& strArr, UINT nSelection);
	
	// Operator =
	void operator=(const CItemData& srcItemData);

	itemdata_enum dataType;
	
	CStringArray stringVal;
	
	// Intended access is read only
	union
	{
		int			 intVal;
		double		 doubleVal;
		bool		 boolVal;
		COLORREF     colorVal;
		UINT		 compassVal;
		int			 listVal;	// Selected item for ITEMDATA_FIXED_LIST
		BYTE		 cLineNum;	// Line number of the edit box
	};

	CStringArray stringListNames; // Name Array for ITEMDATA_FIXED_LIST

};


class CListItem
{
	friend class CInspectorList;
public:	
	void CopyWithNoState(const CListItem& srcListItem);
	void SetDirty();
	CItemData Value;
	CItemData DefValue;


	CString strName;
	CString strToolTip;
	CString strContentType;
	bool bIsDefault;
	bool bIsReadOnly;
	bool bIsContainer;
	bool bIsContainerOpen;

	// Supporting multiple selections
	// If there is more than one object selected 
	// the value vcan be different
	bool bIsDifferentValue;

	// User data
	void* dwKeyValue;
	DWORD dwUserData;

	CListItem();
	CListItem(CListItem& srcListItem);
	void RestoreDefault();

	void operator =(const CListItem& srcListItem);

private:
	// For containtment
	CArray<CListItem,CListItem&> m_ContainedListItemArray;
};
#endif // !defined(AFX_ITEMDATA_H__10E7D725_9DA8_4F76_A3E3_9FB83D386870__INCLUDED_)
