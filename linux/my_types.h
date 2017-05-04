#ifndef _MY_TYPES_H_
#define _MY_TYPES_H_

#include <limits.h>

#ifdef WIN32
#include <windows.h>
#endif /* WIN32 */

#ifdef _MSC_VER /* WIN32 */

/* from http://code.google.com/p/msinttypes/ */
#if _MSC_VER > 1000
#pragma once
#endif

#if (_MSC_VER < 1300)
   typedef signed char       int8_t;
   typedef signed short      int16_t;
   typedef signed int        int32_t;
   typedef unsigned char     uint8_t;
   typedef unsigned short    uint16_t;
   typedef unsigned int      uint32_t;
#else
   typedef signed __int8     int8_t;
   typedef signed __int16    int16_t;
   typedef signed __int32    int32_t;
   typedef unsigned __int8   uint8_t;
   typedef unsigned __int16  uint16_t;
   typedef unsigned __int32  uint32_t;
#endif
typedef signed __int64       int64_t;
typedef unsigned __int64     uint64_t;

/********************************/

#else /* *nix */

/* C99 compiler has this head file */
#include <stdint.h>
/* do not nee this */

#endif /* _MSC_VER */

/********************************/

typedef uint8_t     BYTE8, *PBYTE8, *LPBYTE8;
typedef int8_t      SBYTE8, *PSBYTE8, *LPSBYTE8;
typedef uint16_t    WORD16, *PWORD16, *LPWORD16;
typedef int16_t     SWORD16, *PSWORD16, *LPSWORD16;
typedef uint32_t    WORD32, DWORD32, *PDWORD32, *LPDWORD32;
typedef int32_t     SDWORD32, *PSDWORD32, *LPSDWORD32;
typedef uint64_t    WORD64, DWORD64, QWORD64, *PQWORD64, *LPQWORD64;
typedef int64_t     SQWORD64, *PSQWORD64, *LPSQWORD64;

/********************************************************************/

#ifndef WIN32

#ifndef CONST
#define CONST   const
#endif

typedef unsigned long   DWORD;
typedef int             BOOL;
typedef unsigned char   BYTE;
typedef unsigned short  WORD;
typedef float           FLOAT;
typedef FLOAT           *PFLOAT;

typedef int             INT;
typedef unsigned int    UINT;
typedef unsigned int    *PUINT;

// Void
typedef void *PVOID;

// Basics
#ifndef VOID
#define VOID    void
typedef char    CHAR;
typedef short   SHORT;
typedef long    LONG;
#endif

// ANSI (Multi-byte Character) types
typedef CHAR    *PCHAR;
typedef CHAR    *LPCH, *PCH;

typedef CONST CHAR *LPCCH, *PCCH;
typedef CHAR *NPSTR;
typedef CHAR *LPSTR, *PSTR;
typedef CONST CHAR *LPCSTR, *PCSTR;



#endif  /* WIN32 */

#endif /* _MY_TYPES_H_ */
