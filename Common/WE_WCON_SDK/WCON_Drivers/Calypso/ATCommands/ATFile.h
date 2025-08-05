/*
 ***************************************************************************************************
 * This file is part of WIRELESS CONNECTIVITY SDK:
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
 * COPYRIGHT (c) 2025 Würth Elektronik eiSos GmbH & Co. KG
 *
 ***************************************************************************************************
 */

/**
 * @file
 * @brief AT commands for file I/O functionality.
 */

#ifndef CALYPSO_AT_FILE_H_INCLUDED
#define CALYPSO_AT_FILE_H_INCLUDED
#include <Calypso/Calypso.h>
#include <global/ATCommands.h>
#include <stdbool.h>
#include <stdint.h>

#define ATFILE_FILENAME_MAX_LENGTH (uint8_t)180      /**< Maximum file name length */
#define ATFILE_FILE_MIN_SIZE (uint16_t)4096          /**< Minimum file size in bytes */
#define ATFILE_FILE_FAILSAFE_MIN_SIZE (uint16_t)8192 /**< Failsafe minimum file size in bytes */
#define ATFILE_FILE_MAX_CHUNK_SIZE                                                                                                                                                                                                                                                                                                                                                                             \
    (uint16_t)750 /**< Max. recommended data size to transfer in one go
                                                                         (using Calypso_ATFile_Write() and Calypso_ATFile_Read()). Is limited
                                                                         to 750 bytes because of issues when using Base64 encoding. */

#ifdef __cplusplus
extern "C"
{
#endif

    /**
 * @brief Open flags for AT+fileOpen command.
 */
    typedef enum Calypso_ATFile_OpenFlags_t
    {
        Calypso_ATFile_OpenFlags_Create = 1 << 0,            /**< Create a new file (optionally combined with Calypso_ATFile_OpenFlags_Write or Calypso_ATFile_OpenFlags_Overwrite) */
        Calypso_ATFile_OpenFlags_Read = 1 << 1,              /**< Open file for reading (no bitmask) */
        Calypso_ATFile_OpenFlags_Write = 1 << 2,             /**< Open file for writing (optionally combined with Calypso_ATFile_OpenFlags_Create) */
        Calypso_ATFile_OpenFlags_Overwrite = 1 << 3,         /**< Opens an existing file (optionally combined with Calypso_ATFile_OpenFlags_Create) */
        Calypso_ATFile_OpenFlags_CreateFailsafe = 1 << 4,    /**< Fail safe */
        Calypso_ATFile_OpenFlags_CreateSecure = 1 << 5,      /**< Secure file */
        Calypso_ATFile_OpenFlags_CreateNoSignature = 1 << 6, /**< For secure files only */
        Calypso_ATFile_OpenFlags_CreateStaticToken = 1 << 7, /**< For secure files only */
        Calypso_ATFile_OpenFlags_CreateVendorToken = 1 << 8, /**< For secure files only */
        Calypso_ATFile_OpenFlags_CreatePublicWrite = 1 << 9, /**< For secure files only */
        Calypso_ATFile_OpenFlags_CreatePublicRead = 1 << 10, /**< For secure files only */
        Calypso_ATFile_OpenFlags_NumberOfValues = 11         /**< Number of flags in this enumeration */
    } Calypso_ATFile_OpenFlags_t;

    /**
 * @brief File property flags used in file list entries.
 *
 * @see Calypso_ATFile_FileListEntry_t, Calypso_ATFile_GetFileList()
 */
    typedef enum Calypso_ATFile_FileProperties_t
    {
        Calypso_ATFile_FileProperties_OpenWrite = 1 << 0,
        Calypso_ATFile_FileProperties_OpenRead = 1 << 1,
        Calypso_ATFile_FileProperties_MustCommit = 1 << 2,
        Calypso_ATFile_FileProperties_BundleFile = 1 << 3,
        Calypso_ATFile_FileProperties_PendingCommit = 1 << 4,
        Calypso_ATFile_FileProperties_PendingBundleCommit = 1 << 5,
        Calypso_ATFile_FileProperties_NotFailsafe = 1 << 6,
        Calypso_ATFile_FileProperties_NotValid = 1 << 7,
        Calypso_ATFile_FileProperties_SysFile = 1 << 8,
        Calypso_ATFile_FileProperties_Secure = 1 << 9,
        Calypso_ATFile_FileProperties_NoSignature = 1 << 10,
        Calypso_ATFile_FileProperties_PublicWrite = 1 << 11,
        Calypso_ATFile_FileProperties_PublicRead = 1 << 12,
        Calypso_ATFile_FileProperties_NumberOfValues = 13
    } Calypso_ATFile_FileProperties_t;

    /**
 * @brief File info as returned by Calypso_ATFile_GetInfo()
 */
    typedef struct Calypso_ATFile_FileInfo_t
    {
        uint32_t size;
        uint32_t allocatedSize;
        uint32_t storageSize;
        uint32_t writeCounter;
    } Calypso_ATFile_FileInfo_t;

    /**
 * @brief File list entry used with Calypso_ATFile_GetFileList() and Calypso_ATEvent_FileListEntry.
 */
    typedef struct Calypso_ATFile_FileListEntry_t
    {
        char fileName[ATFILE_FILENAME_MAX_LENGTH]; /**< File name */
        uint32_t maxFileSize;                      /**< Max. size of file */
        uint32_t properties;                       /**< File properties (see Calypso_ATFile_FileProperties_t)  */
        uint32_t allocatedBlocks;                  /**< Allocated blocks */
    } Calypso_ATFile_FileListEntry_t;

    extern bool Calypso_ATFile_Open(const char* fileName, uint32_t options, uint16_t fileSize, uint32_t* fileID, uint32_t* secureToken);
    extern bool Calypso_ATFile_Close(uint32_t fileID, char* certFileName, char* signature);
    extern bool Calypso_ATFile_Delete(const char* fileName, uint32_t secureToken);
    extern bool Calypso_ATFile_Read(uint32_t fileID, uint16_t offset, Calypso_DataFormat_t format, bool decodeBase64, uint16_t bytesToRead, char* data, uint16_t* bytesRead);
    extern bool Calypso_ATFile_Write(uint32_t fileID, uint16_t offset, Calypso_DataFormat_t format, bool encodeAsBase64, uint16_t bytesToWrite, const char* data, uint16_t* bytesWritten);
    extern bool Calypso_ATFile_GetInfo(const char* fileName, uint32_t secureToken, Calypso_ATFile_FileInfo_t* fileInfo);
    extern bool Calypso_ATFile_GetFileList();
    extern bool Calypso_ATFile_ParseFileListEntry(char** pInArguments, Calypso_ATFile_FileListEntry_t* fileListEntry);
    extern bool Calypso_ATFile_PrintFileProperties(uint32_t properties, char* pOutStr, size_t maxLength);

#ifdef __cplusplus
}
#endif

#endif /* CALYPSO_AT_FILE_H_INCLUDED */
