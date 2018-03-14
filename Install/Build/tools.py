#!/usr/bin/python
#
# Copyright (c) 2006 ISIS, Vanderbilt University
#
# Author: Peter Volgyesi (peter.volgyesi@vanderbilt.edu)
# Kevin Smyth (ksmyth@isis.vanderbilt.edu)
#
"""GME Build System - tools module"""

import os
import os.path
import win32com.client
from prefs import prefs

#
# Constants
#
ZIP_PRG = os.path.abspath(os.path.join(os.path.dirname(__file__), "zip.exe"))
WIX_CANDLE_PRG = "candle.exe"
WIX_CANDLE_ARG = "-dPIADir.1.0.0.0=..\GME\DotNetPIAs_1.0.0.0 -dPIADir.1.0.1.0=..\GME\DotNetPIAs_1.0.1.0 -dPIADir=..\GME\DotNetPIAs"
WIX_LIGHT_PRG = "light.exe"
WIX_LIGHT_ARG = "-sw1076 -sw1055 -sw1056 -sice:ICE43 -sice:ICE57 -ext WixUIExtension -ext WixUtilExtension -ext WiXNetFxExtension" # See comments in GME.wxs
MSBUILD = r"c:\Windows\Microsoft.NET\Framework\v4.0.30319\MSBuild.exe"

#
# Classes
#
class BuildException(Exception):
    "General Exception Class for all build problems"
    pass

def Dispatch(progid):
    from pythoncom import CLSCTX_ALL, CLSCTX_LOCAL_SERVER
    CLSCTX_ACTIVATE_32_BIT_SERVER = 0x40000
    CLSCTX_ACTIVATE_64_BIT_SERVER = 0x80000
    if prefs['arch'] == 'x64':
        return win32com.client.DispatchEx(progid, clsctx=CLSCTX_LOCAL_SERVER|CLSCTX_ACTIVATE_64_BIT_SERVER)
    else:
        return win32com.client.DispatchEx(progid)
        # return win32com.client.DispatchEx(progid, clsctx=CLSCTX_LOCAL_SERVER|CLSCTX_ACTIVATE_32_BIT_SERVER)

#
# Tools/utilities
#
def toolmsg(str):
    """
    Displays log messages, used by other tool functions
    """
    if prefs['verbose']:
        print "\t" + str + "..."

def test_SVN():
    "Test for SVN client. Raises exception if not found."
    system(['svn', '-v', '>NUL'])


def test_zip():
    "Test for ZIP utility. Raises exception if not found."
    system([ZIP_PRG, '>NUL'])
    
    
def zip(dirname, zipname, list=None):
    """
    Build zip archive in the specified directory.
    The optional 'list' argument can specify the name of the file containing the
    include patterns for the archive.
    params
        dirname : the path to the root of the files to be archived
        zipname : the name of the zip archive (will be created in 'dirname')
        list    : name of the list file (see above)
    """
    cmd_line = [ZIP_PRG, '-9', '-r']
    if list is not None:
        cmd_line.append("-i@" + list)
    cmd_line.extend([zipname, '.', '>NUL'])
    system(cmd_line, dirname)
    
def collect_and_zip(dirname, zipname, pattern=None):
    """
    Collect files (recursively) and build zip archive in the specified directory.
    params
        dirname : the path to the root of the files to be archived
        zipname : the name of the zip archive (will be created in 'dirname')
        pattern : file name pattern for selecting files to be included
    """
    cmd_line = [ZIP_PRG, '-9', '-r', zipname, '.']
    if pattern is not None:
        cmd_line.extend(["-i"] + pattern.split())
    system(cmd_line, dirname)


def system(args, dirname=None):
    """
    Executes a system command (throws an exception on error)
    params
        args : [command, arg1, arg2, ...]
        dirname : if set, execute the command within this directory
    """
    toolmsg("Executing " + str(args))
    #toolmsg(" ".join(map(lambda x: '"' + x + '"', args)))
    import subprocess
    with open(os.devnull, "w") as nulfp:
        # n.b. stderr=subprocess.STDOUT fails mysteriously
        import sys
        subprocess.check_call(args, stdout=(sys.stdout if prefs['verbose'] else nulfp), stderr=subprocess.STDOUT, shell=True, cwd=dirname)



def test_VS():
    "Test for Microsoft Visual Studio 2010. Raises exception if not found."
    toolmsg("Trying to create VisualStudio.DTE object")
    win32com.client.Dispatch("VisualStudio.DTE.10.0")

def build_VS(sln_path, config_name, arch=None, msbuild=MSBUILD, target=None):
    """
    Builds a Microsoft Visual Studio 2010 project or entire solution.
    It cleans the project/solution before building it if the global 'clean' preference
    is set.
    params
        sln_path     : full path to the solution (.sln) file
        config_name  : name of the build configuration (e.g.: "Release")
    """
    msg = "Cleaning and " * prefs['clean']
    msg += "Compiling " + sln_path + "(" + config_name + ") "
    toolmsg(msg)
    arch = arch or prefs['arch']
    target = target or ("Clean;" * prefs['clean']) + 'Build'

    import subprocess
    # , '/fl', '/flp:Verbosity=diagnostic'
    # , '/m'
    args = [msbuild, sln_path, '/m', '/t:' + target, 
         '/p:VisualStudioVersion=%s.0;PlatformToolset=v%s0;Configuration=%s' % (prefs['toolset'], prefs['toolset'], config_name) +
        (';Platform=x64' if arch == 'x64' else '') ]
    with open(os.devnull, "w") as nulfp:
        # n.b. stderr=subprocess.STDOUT fails mysteriously
        import sys
        subprocess.check_call(args, stdout=(sys.stdout if prefs['verbose'] else nulfp), stderr=None, shell=True)

