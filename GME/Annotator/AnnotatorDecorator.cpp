//################################################################################################
//
// Annotator decorator class
//	AnnotatorDecorator.cpp
//
//################################################################################################

#include "StdAfx.h"
#include "AnnotatorDecorator.h"
#include "AnnotatorCompositePart.h"


namespace AnnotatorDecor {

//################################################################################################
//
// CLASS : AnnotatorDecorator
//
//################################################################################################

AnnotatorDecorator::AnnotatorDecorator(CComPtr<IMgaCommonDecoratorEvents>& eventSink):
	m_part(NULL),
	m_eventSink(eventSink)
{
}

AnnotatorDecorator::~AnnotatorDecorator()
{
}

void AnnotatorDecorator::Initialize(CComPtr<IMgaProject>& pProject, CComPtr<IMgaMetaPart>& pPart, CComPtr<IMgaFCO>& pFCO)
{
	m_part->Initialize(pProject, pPart, pFCO);
}

void AnnotatorDecorator::Destroy()
{
	if (m_part) {
		m_part->Destroy();
		delete m_part;
		m_part = NULL;
	}
}

CString AnnotatorDecorator::GetMnemonic(void) const
{
	return m_part->GetMnemonic();
}

feature_code AnnotatorDecorator::GetFeatures(void) const
{
	return m_part->GetFeatures();
}

void AnnotatorDecorator::SetParam(const CString& strName, VARIANT vValue)
{
	m_strName = strName;
	m_vValue = vValue;
}

void AnnotatorDecorator::GetParam(const CString& strName, VARIANT* pvValue)
{
	m_part->GetParam(strName, pvValue);
}

void AnnotatorDecorator::SetActive(bool bIsActive)
{
	m_part->SetActive(bIsActive);
}

CSize AnnotatorDecorator::GetPreferredSize(void) const
{
	return m_part->GetPreferredSize();
}


void AnnotatorDecorator::SetLocation(const CRect& location)
{
	m_part->SetLocation(location);
}

CRect AnnotatorDecorator::GetLocation(void) const
{
	return m_part->GetLocation();
}

CRect AnnotatorDecorator::GetLabelLocation(void) const
{
	return m_part->GetLabelLocation();
}

CRect AnnotatorDecorator::GetPortLocation(CComPtr<IMgaFCO>& fco) const
{
	return m_part->GetPortLocation(fco);
}

void AnnotatorDecorator::GetPorts(CComPtr<IMgaFCOs>& portFCOs) const
{
	m_part->GetPorts(portFCOs);
}

void AnnotatorDecorator::Draw(CDC* pDC, Gdiplus::Graphics* gdip)
{
	m_part->Draw(pDC, gdip);
}

void AnnotatorDecorator::SaveState()
{
	m_part->SaveState();
}

// New functions
void AnnotatorDecorator::InitializeEx(CComPtr<IMgaProject>& pProject, CComPtr<IMgaMetaPart>& pPart, CComPtr<IMgaFCO>& pFCO,
									  HWND parentWnd)
{
	AnnotatorCompositePart* annotatorComposite = new AnnotatorCompositePart(NULL, m_eventSink);
	m_part = annotatorComposite;
	m_part->SetParam(m_strName, m_vValue);

	annotatorComposite->InitializeEx(pProject, pPart, pFCO, parentWnd);
}

void AnnotatorDecorator::SetSelected(bool bIsSelected)
{
	m_part->SetSelected(bIsSelected);
}

bool AnnotatorDecorator::MouseMoved(UINT nFlags, const CPoint& point, HDC transformHDC)
{
	return m_part->MouseMoved(nFlags, point, transformHDC);
}

bool AnnotatorDecorator::MouseLeftButtonDown(UINT nFlags, const CPoint& point, HDC transformHDC)
{
	return m_part->MouseLeftButtonDown(nFlags, point, transformHDC);
}

bool AnnotatorDecorator::MouseLeftButtonUp(UINT nFlags, const CPoint& point, HDC transformHDC)
{
	return m_part->MouseLeftButtonUp(nFlags, point, transformHDC);
}

bool AnnotatorDecorator::MouseLeftButtonDoubleClick(UINT nFlags, const CPoint& point, HDC transformHDC)
{
	return m_part->MouseLeftButtonDoubleClick(nFlags, point, transformHDC);
}

bool AnnotatorDecorator::MouseRightButtonDown(HMENU hCtxMenu, UINT nFlags, const CPoint& point, HDC transformHDC)
{
	return m_part->MouseRightButtonDown(hCtxMenu, nFlags, point, transformHDC);
}

bool AnnotatorDecorator::MouseRightButtonUp(UINT nFlags, const CPoint& point, HDC transformHDC)
{
	return m_part->MouseRightButtonUp(nFlags, point, transformHDC);
}

bool AnnotatorDecorator::MouseRightButtonDoubleClick(UINT nFlags, const CPoint& point, HDC transformHDC)
{
	return m_part->MouseRightButtonDoubleClick(nFlags, point, transformHDC);
}

bool AnnotatorDecorator::MouseMiddleButtonDown(UINT nFlags, const CPoint& point, HDC transformHDC)
{
	return m_part->MouseMiddleButtonDown(nFlags, point, transformHDC);
}

bool AnnotatorDecorator::MouseMiddleButtonUp(UINT nFlags, const CPoint& point, HDC transformHDC)
{
	return m_part->MouseMiddleButtonUp(nFlags, point, transformHDC);
}

bool AnnotatorDecorator::MouseMiddleButtonDoubleClick(UINT nFlags, const CPoint& point, HDC transformHDC)
{
	return m_part->MouseMiddleButtonDoubleClick(nFlags, point, transformHDC);
}

bool AnnotatorDecorator::MouseWheelTurned(UINT nFlags, short distance, const CPoint& point, HDC transformHDC)
{
	return m_part->MouseWheelTurned(nFlags, distance, point, transformHDC);
}

bool AnnotatorDecorator::DragEnter(DROPEFFECT* dropEffect, COleDataObject* pDataObject, DWORD dwKeyState, const CPoint& point, HDC transformHDC)
{
	return m_part->DragEnter(dropEffect, pDataObject, dwKeyState, point, transformHDC);
}

bool AnnotatorDecorator::DragOver(DROPEFFECT* dropEffect, COleDataObject* pDataObject, DWORD dwKeyState, const CPoint& point, HDC transformHDC)
{
	return m_part->DragOver(dropEffect, pDataObject, dwKeyState, point, transformHDC);
}

bool AnnotatorDecorator::Drop(COleDataObject* pDataObject, DROPEFFECT dropEffect, const CPoint& point, HDC transformHDC)
{
	return m_part->Drop(pDataObject, dropEffect, point, transformHDC);
}

bool AnnotatorDecorator::DropFile(HDROP p_hDropInfo, const CPoint& point, HDC transformHDC)
{
	return m_part->DropFile(p_hDropInfo, point, transformHDC);
}

bool AnnotatorDecorator::MenuItemSelected(UINT menuItemId, UINT nFlags, const CPoint& point, HDC transformHDC)
{
	return m_part->MenuItemSelected(menuItemId, nFlags, point, transformHDC);
}

bool AnnotatorDecorator::OperationCanceledByGME(void)
{
	return m_part->OperationCanceledByGME();
}

}; // namespace AnnotatorDecor
