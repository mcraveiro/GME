// Core.cpp : Implementation of DLL Exports.


// Note: Proxy/Stub Information
//      To merge the proxy/stub code into the object DLL, add the file 
//      dlldatax.c to the project.  Make sure precompiled headers 
//      are turned off for this file, and add _MERGE_PROXYSTUB to the 
//      defines for the project.  
//
//      If you are not running WinNT4.0 or Win95 with DCOM, then you
//      need to remove the following define from dlldatax.c
//      #define _WIN32_WINNT 0x0400
//
//      Further, if you are running MIDL without /Oicf switch, you also 
//      need to remove the following define from dlldatax.c.
//      #define USE_STUBLESS_PROXY
//
//      Modify the custom build rule for Core.idl by adding the following 
//      files to the Outputs.
//          Core_p.c
//          dlldata.c
//      To build a separate proxy/stub DLL, 
//      run nmake -f Coreps.mk in the project directory.

#include "stdafx.h"
#include "resource.h"

#include "CommonError.h"
#include "CommonSmart.h"

#include "CoreProject.h"
#include "CoreBinFile.h"
#include "CoreMetaProject.h"

#import "CoreLib.tlb" implementation_only no_namespace raw_method_prefix("") high_method_prefix("__") no_registry

#ifdef _MERGE_PROXYSTUB
extern "C" HINSTANCE hProxyDll;
#endif

CComModule _Module;

class CCoreCollectionHandler;

class CCoreCollectionHandlerTearOff : public ICoreMetaObjects
{
	friend class CCoreCollectionHandler;
	long refcount;
	CComPtr<ICoreMetaObjects> m_pInner;
	CComPtr<CCoreCollectionHandler> m_pHandler;

    public:
		CCoreCollectionHandlerTearOff() :
			refcount(1)
		{
			_pAtlModule->Lock();
		}

		virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, __RPC__deref_out void __RPC_FAR *__RPC_FAR *ppvObject);

        virtual ULONG STDMETHODCALLTYPE AddRef(void)
		{
			return InterlockedIncrement(&this->refcount);
		}

        virtual ULONG STDMETHODCALLTYPE Release(void)
		{
			long refcount = InterlockedDecrement(&this->refcount);
			if (refcount == 0)
			{
				delete this;
				_pAtlModule->Unlock();
			}
			return refcount;
		}

		// standard marshaller does not marshal array of interface pointers
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetAll(/* [in] */ long count, /* [size_is][out] */ ICoreMetaObject **p)
		{
			for (long i = 0; i < count; i++)
			{
				ICoreMetaObject* o = nullptr;
				HRESULT hr = get_Item(i+1, &o); // get_Item has 1-based index
				if (FAILED(hr))
				{
					for (i--; i >= 0; i--)
					{
						(*(p + i))->Release();
						(*(p + i)) = nullptr;
					}
					return hr;
				}
				(*(p + i)) = o;
			}
			return S_OK;
		}

		// Rest of this class delegates calls to m_pInner

		virtual HRESULT STDMETHODCALLTYPE GetTypeInfoCount(__RPC__out UINT *pctinfo)
		{
			return m_pInner->GetTypeInfoCount(pctinfo);
		}
        
        virtual HRESULT STDMETHODCALLTYPE GetTypeInfo(/* [in] */ UINT iTInfo, /* [in] */ LCID lcid, /* [out] */ __RPC__deref_out_opt ITypeInfo **ppTInfo)
		{
			return m_pInner->GetTypeInfo(iTInfo, lcid, ppTInfo);
		}
        
        virtual HRESULT STDMETHODCALLTYPE GetIDsOfNames(/* [in] */ __RPC__in REFIID riid, /* [size_is][in] */ __RPC__in_ecount_full(cNames) LPOLESTR *rgszNames,
            /* [range][in] */ __RPC__in_range(0,16384) UINT cNames, /* [in] */ LCID lcid, /* [size_is][out] */ __RPC__out_ecount_full(cNames) DISPID *rgDispId)
		{
			return m_pInner->GetIDsOfNames(riid, rgszNames, cNames, lcid, rgDispId);
		}
        
        virtual /* [local] */ HRESULT STDMETHODCALLTYPE Invoke(/* [in] */ DISPID dispIdMember, /* [in] */ REFIID riid, /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags, /* [out][in] */ DISPPARAMS *pDispParams, /* [out] */ VARIANT *pVarResult, /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr)
		{
			return m_pInner->Invoke(dispIdMember, riid, lcid, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr);
		}

		virtual HRESULT STDMETHODCALLTYPE get_Count(/* [retval][out] */ long *p)
		{
			return m_pInner->get_Count(p);
		}
        
        virtual HRESULT STDMETHODCALLTYPE get_Item(/* [in] */ long n, /* [retval][out] */ ICoreMetaObject **p)
		{
			return m_pInner->get_Item(n, p);
		}
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get__NewEnum(/* [retval][out] */ IUnknown **p)
		{
			return m_pInner->get__NewEnum(p);
		}

		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Insert(/* [in] */ ICoreMetaObject *p, /* [in] */ long at)
		{
			return m_pInner->Insert(p, at);
		}

		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Append(/* [in] */ ICoreMetaObject *p)
		{
			return m_pInner->Append(p);
		}
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Find(/* [in] */ ICoreMetaObject *p, /* [in] */ long start, /* [retval][out] */ long *res)
		{
			return m_pInner->Find(p, start, res);
		}
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Remove(/* [in] */ long n)
		{
			return m_pInner->Remove(n);
		}
};


