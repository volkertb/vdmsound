/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 5.01.0164 */
/* at Wed Mar 07 00:37:21 2001
 */
/* Compiler settings for F:\VDMS\CVSsource\VDMSModules\SOURCES\EmuJoystick\EmuJoystick.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __EmuJoystick_h__
#define __EmuJoystick_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __EmuJoysticks_FWD_DEFINED__
#define __EmuJoysticks_FWD_DEFINED__

#ifdef __cplusplus
typedef class EmuJoysticks EmuJoysticks;
#else
typedef struct EmuJoysticks EmuJoysticks;
#endif /* __cplusplus */

#endif 	/* __EmuJoysticks_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 


#ifndef __EMUJOYSTICKLib_LIBRARY_DEFINED__
#define __EMUJOYSTICKLib_LIBRARY_DEFINED__

/* library EMUJOYSTICKLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_EMUJOYSTICKLib;

EXTERN_C const CLSID CLSID_EmuJoysticks;

#ifdef __cplusplus

class DECLSPEC_UUID("F675312E-ECE0-4FD3-B96F-2E84597E6000")
EmuJoysticks;
#endif
#endif /* __EMUJOYSTICKLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif
