// Attribute.cpp: implementation of the CAttribute class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "objectinspector.h"
#include "ObjectInspectorCtl.h"

#include "Attribute.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

enum project_attributes {PROJECT_AUTHOR, PROJECT_VERSION, PROJECT_COMMENT};
TCHAR *project_attribute_tooltips[] = {
	_T("Author(s) of the project"),
	_T("Current version string"),
	_T("Arbitrary comments"),
};


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAttribute::CAttribute()
{

}

CAttribute::~CAttribute()
{

}

// Must be in transaction
void CAttribute::CreateList(const CMgaFCOPtrList& MgaFCOPtrList,CArray<CListItem,CListItem&> &ListItemArray)
{
	m_MetaAttributePtrList.RemoveAll();
	ListItemArray.RemoveAll();

	if(MgaFCOPtrList.GetCount()==0)return;

	POSITION posFirst=MgaFCOPtrList.GetHeadPosition();
	POSITION posSecond=posFirst;

	// Getting the first FCO and update the position of the second
	CComPtr<IMgaFCO> ccpFirstFCO=MgaFCOPtrList.GetNext(posSecond);

	// Iterating through the FirstFCO's attributes
	CComPtr<IMgaAttributes> ccpMgaAttributes;
	COMTHROW(ccpFirstFCO->get_Attributes(&ccpMgaAttributes));
			
	
	
	MGACOLL_ITERATE(IMgaAttribute, ccpMgaAttributes)
	{
		CComPtr<IMgaMetaAttribute> ccpMetaAttribute;
		COMTHROW(MGACOLL_ITER->get_Meta(&ccpMetaAttribute));
	
		// Iterate through FCOs checking if they have the current attribute common
		POSITION posIterate=posSecond;
		
			
		bool bIsCommonAttr = isViewable( ccpFirstFCO, ccpMetaAttribute);
		bool bIsDirty=false; //is attribute values different for attributes of same type?
		
		// Getting the value of the first attribute
		CComVariant ccvtFirstValue;
		COMTHROW(MGACOLL_ITER->get_Value(&ccvtFirstValue));
		
		// Check whether it is a default value (if it is defined in the meta it's default)
		long lStatus;
		COMTHROW(MGACOLL_ITER->get_Status(&lStatus));
		bool bIsDefault=(lStatus==-1?true:false);

		while(posIterate)
		{
			const CComPtr<IMgaFCO>ccpCurrFCO=MgaFCOPtrList.GetNext(posIterate);

			CComPtr<IMgaAttribute> ccpCurrAttribute;

			HRESULT hrRes=ccpCurrFCO->get_Attribute(ccpMetaAttribute,&ccpCurrAttribute);
			
			
			if(hrRes==E_META_INVALIDATTR)
			{
				bIsCommonAttr=false;
				break;
			}
			else
			{
				COMTHROW(hrRes);
			}

			if( !(bIsCommonAttr = isViewable( ccpCurrFCO, ccpMetaAttribute)))
			{
				break;
			}

			COMTHROW(ccpCurrAttribute->get_Status(&lStatus));
			
			// Check whether it is a default value (if it is defined in the meta it's default)
			bIsDefault&=(lStatus==-1?true:false);

			
			// Check whether the value is the same for all
			CComVariant ccvtCurrValue;
			COMTHROW(ccpCurrAttribute->get_Value(&ccvtCurrValue));
			if(ccvtCurrValue!=ccvtFirstValue)
			{
				bIsDirty=true;
			}

		}

		if(bIsCommonAttr)
		{
			POSITION posCurrent=m_MetaAttributePtrList.AddTail(ADAPT_META_ATTRIBUTE(ccpMetaAttribute));
			CListItem ListItem;

			if(CreateListItem(ListItem,ccvtFirstValue,bIsDirty,bIsDefault,posCurrent))
			{
				ListItemArray.Add(ListItem);
			}
		}

	}MGACOLL_ITERATE_END;



}

