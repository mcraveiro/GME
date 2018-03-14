# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

!IF "$(CFG)" == ""
CFG=demo - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to demo - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "demo - Win32 Release" && "$(CFG)" != "demo - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "demo.mak" CFG="demo - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "demo - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "demo - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 
################################################################################
# Begin Project
# PROP Target_Last_Scanned "demo - Win32 Debug"
CPP=cl.exe
F90=fl32.exe
RSC=rc.exe

!IF  "$(CFG)" == "demo - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
OUTDIR=.
INTDIR=.

ALL : "$(OUTDIR)\demo.exe"

CLEAN : 
	-@erase "$(INTDIR)\demo.obj"
	-@erase "$(OUTDIR)\demo.exe"

# ADD BASE F90 /Ox /c /nologo
# ADD F90 /Ox /c /nologo
F90_PROJ=/Ox /c /nologo 
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /I "../include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "../include" /D "WIN32" /D "NDEBUG" /D\
 "_CONSOLE" /Fp"demo.pch" /YX /c 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/demo.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 /nologo /subsystem:console /machine:I386
LINK32_FLAGS=/nologo /subsystem:console /incremental:no\
 /pdb:"$(OUTDIR)/demo.pdb" /machine:I386 /out:"$(OUTDIR)/demo.exe" 
LINK32_OBJS= \
	"$(INTDIR)\demo.obj" \
	"..\lib\mgk60.lib"

"$(OUTDIR)\demo.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "demo - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
OUTDIR=.
INTDIR=.

ALL : "$(OUTDIR)\demo_d.exe"

CLEAN : 
	-@erase "$(INTDIR)\demo.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\demo_d.exe"
	-@erase "$(OUTDIR)\demo_d.ilk"
	-@erase "$(OUTDIR)\demo_d.pdb"

# ADD BASE F90 /Zi /c /nologo
# ADD F90 /Zi /c /nologo
F90_PROJ=/Zi /c /nologo /Fd"demo.pdb" 
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "../include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /c
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /I "../include" /D "WIN32" /D\
 "_DEBUG" /D "_CONSOLE" /Fp"demo.pch" /YX /c 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/demo.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386
# ADD LINK32 /nologo /subsystem:console /debug /machine:I386 /out:"demo_d.exe"
LINK32_FLAGS=/nologo /subsystem:console /incremental:yes\
 /pdb:"$(OUTDIR)/demo_d.pdb" /debug /machine:I386 /out:"$(OUTDIR)/demo_d.exe" 
LINK32_OBJS= \
	"$(INTDIR)\demo.obj" \
	"..\lib\mgk60_d.lib"

"$(OUTDIR)\demo_d.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx.obj:
   $(CPP) $(CPP_PROJ) $<  

.c.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx.sbr:
   $(CPP) $(CPP_PROJ) $<  

.for.obj:
   $(F90) $(F90_PROJ) $<  

.f.obj:
   $(F90) $(F90_PROJ) $<  

.f90.obj:
   $(F90) $(F90_PROJ) $<  

################################################################################
# Begin Target

# Name "demo - Win32 Release"
# Name "demo - Win32 Debug"

!IF  "$(CFG)" == "demo - Win32 Release"

!ELSEIF  "$(CFG)" == "demo - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\demo.c
DEP_CPP_DEMO_=\
	"..\demo\demo.c"\
	".\../include\mgk60.h"\
	".\libmgk.h"\
	".\remcmd.h"\
	

"$(INTDIR)\demo.obj" : $(SOURCE) $(DEP_CPP_DEMO_) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=\work\Mgk60\lib\mgk60.lib

!IF  "$(CFG)" == "demo - Win32 Release"

!ELSEIF  "$(CFG)" == "demo - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\work\Mgk60\lib\mgk60_d.lib

!IF  "$(CFG)" == "demo - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "demo - Win32 Debug"

!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
