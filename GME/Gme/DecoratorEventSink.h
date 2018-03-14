//################################################################################################
//
// Decorator Event Sink implementation
//	DecoratorEventSink.h
//
//################################################################################################

#if !defined(AFX_DECORATOREVENTSINK_H__EB95C56D_04BC_47c9_A71B_305184F5FC44__INCLUDED_)
#define AFX_DECORATOREVENTSINK_H__EB95C56D_04BC_47c9_A71B_305184F5FC44__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdAfx.h"
#include "mga.h"
#include "GmeLib.h"
#include "resource.h"


class CGMEView;
class CGuiObject;

//################################################################################################
//
// CLASS : CDecoratorEventSink
//
//################################################################################################

class ATL_NO_VTABLE CDecoratorEventSink :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CDecoratorEventSink, &__uuidof(DecoratorEventSink)>,
	public ISupportErrorInfoImpl<&__uuidof(IMgaElementDecoratorEvents)>,
	public IMgaElementDecoratorEvents
{
protected:
	CDecoratorEventSink();			// protected constructor used by dynamic creation

public:
DECLARE_REGISTRY_RESOURCEID(IDR_DECORATOREVENTSINK)
DECLARE_NOT_AGGREGATABLE(CDecoratorEventSink)

BEGIN_COM_MAP(CDecoratorEventSink)
	COM_INTERFACE_ENTRY(IMgaElementDecoratorEvents)
	COM_INTERFACE_ENTRY(IMgaCommonDecoratorEvents)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()

// Attributes
public:

// Operations
public:
	void SetView(CGMEView* view);
	void SetGuiObject(CGuiObject* guiObject);

// Overrides
public:
// Implementation

public:
	~CDecoratorEventSink();

protected:
	CGMEView*					m_view;
	CGuiObject*					m_guiObject;
	void*						m_operationData;

public:
	STDMETHOD( Refresh )						( /*[in]*/ refresh_mode_enum refreshMode );
	STDMETHOD( OperationCanceled )				( void );
	STDMETHOD( CursorChanged )					( /*[in]*/ LONG newCursorID );
	STDMETHOD( CursorRestored )					( void );

	STDMETHOD( LabelEditingStarted )			( /*[in]*/ LONG left, /*[in]*/ LONG top, /*[in]*/ LONG right, /*[in]*/ LONG bottom );
	STDMETHOD( LabelEditingFinished )			( /*[in]*/ LONG left, /*[in]*/ LONG top, /*[in]*/ LONG right, /*[in]*/ LONG bottom );
	STDMETHOD( LabelChanged )					( /*[in]*/ BSTR newLabel );
	STDMETHOD( LabelMovingStarted )				( /*[in]*/ LONG nFlags, /*[in]*/ LONG left, /*[in]*/ LONG top, /*[in]*/ LONG right, /*[in]*/ LONG bottom );
	STDMETHOD( LabelMoving )					( /*[in]*/ LONG nFlags, /*[in]*/ LONG left, /*[in]*/ LONG top, /*[in]*/ LONG right, /*[in]*/ LONG bottom );
	STDMETHOD( LabelMovingFinished )			( /*[in]*/ LONG nFlags, /*[in]*/ LONG left, /*[in]*/ LONG top, /*[in]*/ LONG right, /*[in]*/ LONG bottom );
	STDMETHOD( LabelMoved )						( /*[in]*/ LONG nFlags, /*[in]*/ LONG x, /*[in]*/ LONG y );
	STDMETHOD( LabelResizingStarted )			( /*[in]*/ LONG nFlags, /*[in]*/ LONG left, /*[in]*/ LONG top, /*[in]*/ LONG right, /*[in]*/ LONG bottom );
	STDMETHOD( LabelResizing )					( /*[in]*/ LONG nFlags, /*[in]*/ LONG left, /*[in]*/ LONG top, /*[in]*/ LONG right, /*[in]*/ LONG bottom );
	STDMETHOD( LabelResizingFinished )			( /*[in]*/ LONG nFlags, /*[in]*/ LONG left, /*[in]*/ LONG top, /*[in]*/ LONG right, /*[in]*/ LONG bottom );
	STDMETHOD( LabelResized )					( /*[in]*/ LONG nFlags, /*[in]*/ LONG cx, /*[in]*/ LONG cy );

	STDMETHOD( GeneralOperationStarted )		( /*[in]*/ ULONGLONG operationData );
	STDMETHOD( GeneralOperationFinished )		( /*[out]*/ ULONGLONG* operationData );

	STDMETHOD( WindowMovingStarted )			( /*[in]*/ LONG nFlags, /*[in]*/ LONG left, /*[in]*/ LONG top, /*[in]*/ LONG right, /*[in]*/ LONG bottom );
	STDMETHOD( WindowMoving )					( /*[in]*/ LONG nFlags, /*[in]*/ LONG left, /*[in]*/ LONG top, /*[in]*/ LONG right, /*[in]*/ LONG bottom );
	STDMETHOD( WindowMovingFinished )			( /*[in]*/ LONG nFlags, /*[in]*/ LONG left, /*[in]*/ LONG top, /*[in]*/ LONG right, /*[in]*/ LONG bottom );
	STDMETHOD( WindowMoved )					( /*[in]*/ LONG nFlags, /*[in]*/ LONG x, /*[in]*/ LONG y );

	STDMETHOD( WindowResizingStarted )			( /*[in]*/ LONG nFlags, /*[in]*/ LONG left, /*[in]*/ LONG top, /*[in]*/ LONG right, /*[in]*/ LONG bottom );
	STDMETHOD( WindowResizing )					( /*[in]*/ LONG nFlags, /*[in]*/ LONG left, /*[in]*/ LONG top, /*[in]*/ LONG right, /*[in]*/ LONG bottom );
	STDMETHOD( WindowResizingFinished )			( /*[in]*/ LONG nFlags, /*[in]*/ LONG left, /*[in]*/ LONG top, /*[in]*/ LONG right, /*[in]*/ LONG bottom );
	STDMETHOD( WindowResized )					( /*[in]*/ LONG nFlags, /*[in]*/ LONG cx, /*[in]*/ LONG cy );
};

OBJECT_ENTRY_AUTO(__uuidof(DecoratorEventSink), CDecoratorEventSink) 

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DECORATOREVENTSINK_H__EB95C56D_04BC_47c9_A71B_305184F5FC44__INCLUDED_)
