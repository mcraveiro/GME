// MgaModel.h : Declaration of the CMgaModel

#ifndef __MGAMODEL_H_
#define __MGAMODEL_H_

#include "resource.h"       // main symbols
#include "MgaFCO.h"
#include "MgaFolder.h"


/////////////////////////////////////////////////////////////////////////////
// CMgaModel
class ATL_NO_VTABLE CMgaModel : 
	public CComCoClass<CMgaModel, &__uuidof(MgaModel)>,
	public IMgaContainerImpl< IMgaFCOImpl< 
												CComObjectRootEx<CComSingleThreadModel>,
											    IDispatchImpl<IMgaModel, &__uuidof(IMgaModel), &__uuidof(__MGALib)> > >,
	public ISupportErrorInfoImpl<&__uuidof(IMgaModel)>
{
public:
	CMgaModel()
	{
	}


DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CMgaModel)
	COM_INTERFACE_ENTRY(IMgaModel)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY_IID(__uuidof(IMgaFCO),IMgaModel)
	COM_INTERFACE_ENTRY_IID(__uuidof(IMgaObject),IMgaModel)
	COM_INTERFACE_ENTRY_IID(IID_ISupportErrorInfo, IMyErrorInfoBase)
//	COM_INTERFACE_ENTRY_IID(IID_IMgaContainer,IMgaModel)
END_COM_MAP()

// IMgaModel
public:
	STDMETHOD(DeriveChildObject)(IMgaFCO *base, IMgaMetaRole *role, VARIANT_BOOL inst, IMgaFCO **newobj) {
		return inFCO->DeriveFCO(base, role, inst, newobj); }
	STDMETHOD(CreateChildObject)(IMgaMetaRole *role, IMgaFCO **newobj) {
		return inFCO->CreateChildObject(role, newobj); }
	STDMETHOD(get_ChildDerivedFrom)(IMgaFCO *baseobj, IMgaFCO **pVal) {
												    return inFCO->get_ChildDerivedFrom(baseobj, pVal); }
	STDMETHOD(GetChildDerivedFromDisp)(IMgaFCO *baseobj, IMgaFCO **pVal) {
												    return inFCO->get_ChildDerivedFrom(baseobj, pVal); }
	STDMETHOD(CreateReference)( IMgaMetaRole *meta,  IMgaFCO *target,  IMgaFCO **newobj) {
		return inFCO->CreateReference( meta,  target,  newobj); }
	STDMETHOD(get_AspectParts)(IMgaMetaAspect * asp, unsigned int filter, IMgaParts **pVal) {
		return inFCO->get_AspectParts(asp, filter, pVal);	}
	STDMETHOD(GetAspectPartsDisp)(IMgaMetaAspect * asp, unsigned int filter, IMgaParts **pVal) {
		return inFCO->get_AspectParts(asp, filter, pVal);	}
	STDMETHOD(CopyFCOs)(IMgaFCOs *list,  IMgaMetaRoles *rlist, IMgaFCOs **objs) { 
		return inFCO->CopyFCOs(list, rlist, objs); } 
	STDMETHOD(MoveFCOs)(IMgaFCOs *list,  IMgaMetaRoles *rlist, IMgaFCOs **objs) { 
		return inFCO->MoveFCOs(list, rlist, objs); } 
	STDMETHOD(CopyFCODisp)(IMgaFCO *obj,  IMgaMetaRole *role, IMgaFCO **nobj) { 
		return inFCO->CopyFCODisp( obj, role, nobj); } 
	STDMETHOD(MoveFCODisp)(IMgaFCO *obj,  IMgaMetaRole *role, IMgaFCO **nobj) { 
		return inFCO->MoveFCODisp( obj, role, nobj); } 
	STDMETHOD(CreateSimpleConn)( IMgaMetaRole *meta,  IMgaFCO *src,  IMgaFCO *dst, IMgaFCOs *srcrefs, IMgaFCOs *dstrefs, IMgaFCO **newobj) {
		return inFCO->CreateSimpleConn(meta, src, dst, srcrefs, dstrefs, newobj); }
	STDMETHOD(CreateSimpleConnDisp)( IMgaMetaRole *meta,  IMgaFCO *src,  IMgaFCO *dst, IMgaFCO *srcref, IMgaFCO *dstref, IMgaFCO **newobj) {
		return inFCO->CreateSimpleConnDisp(meta, src, dst, srcref, dstref, newobj); }
	STDMETHOD(CreateSimplerConnDisp)( IMgaMetaRole *meta,  IMgaFCO *src,  IMgaFCO *dst, IMgaFCO **newobj) { return CreateSimpleConnDisp(meta, src, dst, 0, 0, newobj); }
	STDMETHOD(AddInternalConnections)(IMgaFCOs *inobjs, IMgaFCOs **objs) { 
		return inFCO->AddInternalConnections(inobjs, objs);}
	STDMETHOD(get_ChildRelIDCounter)(long *pVal) { 
		return inFCO->get_ChildRelIDCounter(pVal); }
	STDMETHOD(put_ChildRelIDCounter)(long pVal) { 
		return inFCO->put_ChildRelIDCounter(pVal); }
	STDMETHOD(GetSourceControlInfo)( long *p_scInfo) {
		return inFCO->GetSourceControlInfo( p_scInfo);}
	};
#endif //__MGAMODEL_H_
