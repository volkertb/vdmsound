# Microsoft Developer Studio Project File - Name="VDDLoader" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=VDDLoader - Win32 Debug (NT)
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "VDDLoader.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "VDDLoader.mak" CFG="VDDLoader - Win32 Debug (NT)"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "VDDLoader - Win32 Debug (NT)" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "VDDLoader - Win32 Debug (9x)" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "VDDLoader - Win32 Unicode Debug (NT)" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "VDDLoader - Win32 Release MinSize (NT)" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "VDDLoader - Win32 Release MinSize (9x)" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "VDDLoader - Win32 Release MinDependency (NT)" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "VDDLoader - Win32 Release MinDependency (9x)" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "VDDLoader - Win32 Unicode Release MinSize (NT)" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "VDDLoader - Win32 Unicode Release MinDependency (NT)" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/VDMSCore/VDDLoader", DBAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "VDDLoader - Win32 Debug (NT)"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug.NT"
# PROP BASE Intermediate_Dir "Debug.NT"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug.NT"
# PROP Intermediate_Dir "Debug.NT"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /Gz /MDd /W3 /Gm /GX /ZI /Od /I "$(NTDDKPath)/inc" /I "../Interfaces" /I "../MFCUtil" /I "../VDMUtil" /D "WIN32" /D "_DEBUG" /D "_ATL_DEBUG_INTERFACES" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /D "_NTVDM_SVC" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /I "../Interfaces"
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL" /d "_NTVDM_SVC"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept /libpath:"../MFCUtil/Debug" /libpath:"../VDMUtil/Debug" /libpath:"$(NTDDKPath)/lib/i386/free"
# Begin Custom Build - Performing registration
OutDir=.\Debug.NT
TargetPath=.\Debug.NT\VDDLoader.dll
InputPath=.\Debug.NT\VDDLoader.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "VDDLoader - Win32 Debug (9x)"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug.9x"
# PROP BASE Intermediate_Dir "Debug.9x"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug.9x"
# PROP Intermediate_Dir "Debug.9x"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /Gz /MDd /W3 /Gm /GX /ZI /Od /I "../Interfaces" /I "../MFCUtil" /I "../VDMUtil" /D "WIN32" /D "_DEBUG" /D "_ATL_DEBUG_INTERFACES" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /D "_VXD_SVC" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /I "../Interfaces"
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL" /d "_VXD_SVC"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept /libpath:"../MFCUtil/Debug" /libpath:"../VDMUtil/Debug"
# Begin Custom Build - Performing registration
OutDir=.\Debug.9x
TargetPath=.\Debug.9x\VDDLoader.dll
InputPath=.\Debug.9x\VDDLoader.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "VDDLoader - Win32 Unicode Debug (NT)"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DebugU.NT"
# PROP BASE Intermediate_Dir "DebugU.NT"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugU.NT"
# PROP Intermediate_Dir "DebugU.NT"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /Gz /MDd /W3 /Gm /GX /ZI /Od /I "$(NTDDKPath)/inc" /I "../Interfaces" /I "../MFCUtil" /I "../VDMUtil" /D "WIN32" /D "_DEBUG" /D "_ATL_DEBUG_INTERFACES" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "_UNICODE" /D "_NTVDM_SVC" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /I "../Interfaces"
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL" /d "_NTVDM_SVC"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept /libpath:"../MFCUtil/DebugU" /libpath:"../VDMUtil/DebugU" /libpath:"$(NTDDKPath)/lib/i386/free"
# Begin Custom Build - Performing registration
OutDir=.\DebugU.NT
TargetPath=.\DebugU.NT\VDDLoader.dll
InputPath=.\DebugU.NT\VDDLoader.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if "%OS%"=="" goto NOTNT 
	if not "%OS%"=="Windows_NT" goto NOTNT 
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	goto end 
	:NOTNT 
	echo Warning : Cannot register Unicode DLL on Windows 95 
	:end 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "VDDLoader - Win32 Release MinSize (NT)"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseMinSize.NT"
