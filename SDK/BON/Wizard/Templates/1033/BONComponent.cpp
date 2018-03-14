#include "stdafx.h"

#include <Console.h>
#include "BONComponent.h"


/* 
// This method is usually no longer in use and does not need to be implemented,
// except when DEPRECATED_BON_INVOKE_IMPLEMENTED is defined in Component.h.
// See the top section of ComponentObj.cpp for details.
void CComponent::Invoke(CBuilder &builder,CBuilderObjectList &selected, long param)
{
	AfxMessageBox("GME Component --- Sample", MB_OK | MB_ICONSTOP);
}
*/

void CComponent::InvokeEx(CBuilder &builder,CBuilderObject *focus, CBuilderObjectList &selected, long param) 
{
	using namespace GMEConsole;
	Console::Out::WriteLine(_T("Interpreter started..."));
	AfxMessageBox(_T("GME Component --- Sample rf:") + builder.GetRootFolder()->GetName(), MB_OK | MB_ICONSTOP);
	Console::Out::WriteLine(_T("Interpreter completed..."));
}
