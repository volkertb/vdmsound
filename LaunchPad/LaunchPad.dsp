# Microsoft Developer Studio Project File - Name="LaunchPad" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=LaunchPad - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "LaunchPad.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "LaunchPad.mak" CFG="LaunchPad - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "LaunchPad - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "LaunchPad - Win32 Unicode Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "LaunchPad - Win32 Release MinSize" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "LaunchPad - Win32 Release MinDependency" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "LaunchPad - Win32 Unicode Release MinSize" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "LaunchPad - Win32 Unicode Release MinDependency" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "LaunchPad - Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "_MBCS" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 delayimp.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept /delayload:shell32.dll /delayload:advapi32.dll /delayload:shlwapi.dll
# Begin Custom Build - Performing registration
OutDir=.\Debug
TargetPath=.\Debug\LaunchPad.dll
InputPath=.\Debug\LaunchPad.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "LaunchPad - Win32 Unicode Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DebugU"
# PROP BASE Intermediate_Dir "DebugU"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugU"
# PROP Intermediate_Dir "DebugU"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "_UNICODE" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 delayimp.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept /delayload:shell32.dll /delayload:advapi32.dll /delayload:shlwapi.dll
# Begin Custom Build - Performing registration
OutDir=.\DebugU
TargetPath=.\DebugU\LaunchPad.dll
InputPath=.\DebugU\LaunchPad.dll
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

!ELSEIF  "$(CFG)" == "LaunchPad - Win32 Release MinSize"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseMinSize"
# PROP BASE Intermediate_Dir "ReleaseMinSize"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseMinSize"
# PROP Intermediate_Dir "ReleaseMinSize"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O1 /D "NDEBUG" /D "_MBCS" /D "_ATL_DLL" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 delayimp.lib /nologo /subsystem:windows /dll /machine:I386 /delayload:shell32.dll /delayload:advapi32.dll /delayload:shlwapi.dll
# Begin Custom Build - Performing registration
OutDir=.\ReleaseMinSize
TargetPath=.\ReleaseMinSize\LaunchPad.dll
InputPath=.\ReleaseMinSize\LaunchPad.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "LaunchPad - Win32 Release MinDependency"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseMinDependency"
# PROP BASE Intermediate_Dir "ReleaseMinDependency"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseMinDependency"
# PROP Intermediate_Dir "ReleaseMinDependency"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O1 /D "NDEBUG" /D "_MBCS" /D "_ATL_STATIC_REGISTRY" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 delayimp.lib /nologo /subsystem:windows /dll /machine:I386 /delayload:shell32.dll /delayload:advapi32.dll /delayload:shlwapi.dll
# Begin Custom Build - Performing registration
OutDir=.\ReleaseMinDependency
TargetPath=.\ReleaseMinDependency\LaunchPad.dll
InputPath=.\ReleaseMinDependency\LaunchPad.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "LaunchPad - Win32 Unicode Release MinSize"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseUMinSize"
# PROP BASE Intermediate_Dir "ReleaseUMinSize"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseUMinSize"
# PROP Intermediate_Dir "ReleaseUMinSize"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "_UNICODE" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O1 /D "NDEBUG" /D "_UNICODE" /D "_ATL_DLL" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 delayimp.lib /nologo /subsystem:windows /dll /machine:I386 /delayload:shell32.dll /delayload:advapi32.dll /delayload:shlwapi.dll
# Begin Custom Build - Performing registration
OutDir=.\ReleaseUMinSize
TargetPath=.\ReleaseUMinSize\LaunchPad.dll
InputPath=.\ReleaseUMinSize\LaunchPad.dll
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

!ELSEIF  "$(CFG)" == "LaunchPad - Win32 Unicode Release MinDependency"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseUMinDependency"
# PROP BASE Intermediate_Dir "ReleaseUMinDependency"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseUMinDependency"
# PROP Intermediate_Dir "ReleaseUMinDependency"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "_UNICODE" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O1 /D "NDEBUG" /D "_UNICODE" /D "_ATL_STATIC_REGISTRY" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 delayimp.lib /nologo /subsystem:windows /dll /machine:I386 /delayload:shell32.dll /delayload:advapi32.dll /delayload:shlwapi.dll
# Begin Custom Build - Performing registration
OutDir=.\ReleaseUMinDependency
TargetPath=.\ReleaseUMinDependency\LaunchPad.dll
InputPath=.\ReleaseUMinDependency\LaunchPad.dll
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

# Name "LaunchPad - Win32 Debug"
# Name "LaunchPad - Win32 Unicode Debug"
# Name "LaunchPad - Win32 Release MinSize"
# Name "LaunchPad - Win32 Release MinDependency"
# Name "LaunchPad - Win32 Unicode Release MinSize"
# Name "LaunchPad - Win32 Unicode Release MinDependency"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "Advanced Settings Source Files"

