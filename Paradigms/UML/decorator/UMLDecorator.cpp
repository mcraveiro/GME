//################################################################################################
//
// UML decorator class
//	UMLDecorator.cpp
//
//################################################################################################

#include "StdAfx.h"
#include "UMLDecorator.h"
#include "UMLCompositePart.h"


namespace UMLDecor {

//################################################################################################
//
// CLASS : UMLDecorator
//
//################################################################################################

UMLDecorator::UMLDecorator(CComPtr<IMgaCommonDecoratorEvents> eventSink):
	m_part(NULL),
	m_eventSink(eventSink)
{
}

UMLDecorator::~UMLDecorator()
{
}

void UMLDecorator::Initialize(CComPtr<IMgaProject>& pProject, CComPtr<IMgaMetaPart>& pPart, CComPtr<IMgaFCO>& pFCO)
{
	m_part->Initialize(pProject, pPart, pFCO);
}

void UMLDecorator::Destroy()
{
	if (m_part) {
		m_part->Destroy();
		delete m_part;
		m_part = NULL;
	}
}

CString UMLDecorator::GetMnemonic(void) const
{
	return m_part->GetMnemonic();
}

feature_code UMLDecorator::GetFeatures(void) const
{
	return m_part->GetFeatures();
}

void UMLDecorator::SetParam(const CString& strName, VARIANT vValue)
{
	m_part->SetParam(strName, vValue);
}

void UMLDecorator::GetParam(const CString& strName, VARIANT* pvValue)
{
	m_part->GetParam(strName, pvValue);
}

void UMLDecorator::SetActive(bool bIsActive)
{
	m_part->SetActive(bIsActive);
}

CSize UMLDecorator::GetPreferredSize(void) const
{
	return m_part->GetPreferredSize();
}


void UMLDecorator::SetLocation(const CRect& location)
{
	m_part->SetLocation(location);
}

CRect UMLDecorator::GetLocation(void) const
{
	return m_part->GetLocation();
}

CRect UMLDecorator::GetLabelLocation(void) const
{
	return m_part->GetLabelLocation();
}

CRect UMLDecorator::GetPortLocation(CComPtr<IMgaFCO>& fco) const
{
	return m_part->GetPortLocation(fco);
}

void UMLDecorator::GetPorts(CComPtr<IMgaFCOs>& portFCOs) const
{
	m_part->GetPorts(portFCOs);
}

void UMLDecorator::Draw(CDC* pDC, Gdiplus::Graphics* gdip)
{
	m_part->Draw(pDC, gdip);
}

void UMLDecorator::SaveState()
{
	m_part->SaveState();
}

// New functions
void UMLDecorator::InitializeEx(CComPtr<IMgaProject>& pProject, CComPtr<IMgaMetaPart>& pPart, CComPtr<IMgaFCO>& pFCO,
								HWND parentWnd)
{
	UMLCompositePart* umlComposite = new UMLCompositePart(NULL, m_eventSink);
	m_part = umlComposite;

	umlComposite->InitializeEx(pProject, pPart, pFCO, parentWnd);
}

void UMLDecorator::SetSelected(bool bIsSelected)
{
	m_part->SetSelected(bIsSelected);
}

bool UMLDecorator::MouseMoved(UINT nFlags, const CPoint& point, HDC transformHDC)
{
	return m_part->MouseMoved(nFlags, point, transformHDC);
}

bool UMLDecorator::MouseLeftButtonDown(UINT nFlags, const CPoint& point, HDC transformHDC)
{
	return m_part->MouseLeftButtonDown(nFlags, point, transformHDC);
}

bool UMLDecorator::MouseLeftButtonUp(UINT nFlags, const CPoint& point, HDC transformHDC)
{
	return m_part->MouseLeftButtonUp(nFlags, point, transformHDC);
}

bool UMLDecorator::MouseLeftButtonDoubleClick(UINT nFlags, const CPoint& point, HDC transformHDC)
{
	return m_part->MouseLeftButtonDoubleClick(nFlags, point, transformHDC);
}

bool UMLDecorator::MouseRightButtonDown(HMENU hCtxMenu, UINT nFlags, const CPoint& point, HDC transformHDC)
{
	return m_part->MouseRightButtonDown(hCtxMenu, nFlags, point, transformHDC);
}

bool UMLDecorator::MouseRightButtonUp(UINT nFlags, const CPoint& point, HDC transformHDC)
{
	return m_part->MouseRightButtonUp(nFlags, point, transformHDC);
}

bool UMLDecorator::MouseRightButtonDoubleClick(UINT nFlags, const CPoint& point, HDC transformHDC)
{
	return m_part->MouseRightButtonDoubleClick(nFlags, point, transformHDC);
}

bool UMLDecorator::MouseMiddleButtonDown(UINT nFlags, const CPoint& point, HDC transformHDC)
{
	return m_part->MouseMiddleButtonDown(nFlags, point, transformHDC);
}

bool UMLDecorator::MouseMiddleButtonUp(UINT nFlags, const CPoint& point, HDC transformHDC)
{
	return m_part->MouseMiddleButtonUp(nFlags, point, transformHDC);
}

bool UMLDecorator::MouseMiddleButtonDoubleClick(UINT nFlags, const CPoint& point, HDC transformHDC)
{
	return m_part->MouseMiddleButtonDoubleClick(nFlags, point, transformHDC);
}

bool UMLDecorator::MouseWheelTurned(UINT nFlags, short distance, const CPoint& point, HDC transformHDC)
{
	return m_part->MouseWheelTurned(nFlags, distance, point, transformHDC);
}

bool UMLDecorator::DragEnter(DROPEFFECT* dropEffect, COleDataObject* pDataObject, DWORD dwKeyState, const CPoint& point, HDC transformHDC)
{
	return m_part->DragEnter(dropEffect, pDataObject, dwKeyState, point, transformHDC);
}

bool UMLDecorator::DragOver(DROPEFFECT* dropEffect, COleDataObject* pDataObject, DWORD dwKeyState, const CPoint& point, HDC transformHDC)
{
	return m_part->DragOver(dropEffect, pDataObject, dwKeyState, point, transformHDC);
}

bool UMLDecorator::Drop(COleDataObject* pDataObject, DROPEFFECT dropEffect, const CPoint& point, HDC transformHDC)
{
	return m_part->Drop(pDataObject, dropEffect, point, transformHDC);
}

bool UMLDecorator::DropFile(HDROP p_hDropInfo, const CPoint& point, HDC transformHDC)
{
	return m_part->DropFile(p_hDropInfo, point, transformHDC);
}

bool UMLDecorator::MenuItemSelected(UINT menuItemId, UINT nFlags, const CPoint& point, HDC transformHDC)
{
	return m_part->MenuItemSelected(menuItemId, nFlags, point, transformHDC);
}

bool UMLDecorator::OperationCanceledByGME(void)
{
	return m_part->OperationCanceledByGME();
}

}; // namespace UMLDecor
