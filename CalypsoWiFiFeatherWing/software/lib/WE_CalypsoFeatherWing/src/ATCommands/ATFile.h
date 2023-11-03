/*
 ***************************************************************************************************
 * This file is part of WIRELESS CONNECTIVITY SDK for STM32:
 *
 *
 * THE SOFTWARE INCLUDING THE SOURCE CODE IS PROVIDED “AS IS”. YOU ACKNOWLEDGE THAT WÜRTH ELEKTRONIK
 * EISOS MAKES NO REPRESENTATIONS AND WARRANTIES OF ANY KIND RELATED TO, BUT NOT LIMITED
 * TO THE NON-INFRINGEMENT OF THIRD PARTIES’ INTELLECTUAL PROPERTY RIGHTS OR THE
 * MERCHANTABILITY OR FITNESS FOR YOUR INTENDED PURPOSE OR USAGE. WÜRTH ELEKTRONIK EISOS DOES NOT
 * WARRANT OR REPRESENT THAT ANY LICENSE, EITHER EXPRESS OR IMPLIED, IS GRANTED UNDER ANY PATENT
 * RIGHT, COPYRIGHT, MASK WORK RIGHT, OR OTHER INTELLECTUAL PROPERTY RIGHT RELATING TO ANY
 * COMBINATION, MACHINE, OR PROCESS IN WHICH THE PRODUCT IS USED. INFORMATION PUBLISHED BY
 * WÜRTH ELEKTRONIK EISOS REGARDING THIRD-PARTY PRODUCTS OR SERVICES DOES NOT CONSTITUTE A LICENSE
 * FROM WÜRTH ELEKTRONIK EISOS TO USE SUCH PRODUCTS OR SERVICES OR A WARRANTY OR ENDORSEMENT
 * THEREOF
 *
 * THIS SOURCE CODE IS PROTECTED BY A LICENSE.
 * FOR MORE INFORMATION PLEASE CAREFULLY READ THE LICENSE AGREEMENT FILE LOCATED
 * IN THE ROOT DIRECTORY OF THIS DRIVER PACKAGE.
 *
 * COPYRIGHT (c) 2023 Würth Elektronik eiSos GmbH & Co. KG
 *
 ***************************************************************************************************
 */

/**
 * @file
 * @brief AT commands for file I/O functionality.
 */

#ifndef AT_FILE_H_INCLUDED
#define AT_FILE_H_INCLUDED

#include <ATCommands.h>
#include <Calypso.h>
#include <stdbool.h>
#include <stdint.h>


#define ATFILE_FILENAME_MAX_LENGTH          (uint8_t)  180          /**< Maximum file name length */
#define ATFILE_FILE_MIN_SIZE                (uint16_t) 4096         /**< Minimum file size in bytes */
#define ATFILE_FILE_FAILSAFE_MIN_SIZE       (uint16_t) 8192         /**< Failsafe minimum file size in bytes */
#define ATFILE_FILE_MAX_CHUNK_SIZE          (uint16_t) 750          /**< Max. recommended data size to transfer in one go
                                                                         (using ATFile_Write() and ATFile_Read()). Is limited
                                                                         to 750 bytes because of issues when using Base64 encoding. */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Open flags for AT+fileOpen command.
 */
typedef enum ATFile_OpenFlags_t
{
    ATFile_OpenFlags_Create                  = 1 << 0,       /**< Create a new file (optionally combined with ATFile_OpenFlags_Write or ATFile_OpenFlags_Overwrite) */
    ATFile_OpenFlags_Read                    = 1 << 1,       /**< Open file for reading (no bitmask) */
    ATFile_OpenFlags_Write                   = 1 << 2,       /**< Open file for writing (optionally combined with ATFile_OpenFlags_Create) */
    ATFile_OpenFlags_Overwrite               = 1 << 3,       /**< Opens an existing file (optionally combined with ATFile_OpenFlags_Create) */
    ATFile_OpenFlags_CreateFailsafe          = 1 << 4,       /**< Fail safe */
    ATFile_OpenFlags_CreateSecure            = 1 << 5,       /**< Secure file */
    ATFile_OpenFlags_CreateNoSignature       = 1 << 6,       /**< For secure files only */
    ATFile_OpenFlags_CreateStaticToken       = 1 << 7,       /**< For secure files only */
    ATFile_OpenFlags_CreateVendorToken       = 1 << 8,       /**< For secure files only */
    ATFile_OpenFlags_CreatePublicWrite       = 1 << 9,       /**< For secure files only */
    ATFile_OpenFlags_CreatePublicRead        = 1 << 10,      /**< For secure files only */
    ATFile_OpenFlags_NumberOfValues          = 11            /**< Number of flags in this enumeration */
} ATFile_OpenFlags_t;

