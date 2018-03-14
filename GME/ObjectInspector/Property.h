// Property.h: interface for the CProperty class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROPERTY_H__A99D2F88_1048_42DA_9C89_314D35AE86DF__INCLUDED_)
#define AFX_PROPERTY_H__A99D2F88_1048_42DA_9C89_314D35AE86DF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CProperty  
{
	bool m_bIsAllFCO;
	bool m_bIsAllMARS;
	bool m_bIsAllConnection;
	bool m_bIsAllFolder;


	void CreateGeneralList(const CMgaFCOPtrList& MgaFCOPtrList,const CMgaFolderPtrList& MgaFolderPtrList,CArray<CListItem,CListItem&> &ListItemArray);

	void GetGeneralProperties(const CComPtr<IMgaObject>& ccpMgaObject,
									 CString &strType, CString &strKindName,
									 CString &strMetaID, CString &strObjectID, 
									 CString &strRelID, CString &strGUID, bool &bIsTypeDirty,
									 bool &bIsKindNameDirty, bool bIsMultiple, 
									 bool bIsFirst);

	void CreateFCOList(const CMgaFCOPtrList& MgaFCOPtrList,CArray<CListItem,CListItem&> &ListItemArray);

	void CreateMarsList(const CMgaFCOPtrList& MgaFCOPtrList,CArray<CListItem,CListItem&> &ListItemArray);
	void CreateConnectionList(const CMgaFCOPtrList& MgaFCOPtrList,CArray<CListItem,CListItem&> &ListItemArray);
	void CreateFolderList(const CMgaFolderPtrList& MgaFolderPtrList,CArray<CListItem,CListItem&> &ListItemArray);



public:
	CProperty();
	virtual ~CProperty();
	void CreateList(const CMgaFCOPtrList& MgaFCOPtrList,const CMgaFolderPtrList& MgaFolderPtrList,CArray<CListItem,CListItem&> &ListItemArray);
};

#endif // !defined(AFX_PROPERTY_H__A99D2F88_1048_42DA_9C89_314D35AE86DF__INCLUDED_)
