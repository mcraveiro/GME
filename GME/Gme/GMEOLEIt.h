#if !defined(AFX_GMEOLEIT_H__338DD18C_3C58_4C65_9202_2B5E00CF740A__INCLUDED_)
#define AFX_GMEOLEIT_H__338DD18C_3C58_4C65_9202_2B5E00CF740A__INCLUDED_

#include "GuiMeta.h"
#include "GMEView.h"
#include "MainFrm.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GMEOLEIt.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGMEOLEIt command target

class CGMEOLEIt : public CCmdTarget
{
	DECLARE_DYNCREATE(CGMEOLEIt)

	CGMEOLEIt();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

// Implementation
protected:
	CGMEView *		m_theView;
	CComPtr<IMgaModel> m_theMgaModel;
	virtual ~CGMEOLEIt();

	// this method is called by PRECONDITION_VALID_MODEL macro
	bool AmIValid();

	// mga related helpers
	static bool createObjs( CComPtr<IMgaModel>& mm, LPCTSTR partname, LPCTSTR newnm, IMgaFCO** pNewObj );
	static bool cloneObjs( CComPtr<IMgaModel>& mm, IMgaFCO* pExistingObj, LPCTSTR partname, LPCTSTR newnm, IMgaFCO** pNewObj );
	static void rename( CComPtr<IMgaFCO> pFco, LPCTSTR pName);
	static CComPtr<IMgaObject> myParent( const CComPtr<IMgaObject>& obj);
	static CComPtr<IMgaFCO> fcoByPath( const CComPtr<IMgaModel>& mod, LPCTSTR p_objPath);
	static CComPtr<IMgaFCO> getChildInByName( CComPtr<IMgaModel> model, LPCTSTR name );
	static bool isValidChildIn( CComPtr<IMgaModel> model, CComPtr<IMgaFCO> fco);
	static CComPtr<IMgaMetaRole> metaRolePtrInByName( CComPtr<IMgaModel> p_model, CComBSTR p_part );
	static CComPtr<IMgaMetaRole> connMetaRolePtrInBetween( CComPtr<IMgaModel> p_model, CComPtr<IMgaFCO> p_src, CComPtr<IMgaFCO> p_dst );
	static CComPtr<IMgaMetaRole> connMetaRolePtrInBetween( CComPtr<IMgaModel> p_model, CComPtr<IMgaFCO> p_srcRole1, CComPtr<IMgaFCO> p_srcRole2, CComPtr<IMgaFCO> p_dstRole1, CComPtr<IMgaFCO> p_dstRole2);
	static CComPtr<IMgaMetaRole> connMetaRolePtrInBetween( CComPtr<IMgaModel> p_model, const CComBSTR& src_localpath, const CComBSTR& dst_localpath);
	static CComBSTR roleNameOf( CComPtr<IMgaFCO> one_fco );
	static objtype_enum myTypeIs( CComPtr<IMgaFCO> fco);
	static void refPortFinderAndChainBuilder( LPCTSTR p_portRole, CComPtr<IMgaFCO> p_ref, CComPtr<IMgaFCO>& p_portResult, CComPtr<IMgaFCOs>& p_refChainResult );
	static bool areCollectionsEqual( const CComPtr<IMgaFCOs>& op1, const CComPtr<IMgaFCOs>& op2);
	static bool refChainBuilder2( CComPtr<IMgaFCO> p_ref, CComPtr<IMgaFCO> port, CComPtr<IMgaFCOs>& chain);

	// gui related helpers
	void moveAspect( int dir);
	void moveView( int dir);
	void setObjPos( CComPtr<IMgaFCO> pChild, LPCTSTR pAspectName, long parX, long parY);

	// transaction related helper methods
	bool m_isInUserInitiatedTransaction;
	bool m_initedByMe;
	bool start();
	bool end();
	bool beginTrans();
	bool commitTrans();
	bool abortTrans();
	LPDISPATCH putInTerr( LPDISPATCH);
	CComPtr<IMgaFCO> putInTerr( const CComPtr<IMgaFCO>& fco);

