#ifndef __PIF_FMT_H
#define __PIF_FMT_H

/****************************************************************************
**
** Based on "The PIF file format in various Windows versions" (14-Feb-2000)
**  (C) 2000 Sergey Merzlikin <smsoft@chat.ru>
**
****************************************************************************/

#pragma pack ( push, 1 )

/*
**
** Section Header
**
*/
struct PIF_HDRSECTION {               
  CHAR    szSectionName[16];                  // Name of the section.  ANSI character set, is trailed by a null symbol.
  WORD    wNextSecHeaderOffset;               // Offset of heading of the next section from the beginning of the file.  In last section accepts value FFFFh.
  WORD    wThisSecBodyOffset;                 // Offset of the beginning of the section data from the beginning of the file.
  WORD    wThisSecBodyLength;                 // Section data length.  In bytes.
};


/*
**
** Basic section
**
*/
#define PIF_BAS_DIRECTMEMMODIF        0x0001  // Directly modify memory
#define PIF_BAS_MULTIMODE             0x0002  // Graphics / Multiple Text mode
#define PIF_BAS_PREVENTPROGSWITCH     0x0004  // Prevent program switch
#define PIF_BAS_NOSCREENEXCHANGE      0x0008  // No screen exchange
#define PIF_BAS_CLOSEONEXIT           0x0010  // Close window on exit
#define PIF_BAS_DIRECTCOM1            0x0040  // Direct interaction with COM1
#define PIF_BAS_DIRECTCOM2            0x0080  // Direct interaction with COM2
                                      
#define PIF_BAS_DIRECTKBD             0x0010  // Direct interaction with the keyboard
#define PIF_BAS_USECOPROC             0x0020  // ?? Use coprocessor
#define PIF_BAS_STOPINBACKGROUND      0x0040  // ?? Stop in background mode
#define PIF_BAS_DIRECTSCRMODIF        0x0080  // Directly modify screen
#define PIF_BAS_ISRXCHG               0x2000  // ?? Exchange interrupt vectors
#define PIF_BAS_HASPARAMS             0x4000  // There are parameters in the command line

struct PIF_BASICSECTION {
  BYTE    cReserved1;                         // Not used.
  BYTE    cChecksum;                          // Checksum.  In Windows 95/98 - always 78h, in other OS - the value varies. Never is checked.
  CHAR    szWndTitle[30];                     // Window title.  The OEM character set. Is padded by blanks up to 30 symbols.
  WORD    wMaxConvMem;                        // Maximal amount of the conventional memory in kilobytes.  Is set by the PIF-editor only in Windows 1.X, 2.X and Windows 3.X in standard mode, in other cases accepts a default value.
  WORD    wMinConvMem;                        // Minimal amount of the conventional memory in kilobytes.  Is set by the PIF-editor only in Windows 1.X, 2.X and Windows 3.X in standard mode, in other cases accepts a default value.
  CHAR    szProgFName[63];                    // The program filename without parameters.  Can contain complete path, long filename without quotes, trailed by a null symbol, OEM character set.
  WORD    wFlags1;                            // Bit mask. In Windows 1.X, 2.X bit 02h concerns to parameter Screen Exchange - Graphics / Text.
  CHAR    szWorkingDir[64];                   // The working directory.  Long pathname without quotes, trailed by a null symbol, OEM character set.
  CHAR    szParams[64];                       // Parameters string.  Is trailed by a null symbol. OEM character set. In 32-bit OS and in enhanced mode of Windows 3.X this value is used only if there is no section containing a parameters string for given OS.
  CHAR    cReserved2;                         // ?? Video mode.  Not used.
  CHAR    cReserved3;                         // ?? Quantity of the allocated text video pages.  Not used.
  CHAR    cReserved4;                         // ?? The first used interrupt.  Not used.
  CHAR    cReserved5;                         // ?? Last used interrupt.  Not used.
  CHAR    cReserved6;                         // ?? Height of the screen in symbols.  Not used.
  CHAR    cReserved7;                         // ?? Width of the screen in symbols.  Not used.
  CHAR    cReserved8;                         // ?? Horizontal window position.  Not used.
  CHAR    cReserved9;                         // ?? Vertical window position.  Not used.
  WORD    wReserved10;                        // Number of last text video page is not used. In Windows 1.X, 2.X bit 0010h concerns to parameter Program Switch - Graphics / Multiple Text.
  CHAR    cReserved11[128];                   // Not used
  WORD    wFlags2;                            // Bit mask. In Windows 1.X, 2.X bit 4000h is always set, when the parameter Directly Modify Screen is changed the bits 0080h and 0040h are set simultaneously.
};


