######################################
### VDMSound NSIS Installer Script ###
########## By  Colin Snover ##########
######################################
##
#### todo: HKCU versus HKLM (esp. PATH var)==> for 2.1.1


SetCompressor bzip2 # must be before anything else

### ModernUI Setup

	!include "MUI.nsh" # ModernUI header

	## program information
	!define MUI_PRODUCT "VDMSound"
	!define MUI_VERSION "2.1.0"
	!define MUI_DESCRIPTION "VDMSound is a SoundBlaster/AdLib emulator for Windows NT's Virtual DOS Machine."
	!define MUI_AUTHOR "Vlad Romascanu"
	!define MUI_CONTACT "ntvdm@hotmail.com"
	!define MUI_INFO_URL "http://ntvdm.cjb.net/"
	!define MUI_HELP_URL "http://ntvdm.cjb.net/faq/"
	!define MUI_UPDATE_URL "http://ntvdm.cjb.net/download/"
	!define MUI_UNINSTALLER_FILENAME "uninst.exe"
	!define INSTALLFROM "C:\sbin\VDMSound"         # where the installer will compile files from
#	!define HAVE_RESHACK "C:\sbin\Resource Hacker" # where Resource Hacker is
 	!define HAVE_UPX "${NSISDIR}"                  # where the UPX installer packer is
	!define ALLUSERS                               # until HKCU vs HKLM is established
	!define UPGRADEDLL_NOREGISTER                  # some upgraded DLLs can't be registered

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;; TOUCH AFTER THIS LINE WITHOUT KNOWING WHAT YOU ARE DOING AND DIEEEEEE ;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	## pages to be displayed
	!define MUI_WELCOMEPAGE
	!define MUI_LICENSEPAGE
	!define MUI_CUSTOMFUNCTION_COMPONENTS_PRE checkPrevInstall
	!define MUI_COMPONENTSPAGE
		!define MUI_COMPONENTSPAGE_SMALLDESC
	!define MUI_DIRECTORYPAGE
	!define MUI_CUSTOMFUNCTION_INSTFILES_PRE checkAllUsers
	!define MUI_FINISHPAGE
		!define MUI_FINISHPAGE_SHOWREADME "$INSTDIR\Docs\readme.rtf"
		!define MUI_FINISHPAGE_NOAUTOCLOSE ## turn on for debugging
	!define MUI_ABORTWARNING
	!define MUI_UNINSTALLER
		!define MUI_UNCONFIRMPAGE

### ModernUI Settings

#### font definitions cause problems with multibyte operating systems
	## interface settings
	!define MUI_UI "${NSISDIR}\Contrib\UIs\modern.exe"
	!define MUI_UI_HEADERBITMAP "${NSISDIR}\Contrib\UIs\modern_headerbmp.exe"
	!define MUI_UI_SMALLDESCRIPTION "${NSISDIR}\Contrib\UIs\modern_smalldesc.exe"
	!define MUI_ICON "${NSISDIR}\Contrib\Icons\yi-simple2_vdmsinstall.ico"
	!define MUI_UNICON "${NSISDIR}\Contrib\Icons\yi-simple2_vdmsuninstall.ico"
	!define MUI_CHECKBITMAP "${NSISDIR}\Contrib\Icons\yi-simple2_check.bmp"
#	!define MUI_FONT "Tahoma"
	!define MUI_FONTSIZE "8"
#	!define MUI_FONT_HEADER "Tahoma"
	!define MUI_FONTSIZE_HEADER "11"
	!define MUI_FONTSTYLE_HEADER "700"
#	!define MUI_FONT_TITLE "Verdana"
	!define MUI_FONTSIZE_TITLE "12"
	!define MUI_FONTSTYLE_TITLE "700"
	!define MUI_INSTALLCOLORS /windows
	!define MUI_PROGRESSBAR "smooth"
	!define MUI_HEADERBITMAP "${NSISDIR}\Contrib\Icons\modern-header 2.bmp"
	!define MUI_SPECIALINI "${NSISDIR}\Contrib\Modern UI\ioSpecial.ini"
	!define MUI_SPECIALBITMAP "${NSISDIR}\Contrib\Icons\modern-wizard.bmp"

	## language includes
	!insertmacro MUI_LANGUAGE "English"
	!insertmacro MUI_LANGUAGE "French"
	!insertmacro MUI_LANGUAGE "German"
	!insertmacro MUI_LANGUAGE "Spanish"
	!insertmacro MUI_LANGUAGE "SimpChinese"
	!insertmacro MUI_LANGUAGE "TradChinese"
	!insertmacro MUI_LANGUAGE "Japanese"
	!insertmacro MUI_LANGUAGE "Korean"
	!insertmacro MUI_LANGUAGE "Italian"
	!insertmacro MUI_LANGUAGE "Dutch"
	!insertmacro MUI_LANGUAGE "Danish"
	!insertmacro MUI_LANGUAGE "Greek"
	!insertmacro MUI_LANGUAGE "Russian"
	!insertmacro MUI_LANGUAGE "PortugueseBR"
	!insertmacro MUI_LANGUAGE "Polish"
	!insertmacro MUI_LANGUAGE "Ukrainian"
	!insertmacro MUI_LANGUAGE "Czech"
#	!insertmacro MUI_LANGUAGE "Slovak"
#	!insertmacro MUI_LANGUAGE "Croatian"
	!insertmacro MUI_LANGUAGE "Bulgarian"