	// Generated message map functions
	//{{AFX_MSG(CGMEOLEIt)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	// OLE dispatch map functions
	afx_msg LPDISPATCH GetAspects();
	afx_msg void SetAspects(LPDISPATCH newValue);
	afx_msg void Print();
	afx_msg void PrintDialog();
	afx_msg void DumpWindowsMetaFile(LPCTSTR filePath);
	afx_msg void CheckConstraints();
	afx_msg void RunComponent(LPCTSTR appID);
	afx_msg void RunComponentDialog();
	afx_msg void Close();
	afx_msg void GrayOutFCO(BOOL bGray, BOOL bNeighbours, LPDISPATCH mgaFCOs);
	afx_msg void GrayOutHide();
	afx_msg void ShowSetMembers(LPDISPATCH mgaFCO);
	afx_msg void HideSetMembers();
	afx_msg void Zoom(long percent);
	afx_msg void ZoomTo(LPDISPATCH mgaFCOs);
	afx_msg void Scroll(long bar, long scroll);
	afx_msg void DumpModelGeometryXML(LPCTSTR filePath);

	afx_msg BOOL GetValid();
	afx_msg void SetValid(BOOL);
	afx_msg LPDISPATCH GetMgaModel();
	afx_msg void SetMgaModel(LPDISPATCH); 

	afx_msg void ShowFCO( LPCTSTR, BOOL);
	afx_msg void ShowFCOPtr( LPDISPATCH, BOOL);
	afx_msg LPDISPATCH Child( LPCTSTR);
	afx_msg LPDISPATCH Create( LPCTSTR, LPCTSTR);
	afx_msg LPDISPATCH CreateInChild( LPCTSTR, LPCTSTR, LPCTSTR);
	afx_msg LPDISPATCH CreateInChildFCO( LPDISPATCH, LPCTSTR, LPCTSTR);
	afx_msg LPDISPATCH Duplicate( LPCTSTR, LPCTSTR);
	afx_msg LPDISPATCH DuplicateFCO( LPDISPATCH, LPCTSTR);
	afx_msg void Rename( LPCTSTR, LPCTSTR);
	afx_msg void SetName( LPDISPATCH, LPCTSTR);
	afx_msg void Include( LPCTSTR, LPCTSTR);
	afx_msg void IncludeFCO( LPDISPATCH, LPDISPATCH);
	afx_msg void Exclude( LPCTSTR, LPCTSTR);
	afx_msg void ExcludeFCO( LPDISPATCH, LPDISPATCH);
	afx_msg LPDISPATCH Connect( LPCTSTR, LPCTSTR, LPCTSTR);
	afx_msg void Disconnect( LPCTSTR, LPCTSTR, LPCTSTR);
	afx_msg LPDISPATCH ConnectThruPort( LPCTSTR, LPCTSTR, LPCTSTR, LPCTSTR, LPCTSTR);
	afx_msg LPDISPATCH ConnectThruPortFCO( LPDISPATCH, LPDISPATCH, LPDISPATCH, LPDISPATCH, LPCTSTR);
	afx_msg void DisconnectThruPort( LPCTSTR, LPCTSTR, LPCTSTR, LPCTSTR, LPCTSTR);
	afx_msg LPDISPATCH ConnectFCOs( LPDISPATCH, LPDISPATCH, LPCTSTR);
	afx_msg void DisconnectFCOs( LPDISPATCH, LPDISPATCH, LPCTSTR);
	afx_msg void Refer( LPCTSTR, LPCTSTR);
	afx_msg void ReferFCO( LPDISPATCH, LPDISPATCH);
	afx_msg void ClearRef( LPCTSTR);
	afx_msg void ClearRefFCO( LPDISPATCH);
	afx_msg void FollowRef( LPCTSTR);
	afx_msg void FollowRefFCO( LPDISPATCH);
	afx_msg LPDISPATCH NullFCO();

	afx_msg void SetAttribute( LPCTSTR, LPCTSTR, VARIANT&);
	afx_msg VARIANT GetAttribute( LPCTSTR, LPCTSTR);

	afx_msg void SetAttributeFCO( LPDISPATCH, LPCTSTR, VARIANT&);
	afx_msg VARIANT GetAttributeFCO( LPDISPATCH, LPCTSTR);

	afx_msg LPDISPATCH SubType( LPCTSTR, LPCTSTR);
	afx_msg LPDISPATCH Instantiate( LPCTSTR, LPCTSTR);
	afx_msg LPDISPATCH SubTypeFCO( LPDISPATCH, LPCTSTR);
	afx_msg LPDISPATCH InstantiateFCO( LPDISPATCH, LPCTSTR);

