;/***************************************************************************
; *
; * This file contains the message definitions for VDMConfig.dll
; *
; ***************************************************************************/
;
;

MessageIdTypedef=DWORD

MessageId=0x2100
Severity=Error
Facility=Application
SymbolicName=MSG_ERR_COMINIT
Language=English
Unable to initialize the COM framework:%n%2 (0x%1!08x!)%0
.

MessageId=
Severity=Error
Facility=Application
SymbolicName=MSG_ERR_STL_EXCEPTION
Language=English
An unhandled exception was encountered:%n%n%1%0
.

MessageId=
Severity=Error
Facility=Application
SymbolicName=MSG_ERR_IO_OPEN
Language=English
An error was encountered while attempting to open file '%1' for reading.%n%nLast error reported by Windows:%n0x%2!08x! - %3%0
.

MessageId=
Severity=Error
Facility=Application
SymbolicName=MSG_ERR_IO
Language=English
Unknown I/O error encountered while accessing the file '%1':%n%nLast error reported by Windows:%n0x%2!08x! - %3%0
.

MessageId=
Severity=Error
Facility=Application
SymbolicName=MSG_ERR_PARSE_SYNTAX
Language=English
Syntax error (%1):%nExpected '%3', but found '%2' instead.%0
.

MessageId=
Severity=Error
Facility=Application
SymbolicName=MSG_ERR_PARSE_FORMAT
Language=English
Unexpected data (%1):%nA non-empty line was encountered before the start of a section.%0
.

MessageId=
Severity=Error
Facility=Application
SymbolicName=MSG_ERR_PARSE_CONFLICT
Language=English
Symbol redefinition (%1):%nThe entry '%3' was already defined (see %2).%0
.

MessageId=
Severity=Error
Facility=Application
SymbolicName=MSG_ERR_PARSE
Language=English
Unknown parsing error (%1):%nDescription not available.%0
.

MessageId=
Severity=Error
Facility=Application
SymbolicName=MSG_ERR_MODULENAME
Language=English
The following section(s) are reserved and must not be used:%n%n%1%0
.

MessageId=
Severity=Warning
Facility=Application
SymbolicName=MSG_WRN_ORPHANS
Language=English
The following section(s) could not be associated with any module:%n%n%1%nDo you want to continue?%0
.

MessageId=
Severity=Error
Facility=Application
SymbolicName=MSG_ERR_CYCLICDEP
Language=English
One or more dependency cycles were detected between the emulation modules:%n%n%1%n%nThe following modules cannot, therefore, be intialized:%n%n%2%nDo you want to continue?%0
.

MessageId=
Severity=Error
Facility=Application
SymbolicName=MSG_ERR_TIPS
Language=English
An error was encountered while attempting to load and display the Tip of the Day:%n%nLast error reported by Windows:%n0x%1!08x! - %2%n%nThe initialization process will now continue.%0
.


; /////////////////////////////////////////////////////////////////////////////


MessageId=0x2200
Severity=Error
Facility=Application
SymbolicName=MSG_ERR_QUERY_NOT_INIT
Language=English
Query object not properly initialized (m_pEnv = 0x%1!08x!).%0
.

MessageId=
Severity=Error
Facility=Application
SymbolicName=MSG_ERR_QUERY_DEP_INISECTION
Language=English
Could not find the INI section [%2] describing the dependencies of module '%1'.%0
.

MessageId=
Severity=Error
Facility=Application
SymbolicName=MSG_ERR_QUERY_DEP_INIKEY
Language=English
Could not locate the dependency reference '%3' in either of the [%2] or [%5] INI sections (see %4).%0
.

MessageId=
Severity=Error
Facility=Application
SymbolicName=MSG_ERR_QUERY_DEP_INST
Language=English
No instance of module '%2' is available.  Module '%2' ('%3') is a dependency of '%1' (see %4).%0
.

MessageId=
Severity=Error
Facility=Application
SymbolicName=MSG_ERR_QUERY_CFG_INISECTION
Language=English
Could not find the INI section [%2] describing the configuration of module '%1'.%0
.

MessageId=
Severity=Error
Facility=Application
SymbolicName=MSG_ERR_QUERY_CFG_INIKEY
Language=English
Could not locate the configuration property '%3' in either of the [%2] or [%5] INI sections (see %4).%0
.


; /////////////////////////////////////////////////////////////////////////////


MessageId=0x3000
Severity=Informational
Facility=Application
SymbolicName=MSG_STR_MODULE_INST
Language=English
Unable to instantiate emulation module '%1'.%n%n%2%nDo you want to continue?%0
.

MessageId=
Severity=Error
Facility=Application
SymbolicName=MSG_ERR_LOADMETHOD
Language=English
No appropriate load method could be used to instantiate '%1' (see %2).%0
.

MessageId=
Severity=Informational
Facility=Application
SymbolicName=MSG_STR_MODULE_CONFIG
Language=English
Unable to initialize and configure emulation module '%1'.%n%n%2%n%nDo you want to continue?%0
.

MessageId=
Severity=Informational
Facility=Application
SymbolicName=MSG_STR_MODULE_DESTROY
Language=English
An error was encountered while releasing module '%1'.%n%n%2%0
.