#	!insertmacro MUI_LANGUAGE "Hungarian"
	!insertmacro MUI_LANGUAGE "Thai"
	!insertmacro MUI_LANGUAGE "Romanian"
#	!insertmacro MUI_LANGUAGE "Macedonian"
	!insertmacro MUI_LANGUAGE "Turkish"

# 	!define MUI_LANGDLL_REGISTRY_ROOT "HKCU"
# 	!define MUI_LANGDLL_REGISTRY_KEY "Software\${MUI_PRODUCT}"
# 	!define MUI_LANGDLL_REGISTRY_VALUENAME "Installer Language"

	## reserve files (to make installer start faster; these files go first in the archive)
	!insertmacro MUI_RESERVEFILE_WELCOMEFINISHPAGE
	!insertmacro MUI_RESERVEFILE_LANGDLL
	!insertmacro MUI_RESERVEFILE_SPECIALINI
	!insertmacro MUI_RESERVEFILE_SPECIALBITMAP

### NSIS Settings

# 	!ifdef HAVE_RESHACK ## there is no multipass !packhdr
# 		!packhdr tmp.dat "${HAVE_RESHACK}\ResHacker.exe -addoverwrite tmp.dat, tmp.dat, vdmsound.res, versioninfo,1,"
# 	!endif
	!ifdef HAVE_UPX
		!packhdr tmp.dat "${HAVE_UPX}\upx -9 tmp.dat"
	!endif

	OutFile "${MUI_PRODUCT}${MUI_VERSION}.exe" # installer executable name
	XPStyle on                                 # XP style manifest, hey why the fuck not?
	SetOverwrite on

	InstallDir "$PROGRAMFILES\${MUI_PRODUCT}"
	InstallDirRegKey HKLM "SYSTEM\CurrentControlSet\Session Manager\Environment" "VDMSPath"

	ShowInstDetails show
	ShowUninstDetails show

	LicenseData /LANG=${LANG_ENGLISH} "${INSTALLFROM}\Docs\License\license.en.txt"
	LicenseData /LANG=${LANG_FRENCH} "${INSTALLFROM}\Docs\License\license.fr.txt"
	LicenseData /LANG=${LANG_GERMAN} "${INSTALLFROM}\Docs\License\license.de.txt"
	LicenseData /LANG=${LANG_SPANISH} "${INSTALLFROM}\Docs\License\license.es.txt"
	LicenseData /LANG=${LANG_SIMPCHINESE} "${INSTALLFROM}\Docs\License\license.cn.big5"
	LicenseData /LANG=${LANG_TRADCHINESE} "${INSTALLFROM}\Docs\License\license.cn.big5"
	LicenseData /LANG=${LANG_JAPANESE} "${INSTALLFROM}\Docs\License\license.jp.sjs"
	LicenseData /LANG=${LANG_KOREAN} "${INSTALLFROM}\Docs\License\license.ko.txt"
	LicenseData /LANG=${LANG_ITALIAN} "${INSTALLFROM}\Docs\License\license.it.txt"
	LicenseData /LANG=${LANG_DUTCH} "${INSTALLFROM}\Docs\License\license.nl.txt"
	LicenseData /LANG=${LANG_DANISH} "${INSTALLFROM}\Docs\License\license.da.txt"
	LicenseData /LANG=${LANG_GREEK} "${INSTALLFROM}\Docs\License\license.el.txt"
	LicenseData /LANG=${LANG_RUSSIAN} "${INSTALLFROM}\Docs\License\license.ru.txt"
	LicenseData /LANG=${LANG_PORTUGESEBR} "${INSTALLFROM}\Docs\License\license.pt_br.txt"
	LicenseData /LANG=${LANG_POLISH} "${INSTALLFROM}\Docs\License\license.pl.txt"
	LicenseData /LANG=${LANG_UKRANIAN} "${INSTALLFROM}\Docs\License\license.ua.txt"
	LicenseData /LANG=${LANG_CZECH} "${INSTALLFROM}\Docs\License\license.cz.txt"
	LicenseData /LANG=${LANG_BULGARIAN} "${INSTALLFROM}\Docs\License\license.bg.txt"
	LicenseData /LANG=${LANG_THAI} "${INSTALLFROM}\Docs\License\license.th.txt"
	LicenseData /LANG=${LANG_ROMANIAN} "${INSTALLFROM}\Docs\License\license.ro.txt"
	LicenseData /LANG=${LANG_TURKISH} "${INSTALLFROM}\Docs\License\license.tr.txt"

### Predefined Functions

