#!/usr/bin/python
#
# Copyright (c) 2006-2008 ISIS, Vanderbilt University
#
# Author: Peter Volgyesi (peter.volgyesi@vanderbilt.edu)
#
"""GME Build System - central module"""

import sys
import os, os.path
import getopt
from prefs import prefs
import tools
import win32com


#
# Constants
#
GME_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))

paradigms_root = os.path.join(GME_ROOT, "Paradigms")
METAGME_XMP = os.path.join(paradigms_root, "MetaGME", "MetaGME.xmp")
UML_XMP = os.path.join(paradigms_root, "UML", "UML.xmp")
SF_XMP = os.path.join(paradigms_root, "SF", "SF.xmp")
HFSM_XMP = os.path.join(paradigms_root, "HFSM", "HFSM.xmp")
def replace_ext(file, newext):
    import os.path
    return os.path.splitext(file)[0] + "." + newext
def mta_for_xmp(file):
    return replace_ext(file, "mta")

#
# Build steps
#

def check_prerequisites():
    "Check prerequisites (required tools, etc.)"
    
    # Test for GME_ROOT environment variable
    if not os.environ['GME_ROOT']:
        print "GME_ROOT environment variable is not set! (It should point to the GMESRC folder)"
        raise
        
    if not os.environ['JAVA_HOME']:
        print "JAVA_HOME environment variable is not set! (It should point to the JDK root folder)"
        raise
        
    if os.path.normpath(os.path.abspath(os.environ['GME_ROOT'])) != GME_ROOT:
        print "GME_ROOT environment variable is not set to the current dev. source tree!"
        print "GME_ROOT =", os.environ['GME_ROOT']
        print "Current dev. source tree:", GME_ROOT
        raise
    
    # Test for Microsoft Visual Studio 2010
    try:
        tools.test_VS()
    except:
        print "Microsoft Visual Studio 2010 is not installed!"
        raise
    
    # Test for zip utility
    try:
        tools.test_zip()
    except:
        print "ZIP utility cannot be found!"
        raise
    
    # Test for InstallShield
    try:
        tools.test_WiX()
    except:
        print "WiX toolset cannot be found in your path!"
        raise
    
    # Test for SVN
    try:
        tools.test_SVN()
    except:
        print "Warning: SVN client cannot be found! You won't be able to check in & tag releases"


def update_version_str():
    "Update version strings in source files"
    with open(os.path.join(GME_ROOT, 'GME/Gme/GMEVersion.h.tmpl')) as template:
        template_text = "".join(template.readlines())
    old_text = None
    try:
        with open(os.path.join(GME_ROOT, 'GME/Gme/GMEVersion.h')) as header:
            old_text = "".join(header.readlines())
    except:
        pass
    text = template_text % (prefs["version_major"], prefs["version_minor"], prefs["version_patch"], prefs["version_build"])
    if text != old_text:
        with open(os.path.join(GME_ROOT, 'GME/Gme/GMEVersion.h'), 'w') as header:
            header.write(text)
    # resource compiler doesn't do dependency tracking well
    #  echo `grep -rlI GME_VERSION GME | grep \\.rc | sed 's/^/"/; s/$/",/'`
    for filename in ("GME/Console/Console.rc", "GME/ConstraintManager/ConstraintManager.rc", "GME/Core/Core.rc", "GME/Gme/GME.rc", "GME/Gme/res/AboutBox.rc", 
            "GME/GMEActiveBrowser/GMEActiveBrowser.rc", "GME/Meta/Meta.rc", "GME/Mga/Mga.rc", "GME/MgaUtil/MgaUtil.rc", "GME/ObjectInspector/ObjectInspector.rc", 
            "GME/PanningView/PanningView.rc", "GME/Parser/Parser.rc", "GME/PartBrowser/PartBrowser.rc", "GME/Search/Search.rc", "GME/XmlBackEnd/XmlBackEnd.rc"):
        os.utime(os.path.join(GME_ROOT, filename), None)

def _remove_dlldata_from_tlog():
    ''' Workaround for http://connect.microsoft.com/VisualStudio/feedback/details/763929/incremental-build-of-idl-files-behavior-changes-after-installing-visual-studio-2012
    Incremental build of IDL files behavior changes after installing Visual Studio 2012
    MSBuild always rebuilds the project because dlldata.c is included in the file tracker log (midl.read.1.tlog) of the project
    '''
    tlog = os.path.join(GME_ROOT, "GME", "Interfaces", "Release", "midl.read.1.tlog")
    if not os.path.isfile(tlog):
        return
    import codecs
    with codecs.open(tlog, encoding='utf-16-le') as f:
        lines = f.readlines()
        lines = [line for line in lines if line.find(u'DLLDATA.C') == -1]
    with codecs.open(tlog, 'w', encoding='utf-16-le') as f:
        for line in lines: f.write(line)
        