/*
**
** Windows 386 v3.0 section
**
*/
#define PIF_386_ALLOWCLOSE        0x00000001  // Permit an exit when is active
#define PIF_386_WORKINBACGKGROUND 0x00000002  // Continuation of work in a background mode
#define PIF_386_EXCLUSIVERUN      0x00000004  // Exclusive run mode
#define PIF_386_FULLSCREEN        0x00000008  // Full-screen mode
#define PIF_386_NOALTTAB          0x00000020  // Not use Alt+Tab
#define PIF_386_NOALTESC          0x00000040  // Not use Alt+Esc
#define PIF_386_NOALTSPACE        0x00000080  // Not use Alt+Space
#define PIF_386_NOALTENTER        0x00000100  // Not use Alt+Enter
#define PIF_386_NOALTPRTSC        0x00000200  // Not use Alt+PrtSc
#define PIF_386_NOPRTSC           0x00000400  // Not use PrtSc
#define PIF_386_NOCTRLESC         0x00000800  // Not use Ctrl+Esc
#define PIF_386_DETECT_IDLE       0x00001000  // Detect idle time
#define PIF_386_NOHMA             0x00002000  // Not use HMA
#define PIF_386_USESHORTCUT       0x00004000  // Use shortcut key
#define PIF_386_EMSLOCK           0x00008000  // EMS memory is locked
#define PIF_386_XMSLOCK           0x00010000  // XMS memory is locked
#define PIF_386_FASTPASTE         0x00020000  // Use fast paste
#define PIF_386_APPMEMLOCK        0x00040000  // Lock application memory
#define PIF_386_MEMPROTECTION     0x00080000  // Memory protection
#define PIF_386_MINWND            0x00100000  // Minimized window
#define PIF_386_MAXWND            0x00200000  // Maximized window
#define PIF_386_MSDOSMODE         0x00800000  // MS-DOS mode
#define PIF_386_NOWINDETECT       0x01000000  // Prevent Windows detection
#define PIF_386_NOAUTOMSDOSMODE   0x04000000  // Not automatically offer transition in a MS-DOS mode
#define PIF_386_NOWARNMSDOSMODE   0x10000000  // Not warn before transition in a MS-DOS mode

#define PIF_386_VIDEOROMEMU           0x0001  // Video ROM emulation
#define PIF_386_NOTXTPORTCHK          0x0002  // Do not check ports: text
#define PIF_386_NOLOGFXPORTCHK        0x0004  // Do not check ports: low graphics
#define PIF_386_NOHIGFXPORTCHK        0x0008  // Do not check ports: high graphics
#define PIF_386_TXTVIDEOMEM           0x0010  // Video memory: text
#define PIF_386_LOGFXVIDEOMEM         0x0020  // Video memory: low graphics
#define PIF_386_HIGFXVIDEOMEM         0x0040  // Video memory: high graphics
#define PIF_386_RETAINVIDEOMEM        0x0080  // Retain video memory

#define PIF_386_SHORTCUTMODIFSHIFT    0x0003  // Retain video memory
#define PIF_386_SHORTCUTMODIFCTRL     0x0004  // Retain video memory
#define PIF_386_SHORTCUTMODIFALT      0x0008  // Retain video memory

