//################################################################################################
//
// Decorator Event Sink implementation
//	DecoratorEventSink.cpp
//
//################################################################################################

#include "StdAfx.h"
#include "DecoratorEventSink.h"
#include "GMEView.h"


//################################################################################################
//
// CLASS : CDecoratorEventSink
//
//################################################################################################

void CDecoratorEventSink::SetView(CGMEView* view)
{
	m_view = view;
}

void CDecoratorEventSink::SetGuiObject(CGuiObject* guiObject)
{
	m_guiObject = guiObject;
}

CDecoratorEventSink::CDecoratorEventSink():
	m_view(NULL),
	m_guiObject(NULL),
	m_operationData(NULL)
{
}

CDecoratorEventSink::~CDecoratorEventSink()
{
}


///////////////////////////////////////////////////////////////////////////
// IMgaElementDecoratorEvents

STDMETHODIMP CDecoratorEventSink::Refresh(refresh_mode_enum refreshMode)
{
	if (refreshMode == RM_REDRAW_SELF)
		m_view->Invalidate(true);
	else if (refreshMode != RM_NOREFRESH)
		m_view->PostMessage(WM_USER_DECOR_VIEWREFRESH_REQ, 0, refreshMode);

	return S_OK;
}

STDMETHODIMP CDecoratorEventSink::OperationCanceled()
{
	m_view->CancelDecoratorOperation(false);

	return S_OK;
}

STDMETHODIMP CDecoratorEventSink::CursorChanged(LONG newCursorID)
{
	m_view->SetIsCursorChangedByDecorator(true);

	return S_OK;
}

STDMETHODIMP CDecoratorEventSink::CursorRestored()
{
	m_view->SetIsCursorChangedByDecorator(false);

	return S_OK;
}

STDMETHODIMP CDecoratorEventSink::LabelEditingStarted(LONG left, LONG top, LONG right, LONG bottom)
{
	m_view->SetInElementDecoratorOperation(true);
	m_view->SetDecoratorOrAnnotator(true);
	m_view->SetShouldCommitOperation(false);
	m_view->StartDecoratorOperation();

	return S_OK;
}

STDMETHODIMP CDecoratorEventSink::LabelEditingFinished(LONG left, LONG top, LONG right, LONG bottom)
{
	if (!m_view->IsInOpenedDecoratorTransaction()) {
		m_view->SetShouldCommitOperation(false);
		m_view->SetInElementDecoratorOperation(false);
		m_view->SetObjectInDecoratorOperation(NULL);
		m_view->SetAnnotatorInDecoratorOperation(NULL);
	}

	return S_OK;
}

STDMETHODIMP CDecoratorEventSink::LabelChanged(BSTR newLabel)
{
	m_view->BeginTransaction();
	m_view->SetInOpenedDecoratorTransaction(true);
	m_view->SetShouldCommitOperation(true);
	m_view->EndDecoratorOperation();

	return S_OK;
}

STDMETHODIMP CDecoratorEventSink::LabelMovingStarted(LONG nFlags, LONG left, LONG top, LONG right, LONG bottom)
{
	m_view->SetInElementDecoratorOperation(true);
	m_view->SetDecoratorOrAnnotator(true);
	m_view->SetShouldCommitOperation(false);
	m_view->SetOriginalRect(CRect(left, top, right, bottom));

	return S_OK;
}

STDMETHODIMP CDecoratorEventSink::LabelMoving(LONG nFlags, LONG left, LONG top, LONG right, LONG bottom)
{
//	m_guiObject->ResizeObject(CRect(left, top, right, bottom));
	m_view->Invalidate();
	m_view->SetShouldCommitOperation(false);

	return S_OK;
}

STDMETHODIMP CDecoratorEventSink::LabelMovingFinished(LONG nFlags, LONG left, LONG top, LONG right, LONG bottom)
{
//	m_guiObject->ResizeObject(CRect(left, top, right, bottom));
	m_view->SetInElementDecoratorOperation(false);
	m_view->SetOriginalRectEmpty();

	return S_OK;
}

STDMETHODIMP CDecoratorEventSink::LabelMoved(LONG nFlags, LONG x, LONG y)
{
	m_view->BeginTransaction();
	m_view->SetInOpenedDecoratorTransaction(true);
	m_view->SetShouldCommitOperation(true);

	return S_OK;
}

STDMETHODIMP CDecoratorEventSink::LabelResizingStarted(LONG nFlags, LONG left, LONG top, LONG right, LONG bottom)
{
	m_view->SetInElementDecoratorOperation(true);
	m_view->SetDecoratorOrAnnotator(true);
	m_view->SetShouldCommitOperation(false);
	m_view->SetOriginalRect(CRect(left, top, right, bottom));

	return S_OK;
}