def compile_GME():
    "Compile GME core components"
    if prefs['arch'] == 'x64':
        # To use 32bit Python.exe for the tests, 32bit CoreCollectionHandler must be registered
        sln_file = os.path.join(GME_ROOT, "GME", "GME.sln")
        _remove_dlldata_from_tlog()
        tools.build_VS(sln_file, 'Release', arch='Win32', target='Core')
        _remove_dlldata_from_tlog()
        tools.system(['regsvr32', '/s', os.path.join(GME_ROOT, "GME", "Release", "Core.dll")])
    sln_file = os.path.join(GME_ROOT, "GME", "GME.sln")
    _remove_dlldata_from_tlog()
    tools.build_VS( sln_file, "Release" )
    _remove_dlldata_from_tlog()
    sln_file = os.path.join(GME_ROOT, "GME", "GMEDecorators.sln")
    tools.build_VS( sln_file, "Release" )
    cmd_dir = os.path.join(GME_ROOT, "GME")
    tools.system( ['call', 'regrelease.bat'] + (['x64'] if prefs['arch'] == 'x64' else []) + ['<NUL'], cmd_dir)
    sln_file = os.path.join(GME_ROOT, "GME", "DotNetPIAs", "DotNetPIAs.vcxproj")
    tools.build_VS( sln_file, "Release" )
    
    for filename in ('policy.1.0.GME.MGA.Core', 'policy.1.0.GME.MGA.Meta', 'policy.1.0.GME.MGA', 'policy.1.0.GME', 'policy.1.0.GME.Util', 'policy.1.0.GME.MGA.Parser'):
        pia_dir = os.path.join(GME_ROOT, "GME", "DotNetPIAs_1.0.1.0")
        config = '%s.config' % filename
        dll = '%s.dll' % filename
        if newer(os.path.join(pia_dir, config), os.path.join(pia_dir, dll)):
            tools.system([r'C:\Program Files (x86)\Microsoft SDKs\Windows\v7.0A\Bin\al.exe',
                '/link:' + config, '/out:' + dll,
                '/keyfile:..\MgaDotNetServices\MgaDotNetServicesKey.snk', '/platform:anycpu', '/version:1.0.0.0'], pia_dir)
            tools.system([r'C:\Program Files (x86)\Microsoft SDKs\Windows\v7.0A\Bin\gacutil.exe', '/i', dll], pia_dir)
    
def _Release_PGO_dir():
    if prefs['arch'] == 'x64':
        return os.path.join(GME_ROOT, 'GME', 'x64', 'Release_PGO')
    else:
        return os.path.join(GME_ROOT, 'GME', 'Release_PGO')

def newer (source, target):
    """Return true if 'source' exists and is more recently modified than
       'target', or if 'source' exists and 'target' doesn't.  Return
       false if both exist and 'target' is the same age or younger than
       'source'. """

    if not os.path.exists (target):
        return 1

    from stat import ST_MTIME
    mtime1 = os.stat(source)[ST_MTIME]
    mtime2 = os.stat(target)[ST_MTIME]

    return mtime1 > mtime2

def copy_if_newer(source, target):
    import shutil
    if newer(source, target):
        shutil.copyfile(source, target)

def compile_GME_PGO_Instrument():
    "Compile GME core components (PGO Instrument)"
    # e.g. Dispatch("MGA.MgaParser") locks Parser.dll. FreeLibary it here
    import ctypes
    ctypes.windll.ole32.CoFreeUnusedLibraries()
    import errno
    try:
        os.makedirs(_Release_PGO_dir())
    except OSError as exc:
        if exc.errno != errno.EEXIST:
            raise
    VC_path = os.path.join(prefs['VS_dir'], r"VC\bin\%s" % (prefs['arch'] == 'x64' and 'amd64\\' or ''))
    copy_if_newer(os.path.join(VC_path, r"pgort%s0.dll" % prefs['toolset']), os.path.join(_Release_PGO_dir(), 'pgort%s0.dll' % prefs['toolset']))
    sln_file = os.path.join(GME_ROOT, "GME", "GME.sln")
    _remove_dlldata_from_tlog()
    tools.build_VS(sln_file, "Release_PGO_Instrument")
    _remove_dlldata_from_tlog()
    cmd_dir = os.path.join(GME_ROOT, "GME")
    tools.system( ['call', 'regPGO.bat'] + (['x64'] if prefs['arch'] == 'x64' else []) + ['<NUL'], cmd_dir)