class CCoreCollectionHandler : 
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CCoreCollectionHandler, &__uuidof(CoreCollectionHandler)>,
	public IUnknown
{

	DECLARE_GET_CONTROLLING_UNKNOWN()

	DECLARE_REGISTRY_RESOURCEID(IDR_CORECOLLECTIONHANDLER)

	CComPtr<IUnknown> m_pUnknownInner;
	HRESULT FinalConstruct()
	{
		HRESULT hr = CoGetStdMarshalEx(m_pOuterUnknown, SMEXF_HANDLER, &m_pUnknownInner);
		return hr;
	}

	static HRESULT WINAPI QI(void* pv, REFIID riid, LPVOID* ppv, DWORD_PTR dw)
	{
		CCoreCollectionHandler* _this = (CCoreCollectionHandler*)pv;
		return _this->QI(riid, ppv, dw);
	}

	HRESULT WINAPI QI(REFIID riid, LPVOID* ppv, DWORD_PTR dw)
	{
		if (ppv == NULL)
			return E_POINTER;
	
		static const ::GUID guids[] = {
//def lit(id): return "{%s, %s, %s, {%s, %s, %s}}" % ( hex(id.time_low), hex(id.time_mid), hex(id.time_hi_version), hex(id.clock_seq_hi_variant), hex(id.clock_seq_low), ', '.join((hex(ord(b)) for b in id.bytes[10:])))
//print "\n".join(["%s, // %ss %s" % (lit(l[0]), l[1], l[0]) for l in ids])
{0xc5aac2f0L, 0xc1fdL, 0x11d3L, {0x9aL, 0xd2L, 0x0, 0xaa, 0x0, 0xb6, 0xfe, 0x26}}, // MgaObjects c5aac2f0-c1fd-11d3-9ad2-00aa00b6fe26		
{0xc59c2210L, 0xc1fdL, 0x11d3L, {0x9aL, 0xd2L, 0x0, 0xaa, 0x0, 0xb6, 0xfe, 0x26}}, // MgaFolders c59c2210-c1fd-11d3-9ad2-00aa00b6fe26
{0xc5f4c2f0L, 0xc1fdL, 0x11d3L, {0x9aL, 0xd2L, 0x0, 0xaa, 0x0, 0xb6, 0xfe, 0x26}}, // MgaFCOs c5f4c2f0-c1fd-11d3-9ad2-00aa00b6fe26
{0xc6f3f340L, 0xc1fdL, 0x11d3L, {0x9aL, 0xd2L, 0x0, 0xaa, 0x0, 0xb6, 0xfe, 0x26}}, // MgaAttributes c6f3f340-c1fd-11d3-9ad2-00aa00b6fe26
{0xc71a26f0L, 0xc1fdL, 0x11d3L, {0x9aL, 0xd2L, 0x0, 0xaa, 0x0, 0xb6, 0xfe, 0x26}}, // MgaParts c71a26f0-c1fd-11d3-9ad2-00aa00b6fe26
{0xc7f19c60L, 0xc1fdL, 0x11d3L, {0x9aL, 0xd2L, 0x0, 0xaa, 0x0, 0xb6, 0xfe, 0x26}}, // MgaConnPoints c7f19c60-c1fd-11d3-9ad2-00aa00b6fe26
{0xc79f1840L, 0xc1fdL, 0x11d3L, {0x9aL, 0xd2L, 0x0, 0xaa, 0x0, 0xb6, 0xfe, 0x26}}, // MgaRegNodes c79f1840-c1fd-11d3-9ad2-00aa00b6fe26
{0xc66a6c60L, 0xc1fdL, 0x11d3L, {0x9aL, 0xd2L, 0x0, 0xaa, 0x0, 0xb6, 0xfe, 0x26}}, // MgaMetaFCOs c66a6c60-c1fd-11d3-9ad2-00aa00b6fe26
{0x43d12111L, 0x12caL, 0x11d3L, {0xa6L, 0xecL, 0x0, 0x60, 0x8, 0x2d, 0xf8, 0x84}}, // CoreMetaObjects 43d12111-12ca-11d3-a6ec-0060082df884
{0x43d12127L, 0x12caL, 0x11d3L, {0xa6L, 0xecL, 0x0, 0x60, 0x8, 0x2d, 0xf8, 0x84}}, // CoreMetaAttributes 43d12127-12ca-11d3-a6ec-0060082df884
{0x43d12111L, 0x22caL, 0x11d3L, {0xa6L, 0xecL, 0x0, 0x60, 0x8, 0x2d, 0xf8, 0x84}}, // CoreObjects 43d12111-22ca-11d3-a6ec-0060082df884
{0x43d12127L, 0x22caL, 0x11d3L, {0xa6L, 0xecL, 0x0, 0x60, 0x8, 0x2d, 0xf8, 0x84}}, // CoreAttributes 43d12127-22ca-11d3-a6ec-0060082df884
{0xc61352b0L, 0xc1fdL, 0x11d3L, {0x9aL, 0xd2L, 0x0, 0xaa, 0x0, 0xb6, 0xfe, 0x26}}, // MgaMetaAspects c61352b0-c1fd-11d3-9ad2-00aa00b6fe26
{0xc851e130L, 0xc1fdL, 0x11d3L, {0x9aL, 0xd2L, 0x0, 0xaa, 0x0, 0xb6, 0xfe, 0x26}}, // MgaMetaRoles c851e130-c1fd-11d3-9ad2-00aa00b6fe26
{0xc6922740L, 0xc1fdL, 0x11d3L, {0x9aL, 0xd2L, 0x0, 0xaa, 0x0, 0xb6, 0xfe, 0x26}}, // MgaMetaAttributes c6922740-c1fd-11d3-9ad2-00aa00b6fe26
{0xc6b85af0L, 0xc1fdL, 0x11d3L, {0x9aL, 0xd2L, 0x0, 0xaa, 0x0, 0xb6, 0xfe, 0x26}}, // MgaMetaFolders c6b85af0-c1fd-11d3-9ad2-00aa00b6fe26
{0xc761f8c0L, 0xc1fdL, 0x11d3L, {0x9aL, 0xd2L, 0x0, 0xaa, 0x0, 0xb6, 0xfe, 0x26}}, // MgaMetaPointerSpecs c761f8c0-c1fd-11d3-9ad2-00aa00b6fe26
{0xc795ed20L, 0xc1fdL, 0x11d3L, {0x9aL, 0xd2L, 0x0, 0xaa, 0x0, 0xb6, 0xfe, 0x27}}, // MgaMetaRegNodes c795ed20-c1fd-11d3-9ad2-00aa00b6fe27
{0xc8ec3720L, 0xc1fdL, 0x11d3L, {0x9aL, 0xd2L, 0x0, 0xaa, 0x0, 0xb6, 0xfe, 0x26}}, // MgaConstraints c8ec3720-c1fd-11d3-9ad2-00aa00b6fe26
{0xc8ec3720L, 0xc1fdL, 0x11d3L, {0x9aL, 0xd2L, 0x0, 0xaa, 0x0, 0xb6, 0xfe, 0x27}}, // MgaMetaParts c8ec3720-c1fd-11d3-9ad2-00aa00b6fe27
{0xc8ec3723L, 0xc1fdL, 0x11d3L, {0x9aL, 0xd2L, 0x0, 0xaa, 0x0, 0xb6, 0xfe, 0x27}}, // MgaMetaModels c8ec3723-c1fd-11d3-9ad2-00aa00b6fe27
{0xc8432623L, 0xc1fdL, 0x11d3L, {0x9aL, 0xd2L, 0x0, 0xaa, 0x0, 0xb6, 0xfe, 0x27}}, // MgaMetaConnJoints c8432623-c1fd-11d3-9ad2-00aa00b6fe27
{0xc8124623L, 0xc51dL, 0x11d3L, {0x9aL, 0xd2L, 0x0, 0xaa, 0x0, 0xb6, 0xfe, 0x27}}, // MgaMetaPointerItems c8124623-c51d-11d3-9ad2-00aa00b6fe27
{0xc8124623L, 0xc51dL, 0x1ad3L, {0x9aL, 0xd2L, 0x11, 0xaa, 0x1, 0x76, 0xfe, 0x27}}, // MgaMetaEnumItems c8124623-c51d-1ad3-9ad2-11aa0176fe27
{0xc8a5ec80L, 0xc1fdL, 0x11d3L, {0x9aL, 0xd2L, 0x0, 0xaa, 0x0, 0xb6, 0xfe, 0x26}}, // MgaAddOns c8a5ec80-c1fd-11d3-9ad2-00aa00b6fe26
{0xc87caa70L, 0xc1fdL, 0x11d3L, {0x9aL, 0xd2L, 0x0, 0xaa, 0x0, 0xb6, 0xfe, 0x26}}, // IMgaTerritorys c87caa70-c1fd-11d3-9ad2-00aa00b6fe26
{0xc8cda760L, 0xc1fdL, 0x11d3L, {0x9aL, 0xd2L, 0x0, 0xaa, 0x0, 0xb6, 0xfe, 0x26}}, // MgaComponents c8cda760-c1fd-11d3-9ad2-00aa00b6fe26
{0xc9d8df93L, 0xc1fdL, 0x11d3L, {0x9aL, 0xd2L, 0x0, 0xaa, 0x0, 0xb6, 0xfe, 0x26}}, // MgaClients c9d8df93-c1fd-11d3-9ad2-00aa00b6fe26
		};

		bool match = false;
		if (riid != __uuidof(IDispatch)) // microoptimization
		{
			for (int i = 0; i < sizeof(guids) / sizeof(guids[0]); i++)
			{
				if (guids[i] == riid)
				{
					match = true;
					break;
				}
			}
		}
		HRESULT hr;
		CComPtr<ICoreMetaObjects> pCollection;
		if (FAILED(hr = m_pUnknownInner->QueryInterface(riid, (void**)&pCollection)))
			return hr;
		if (!match)
		{
			*ppv = pCollection.Detach();
			return S_OK;
		}
		CCoreCollectionHandlerTearOff* tearoff = new CCoreCollectionHandlerTearOff(); // refcount == 1
		// We know that all the IMgaXXXs have the same vtable, so this works
		tearoff->m_pInner = pCollection;
		tearoff->m_pHandler = this;
		*ppv = (ICoreMetaObjects*)tearoff;
		return S_OK;
	}

BEGIN_COM_MAP(CCoreCollectionHandler)
	COM_INTERFACE_ENTRY(IUnknown)
	COM_INTERFACE_ENTRY_FUNC_BLIND(0, QI)
END_COM_MAP()

};