### Functions by Joost (UpdateDLL)
 ; Macro - Upgrade DLL File
 ; Written by Joost Verburg
 ; ------------------------
 ;
 ; Example of usage:
 ; !insertmacro UpgradeDLL "dllname.dll" "$SYSDIR\dllname.dll"
 ;
 ; !define UPGRADEDLL_NOREGISTER if you want to upgrade a DLL which cannot
 ; be registered
 ;
 ; Note that this macro sets overwrite to ON (the default) when it has been
 ; inserted.
 ; If you are using another setting, set it again after inserting the macro.

 !macro UpgradeDLL LOCALFILE DESTFILE

   Push $R0
   Push $R1
   Push $R2
   Push $R3

   ;------------------------
   ;Check file and version

   IfFileExists "${DESTFILE}" "" "copy_${LOCALFILE}"

   ClearErrors
     GetDLLVersionLocal "${LOCALFILE}" $R0 $R1
     GetDLLVersion "${DESTFILE}" $R2 $R3
   IfErrors "upgrade_${LOCALFILE}"

   IntCmpU $R0 $R2 "" "done_${LOCALFILE}" "upgrade_${LOCALFILE}"
   IntCmpU $R1 $R3 "done_${LOCALFILE}" "done_${LOCALFILE}" \
   "upgrade_${LOCALFILE}"

   ;------------------------
   ;Let's upgrade the DLL!

   SetOverwrite try

   "upgrade_${LOCALFILE}:"
     !ifndef UPGRADEDLL_NOREGISTER
       ;Unregister the DLL
       UnRegDLL "${DESTFILE}"
     !endif

   ;------------------------
   ;Try to copy the DLL directly

   ClearErrors
     StrCpy $R0 "${DESTFILE}"
     Call ":file_${LOCALFILE}"
   IfErrors "" "noreboot_${LOCALFILE}"

   ;------------------------
   ;DLL is in use. Copy it to a temp file and Rename it on reboot.

   GetTempFileName $R0
     Call ":file_${LOCALFILE}"
   Rename /REBOOTOK $R0 "${DESTFILE}"

   ;------------------------
   ;Register the DLL on reboot

   !ifndef UPGRADEDLL_NOREGISTER
     WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\RunOnce" \
     "Register ${DESTFILE}" '"$SYSDIR\rundll32.exe" \
     "${DESTFILE},DllRegisterServer"'
   !endif

   Goto "done_${LOCALFILE}"

   ;------------------------
   ;DLL does not exist - just extract

   "copy_${LOCALFILE}:"
     StrCpy $R0 "${DESTFILE}"
     Call ":file_${LOCALFILE}"

   ;------------------------
   ;Register the DLL

   "noreboot_${LOCALFILE}:"
     !ifndef UPGRADEDLL_NOREGISTER
       RegDLL "${DESTFILE}"
     !endif

   ;------------------------
   ;Done

   "done_${LOCALFILE}:"

   Pop $R3
   Pop $R2
   Pop $R1
   Pop $R0

   ;------------------------
   ;End

   Goto "end_${LOCALFILE}"

   ;------------------------
   ;Called to extract the DLL

   "file_${LOCALFILE}:"
     File /oname=$R0 "${LOCALFILE}"
     Return

   "end_${LOCALFILE}:"

   ;------------------------
   ;Set overwrite to default
   ;(was set to TRY above)

   SetOverwrite on

 !macroend

### Functions by KiCHiK (PATHENV)
;====================================================
; AddToPath - Adds the given dir to the search path.
;        Input - head of the stack
;        Note - Win9x systems requires reboot
;====================================================
Function AddToPath
  Exch $0
  Push $1
  
  Call IsNT
  Pop $1
  StrCmp $1 1 AddToPath_NT
    ; Not on NT
    StrCpy $1 $WINDIR 2
    FileOpen $1 "$1\autoexec.bat" a
    FileSeek $1 0 END
    GetFullPathName /SHORT $0 $0
    FileWrite $1 "$\r$\nSET PATH=%PATH%;$0$\r$\n"
    FileClose $1
    Goto AddToPath_done

  AddToPath_NT:
		!ifdef ALLUSERS
			ReadRegStr $1 HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "PATH"
		!else
	    ReadRegStr $1 HKCU "Environment" "PATH"
		!endif
    StrCmp $1 "" AddToPath_NTdoIt
      StrCpy $0 "$1;$0"
      Goto AddToPath_NTdoIt
    AddToPath_NTdoIt:
		!ifdef ALLUSERS
      WriteRegStr HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "PATH" $0
		!else
      WriteRegStr HKCU "Environment" "PATH" $0
		!endif

  AddToPath_done:
    Pop $1
    Pop $0
FunctionEnd

;====================================================
; RemoveFromPath - Remove a given dir from the path
;     Input: head of the stack
;====================================================
Function un.RemoveFromPath
  Exch $0
  Push $1
  Push $2
  Push $3
  Push $4
  
  Call un.IsNT
  Pop $1
  StrCmp $1 1 unRemoveFromPath_NT
    ; Not on NT
    StrCpy $1 $WINDIR 2
    FileOpen $1 "$1\autoexec.bat" r
    GetTempFileName $4
    FileOpen $2 $4 w
    GetFullPathName /SHORT $0 $0
    StrCpy $0 "SET PATH=%PATH%;$0"
    SetRebootFlag true
    Goto unRemoveFromPath_dosLoop
    
    unRemoveFromPath_dosLoop:
      FileRead $1 $3
      StrCmp $3 "$0$\r$\n" unRemoveFromPath_dosLoop
      StrCmp $3 "$0$\n" unRemoveFromPath_dosLoop
      StrCmp $3 "$0" unRemoveFromPath_dosLoop
      StrCmp $3 "" unRemoveFromPath_dosLoopEnd
      FileWrite $2 $3
      Goto unRemoveFromPath_dosLoop
    
    unRemoveFromPath_dosLoopEnd:
      FileClose $2
      FileClose $1
      StrCpy $1 $WINDIR 2
      Delete "$1\autoexec.bat"
      CopyFiles /SILENT $4 "$1\autoexec.bat"
      Delete $4
      Goto unRemoveFromPath_done

  unRemoveFromPath_NT:
    StrLen $2 $0
		!ifdef ALLUSERS
			ReadRegStr $1 HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "PATH"
		!else
	    ReadRegStr $1 HKCU "Environment" "PATH"
		!endif
    Push $1
    Push $0
    Call un.StrStr ; Find $0 in $1
    Pop $0 ; pos of our dir
    IntCmp $0 -1 unRemoveFromPath_done
      ; else, it is in path
      StrCpy $3 $1 $0 ; $3 now has the part of the path before our dir
      IntOp $2 $2 + $0 ; $2 now contains the pos after our dir in the path (';')
      IntOp $2 $2 + 1 ; $2 now containts the pos after our dir and the semicolon.
      StrLen $0 $1
      StrCpy $1 $1 $0 $2
      StrCpy $3 "$3$1"

		!ifdef ALLUSERS
      WriteRegStr HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "PATH" $3
		!else
      WriteRegStr HKCU "Environment" "PATH" $3
		!endif
  
  unRemoveFromPath_done:
    Pop $4
    Pop $3
    Pop $2
    Pop $1
    Pop $0