# PROP BASE Intermediate_Dir "ReleaseMinSize.NT"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseMinSize.NT"
# PROP Intermediate_Dir "ReleaseMinSize.NT"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Gz /MD /W3 /GX /O2 /Ob2 /I "$(NTDDKPath)/inc" /I "../Interfaces" /I "../MFCUtil" /I "../VDMUtil" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /D "_ATL_DLL" /D "_NTVDM_SVC" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /I "../Interfaces"
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL" /d "_NTVDM_SVC"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386 /libpath:"../MFCUtil/Release" /libpath:"../VDMUtil/Release" /libpath:"$(NTDDKPath)/lib/i386/free"
# Begin Custom Build - Performing registration
OutDir=.\ReleaseMinSize.NT
TargetPath=.\ReleaseMinSize.NT\VDDLoader.dll
InputPath=.\ReleaseMinSize.NT\VDDLoader.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "VDDLoader - Win32 Release MinSize (9x)"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseMinSize.9x"
# PROP BASE Intermediate_Dir "ReleaseMinSize.9x"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseMinSize.9x"
# PROP Intermediate_Dir "ReleaseMinSize.9x"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Gz /MD /W3 /GX /O2 /Ob2 /I "../Interfaces" /I "../MFCUtil" /I "../VDMUtil" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /D "_ATL_DLL" /D "_VXD_SVC" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /I "../Interfaces"
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL" /d "_VXD_SVC"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386 /libpath:"../MFCUtil/Release" /libpath:"../VDMUtil/Release"
# Begin Custom Build - Performing registration
OutDir=.\ReleaseMinSize.9x
TargetPath=.\ReleaseMinSize.9x\VDDLoader.dll
InputPath=.\ReleaseMinSize.9x\VDDLoader.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "VDDLoader - Win32 Release MinDependency (NT)"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseMinDependency.NT"
# PROP BASE Intermediate_Dir "ReleaseMinDependency.NT"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseMinDependency.NT"
# PROP Intermediate_Dir "ReleaseMinDependency.NT"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Gz /MD /W3 /GX /O2 /Ob2 /I "$(NTDDKPath)/inc" /I "../Interfaces" /I "../MFCUtil" /I "../VDMUtil" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /D "_ATL_STATIC_REGISTRY" /D "_NTVDM_SVC" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /I "../Interfaces"
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL" /d "_NTVDM_SVC"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386 /libpath:"../MFCUtil/Release" /libpath:"../VDMUtil/Release" /libpath:"$(NTDDKPath)/lib/i386/free"
# Begin Custom Build - Performing registration
OutDir=.\ReleaseMinDependency.NT
TargetPath=.\ReleaseMinDependency.NT\VDDLoader.dll
InputPath=.\ReleaseMinDependency.NT\VDDLoader.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "VDDLoader - Win32 Release MinDependency (9x)"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseMinDependency.9x"
# PROP BASE Intermediate_Dir "ReleaseMinDependency.9x"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseMinDependency.9x"
# PROP Intermediate_Dir "ReleaseMinDependency.9x"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Gz /MD /W3 /GX /O2 /Ob2 /I "../Interfaces" /I "../MFCUtil" /I "../VDMUtil" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /D "_ATL_STATIC_REGISTRY" /D "_VXD_SVC" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /I "../Interfaces"
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL" /d "_VXD_SVC"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386 /libpath:"../MFCUtil/Release" /libpath:"../VDMUtil/Release"
# Begin Custom Build - Performing registration
OutDir=.\ReleaseMinDependency.9x
TargetPath=.\ReleaseMinDependency.9x\VDDLoader.dll
InputPath=.\ReleaseMinDependency.9x\VDDLoader.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "VDDLoader - Win32 Unicode Release MinSize (NT)"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseUMinSize.NT"
# PROP BASE Intermediate_Dir "ReleaseUMinSize.NT"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseUMinSize.NT"
# PROP Intermediate_Dir "ReleaseUMinSize.NT"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "_UNICODE" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Gz /MD /W3 /GX /O2 /Ob2 /I "$(NTDDKPath)/inc" /I "../Interfaces" /I "../MFCUtil" /I "../VDMUtil" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "_UNICODE" /D "_ATL_DLL" /D "_NTVDM_SVC" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /I "../Interfaces"
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL" /d "_NTVDM_SVC"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386 /libpath:"../MFCUtil/ReleaseU" /libpath:"../VDMUtil/ReleaseU" /libpath:"$(NTDDKPath)/lib/i386/free"
# Begin Custom Build - Performing registration
OutDir=.\ReleaseUMinSize.NT
TargetPath=.\ReleaseUMinSize.NT\VDDLoader.dll
InputPath=.\ReleaseUMinSize.NT\VDDLoader.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if "%OS%"=="" goto NOTNT 
	if not "%OS%"=="Windows_NT" goto NOTNT 
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	goto end 
	:NOTNT 
	echo Warning : Cannot register Unicode DLL on Windows 95 
	:end 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "VDDLoader - Win32 Unicode Release MinDependency (NT)"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseUMinDependency.NT"
