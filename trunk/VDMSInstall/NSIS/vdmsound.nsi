######################################
### VDMSound NSIS Installer Script ###
########## By  Colin Snover ##########
######################################
##
#### todo: HKCU versus HKLM (esp. PATH var)


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
	!define HAVE_RESHACK "C:\sbin\Resource Hacker" # where Resource Hacker is
 	!define HAVE_UPX "${NSISDIR}"                  # where the UPX installer packer is

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;; TOUCH AFTER THIS LINE WITHOUT KNOWING WHAT YOU ARE DOING AND DIEEEEEE ;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	## pages to be displayed
	!define MUI_WELCOMEPAGE
	!define MUI_CUSTOMFUNCTION_LICENSE_PRE checkPrevInstall
	!define MUI_LICENSEPAGE
	!define MUI_DIRECTORYPAGE
	!define MUI_COMPONENTSPAGE
		!define MUI_COMPONENTSPAGE_SMALLDESC
	!define MUI_CUSTOMFUNCTION_INSTFILES_PRE checkAllUsers
	!define MUI_FINISHPAGE
		!define MUI_FINISHPAGE_SHOWREADME "$INSTDIR\Docs\readme.rtf"
		!define MUI_FINISHPAGE_NOAUTOCLOSE # for debugging
	!define MUI_ABORTWARNING
	!define MUI_UNINSTALLER
		!define MUI_UNCONFIRMPAGE

## ALL BROKEN
#  	!insertmacro MUI_UNPAGECOMMAND_CONFIRM
#  	!insertmacro MUI_UNPAGECOMMAND_INSTFILES
# 	!insertmacro MUI_UNFINISHHEADER
# 	!insertmacro MUI_UNPAGECOMMAND_COMPONENTS ## does not exist "yet"

### ModernUI Settings

	## interface settings
	!define MUI_UI "${NSISDIR}\Contrib\UIs\modern.exe"
	!define MUI_UI_HEADERBITMAP "${NSISDIR}\Contrib\UIs\modern_headerbmp.exe"
	!define MUI_UI_SMALLDESCRIPTION "${NSISDIR}\Contrib\UIs\modern_smalldesc.exe"
	!define MUI_ICON "${NSISDIR}\Contrib\Icons\yi-simple2_install.ico"
	!define MUI_UNICON "${NSISDIR}\Contrib\Icons\yi-simple2_uninstall.ico"
	!define MUI_CHECKBITMAP "${NSISDIR}\Contrib\Icons\yi-simple2_check.bmp"
	!define MUI_FONT "Tahoma"
	!define MUI_FONTSIZE "8"
	!define MUI_FONT_HEADER "Tahoma"
	!define MUI_FONTSIZE_HEADER "11"
	!define MUI_FONTSTYLE_HEADER "700"
	!define MUI_FONT_TITLE "Verdana"
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
#	!insertmacro MUI_LANGUAGE "SimpChinese"
	!insertmacro MUI_LANGUAGE "TradChinese"
	!insertmacro MUI_LANGUAGE "Japanese"