/**
 * @brief File property flags used in file list entries.
 *
 * @see ATFile_FileListEntry_t, ATFile_GetFileList()
 */
typedef enum ATFile_FileProperties_t
{
    ATFile_FileProperties_OpenWrite             = 1 << 0,
    ATFile_FileProperties_OpenRead              = 1 << 1,
    ATFile_FileProperties_MustCommit            = 1 << 2,
    ATFile_FileProperties_BundleFile            = 1 << 3,
    ATFile_FileProperties_PendingCommit         = 1 << 4,
    ATFile_FileProperties_PendingBundleCommit   = 1 << 5,
    ATFile_FileProperties_NotFailsafe           = 1 << 6,
    ATFile_FileProperties_NotValid              = 1 << 7,
    ATFile_FileProperties_SysFile               = 1 << 8,
    ATFile_FileProperties_Secure                = 1 << 9,
    ATFile_FileProperties_NoSignature           = 1 << 10,
    ATFile_FileProperties_PublicWrite           = 1 << 11,
    ATFile_FileProperties_PublicRead            = 1 << 12,
    ATFile_FileProperties_NumberOfValues        = 13
} ATFile_FileProperties_t;

/**
 * @brief File info as returned by ATFile_GetInfo()
 */
typedef struct ATFile_FileInfo_t
{
    uint32_t size;
    uint32_t allocatedSize;
    uint32_t storageSize;
    uint32_t writeCounter;
} ATFile_FileInfo_t;

/**
 * @brief File list entry used with ATFile_GetFileList() and ATEvent_FileListEntry.
 */
typedef struct ATFile_FileListEntry_t
{
    char fileName[ATFILE_FILENAME_MAX_LENGTH];          /**< File name */
    uint32_t maxFileSize;                               /**< Max. size of file */
    uint32_t properties;                                /**< File properties (see ATFile_FileProperties_t)  */
    uint32_t allocatedBlocks;                           /**< Allocated blocks */
} ATFile_FileListEntry_t;


extern bool ATFile_Open(const char *fileName,
                        uint32_t options,
                        uint16_t fileSize,
                        uint32_t *fileID,
                        uint32_t *secureToken);
extern bool ATFile_Close(uint32_t fileID, char *certFileName, char *signature);
extern bool ATFile_Delete(const char *fileName, uint32_t secureToken);
extern bool ATFile_Read(uint32_t fileID,
                        uint16_t offset,
                        Calypso_DataFormat_t format,
                        bool decodeBase64,
                        uint16_t bytesToRead,
                        char *data,
                        uint16_t *bytesRead);
extern bool ATFile_Write(uint32_t fileID,
                         uint16_t offset,
                         Calypso_DataFormat_t format,
                         bool encodeAsBase64,
                         uint16_t bytesToWrite,
                         const char *data,
                         uint16_t *bytesWritten);
extern bool ATFile_GetInfo(const char *fileName,
                           uint32_t secureToken,
                           ATFile_FileInfo_t* fileInfo);
extern bool ATFile_GetFileList();
extern bool ATFile_ParseFileListEntry(char **pInArguments, ATFile_FileListEntry_t* fileListEntry);
extern bool ATFile_PrintFileProperties(uint32_t properties, char *pOutStr, size_t maxLength);


#ifdef __cplusplus
}
#endif

#endif /* AT_FILE_H_INCLUDED */