struct PIF_WIN386_30_SECTION {
  WORD    wMaxConvMem;                        // Maximal amount of the conventional memory in kilobytes, which can occupy the application.  For 32-bit OS auto - 640 and more, for Windows 3.X to allocate all memory - FFFFh.
  WORD    wReqConvMem;                        // Required amount of the conventional memory in kilobytes.  For 32-bit OS auto - 0, for Windows 3.X to allocate all memory - FFFFh.
  WORD    wActivePrio;                        // Active priority.  Range of values: 1-10000.
  WORD    wBackgroundPrio;                    // Background priority  Range of values: 1-10000.
  WORD    wMaxEMSMem;                         // Maximal amount of EMS memory in kilobytes, which can occupy the application.  The maximal value - 16384, auto for 32-bit OS or to allocate all for Windows 3.X - FFFFh.
  WORD    wReqEMSMem;                         // Required amount of EMS memory.  The maximal value - 16384, auto for 32-bit OS - 0 to allocate all for Windows 3.X - FFFFh.
  WORD    wMaxXMSMem;                         // Maximal amount of XMS memory in kilobytes, which can occupy the application.  The maximal value - 16384, auto for 32-bit OS or to allocate all for Windows 3.X - FFFFh.
  WORD    wReqXMSMem;                         // Required amount of XMS memory.  The maximal value - 16384, auto for 32-bit OS - 0 to allocate all for Windows 3.X - FFFFh.
  DWORD   dwFlags1;                           // Bit mask. In 32-bit OS the bit 00001000h is set to zero when the maximal priority is set.
  WORD    wFlags2;                            // Bit mask.
  WORD    wReserved1;                         // Unknown.
  WORD    wShortcutSCode;                     // Scan code of a shortcut key.  See Microsoft KB Q37617.
  WORD    wShortcutModif;                     // Bit mask.
  WORD    wShortcutEnable;                    // Use shortcut key.
  WORD    wShortcutIsExtended;                // The shortcut key scan code is extended.
  WORD    wReserved2;                         // Unknown.
  WORD    wReserved3;                         // Unknown.
  DWORD   dwReserved4;                        // Unknown.
  CHAR    szParams[64];                       // Parameters string.  Is trailed by a null symbol. OEM character set. There is a bug in Windows 95/98/NT/2000, due to which the parameters string saves in the ANSI charset, besides in Windows 95/98 this value passes to program once again converted into the ANSI charset.
};


/*
**
** Windows VMM v4.0 section
**
*/
#define PIF_VMM_WORKINBACGKGROUND     0x0002  // Continue to run in background mode
#define PIF_VMM_NOWARNONEXIT          0x0010  // Not warn on exit
#define PIF_VMM_NOSCREENSAVER         0x0200  // Disallow Screen Saver

#define PIF_VMM_VIDEOROMEMU           0x0001  // Video-ROM emulation.
#define PIF_VMM_NODYNVIDEOMEM         0x0080  // Do not dynamically allocate video memory.
#define PIF_VMM_FULLSCREEN            0x0100  // Full-screen mode

#define PIF_VMM_FASTPASTE             0x0001  // Fast paste
#define PIF_VMM_NOALTTAB              0x0020  // Not use Alt+Tab
#define PIF_VMM_NOALTESC              0x0040  // Not use Alt+Esc
#define PIF_VMM_NOALTSPACE            0x0080  // Not use Alt+Space
#define PIF_VMM_NOALTENTER            0x0100  // Not use Alt+Enter
#define PIF_VMM_NOALTPRTSC            0x0200  // Not use Alt+PrtSc
#define PIF_VMM_NOPRTSC               0x0400  // Not use PrtSc
#define PIF_VMM_NOCTRLESC             0x0800  // Not use Ctrl+Esc

#define PIF_VMM_NOMOUSESEL            0x0001  // Not use the mouse for selection.
#define PIF_VMM_EXCLUSIVEMOUSE        0x0002  // Exclusive use of the mouse.

#define PIF_VMM_FNTUSERASTER          0x0004  // Use raster fonts.
#define PIF_VMM_FNTUSETTF             0x0008  // Use TrueType fonts.
#define PIF_VMM_FNTAUTOSIZE           0x0010  // Automatically choose the font size.
#define PIF_VMM_FNTCURISRASTER        0x0400  // The current font is raster.
#define PIF_VMM_FNTCURISTTF           0x0800  // The current font is TrueType.

#define PIF_VMM_SHOWTOOLBAR           0x0020  // Show toolbar.

#define PIF_VMM_WNDNORMAL             0x0001  // At last start the window was of the normal size
#define PIF_VMM_WNDMIN                0x0002  // At last start the window was minimized
#define PIF_VMM_WNDMAX                0x0003  // At last start the window was maximized

