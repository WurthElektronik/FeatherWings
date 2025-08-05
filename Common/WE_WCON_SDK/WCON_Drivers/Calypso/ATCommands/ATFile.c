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
#include <Calypso/ATCommands/ATFile.h>
#include <global/ATCommands.h>
#include <global/global.h>

static const char* Calypso_ATFile_OpenFlags_Strings[Calypso_ATFile_OpenFlags_NumberOfValues] = {"CREATE", "READ", "WRITE", "OVERWRITE", "CREATE_FAILSAFE", "CREATE_SECURE", "CREATE_NOSIGNATURE", "CREATE_STATIC_TOKEN", "CREATE_VENDOR_TOKEN", "CREATE_PUBLIC_WRITE", "CREATE_PUBLIC_READ"};

static const char* Calypso_ATFile_FileProperties_Strings[Calypso_ATFile_FileProperties_NumberOfValues] = {"open_write", "open_read", "must_commit", "bundle_file", "pending_commit", "pending_bundle_commit", "not_failsafe", "not_valid", "sys_file", "secure", "nosignature", "public_write", "public_read"};

static bool Calypso_ATFile_AddArgumentsFileOpen(char* pAtCommand, const char* fileName, uint32_t options, uint16_t fileSize);
static bool Calypso_ATFile_AddArgumentsFileClose(char* pAtCommand, uint32_t fileID, const char* certName, const char* signature);
static bool Calypso_ATFile_AddArgumentsFileDel(char* pAtCommand, const char* fileName, uint32_t secureToken);
static bool Calypso_ATFile_AddArgumentsFileRead(char* pAtCommand, uint32_t fileID, uint16_t offset, Calypso_DataFormat_t format, uint16_t bytesToRead);
static bool Calypso_ATFile_AddArgumentsFileWrite(char* pAtCommand, uint32_t fileID, uint16_t offset, Calypso_DataFormat_t format, uint16_t bytesToWrite, const char* data);

static bool Calypso_ATFile_ParseResponseFileOpen(char** pAtCommand, uint32_t* fileID, uint32_t* secureToken);
static bool Calypso_ATFile_ParseResponseFileRead(char** pAtCommand, uint16_t bytesToRead, bool decodeBase64, uint16_t* bytesRead, char* data);
static bool Calypso_ATFile_ParseResponseFileWrite(char** pAtCommand, uint16_t* bytesWritten);

/**
 * @brief Opens a file (using the AT+FileOpen command).
 *
 * @param[in] fileName Name of file to be opened
 * @param[in] options Option flags (see Calypso_ATFile_OpenFlags_t)
 * @param[in] fileSize Maximum size of the file. Will be allocated on creation.
 * @param[out] fileID ID of the opened file. Can be used for further file functions.
 * @param[out] secureToken Secure token of the opened file.
 *
 * @return true if successful, false otherwise
 */
bool Calypso_ATFile_Open(const char* fileName, uint32_t options, uint16_t fileSize, uint32_t* fileID, uint32_t* secureToken)
{

    char* pRequestCommand = AT_commandBuffer;
    char* pRespondCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+fileOpen=");

    if (!Calypso_ATFile_AddArgumentsFileOpen(pRequestCommand, fileName, options, fileSize))
    {
        return false;
    }

    if (!Calypso_SendRequest(pRequestCommand))
    {
        return false;
    }
    if (!Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_General), Calypso_CNFStatus_Success, pRespondCommand))
    {
        return false;
    }

    return Calypso_ATFile_ParseResponseFileOpen(&pRespondCommand, fileID, secureToken);
}

/**
 * @brief Closes a file (using the AT+FileClose command).
 *
 * @param[in] fileID File ID as returned by Calypso_ATFile_Open
 * @param[in] certFileName Full path to certificate (optional). Can be NULL if not used.
 * @param[in] signature SHA1 signature (optional). Can be NULL if not used.
 *
 * @return true if successful, false otherwise
 */