// Must be in transaction
void CAttribute::CreateList(CComPtr<IMgaProject> ccpProject, CArray<CListItem,CListItem&> &ListItemArray)
{
	m_MetaAttributePtrList.RemoveAll();
	ListItemArray.RemoveAll();

	if(ccpProject == NULL) return;


	{
		CListItem ListItem;

		ListItem.strName = _T("Author");
		ListItem.dwKeyValue = (void*)(size_t)PROJECT_AUTHOR;
		ListItem.bIsDefault = false;
		ListItem.bIsDifferentValue = false;
		ListItem.strToolTip = project_attribute_tooltips[PROJECT_AUTHOR];

		CComBSTR bstr;
		COMTHROW(ccpProject->get_Author(&bstr));
		CString strValue(bstr);
		ListItem.Value.SetStringValue(strValue);

		ListItem.DefValue.SetStringValue(_T(""));
		
		ListItemArray.Add(ListItem);
	}

	{
		CListItem ListItem;

		ListItem.strName = _T("Version");
		ListItem.dwKeyValue = (void*)(size_t)PROJECT_VERSION;
		ListItem.bIsDefault = false;
		ListItem.bIsDifferentValue = false;
		ListItem.strToolTip = project_attribute_tooltips[PROJECT_VERSION];

		CComBSTR bstr;
		COMTHROW(ccpProject->get_Version(&bstr));
		CString strValue(bstr);
		ListItem.Value.SetStringValue(strValue);

		ListItem.DefValue.SetStringValue(_T(""));
		
		ListItemArray.Add(ListItem);
	}

	{
		CListItem ListItem;

		ListItem.strName = _T("Comment");
		ListItem.dwKeyValue = (void*)(size_t)PROJECT_COMMENT;
		ListItem.bIsDefault = false;
		ListItem.bIsDifferentValue = false;
		ListItem.strToolTip = project_attribute_tooltips[PROJECT_COMMENT];

		// Setting value
		CComBSTR bstr;
		COMTHROW(ccpProject->get_Comment(&bstr));
		CString strValue(bstr);
		CStringArray strValueArray;
		str_split(strValue, strValueArray);
		ListItem.Value.SetStringValue(strValueArray,6);

		// Setting default value
		CString strDefValue = _T("");
		CStringArray strDefValueArray;
		strDefValueArray.Add(strDefValue);
		ListItem.DefValue.SetStringValue(strDefValueArray,6);
		
		ListItemArray.Add(ListItem);
	}


}

//
// We decide if a model attribute is shown based on the model's first aspect
// (currently for all aspects the same attribute list is used)
bool CAttribute::isViewable( const CComPtr<IMgaFCO>& fco, const CComPtr<IMgaMetaAttribute>& attr_meta) const
{
	VARIANT_BOOL vbl = VARIANT_TRUE;
	COMTHROW( attr_meta->get_Viewable( &vbl));
	return vbl == VARIANT_TRUE;
	// attribute hiding policy changed by introducing 'viewable' attribute
	//  for IMgaMetaAttribute. Section below commented out:
	/*
	bool res = true;
	objtype_enum objtype;
	COMTHROW( fco->get_ObjType( &objtype) );
	if( objtype == OBJTYPE_MODEL)
	{
		res = false;
		CComPtr<IMgaMetaFCO> meta;
		CComPtr<IMgaMetaModel> model_meta;
		CComPtr<IMgaMetaAspects> asps;
		CComPtr<IMgaMetaAttributes> shown_attrs;
		metaref_type req_metaid;

		COMTHROW( fco->get_Meta( &meta));
		COMTHROW( meta.QueryInterface( &model_meta));
		COMTHROW( model_meta->get_Aspects( &asps ));
		COMTHROW( attr_meta->get_MetaRef( &req_metaid));
		
		MGACOLL_ITERATE(IMgaMetaAspect, asps) {
			COMTHROW( MGACOLL_ITER->get_Attributes( &shown_attrs));
			break;
		}
		MGACOLL_ITERATE_END;


		MGACOLL_ITERATE(IMgaMetaAttribute, shown_attrs) {
			metaref_type metaid;
			COMTHROW(MGACOLL_ITER->get_MetaRef(&metaid));

			if( metaid == req_metaid)
			{
				res = true;
				break;
			}
			
		}
		MGACOLL_ITERATE_END;
	}
	return res;
	*/
}

