// PendingObjectPosRequest.cpp: implementation of the CPendingObjectPosRequest class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GMEstd.h"
#include "GuiObject.h"
#include "GMEApp.h"
#include "GMEview.h"
#include "guimeta.h"
#include "PendingObjectPosRequest.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPendingObjectPosRequest::~CPendingObjectPosRequest()
{

}

void CPendingObjectPosRequest::Execute(CGMEView *view)
{
	// TRACE("\tXV: Pending request execute:");
	VERIFY(view->inTransaction);
	if (!object)
		return;
	if (!object->mgaFco)
		return;
	if(aspect < 0)
		aspect = object->GetParentAspect();
	VERIFY(aspect >= 0);
	// TRACE(" object: %s, mgaptr: %p, location: %d-%d\n", object->name, object->mgaFco, (int)location.TopLeft().x, (int)location.TopLeft().y);
	try {

		long status;
		COMTHROW(object->mgaFco->get_Status(&status));
		if (status != OBJECT_EXISTS)
			return;

		CComPtr<IMgaPart> part;
		CGuiMetaAspect *guiAsp = view->guiMeta->FindAspect(aspect);
		CComPtr<IMgaMetaBase> mBase = guiAsp->mgaMeta;
		CComPtr<IMgaMetaAspect> mAspect;
		COMTHROW(mBase.QueryInterface(&mAspect));
		(object->mgaFco)->get_Part(mAspect,&part);
		COMTHROW(part->SetGmeAttrs(0, location.left, location.top));
		// TRACE(" \t\tEXECUTED\n");
	}
	catch(hresult_exception &) {
		// AfxMessageBox("Cannot execute pendig request (ObjectPos)!");
	}
}