	afx_msg void BeginTransaction( );
	afx_msg void CommitTransaction( );
	afx_msg void AbortTransaction( );
	afx_msg VARIANT_BOOL IsInTransaction( );
	afx_msg LPDISPATCH PutInTerritory( LPDISPATCH);

	afx_msg void Help( );

	afx_msg void SetSelected( LPCTSTR);
	afx_msg BSTR GetSelected( );
	afx_msg void SetSelectedFCOs( LPDISPATCH);
	afx_msg LPDISPATCH GetSelectedFCOs( );

	afx_msg CString GetCurrentAspect(void);
	afx_msg void SetCurrentAspect(const CString& aspectName);
	afx_msg void NextAspect( );
	afx_msg void PrevAspect( );
	afx_msg void Next( );
	afx_msg void Prev( );
	afx_msg void Position( LPCTSTR, LPCTSTR, long, long);
	afx_msg void PositionFCO( LPDISPATCH, LPCTSTR, long, long);

	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

	BEGIN_DUAL_INTERFACE_PART(Dual, IGMEOLEIt)
		STDMETHOD(get_Valid)(THIS_ VARIANT_BOOL* isValid);
		STDMETHOD(get_MgaModel)(THIS_ IMgaModel** model);
		STDMETHOD(get_Aspects)(THIS_ IGMEOLEColl** coll);

		STDMETHOD(Print)(THIS);
		STDMETHOD(PrintDialog)(THIS);
		STDMETHOD(DumpWindowsMetaFile)(THIS_ BSTR filePath);
		STDMETHOD(CheckConstraints)(THIS);
		STDMETHOD(RunComponent)(THIS_ BSTR appID);
		STDMETHOD(RunComponentDialog)(THIS);
		STDMETHOD(Close)(THIS);
		STDMETHOD(GrayOutFCO)(THIS_ VARIANT_BOOL bGray, VARIANT_BOOL bNeighbours, IMgaFCOs* mgaFCO);
		STDMETHOD(GrayOutHide)(THIS);
		STDMETHOD(ShowSetMembers)(THIS_ IMgaFCO* mgaFCO);
		STDMETHOD(HideSetMembers)(THIS);
		STDMETHOD(Zoom)(THIS_ DWORD percent);
		STDMETHOD(ZoomTo)(THIS_ IMgaFCOs* mgaFCOs);
		STDMETHOD(Scroll)(THIS_ bar_enum bar, scroll_enum scroll);
		STDMETHOD(DumpModelGeometryXML)(THIS_ BSTR filePath);

		STDMETHOD(ShowFCO)(THIS_ BSTR path, VARIANT_BOOL inparent);
		STDMETHOD(ShowFCOPtr)(THIS_ IMgaFCO* ptr, VARIANT_BOOL inparent);
		STDMETHOD(Child)(THIS_ BSTR name, IMgaFCO** child_fco);
		STDMETHOD(Create)(THIS_ BSTR part, BSTR name, IMgaFCO** new_fco);
		STDMETHOD(CreateInChild)(THIS_ BSTR child_as_parent, BSTR part, BSTR name, IMgaFCO** new_fco);
		STDMETHOD(CreateInChildFCO)(THIS_ IMgaFCO* child_as_parent, BSTR part, BSTR name, IMgaFCO** new_fco);