def compile_GME_PGO_Optimize():
    "Compile GME core components (PGO Optimize)"
    sln_file = os.path.join(GME_ROOT, "GME", "GME.sln")
    _remove_dlldata_from_tlog()
    tools.build_VS( sln_file, "Release_PGO_Optimize" )
    _remove_dlldata_from_tlog()

def PGO_train():
    "Run tests/Create training data for the PGO binaries"
    import glob
    for file in glob.glob(GME_ROOT + '\\GME' + ('\\x64' if prefs['arch'] == 'x64' else '') + '\\Release_PGO\\*.pgc'):
        os.remove(file)
    tools.system([sys.executable, '-m', 'GPyUnit.__main__', '-x'] + (['-a', 'x64', '-o', 'tests_x64.xml'] if prefs['arch'] == 'x64' else []), os.path.join(GME_ROOT, 'Tests'))
    if prefs['arch'] == 'x64':
        # wait for dllhost.exe to exit
        import time
        time.sleep(31)

def compile_meta():
    "Compile MetaGME components"
    sln_file = os.path.join(GME_ROOT, "Paradigms", "MetaGME", "MetaGME.sln")
    tools.build_VS( sln_file, "Release" )
    cmd_dir = os.path.join(GME_ROOT, "Paradigms", "MetaGME")
    tools.system( ['call', 'regrelease.bat'] + (['x64'] if prefs['arch'] == 'x64' else []) + ['<NUL'], cmd_dir)

        
def compile_JBON():
    "Compile Java component support (JBON)"
    if prefs['arch'] == 'x64': return
    tools.system(r"reg add HKLM\Software\GME /t REG_SZ /v JavaClassPath /d".split() +
        [os.path.join(GME_ROOT, "SDK", "Java", "gme.jar"), "/reg:32", "/f"])
    sln_file = os.path.join(GME_ROOT, "SDK", "Java", "native", "JavaSupport.sln")
    tools.build_VS( sln_file, "Release" )

        
def compile_tools():
    "Compile external tool components"
    import _winreg
    
    # Auto Layout
    sln_file = os.path.join(GME_ROOT, "Tools", "AutoLayout", "AutoLayout.sln")
    tools.build_VS( sln_file, "Release" )

    sln_file = os.path.join(GME_ROOT, "SDK", "DotNet", "DsmlGenerator", "DsmlGenerator.sln")
    tools.build_VS(sln_file, "Release", arch='Any CPU', msbuild=(prefs['arch'] == 'x64' and tools.MSBUILD.replace('Framework', 'Framework64') or tools.MSBUILD))
    
    with _winreg.OpenKey(_winreg.HKEY_LOCAL_MACHINE, r"SOFTWARE\Microsoft", 0, _winreg.KEY_WOW64_32KEY | _winreg.KEY_WRITE | _winreg.KEY_READ) as ms:
        with _winreg.CreateKey(ms, r".NETFramework\v4.0.30319\AssemblyFoldersEx\ISIS.GME.Common") as key:
            _winreg.SetValueEx(key, None, 0, _winreg.REG_SZ, os.path.join(os.environ['windir'], r"Microsoft.NET\assembly\GAC_MSIL\ISIS.GME.Common\v4.0_1.0.7.0__1321e6b92842fe54"))
    
    sln_file = os.path.join(GME_ROOT, "Tools", "DumpWMF", "DumpWMF.sln")
    tools.build_VS(sln_file, "Release", arch='Any CPU', msbuild=(prefs['arch'] == 'x64' and tools.MSBUILD.replace('Framework', 'Framework64') or tools.MSBUILD))

    if prefs['arch'] == 'x64':
        tools.system([r'%windir%\Microsoft.NET\Framework64\v4.0.30319\RegAsm.exe', '/codebase',
                      os.path.join(GME_ROOT, 'Tools', 'DumpWMF', 'bin', 'Release', 'DumpWMF.dll')])
        with _winreg.OpenKey(_winreg.HKEY_CLASSES_ROOT, r"CLSID\{A051FEEA-E310-3F6A-8D71-A55E3F4F2E14}", 0, _winreg.KEY_WRITE | _winreg.KEY_WOW64_64KEY) as key:
            _winreg.SetValueEx(key, "AppID", 0, _winreg.REG_SZ, "{461F30AF-3BF0-11D4-B3F0-005004D38590}")

        tools.system([r'%windir%\Microsoft.NET\Framework64\v4.0.30319\RegAsm.exe', '/codebase',
                      os.path.join(GME_ROOT, 'SDK', 'DotNet', 'DsmlGenerator', 'CSharpDsmlGenerator', 'bin', 'Release', 'CSharpDSMLGenerator.dll')])
        return

    # Table Editor
    sln_file = os.path.join(GME_ROOT, "Tools", "TableEditor", "TableEditor.sln")
    tools.build_VS( sln_file, "Release" )
    
    # GMEplink
    sln_file = os.path.join(GME_ROOT, "Tools", "GMEplink", "GMEplink.sln")
    tools.build_VS( sln_file, "Release" )

    sln_file = os.path.join(GME_ROOT, "SDK", "DotNet", "CSharpComponentWizard", "CSharpComponentWizard.sln")
    tools.build_VS( sln_file, "Release" )