def xme2mga(xml_file, paradigm):
    """
    Generates an .mga file from an .xme file
    params
        xml_file    : full path to the xme file 
        paradigm    : use the specified paradigm to parse the project
        
    The generated .mga file will be created with the same name/path but different
    extension.
    """
    toolmsg("Parsing " + xml_file + " with paradigm " + paradigm)
    parser  = Dispatch( "MGA.MgaParser" )
    project = Dispatch( "MGA.MgaProject" )
    mga_file = os.path.splitext(xml_file)[0] + ".mga"
    project.Create( "MGA="+mga_file, paradigm )
    parser.ParseProject( project, xml_file )
    project.Close()    

    
def xmp2mta(xml_file, paradigm):
    """
    Generates and registers (system-wide) an .mta file from an .xmp file
    params
        xml_file    : full path to the xmp file 
        paradigm    : use the specified paradigm name to parse the paradigm file
        
    The generated .mta file will be created with the same name/path but different
    extension.
    """
    toolmsg("Parsing and registering " + xml_file + " (" + paradigm + ")")
    registrar = Dispatch( "MGA.MgaRegistrar" )
    # KMS: registering user fails if system is already registered. TODO: remove so we dont need elevation
    if paradigm in registrar.GetParadigmsDisp(2):
        registrar.UnregisterParadigm(paradigm, 2)
    registrar.RegisterParadigmFromData( "XML=" + xml_file, paradigm, 1 )


def query_GUID(mta_file):
    """
    Queries the current GUID of the specified paradigm.
    params
        paradigm    : the name of the paradigm to be queried
    
    returns the GUID as a string
    """ 
    metaproject = Dispatch("MGA.MgaMetaProject")
    metaproject.Open('MGA=' + mta_file)
    try:
        import uuid
        return '{' + str(uuid.UUID(bytes_le=metaproject.GUID)).upper() + '}'
    finally:
        metaproject.Close()


def _get_wix_path():
    import _winreg
    for wix_ver in ('3.11', '3.10', '3.9', '3.8', '3.7', '3.6', '3.5'):
        try:
            with _winreg.OpenKey(_winreg.HKEY_LOCAL_MACHINE, 'SOFTWARE\\Microsoft\\Windows Installer XML\\' + wix_ver) as wixkey:
                return _winreg.QueryValueEx(wixkey, 'InstallRoot')[0]
        except Exception as e:
            pass
        
def test_WiX():
    "Test for WiX. Raises exception if not found."
    toolmsg("Trying to execute WiX tool candle.exe")
    exepath = os.path.join(_get_wix_path(), WIX_CANDLE_PRG)
    system([exepath])
    

def _x64_suffix(str):
    return str + '_x64' if prefs['arch'] == 'x64' else str
def _get_wixobj(file):
    return _x64_suffix(os.path.splitext(file)[0]) + ".wixobj"

def _candle(file):
    exepath = os.path.join(_get_wix_path(), WIX_CANDLE_PRG)
    cmd_line = [exepath] + WIX_CANDLE_ARG.split() + ['-dVERSIONSTR=' + prefs['version_string'], '-arch', prefs['arch'], '-out', _get_wixobj(file), file]
    system(cmd_line, os.path.dirname(file))

def build_WiX(wix_files):
    """
    Builds a WiX project.
    """
    fullpath = os.path.normpath(os.path.abspath(wix_files[0]))
    dirname = os.path.dirname(fullpath)
    filename = os.path.basename(fullpath)

    toolmsg("Building " + filename + " in " + dirname)
    wxi_files = filter(lambda file: file.find(".wxi") != -1, wix_files)
    mm_files = filter(lambda file: file.find(".wxs") != -1, wix_files)
    
    for file in wix_files:
        _candle(file)
    
    for wxs in mm_files:
        if wxs.find('GME.wxs') == -1:
            exepath = os.path.join(_get_wix_path(), 'lit.exe')
            ext = '.wixlib'
            wixlibs = ['-bf']
        else:
            exepath = os.path.join(_get_wix_path(), 'light.exe')
            ext = '.msi'
            wixlibs = WIX_LIGHT_ARG.split() + ['GME_bin.wixlib', 'GME_SDK.wixlib', 'GME_paradigms.wixlib']
            if prefs['arch'] == 'x64':
                wixlibs += ['GME_bin_x64.wixlib']
        cmd_line = [exepath] + ['-o', _x64_suffix(os.path.splitext(wxs)[0]) + ext] + [ _get_wixobj(file) for file in wxi_files ] + [ _get_wixobj(wxs)] + wixlibs
        system(cmd_line, dirname)