FunctionEnd

;====================================================
; IsNT - Returns 1 if the current system is NT, 0
;        otherwise.
;     Output: head of the stack
;====================================================
Function IsNT
  Push $0
  ReadRegStr $0 HKLM "SOFTWARE\Microsoft\Windows NT\CurrentVersion" CurrentVersion
  StrCmp $0 "" 0 IsNT_yes
  ; we are not NT.
  Pop $0
  Push 0
  Return

  IsNT_yes:
    ; NT!!!
    Pop $0
    Push 1
FunctionEnd

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Uninstall sutff
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;====================================================
; StrStr - Finds a given string in another given string.
;               Returns -1 if not found and the pos if found.
;          Input: head of the stack - string to find
;                      second in the stack - string to find in
;          Output: head of the stack
;====================================================
Function un.StrStr
  Push $0
  Exch
  Pop $0 ; $0 now have the string to find
  Push $1
  Exch 2
  Pop $1 ; $1 now have the string to find in
  Exch
  Push $2
  Push $3
  Push $4
  Push $5

  StrCpy $2 -1
  StrLen $3 $0
  StrLen $4 $1
  IntOp $4 $4 - $3

  unStrStr_loop:
    IntOp $2 $2 + 1
    IntCmp $2 $4 0 0 unStrStrReturn_notFound
    StrCpy $5 $1 $3 $2
    StrCmp $5 $0 unStrStr_done unStrStr_loop

  unStrStrReturn_notFound:
    StrCpy $2 -1

  unStrStr_done:
    Pop $5
    Pop $4
    Pop $3
    Exch $2
    Exch 2
    Pop $0
    Pop $1
FunctionEnd

;====================================================
; IsNT - Returns 1 if the current system is NT, 0
;        otherwise.
;     Output: head of the stack
;====================================================
Function un.IsNT
  Push $0
  ReadRegStr $0 HKLM "SOFTWARE\Microsoft\Windows NT\CurrentVersion" CurrentVersion
  StrCmp $0 "" 0 unIsNT_yes
  ; we are not NT.
  Pop $0
  Push 0
  Return

  unIsNT_yes:
    ; NT!!!
    Pop $0
    Push 1
FunctionEnd

###############################################################################################
###############################################################################################
##### this is an important location because this is where stuff starts to actually happen #####
###############################################################################################
### Functions by Me ###########################################################################
###############################################################################################

	Function .onInit
		!insertmacro MUI_INSTALLOPTIONS_EXTRACT_AS "${NSISDIR}\Contrib\Modern UI\ioSpecial.ini" "ioSpecial.ini" # extract the special InstallOptions
		!insertmacro MUI_LANGDLL_DISPLAY # choose a language prior to installation

		ClearErrors
		UserInfo::GetName
		IfErrors Win9x
		Pop $0
		UserInfo::GetAccountType
		Pop $1
		StrCmp $1 "Admin" isAdmin isNotAdmin
		isNotAdmin:
			MessageBox MB_OK|MB_ICONSTOP "You must be running as an Administrator to install VDMSound."
			Quit
		Win9x:
			MessageBox MB_OK|MB_ICONSTOP "VDMSound is for Windows NT/2000/XP only!"
			Quit
		isAdmin:
			ClearErrors

		SetOutPath "$SYSDIR"
		ClearErrors
		!insertmacro UpgradeDLL "C:\WINNT\system32\atl.dll" "$SYSDIR\atl.dll"
		!insertmacro UpgradeDLL "C:\WINNT\system32\msvcrt.dll" "$SYSDIR\msvcrt.dll"
		!insertmacro UpgradeDLL "C:\WINNT\system32\msvcp60.dll" "$SYSDIR\msvcp60.dll"
		!insertmacro UpgradeDLL "C:\WINNT\system32\mfc42.dll" "$SYSDIR\mfc42.dll"
		ClearErrors
		SetOutPath "$INSTDIR"
	FunctionEnd

	Function un.onInit
		!insertmacro MUI_INSTALLOPTIONS_EXTRACT_AS "${NSISDIR}\Contrib\Modern UI\ioSpecial.ini" "ioSpecial.ini" # extract the special InstallOptions
		!insertmacro MUI_LANGDLL_DISPLAY
		MessageBox MB_YESNO|MB_ICONQUESTION "This will uninstall VDMSound. Continue?" IDYES NoAbort IDNO Abort
		Abort:
			Quit
		NoAbort:
			MessageBox MB_YESNO|MB_ICONQUESTION "Keep your VDMSound configuration file?" IDYES noRemFiles IDNO remFiles
			remFiles:
				Delete "$INSTDIR\VDMS.INI"
			noRemFiles:
	FunctionEnd

	Function checkAllUsers