def compile_samples():
    "Compile sample components"
    
    # UML Paradigm
    sln_file = os.path.join(GME_ROOT, "Paradigms", "UML", "decorator", "UMLDecorator.sln")
    tools.build_VS( sln_file, "Release" )

    if prefs['arch'] == 'x64': return

    sln_file = os.path.join(GME_ROOT, "SDK", "PatternProcessor", "PatternProcessor.sln")
    tools.build_VS( sln_file, "Release" )
    
    # SF Paradigm
    sln_file = os.path.join(GME_ROOT, "Paradigms", "SF", "SFInterpreter", "SFInterpreter.sln")
    tools.build_VS( sln_file, "Release" )
    sln_file = os.path.join(GME_ROOT, "Paradigms", "SF", "BON2SFSample", "BON2SFSample.sln")
    tools.build_VS( sln_file, "Release" )
    sln_file = os.path.join(GME_ROOT, "Paradigms", "SF", "BON2SFInterpreter", "BON2SFInterpreter.sln")
    tools.build_VS( sln_file, "Release" )
    
    # HFSM Paradigm
    sln_file = os.path.join(GME_ROOT, "Paradigms", "HFSM", "HFSMSimulator", "HFSMSimulator.sln")
    tools.build_VS( sln_file, "Release" )
    

def zip_decorsamples():
    "Create PlainDecoratorSample.zip"
    zip_dir = os.path.join(GME_ROOT, "SDK", "Decorator Examples", "PlainSample")
    tools.zip(zip_dir, "PlainDecoratorSample.zip", "packagefiles.lst")

    "Create NewDecoratorSample.zip"
    zip_dir = os.path.join(GME_ROOT, "SDK", "Decorator Examples", "NewSample")
    tools.zip(zip_dir, "NewDecoratorSample.zip", "packagefiles.lst")


def zip_scriptSDK():
    "Create ScriptSDK.zip"
    zip_dir = os.path.join(GME_ROOT, "SDK", "ScriptSDK")
    tools.zip(zip_dir, "ScriptSDK.zip", "packagefiles.lst")

def generate_meta_files():
    "Generate meta files (mta/mga)"
    meta_root = os.path.join(GME_ROOT, "Paradigms", "MetaGME")
    tools.xmp2mta(METAGME_XMP, "MetaGME")
    meta_file = os.path.join(meta_root, "MetaGME-model.xme")
    tools.xme2mga(meta_file, "MetaGME")


def generate_sample_files():
    "Generate sample files (mta/mga)"
    samples_root = os.path.join(GME_ROOT, "Paradigms")
    
    # SF Paradigm
    sample_file = os.path.join(samples_root, "SF", "SFMeta.xme")
    tools.xme2mga(sample_file, "MetaGME")
    tools.xmp2mta(SF_XMP, "SF") 
    sample_file = os.path.join(samples_root, "SF", "SFDemo.xme")
    tools.xme2mga(sample_file, "SF")

    # HFSM Paradigm
    sample_file = os.path.join(samples_root, "HFSM", "HFSM-Meta.xme")
    tools.xme2mga(sample_file, "MetaGME")
    tools.xmp2mta(HFSM_XMP, "HFSM") 
    sample_file = os.path.join(samples_root, "HFSM", "HFSM-Demo01.xme")
    tools.xme2mga(sample_file, "HFSM")
    sample_file = os.path.join(samples_root, "HFSM", "HFSM-Demo02.xme")
    tools.xme2mga(sample_file, "HFSM")
    
    # UML Paradigm
    sample_file = os.path.join(samples_root, "UML", "UMLMeta.xme")
    tools.xme2mga(sample_file, "MetaGME")
    tools.xmp2mta(UML_XMP, "UML")

