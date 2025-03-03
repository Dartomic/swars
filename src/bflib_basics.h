/******************************************************************************/
// Bullfrog Engine Emulation Library - for use to remake classic games like
// Syndicate Wars, Magic Carpet or Dungeon Keeper.
/******************************************************************************/
/** @file bflib_basics.h
 *     Header file for bflib_basics.c.
 * @par Purpose:
 *     Integrates all elements of the library with a common toolkit.
 * @par Comment:
 *     Just a header file - #defines, typedefs, function prototypes etc.
 * @author   Tomasz Lis
 * @date     10 Feb 2008 - 22 Dec 2008
 * @par  Copying and copyrights:
 *     This program is free software; you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
 */
/******************************************************************************/
#ifndef BFLIB_BASICS_H
#define BFLIB_BASICS_H

#include "bftypes.h"

#include <time.h>
#include <stdio.h>

#if defined(WIN32)||defined(DOS)||defined(GO32)
#include <io.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif
/******************************************************************************/
// Buffer sizes
// Disk path max length - restrictive equivalent for FILENAME_MAX
#define DISKPATH_SIZE    144
#define LINEMSG_SIZE     160
#define READ_BUFSIZE     256
#define LOOPED_FILE_LEN 4096
#define COMMAND_WORD_LEN  32

// Max length of any processed string
#define MAX_TEXT_LENGTH 4096
// Smaller buffer, also widely used
#define TEXT_BUFFER_LENGTH 2048

enum TbErrorLogFlags {
        Lb_ERROR_LOG_APPEND = 0,
        Lb_ERROR_LOG_NEW    = 1,
};

enum TbLogFlags {
        LbLog_DateInHeader = 0x0010,
        LbLog_TimeInHeader = 0x0020,
        LbLog_DateInLines  = 0x0040,
        LbLog_TimeInLines  = 0x0080,
        LbLog_LoopedFile   = 0x0100,
};

/******************************************************************************/
#pragma pack(1)

#include "bftime.h"

typedef time_t TbTimeSec;

typedef unsigned char TbChecksum;
typedef unsigned long TbBigChecksum;
typedef long Offset;
typedef short TbScreenPos;


#define LOG_PREFIX_LEN 32

struct TbLog {
        char filename[DISKPATH_SIZE];
        char prefix[LOG_PREFIX_LEN];
        ulong flags;
        TbBool Initialised;
        TbBool Created;
        TbBool Suspended;
        long position;
};

struct TbNetworkCallbackData;
/** Command function result, alias for TbResult. */
typedef int TbError;
/** Command function result, valid values are of TbErrorCode enumeration. */
typedef int TbResult;
typedef size_t TbSize;

#pragma pack()
/******************************************************************************/
#define ERRORLOG(fmt, ...) \
    fprintf(stdout, fmt " in %s\n", ##__VA_ARGS__, __func__)

#define DEBUGLOG(lv, fmt, ...) \
    fprintf(stdout, fmt " in %s\n", ##__VA_ARGS__, __func__)

#define BFLIB_ERRORLOG(fmt, ...) \
    fprintf(stdout, fmt " in %s\n", ##__VA_ARGS__, __func__)

#define BFLIB_DEBUGLOG(lv, fmt, ...) \
    fprintf(stdout, fmt " in %s\n", ##__VA_ARGS__, __func__)

/*
#ifdef __DEBUG
#define BFLIB_DEBUGLOG(lv, fmt, ...) \
    LbSyncLog("%s: " fmt "\n", __func__, ##__VA_ARGS__);
#else
#define BFLIB_DEBUGLOG(lv, fmt, ...)
#endif

 */
/******************************************************************************/
#ifdef __cplusplus
}
#endif
#endif