#		MessageBox MB_YESNO|MB_ICONQUESTION "Install for ALL USERS?$\r$\n(Otherwise, ${MUI_PRODUCT} will only be installed for the current user.)" IDYES AllUserInstall IDNO ThisUserInstall
#			AllUserInstall:
#				!define ALLUSERS
#			ThisUserInstall:
	FunctionEnd

	Function checkPrevInstall
		ClearErrors
		ReadRegStr $1 HKCU "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{8ECBE643-8230-11D5-9D6B-00A024112F81}" "DisplayName"
			IfErrors check2 prevInstall
			check2:
				ClearErrors
				ReadRegStr $1 HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{8ECBE643-8230-11D5-9D6B-00A024112F81}" "DisplayName"
				IfErrors noPrevInstall prevInstall
			prevInstall:
				ClearErrors
				MessageBox MB_YESNO|MB_ICONEXCLAMATION "Previous version of VDMSound detected.$\r$\nIt is HIGHLY recommended that you uninstall the old version first.$\r$\nWould you like to automatically uninstall the previous version?" IDYES uninstall IDNO noUninstall
				uninstall:
					ExecWait "MsiExec.exe /X{8ECBE643-8230-11D5-9D6B-00A024112F81}"
					SetOutPath "$INSTDIR"
					UnRegDLL "$INSTDIR\Launchpad.dll"
					Delete "$INSTDIR\Launchpad.dll"
					UnRegDLL "$INSTDIR\EmuPPDAC.dll"
					Delete "$INSTDIR\EmuPPDAC.dll"
					Delete "$INSTDIR\joy1.map"
					Delete "$INSTDIR\joy2.map"
					Delete "$INSTDIR\joy3.map"
					Delete "$INSTDIR\VDMConfig.GID"
					ClearErrors
					IfFileExists "$INSTDIR\Launchpad.dll" launchpadNoDelete noPrevInstall
					launchpadNoDelete:
						SetRebootFlag true
						MessageBox MB_OKCANCEL "Previous version of VDMSound could not be completely uninstalled.$\r$\nPlease click 'OK' to reboot your system now, or 'cancel' to reboot later. You must reboot before installation can successfully be completed." IDOK rebootNow IDCANCEL rebootLater
						rebootNow:
							WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\RunOnce" "VDMSound Installer" "$CMDLINE"
							MessageBox MB_OK "Installation will be restarted upon reboot of computer."
							Reboot
						rebootLater:
							WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\RunOnce" "VDMSound Installer" "$CMDLINE"
							MessageBox MB_OK "Installation will be restarted upon reboot of computer."
							Quit
				noUninstall:
					ClearErrors
			noPrevInstall:
					ClearErrors
	FunctionEnd
### Installer

	## quick-install packages
	InstType "Typical"
	InstType "Complete"
