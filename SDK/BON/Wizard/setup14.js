// GME Component Wizard Installer
// Copyright (C) Vanderbilt University, ISIS
// Copyright (C) Microsoft Corporation. All rights reserved.
//
// This script is based on a similar installer for the WTL library.
//
// The use and distribution terms for this software are covered by the
// Common Public License 1.0 (http://opensource.org/osi3.0/licenses/cpl1.0.php)
// which can be found in the file CPL.TXT at the root of this distribution.
// By using this software in any fashion, you are agreeing to be bound by
// the terms of this license. You must not remove this notice, or
// any other, from this software.

// Setup program for the GME Component Wizard for VC++ 14.0 (Visual Studio 2015)


main("14.0", "Visual Studio 2015");

function main(vc_ver, vs_ver)
{
	var componentPrefixes = ['RawComponent', 'BONComponent', 'BON2Component'];
	
	// Decode command line arguments
	var bDebug = false;
	var bElevated = false;
	var Args = WScript.Arguments;
	for(var i = 0; i < Args.length; i++)
	{
		if(Args(i) == "/debug")
			bDebug = true;
		else if(Args(i) == "/elevated")
			bElevated = true;
	}

	// See if UAC is enabled
	var Shell = WScript.CreateObject("Shell.Application");
	if(!bElevated && Shell.IsRestricted("System", "EnableLUA"))
	{
		// Check that the script is being run interactively.
		if(!WScript.Interactive)
		{
			WScript.Echo("ERROR: Elevation required.");
			return;
		}
		
		// Now relaunch the script, using the "RunAs" verb to elevate
		var strParams = "\"" + WScript.ScriptFullName + "\"";
		if (bDebug)
			strParams += " /debug";
		strParams += " /elevated";
		Shell.ShellExecute(WScript.FullName, strParams, null, "RunAs");
		return;
	}
	
	// Create shell object
	var WSShell = WScript.CreateObject("WScript.Shell");
	// Create file system object
	var FileSys = WScript.CreateObject("Scripting.FileSystemObject");

	// Get the folder containing the script file
	var strValue = FileSys.GetParentFolderName(WScript.ScriptFullName);
	if(strValue == null || strValue == "")
		strValue = ".";

	//var strSourceFolder = FileSys.BuildPath(strValue, "Files");
	var strSourceFolder = strValue;
	
	if(bDebug)
		WScript.Echo("Source: " + strSourceFolder);

	if(!FileSys.FolderExists(strSourceFolder))
	{
		WScript.Echo("ERROR: Cannot find Wizard folder (should be: " + strSourceFolder + ")");
		return;
	}

	try
	{
		var strVCKey = "HKLM\\Software\\Microsoft\\VisualStudio\\" + vc_ver + "\\Setup\\VC\\ProductDir";
		strValue = WSShell.RegRead(strVCKey);
	}
	catch(e)
	{
		try
		{
			var strVCKey_x64 = "HKLM\\Software\\Wow6432Node\\Microsoft\\VisualStudio\\" + vc_ver + "\\Setup\\VC\\ProductDir";
			strValue = WSShell.RegRead(strVCKey_x64);
		}
		catch(e)
		{
			try
			{
				var strVCExKey = "HKLM\\Software\\Microsoft\\VCExpress\\" + vc_ver + "\\Setup\\VC\\ProductDir";
				strValue = WSShell.RegRead(strVCExKey);
			}
			catch(e)
			{
				WScript.Echo("Cannot find where " + vs_ver + " is installed. UdmInterpreter wizard will be unavailable.");
				return;
			}
			WScript.Echo("Visual Studio Express doesn't support Wizards. UdmInterpreter Wizard will be unavailable.");
			return;
		}
	}

	var strDestFolder = FileSys.BuildPath(strValue, "vcprojects");
	if(bDebug)
		WScript.Echo("Destination: " + strDestFolder);
	if(!FileSys.FolderExists(strDestFolder))
	{
		WScript.Echo("ERROR: Cannot find destination folder (should be: " + strDestFolder + ")");
		return;
	}

	// Copy files
	try
	{
		for (i in componentPrefixes) {
			var strSrc = FileSys.BuildPath(strSourceFolder, componentPrefixes[i] + ".ico");
			var strDest = FileSys.BuildPath(strDestFolder, componentPrefixes[i] + ".ico");
		FileSys.CopyFile(strSrc, strDest);
		}

		strSrc = FileSys.BuildPath(strSourceFolder, "ComponentWizard.vsdir");
		strDest = FileSys.BuildPath(strDestFolder, "ComponentWizard.vsdir");
		FileSys.CopyFile(strSrc, strDest);
	}
	catch(e)
	{
		var strError = "no info";
		if(e.description.length != 0)
			strError = e.description;
		WScript.Echo("ERROR: Cannot copy file (" + strError + ")");
		return;
	}

	// Read and write *Component.vsz, add engine version and replace path when found
	try
	{
		for (i in componentPrefixes) {
			var strSrc = FileSys.BuildPath(strSourceFolder, componentPrefixes[i] + ".vsz");
			var strDest = FileSys.BuildPath(strDestFolder, componentPrefixes[i] + ".vsz");

		var ForReading = 1;
		var fileSrc = FileSys.OpenTextFile(strSrc, ForReading);
		if(fileSrc == null)
		{
			WScript.Echo("ERROR: Cannot open source file " + strSrc);
			return;
		}

		var ForWriting = 2;
		var fileDest = FileSys.OpenTextFile(strDest, ForWriting, true);
		if(fileDest == null)
		{
			WScript.Echo("ERROR: Cannot open destination file" + strDest);
			return;
		}

		while(!fileSrc.AtEndOfStream)
		{
			var strLine = fileSrc.ReadLine();
			if(strLine.indexOf("Wizard=VsWizard.VsWizardEngine") != -1)
				strLine = "Wizard=VsWizard.VsWizardEngine." + vc_ver;
			else if(strLine.indexOf("WIZARD_VERSION") != -1)
				strLine = "Param=\"WIZARD_VERSION = " + vc_ver + "\"";
			else if(strLine.indexOf("ABSOLUTE_PATH") != -1)
				strLine = "Param=\"ABSOLUTE_PATH = " + strSourceFolder + "\"";
			fileDest.WriteLine(strLine);
		}

		fileSrc.Close();
		fileDest.Close();
	}
	}
	catch(e)
	{
		var strError = "no info";
		if(e.description.length != 0)
			strError = e.description;
		WScript.Echo("ERROR: Cannot read and write wizard descriptor (*.vsz) (" + strError + ")");
		return;
	}

	// Create GME folder
	var strDestGMEFolder = "";
	try
	{
		strDestGMEFolder = FileSys.BuildPath(strDestFolder, "GME");
		if(!FileSys.FolderExists(strDestGMEFolder))
			FileSys.CreateFolder(strDestGMEFolder);
		if(bDebug)
			WScript.Echo("GME Folder: " + strDestGMEFolder);
	}
	catch(e)
	{
		var strError = "no info";
		if(e.description.length != 0)
			strError = e.description;
		WScript.Echo("ERROR: Cannot create GME folder (" + strError + ")");
		return;
	}

	// Read and write additional ComponentWizard.vsdir, add path to the wizard location
	try
	{
		var strSrc = FileSys.BuildPath(strSourceFolder, "ComponentWizard.vsdir");
		var strDest = FileSys.BuildPath(strDestGMEFolder, "ComponentWizard.vsdir");

		var ForReading = 1;
		var fileSrc = FileSys.OpenTextFile(strSrc, ForReading);
		if(fileSrc == null)
		{
			WScript.Echo("ERROR: Cannot open source file " + strSrc);
			return;
		}

		var ForWriting = 2;
		var fileDest = FileSys.OpenTextFile(strDest, ForWriting, true);
		if(fileDest == null)
		{
			WScript.Echo("ERROR: Cannot open destination file" + strDest);
			return;
		}

		while(!fileSrc.AtEndOfStream)
		{
			var strLine = fileSrc.ReadLine();
			if(strLine.indexOf(".vsz|") != -1)
				strLine = "..\\" + strLine;
			fileDest.WriteLine(strLine);
		}

		fileSrc.Close();
		fileDest.Close();
	}
	catch(e)
	{
		var strError = "no info";
		if(e.description.length != 0)
			strError = e.description;
		WScript.Echo("ERROR: Cannot read and write GME\\ComponentWizard.vsdir (" + strError + ")");
		return;
	}

	WScript.Echo("GME Component Wizard successfully installed!");
}