bool CAttribute::CreateListItem(CListItem &ListItem, const CComVariant &ccvtValue, bool bIsDirty, bool bIsDefault, POSITION dwKey)
{
	
	CComPtr<IMgaMetaAttribute> ccpMetaAttribute=m_MetaAttributePtrList.GetAt(POSITION(dwKey));
	
	// Getting displayed name
	CComBSTR bstrName;
	COMTHROW(ccpMetaAttribute->get_DisplayedName(&bstrName));

	// Getting default value
	CComVariant ccvtDefValue;
	COMTHROW(ccpMetaAttribute->get_DefaultValue(&ccvtDefValue));
	
	ListItem.strName=bstrName;
	ListItem.dwKeyValue=dwKey;
	ListItem.bIsDefault=bIsDefault;
	ListItem.bIsDifferentValue=bIsDirty;

	// Getting helpText (Undocumented Feature - hopefully coming soon in the MetaInterpreter)
	CComBSTR bstrHelpTextPref;
	CComBSTR bstrHelpTextPrefPath = ATTRIBUTE_HELP_REGISTRY_PATH;
	COMTHROW(ccpMetaAttribute->get_RegistryValue(bstrHelpTextPrefPath,&bstrHelpTextPref));
	CString strHelpText=bstrHelpTextPref;
	ListItem.strToolTip=strHelpText;

	// Getting content-type (stored in the registry like 'helpText')
	CComBSTR bstrContTypePref;
	CComBSTR bstrContTypePrefPath = ATTRIBUTE_CONTENTTYPE_REGISTRY_PATH;
	COMTHROW(ccpMetaAttribute->get_RegistryValue(bstrContTypePrefPath,&bstrContTypePref));
	CString strContentType=bstrContTypePref;
	ListItem.strContentType=strContentType;

	// Getting attribute type
	attval_enum aeAttributeType;
	COMTHROW(ccpMetaAttribute->get_ValueType(&aeAttributeType));

	switch(aeAttributeType)
	{
		case ATTVAL_STRING:
			{
				// Do not trust in VARIANT and MS.
				ASSERT(ccvtValue.vt==VT_BSTR);
				ASSERT(ccvtDefValue.vt==VT_BSTR);
			
				// Checking if it is multiline - reading it from registry	
				CComBSTR bstrMultiLinePref;
				CComBSTR bstrMultiLinePrefPath = ATTRIBUTE_MULTILINE_PREFERENCE_REGISTRY_PATH;
				COMTHROW(ccpMetaAttribute->get_RegistryValue(bstrMultiLinePrefPath,&bstrMultiLinePref));
				
				// Converting to int
				CString strLineNum(bstrMultiLinePref);
				int nLineNumMin = _tstoi(strLineNum);
				
				if(nLineNumMin<2) // Single line edit box
				{
					CString strValue=CComBSTR(ccvtValue.bstrVal);
					ListItem.Value.SetStringValue(strValue);

					CString strDefValue=CComBSTR(ccvtDefValue.bstrVal);
					ListItem.DefValue.SetStringValue(strDefValue);
				}
				else		   // Multiline edit box
				{

					// Setting value
					CString strValue=CComBSTR(ccvtValue.bstrVal);
					CStringArray strValueArray;
					str_split(strValue, strValueArray);
					int nLineNum = max(nLineNumMin, (int)strValueArray.GetCount());
					ListItem.Value.SetStringValue(strValueArray, nLineNum);

					// Setting default value
					CString strDefValue=CComBSTR(ccvtDefValue.bstrVal);
					CStringArray strDefValueArray;
					strDefValueArray.Add(strDefValue);
					nLineNum = max(nLineNum, max(nLineNumMin, (int)strDefValueArray.GetCount()));
					ListItem.DefValue.SetStringValue(strDefValueArray, nLineNum);

				}

			}break;
		
		case ATTVAL_INTEGER:
			{
				// Do not trust in VARIANT and MS.
				ASSERT(ccvtValue.vt==VT_I4);
				ASSERT(ccvtDefValue.vt==VT_I4);

				ListItem.Value.SetIntValue(ccvtValue.intVal);
				ListItem.DefValue.SetIntValue(ccvtDefValue.intVal);
			}break;
		
		case ATTVAL_DOUBLE:
			{
				// Do not trust in VARIANT and MS.
				ASSERT(ccvtValue.vt==VT_R8);
				ASSERT(ccvtDefValue.vt==VT_R8);
				
				ListItem.Value.SetDoubleValue(ccvtValue.dblVal);
				ListItem.DefValue.SetDoubleValue(ccvtDefValue.dblVal);
			}break;

		case ATTVAL_BOOLEAN:
			{
				// Do not trust in VARIANT and MS.
				ASSERT(ccvtValue.vt==VT_BOOL);
				ASSERT(ccvtDefValue.vt==VT_BOOL);

				ListItem.Value.SetBoolValue(ccvtValue.boolVal!=FALSE);
				ListItem.DefValue.SetBoolValue(ccvtDefValue.boolVal!=FALSE);

			}break;
	
		case ATTVAL_ENUM:
			{
				// Do not trust in VARIANT and MS.
				ASSERT(ccvtValue.vt==VT_BSTR);
				ASSERT(ccvtDefValue.vt==VT_BSTR);

				CComBSTR bstrValue(ccvtValue.bstrVal);
				CComBSTR bstrDefValue(ccvtDefValue.bstrVal);

				CString strValue=bstrValue;
				CString strDefValue=bstrDefValue;


				// Getting enum attributes from meta 
				CComPtr<IMgaMetaEnumItems> ccpMgaEnumItems;
				COMTHROW(ccpMetaAttribute->get_EnumItems(&ccpMgaEnumItems));



				int nSelItem=-1;
				int nSelDefaultItem=-1;
				int i=0;
				CStringArray strItemLabelArray;
				CStringArray strItemNameArray;

				MGACOLL_ITERATE(IMgaMetaEnumItem,ccpMgaEnumItems) 
				{
					// Getting item labels
					CComBSTR bstrItemLabel;
					COMTHROW(MGACOLL_ITER->get_DisplayedName(&bstrItemLabel));
					CString strItemLabel(bstrItemLabel);
					strItemLabelArray.Add(strItemLabel);

					CComBSTR bstrItemValue;
					COMTHROW(MGACOLL_ITER->get_Value(&bstrItemValue));
					CString strItemValue(bstrItemValue);
					strItemNameArray.Add(strItemValue);

					if(strItemValue==strValue)
					{
						nSelItem=i;
					}

					if(strItemValue==strDefValue)
					{
						nSelDefaultItem=i;
					}

					i++;
				}
				MGACOLL_ITERATE_END;

				// Selected and default item must be among the fixed list elements
				
				ASSERT(nSelItem>-1);
				ASSERT(nSelDefaultItem>-1);
				ASSERT(nSelItem<=strItemLabelArray.GetSize());
				ASSERT(nSelDefaultItem<=strItemLabelArray.GetSize());
				

				ListItem.Value.SetListValue(strItemLabelArray,strItemNameArray,nSelItem);
				ListItem.DefValue.SetListValue(strItemLabelArray,strItemNameArray, nSelDefaultItem);
			
			}break;

		case ATTVAL_REFERENCE: // Not implemented !!!
			{
				return false;
			}break;

		case ATTVAL_DYNAMIC:   // Not implemented !!!
			{
				return false;
			}break;

		default:
			{
				// Unknown MGA attribute type
				ASSERT(false);
			}
	}
	return true;

}