#	InstType /COMPONENTSONLYONCUSTOM

	## vdmsound files

	SubSection /e "VDMSound" DESC_vdms
		Section "Emulation Core" DESC_core
			SectionIn 1 2 RO
			SetOutPath "$INSTDIR"
			File "${INSTALLFROM}\DOSDRV.EXE"
			File "${INSTALLFROM}\DMAController.dll"
			File "${INSTALLFROM}\DSoundDevice.dll"
			File "${INSTALLFROM}\VDDLoader.dll"
			File "${INSTALLFROM}\VDMConfig.dll"
			File "${INSTALLFROM}\WaveDevice.dll"
			File "${INSTALLFROM}\EmuSBCompat.dll"
			File "${INSTALLFROM}\EmuAdlib.dll"
			File "${INSTALLFROM}\EmuMPU401.dll"
			File "${INSTALLFROM}\MIDIDevice.dll"
			File "${INSTALLFROM}\MIDIIndicator.dll"
			File "${INSTALLFROM}\MIDIToolkit.dll"
			File "${INSTALLFROM}\identity.map"
			File "${INSTALLFROM}\mt2gm.map"
			File "${INSTALLFROM}\gm2mt.map"
			File "${INSTALLFROM}\EmuJoystick.dll"
			File "${INSTALLFROM}\joy1.map"
			File "${INSTALLFROM}\joy2.map"
			File "${INSTALLFROM}\joy3.map"
			File "${INSTALLFROM}\DiskWriter.dll"
			File "${INSTALLFROM}\VDMSound.ico"
			SetOverwrite off
			File "${INSTALLFROM}\VDMS.INI"
			SetOverwrite on

			RegDLL "$INSTDIR\DMAController.dll"
			RegDLL "$INSTDIR\DSoundDevice.dll"
			RegDLL "$INSTDIR\VDDLoader.dll"
			RegDLL "$INSTDIR\VDMConfig.dll"
			RegDLL "$INSTDIR\WaveDevice.dll"
			RegDLL "$INSTDIR\EmuSBCompat.dll"
			RegDLL "$INSTDIR\EmuAdlib.dll"
			RegDLL "$INSTDIR\EmuMPU401.dll"
			RegDLL "$INSTDIR\MIDIDevice.dll"
			RegDLL "$INSTDIR\MIDIIndicator.dll"
			RegDLL "$INSTDIR\MIDIToolkit.dll"
			RegDLL "$INSTDIR\EmuJoystick.dll"
			RegDLL "$INSTDIR\DiskWriter.dll"

			SetOutPath "$INSTDIR\Docs"
			File "${INSTALLFROM}\Docs\readme.rtf"
			File "${INSTALLFROM}\Docs\CHANGES.txt"
			SetOutPath "$INSTDIR\Docs\License"
			File "${INSTALLFROM}\Docs\License\license.en.txt"
			File "${INSTALLFROM}\Docs\License\license.en.txt"
			File "${INSTALLFROM}\Docs\License\license.fr.txt"
			File "${INSTALLFROM}\Docs\License\license.de.txt"
			File "${INSTALLFROM}\Docs\License\license.es.txt"
			File "${INSTALLFROM}\Docs\License\license.cn.big5"
			File "${INSTALLFROM}\Docs\License\license.jp.sjs"
			File "${INSTALLFROM}\Docs\License\license.it.txt"
			File "${INSTALLFROM}\Docs\License\license.nl.txt"
			File "${INSTALLFROM}\Docs\License\license.da.txt"
			File "${INSTALLFROM}\Docs\License\license.el.txt"
			File "${INSTALLFROM}\Docs\License\license.ru.txt"
			File "${INSTALLFROM}\Docs\License\license.pt_br.txt"
			File "${INSTALLFROM}\Docs\License\license.pl.txt"
			File "${INSTALLFROM}\Docs\License\license.ua.txt"
			File "${INSTALLFROM}\Docs\License\license.cz.txt"
			File "${INSTALLFROM}\Docs\License\license.bg.txt"
			File "${INSTALLFROM}\Docs\License\license.th.txt"
			File "${INSTALLFROM}\Docs\License\license.ro.txt"
			File "${INSTALLFROM}\Docs\License\license.tr.txt"
		SectionEnd

		Section "Shell Integration" DESC_lpad
			SectionIn 1 2
			SetOutPath "$INSTDIR"
			File "${INSTALLFROM}\LaunchPad.dll"
			RegDLL "$INSTDIR\LaunchPad.dll"
		SectionEnd

		Section "Help Files" DESC_help
			SectionIn 1 2
			SetOutPath "$INSTDIR"
			File "${INSTALLFROM}\VDMConfig.cnt"
			File "${INSTALLFROM}\VDMConfig.hlp"
		SectionEnd

		SubSection "Optional Modules" DESC_optm
			Section "Parallel-Port DAC Emulation" DESC_pdac
				SectionIn 2
				SetOutPath "$INSTDIR"
				File "${INSTALLFROM}\EmuPPDAC.dll"
				RegDLL "$INSTDIR\EmuPPDAC.dll"
			SectionEnd
		SubSectionEnd

	SubSectionEnd

	SubSection "Utilities" DESC_util

		## mouse2kv files
		SubSection "Mouse2KV" DESC_m2kv
			Section "Program" DESC_m2kp
				SectionIn 2
				SetOutPath "$INSTDIR"
				File "${INSTALLFROM}\MOUSE2KV.EXE"
				SetOutPath "$INSTDIR\Docs"
				File "${INSTALLFROM}\Docs\mouse2kv.rtf"
			SectionEnd
			Section "Source" DESC_m2ks
				SetOutPath "$INSTDIR\Source"
				File "${INSTALLFROM}\Source\MOUSE2KV.C"
			SectionEnd
		SubSectionEnd

		## nolfb files
		SubSection "NOLFB" DESC_nlfb
			Section "Program" DESC_nlfp
				SectionIn 2
				SetOutPath "$INSTDIR"
				File "${INSTALLFROM}\NOLFB.COM"
			SectionEnd
			Section "Source" DESC_nlfs
				SetOutPath "$INSTDIR\Source"
				File "${INSTALLFROM}\Source\NOLFB.ASM"
			SectionEnd
		SubSectionEnd

		## speedset files
		SubSection "SPEEDSET" DESC_spdd
			Section "Program" DESC_spdp
				SectionIn 2
				SetOutPath "$INSTDIR"
				File "${INSTALLFROM}\SPEEDSET.EXE"
				File "${INSTALLFROM}\SPEEDSET.DLL"
				SetOutPath "$INSTDIR\Docs"
				File "${INSTALLFROM}\Docs\speedset.rtf"
			SectionEnd
	#		Section "Source" DESC_spds
	#			SetOutPath "$INSTDIR\Source"
	#			File "${INSTALLFROM}\Source\speedset.asm"
	#		SectionEnd
		SubSectionEnd

		## sapucdex files
		SubSection "SAPUCDEX" DESC_sapu ## needs OK from SaPu
			Section "Program" DESC_sapp
				SectionIn 2
				SetOutPath "$INSTDIR"
				File "${INSTALLFROM}\SAPUCDEX.EXE"
				File "${INSTALLFROM}\SAPUCDEX.DLL"
				SetOutPath "$INSTDIR\Docs"
				File "${INSTALLFROM}\Docs\sapucdex.rtf"
			SectionEnd
			Section "Source" DESC_saps
				SetOutPath "$INSTDIR\Source\SAPUCDEX"
				File "${INSTALLFROM}\Source\SAPUCDEX\_build.bat"
				File "${INSTALLFROM}\Source\SAPUCDEX\DOS.INC"
				File "${INSTALLFROM}\Source\SAPUCDEX\iso_fs.h"
				File "${INSTALLFROM}\Source\SAPUCDEX\isvbop.inc"
				File "${INSTALLFROM}\Source\SAPUCDEX\mscdex.h"
				File "${INSTALLFROM}\Source\SAPUCDEX\nt_vdd.h"
				File "${INSTALLFROM}\Source\SAPUCDEX\Ntddcdrm.h"
				File "${INSTALLFROM}\Source\SAPUCDEX\Ntddscsi.h"
				File "${INSTALLFROM}\Source\SAPUCDEX\Ntddstor.h"
				File "${INSTALLFROM}\Source\SAPUCDEX\ntdll.lib"
				File "${INSTALLFROM}\Source\SAPUCDEX\ntvdm.lib"
				File "${INSTALLFROM}\Source\SAPUCDEX\resource.h"
				File "${INSTALLFROM}\Source\SAPUCDEX\sapucdex.asm"
				File "${INSTALLFROM}\Source\SAPUCDEX\sapucdex.c"
				File "${INSTALLFROM}\Source\SAPUCDEX\sapucdex.def"
				File "${INSTALLFROM}\Source\SAPUCDEX\sapucdex.dsp"
				File "${INSTALLFROM}\Source\SAPUCDEX\sapucdex.rc"
				File "${INSTALLFROM}\Source\SAPUCDEX\Scsi.h"
				File "${INSTALLFROM}\Source\SAPUCDEX\vddsvc.h"
				File "${INSTALLFROM}\Source\SAPUCDEX\Winioctl.h"
			SectionEnd
		SubSectionEnd
	SubSectionEnd

	## this section must be installed
	Section ""
		SetOutPath "$INSTDIR"

		ClearErrors

		!ifdef ALLUSERS
			WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${MUI_PRODUCT}" "DisplayName" "${MUI_PRODUCT}"
			WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${MUI_PRODUCT}" "DisplayIcon" "$INSTDIR\${MUI_PRODUCT}.ico"
			WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${MUI_PRODUCT}" "DisplayVersion" "${MUI_VERSION}"
			WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${MUI_PRODUCT}" "Comments" "${MUI_DESCRIPTION}"
			WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${MUI_PRODUCT}" "Publisher" "${MUI_AUTHOR}"
			WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${MUI_PRODUCT}" "Contact" "${MUI_CONTACT}"
			WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${MUI_PRODUCT}" "URLInfoAbout" "${MUI_INFO_URL}"
			WriteRegExpandStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${MUI_PRODUCT}" "HelpLink" "${MUI_HELP_URL}"
			WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${MUI_PRODUCT}" "URLUpdateInfo" "${MUI_UPDATE_URL}"
			WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${MUI_PRODUCT}" "UninstallString" "$INSTDIR\${MUI_UNINSTALLER_FILENAME}"

			WriteRegExpandStr HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "VDMSPath" "$INSTDIR"
		!else
			WriteRegStr HKCU "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${MUI_PRODUCT}" "DisplayName" "${MUI_PRODUCT}"
			WriteRegStr HKCU "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${MUI_PRODUCT}" "DisplayIcon" "$INSTDIR\${MUI_PRODUCT}.ico"
			WriteRegStr HKCU "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${MUI_PRODUCT}" "DisplayVersion" "${MUI_VERSION}"
			WriteRegStr HKCU "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${MUI_PRODUCT}" "Comments" "${MUI_DESCRIPTION}"
			WriteRegStr HKCU "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${MUI_PRODUCT}" "Publisher" "${MUI_AUTHOR}"
			WriteRegStr HKCU "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${MUI_PRODUCT}" "Contact" "${MUI_CONTACT}"
			WriteRegStr HKCU "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${MUI_PRODUCT}" "URLInfoAbout" "${MUI_INFO_URL}"
			WriteRegExpandStr HKCU "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${MUI_PRODUCT}" "HelpLink" "${MUI_HELP_URL}"
			WriteRegStr HKCU "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${MUI_PRODUCT}" "URLUpdateInfo" "${MUI_UPDATE_URL}"
			WriteRegStr HKCU "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${MUI_PRODUCT}" "UninstallString" "$INSTDIR\${MUI_UNINSTALLER_FILENAME}"

			WriteRegExpandStr HKCU "Environment" "VDMSPath" "$INSTDIR"
		!endif

		Push $INSTDIR
		Call AddToPath

		SendMessage 65535 26 0 "STR:Environment" /TIMEOUT=2500

		WriteUninstaller "${MUI_UNINSTALLER_FILENAME}"
	SectionEnd

	## end of section