struct PIF_VMM_40_SECTION { 
  CHAR    cReserved1[88];                     // Unknown
  CHAR    szIconFile[80];                     // Name of the file containing an icon.  ANSI character set, long filename without quotes, can contain a complete path, is trailed by a null symbol.
  WORD    wIconID;                            // Number of an icon.  The first icon in a file has number 0.
  WORD    wFlags1;                            // Bit mask.
  CHAR    cReserved2[10];                     // Unknown.
  WORD    wPriority;                          // Priority.  Number from 0 up to 100, 0 - maximal priority, 100 - minimal.
  WORD    wFlags2;                            // Bit mask.
  CHAR    cReserved3[8];                      // Unknown.
  WORD    wTxtRows;                           // Number of text lines in a window.  Auto - 0, or appropriate number (25, 43 or 50).
  WORD    wFlags3;                            // Bit mask.
  WORD    wReserved4;                         // Unknown.
  WORD    wReserved5;                         // Unknown.
  WORD    wReserved6;                         // Unknown.
  WORD    wReserved7;                         // Unknown.
  WORD    wReserved8;                         // Unknown.
  WORD    wReserved9;                         // Unknown.
  WORD    wReserved10;                        // Unknown.
  WORD    wReserved11;                        // Unknown.
  WORD    wFlags4;                            // Bit mask.
  CHAR    cReserved12[6];                     // Unknown.
  WORD    wFlags5;                            // Bit mask.
  WORD    wReserved13;                        // Unknown.
  WORD    wFntWidth1;                         // Horizontal size of the current font (only for raster fonts).  Auto or TrueType - 0000h, raster font - horizontal size in pixels.
  WORD    wFntHeight1;                        // Vertical size of the current font.  Auto - any value, otherwise vertical size in pixels.
  WORD    wFntWidth2;                         // Horizontal size of the current font.  Auto - any value, otherwise horizontal size in pixels.
  WORD    wFntHeight2;                        // Vertical size of the current font.  Auto - any value, otherwise vertical size in pixels.
  CHAR    szFntName1[32];                     // The name of a raster font.  Not used, trailed by a null symbol.
  CHAR    szFntName2[32];                     // The name of a TrueType font.  Not used, trailed by a null symbol.
  WORD    wReserved14;                        // Unknown.
  WORD    wFlags6;                            // Bit mask.
  WORD    wNoSettingsRestore;                 // Not restore settings at startup.
  WORD    wWndColumns;                        // ?? The horizontal size of the screen in symbols.  After the first start of the program the value becomes equal to 80.
  WORD    wWndRows;                           // ?? The vertical size of the screen in symbols.  After the first start of the program the value becomes equal to 25.
  WORD    wWndWidth;                          // The horizontal size of the window client area.  In pixels.
  WORD    wWndHeight;                         // The vertical size of the window client area.  In pixels.
  WORD    wWndNCWidth;                        // The horizontal size of the window.  In pixels.
  WORD    wWndNCHeight;                       // The vertical size of the window.  In pixels.
  WORD    wReserved15;                        // Unknown.
  WORD    wWndState1;                         // At last start the window was maximized.  Bit mask. If the given value is not set, the value at offset 0140h is not interpreted.
  WORD    wWndState2;                         // Is not a bit mask. The value 0002h is interpreted as the maximized window.
  WORD    wReserved16;                        // Unknown.
  WORD    wReserved17;                        // Unknown.
  WORD    wWndMaxRPos;                        // The right window border position in maximized window.  In pixels, if the given value is less than or equal to the left border position, and the values at offsets 013Eh, 0140h specify that the window was maximized, it is considered, that the parameters of the window were not saved, and the default values are used.
  WORD    wWndMaxBPos;                        // The bottom window border position in maximized window.  In pixels, if the given value is less than or equal to the top border position, and the values at offsets 013Eh, 0140h specify that the window was maximized, it is considered, that the parameters of the window were not saved, and the default values are used.
  WORD    wWndLPos;                           // Left window border position.  In pixels.
  WORD    wWndTPos;                           // Top window border position.  In pixels.
  WORD    wWndNormRPos;                       // The right window border position in normal window.  In pixels, if the given value is less than or equal to the left border position, and the values at offsets 013Eh, 0140h specify that the window was not maximized, it is considered, that the parameters of the window were not saved, and the default values are used.
  WORD    wWndNormBPos;                       // The bottom window border position in normal window.  In pixels, if the given value is less than or equal to the top border position, and the values at offsets 013Eh, 0140h specify that the window was not maximized, it is considered, that the parameters of the window were not saved, and the default values are used.
  CHAR    cReserved18[4];                     // Unknown.
  CHAR    szBatchName[80];                    // Name of the BAT file.  OEM character set. Can contain a complete path. If the long filename contains spaces, it must be enclosed in quotes. Is trailed by a null symbol.
  WORD    wEnvMem;                            // Memory amount for environment.  Auto - 0, otherwise amount of memory in kilobytes, max - 4096.
  WORD    wDPMIMem;                           // Volume of memory DPMI.  Auto - 0, otherwise amount of memory in kilobytes, max - 16384.
  WORD    wReserved19;                        // Unknown.
};


