// ItemData.cpp: implementation of the CItemData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CommonMath.h"
#include "objectinspector.h"
#include "ItemData.h"
#include "MgaUtil.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// Split Function
void str_split( CString str, CStringArray &returnArray)
{
   int  iNum     = 0;
   int  iCurrPos = 0;
   while( -1 != (iCurrPos = str.FindOneOf(_T("\n"))))
   {
       returnArray.Add(str.Left(iCurrPos));
       returnArray[iNum].TrimRight(_T("\r\n"));
       str = str.Right(str.GetLength() - iCurrPos - 1);
       iNum++;
   }

   if (str.GetLength() > 0)      // the last one...
   {
       returnArray.Add(str);
       returnArray[iNum].TrimRight(_T("\r\n"));
       iNum++;
   }
}

/*static*/ const TCHAR * CItemData::m_defFMTSTR = _T("%.12g");
/*static*/ CString      CItemData::m_fmtStr    = _T("%.12g");
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//static
void CItemData::getRealFmtString()
{
	CComPtr<IMgaRegistrar> registrar;
	try {
		COMTHROW( registrar.CoCreateInstance(OLESTR("MGA.MgaRegistrar")) );
		ASSERT( registrar != NULL );

		CComBSTR bs_fmt_str;
		COMTHROW( registrar->GetRealNmbFmtStr( REGACCESS_USER, &bs_fmt_str));
		if( bs_fmt_str) 
			CopyTo( bs_fmt_str, CItemData::m_fmtStr);

		if( CItemData::m_fmtStr.IsEmpty())
			CItemData::m_fmtStr = CItemData::m_defFMTSTR;
	} 
	catch (hresult_exception &) {
		CItemData::m_fmtStr = CItemData::m_defFMTSTR;
	}
}


//////////// List Item ///////////////
CListItem::CListItem()
{
	bIsContainer=false;
	bIsContainerOpen=true;
	bIsDefault=false;
	bIsReadOnly=false;
	bIsDifferentValue=false;
	dwKeyValue=0;

}

CListItem::CListItem(CListItem& srcListItem)
{
	// Calling operator=
	*this=srcListItem;
}

void CListItem:: operator =(const CListItem& srcListItem)
{
	strName=srcListItem.strName;			
	strToolTip=srcListItem.strToolTip;	
	strContentType=srcListItem.strContentType;
	bIsDefault=srcListItem.bIsDefault;
	bIsReadOnly=srcListItem.bIsReadOnly;
	bIsContainer=srcListItem.bIsContainer;
	bIsContainerOpen=srcListItem.bIsContainerOpen;
	bIsDifferentValue=srcListItem.bIsDifferentValue;

	dwKeyValue=srcListItem.dwKeyValue;
	dwUserData=srcListItem.dwUserData;

	m_ContainedListItemArray.Copy(srcListItem.m_ContainedListItemArray);

	// Values
	Value=srcListItem.Value;
	DefValue=srcListItem.DefValue;
}


void CListItem::RestoreDefault()
{
	if(bIsContainer || bIsReadOnly)return;

	ASSERT(DefValue.dataType!=ITEMDATA_NULL);

	if(!bIsDefault)
	{
		Value=DefValue;
		bIsDefault=true;
	}
}
//////////////////////////////////////

/////////////// Item Data ///////////


CItemData::CItemData()
{	
	CommonInit();
}

CItemData::~CItemData()
{

}

CItemData::CItemData(CItemData&srcItemData)
{
	*this=srcItemData;
}