# PROP Default_Filter "cpp"
# Begin Source File

SOURCE=.\AdvSettingsPage_Adlib.cpp
# End Source File
# Begin Source File

SOURCE=.\AdvSettingsPage_Compat.cpp
# End Source File
# Begin Source File

SOURCE=.\AdvSettingsPage_Debug.cpp
# End Source File
# Begin Source File

SOURCE=.\AdvSettingsPage_Dosenv.cpp
# End Source File
# Begin Source File

SOURCE=.\AdvSettingsPage_Joy.cpp
# End Source File
# Begin Source File

SOURCE=.\AdvSettingsPage_Midi.cpp
# End Source File
# Begin Source File

SOURCE=.\AdvSettingsPage_Perf.cpp
# End Source File
# Begin Source File

SOURCE=.\AdvSettingsPage_Program.cpp
# End Source File
# Begin Source File

SOURCE=.\AdvSettingsPage_Sb.cpp
# End Source File
# Begin Source File

SOURCE=.\AdvSettingsSheet.cpp
# End Source File
# End Group
# Begin Group "Basic Settings Source Files"

# PROP Default_Filter "cpp"
# Begin Source File

SOURCE=.\BasicBrowseDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\BasicSettingsPage.cpp
# End Source File
# End Group
# Begin Group "Shell Extension Source Files"

# PROP Default_Filter "cpp"
# Begin Source File

SOURCE=.\DragNDropFSM.cpp
# End Source File
# Begin Source File

SOURCE=.\LaunchPad.cpp
# End Source File
# Begin Source File

SOURCE=.\LaunchPadSettings.cpp
# End Source File
# Begin Source File

SOURCE=.\LaunchPadShellEx.cpp
# End Source File
# Begin Source File

SOURCE=.\RUNWITHVDMSDispatcher.cpp
# End Source File
# Begin Source File

SOURCE=.\RUNWITHVDMSThread.cpp
# End Source File
# End Group
# Begin Group "Utility Source Files"

# PROP Default_Filter "cpp"
# Begin Source File

SOURCE=.\ChangeIconDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ContextHelp.cpp
# End Source File
# Begin Source File

SOURCE=.\DeviceUtil.cpp
# End Source File
# Begin Source File

SOURCE=.\IconListBox.cpp
# End Source File
# Begin Source File

SOURCE=.\LaunchPadUtil.cpp
# End Source File
# Begin Source File

SOURCE=.\PIFFile.cpp
# End Source File
# End Group
# Begin Group "Wizard Source Files"

# PROP Default_Filter "cpp"
# Begin Source File

SOURCE=.\WizardPage_Custom_1.cpp
# End Source File
# Begin Source File

SOURCE=.\WizardPage_Custom_2.cpp
# End Source File
# Begin Source File

SOURCE=.\WizardPage_Finis.cpp
# End Source File
# Begin Source File

SOURCE=.\WizardPage_Intro.cpp
# End Source File
# Begin Source File

SOURCE=.\WizardSheet.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "Advanced Settings Header Files"

# PROP Default_Filter "h"
# Begin Source File

SOURCE=.\AdvSettingsPage_Adlib.h
# End Source File
# Begin Source File

SOURCE=.\AdvSettingsPage_Compat.h
# End Source File
# Begin Source File

SOURCE=.\AdvSettingsPage_Debug.h
# End Source File
# Begin Source File

SOURCE=.\AdvSettingsPage_Dosenv.h
# End Source File
# Begin Source File

SOURCE=.\AdvSettingsPage_Joy.h
# End Source File
# Begin Source File

SOURCE=.\AdvSettingsPage_Midi.h
# End Source File
# Begin Source File

SOURCE=.\AdvSettingsPage_Perf.h
# End Source File
# Begin Source File

SOURCE=.\AdvSettingsPage_Program.h
# End Source File
# Begin Source File

SOURCE=.\AdvSettingsPage_Sb.h
# End Source File
# Begin Source File

SOURCE=.\AdvSettingsSheet.h
# End Source File
# End Group
# Begin Group "Basic Settings Header Files"

# PROP Default_Filter "h"
# Begin Source File

SOURCE=.\BasicBrowseDlg.h
# End Source File
# Begin Source File

SOURCE=.\BasicSettingsPage.h
# End Source File
# End Group
# Begin Group "Shell Extension Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DragNDropFSM.h
# End Source File
# Begin Source File

SOURCE=.\LaunchPadSettings.h
# End Source File
# Begin Source File

SOURCE=.\LaunchPadShellEx.h
# End Source File
# Begin Source File

SOURCE=.\RUNWITHVDMSDispatcher.h
# End Source File
# Begin Source File

SOURCE=.\RUNWITHVDMSThread.h
# End Source File
# End Group
# Begin Group "Utility Header Files"