void CAttribute::WriteItemToMga(CListItem ListItem,const CMgaFCOPtrList& MgaFCOPtrList)
{


	POSITION posMetaAttribute=(POSITION)ListItem.dwKeyValue;
	CComPtr<IMgaMetaAttribute> ccpMetaAttribute=m_MetaAttributePtrList.GetAt(posMetaAttribute);


	CComBSTR bstrAttrName;
	COMTHROW(ccpMetaAttribute->get_Name(&bstrAttrName));

	POSITION posFCOPtrList=MgaFCOPtrList.GetHeadPosition();
	
	while(posFCOPtrList)
	{
		CComPtr<IMgaFCO> ccpCurrFCO=MgaFCOPtrList.GetNext(posFCOPtrList);

		
		if(ListItem.bIsDefault)
		{
			COMTHROW(ccpCurrFCO->ClearAttrByName(bstrAttrName));
		}
		else
		{
			switch(ListItem.Value.dataType)
			{
				case ITEMDATA_STRING:
				{	
					CString strValue;
					ListItem.Value.toString(strValue);

					strValue.Replace(_T("\r\n"), _T("\n"));

					ccpCurrFCO->__SetStrAttrByNameDisp(static_cast<const BSTR>(bstrAttrName), _bstr_t(strValue));					
				}break;
			
				case ITEMDATA_FIXED_LIST:
				{			
					CString strValue;
					strValue=ListItem.Value.stringListNames.GetAt(ListItem.Value.listVal);
					CComBSTR bstrValue(strValue);
					COMTHROW(ccpCurrFCO->put_StrAttrByName(bstrAttrName,bstrValue));
				}break;

				case ITEMDATA_INTEGER:
					{
						ccpCurrFCO->IntAttrByName[static_cast<const BSTR>(bstrAttrName)] = ListItem.Value.intVal;
					}break;

				case ITEMDATA_DOUBLE:
					{
						ccpCurrFCO->FloatAttrByName[static_cast<const BSTR>(bstrAttrName)] = ListItem.Value.doubleVal;
					}break;

				case ITEMDATA_BOOLEAN:
					{
						ccpCurrFCO->BoolAttrByName[static_cast<const BSTR>(bstrAttrName)] = ListItem.Value.boolVal ? VARIANT_TRUE : VARIANT_FALSE;
					}break;

				default:
					{
						// Invalid ItemData type found here
						ASSERT(0);
					}
			}					
		}
	}	
}

void CAttribute::WriteItemToMga(CListItem ListItem, CComPtr<IMgaProject> ccpProject)
{
	CString strValue;
	ListItem.Value.toString(strValue);
	strValue.Replace(_T("\r\n"), _T("\n"));
	CComBSTR bstrValue(strValue);

	switch ((size_t)ListItem.dwKeyValue) {
	case PROJECT_AUTHOR:
		COMTHROW(ccpProject->put_Author(bstrValue));
		break;
	case PROJECT_VERSION:
		COMTHROW(ccpProject->put_Version(bstrValue));
		break;
	case PROJECT_COMMENT:
		COMTHROW(ccpProject->put_Comment(bstrValue));
		break;
	default:
		break;
	}
}