### Uninstaller

	## uninstall ev-ry-thing
	Section "Uninstall"
		SetOutPath "$INSTDIR"
		ClearErrors

		Delete "$INSTDIR\DOSDRV.EXE"
		UnRegDLL "$INSTDIR\DMAController.dll"
		Delete "$INSTDIR\DMAController.dll"
		UnRegDLL "$INSTDIR\DSoundDevice.dll"
		Delete "$INSTDIR\DSoundDevice.dll"
		UnRegDLL "$INSTDIR\VDDLoader.dll"
		Delete "$INSTDIR\VDDLoader.dll"
		UnRegDLL "$INSTDIR\VDMConfig.dll"
		Delete "$INSTDIR\VDMConfig.dll"
		UnRegDLL "$INSTDIR\WaveDevice.dll"
		Delete "$INSTDIR\WaveDevice.dll"

		RMDir /r "$INSTDIR\Docs\"
		RMDir /r "$INSTDIR\Source\"

		Delete "$INSTDIR\VDMSound.ico"

		Delete "$INSTDIR\VDMConfig.cnt"
		Delete "$INSTDIR\VDMConfig.hlp"
		Delete "$INSTDIR\VDMConfig.GID"

		UnRegDLL "$INSTDIR\EmuSBCompat.dll"
		Delete "$INSTDIR\EmuSBCompat.dll"

		UnRegDLL "EmuAdlib.dll"
		Delete "$INSTDIR\EmuAdlib.dll"

		UnRegDLL "$INSTDIR\EmuMPU401.dll"
		Delete "$INSTDIR\EmuMPU401.dll"
		UnRegDLL "$INSTDIR\MIDIDevice.dll"
		Delete "$INSTDIR\MIDIDevice.dll"
		UnRegDLL "$INSTDIR\MIDIIndicator.dll"
		Delete "$INSTDIR\MIDIIndicator.dll"
		UnRegDLL "$INSTDIR\MIDIToolkit.dll"
		Delete "$INSTDIR\MIDIToolkit.dll"
		Delete "$INSTDIR\identity.map"
			Delete "$INSTDIR\mt2gm.map"
			Delete "$INSTDIR\gm2mt.map"

		UnRegDLL "$INSTDIR\EmuPPDAC.dll"
		Delete "$INSTDIR\EmuPPDAC.dll"

		UnRegDLL "$INSTDIR\EmuJoystick.dll"
		Delete "$INSTDIR\EmuJoystick.dll"
			Delete "$INSTDIR\joy1.map"
			Delete "$INSTDIR\joy2.map"
			Delete "$INSTDIR\joy3.map"

		UnRegDLL "$INSTDIR\DiskWriter.dll"
		Delete "$INSTDIR\DiskWriter.dll"

		## launchpad files
		UnRegDLL "$INSTDIR\Launchpad.dll"
		Delete /REBOOTOK "$INSTDIR\Launchpad.dll"

		## nolfb files
		Delete "$INSTDIR\NOLFB.COM"

		## mouse2kv files
		Delete "$INSTDIR\MOUSE2KV.EXE"

		## speedset files
		Delete "$INSTDIR\SPEEDSET.EXE"
		Delete "$INSTDIR\SPEEDSET.DLL"

		## sapucdex files
		Delete "$INSTDIR\SAPUCDEX.EXE"
		Delete "$INSTDIR\SAPUCDEX.DLL"

		IfErrors rebootMaybeRequired
			rebootMaybeRequired:
				MessageBox MB_OK "You may need to reboot your computer to remove files that could not be deleted during uninstallation. $\r$\n$0 $\r$\n$1 $\r$\n$2 $\r$\n$3 $\r$\n$4 $\r$\n$5 $\r$\n$6 $\r$\n$7 $\r$\n$8 $\r$\n$9"

		Delete "$INSTDIR\${MUI_UNINSTALLER_FILENAME}"

		ClearErrors
		ReadRegStr $1 HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${MUI_PRODUCT}" "DisplayName"
		IfErrors installedInHKCU installedInHKLM
			installedInHKLM:
				DeleteRegKey HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${MUI_PRODUCT}"
				DeleteRegValue HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "VDMSPath"
				Goto "continue"
			installedInHKCU:
				DeleteRegKey HKCU "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${MUI_PRODUCT}"
				DeleteRegValue HKCU "Environment" "VDMSPath"
			continue:

#		DeleteRegValue HKCU "Software\${MUI_PRODUCT}" "Installer Language" ## not used

		Push $INSTDIR
		Call un.RemoveFromPath

		SendMessage 65535 26 0 "STR:Environment" /TIMEOUT=2500

		## end of section
	SectionEnd

### Section Descriptions

	!insertmacro MUI_FUNCTIONS_DESCRIPTION_BEGIN
		!insertmacro MUI_DESCRIPTION_TEXT ${DESC_vdms} "The VDMSound program itself."
		 !insertmacro MUI_DESCRIPTION_TEXT ${DESC_core} "The VDMSound engine and required plugins."
		 !insertmacro MUI_DESCRIPTION_TEXT ${DESC_pdac} "Parallel port Digital-to-Analogue Converter plugin (Disney Sound Source)."
		!insertmacro MUI_DESCRIPTION_TEXT ${DESC_lpad} "Windows GUI for VDMSound."
		!insertmacro MUI_DESCRIPTION_TEXT ${DESC_nlfb} "Disables Linear Framebuffer in DOS VESA games. By Ken Silverman."
		 !insertmacro MUI_DESCRIPTION_TEXT ${DESC_nlfp} "Executable for NOLFB."
		 !insertmacro MUI_DESCRIPTION_TEXT ${DESC_nlfs} "Source code for NOLFB (written in assembly)."
		!insertmacro MUI_DESCRIPTION_TEXT ${DESC_m2kv} "Fixes mouse coordinate constriction problems. By Peter."
		 !insertmacro MUI_DESCRIPTION_TEXT ${DESC_m2kp} "Executable for Mouse2KV."
		 !insertmacro MUI_DESCRIPTION_TEXT ${DESC_m2ks} "Source code for Mouse2KV (written in C)."
		!insertmacro MUI_DESCRIPTION_TEXT ${DESC_spdd} "Fixes sound and game timing problems. By Vlad Romascanu."
		 !insertmacro MUI_DESCRIPTION_TEXT ${DESC_spdp} "Executable for SPEEDSET."
#		 !insertmacro MUI_DESCRIPTION_TEXT ${DESC_spds} "Source code for SPEEDSET (written in assembly)."
		!insertmacro MUI_DESCRIPTION_TEXT ${DESC_sapu} "Restores full MSCDEX functionality (MSCDEXNT replacement). By Sauro Puccini."
		 !insertmacro MUI_DESCRIPTION_TEXT ${DESC_sapp} "Executable for SAPUCDEX."
		 !insertmacro MUI_DESCRIPTION_TEXT ${DESC_saps} "Source code for SAPUCDEX (written in C/C++/assembly)."
	!insertmacro MUI_FUNCTIONS_DESCRIPTION_END

### <<EOF