# 	!insertmacro MUI_LANGUAGE "Korean" ## error in this MUI_LANGUAGE
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
# 	!insertmacro MUI_LANGUAGE "Bulgarian" ## error in this MUI_LANGUAGE
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
# 	!insertmacro MUI_RESERVEFILE_INSTALLOPTION ## doesn't exist
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

	OutFile "${MUI_PRODUCT}${MUI_VERSION}.exe"     # installer executable name
	XPStyle on                                     # XP style manifest, hey why the fuck not?

	InstallDir "$PROGRAMFILES\${MUI_PRODUCT}"
	InstallDirRegKey HKLM "SYSTEM\CurrentControlSet\Session Manager\Environment" "VDMSPath"

	ShowInstDetails show
	ShowUninstDetails show

	LicenseData /LANG=${LANG_ENGLISH} "${NSISDIR}\license.en.txt"
	LicenseData /LANG=${LANG_FRENCH} "${NSISDIR}\license.fr.txt"
	LicenseData /LANG=${LANG_GERMAN} "${NSISDIR}\license.de.txt"
	LicenseData /LANG=${LANG_SPANISH} "${NSISDIR}\license.es.txt"
	LicenseData /LANG=${LANG_TRADCHINESE} "${NSISDIR}\license.cn.big5"
	LicenseData /LANG=${LANG_JAPANESE} "${NSISDIR}\license.jp.sjs"
	LicenseData /LANG=${LANG_ITALIAN} "${NSISDIR}\license.it.txt"
	LicenseData /LANG=${LANG_DUTCH} "${NSISDIR}\license.nl.txt"
	LicenseData /LANG=${LANG_DANISH} "${NSISDIR}\license.da.txt"
	LicenseData /LANG=${LANG_GREEK} "${NSISDIR}\license.el.txt"
	LicenseData /LANG=${LANG_RUSSIAN} "${NSISDIR}\license.ru.txt"
	LicenseData /LANG=${LANG_PORTUGESEBR} "${NSISDIR}\license.pt_br.txt"
	LicenseData /LANG=${LANG_POLISH} "${NSISDIR}\license.pl.txt"
	LicenseData /LANG=${LANG_UKRANIAN} "${NSISDIR}\license.ua.txt"
	LicenseData /LANG=${LANG_CZECH} "${NSISDIR}\license.cz.txt"
	LicenseData /LANG=${LANG_THAI} "${NSISDIR}\license.th.txt"
	LicenseData /LANG=${LANG_ROMANIAN} "${NSISDIR}\license.ro.txt"
	LicenseData /LANG=${LANG_TURKISH} "${NSISDIR}\license.tr.txt"

### Predefined Functions

	Function .onInit
		!insertmacro MUI_INSTALLOPTIONS_EXTRACT_AS "${NSISDIR}\Contrib\Modern UI\ioSpecial.ini" "ioSpecial.ini" # extract the special InstallOptions
		!insertmacro MUI_LANGDLL_DISPLAY # choose a language prior to installation
	FunctionEnd

	Function un.onInit
		!insertmacro MUI_INSTALLOPTIONS_EXTRACT_AS "${NSISDIR}\Contrib\Modern UI\ioSpecial.ini" "ioSpecial.ini" # extract the special InstallOptions
		!insertmacro MUI_LANGDLL_DISPLAY
		MessageBox MB_YESNO|MB_ICONQUESTION "This will uninstall VDMSound. Continue?" IDYES NoAbort IDNO Abort
		Abort:
			Quit
		NoAbort:
			MessageBox MB_YESNO|MB_ICONQUESTION "Keep your VDMS.INI, AUTOEXEC.VDMS and CONFIG.VDMS files?$\r$\nNote: this doesn't work." IDYES remFiles IDNO noRemFiles
			remFiles:
				!define REMFILES
			noRemFiles:
	FunctionEnd

	Function checkAllUsers
#		MessageBox MB_YESNO|MB_ICONQUESTION "Install for ALL USERS?$\r$\n(Otherwise, ${MUI_PRODUCT} will only be installed for the current user.)" IDYES AllUserInstall IDNO ThisUserInstall
#			AllUserInstall:
				!define ALLUSERS
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
					UnRegDLL "Launchpad.dll"
					Delete "$INSTDIR\Launchpad.dll"
					UnRegDLL "EmuPPDAC.dll"
					Delete "$INSTDIR\EmuPPDAC.dll"
					Delete "$INSTDIR\joy1.map"
					Delete "$INSTDIR\joy2.map"
					Delete "$INSTDIR\joy3.map"
					Delete "$INSTDIR\VDMConfig.GID"
					ClearErrors
					IfFileExists "$INSTDIR\Launchpad.dll" launchpadNoDelete
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

### Functions by KiCHiK
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

