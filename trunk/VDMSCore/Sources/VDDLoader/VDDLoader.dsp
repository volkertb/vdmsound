# Microsoft Developer Studio Project File - Name="VDDLoader" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=VDDLoader - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "VDDLoader.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "VDDLoader.mak" CFG="VDDLoader - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "VDDLoader - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "VDDLoader - Win32 Unicode Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "VDDLoader - Win32 Release MinSize" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "VDDLoader - Win32 Release MinDependency" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "VDDLoader - Win32 Unicode Release MinSize" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "VDDLoader - Win32 Unicode Release MinDependency" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/VDMSCore/VDDLoader", DBAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "VDDLoader - Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /Gz /MDd /W3 /Gm /GX /ZI /Od /I "$(NTDDKPath)/inc" /I "../Interfaces" /I "../MFCUtil" /I "../VDMUtil" /D "WIN32" /D "_DEBUG" /D "_ATL_DEBUG_INTERFACES" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /I "../Interfaces"
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept /libpath:"../MFCUtil/Debug" /libpath:"../VDMUtil/Debug" /libpath:"$(NTDDKPath)/lib/i386/free"
# Begin Custom Build - Performing registration
OutDir=.\Debug
TargetPath=.\Debug\VDDLoader.dll
InputPath=.\Debug\VDDLoader.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "VDDLoader - Win32 Unicode Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DebugU"
# PROP BASE Intermediate_Dir "DebugU"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugU"
# PROP Intermediate_Dir "DebugU"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /Gz /MDd /W3 /Gm /GX /ZI /Od /I "$(NTDDKPath)/inc" /I "../Interfaces" /I "../MFCUtil" /I "../VDMUtil" /D "WIN32" /D "_DEBUG" /D "_ATL_DEBUG_INTERFACES" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "_UNICODE" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /I "../Interfaces"
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept /libpath:"../MFCUtil/DebugU" /libpath:"../VDMUtil/DebugU" /libpath:"$(NTDDKPath)/lib/i386/free"
# Begin Custom Build - Performing registration
OutDir=.\DebugU
TargetPath=.\DebugU\VDDLoader.dll
InputPath=.\DebugU\VDDLoader.dll
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

!ELSEIF  "$(CFG)" == "VDDLoader - Win32 Release MinSize"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseMinSize"
# PROP BASE Intermediate_Dir "ReleaseMinSize"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseMinSize"
# PROP Intermediate_Dir "ReleaseMinSize"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Gz /MD /W3 /GX /O2 /Ob2 /I "$(NTDDKPath)/inc" /I "../Interfaces" /I "../MFCUtil" /I "../VDMUtil" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /D "_ATL_DLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /I "../Interfaces"
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386 /libpath:"../MFCUtil/Release" /libpath:"../VDMUtil/Release" /libpath:"$(NTDDKPath)/lib/i386/free"
# Begin Custom Build - Performing registration
OutDir=.\ReleaseMinSize
TargetPath=.\ReleaseMinSize\VDDLoader.dll
InputPath=.\ReleaseMinSize\VDDLoader.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "VDDLoader - Win32 Release MinDependency"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseMinDependency"
# PROP BASE Intermediate_Dir "ReleaseMinDependency"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseMinDependency"
# PROP Intermediate_Dir "ReleaseMinDependency"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Gz /MD /W3 /GX /O2 /Ob2 /I "$(NTDDKPath)/inc" /I "../Interfaces" /I "../MFCUtil" /I "../VDMUtil" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /D "_ATL_STATIC_REGISTRY" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /I "../Interfaces"
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386 /libpath:"../MFCUtil/Release" /libpath:"../VDMUtil/Release" /libpath:"$(NTDDKPath)/lib/i386/free"
# Begin Custom Build - Performing registration
OutDir=.\ReleaseMinDependency
TargetPath=.\ReleaseMinDependency\VDDLoader.dll
InputPath=.\ReleaseMinDependency\VDDLoader.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "VDDLoader - Win32 Unicode Release MinSize"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseUMinSize"
# PROP BASE Intermediate_Dir "ReleaseUMinSize"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseUMinSize"
# PROP Intermediate_Dir "ReleaseUMinSize"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "_UNICODE" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Gz /MD /W3 /GX /O2 /Ob2 /I "$(NTDDKPath)/inc" /I "../Interfaces" /I "../MFCUtil" /I "../VDMUtil" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "_UNICODE" /D "_ATL_DLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /I "../Interfaces"
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386 /libpath:"../MFCUtil/ReleaseU" /libpath:"../VDMUtil/ReleaseU" /libpath:"$(NTDDKPath)/lib/i386/free"
# Begin Custom Build - Performing registration
OutDir=.\ReleaseUMinSize
TargetPath=.\ReleaseUMinSize\VDDLoader.dll
InputPath=.\ReleaseUMinSize\VDDLoader.dll
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

