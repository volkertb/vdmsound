/* this file contains the actual definitions of */
/* the IIDs and CLSIDs */

/* link this file in with the server and any clients */


/* File created by MIDL compiler version 5.01.0164 */
/* at Wed Mar 07 00:37:21 2001
 */
/* Compiler settings for F:\VDMS\CVSsource\VDMSModules\SOURCES\EmuJoystick\EmuJoystick.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )
#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

const IID LIBID_EMUJOYSTICKLib = {0x8CFD797F,0x162B,0x4C9A,{0x9D,0x68,0xBD,0xBE,0xDA,0x30,0x07,0xD8}};


const CLSID CLSID_EmuJoysticks = {0xF675312E,0xECE0,0x4FD3,{0xB9,0x6F,0x2E,0x84,0x59,0x7E,0x60,0x00}};


#ifdef __cplusplus
}
#endif