def build_nuget():
    "Build NuGet packages"
    dsml_generator = os.path.join(GME_ROOT, "SDK", "DotNet", "DsmlGenerator")
    tools.system([tools.MSBUILD, os.path.join(dsml_generator, ".nuget", "NuGet.Targets"), "/t:CheckPrerequisites", "/p:DownloadNuGetExe=True"])
    nuget = os.path.join(dsml_generator, ".nuget", "NuGet.exe")
    tools.system([nuget, "pack", os.path.join(dsml_generator, "GME.DSMLGenerator.nuspec"),
        "-Verbosity", "detailed",
        "-BasePath", dsml_generator,
        "-OutputDirectory", os.path.join(GME_ROOT, "Install")])
        
    tools.system([nuget, "pack", os.path.join(dsml_generator, "GME.DSMLGenerator.Runtime.nuspec"),
        "-Verbosity", "detailed",
        "-BasePath", dsml_generator,
        "-OutputDirectory", os.path.join(GME_ROOT, "Install")])
        
    dotnet_pias = os.path.join(GME_ROOT, "GME", "DotNetPIAs")
    tools.system([nuget, "pack", os.path.join(dotnet_pias, "GME.PIAs.nuspec"),
        "-Verbosity", "detailed",
        "-BasePath", dotnet_pias,
        "-OutputDirectory", os.path.join(GME_ROOT, "Install")])

def build_msms():
    """Build WiX libraries (wixlibs files)
    (Still called build_msms, for historical reasons)
    """
    
    # Prepare include file with dynamic data
    f = open(os.path.join(GME_ROOT, "Install", "GME_dyn.wxi"), 'w')
    print >> f, "<!-- DO NOT EDIT THIS FILE. WILL BE REGENERATED BY THE BUILD SCRIPTS -->"
    print >> f, "<Include>"
    print >> f, "   <?define GUIDSTRMETAGME='%s' ?>" % (tools.query_GUID(mta_for_xmp(METAGME_XMP)))
    print >> f, "   <?define GUIDSTRHFSM='%s' ?>" % (tools.query_GUID(mta_for_xmp(HFSM_XMP)))
    print >> f, "   <?define GUIDSTRSF='%s' ?>" % (tools.query_GUID(mta_for_xmp(SF_XMP)))
    print >> f, "   <?define GUIDSTRUML='%s' ?>" % (tools.query_GUID(mta_for_xmp(UML_XMP)))
    print >> f, "</Include>" 
    f.close()
   
    import glob
    sources = [f for f in glob.glob(os.path.join(GME_ROOT, "Install", "*.wxs")) if f.find('GME.wxs') == -1 ]
    if prefs['arch'] == 'x64':
        sources.remove(os.path.join(GME_ROOT, "Install", "GME_SDK.wxs"))
        sources.remove(os.path.join(GME_ROOT, "Install", "GME_paradigms.wxs"))
    for file_ in sources:
        extras = []
        if os.path.basename(file_) == 'GME_paradigms.wxs':
            extras = glob.glob(os.path.join(GME_ROOT, "Install", "PIA*/*.wxi"))
        tools.build_WiX([file_] + extras)

def build_msi():
    "Build WiX installer (msi file)"

    # Build the msi file
    tools.build_WiX([os.path.join(GME_ROOT, "Install", "GME.wxs")])
   

def zip_pdb():
    "Collect and zip all debug information (*.pdb)"
    tools.system(r"call install\symbols_source_server.cmd <NUL".split(), GME_ROOT)
    zipname = os.path.join(GME_ROOT, "Install", "GME-" + prefs['version_string'] + "-symbols.zip")
    tools.collect_and_zip(GME_ROOT, zipname, "*.pdb *.dll *.exe *.ocx")
    pass

def publish():
    "Publish and archive the install image and debug info"
    pass


def tag_repository():
    "Check in and Tag SVN repository (only for releases)"
    pass


def do_step(num, step):
    "Executing one building step given in param 'step'"
    print str(num) + ".", step.__doc__, "..."
    step()


#
# Main entry point
#

build_steps = [
    check_prerequisites,
    update_version_str,
    compile_GME,
    compile_meta,
    compile_JBON,
    compile_tools,
    compile_samples, 
    zip_decorsamples, 
    zip_scriptSDK, 
    generate_meta_files,
    generate_sample_files, 
    compile_GME_PGO_Instrument,
    PGO_train,
    compile_GME_PGO_Optimize,
    build_nuget,
    build_msms,
    build_msi,
    zip_pdb,
    publish,
    tag_repository
    ]

start_step = 0
end_step = len(build_steps)-1

usage = """
usage: %s [OPTION]...
Build an installation image (msi) for GME.

  -h, --help          display help (this message) and exit
  -v, --verbose       verbose output (default: %s)
  -c, --clean         clean projects before building them (default: %s)
  -s, --start=NUM     start at build step 'NUM' (default: %d)
  -e, --end=NUM       stop at build step 'NUM' (default: %d)
  -i, --include=NUM   include build step 'NUM' explicitly
  -x, --exclude=NUM   exclude build step 'NUM' explicitly

  -V, --version=MAJOR.MINOR.PATCHLEVEL.BUILD
                      set version (default: %d.%d.%d.%d)
  -b, --build_version=BUILD
                      set only the build version  

  -a, --arch=ARCH     set architecture (x64 or x86)
\tBuild steps:

\t%s
""" % (sys.argv[0],
       prefs["verbose"],
       prefs["clean"],
       start_step,
       end_step,
       prefs["version_major"],
       prefs["version_minor"],
       prefs["version_patch"],
       prefs["version_build"],
       "\n\t".join([str(build_steps.index(s)) + ": " + s.__doc__ + ' (' + s.__name__ + ')' for s in build_steps])
       )

try:
    opts, args = getopt.getopt(sys.argv[1:], 'hvcs:e:i:x:V:b:a:',
                               ["help", "verbose", "clean",
                                "start=", "end=", "include=", "exclude=",
                                "version=", "build_version=", "arch="])
    include_steps = []
    exclude_steps = []
    if args:
        print usage
        sys.exit()
    def get_step(arg):
        try:
            return int(arg)
        except ValueError, e:
            return [step.__name__ for step in build_steps].index(arg)
    for opt, val in opts:
        if opt in ("-h", "--help"):
            print usage
            sys.exit()
        if opt in ("-v", "--verbose"):
            prefs["verbose"] = True
        if opt in ("-c", "--clean"):
            prefs["clean"] = True
        if opt in ("-s", "--start"):
            start_step = get_step(val)
        if opt in ("-e", "--end"):
            end_step = get_step(val)
        if opt in ("-i", "--include"):
            step = get_step(val)
            if val not in include_steps:
                include_steps.append(step)
        if opt in ("-x", "--exclude"):
            step = get_step(val)
            if val not in exclude_steps:
                exclude_steps.append(step)
        if opt in ("-b", "--build_version"):
            prefs["version_build"] = int(val)
        if opt in ("-V", "--version"):
            (M, m, p, b) = val.split(".")
            prefs["version_major"] = int(M)
            prefs["version_minor"] = int(m)
            prefs["version_patch"] = int(p)
            prefs["version_build"] = int(b)
        if opt in ("-a", "--arch"):
            prefs["arch"] = val
            
except (getopt.GetoptError, ValueError, AttributeError), e:
    print e
    print usage
    sys.exit(2)
    
prefs["version_string"] = ".".join([str(prefs["version_major"]),
                                   str(prefs["version_minor"]),
                                   str(prefs["version_patch"])] +
                                   ( [ str(prefs["version_build"]) ] if prefs["version_build"] != 0 else [] ))

print "Building GME version " + prefs["version_string"] + " " + prefs["arch"]

_pfx86 = os.environ.get('ProgramFiles(x86)', os.environ['ProgramFiles'])
if prefs['toolset'] == '11':
    prefs['VS_dir'] = os.path.join(_pfx86, r"Microsoft Visual Studio 11.0")
else:
    prefs['VS_dir'] = os.path.join(_pfx86, r"Microsoft Visual Studio 10.0")

try:
    for i in range(len(build_steps)):
        if i in include_steps:
            do_step(i, build_steps[i])
            continue
        if i in exclude_steps:
            continue
        if start_step <= i <= end_step:
            do_step(i, build_steps[i])

    print "Build SUCCEEDED."
except:
    print "!!! Build FAILED: step " + build_steps[i].__name__
    raise
