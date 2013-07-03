/*++

Copyright (c) 1990-2000 Microsoft Corporation, All Rights Reserved

Module Name:

    enumcd.h

Abstract:

    This file includes data declarations for the Enumcd

Author:

    Ramu Ramanathan     05/15/2000

Notes:

Revision History:


--*/

#ifndef _ENUMCD_H_
#define _ENUMCD_H_


//
// Command Descriptor Block constants.
//

#define CDB6GENERIC_LENGTH         6
#define CDB10GENERIC_LENGTH        10


//
// SCSI CDB operation codes
//

#define SCSIOP_TEST_UNIT_READY     0x00
#define SCSIOP_REZERO_UNIT         0x01
#define SCSIOP_REWIND              0x01
#define SCSIOP_REQUEST_BLOCK_ADDR  0x02
#define SCSIOP_REQUEST_SENSE       0x03
#define SCSIOP_FORMAT_UNIT         0x04
#define SCSIOP_READ_BLOCK_LIMITS   0x05
#define SCSIOP_REASSIGN_BLOCKS     0x07
#define SCSIOP_READ6               0x08
#define SCSIOP_RECEIVE             0x08
#define SCSIOP_WRITE6              0x0A
#define SCSIOP_PRINT               0x0A
#define SCSIOP_SEND                0x0A
#define SCSIOP_SEEK6               0x0B
#define SCSIOP_TRACK_SELECT        0x0B
#define SCSIOP_SLEW_PRINT          0x0B
#define SCSIOP_SEEK_BLOCK          0x0C
#define SCSIOP_PARTITION           0x0D
#define SCSIOP_READ_REVERSE        0x0F
#define SCSIOP_WRITE_FILEMARKS     0x10
#define SCSIOP_FLUSH_BUFFER        0x10
#define SCSIOP_SPACE               0x11
#define SCSIOP_INQUIRY             0x12
#define SCSIOP_VERIFY6             0x13
#define SCSIOP_RECOVER_BUF_DATA    0x14
#define SCSIOP_MODE_SELECT         0x15
#define SCSIOP_RESERVE_UNIT        0x16
#define SCSIOP_RELEASE_UNIT        0x17
#define SCSIOP_COPY                0x18
#define SCSIOP_ERASE               0x19
#define SCSIOP_MODE_SENSE          0x1A
#define SCSIOP_START_STOP_UNIT     0x1B
#define SCSIOP_STOP_PRINT          0x1B
#define SCSIOP_LOAD_UNLOAD         0x1B
#define SCSIOP_RECEIVE_DIAGNOSTIC  0x1C
#define SCSIOP_SEND_DIAGNOSTIC     0x1D
#define SCSIOP_MEDIUM_REMOVAL      0x1E
#define SCSIOP_READ_CAPACITY       0x25
#define SCSIOP_READ                0x28
#define SCSIOP_WRITE               0x2A
#define SCSIOP_SEEK                0x2B
#define SCSIOP_LOCATE              0x2B
#define SCSIOP_WRITE_VERIFY        0x2E
#define SCSIOP_VERIFY              0x2F
#define SCSIOP_SEARCH_DATA_HIGH    0x30
#define SCSIOP_SEARCH_DATA_EQUAL   0x31
#define SCSIOP_SEARCH_DATA_LOW     0x32
#define SCSIOP_SET_LIMITS          0x33
#define SCSIOP_READ_POSITION       0x34
#define SCSIOP_SYNCHRONIZE_CACHE   0x35
#define SCSIOP_COMPARE             0x39
#define SCSIOP_COPY_COMPARE        0x3A
#define SCSIOP_WRITE_DATA_BUFF     0x3B
#define SCSIOP_READ_DATA_BUFF      0x3C
#define SCSIOP_CHANGE_DEFINITION   0x40
#define SCSIOP_READ_SUB_CHANNEL    0x42
#define SCSIOP_READ_TOC            0x43
#define SCSIOP_READ_HEADER         0x44
#define SCSIOP_PLAY_AUDIO          0x45
#define SCSIOP_PLAY_AUDIO_MSF      0x47
#define SCSIOP_PLAY_TRACK_INDEX    0x48
#define SCSIOP_PLAY_TRACK_RELATIVE 0x49
#define SCSIOP_PAUSE_RESUME        0x4B
#define SCSIOP_LOG_SELECT          0x4C
#define SCSIOP_LOG_SENSE           0x4D

#define MODE_PAGE_CAPABILITIES  0x2A


ULONG   DebugLevel = 1;
                            // 0 = Suppress All Messages
                            // 1 = Display & Fatal Error Message
                            // 2 = Warning & Debug Messages
                            // 3 = Informational Messages

//
// Bus Type
//

static char* BusType[] = {
    "UNKNOWN",  // 0x00
    "SCSI",
    "ATAPI",
    "ATA",
    "IEEE 1394",
    "SSA",
    "FIBRE",
    "USB",
    "RAID"
};

//
// SCSI Device Type
//

static char* DeviceType[] = {
    "Direct Access Device", // 0x00
    "Tape Device",          // 0x01
    "Printer Device",       // 0x02
    "Processor Device",     // 0x03
    "WORM Device",          // 0x04
    "CDROM Device",         // 0x05
    "Scanner Device",       // 0x06
    "Optical Disk",         // 0x07
    "Media Changer",        // 0x08
    "Comm. Device",         // 0x09
    "ASCIT8",               // 0x0A
    "ASCIT8",               // 0x0B
    "Array Device",         // 0x0C
    "Enclosure Device",     // 0x0D
    "RBC Device",           // 0x0E
    "Unknown Device"        // 0x0F
};


typedef struct _SCSI_PASS_THROUGH_WITH_BUFFERS {
    SCSI_PASS_THROUGH Spt;
    ULONG             Filler;      // realign buffers to double word boundary
    UCHAR             SenseBuf[32];
    UCHAR             DataBuf[512];
} SCSI_PASS_THROUGH_WITH_BUFFERS, *PSCSI_PASS_THROUGH_WITH_BUFFERS;


VOID  PrintError( ULONG );
VOID  PrintDataBuffer( PUCHAR, ULONG );
VOID  PrintStatusResults( BOOL, DWORD, PSCSI_PASS_THROUGH_WITH_BUFFERS );
VOID  PrintCapResults( BOOL, DWORD, PSCSI_PASS_THROUGH_WITH_BUFFERS );
VOID  PrintSenseInfo( PSCSI_PASS_THROUGH_WITH_BUFFERS );
BOOL  GetRegistryProperty( HDEVINFO, DWORD );
BOOL  GetDeviceProperty( HDEVINFO, DWORD );
VOID  DebugPrint( USHORT, PCHAR, ... );
ULONG GetMediaType(HANDLE hDevice, PUCHAR cdTypeString);

#endif    // _ENUMCD_H_