/*
**
** Windows NT 3.1 section
**
*/
#define PIF_NT3_COMPATTIMER           0x0010  // Compatible hardware timer emulation.


struct PIF_NT_31_SECTION { 
  WORD    wFlags;                             // Bit mask.
  CHAR    cReserved1[10];                     // Unknown.
  CHAR    szConfig[64];                       // The name of a file used in place of Config.sys.  ANSI character set, trailed by a null symbol.
  CHAR    szAutoexec[64];                     // The name of a file used in place of Autoexec.bat.  ANSI character set, trailed by a null symbol.
  WORD    wReserved2;                         // Unknown.
};


/*
**
** Windows NT 4.0 section
**
*/
struct PIF_NT_40_SECTION { 
  DWORD   dwReserved1;                        // Unknown.
  WCHAR   szCmdLineW[128];                    // The command line with parameters.  Unicode character set, is trailed by a null symbol, can contain a full path, if the long filename contains spaces, it must be enclosed in quotes. This parameter is used only if the value at offset 0104h coincides (after charset conversion) with value constituted of the filename from the basic section and parameters string from the section “WINDOWS VMM 4.0”, if this section exists, or from the basic section otherwise.
  CHAR    szCmdLineA[128];                    // The command line with parameters.  ANSI character set, is trailed by a null symbol, can contain a full path, if the long filename contains spaces, it must be enclosed in quotes. See comment to the value at offset 0004h.
  CHAR    szReserved2[240];                   // Unknown.
  WCHAR   szPifFileW[80];                     // The filename of PIF file.  Unicode character set, is trailed by a null symbol, contains a full path, the long filename without quotes.
  CHAR    szPifFileA[80];                     // The filename of PIF file.  ANSI character set, is trailed by a null symbol, contains a full path, the long filename without quotes.
  WCHAR   szWndTitleW[30];                    // Window title.  Unicode character set, is trailed by a null symbol. This parameter is used only if the value at offset 03A0h coincides (after charset conversion) with the window title from the basic section.
  CHAR    szWndTitleA[30];                    // Window title.  ANSI character set, is trailed by a null symbol. See comment to the value at offset 0364h.
  WCHAR   szIconFileW[80];                    // Name of the file containing an icon.  Unicode character set, is trailed by a null symbol, can contain a full path, the long filename without quotes. This parameter is used only if the value at offset 045Eh coincides (after charset conversion) with value from the section “WINDOWS VMM 4.0”, or if this section is missing.
  CHAR    szIconFileA[80];                    // Name of the file containing an icon.  ANSI character set, is trailed by a null symbol, can contain a full path, the long filename without quotes. See comment to the value at offset 036Eh.
  WCHAR   szWorkDirW[64];                     // Working directory.  Unicode character set, is trailed by a null symbol, if the long pathname contains spaces, it must be enclosed in quotes. This parameter is used only if the value at offset 052Eh coincides (after charset conversion) with corresponding value from the basic section.
  CHAR    szWorkDirA[64];                     // Working directory.  ANSI character set, is trailed by a null symbol, if the long pathname contains spaces, it must be enclosed in quotes. See comment to the value at offset 04AEh.
  CHAR    cReserved3[286];                    // Unknown.
};

#pragma pack ( pop )

#endif // __PIF_FMT_H