bool Calypso_ATFile_Close(uint32_t fileID, char* certFileName, char* signature)
{

    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+fileClose=");

    if (!Calypso_ATFile_AddArgumentsFileClose(pRequestCommand, fileID, certFileName, signature))
    {
        return false;
    }

    if (!Calypso_SendRequest(pRequestCommand))
    {
        return false;
    }

    return Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_General), Calypso_CNFStatus_Success, NULL);
}

/**
 * @brief Deletes a file (using the AT+FileDel command).
 *
 * @param[in] fileName Name of the file to delete
 * @param[in] secureToken Secure token returned by Calypso_ATFile_Open() (optional)
 *
 * @return true if successful, false otherwise
 */
bool Calypso_ATFile_Delete(const char* fileName, uint32_t secureToken)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+fileDel=");

    if (!Calypso_ATFile_AddArgumentsFileDel(pRequestCommand, fileName, secureToken))
    {
        return false;
    }

    if (!Calypso_SendRequest(pRequestCommand))
    {
        return false;
    }
    return Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_General), Calypso_CNFStatus_Success, NULL);
}

/**
 * @brief Reads a file's contents (using the AT+FileRead command).
 *
 * The file has to be opened using Calypso_ATFile_Open() prior to reading!
 *
 * @param[in] fileID ID of file to read as returned by Calypso_ATFile_Open()
 * @param[in] offset Offset for the read operation
 * @param[in] format Format of the output data. When using Calypso_DataFormat_Base64,
 *                   you can set decodeBase64 to true to automatically decode the received data.
 * @param[in] decodeBase64 Enables decoding of received Base64 data
 * @param[in] bytesToRead Number of bytes to read. Note that the returned data is a
 *                        null terminated string, so the output buffer must have a
 *                        minimum size of bytesToRead+1.
 * @param[out] bytesRead Number of bytes which have been read
 * @param[out] data File contents which has been read
 *
 * @return true if successful, false otherwise
 */
bool Calypso_ATFile_Read(uint32_t fileID, uint16_t offset, Calypso_DataFormat_t format, bool decodeBase64, uint16_t bytesToRead, char* data, uint16_t* bytesRead)
{
    *bytesRead = 0;

    uint16_t chunkSize = 0;
    for (uint16_t chunkOffset = 0; chunkOffset < bytesToRead; chunkOffset += chunkSize)
    {
        chunkSize = bytesToRead - chunkOffset;
        if (chunkSize > ATFILE_FILE_MAX_CHUNK_SIZE)
        {
            chunkSize = ATFILE_FILE_MAX_CHUNK_SIZE;
        }

        char* pRequestCommand = AT_commandBuffer;
        char* pRespondCommand = AT_commandBuffer;

        strcpy(pRequestCommand, "AT+fileRead=");

        if (!Calypso_ATFile_AddArgumentsFileRead(pRequestCommand, fileID, offset + chunkOffset, format, chunkSize))
        {
            return false;
        }

        if (!Calypso_SendRequest(pRequestCommand))
        {
            return false;
        }
        if (!Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_FileIO), Calypso_CNFStatus_Success, pRespondCommand))
        {
            return false;
        }

        uint16_t chunkBytesRead = 0;
        if (!Calypso_ATFile_ParseResponseFileRead(&pRespondCommand, chunkSize, decodeBase64, &chunkBytesRead, data + chunkOffset))
        {
            return false;
        }

        *bytesRead += chunkBytesRead;

    }

    return true;
}

/**
 * @brief Writes a file's contents (using the AT+FileWrite command).
 *
 * The file has to be opened using Calypso_ATFile_Open() prior to writing!
 *
 * @param[in] fileID ID of file to write as returned by Calypso_ATFile_Open()
 * @param[in] offset Offset for the write operation
 * @param[in] format Format of the data to be written. Note that when using Calypso_DataFormat_Base64,
 *                   you either need to provide Base64 encoded data or set encodeAsBase64 to true.
 * @param[in] encodeAsBase64 Encode the data in Base64 format before sending it to the Calypso module
 * @param[in] bytestoWrite Number of bytes to write
 * @param[in] data Data to be written
 * @param[out] bytesWritten Number of bytes which have been written
 *
 * @return true if successful, false otherwise
 */
