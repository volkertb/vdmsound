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
SymbolicName=MSG_ERR_E_POINTER
Language=English
In function '%1': the pointer passed in argument '%2' is NULL.%0
.

MessageId=
Severity=Error
Facility=Application
SymbolicName=MSG_ERR_E_INVALIDARG
Language=English
In function '%1': the value passed in argument '%2' is invalid (0x%3!08x!).%0
.

MessageId=
Severity=Error
Facility=Application
SymbolicName=MSG_ERR_APIFAIL
Language=English
In function '%1': the call to function '%2' failed.%0
.

MessageId=0x2200
Severity=Error
Facility=Application
SymbolicName=MSG_ERR_ALREADYINIT
Language=English
Cannot create more than one instance of the WinNT VDM Service Provider.%0
.

MessageId=
Severity=Error
Facility=Application
SymbolicName=MSG_ERR_NOTINIT
Language=English
Attempted to destroy a module that was instantiated, but not initialized.%0
.

MessageId=0x2300
Severity=Error
Facility=Application
SymbolicName=MSG_ERR_IOADDCONFLICT
Language=English
An emulation module attempted to take ownership of ports that were already hooked by another module.%n%nTargeted port range: 0x%1!03x! ... 0x%2!03x!%0
.

MessageId=
Severity=Error
Facility=Application
SymbolicName=MSG_ERR_IODELCONFLICT
Language=English
An emulation module attempted to unhook one or more ports that it did not own.%n%nTargeted port range: 0x%1!03x! ... 0x%2!03x!%0
.

MessageId=0x2400
Severity=Error
Facility=Application
SymbolicName=MSG_ERR_IOHOOKINST
Language=English
An error was encountered while attempting to register I/O hooks with the VDM for the following port ranges:%n%n%3%n%n0x%1!08x! - %2%n%nDo you want to continue?%0
.