# PROP BASE Intermediate_Dir "ReleaseUMinDependency.NT"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseUMinDependency.NT"
# PROP Intermediate_Dir "ReleaseUMinDependency.NT"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "_UNICODE" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Gz /MD /W3 /GX /O2 /Ob2 /I "$(NTDDKPath)/inc" /I "../Interfaces" /I "../MFCUtil" /I "../VDMUtil" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "_UNICODE" /D "_ATL_STATIC_REGISTRY" /D "_NTVDM_SVC" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /I "../Interfaces"
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL" /d "_NTVDM_SVC"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386 /libpath:"../MFCUtil/ReleaseU" /libpath:"../VDMUtil/ReleaseU" /libpath:"$(NTDDKPath)/lib/i386/free"
# Begin Custom Build - Performing registration
OutDir=.\ReleaseUMinDependency.NT
TargetPath=.\ReleaseUMinDependency.NT\VDDLoader.dll
InputPath=.\ReleaseUMinDependency.NT\VDDLoader.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if "%OS%"=="" goto NOTNT 
	if not "%OS%"=="Windows_NT" goto NOTNT 
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	goto end 
	:NOTNT 
	echo Warning : Cannot register Unicode DLL on Windows 95 
	:end 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "VDDLoader - Win32 Debug (NT)"
# Name "VDDLoader - Win32 Debug (9x)"
# Name "VDDLoader - Win32 Unicode Debug (NT)"
# Name "VDDLoader - Win32 Release MinSize (NT)"
# Name "VDDLoader - Win32 Release MinSize (9x)"
# Name "VDDLoader - Win32 Release MinDependency (NT)"
# Name "VDDLoader - Win32 Release MinDependency (9x)"
# Name "VDDLoader - Win32 Unicode Release MinSize (NT)"
# Name "VDDLoader - Win32 Unicode Release MinDependency (NT)"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ApiHook.cpp
# End Source File
# Begin Source File

SOURCE=.\IOPortMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\VDDDispatch.cpp
# End Source File
# Begin Source File

SOURCE=.\VDDLoader.cpp
# End Source File
# Begin Source File

SOURCE=.\VDMServices.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\ApiHook.h
# End Source File
# Begin Source File

SOURCE=.\IOPortMgr.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\VDMServices.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\Messages.mc

!IF  "$(CFG)" == "VDDLoader - Win32 Debug (NT)"

# Begin Custom Build - Custom Build - Generating Message Table ($(InputPath))
InputPath=.\Messages.mc
InputName=Messages

BuildCmds= \
	mc $(InputPath)

"$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "VDDLoader - Win32 Debug (9x)"

# Begin Custom Build - Custom Build - Generating Message Table ($(InputPath))
InputPath=.\Messages.mc
InputName=Messages

BuildCmds= \
	mc $(InputPath)

"$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "VDDLoader - Win32 Unicode Debug (NT)"

# Begin Custom Build - Custom Build - Generating Message Table ($(InputPath))
InputPath=.\Messages.mc
InputName=Messages

BuildCmds= \
	mc $(InputPath)

"$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "VDDLoader - Win32 Release MinSize (NT)"

# Begin Custom Build - Custom Build - Generating Message Table ($(InputPath))
InputPath=.\Messages.mc
InputName=Messages

BuildCmds= \
	mc $(InputPath)

"$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "VDDLoader - Win32 Release MinSize (9x)"

# Begin Custom Build - Custom Build - Generating Message Table ($(InputPath))
InputPath=.\Messages.mc
InputName=Messages

BuildCmds= \
	mc $(InputPath)

"$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "VDDLoader - Win32 Release MinDependency (NT)"

# Begin Custom Build - Custom Build - Generating Message Table ($(InputPath))
InputPath=.\Messages.mc
InputName=Messages

BuildCmds= \
	mc $(InputPath)

"$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "VDDLoader - Win32 Release MinDependency (9x)"

# Begin Custom Build - Custom Build - Generating Message Table ($(InputPath))
InputPath=.\Messages.mc
InputName=Messages

BuildCmds= \
	mc $(InputPath)

"$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "VDDLoader - Win32 Unicode Release MinSize (NT)"

# Begin Custom Build - Custom Build - Generating Message Table ($(InputPath))
InputPath=.\Messages.mc
InputName=Messages

BuildCmds= \
	mc $(InputPath)

"$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "VDDLoader - Win32 Unicode Release MinDependency (NT)"