bool CItemData::Validate()
{
	bool retVal=false;
	switch(dataType)
	{
		case ITEMDATA_NULL:
			{
				retVal=false;
			}break;
		case ITEMDATA_STRING:
			{
				retVal=true;
			}break;
		case ITEMDATA_FIXED_LIST:
			{
				retVal=(listVal<stringVal.GetSize()&&listVal>=0)?true:false;				
			}break;

		case ITEMDATA_INTEGER:
			{
				retVal=false;
				if(stringVal.GetSize()==1)				
				{
					TCHAR* endptr;
					long l = _tcstol(stringVal[0], &endptr, 10);
					if(*endptr==_T('\0'))
					{
						intVal=l;
						retVal=true;
					}				
				}
				else
				{
					ASSERT(0);
				}
			}break;
		case ITEMDATA_DOUBLE:	
			{
				retVal=false;
				if(stringVal.GetSize()==1)				
				{
					TCHAR* endptr;
					double d;
					if (ParseSpecialDouble(stringVal[0], d))
					{
						doubleVal = d;
						retVal = true;
					}
					else
					{
						d = _tcstod(stringVal[0], &endptr);
						if (*endptr == _T('\0'))
						{
							doubleVal = d;
							retVal = true;
						}
					}
				}
				else
				{
					ASSERT(0);
				}

			}break;

		case ITEMDATA_BOOLEAN:
			{
				retVal=false;
				if(stringVal.GetSize()==1)				
				{
					if(stringVal[0]==_T("True"))
					{
						boolVal=true;
						retVal=true;
					}
					else if(stringVal[0]==_T("False"))
					{
						boolVal=false;
						retVal=true;
					}
				}
				else
				{
					ASSERT(0);
				}

			}break;

		case ITEMDATA_COLOR:
			{
				retVal=false;
				if(stringVal.GetSize()==1)				
				{
					TCHAR* endptr;
					long l = _tcstol(stringVal[0], &endptr, 16);
					if(*endptr == _T('\0'))
					{
						colorVal=l;
						retVal=true;
					}				
				}
				else
				{
					ASSERT(0);
				}


			}break;

		case ITEMDATA_COMPASS:
			{
				retVal=false;
				if(stringVal.GetSize()==1)				
				{
					UINT uValue;
					retVal=CCompassData::ParseCompassValue(stringVal[0],uValue);
					if(retVal) compassVal=uValue;
				}
				else
				{
					ASSERT(0);
				}

			}break;
		case ITEMDATA_COMPASS_EXCL:
			{
				retVal=false;
				if(stringVal.GetSize()==1)				
				{
					UINT uValue;
					retVal=CCompassData::ParseCompassValue(stringVal[0],uValue);
					if(retVal) compassVal=uValue;
				}
				else
				{
					ASSERT(0);
				}

			}break;
	}
	return retVal;

};


//Set functions
void CItemData::SetIntValue(int i)
{
	stringListNames.RemoveAll();
	stringVal.RemoveAll();
	intVal=i; dataType=ITEMDATA_INTEGER;
	toString();
}


void CItemData::SetDoubleValue(double d)
{
	stringVal.RemoveAll();
	stringListNames.RemoveAll();
	doubleVal=d; 
	dataType=ITEMDATA_DOUBLE;
	toString();
}


void CItemData::SetBoolValue(bool b)
{
	stringVal.RemoveAll();
	stringListNames.RemoveAll();
	boolVal=b; 
	dataType=ITEMDATA_BOOLEAN;
	toString();
};


void CItemData::SetColorValue(COLORREF c)
{
	stringVal.RemoveAll();
	stringListNames.RemoveAll();
	colorVal=c; 
	dataType=ITEMDATA_COLOR;
	toString();
}


void CItemData::SetCompassValue(UINT cmps)
{
	stringVal.RemoveAll();
	stringListNames.RemoveAll();
	compassVal=cmps; 
	dataType=ITEMDATA_COMPASS;
	toString();
}


void CItemData::SetCompassExclValue(UINT cmps)
{
	stringVal.RemoveAll();
	stringListNames.RemoveAll();
	compassVal=cmps; 
	dataType=ITEMDATA_COMPASS_EXCL;
	toString();
}

void CItemData::SetListValue(CStringArray& strArr,CStringArray& strArrNames, UINT nSelection)
{
	ASSERT(nSelection<=(UINT)strArr.GetUpperBound());
	
	dataType=ITEMDATA_FIXED_LIST;
	stringVal.Copy(strArr); 
	stringListNames.Copy(strArrNames);

	listVal=nSelection;	
}

void CItemData::SetListValue(CStringArray& strArr, UINT nSelection)
{
	ASSERT(nSelection<=(UINT)strArr.GetUpperBound());
	
	dataType=ITEMDATA_FIXED_LIST;
	stringVal.Copy(strArr); 

	listVal=nSelection;	
}



void CItemData::SetStringValue(CStringArray& strArr,BYTE cDispLineNum)
{
	stringVal.Copy(strArr);
	dataType=ITEMDATA_STRING;
	cLineNum=cDispLineNum;
}