# PROP Default_Filter "h"
# Begin Source File

SOURCE=.\ChangeIconDlg.h
# End Source File
# Begin Source File

SOURCE=.\ContextHelp.h
# End Source File
# Begin Source File

SOURCE=.\DeviceUtil.h
# End Source File
# Begin Source File

SOURCE=.\IconListBox.h
# End Source File
# Begin Source File

SOURCE=.\LaunchPadUtil.h
# End Source File
# Begin Source File

SOURCE=.\PIF_FMT.H
# End Source File
# Begin Source File

SOURCE=.\PIFFile.h
# End Source File
# End Group
# Begin Group "Wizard Header Files"

# PROP Default_Filter "h"
# Begin Source File

SOURCE=.\WizardPage_Custom_1.h
# End Source File
# Begin Source File

SOURCE=.\WizardPage_Custom_2.h
# End Source File
# Begin Source File

SOURCE=.\WizardPage_Finis.h
# End Source File
# Begin Source File

SOURCE=.\WizardPage_Intro.h
# End Source File
# Begin Source File

SOURCE=.\WizardSheet.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Group "Images"

# PROP Default_Filter "ico;bmp"
# Begin Source File

SOURCE=.\Resources\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\Resources\icon2.ico
# End Source File
# Begin Source File

SOURCE=.\Resources\VDMSound.ico
# End Source File
# Begin Source File

SOURCE=.\Resources\wizard_back_16.bmp
# End Source File
# Begin Source File

SOURCE=.\Resources\wizard_back_256.bmp
# End Source File
# Begin Source File

SOURCE=.\Resources\wizard_head_16.bmp
# End Source File
# Begin Source File

SOURCE=.\Resources\wizard_head_256.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\LaunchPad.rc
# End Source File
# Begin Source File

SOURCE=.\LaunchPadShellEx.rgs
# End Source File
# Begin Source File

SOURCE=.\Resource.h

!IF  "$(CFG)" == "LaunchPad - Win32 Debug"

# Begin Custom Build
TargetName=LaunchPad
InputPath=.\Resource.h

"Help\$(TargetName).hm" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	makehm IDC_,HIDC_,0x0 Resource.h Help\$(TargetName).hm

# End Custom Build

!ELSEIF  "$(CFG)" == "LaunchPad - Win32 Unicode Debug"

# Begin Custom Build
TargetName=LaunchPad
InputPath=.\Resource.h

"Help\$(TargetName).hm" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	makehm IDC_,HIDC_,0x0 Resource.h Help\$(TargetName).hm

# End Custom Build

!ELSEIF  "$(CFG)" == "LaunchPad - Win32 Release MinSize"

# Begin Custom Build
TargetName=LaunchPad
InputPath=.\Resource.h

"Help\$(TargetName).hm" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	makehm IDC_,HIDC_,0x0 Resource.h Help\$(TargetName).hm

# End Custom Build

!ELSEIF  "$(CFG)" == "LaunchPad - Win32 Release MinDependency"

# Begin Custom Build
TargetName=LaunchPad
InputPath=.\Resource.h

"Help\$(TargetName).hm" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	makehm IDC_,HIDC_,0x0 Resource.h Help\$(TargetName).hm

# End Custom Build

!ELSEIF  "$(CFG)" == "LaunchPad - Win32 Unicode Release MinSize"

# Begin Custom Build
TargetName=LaunchPad
InputPath=.\Resource.h

"Help\$(TargetName).hm" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	makehm IDC_,HIDC_,0x0 Resource.h Help\$(TargetName).hm

# End Custom Build

!ELSEIF  "$(CFG)" == "LaunchPad - Win32 Unicode Release MinDependency"

# Begin Custom Build
TargetName=LaunchPad
InputPath=.\Resource.h

"Help\$(TargetName).hm" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	makehm IDC_,HIDC_,0x0 Resource.h Help\$(TargetName).hm

# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "Interfaces"

# PROP Default_Filter "idl;tlb"
# Begin Source File

SOURCE=.\LaunchPad.idl
# ADD MTL /tlb ".\LaunchPad.tlb" /h "LaunchPad.h" /iid "LaunchPad_i.c" /Oicf
# End Source File
# End Group
# Begin Group "HelpFiles"

# PROP Default_Filter "hpj;hm;rtf"
# Begin Source File

SOURCE=.\Help\LaunchPad.cnt
# End Source File
# Begin Source File

SOURCE=.\Help\LaunchPad.hpj

!IF  "$(CFG)" == "LaunchPad - Win32 Debug"

USERDEP__LAUNC="Help\LaunchPad.hm"	"Help\LaunchPad.rtf"	"Help\LaunchPad.cnt"	
# Begin Custom Build
OutDir=.\Debug
InputPath=.\Help\LaunchPad.hpj
InputName=LaunchPad

