//################################################################################################
//
// Annotator Decorator COM side Implementation
//	AnnotatorDecoratorImpl.h
//
//################################################################################################

#ifndef __ANNOTATORDECORATORIMPL_H_
#define __ANNOTATORDECORATORIMPL_H_



#include "StdAfx.h"
#include "Gme.h"
#include "resource.h"       // main symbols
#include "DecoratorInterface.h"


class DecoratorInterface;

//################################################################################################
//
// CLASS : CAnnotatorDecoratorImpl
//
//################################################################################################

class ATL_NO_VTABLE CAnnotatorDecoratorImpl :
	public CComObjectRootEx<CComSingleThreadModel>,
	public IMgaElementDecorator,
	public CComCoClass<CAnnotatorDecoratorImpl, &CLSID_AnnotatorDecorator>
{
protected:
	DecoratorSDK::DecoratorInterface*	m_pElementDecorator;
	bool								m_bLocationSet;
	bool								m_bInitCallFromEx;

	BSTR								m_bstrName;
	VARIANT								m_vValue;

public:
	CAnnotatorDecoratorImpl();
	~CAnnotatorDecoratorImpl();

	DECLARE_REGISTRY_RESOURCEID( IDR_DECORATOR )
	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP( CAnnotatorDecoratorImpl )
		COM_INTERFACE_ENTRY( IMgaElementDecorator )
		COM_INTERFACE_ENTRY( IMgaDecoratorCommon )
		COM_INTERFACE_ENTRY( IMgaDecorator )
	END_COM_MAP()

public:
	// =============== inherited from IMgaDecorator
	STDMETHOD( Initialize )						( /*[in]*/ IMgaProject* pProject, /*[in]*/ IMgaMetaPart* pPart, /*[in]*/ IMgaFCO* pFCO );
	STDMETHOD( Destroy )						( void );
	STDMETHOD( GetMnemonic )					( /*[out]*/ BSTR* bstrMnemonic );
	STDMETHOD( GetFeatures )					( /*[out]*/ feature_code* pFeatureCodes );
	STDMETHOD( SetParam )						( /*[in]*/ BSTR bstrName, /*[in]*/ VARIANT vValue );
	STDMETHOD( GetParam )						( /*[in]*/ BSTR bstrName, /*[out]*/ VARIANT* pvValue );
	STDMETHOD( SetActive )						( /*[in]*/ VARIANT_BOOL bIsActive );
	STDMETHOD( GetPreferredSize )				( /*[out]*/ LONG* plWidth, /*[out]*/ LONG* plHeight );
	STDMETHOD( SetLocation )					( /*[in]*/ LONG sx, /*[in]*/ LONG sy, /*[in]*/ LONG ex, /*[in]*/ LONG ey );
	STDMETHOD( GetLocation )					( /*[out]*/ LONG* sx, /*[out]*/ LONG* sy, /*[out]*/ LONG* ex, /*[out]*/ LONG* ey );
	STDMETHOD( GetLabelLocation )				( /*[out]*/ LONG* sx, /*[out]*/ LONG* sy, /*[out]*/ LONG* ex, /*[out]*/ LONG* ey );
	STDMETHOD( GetPortLocation )				( /*[in]*/ IMgaFCO* fco, /*[out]*/ LONG* sx, /*[out]*/ LONG* sy, /*[out]*/ LONG* ex, /*[out]*/ LONG* ey );
	STDMETHOD( GetPorts )						( /*[out, retval]*/ IMgaFCOs** portFCOs );
	STDMETHOD( Draw )							( /*[in]*/ ULONG hdc );
	STDMETHOD( SaveState )						( void );

	// =============== IMgaElementDecorator
	STDMETHOD( InitializeEx )					( /*[in]*/ IMgaProject* pProject, /*[in]*/ IMgaMetaPart* pPart, /*[in]*/ IMgaFCO* pFCO, /*[in]*/ IMgaCommonDecoratorEvents* eventSink, /*[in]*/ ULONGLONG parentWnd );
	STDMETHOD( DrawEx )							( /*[in]*/ ULONG hdc, /*[in]*/ ULONGLONG gdipGraphics );
	STDMETHOD( SetSelected )					( /*[in]*/ VARIANT_BOOL bIsSelected );
	STDMETHOD( MouseMoved )						( /*[in]*/ ULONG nFlags, /*[in]*/ LONG pointx, /*[in]*/ LONG pointy, /*[in]*/ ULONGLONG transformHDC );
	STDMETHOD( MouseLeftButtonDown )			( /*[in]*/ ULONG nFlags, /*[in]*/ LONG pointx, /*[in]*/ LONG pointy, /*[in]*/ ULONGLONG transformHDC );
	STDMETHOD( MouseLeftButtonUp )				( /*[in]*/ ULONG nFlags, /*[in]*/ LONG pointx, /*[in]*/ LONG pointy, /*[in]*/ ULONGLONG transformHDC );
	STDMETHOD( MouseLeftButtonDoubleClick )		( /*[in]*/ ULONG nFlags, /*[in]*/ LONG pointx, /*[in]*/ LONG pointy, /*[in]*/ ULONGLONG transformHDC );
	STDMETHOD( MouseRightButtonDown )			( /*[in]*/ ULONGLONG hCtxMenu, /*[in]*/ ULONG nFlags, /*[in]*/ LONG pointx, /*[in]*/ LONG pointy, /*[in]*/ ULONGLONG transformHDC );
	STDMETHOD( MouseRightButtonUp )				( /*[in]*/ ULONG nFlags, /*[in]*/ LONG pointx, /*[in]*/ LONG pointy, /*[in]*/ ULONGLONG transformHDC );
	STDMETHOD( MouseRightButtonDoubleClick )	( /*[in]*/ ULONG nFlags, /*[in]*/ LONG pointx, /*[in]*/ LONG pointy, /*[in]*/ ULONGLONG transformHDC );
	STDMETHOD( MouseMiddleButtonDown )			( /*[in]*/ ULONG nFlags, /*[in]*/ LONG pointx, /*[in]*/ LONG pointy, /*[in]*/ ULONGLONG transformHDC );
	STDMETHOD( MouseMiddleButtonUp )			( /*[in]*/ ULONG nFlags, /*[in]*/ LONG pointx, /*[in]*/ LONG pointy, /*[in]*/ ULONGLONG transformHDC );
	STDMETHOD( MouseMiddleButtonDoubleClick )	( /*[in]*/ ULONG nFlags, /*[in]*/ LONG pointx, /*[in]*/ LONG pointy, /*[in]*/ ULONGLONG transformHDC );
	STDMETHOD( MouseWheelTurned )				( /*[in]*/ ULONG nFlags, /*[in]*/ LONG distance, /*[in]*/ LONG pointx, /*[in]*/ LONG pointy, /*[in]*/ ULONGLONG transformHDC );
	STDMETHOD( DragEnter )						( /*[out]*/ ULONG* dropEffect, /*[in]*/ ULONGLONG pCOleDataObject, /*[in]*/ ULONG keyState, /*[in]*/ LONG pointx, /*[in]*/ LONG pointy, /*[in]*/ ULONGLONG transformHDC );
	STDMETHOD( DragOver )						( /*[out]*/ ULONG* dropEffect, /*[in]*/ ULONGLONG pCOleDataObject, /*[in]*/ ULONG keyState, /*[in]*/ LONG pointx, /*[in]*/ LONG pointy, /*[in]*/ ULONGLONG transformHDC );
	STDMETHOD( Drop )							( /*[in]*/ ULONGLONG pCOleDataObject, /*[in]*/ ULONG dropEffect, /*[in]*/ LONG pointx, /*[in]*/ LONG pointy, /*[in]*/ ULONGLONG transformHDC );
	STDMETHOD( DropFile )						( /*[in]*/ ULONGLONG hDropInfo, /*[in]*/ LONG pointx, /*[in]*/ LONG pointy, /*[in]*/ ULONGLONG transformHDC );
	STDMETHOD( MenuItemSelected )				( /*[in]*/ ULONG menuItemId, /*[in]*/ ULONG nFlags, /*[in]*/ LONG pointx, /*[in]*/ LONG pointy, /*[in]*/ ULONGLONG transformHDC );
	STDMETHOD( OperationCanceled )				( void );
};

#endif //__ANNOTATORDECORATORIMPL_H_