bool Calypso_ATFile_Write(uint32_t fileID, uint16_t offset, Calypso_DataFormat_t format, bool encodeAsBase64, uint16_t bytesToWrite, const char* data, uint16_t* bytesWritten)
{
    *bytesWritten = 0;

    if (encodeAsBase64)
    {
        /* Base64 encoded data might exceed the max. chunk size. To limit the required buffer size,
		 * the data is encoded in chunks, if necessary. */

        uint16_t maxChunkSize = (((ATFILE_FILE_MAX_CHUNK_SIZE - 1) * 3) / 4) - 2;
        uint16_t chunkSize = 0;
        for (uint16_t chunkOffset = 0; chunkOffset < bytesToWrite; chunkOffset += chunkSize)
        {
            chunkSize = bytesToWrite - chunkOffset;
            if (chunkSize > maxChunkSize)
            {
                chunkSize = maxChunkSize;
            }

            /* Encode as Base64 */
            uint32_t lengthEncoded;
            if (!Base64_GetEncBufSize(chunkSize, &lengthEncoded))
            {
                return false;
            }

            char base64Buffer[lengthEncoded];
            if (!Base64_Encode((uint8_t*)data + chunkOffset, chunkSize, (uint8_t*)base64Buffer, &lengthEncoded))
            {
                return false;
            }

            /* Recursively call Calypso_ATFile_Write() with the encoded binary data */
            uint16_t chunkBytesWritten = 0;
            bool ok = Calypso_ATFile_Write(fileID, offset + chunkOffset, format, false, lengthEncoded, base64Buffer, &chunkBytesWritten);
            if (!ok)
            {
                return false;
            }
            *bytesWritten += chunkSize;
        }

        return true;
    }

    uint16_t chunkBytesWritten = 0;
    for (uint16_t chunkOffset = 0; chunkOffset < bytesToWrite; chunkOffset += chunkBytesWritten)
    {
        uint16_t chunkSize = bytesToWrite - chunkOffset;
        if (chunkSize > ATFILE_FILE_MAX_CHUNK_SIZE)
        {
            chunkSize = ATFILE_FILE_MAX_CHUNK_SIZE;
        }

        char* pRequestCommand = AT_commandBuffer;
        char* pRespondCommand = AT_commandBuffer;

        strcpy(pRequestCommand, "AT+fileWrite=");

        if (!Calypso_ATFile_AddArgumentsFileWrite(pRequestCommand, fileID, offset + chunkOffset, format, chunkSize, data + chunkOffset))
        {
            return false;
        }

        if (!Calypso_SendRequest(pRequestCommand))
        {
            return false;
        }
        if (!Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_FileIO), Calypso_CNFStatus_Success, pRespondCommand))
        {
            return false;
        }

        if (!Calypso_ATFile_ParseResponseFileWrite(&pRespondCommand, &chunkBytesWritten))
        {
            return false;
        }

        *bytesWritten += chunkBytesWritten;

    }

    return true;
}

/**
 * @brief Returns information on a file.
 *
 * @param[in] fileName Name of file
 * @param[in] secureToken Secure token
 * @param[out] fileInfo The returned file information
 *
 * @return true if successful, false otherwise
 */