void CItemData::SetStringValue(CString str)
{
	stringVal.RemoveAll();
	stringListNames.RemoveAll();
	stringVal.SetAtGrow(0,str); 
	dataType=ITEMDATA_STRING;
	cLineNum=1;
}







//////////////////////////////////////////////
// Converts the actual value to string
//////////////////////////////////////////////
bool CItemData::toString(CString& strString)const
{
	bool bRetVal=false;
	switch(dataType)
	{
	case ITEMDATA_NULL:
		{
			bRetVal=false;
		}break;

	case ITEMDATA_STRING:
		{		
			if(cLineNum>1) // Multiline
			{			
				strString.Empty();
				INT_PTR nUpperBound=stringVal.GetUpperBound();
				for(int i=0;i<=nUpperBound;i++)
				{
					strString+=stringVal[i];
					if( i != nUpperBound)
						strString+=_T("\r\n");
				}
				//WAS: Trimming last _T("\r\n")
				//WAS: strString=strString.Left(strString.GetLength()-2);
				bRetVal=true;
			}
			else
			{
				strString=stringVal[0];
				bRetVal=true;
			}
			
		}break;
	
		case ITEMDATA_FIXED_LIST:
		{			
			if(listVal<stringVal.GetSize()&&listVal>=0)
			{
				strString=stringVal[listVal];
				bRetVal=true;
			}
			else
			{
				bRetVal=false;
			}
			
		}break;


	case ITEMDATA_INTEGER:
		{
			strString.Format(_T("%d"),intVal);
			bRetVal=true;
		}break;
	case ITEMDATA_DOUBLE:
		{
			strString.Format( CItemData::m_fmtStr,doubleVal);
			bRetVal=true;
		}break;
	case ITEMDATA_BOOLEAN:
		{
			if(boolVal)
			{
				strString=_T("True");
			}
			else
			{
				strString=_T("False");
			}
			bRetVal=true;
		}break;

	case ITEMDATA_COLOR	:
		{
			strString.Format(_T("Red: %u; Green: %u; Blue: %u;"),GetRValue(colorVal),GetGValue(colorVal),GetBValue(colorVal));			
			bRetVal=true;
		}break;

	case ITEMDATA_COMPASS:
		{
			strString=CCompassData::toString(compassVal);
			bRetVal=true;
		}break;
	case ITEMDATA_COMPASS_EXCL:
		{
			strString=CCompassData::toString(compassVal);
			bRetVal=true;
		}break;


	}

	return bRetVal;
}


void CItemData::operator=(const CItemData&srcItemData)
{
	dataType=srcItemData.dataType;

	stringVal.Copy(srcItemData.stringVal);
	stringListNames.Copy(srcItemData.stringListNames);

	//For the union copy only the largest size data
	// If Microsoft tries to screw us later, here come some asserts:
	ASSERT(sizeof(double)==8);
	ASSERT(sizeof(int)==4);
	ASSERT(sizeof(COLORREF)==4);
	ASSERT(sizeof(bool)==1);
	ASSERT(sizeof(UINT)==4);


	doubleVal=srcItemData.doubleVal;

}

void CItemData::toString()
{
	if(dataType==ITEMDATA_STRING||dataType==ITEMDATA_FIXED_LIST)
	{
		return;
	}

	stringVal.RemoveAll();

	CString strTemp;
	if(toString(strTemp))
	{
		stringVal.SetAtGrow(0,strTemp);
	}
}

void CItemData::CommonInit()
{
	dataType=ITEMDATA_NULL;
}

void CListItem::SetDirty()
{
	bIsDefault=false;
}

void CListItem::CopyWithNoState(const CListItem &srcListItem)
{

	// Copying without bIsContainerOpen and m_ContainedListItemArray

	strName=srcListItem.strName;			
	strToolTip=srcListItem.strToolTip;	
	strContentType=srcListItem.strContentType;
	bIsDefault=srcListItem.bIsDefault;
	bIsReadOnly=srcListItem.bIsReadOnly;
	bIsContainer=srcListItem.bIsContainer;
	bIsDifferentValue=srcListItem.bIsDifferentValue;

	dwKeyValue=srcListItem.dwKeyValue;
	dwUserData=srcListItem.dwUserData;
	

	// Values
	Value=srcListItem.Value;
	DefValue=srcListItem.DefValue;
}
