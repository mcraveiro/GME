#include "stdafx.h"
#include "UACUtils.h"

// Vista-dependent icon constants (copied from Commctrl.h)
#define BCM_FIRST               0x1600      // Button control messages
#define BCM_SETSHIELD            (BCM_FIRST + 0x000C)

void CUACUtils::SetShieldIcon(const CButton& button, bool on)
{
	/*
	HICON ShieldIcon;
	SHSTOCKICONINFO sii = {0};
	sii.cbSize = sizeof(sii);
	
	typedef HRESULT (*PSHGSII)(SHSTOCKICONID siid, UINT uFlags, SHSTOCKICONINFO *psii);
	PSHGSII pSHGSII = (PSHGSII) GetProcAddress(
						GetModuleHandle(TEXT("Shell32.dll")),
						"SHGetStockIconInfo");
	if(NULL != pSHGSII)
      pSHGSII(SIID_SHIELD, SHGFI_ICON | SHGFI_SMALLICON, &sii);

	ShieldIcon = sii.hIcon;
	*/
	button.SendMessage(BCM_SETSHIELD, 0, on ? TRUE : FALSE);
}

