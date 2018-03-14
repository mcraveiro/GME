//################################################################################################
//
// Meta decorator class
//	MetaDecorator.cpp
//
//################################################################################################

#include "StdAfx.h"
#include "MetaDecorator.h"
#include "MetaCompositePart.h"


namespace MetaDecor {

//################################################################################################
//
// CLASS : MetaDecorator
//
//################################################################################################

MetaDecorator::MetaDecorator(CComPtr<IMgaCommonDecoratorEvents>& eventSink):
	m_part(NULL),
	m_eventSink(eventSink)
{
}

MetaDecorator::~MetaDecorator()
{
}

void MetaDecorator::Initialize(CComPtr<IMgaProject>& pProject, CComPtr<IMgaMetaPart>& pPart, CComPtr<IMgaFCO>& pFCO)
{
	m_part->Initialize(pProject, pPart, pFCO);
}

void MetaDecorator::Destroy()
{
	if (m_part) {
		m_part->Destroy();
		delete m_part;
		m_part = NULL;
	}
}

CString MetaDecorator::GetMnemonic(void) const
{
	return m_part->GetMnemonic();
}

feature_code MetaDecorator::GetFeatures(void) const
{
	return m_part->GetFeatures();
}

void MetaDecorator::SetParam(const CString& strName, VARIANT vValue)
{
	m_part->SetParam(strName, vValue);
}

void MetaDecorator::GetParam(const CString& strName, VARIANT* pvValue)
{
	m_part->GetParam(strName, pvValue);
}

void MetaDecorator::SetActive(bool bIsActive)
{
	m_part->SetActive(bIsActive);
}

CSize MetaDecorator::GetPreferredSize(void) const
{
	return m_part->GetPreferredSize();
}


void MetaDecorator::SetLocation(const CRect& location)
{
	m_part->SetLocation(location);
}

CRect MetaDecorator::GetLocation(void) const
{
	return m_part->GetLocation();
}

CRect MetaDecorator::GetLabelLocation(void) const
{
	return m_part->GetLabelLocation();
}

CRect MetaDecorator::GetPortLocation(CComPtr<IMgaFCO>& fco) const
{
	return m_part->GetPortLocation(fco);
}

void MetaDecorator::GetPorts(CComPtr<IMgaFCOs>& portFCOs) const
{
	m_part->GetPorts(portFCOs);
}

void MetaDecorator::Draw(CDC* pDC, Gdiplus::Graphics* gdip)
{
	m_part->Draw(pDC, gdip);
}

void MetaDecorator::SaveState()
{
	m_part->SaveState();
}

// New functions
void MetaDecorator::InitializeEx(CComPtr<IMgaProject>& pProject, CComPtr<IMgaMetaPart>& pPart, CComPtr<IMgaFCO>& pFCO,
								 HWND parentWnd)
{
	MetaCompositePart* metaComposite = new MetaCompositePart(NULL, m_eventSink);
	m_part = metaComposite;

	metaComposite->InitializeEx(pProject, pPart, pFCO, parentWnd);
}

void MetaDecorator::SetSelected(bool bIsSelected)
{
	m_part->SetSelected(bIsSelected);
}

bool MetaDecorator::MouseMoved(UINT nFlags, const CPoint& point, HDC transformHDC)
{
	return m_part->MouseMoved(nFlags, point, transformHDC);
}

bool MetaDecorator::MouseLeftButtonDown(UINT nFlags, const CPoint& point, HDC transformHDC)
{
	return m_part->MouseLeftButtonDown(nFlags, point, transformHDC);
}

bool MetaDecorator::MouseLeftButtonUp(UINT nFlags, const CPoint& point, HDC transformHDC)
{
	return m_part->MouseLeftButtonUp(nFlags, point, transformHDC);
}

bool MetaDecorator::MouseLeftButtonDoubleClick(UINT nFlags, const CPoint& point, HDC transformHDC)
{
	return m_part->MouseLeftButtonDoubleClick(nFlags, point, transformHDC);
}

bool MetaDecorator::MouseRightButtonDown(HMENU hCtxMenu, UINT nFlags, const CPoint& point, HDC transformHDC)
{
	return m_part->MouseRightButtonDown(hCtxMenu, nFlags, point, transformHDC);
}

bool MetaDecorator::MouseRightButtonUp(UINT nFlags, const CPoint& point, HDC transformHDC)
{
	return m_part->MouseRightButtonUp(nFlags, point, transformHDC);
}

bool MetaDecorator::MouseRightButtonDoubleClick(UINT nFlags, const CPoint& point, HDC transformHDC)
{
	return m_part->MouseRightButtonDoubleClick(nFlags, point, transformHDC);
}

bool MetaDecorator::MouseMiddleButtonDown(UINT nFlags, const CPoint& point, HDC transformHDC)
{
	return m_part->MouseMiddleButtonDown(nFlags, point, transformHDC);
}

bool MetaDecorator::MouseMiddleButtonUp(UINT nFlags, const CPoint& point, HDC transformHDC)
{
	return m_part->MouseMiddleButtonUp(nFlags, point, transformHDC);
}

bool MetaDecorator::MouseMiddleButtonDoubleClick(UINT nFlags, const CPoint& point, HDC transformHDC)
{
	return m_part->MouseMiddleButtonDoubleClick(nFlags, point, transformHDC);
}

bool MetaDecorator::MouseWheelTurned(UINT nFlags, short distance, const CPoint& point, HDC transformHDC)
{
	return m_part->MouseWheelTurned(nFlags, distance, point, transformHDC);
}

bool MetaDecorator::DragEnter(DROPEFFECT* dropEffect, COleDataObject* pDataObject, DWORD dwKeyState, const CPoint& point, HDC transformHDC)
{
	return m_part->DragEnter(dropEffect, pDataObject, dwKeyState, point, transformHDC);
}

bool MetaDecorator::DragOver(DROPEFFECT* dropEffect, COleDataObject* pDataObject, DWORD dwKeyState, const CPoint& point, HDC transformHDC)
{
	return m_part->DragOver(dropEffect, pDataObject, dwKeyState, point, transformHDC);
}

bool MetaDecorator::Drop(COleDataObject* pDataObject, DROPEFFECT dropEffect, const CPoint& point, HDC transformHDC)
{
	return m_part->Drop(pDataObject, dropEffect, point, transformHDC);
}

bool MetaDecorator::DropFile(HDROP p_hDropInfo, const CPoint& point, HDC transformHDC)
{
	return m_part->DropFile(p_hDropInfo, point, transformHDC);
}

bool MetaDecorator::MenuItemSelected(UINT menuItemId, UINT nFlags, const CPoint& point, HDC transformHDC)
{
	return m_part->MenuItemSelected(menuItemId, nFlags, point, transformHDC);
}

bool MetaDecorator::OperationCanceledByGME(void)
{
	return m_part->OperationCanceledByGME();
}

}; // namespace MetaDecor
