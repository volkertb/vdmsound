# Microsoft Developer Studio Project File - Name="Interfaces" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Generic Project" 0x010a

CFG=Interfaces - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Interfaces.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Interfaces.mak" CFG="Interfaces - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Interfaces - Win32 Release" (based on "Win32 (x86) Generic Project")
!MESSAGE "Interfaces - Win32 Debug" (based on "Win32 (x86) Generic Project")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/VDMSModules/Sources/Interfaces", HHAAAAAA"
# PROP Scc_LocalPath "."
MTL=midl.exe

!IF  "$(CFG)" == "Interfaces - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "."
# PROP Intermediate_Dir "."
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "Interfaces - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "."
# PROP Intermediate_Dir "."
# PROP Target_Dir ""

!ENDIF 

# Begin Target

# Name "Interfaces - Win32 Release"
# Name "Interfaces - Win32 Debug"
# Begin Source File

SOURCE=.\IAddressable.idl
# End Source File
# Begin Source File

SOURCE=.\IDMAC.idl
# End Source File
# Begin Source File

SOURCE=.\IDMAHandlers.idl
# End Source File
# Begin Source File

SOURCE=.\IMIDI.idl
# End Source File
# Begin Source File

SOURCE=.\IWave.idl
# End Source File
# End Target
# End Project