# Begin Custom Build - Custom Build - Generating Message Table ($(InputPath))
InputPath=.\Messages.mc
InputName=Messages

BuildCmds= \
	mc $(InputPath)

"$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\VDDLoader.rc
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x418
# End Source File
# Begin Source File

SOURCE=.\VDMServices.rgs
# End Source File
# End Group
# Begin Group "Interface Files"

# PROP Default_Filter "idl;tlb"
# Begin Source File

SOURCE=.\VDDLoader.idl
# ADD MTL /tlb ".\VDDLoader.tlb" /h "VDDLoader.h" /iid "VDDLoader_i.c" /Oicf
# End Source File
# End Group
# Begin Group "WinNT-specific"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DOSDrv.cpp

!IF  "$(CFG)" == "VDDLoader - Win32 Debug (NT)"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Compiling 16-bit driver $(InputPath) $(InputName)
InputPath=.\DOSDrv.cpp
InputName=DOSDrv

".\16bitDrv\Debug\$(InputName).exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	mkdir .\16bitDrv\Debug 
	bcc -I$(BCPATH)\include -L$(BCPATH)\lib -n.\16bitDrv\Debug -R -mt -2 -f- -N -v -y -r- -Od $(InputPath) 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "VDDLoader - Win32 Debug (9x)"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=.\DOSDrv.cpp
InputName=DOSDrv

# End Custom Build

!ELSEIF  "$(CFG)" == "VDDLoader - Win32 Unicode Debug (NT)"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Compiling 16-bit driver $(InputPath)
InputPath=.\DOSDrv.cpp
InputName=DOSDrv

".\16bitDrv\Debug\$(InputName).exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	mkdir .\16bitDrv\Debug 
	bcc -I$(BCPATH)\include -L$(BCPATH)\lib -n.\16bitDrv\Debug -R -mt -2 -f- -N -v -y -r- -Od $(InputPath) 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "VDDLoader - Win32 Release MinSize (NT)"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Compiling 16-bit driver $(InputPath)
InputPath=.\DOSDrv.cpp
InputName=DOSDrv

".\16bitDrv\Release\$(InputName).exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	mkdir .\16bitDrv\Release 
	bcc -I$(BCPATH)\include -L$(BCPATH)\lib -n.\16bitDrv\Release -R -mt -2 -G- -f- -N- -v- -vi -y- -r -d $(InputPath) 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "VDDLoader - Win32 Release MinSize (9x)"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=.\DOSDrv.cpp
InputName=DOSDrv

# End Custom Build

!ELSEIF  "$(CFG)" == "VDDLoader - Win32 Release MinDependency (NT)"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Compiling 16-bit driver $(InputPath)
InputPath=.\DOSDrv.cpp
InputName=DOSDrv

".\16bitDrv\Release\$(InputName).exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	mkdir .\16bitDrv\Release 
	bcc -I$(BCPATH)\include -L$(BCPATH)\lib -n.\16bitDrv\Release -R -mt -2 -G- -f- -N- -v- -vi -y- -r -d $(InputPath) 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "VDDLoader - Win32 Release MinDependency (9x)"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=.\DOSDrv.cpp
InputName=DOSDrv

# End Custom Build

!ELSEIF  "$(CFG)" == "VDDLoader - Win32 Unicode Release MinSize (NT)"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Compiling 16-bit driver $(InputPath)
InputPath=.\DOSDrv.cpp
InputName=DOSDrv

".\16bitDrv\Release\$(InputName).exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	mkdir .\16bitDrv\Release 
	bcc -I$(BCPATH)\include -L$(BCPATH)\lib -n.\16bitDrv\Release -R -mt -2 -G- -f- -N- -v- -vi -y- -r -d $(InputPath) 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "VDDLoader - Win32 Unicode Release MinDependency (NT)"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Compiling 16-bit driver $(InputPath)
InputPath=.\DOSDrv.cpp
InputName=DOSDrv

".\16bitDrv\Release\$(InputName).exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	mkdir .\16bitDrv\Release 
	bcc -I$(BCPATH)\include -L$(BCPATH)\lib -n.\16bitDrv\Release -R -mt -2 -G- -f- -N- -v- -vi -y- -r -d $(InputPath) 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DOSDrv.h
# End Source File
# End Group
# Begin Group "Win9x-specific"
# Begin Source File

SOURCE=.\vxdsdll.h
# End Source File
# End Group

# Begin Source File

SOURCE=.\VDDLoader.def
# End Source File
# End Target
# End Project