### Installer

	## quick-install packages
	InstType "Complete"
	InstType "Just VDMSound"
	InstType /COMPONENTSONLYONCUSTOM

	## vdmsound files

	SubSection "VDMSound" DESC_vdms
		Section "VDMSound Core" DESC_core
			SectionIn 1 2 RO
			SetOutPath "$INSTDIR"
			File "${INSTALLFROM}\DOSDRV.EXE"
			File "${INSTALLFROM}\DMAController.dll"
			File "${INSTALLFROM}\DSoundDevice.dll"
			File "${INSTALLFROM}\VDDLoader.dll"
			File "${INSTALLFROM}\VDMConfig.dll"
			File "${INSTALLFROM}\WaveDevice.dll"

			RegDLL "DMAController.dll"
			RegDLL "DSoundDevice.dll"
			RegDLL "VDDLoader.dll"
			RegDLL "VDMConfig.dll"
			RegDLL "WaveDevice.dll"

			SetOutPath "$INSTDIR\Docs"
			File "${INSTALLFROM}\Docs\readme.rtf"
			File "${INSTALLFROM}\Docs\CHANGES.txt"
			SetOutPath "$INSTDIR\Docs\License"
			File "${NSISDIR}\license.en.txt"
			File "${NSISDIR}\license.en.txt"
			File "${NSISDIR}\license.fr.txt"
			File "${NSISDIR}\license.de.txt"
			File "${NSISDIR}\license.es.txt"
			File "${NSISDIR}\license.cn.big5"
			File "${NSISDIR}\license.jp.sjs"
			File "${NSISDIR}\license.it.txt"
			File "${NSISDIR}\license.nl.txt"
			File "${NSISDIR}\license.da.txt"
			File "${NSISDIR}\license.el.txt"
			File "${NSISDIR}\license.ru.txt"
			File "${NSISDIR}\license.pt_br.txt"
			File "${NSISDIR}\license.pl.txt"
			File "${NSISDIR}\license.ua.txt"
			File "${NSISDIR}\license.cz.txt"
			File "${NSISDIR}\license.th.txt"
			File "${NSISDIR}\license.ro.txt"
			File "${NSISDIR}\license.tr.txt"

			SetOutPath "$INSTDIR"
			File "${INSTALLFROM}\VDMSound.ico"