		STDMETHOD(Duplicate)(THIS_ BSTR existing_fco_name, BSTR new_fco_name, IMgaFCO** new_fco);
		STDMETHOD(DuplicateFCO)(THIS_ IMgaFCO* existing_fco, BSTR new_fco_name, IMgaFCO** new_fco);
		STDMETHOD(Rename)(THIS_ BSTR oldname, BSTR newname);
		STDMETHOD(SetName)(THIS_ IMgaFCO* fco, BSTR name);
		STDMETHOD(Include)(THIS_ BSTR setname, BSTR fconame);
		STDMETHOD(IncludeFCO)(THIS_ IMgaSet* set, IMgaFCO* fco);
		STDMETHOD(Exclude)(THIS_ BSTR setname, BSTR fconame);
		STDMETHOD(ExcludeFCO)(THIS_ IMgaSet* set, IMgaFCO* fco);
		STDMETHOD(Connect)(THIS_ BSTR name1, BSTR name2, BSTR connection, IMgaConnection** conn );
		STDMETHOD(Disconnect)(THIS_  BSTR part1, BSTR part2, BSTR connection);
		STDMETHOD(ConnectThruPort)(THIS_ BSTR part1, BSTR port1, BSTR part2, BSTR port2, BSTR connection, IMgaConnection ** conn );
		STDMETHOD(ConnectThruPortFCO)(THIS_ IMgaFCO* part1, IMgaFCO* port1, IMgaFCO* part2, IMgaFCO* port2, BSTR connection_role, IMgaConnection ** conn);
		STDMETHOD(DisconnectThruPort)(THIS_ BSTR part1, BSTR port1, BSTR part2, BSTR port2, BSTR connection);
		STDMETHOD(ConnectFCOs)(THIS_  IMgaFCO* part1,  IMgaFCO* part2,  BSTR connection, IMgaConnection ** conn );
		STDMETHOD(DisconnectFCOs)(THIS_  IMgaFCO* part1,  IMgaFCO* part2,  BSTR connection);
		STDMETHOD(Refer)(THIS_  BSTR reference,  BSTR referred);
		STDMETHOD(ReferFCO)(THIS_  IMgaReference* reference,  IMgaFCO* referred);
		STDMETHOD(ClearRef)(THIS_ BSTR reference);
		STDMETHOD(ClearRefFCO)(THIS_ IMgaReference* reference);
		STDMETHOD(FollowRef)(THIS_ BSTR reference);
		STDMETHOD(FollowRefFCO)(THIS_ IMgaReference* reference);
		STDMETHOD(NullFCO)(THIS_ IMgaFCO** nullobj);

		STDMETHOD(SetAttribute)(THIS_ BSTR name,  BSTR attr_name,  VARIANT value);
		STDMETHOD(GetAttribute)(THIS_ BSTR name,  BSTR attr_name, VARIANT* value);

		STDMETHOD(SetAttributeFCO)(THIS_ IMgaFCO* fco,  BSTR attr_name,  VARIANT value);
		STDMETHOD(GetAttributeFCO)(THIS_ IMgaFCO* fco,  BSTR attr_name, VARIANT* value);

		STDMETHOD(SubType)( THIS_ BSTR base, BSTR name_of_subtype, IMgaFCO** subtype);	
		STDMETHOD(Instantiate)( THIS_ BSTR base, BSTR name_of_instance, IMgaFCO** instance);

		STDMETHOD(SubTypeFCO)( THIS_ IMgaFCO* base, BSTR name_of_subtype, IMgaFCO** subtype);
		STDMETHOD(InstantiateFCO)( THIS_ IMgaFCO* base, BSTR name_of_instance, IMgaFCO** instance);

		STDMETHOD(BeginTransaction)(THIS);
		STDMETHOD(CommitTransaction)(THIS);
		STDMETHOD(AbortTransaction)(THIS);
		STDMETHOD(IsInTransaction)(THIS_ VARIANT_BOOL* intrans);
		STDMETHOD(PutInTerritory)(THIS_ IMgaFCO* fco, IMgaFCO** newfco);

		STDMETHOD(Help)(THIS);

		STDMETHOD(SetSelected)(THIS_ BSTR space_delimited_string );
		STDMETHOD(GetSelected)(THIS_ BSTR * space_delimited_string );
		STDMETHOD(SetSelectedFCOs)(THIS_ IMgaFCOs* fcos_to_select);
		STDMETHOD(GetSelectedFCOs)(THIS_ IMgaFCOs** selected_fcos);

		STDMETHOD(GetCurrentAspect)(THIS_ BSTR* aspectName);
		STDMETHOD(SetCurrentAspect)(THIS_ BSTR aspectName);
		STDMETHOD(NextAspect)(THIS);
		STDMETHOD(PrevAspect)(THIS);
		STDMETHOD(Next)(THIS);
		STDMETHOD(Prev)(THIS);
		STDMETHOD(Position)(THIS_ BSTR child, BSTR aspect, long parX, long parY);
		STDMETHOD(PositionFCO)(THIS_ IMgaFCO* child, BSTR aspect, long parX, long parY);

	END_DUAL_INTERFACE_PART(Dual)

	//     add declaration of ISupportErrorInfo implementation
	//     to indicate we support the OLE Automation error object
	DECLARE_DUAL_ERRORINFO()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GMEOLEIT_H__338DD18C_3C58_4C65_9202_2B5E00CF740A__INCLUDED_)