"$(OutDir)\$(InputName).hlp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	start /wait hcw /C /E /M "Help\$(InputName).hpj" 
	if errorlevel 1 goto :Error 
	if not exist "Help\$(InputName).hlp" goto :Error 
	copy "Help\$(InputName).hlp" $(OutDir) 
	goto :done 
	:Error 
	echo "Help\$(InputName).hpj(1)" : error: Problem encountered creating help file 
	type "Help\$(InputName).log" 
	:done 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "LaunchPad - Win32 Unicode Debug"

USERDEP__LAUNC="Help\LaunchPad.hm"	"Help\LaunchPad.rtf"	"Help\LaunchPad.cnt"	
# Begin Custom Build
OutDir=.\DebugU
InputPath=.\Help\LaunchPad.hpj
InputName=LaunchPad

"$(OutDir)\$(InputName).hlp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	start /wait hcw /C /E /M "Help\$(InputName).hpj" 
	if errorlevel 1 goto :Error 
	if not exist "Help\$(InputName).hlp" goto :Error 
	copy "Help\$(InputName).hlp" $(OutDir) 
	goto :done 
	:Error 
	echo "Help\$(InputName).hpj(1)" : error: Problem encountered creating help file 
	type "Help\$(InputName).log" 
	:done 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "LaunchPad - Win32 Release MinSize"

USERDEP__LAUNC="Help\LaunchPad.hm"	"Help\LaunchPad.rtf"	"Help\LaunchPad.cnt"	
# Begin Custom Build
OutDir=.\ReleaseMinSize
InputPath=.\Help\LaunchPad.hpj
InputName=LaunchPad

"$(OutDir)\$(InputName).hlp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	start /wait hcw /C /E /M "Help\$(InputName).hpj" 
	if errorlevel 1 goto :Error 
	if not exist "Help\$(InputName).hlp" goto :Error 
	copy "Help\$(InputName).hlp" $(OutDir) 
	goto :done 
	:Error 
	echo "Help\$(InputName).hpj(1)" : error: Problem encountered creating help file 
	type "Help\$(InputName).log" 
	:done 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "LaunchPad - Win32 Release MinDependency"

USERDEP__LAUNC="Help\LaunchPad.hm"	"Help\LaunchPad.rtf"	"Help\LaunchPad.cnt"	
# Begin Custom Build
OutDir=.\ReleaseMinDependency
InputPath=.\Help\LaunchPad.hpj
InputName=LaunchPad

"$(OutDir)\$(InputName).hlp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	start /wait hcw /C /E /M "Help\$(InputName).hpj" 
	if errorlevel 1 goto :Error 
	if not exist "Help\$(InputName).hlp" goto :Error 
	copy "Help\$(InputName).hlp" $(OutDir) 
	goto :done 
	:Error 
	echo "Help\$(InputName).hpj(1)" : error: Problem encountered creating help file 
	type "Help\$(InputName).log" 
	:done 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "LaunchPad - Win32 Unicode Release MinSize"

USERDEP__LAUNC="Help\LaunchPad.hm"	"Help\LaunchPad.rtf"	"Help\LaunchPad.cnt"	
# Begin Custom Build
OutDir=.\ReleaseUMinSize
InputPath=.\Help\LaunchPad.hpj
InputName=LaunchPad

"$(OutDir)\$(InputName).hlp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	start /wait hcw /C /E /M "Help\$(InputName).hpj" 
	if errorlevel 1 goto :Error 
	if not exist "Help\$(InputName).hlp" goto :Error 
	copy "Help\$(InputName).hlp" $(OutDir) 
	goto :done 
	:Error 
	echo "Help\$(InputName).hpj(1)" : error: Problem encountered creating help file 
	type "Help\$(InputName).log" 
	:done 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "LaunchPad - Win32 Unicode Release MinDependency"

USERDEP__LAUNC="Help\LaunchPad.hm"	"Help\LaunchPad.rtf"	"Help\LaunchPad.cnt"	
# Begin Custom Build
OutDir=.\ReleaseUMinDependency
InputPath=.\Help\LaunchPad.hpj
InputName=LaunchPad

"$(OutDir)\$(InputName).hlp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	start /wait hcw /C /E /M "Help\$(InputName).hpj" 
	if errorlevel 1 goto :Error 
	if not exist "Help\$(InputName).hlp" goto :Error 
	copy "Help\$(InputName).hlp" $(OutDir) 
	goto :done 
	:Error 
	echo "Help\$(InputName).hpj(1)" : error: Problem encountered creating help file 
	type "Help\$(InputName).log" 
	:done 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Help\LaunchPad.rtf
# End Source File
# End Group
# Begin Source File

SOURCE=.\LaunchPad.def
# End Source File
# End Target
# End Project
