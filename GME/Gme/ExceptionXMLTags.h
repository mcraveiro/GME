/////////////////////////////////////////////////////////////////////////////
//
// ExceptionXMLTags.h : Constant definitions for the XML format crash dump
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(EXCEPTION_XML_TAGS_INCLUDED)
#define EXCEPTION_XML_TAGS_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


const TCHAR* CrashDumpXMLTag				= "CrashDump";
const TCHAR* ComputerInfoXMLTag				= "ComputerInfo";
const TCHAR* ComputerNameXMLTag				= "ComputerName";
const TCHAR* UserNameXMLTag					= "UserName";
const TCHAR* AcountNameXMLTag				= "AccountName";
const TCHAR* DomainNameXMLTag				= "DomainName";
const TCHAR* ModulVersionXMLTag				= "ModulVersion";
const TCHAR* ProgramInfoXMLTag				= "ProgramInfo";
const TCHAR* ProgramVersionXMLTag			= "ProgramVersion";
const TCHAR* CommandLineXMLTag				= "CommandLine";
const TCHAR* ProcessIDXMLTag				= "ProcessID";
const TCHAR* ThreadIDXMLTag					= "ThreadID";
const TCHAR* OSInfoXMLTag					= "OSInfo";
const TCHAR* VersionInfoXMLTag				= "VersionInfo";
const TCHAR* PlatformIdXMLTag				= "PlatformId";
const TCHAR* Windows9xPlatform				= "Windows 9x";
const TCHAR* WindowsNTPlatform				= "Windows NT";
const TCHAR* MajorVersionXMLTag				= "MajorVersion";
const TCHAR* MinorVersionXMLTag				= "MinorVersion";
const TCHAR* BuildNumberXMLTag				= "BuildNumber";
const TCHAR* BitLengthXMLTag				= "BitLength";
const TCHAR* Length32Bit					= "32-bit";
const TCHAR* Length64Bit					= "64-bit";
const TCHAR* CSDVersionXMLTag				= "CSDVersion";
const TCHAR* RevisionNumberXMLTag			= "RevisionNumber";
const TCHAR* PatchLevelXMLTag				= "PatchLevel";
const TCHAR* ReservedXMLTag					= "Reserved";
const TCHAR* SessionStartXMLTag				= "SessionStart";
const TCHAR* SystemTimeXMLTag				= "SystemTime";
const TCHAR* YearXMLTag						= "Year";
const TCHAR* MonthXMLTag					= "Month";
const TCHAR* DayXMLTag						= "Day";
const TCHAR* HourXMLTag						= "Hour";
const TCHAR* MinuteXMLTag					= "Minute";
const TCHAR* SecondXMLTag					= "Second";
const TCHAR* ExceptionInfoXMLTag			= "ExceptionInfo";
const TCHAR* ExceptionUserInfoXMLTag		= "ExceptionUserInfo";
const TCHAR* ExceptionCodeXMLTag			= "ExceptionCode";
const TCHAR* ExceptionStringXMLTag			= "ExceptionString";
const TCHAR* ExceptionAddressXMLTag			= "ExceptionAddress";
const TCHAR* FaultAddressXMLTag				= "FaultAddress";
const TCHAR* FaultingModuleXMLTag			= "FaultingModule";
const TCHAR* ImageSectionXMLTag				= "ImageSection";
const TCHAR* OffsetXMLTag					= "Offset";
const TCHAR* IllegalAddressString			= "Illegal";
const TCHAR* RegistersXMLTag				= "Registers";
const TCHAR* EAXRegisterXMLTag				= "EAX";
const TCHAR* EBXRegisterXMLTag				= "EBX";
const TCHAR* ECXRegisterXMLTag				= "ECX";
const TCHAR* EDXRegisterXMLTag				= "EDX";
const TCHAR* ESIRegisterXMLTag				= "ESI";
const TCHAR* EDIRegisterXMLTag				= "EDI";
const TCHAR* CSRegisterXMLTag				= "CS";
const TCHAR* EIPRegisterXMLTag				= "EIP";
const TCHAR* SSRegisterXMLTag				= "SS";
const TCHAR* ESPRegisterXMLTag				= "ESP";
const TCHAR* EBPRegisterXMLTag				= "EBP";
const TCHAR* DSRegisterXMLTag				= "DS";
const TCHAR* ESRegisterXMLTag				= "ES";
const TCHAR* FSRegisterXMLTag				= "FS";
const TCHAR* GSRegisterXMLTag				= "GS";
const TCHAR* FlagsRegisterXMLTag			= "Flags";
const TCHAR* FPRegistersXMLTag				= "FPRegisters";
const TCHAR* ControlWordXMLTag				= "ControlWord";
const TCHAR* StatusWordXMLTag				= "StatusWord";
const TCHAR* TagWordXMLTag					= "TagWord";
const TCHAR* ErrorSelectorXMLTag			= "ErrorSelector";
const TCHAR* ErrorOffsetXMLTag				= "ErrorOffset";
const TCHAR* DataSelectorXMLTag				= "DataSelector";
const TCHAR* DataOffsetXMLTag				= "DataOffset";
const TCHAR* Cr0NpxStateXMLTag				= "Cr0NpxState";
const TCHAR* STXMLTag						= "STRegisters";
const TCHAR* FPRegisterXMLTag				= "FPRegister";
const TCHAR* FPHexStringXMLTag				= "FPHexString";
const TCHAR* CallStackXMLTag				= "CallStack";
const TCHAR* DbgHelpVersionXMLTag			= "DbgHelpVersion";
const TCHAR* ImageHelpApiVersionXMLTag		= "ImageHelpAPIVersion";
const TCHAR* StackLevelXMLTag				= "StackLevel";
const TCHAR* AddressXMLTag					= "Address";
const TCHAR* FrameXMLTag					= "Frame";
const TCHAR* SymbolNameXMLTag				= "SymbolName";
const TCHAR* SymbolDisplacementXMLTag		= "SymbolDisplacement";
const TCHAR* ModuleNameXMLTag				= "ModuleName";
const TCHAR* SourceFileXMLTag				= "SourceFile";
const TCHAR* LineInfoXMLTag					= "LineInfo";
const TCHAR* VariablesXMLTag				= "Variables";
const TCHAR* ParameterXMLTag				= "Parameter";
const TCHAR* LocalXMLTag					= "Local";
const TCHAR* VariableNameXMLTag				= "VariableName";
const TCHAR* SymTagFunctionXMLTag			= "SymTagFunction";
const TCHAR* RegisterXMLTag					= "Register";
const TCHAR* ValueXMLTag					= "Value";
const TCHAR* TypeNameXMLTag					= "TypeName";
const TCHAR* TypeDumpXMLTag					= "TypeDump";
const TCHAR* GlobalVariablesXMLTag			= "GlobalVariables";


#endif // !defined(EXCEPTION_XML_TAGS_INCLUDED)