# 			File "${INSTALLFROM}\VDMSRun.pif" ## vlad doesn't want these?
# 			File "${INSTALLFROM}\VDMSRun.bat" ## vlad doesn't want these?

			File "${INSTALLFROM}\VDMS.INI"
			File "${INSTALLFROM}\AUTOEXEC.VDMS"
			File "${INSTALLFROM}\CONFIG.VDMS"
		SectionEnd
		Section "VDMSound Help" DESC_help
			SectionIn 1 2
			File "${INSTALLFROM}\VDMConfig.cnt"
			File "${INSTALLFROM}\VDMConfig.hlp"
		SectionEnd
		Section "SoundBlaster" DESC_sndb
			SectionIn 1 2
			File "${INSTALLFROM}\EmuSBCompat.dll"
			RegDLL "EmuSBCompat.dll"
		SectionEnd
		Section "AdLib" DESC_adlb
			SectionIn 1 2
			File "${INSTALLFROM}\EmuAdlib.dll"
			RegDLL "EmuAdlib.dll"
		SectionEnd
		SubSection "MPU-401" DESC_m401
			Section "MPU-401 Core" DESC_mcor
				SectionIn 1 2
					File "${INSTALLFROM}\EmuMPU401.dll"
				RegDLL "EmuMPU401.dll"
				File "${INSTALLFROM}\MIDIDevice.dll"
				RegDLL "MIDIDevice.dll"
				File "${INSTALLFROM}\MIDIIndicator.dll"
				RegDLL "MIDIIndicator.dll"
				File "${INSTALLFROM}\MIDIToolkit.dll"
				RegDLL "MIDIToolkit.dll"
				File "${INSTALLFROM}\identity.map"
			SectionEnd
			Section "MT2GM patch" DESC_gm32
				SectionIn 1 2
					File "${INSTALLFROM}\mt2gm.map"
			SectionEnd
			Section "GM2MT patch" DESC_32gm
				SectionIn 1 2
					File "${INSTALLFROM}\gm2mt.map"
			SectionEnd
		SubSectionEnd
		Section "PPDAC" DESC_pdac
			SectionIn 1 2
			File "${INSTALLFROM}\EmuPPDAC.dll"
			RegDLL "EmuPPDAC.dll"
		SectionEnd
		SubSection "Joystick" DESC_joys
			Section "Joystick Core" DESC_jcor
				SectionIn 1 2
					File "${INSTALLFROM}\EmuJoystick.dll"
				RegDLL "EmuJoystick.dll"
			SectionEnd
			Section "4-button 4-axis map" DESC_joy1
				SectionIn 1 2
					File "${INSTALLFROM}\joy1.map"
			SectionEnd
			Section "Two 2-button 2-axis map" DESC_joy2
				SectionIn 1 2
					File "${INSTALLFROM}\joy2.map"
			SectionEnd
			Section "2-button 3-axis rudder & POV map" DESC_joy3
				SectionIn 1 2
					File "${INSTALLFROM}\joy3.map"
			SectionEnd
		SubSectionEnd
		Section "DiskWriter" DESC_dskw
			SectionIn 1 2
			File "${INSTALLFROM}\DiskWriter.dll"
			RegDLL "DiskWriter.dll"
		SectionEnd
	SubSectionEnd

	## vdmsound launchpad files
	Section "Launchpad" DESC_lpad
		SectionIn 1
		File "${INSTALLFROM}\Launchpad.dll"
		RegDLL "Launchpad.dll"
	SectionEnd

	## nolfb files
	SubSection "NOLFB" DESC_nlfb
		Section "Program" DESC_nlfp
			SectionIn 1
			File "${INSTALLFROM}\NOLFB.COM"
		SectionEnd
		Section "Source" DESC_nlfs
			SetOutPath "$INSTDIR\Source"
			File "${INSTALLFROM}\NOLFB.ASM"
		SectionEnd
	SubSectionEnd
 #	Section "SAPUCDEX" DESC_sapu ## needs OK from SaPu
 #		SectionIn 1
 #	SectionEnd

	## this section must be installed
	Section ""
		SetOutPath "$INSTDIR"
		!ifdef ALLUSERS
			WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${MUI_PRODUCT}" "DisplayName" "${MUI_PRODUCT}"
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
		UnRegDLL "DMAController.dll"
		Delete "$INSTDIR\DMAController.dll"
		UnRegDLL "DSoundDevice.dll"
		Delete "$INSTDIR\DSoundDevice.dll"
		UnRegDLL "VDDLoader.dll"
		Delete "$INSTDIR\VDDLoader.dll"
		UnRegDLL "VDMConfig.dll"
		Delete "$INSTDIR\VDMConfig.dll"
		UnRegDLL "WaveDevice.dll"
		Delete "$INSTDIR\WaveDevice.dll"

		RMDir /r "$INSTDIR\Docs\"

		Delete "$INSTDIR\VDMSound.ico"

		Delete "$INSTDIR\VDMConfig.cnt"
		Delete "$INSTDIR\VDMConfig.hlp"
		Delete "$INSTDIR\VDMConfig.GID"

		UnRegDLL "EmuSBCompat.dll"
		Delete "$INSTDIR\EmuSBCompat.dll"

		UnRegDLL "EmuAdlib.dll"
		Delete "$INSTDIR\EmuAdlib.dll"

		UnRegDLL "EmuMPU401.dll"
		Delete "$INSTDIR\EmuMPU401.dll"
		UnRegDLL "MIDIDevice.dll"
		Delete "$INSTDIR\MIDIDevice.dll"
		UnRegDLL "MIDIIndicator.dll"
		Delete "$INSTDIR\MIDIIndicator.dll"
		UnRegDLL "MIDIToolkit.dll"
		Delete "$INSTDIR\MIDIToolkit.dll"
		Delete "$INSTDIR\identity.map"
			Delete "$INSTDIR\mt2gm.map"
			Delete "$INSTDIR\gm2mt.map"

		UnRegDLL "EmuPPDAC.dll"
		Delete "$INSTDIR\EmuPPDAC.dll"

		UnRegDLL "EmuJoystick.dll"
		Delete "$INSTDIR\EmuJoystick.dll"
			Delete "$INSTDIR\joy1.map"
			Delete "$INSTDIR\joy2.map"
			Delete "$INSTDIR\joy3.map"

		UnRegDLL "DiskWriter.dll"
		Delete "$INSTDIR\DiskWriter.dll"

		## launchpad
		UnRegDLL "Launchpad.dll"
		Delete "$INSTDIR\Launchpad.dll"

		## nolfb
		Delete "$INSTDIR\NOLFB.COM"
		Delete "$INSTDIR\Source\NOLFB.ASM"

		!ifdef REMFILES
			Delete "$INSTDIR\VDMS.INI"
			Delete "$INSTDIR\AUTOEXEC.VDMS"
			Delete "$INSTDIR\CONFIG.VDMS"
			RMDir "$INSTDIR"
		!endif

		Delete "$INSTDIR\${MUI_UNINSTALLER_FILENAME}"

		IfErrors rebootMaybeRequired
			rebootMaybeRequired:
				MessageBox MB_OK "You may need to reboot your computer to remove files that could not be deleted during uninstallation. $\r$\n$0 $\r$\n$1 $\r$\n$2 $\r$\n$3 $\r$\n$4 $\r$\n$5 $\r$\n$6 $\r$\n$7 $\r$\n$8 $\r$\n$9"

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
		 !insertmacro MUI_DESCRIPTION_TEXT ${DESC_core} "The VDMSound engine."
	   !insertmacro MUI_DESCRIPTION_TEXT ${DESC_help} "VDMS.INI configuration guide."
		 !insertmacro MUI_DESCRIPTION_TEXT ${DESC_sndb} "SoundBlaster Compatible plugin."
		 !insertmacro MUI_DESCRIPTION_TEXT ${DESC_adlb} "AdLib OPL2 and OPL3 plugin."
		 !insertmacro MUI_DESCRIPTION_TEXT ${DESC_m401} "MPU-401 General MIDI Interface."
		  !insertmacro MUI_DESCRIPTION_TEXT ${DESC_mcor} "MPU-401 General MIDI Interface plugins."
		  !insertmacro MUI_DESCRIPTION_TEXT ${DESC_gm32} "MT-32 to General MIDI patch."
		  !insertmacro MUI_DESCRIPTION_TEXT ${DESC_32gm} "General MIDI to MT-32 patch."
		  !insertmacro MUI_DESCRIPTION_TEXT ${DESC_pdac} "Parallel port Digital-to-Analogue Converter plugin (Disney Sound Source)."
		 !insertmacro MUI_DESCRIPTION_TEXT ${DESC_joys} "Gameport Joystick."
		  !insertmacro MUI_DESCRIPTION_TEXT ${DESC_jcor} "Gameport Joystick plugin."
		  !insertmacro MUI_DESCRIPTION_TEXT ${DESC_joy1} "Map file for one joystick with four buttons and four axes."
		  !insertmacro MUI_DESCRIPTION_TEXT ${DESC_joy2} "Map file for two joysticks with two buttons and two axes per joystick."
		  !insertmacro MUI_DESCRIPTION_TEXT ${DESC_joy3} "Map file for one joystick with two buttons, three axes, rudder and discrete POV."
		 !insertmacro MUI_DESCRIPTION_TEXT ${DESC_dskw} "Enables the ability to output sound to disk."
		!insertmacro MUI_DESCRIPTION_TEXT ${DESC_lpad} "Windows GUI for VDMSound."
		!insertmacro MUI_DESCRIPTION_TEXT ${DESC_nlfb} "Disables Linear Framebuffer in DOS VESA games. By Ken Silverman."
		 !insertmacro MUI_DESCRIPTION_TEXT ${DESC_nlfp} "Executable for NOLFB."
		 !insertmacro MUI_DESCRIPTION_TEXT ${DESC_nlfs} "Source code for NOLFB (written in assembly)."
 #		!insertmacro MUI_DESCRIPTION_TEXT ${DESC_sapu} "Restores full MSCDEX functionality (MSCDEXNT replacement). By Sauro Puccini." ## does not exist yet
	!insertmacro MUI_FUNCTIONS_DESCRIPTION_END

### <<EOF