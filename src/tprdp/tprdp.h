#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef _WIN32
#define WINVER 0x0400
#include <windows.h>
#include <winsock.h>
#include <time.h>
#define DIR int
#else
#include <dirent.h>
#include <sys/time.h>
#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#else
#include <sys/types.h>
#include <unistd.h>
#endif
#endif
#include <limits.h> /* PATH_MAX */
#ifdef HAVE_SYSEXITS_H
#include <sysexits.h>
#endif

/* standard exit codes */
#ifndef EX_OK
#define EX_OK 0
#endif
#ifndef EX_USAGE
#define EX_USAGE 64
#endif
#ifndef EX_DATAERR
#define EX_DATAERR 65
#endif
#ifndef EX_NOINPUT
#define EX_NOINPUT 66
#endif
#ifndef EX_NOUSER
#define EX_NOUSER 67
#endif
#ifndef EX_NOHOST
#define EX_NOHOST 68
#endif
#ifndef EX_UNAVAILABLE
#define EX_UNAVAILABLE 69
#endif
#ifndef EX_SOFTWARE
#define EX_SOFTWARE 70
#endif
#ifndef EX_OSERR
#define EX_OSERR 71
#endif
#ifndef EX_OSFILE
#define EX_OSFILE 72
#endif
#ifndef EX_CANTCREAT
#define EX_CANTCREAT 73
#endif
#ifndef EX_IOERR
#define EX_IOERR 74
#endif
#ifndef EX_TEMPFAIL
#define EX_TEMPFAIL 75
#endif
#ifndef EX_PROTOCOL
#define EX_PROTOCOL 76
#endif
#ifndef EX_NOPERM
#define EX_NOPERM 77
#endif
#ifndef EX_CONFIG
#define EX_CONFIG 78
#endif

/* tprdp specific exit codes, lined up with disconnect PDU reasons */
#define EXRD_DISCONNECT_BY_ADMIN 1
#define EXRD_LOGOFF_BY_ADMIN 2
#define EXRD_IDLE_TIMEOUT 3
#define EXRD_LOGON_TIMEOUT 4
#define EXRD_REPLACED 5
#define EXRD_OUT_OF_MEM 6
#define EXRD_DENIED 7
#define EXRD_DENIED_FIPS 8
#define EXRD_INSUFFICIENT_PRIVILEGES 9
#define EXRD_FRESH_CREDENTIALS_REQUIRED 10
#define EXRD_DISCONNECT_BY_USER 11
#define EXRD_LOGOFF_BY_USER 12

#define EXRD_LIC_INTERNAL 16
#define EXRD_LIC_NOSERVER 17
#define EXRD_LIC_NOLICENSE 18
#define EXRD_LIC_MSG 19
#define EXRD_LIC_HWID 20
#define EXRD_LIC_CLIENT 21
#define EXRD_LIC_NET 22
#define EXRD_LIC_PROTO 23
#define EXRD_LIC_ENC 24
#define EXRD_LIC_UPGRADE 25
#define EXRD_LIC_NOREMOTE 26

#define EXRD_CB_DEST_NOT_FOUND 30
#define EXRD_CB_DEST_LOADING 32
#define EXRD_CB_REDIR_DEST 34
#define EXRD_CB_VM_WAKE 35
#define EXRD_CB_VM_BOOT 36
#define EXRD_CB_VM_NODNS 37
#define EXRD_CB_DEST_POOL_NOT_FREE 38
#define EXRD_CB_CONNECTION_CANCELLED 39
#define EXRD_CB_INVALID_SETTINGS 40
#define EXRD_CB_VM_BOOT_TIMEOUT 41
#define EXRD_CB_VM_BOOT_SESSMON_FAILED 42

#define EXRD_RDP_REMOTEAPPSNOTENABLED 50
#define EXRD_RDP_UPDATESESSIONKEYFAILED 51
#define EXRD_RDP_DECRYPTFAILED 52
#define EXRD_RDP_ENCRYPTFAILED 53

/* other exit codes */
#define EXRD_WINDOW_CLOSED 62
#define EXRD_UNKNOWN 63

#define STRNCPY(dst, src, n)              \
        {                                 \
                strncpy(dst, src, n - 1); \
                dst[n - 1] = 0;           \
        }

#ifndef MIN
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#endif

#ifndef MAX
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#endif

/* timeval macros */
#ifndef timerisset
#define timerisset(tvp) \
        ((tvp)->tv_sec || (tvp)->tv_usec)
#endif
#ifndef timercmp
#define timercmp(tvp, uvp, cmp)            \
        ((tvp)->tv_sec cmp(uvp)->tv_sec || \
         (tvp)->tv_sec == (uvp)->tv_sec && \
             (tvp)->tv_usec cmp(uvp)->tv_usec)
#endif
#ifndef timerclear
#define timerclear(tvp) \
        ((tvp)->tv_sec = (tvp)->tv_usec = 0)
#endif

/* If configure does not define the endianness, try
   to find it out */
#if !defined(L_ENDIAN) && !defined(B_ENDIAN)
#if __BYTE_ORDER == __LITTLE_ENDIAN
#define L_ENDIAN
#elif __BYTE_ORDER == __BIG_ENDIAN
#define B_ENDIAN
#else
#error Unknown endianness. Edit tprdp.h.
#endif
#endif /* B_ENDIAN, L_ENDIAN from configure */

/* No need for alignment on x86 and amd64 */
#if !defined(NEED_ALIGN)
#if !(defined(__x86__) || defined(__x86_64__) || \
      defined(__AMD64__) || defined(_M_IX86) ||  \
      defined(__i386__))
#define NEED_ALIGN
#endif
#endif

#include "./utilities/utils.h"
#include "./include/stream.h"
#include "./include/constants.h"
#include "./include/types.h"
#include "./include/proto.h"
