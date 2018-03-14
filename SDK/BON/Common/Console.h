#pragma once
#include "ComHelp.h"
#include "GMECOM.h"

struct IGMEOLEApp;
struct IMgaProject;
enum msgtype_enum;

namespace GMEConsole
{
	class Console
	{
	public:

		static CComPtr<IGMEOLEApp> gmeoleapp;
		
		static void SetupConsole(CComPtr<IMgaProject> project); 
		static void ReleaseConsole();
		static void WriteLine(const TCHAR* message, msgtype_enum type);

		static void Clear();

		static void SetContents(const TCHAR* contents);

		static void NavigateTo(const TCHAR* url);

		class Error
		{
		public:
			static void WriteLine(const TCHAR* message);
		};
		class Out
		{
		public:
			static void WriteLine(const TCHAR* message);
		};
		class Warning
		{
		public:
			static void WriteLine(const TCHAR* message);
		};
		class Info
		{
		public:
			static void WriteLine(const TCHAR* message);
			// deprecated:
			static void writeLine(const TCHAR* message);
		};
	};
}