HRESULT STDMETHODCALLTYPE CCoreCollectionHandlerTearOff::QueryInterface(REFIID riid, __RPC__deref_out void __RPC_FAR *__RPC_FAR *ppvObject)
{
	return this->m_pHandler->QueryInterface(riid, ppvObject);
}

BEGIN_OBJECT_MAP(ObjectMap)
OBJECT_ENTRY(__uuidof(CoreProject), CCoreProject)
OBJECT_ENTRY(__uuidof(CoreBinFile), CCoreBinFile)
OBJECT_ENTRY(__uuidof(CoreMetaProject), CCoreMetaProject)
OBJECT_ENTRY(__uuidof(CoreCollectionHandler), CCoreCollectionHandler)
END_OBJECT_MAP()

/////////////////////////////////////////////////////////////////////////////
// DLL Entry Point

extern "C"
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
    lpReserved;
#ifdef _MERGE_PROXYSTUB
    if (!PrxDllMain(hInstance, dwReason, lpReserved))
        return FALSE;
#endif
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        _Module.Init(ObjectMap, hInstance, &LIBID_MGACoreLib);
        DisableThreadLibraryCalls(hInstance);
    }
    else if (dwReason == DLL_PROCESS_DETACH)
        _Module.Term();
    return TRUE;    // ok
}