bool Calypso_ATFile_GetInfo(const char* fileName, uint32_t secureToken, Calypso_ATFile_FileInfo_t* fileInfo)
{
    char* pRequestCommand = AT_commandBuffer;
    char* pRespondCommand = AT_commandBuffer;

    strcpy(AT_commandBuffer, "AT+fileGetInfo=");

    if (!ATCommand_AppendArgumentString(pRequestCommand, fileName, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, secureToken, ATCOMMAND_INTFLAGS_SIZE32 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!Calypso_SendRequest(pRequestCommand))
    {
        return false;
    }
    if (!Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_General), Calypso_CNFStatus_Success, pRespondCommand))
    {
        return false;
    }

    const char* cmd = "+filegetinfo:";
    const size_t cmdLength = strlen(cmd);

    if (0 != strncmp(pRespondCommand, cmd, cmdLength))
    {
        return false;
    }
    pRespondCommand += cmdLength;

    char temp[128];

    /* Discard flags */
    if (!ATCommand_GetNextArgumentString(&pRespondCommand, temp, ATCOMMAND_ARGUMENT_DELIM, sizeof(temp)))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentInt(&pRespondCommand, &fileInfo->size, ATCOMMAND_INTFLAGS_SIZE32 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentInt(&pRespondCommand, &fileInfo->allocatedSize, ATCOMMAND_INTFLAGS_SIZE32 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    /* Discard tokens */
    if (!ATCommand_GetNextArgumentString(&pRespondCommand, temp, ATCOMMAND_ARGUMENT_DELIM, sizeof(temp)))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentInt(&pRespondCommand, &fileInfo->storageSize, ATCOMMAND_INTFLAGS_SIZE32 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentInt(&pRespondCommand, &fileInfo->writeCounter, ATCOMMAND_INTFLAGS_SIZE32 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    return true;
}

/**
 * @brief Retrieves a list of the files stored in the Calypso module.
 *
 * The file list entries are made available in the form of events of type
 * Calypso_ATEvent_FileListEntry as soon as they are received (the complete list of files
 * may be too large to be returned by this function as a whole). The events are
 * triggered before this function returns.
 *
 * @return true if successful, false otherwise
 */
bool Calypso_ATFile_GetFileList()
{
    if (!Calypso_SendRequest("AT+fileGetFileList\r\n"))
    {
        return false;
    }
    return Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_FileIO), Calypso_CNFStatus_Success, NULL);
}

/**
 * @brief Parses the values of a file list entry string.
 *
 * @param[in,out] pInArguments String containing arguments of the AT command
 * @param[out] fileListEntry The parsed file list entry data
 *
 * @return true if parsed successfully, false otherwise
 */
bool Calypso_ATFile_ParseFileListEntry(char** pInArguments, Calypso_ATFile_FileListEntry_t* fileListEntry)
{
    if (!ATCommand_GetNextArgumentString(pInArguments, fileListEntry->fileName, ATCOMMAND_ARGUMENT_DELIM, sizeof(fileListEntry->fileName)))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentInt(pInArguments, &fileListEntry->maxFileSize, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE32 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentBitmask(pInArguments, Calypso_ATFile_FileProperties_Strings, Calypso_ATFile_FileProperties_NumberOfValues, 25, &fileListEntry->properties, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    return ATCommand_GetNextArgumentInt(pInArguments, &fileListEntry->allocatedBlocks, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE32 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE);
}

/**
 * @brief Prints file property flags to string.
 *
 * @param[in] properties File properties (see Calypso_ATFile_FileProperties_t)
 * @param[out] pOutStr Output string
 * @param[in] maxLength Max. length of output string
 *
 * @return true if successful, false otherwise
 */
bool Calypso_ATFile_PrintFileProperties(uint32_t properties, char* pOutStr, size_t maxLength) { return ATCommand_AppendArgumentBitmask(pOutStr, Calypso_ATFile_FileProperties_Strings, Calypso_ATFile_FileProperties_NumberOfValues, properties, ATCOMMAND_STRING_TERMINATE, maxLength); }

/**
 * @brief Adds arguments to the AT+fileOpen command string.
 *
 * @param[in] pAtCommand The AT command string to add the arguments to
 * @param[in] fileName Name of file to be opened
 * @param[in] options Option flags
 * @param[in] fileSize Maximum size of the file. Will be allocated on creation.
 *
 * @return true if successful, false otherwise
 */
static bool Calypso_ATFile_AddArgumentsFileOpen(char* pAtCommand, const char* fileName, uint32_t options, uint16_t fileSize)
{

    if ((NULL == pAtCommand) || (NULL == fileName))
    {
        return false;
    }

    if (strlen(fileName) > ATFILE_FILENAME_MAX_LENGTH || (0 != (options & Calypso_ATFile_OpenFlags_Create) && ATFILE_FILE_MIN_SIZE > fileSize))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentString(pAtCommand, fileName, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentBitmask(pAtCommand, Calypso_ATFile_OpenFlags_Strings, Calypso_ATFile_OpenFlags_NumberOfValues, options, ATCOMMAND_ARGUMENT_DELIM, AT_MAX_COMMAND_BUFFER_SIZE))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentInt(pAtCommand, fileSize, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    return ATCommand_AppendArgumentString(pAtCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE);
}

/**
 * @brief Adds arguments to the AT+fileClose command string.
 *
 * @param[in] pAtCommand The AT command string to add the arguments to
 * @param[in] fileID ID of opened file as returned by Calypso_ATFile_Open()
 * @param[in] certName Full path to certificate (optional). Can be NULL if not used.
 * @param[in] signature SHA1 signature (optional). Can be NULL if not used.
 *
 * @return true if successful, false otherwise
 */
static bool Calypso_ATFile_AddArgumentsFileClose(char* pAtCommand, uint32_t fileID, const char* certName, const char* signature)
{

    if (NULL == pAtCommand)
    {
        return false;
    }

    if (!ATCommand_AppendArgumentInt(pAtCommand, fileID, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentString(pAtCommand, certName, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pAtCommand, signature, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    return ATCommand_AppendArgumentString(pAtCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE);
}

/**
 * @brief Adds arguments to the AT+fileDel command string
 *
 * @param[in] pAtCommand The AT command string to add the arguments to
 * @param[in] fileName Name of the file to delete
 * @param[in] secureToken Secure token returned by Calypso_ATFile_Open() (optional)
 *
 * @return true if successful, false otherwise
 */
static bool Calypso_ATFile_AddArgumentsFileDel(char* pAtCommand, const char* fileName, uint32_t secureToken)
{

    if (NULL == pAtCommand)
    {
        return false;
    }

    if (!ATCommand_AppendArgumentString(pAtCommand, fileName, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentInt(pAtCommand, secureToken, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    return ATCommand_AppendArgumentString(pAtCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE);
}

/**
 * @brief Adds arguments to the AT+fileRead command string
 *
 * @param[in] pAtCommand The AT command string to add the arguments to
 * @param[in] fileID ID of file to read as returned by Calypso_ATFile_Open()
 * @param[in] offset Offset for the read operation
 * @param[in] format Format of the output data.
 * @param[in] bytesToRead Number of bytes to read
 *
 * @return true if successful, false otherwise
 */
static bool Calypso_ATFile_AddArgumentsFileRead(char* pAtCommand, uint32_t fileID, uint16_t offset, Calypso_DataFormat_t format, uint16_t bytesToRead)
{

    if (NULL == pAtCommand)
    {
        return false;
    }

    if (!ATCommand_AppendArgumentInt(pAtCommand, fileID, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentInt(pAtCommand, offset, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentInt(pAtCommand, format, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentInt(pAtCommand, bytesToRead, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    return ATCommand_AppendArgumentString(pAtCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE);
}

/**
 * @brief Adds arguments to the AT+fileWrite command string
 *
 * @param[in] pAtCommand The AT command string to add the arguments to
 * @param[in] fileID ID of file to write as returned by Calypso_ATFile_Open()
 * @param[in] offset Offset for the write operation
 * @param[in] format Format of the data to be written.
 * @param[in] bytestoWrite Number of bytes to write
 * @param[in] data Data to be written
 *
 * @return true if successful, false otherwise
 */
static bool Calypso_ATFile_AddArgumentsFileWrite(char* pAtCommand, uint32_t fileID, uint16_t offset, Calypso_DataFormat_t format, uint16_t bytesToWrite, const char* data)
{

    if (NULL == pAtCommand)
    {
        return false;
    }

    if (!ATCommand_AppendArgumentInt(pAtCommand, fileID, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentInt(pAtCommand, offset, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentInt(pAtCommand, format, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentInt(pAtCommand, bytesToWrite, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentBytes(pAtCommand, data, bytesToWrite, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    return ATCommand_AppendArgumentString(pAtCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE);
}

/**
 * @brief Parses the response of a AT+fileOpen command
 *
 * @param[in,out] pAtCommand  The string response by the module
 * @param[out] fileID ID of the opened file. Can be used for further file functions.
 * @param[out] secureToken Secure token of the opened file.
 *
 * @return true if successful, false otherwise
 */
static bool Calypso_ATFile_ParseResponseFileOpen(char** pAtCommand, uint32_t* fileID, uint32_t* secureToken)
{
    const char* cmd = "+fileopen:";
    const size_t cmdLength = strlen(cmd);

    if (0 != strncmp(*pAtCommand, cmd, cmdLength))
    {
        return false;
    }
    *pAtCommand += cmdLength;
    if (!ATCommand_GetNextArgumentInt(pAtCommand, fileID, ATCOMMAND_INTFLAGS_SIZE32 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    return ATCommand_GetNextArgumentInt(pAtCommand, secureToken, ATCOMMAND_INTFLAGS_SIZE32 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE);
}

/**
 * @brief Parses the response of a AT+fileRead command
 *
 * Note that the data is returned as a null terminated string,
 * so the destination buffer should have size bytesToRead+1.
 *
 * @param[in,out] pAtCommand The string response by the module
 * @param[in] decodeBase64 Enables decoding of received Base64 data
 * @param[out] bytesRead Number of bytes which have been read
 * @param[in] bytesToRead Number of bytes to read. Note that the returned data is a
 *                        null terminated string, so the output buffer must have a
 *                        minimum size of bytesToRead+1.
 * @param[out] data File content which has been read
 *
 * @return true if successful, false otherwise
 */
static bool Calypso_ATFile_ParseResponseFileRead(char** pAtCommand, uint16_t bytesToRead, bool decodeBase64, uint16_t* bytesRead, char* data)
{
    const char* cmd = "+fileread:";
    const size_t cmdLength = strlen(cmd);

    if ((0 != strncmp(*pAtCommand, cmd, cmdLength)) || (bytesRead == NULL) || (data == NULL))
    {
        return false;
    }

    *pAtCommand += cmdLength;

    uint8_t outFormat;
    if (!ATCommand_GetNextArgumentInt(pAtCommand, &outFormat, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(pAtCommand, bytesRead, ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (*bytesRead > 0)
    {
        if (decodeBase64)
        {
            uint32_t decodedSize;
            if (!Base64_GetDecBufSize((uint8_t*)*pAtCommand, *bytesRead, &decodedSize))
            {
                return false;
            }

            /* we are expecting only 'bytesToRead' of decoded data */
            if (decodedSize > bytesToRead)
            {
                return false;
            }

            decodedSize = bytesToRead;
            if (!Base64_Decode((uint8_t*)*pAtCommand, *bytesRead, (uint8_t*)data, &decodedSize))
            {
                return false;
            }
            /* add string termination character needed by the Calypso functions */
            data[decodedSize] = '\0';

            *bytesRead = decodedSize;
            return true;
        }
        else
        {
            if (*bytesRead > bytesToRead)
            {
                return false;
            }
            return ATCommand_GetNextArgumentString(pAtCommand, data, ATCOMMAND_STRING_TERMINATE, *bytesRead + 1);
        }
    }

    return false;
}

/**
 * @brief Parses the response of a AT+fileWrite command
 *
 * @param[in,out] pAtCommand The string response by the module
 * @param[out] bytesWritten Number of bytes which have been written
 *
 * @return true if successful, false otherwise
 */
static bool Calypso_ATFile_ParseResponseFileWrite(char** pAtCommand, uint16_t* bytesWritten)
{
    const char* cmd = "+filewrite:";
    const size_t cmdLength = strlen(cmd);

    if (0 != strncmp(*pAtCommand, cmd, cmdLength))
    {
        return false;
    }
    *pAtCommand += cmdLength;
    return ATCommand_GetNextArgumentInt(pAtCommand, bytesWritten, ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE);
}
