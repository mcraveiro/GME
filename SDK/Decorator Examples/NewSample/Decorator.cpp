//################################################################################################
//
// Decorator class
//	Decorator.cpp
// This class represents the C++ side of the decorator COM connection layer
// This is still just a relay class like teh COM/ATL DecoratorImpl.h,cpp,
// the meat should go into the composite part or into the parts in the composite part
//
//################################################################################################

#include "StdAfx.h"
#include "Decorator.h"
#include "DecoratorCompositePart.h"


namespace Decor {

//################################################################################################
//
// CLASS : Decorator
//
//################################################################################################

Decorator::Decorator(CComPtr<IMgaCommonDecoratorEvents>& eventSink):
	m_part(NULL),
	m_eventSink(eventSink)
{
}

Decorator::~Decorator()
{
}

void Decorator::Initialize(CComPtr<IMgaProject>& pProject, CComPtr<IMgaMetaPart>& pPart, CComPtr<IMgaFCO>& pFCO)
{
	m_part->Initialize(pProject, pPart, pFCO);
}

void Decorator::Destroy()
{
	if (m_part) {
		m_part->Destroy();
		delete m_part;
		m_part = NULL;
	}
}

CString Decorator::GetMnemonic(void) const
{
	return m_part->GetMnemonic();
}

feature_code Decorator::GetFeatures(void) const
{
	return m_part->GetFeatures();
}

void Decorator::SetParam(const CString& strName, VARIANT vValue)
{
	m_part->SetParam(strName, vValue);
}

void Decorator::GetParam(const CString& strName, VARIANT* pvValue)
{
	m_part->GetParam(strName, pvValue);
}

void Decorator::SetActive(bool bIsActive)
{
	m_part->SetActive(bIsActive);
}

CSize Decorator::GetPreferredSize(void) const
{
	return m_part->GetPreferredSize();
}


void Decorator::SetLocation(const CRect& location)
{
	m_part->SetLocation(location);
}

CRect Decorator::GetLocation(void) const
{
	return m_part->GetLocation();
}

CRect Decorator::GetLabelLocation(void) const
{
	return m_part->GetLabelLocation();
}

CRect Decorator::GetPortLocation(CComPtr<IMgaFCO>& fco) const
{
	return m_part->GetPortLocation(fco);
}

void Decorator::GetPorts(CComPtr<IMgaFCOs>& portFCOs) const
{
	m_part->GetPorts(portFCOs);
}

void Decorator::Draw(CDC* pDC, Gdiplus::Graphics* gdip)
{
	m_part->Draw(pDC, gdip);
}

void Decorator::SaveState()
{
	m_part->SaveState();
}

// New functions
void Decorator::InitializeEx(CComPtr<IMgaProject>& pProject, CComPtr<IMgaMetaPart>& pPart, CComPtr<IMgaFCO>& pFCO,
							 HWND parentWnd)
{
	DecoratorCompositePart* decoratorComposite = new DecoratorCompositePart(NULL, m_eventSink);
	m_part = decoratorComposite;

	decoratorComposite->InitializeEx(pProject, pPart, pFCO, parentWnd);
}

void Decorator::SetSelected(bool bIsSelected)
{
	m_part->SetSelected(bIsSelected);
}

bool Decorator::MouseMoved(UINT nFlags, const CPoint& point, HDC transformHDC)
{
	return m_part->MouseMoved(nFlags, point, transformHDC);
}

bool Decorator::MouseLeftButtonDown(UINT nFlags, const CPoint& point, HDC transformHDC)
{
	return m_part->MouseLeftButtonDown(nFlags, point, transformHDC);
}

bool Decorator::MouseLeftButtonUp(UINT nFlags, const CPoint& point, HDC transformHDC)
{
	return m_part->MouseLeftButtonUp(nFlags, point, transformHDC);
}

bool Decorator::MouseLeftButtonDoubleClick(UINT nFlags, const CPoint& point, HDC transformHDC)
{
	return m_part->MouseLeftButtonDoubleClick(nFlags, point, transformHDC);
}

bool Decorator::MouseRightButtonDown(HMENU hCtxMenu, UINT nFlags, const CPoint& point, HDC transformHDC)
{
	return m_part->MouseRightButtonDown(hCtxMenu, nFlags, point, transformHDC);
}

bool Decorator::MouseRightButtonUp(UINT nFlags, const CPoint& point, HDC transformHDC)
{
	return m_part->MouseRightButtonUp(nFlags, point, transformHDC);
}

bool Decorator::MouseRightButtonDoubleClick(UINT nFlags, const CPoint& point, HDC transformHDC)
{
	return m_part->MouseRightButtonDoubleClick(nFlags, point, transformHDC);
}

bool Decorator::MouseMiddleButtonDown(UINT nFlags, const CPoint& point, HDC transformHDC)
{
	return m_part->MouseMiddleButtonDown(nFlags, point, transformHDC);
}

bool Decorator::MouseMiddleButtonUp(UINT nFlags, const CPoint& point, HDC transformHDC)
{
	return m_part->MouseMiddleButtonUp(nFlags, point, transformHDC);
}

bool Decorator::MouseMiddleButtonDoubleClick(UINT nFlags, const CPoint& point, HDC transformHDC)
{
	return m_part->MouseMiddleButtonDoubleClick(nFlags, point, transformHDC);
}

bool Decorator::MouseWheelTurned(UINT nFlags, short distance, const CPoint& point, HDC transformHDC)
{
	return m_part->MouseWheelTurned(nFlags, distance, point, transformHDC);
}

bool Decorator::DragEnter(DROPEFFECT* dropEffect, COleDataObject* pDataObject, DWORD dwKeyState, const CPoint& point, HDC transformHDC)
{
	return m_part->DragEnter(dropEffect, pDataObject, dwKeyState, point, transformHDC);
}

bool Decorator::DragOver(DROPEFFECT* dropEffect, COleDataObject* pDataObject, DWORD dwKeyState, const CPoint& point, HDC transformHDC)
{
	return m_part->DragOver(dropEffect, pDataObject, dwKeyState, point, transformHDC);
}

bool Decorator::Drop(COleDataObject* pDataObject, DROPEFFECT dropEffect, const CPoint& point, HDC transformHDC)
{
	return m_part->Drop(pDataObject, dropEffect, point, transformHDC);
}

bool Decorator::DropFile(HDROP p_hDropInfo, const CPoint& point, HDC transformHDC)
{
	return m_part->DropFile(p_hDropInfo, point, transformHDC);
}

bool Decorator::MenuItemSelected(UINT menuItemId, UINT nFlags, const CPoint& point, HDC transformHDC)
{
	return m_part->MenuItemSelected(menuItemId, nFlags, point, transformHDC);
}

bool Decorator::OperationCanceledByGME(void)
{
	return m_part->OperationCanceledByGME();
}

}; // namespace Decor