STDMETHODIMP CDecoratorEventSink::LabelResizing(LONG nFlags, LONG left, LONG top, LONG right, LONG bottom)
{
//	m_guiObject->ResizeObject(CRect(left, top, right, bottom));
	m_view->Invalidate();
	m_view->SetShouldCommitOperation(false);

	return S_OK;
}

STDMETHODIMP CDecoratorEventSink::LabelResizingFinished(LONG nFlags, LONG left, LONG top, LONG right, LONG bottom)
{
//	m_guiObject->ResizeObject(CRect(left, top, right, bottom));
	m_view->SetInElementDecoratorOperation(false);
	m_view->SetOriginalRectEmpty();

	return S_OK;
}

STDMETHODIMP CDecoratorEventSink::LabelResized(LONG nFlags, LONG cx, LONG cy)
{
	m_view->BeginTransaction();
	m_view->SetInOpenedDecoratorTransaction(true);
	m_view->SetShouldCommitOperation(true);

	return S_OK;
}

STDMETHODIMP CDecoratorEventSink::GeneralOperationStarted(ULONGLONG operationData)
{
	m_operationData = (void*)operationData;

	m_view->BeginTransaction();
	m_view->SetInElementDecoratorOperation(true);
	m_view->SetDecoratorOrAnnotator(true);
	m_view->SetInOpenedDecoratorTransaction(true);
	m_view->SetShouldCommitOperation(false);
	m_view->StartDecoratorOperation();

	return S_OK;
}

STDMETHODIMP CDecoratorEventSink::GeneralOperationFinished(ULONGLONG* operationData)
{
	if (operationData != NULL)
		*operationData = (ULONGLONG)m_operationData;

	m_view->SetInElementDecoratorOperation(false);
	m_view->SetShouldCommitOperation(true);
	m_view->EndDecoratorOperation();

	return S_OK;
}

STDMETHODIMP CDecoratorEventSink::WindowMovingStarted(LONG nFlags, LONG left, LONG top, LONG right, LONG bottom)
{
	m_view->SetInElementDecoratorOperation(true);
	m_view->SetDecoratorOrAnnotator(true);
	m_view->SetShouldCommitOperation(false);
	m_view->SetOriginalRect(CRect(left, top, right, bottom));

	return S_OK;
}

STDMETHODIMP CDecoratorEventSink::WindowMoving(LONG nFlags, LONG left, LONG top, LONG right, LONG bottom)
{
//	m_guiObject->ResizeObject(CRect(left, top, right, bottom));
	m_view->Invalidate();
	m_view->SetShouldCommitOperation(false);

	return S_OK;
}

STDMETHODIMP CDecoratorEventSink::WindowMovingFinished(LONG nFlags, LONG left, LONG top, LONG right, LONG bottom)
{
//	m_guiObject->ResizeObject(CRect(left, top, right, bottom));
	m_view->SetInElementDecoratorOperation(false);
	m_view->SetOriginalRectEmpty();

	return S_OK;
}

STDMETHODIMP CDecoratorEventSink::WindowMoved(LONG nFlags, LONG x, LONG y)
{
	m_view->BeginTransaction();
	m_view->SetInOpenedDecoratorTransaction(true);
	m_view->SetShouldCommitOperation(true);

	return S_OK;
}

STDMETHODIMP CDecoratorEventSink::WindowResizingStarted(LONG nFlags, LONG left, LONG top, LONG right, LONG bottom)
{
	m_view->SetInElementDecoratorOperation(true);
	m_view->SetDecoratorOrAnnotator(true);
	m_view->SetShouldCommitOperation(false);
	m_view->SetOriginalRect(CRect(left, top, right, bottom));

	return S_OK;
}

STDMETHODIMP CDecoratorEventSink::WindowResizing(LONG nFlags, LONG left, LONG top, LONG right, LONG bottom)
{
	m_guiObject->ResizeObject(CRect(left, top, right, bottom));
	m_view->Invalidate();
	m_view->SetShouldCommitOperation(false);

	return S_OK;
}

STDMETHODIMP CDecoratorEventSink::WindowResizingFinished(LONG nFlags, LONG left, LONG top, LONG right, LONG bottom)
{
	m_guiObject->ResizeObject(CRect(left, top, right, bottom));
	m_view->SetInElementDecoratorOperation(false);
	m_view->SetOriginalRectEmpty();

	return S_OK;
}

STDMETHODIMP CDecoratorEventSink::WindowResized(LONG nFlags, LONG cx, LONG cy)
{
	m_view->BeginTransaction();
	m_view->SetInOpenedDecoratorTransaction(true);
	m_view->SetShouldCommitOperation(true);

	return S_OK;
}