!ELSEIF  "$(CFG)" == "VDDLoader - Win32 Unicode Release MinDependency"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseUMinDependency"
# PROP BASE Intermediate_Dir "ReleaseUMinDependency"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseUMinDependency"
# PROP Intermediate_Dir "ReleaseUMinDependency"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "_UNICODE" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Gz /MD /W3 /GX /O2 /Ob2 /I "$(NTDDKPath)/inc" /I "../Interfaces" /I "../MFCUtil" /I "../VDMUtil" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "_UNICODE" /D "_ATL_STATIC_REGISTRY" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /I "../Interfaces"
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386 /libpath:"../MFCUtil/ReleaseU" /libpath:"../VDMUtil/ReleaseU" /libpath:"$(NTDDKPath)/lib/i386/free"
# Begin Custom Build - Performing registration
OutDir=.\ReleaseUMinDependency
TargetPath=.\ReleaseUMinDependency\VDDLoader.dll
InputPath=.\ReleaseUMinDependency\VDDLoader.dll
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

# Name "VDDLoader - Win32 Debug"
# Name "VDDLoader - Win32 Unicode Debug"
# Name "VDDLoader - Win32 Release MinSize"
# Name "VDDLoader - Win32 Release MinDependency"
# Name "VDDLoader - Win32 Unicode Release MinSize"
# Name "VDDLoader - Win32 Unicode Release MinDependency"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
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

!IF  "$(CFG)" == "VDDLoader - Win32 Debug"

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

!ELSEIF  "$(CFG)" == "VDDLoader - Win32 Unicode Debug"

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

!ELSEIF  "$(CFG)" == "VDDLoader - Win32 Release MinSize"

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

!ELSEIF  "$(CFG)" == "VDDLoader - Win32 Release MinDependency"

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

!ELSEIF  "$(CFG)" == "VDDLoader - Win32 Unicode Release MinSize"

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

!ELSEIF  "$(CFG)" == "VDDLoader - Win32 Unicode Release MinDependency"

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
# Begin Group "16-bit"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DOSDrv.cpp

!IF  "$(CFG)" == "VDDLoader - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Compiling 16-bit driver $(InputPath) $(InputName)
InputPath=.\DOSDrv.cpp
InputName=DOSDrv

".\16bitDrv\Debug\$(InputName).exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	mkdir .\16bitDrv\Debug 
	bcc -I$(BCPATH)\include -L$(BCPATH)\lib -n.\16bitDrv\Debug -R -mt -2 -f- -N -v -y -r- -Od $(InputPath) 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "VDDLoader - Win32 Unicode Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Compiling 16-bit driver $(InputPath)
InputPath=.\DOSDrv.cpp
InputName=DOSDrv

".\16bitDrv\Debug\$(InputName).exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	mkdir .\16bitDrv\Debug 
	bcc -I$(BCPATH)\include -L$(BCPATH)\lib -n.\16bitDrv\Debug -R -mt -2 -f- -N -v -y -r- -Od $(InputPath) 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "VDDLoader - Win32 Release MinSize"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Compiling 16-bit driver $(InputPath)
InputPath=.\DOSDrv.cpp
InputName=DOSDrv

".\16bitDrv\Release\$(InputName).exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	mkdir .\16bitDrv\Release 
	bcc -I$(BCPATH)\include -L$(BCPATH)\lib -n.\16bitDrv\Release -R -mt -2 -G- -O1 -f- -N- -v- -vi -y- -r -d $(InputPath) 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "VDDLoader - Win32 Release MinDependency"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Compiling 16-bit driver $(InputPath)
InputPath=.\DOSDrv.cpp
InputName=DOSDrv

".\16bitDrv\Release\$(InputName).exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	mkdir .\16bitDrv\Release 
	bcc -I$(BCPATH)\include -L$(BCPATH)\lib -n.\16bitDrv\Release -R -mt -2 -G- -O1 -f- -N- -v- -vi -y- -r -d $(InputPath) 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "VDDLoader - Win32 Unicode Release MinSize"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Compiling 16-bit driver $(InputPath)
InputPath=.\DOSDrv.cpp
InputName=DOSDrv

".\16bitDrv\Release\$(InputName).exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	mkdir .\16bitDrv\Release 
	bcc -I$(BCPATH)\include -L$(BCPATH)\lib -n.\16bitDrv\Release -R -mt -2 -G- -O1 -f- -N- -v- -vi -y- -r -d $(InputPath) 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "VDDLoader - Win32 Unicode Release MinDependency"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Compiling 16-bit driver $(InputPath)
InputPath=.\DOSDrv.cpp
InputName=DOSDrv

".\16bitDrv\Release\$(InputName).exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	mkdir .\16bitDrv\Release 
	bcc -I$(BCPATH)\include -L$(BCPATH)\lib -n.\16bitDrv\Release -R -mt -2 -G- -O1 -f- -N- -v- -vi -y- -r -d $(InputPath) 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DOSDrv.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\VDDLoader.def
# End Source File
# End Target
# End Project