/////////////////////////////////////////////////////////////////////////////
// Used to determine whether the DLL can be unloaded by OLE

STDAPI DllCanUnloadNow(void)
{
#ifdef _MERGE_PROXYSTUB
    if (PrxDllCanUnloadNow() != S_OK)
        return S_FALSE;
#endif
    return (_Module.GetLockCount()==0) ? S_OK : S_FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// Returns a class factory to create an object of the requested type

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
#ifdef _MERGE_PROXYSTUB
    if (PrxDllGetClassObject(rclsid, riid, ppv) == S_OK)
        return S_OK;
#endif
    return _Module.GetClassObject(rclsid, riid, ppv);
}

/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer(void)
{
#ifdef _MERGE_PROXYSTUB
    HRESULT hRes = PrxDllRegisterServer();
    if (FAILED(hRes))
        return hRes;
#endif
    // registers object, typelib and all interfaces in typelib
    return _Module.RegisterServer(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
#ifdef _MERGE_PROXYSTUB
    PrxDllUnregisterServer();
#endif
    return _Module.UnregisterServer(TRUE);
}

HRESULT check_location_compatibility(ICoreMetaObject *newobj, ICoreMetaObject *oldobj) {
	CComPtr<ICoreMetaProject> proj1, proj2;
	COMTRY {
		if(!oldobj) {
			COMTHROW(newobj->get_Project(&proj1));
			//instead of proj1->get_Name
			if( &CCoreMetaProject::get_Name != &ICoreMetaProject::get_Name) return E_SAMEPROJECT;
		}
		else {
			COMTHROW(oldobj->get_Project(&proj2));
			if(proj1 != proj2) return E_SAMEPROJECT;
		}
	}
	COMCATCH(;)
	
}

HRESULT check_location_compatibility(ICoreMetaAttribute *newobj, ICoreMetaAttribute *oldobj) {
	CComPtr<ICoreMetaObject> oldo, newo;
	COMTRY {
		COMTHROW(newobj->get_Object(&newo));
		if(oldobj) COMTHROW(oldobj->get_Object(&oldo));
		COMTHROW(check_location_compatibility(newo, oldo));
	}
	COMCATCH(;)
}

HRESULT check_location_compatibility(ICoreObject *newobj, ICoreObject *oldobj) {
	CComPtr<ICoreProject> proj1, proj2;
	COMTRY {
		if(!oldobj) {
			COMTHROW(newobj->get_Project(&proj1));
			//instead of proj1->OpenProject
			if( &CCoreProject::OpenProject != &ICoreProject::OpenProject) return E_SAMEPROJECT;
		}
		else {
			COMTHROW(oldobj->get_Project(&proj2));
			if(proj1 != proj2) return E_SAMEPROJECT;
		}
	}
	COMCATCH(;)
	
}

HRESULT check_location_compatibility(ICoreAttribute *newobj, ICoreAttribute *oldobj) {
	CComPtr<ICoreObject> oldo, newo;
	COMTRY {
		COMTHROW(newobj->get_Object(&newo));
		if(oldobj) COMTHROW(oldobj->get_Object(&oldo));
		COMTHROW(check_location_compatibility(newo, oldo));
	}
	COMCATCH(;)
}

#ifdef _ATL_DEBUG_INTERFACES
bool IsQIThunk(IUnknown *p) {
	// Is p a thunk?
	// dynamic_cast can't work, since ATL::_QIThunk has no superclass (not even IUnknown)
	//  solution: compare virtual function tables
	ATL::_QIThunk dummy((IUnknown*)(void*)1, L"dummy", IID_IUnknown, 0, false);

	return *((int**)(void*)p) == *((int**)(void*)&dummy);
}
#endif
