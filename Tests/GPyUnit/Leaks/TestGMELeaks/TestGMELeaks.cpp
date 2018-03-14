
#define UNICODE
#define _UNICODE

#if !defined(__INTELLISENSE__)
#import "libid:9E9AAACF-28B8-11D3-B36C-0060082DF884" // Core
#import "libid:0ADEEC71-D83A-11D3-B36B-005004D38590" // Meta
#import "libid:270B4F86-B17C-11D3-9AD1-00AA00B6FE26" // Mga
#else
// IntelliSense has a known issue with the above lines.
//  http://connect.microsoft.com/VisualStudio/feedback/details/533526/vc-2010-intellisense-import-directive-using-libid-does-not-work
#ifdef _DEBUG
// If IntelliSense reports "Cannot open source file", compile then reopen the project
// KMS: VS intellisense has a hard time loading from typelib GUID. This fixes it:
#include "Debug\Core.tlh"
#include "Debug\Mga.tlh"
#include "Debug\Meta.tlh"
#else
#include "Release\Core.tlh"
#include "Release\Mga.tlh"
#include "Release\Meta.tlh"
#endif
#endif

#include <Windows.h>
#include <comdef.h>
#include <stdio.h>

int main(int argc, char** argv)
{
	setbuf(stdout, NULL);
	char* gme_root = getenv("GME_ROOT");
	if (gme_root == NULL)
	{
		printf("GME_ROOT not defined\n");
		return 12;
	}
	CoInitialize(0);
	for (int i = 0; i < 14; i++)
	{
		try
		{
			using namespace MGALib;
			IMgaProjectPtr project;
			project.CreateInstance(L"MGA.MgaProject", 0, CLSCTX_INPROC);
			project->OpenEx(_bstr_t("MGA=") + _bstr_t(gme_root) + L"\\Paradigms\\MetaGME\\MetaGME-model.mga", _bstr_t("MetaGME"), _variant_t());
			project->BeginTransactionInNewTerr(TRANSACTION_GENERAL);
			project->CommitTransaction();

			project->BeginTransactionInNewTerr(TRANSACTION_GENERAL);
			auto aspects = project->ObjectByPath[L"/@Aspects"];
			IMgaFCOPtr aspectsFco = aspects;
			auto copy = aspectsFco->ParentFolder->CopyFCODisp(aspectsFco);
			copy->DestroyObject();

			project->CommitTransaction();
			project->Undo();
			project->Redo();
			project->Undo();
			project->FlushUndoQueue();


			project->Close(VARIANT_TRUE);
		}
		catch (_com_error& e)
		{
			wprintf(L"%s\n", e.ErrorMessage());
			return 11;
		}
		if (i == 0)
		{
			wprintf(L"%S starting\n", argv[0]);
			while (fgetc(stdin) != '\n')
				;
		}
	}

	wprintf(L"%S finished\n", argv[0]);
	while (fgetc(stdin) != '\n')
		;

	CoFreeUnusedLibraries();
	CoUninitialize();
	return 0;
}
