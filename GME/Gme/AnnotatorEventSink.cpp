//################################################################################################
//
// Annotator Event Sink implementation
//	AnnotatorEventSink.cpp
//
//################################################################################################

#include "StdAfx.h"
#include "AnnotatorEventSink.h"
#include "GMEView.h"


//################################################################################################
//
// CLASS : CAnnotatorEventSink
//
//################################################################################################

void CAnnotatorEventSink::SetView(CGMEView* view)
{
	m_view = view;
}

void CAnnotatorEventSink::SetGuiAnnotator(CGuiAnnotator* guiAnnotator)
{
	m_guiAnnotator = guiAnnotator;
}

CAnnotatorEventSink::CAnnotatorEventSink():
	m_view(NULL),
	m_guiAnnotator(NULL)
{
}

CAnnotatorEventSink::~CAnnotatorEventSink()
{
}


///////////////////////////////////////////////////////////////////////////
// IMgaElementDecoratorEvents

STDMETHODIMP CAnnotatorEventSink::Refresh(refresh_mode_enum refreshMode)
{
	if (refreshMode == RM_REDRAW_SELF)
		m_view->Invalidate(true);
	else if (refreshMode != RM_NOREFRESH)
		m_view->PostMessage(WM_USER_DECOR_VIEWREFRESH_REQ, 0, refreshMode);

	return S_OK;
}

STDMETHODIMP CAnnotatorEventSink::OperationCanceled()
{
	m_view->CancelDecoratorOperation(false);

	return S_OK;
}

STDMETHODIMP CAnnotatorEventSink::CursorChanged(LONG newCursorID)
{
	m_view->SetIsCursorChangedByDecorator(true);

	return S_OK;
}

STDMETHODIMP CAnnotatorEventSink::CursorRestored()
{
	m_view->SetIsCursorChangedByDecorator(false);

	return S_OK;
}

STDMETHODIMP CAnnotatorEventSink::LabelEditingStarted(LONG left, LONG top, LONG right, LONG bottom)
{
	m_view->SetInElementDecoratorOperation(true);
	m_view->SetDecoratorOrAnnotator(false);
	m_view->SetShouldCommitOperation(false);
	m_view->StartDecoratorOperation();

	return S_OK;
}

STDMETHODIMP CAnnotatorEventSink::LabelEditingFinished(LONG left, LONG top, LONG right, LONG bottom)
{
	if (!m_view->IsInOpenedDecoratorTransaction()) {
		m_view->SetShouldCommitOperation(false);
		m_view->SetInElementDecoratorOperation(false);
		m_view->SetObjectInDecoratorOperation(NULL);
		m_view->SetAnnotatorInDecoratorOperation(NULL);
	}

	return S_OK;
}

STDMETHODIMP CAnnotatorEventSink::LabelChanged(BSTR newLabel)
{
	m_view->BeginTransaction();
	m_view->SetInOpenedDecoratorTransaction(true);
	m_view->SetShouldCommitOperation(true);
	m_view->EndDecoratorOperation();

	return S_OK;
}

STDMETHODIMP CAnnotatorEventSink::LabelMovingStarted(LONG nFlags, LONG left, LONG top, LONG right, LONG bottom)
{
	return S_OK;
}

STDMETHODIMP CAnnotatorEventSink::LabelMoving(LONG nFlags, LONG left, LONG top, LONG right, LONG bottom)
{
	return S_OK;
}

STDMETHODIMP CAnnotatorEventSink::LabelMovingFinished(LONG nFlags, LONG left, LONG top, LONG right, LONG bottom)
{
	return S_OK;
}

STDMETHODIMP CAnnotatorEventSink::LabelMoved(LONG nFlags, LONG x, LONG y)
{
	return S_OK;
}

STDMETHODIMP CAnnotatorEventSink::LabelResizingStarted(LONG nFlags, LONG left, LONG top, LONG right, LONG bottom)
{
	return S_OK;
}

STDMETHODIMP CAnnotatorEventSink::LabelResizing(LONG nFlags, LONG left, LONG top, LONG right, LONG bottom)
{
	return S_OK;
}

STDMETHODIMP CAnnotatorEventSink::LabelResizingFinished(LONG nFlags, LONG left, LONG top, LONG right, LONG bottom)
{
	return S_OK;
}

STDMETHODIMP CAnnotatorEventSink::LabelResized(LONG nFlags, LONG cx, LONG cy)
{
	return S_OK;
}

STDMETHODIMP CAnnotatorEventSink::GeneralOperationStarted(ULONGLONG operationData)
{
	m_operationData = (void*)operationData;

	m_view->BeginTransaction();
	m_view->SetInElementDecoratorOperation(true);
	m_view->SetDecoratorOrAnnotator(false);
	m_view->SetInOpenedDecoratorTransaction(true);
	m_view->SetShouldCommitOperation(false);
	m_view->StartDecoratorOperation();

	return S_OK;
}

STDMETHODIMP CAnnotatorEventSink::GeneralOperationFinished(ULONGLONG* operationData)
{
	if (operationData != NULL)
		*operationData = (ULONGLONG)m_operationData;

	m_view->SetInElementDecoratorOperation(false);
	m_view->SetShouldCommitOperation(true);
	m_view->EndDecoratorOperation();

	return S_OK;
}

STDMETHODIMP CAnnotatorEventSink::WindowMovingStarted(LONG nFlags, LONG left, LONG top, LONG right, LONG bottom)
{
	return S_OK;
}

STDMETHODIMP CAnnotatorEventSink::WindowMoving(LONG nFlags, LONG left, LONG top, LONG right, LONG bottom)
{
	return S_OK;
}

STDMETHODIMP CAnnotatorEventSink::WindowMovingFinished(LONG nFlags, LONG left, LONG top, LONG right, LONG bottom)
{
	return S_OK;
}

STDMETHODIMP CAnnotatorEventSink::WindowMoved(LONG nFlags, LONG x, LONG y)
{
	return S_OK;
}

STDMETHODIMP CAnnotatorEventSink::WindowResizingStarted(LONG nFlags, LONG left, LONG top, LONG right, LONG bottom)
{
	m_view->SetInElementDecoratorOperation(true);
	m_view->SetDecoratorOrAnnotator(false);
	m_view->SetShouldCommitOperation(false);
	m_view->SetOriginalRect(CRect(left, top, right, bottom));

	return S_OK;
}

STDMETHODIMP CAnnotatorEventSink::WindowResizing(LONG nFlags, LONG left, LONG top, LONG right, LONG bottom)
{
//	TODO m_guiAnnotator->ResizeObject(CRect(left, top, right, bottom));
	m_view->Invalidate();
	m_view->SetShouldCommitOperation(false);

	return S_OK;
}

STDMETHODIMP CAnnotatorEventSink::WindowResizingFinished(LONG nFlags, LONG left, LONG top, LONG right, LONG bottom)
{
//	TODO m_guiAnnotator->ResizeObject(CRect(left, top, right, bottom));
	m_view->SetInElementDecoratorOperation(false);
	m_view->SetOriginalRectEmpty();

	return S_OK;
}

STDMETHODIMP CAnnotatorEventSink::WindowResized(LONG nFlags, LONG cx, LONG cy)
{
	m_view->BeginTransaction();
	m_view->SetInOpenedDecoratorTransaction(true);
	m_view->SetShouldCommitOperation(true);

	return S_OK;
}
