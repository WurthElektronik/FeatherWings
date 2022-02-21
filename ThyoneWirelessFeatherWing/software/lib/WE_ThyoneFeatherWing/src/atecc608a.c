/**
 * \file
 * \brief File for the ATECC608A secure element.
 *
 * \copyright (c) 2020 Würth Elektronik eiSos GmbH & Co. KG
 *
 * \page License
 *
 * THE SOFTWARE INCLUDING THE SOURCE CODE IS PROVIDED “AS IS”. YOU ACKNOWLEDGE
 * THAT WÜRTH ELEKTRONIK EISOS MAKES NO REPRESENTATIONS AND WARRANTIES OF ANY
 * KIND RELATED TO, BUT NOT LIMITED TO THE NON-INFRINGEMENT OF THIRD PARTIES’
 * INTELLECTUAL PROPERTY RIGHTS OR THE MERCHANTABILITY OR FITNESS FOR YOUR
 * INTENDED PURPOSE OR USAGE. WÜRTH ELEKTRONIK EISOS DOES NOT WARRANT OR
 * REPRESENT THAT ANY LICENSE, EITHER EXPRESS OR IMPLIED, IS GRANTED UNDER ANY
 * PATENT RIGHT, COPYRIGHT, MASK WORK RIGHT, OR OTHER INTELLECTUAL PROPERTY
 * RIGHT RELATING TO ANY COMBINATION, MACHINE, OR PROCESS IN WHICH THE PRODUCT
 * IS USED. INFORMATION PUBLISHED BY WÜRTH ELEKTRONIK EISOS REGARDING
 * THIRD-PARTY PRODUCTS OR SERVICES DOES NOT CONSTITUTE A LICENSE FROM WÜRTH
 * ELEKTRONIK EISOS TO USE SUCH PRODUCTS OR SERVICES OR A WARRANTY OR
 * ENDORSEMENT THEREOF
 *
 * THIS SOURCE CODE IS PROTECTED BY A LICENSE.
 * FOR MORE INFORMATION PLEASE CAREFULLY READ THE LICENSE AGREEMENT FILE LOCATED
 * IN THE ROOT DIRECTORY OF THIS PACKAGE
 */

#include "atecc608a.h"
#include "cryptoauthlib.h"
#include "atcacert/atcacert.h"
#include "atcacert/atcacert_client.h"
#include "atcacert/atcacert_pem.h"
#include "app/tng/tngtls_cert_def_1_signer.h"
#include "app/tng/tngtls_cert_def_3_device.h"

uint8_t subject_key_id[20];
static const char *bin2hex(const void *data, size_t data_size);

/**
 * @brief  Initialize secure element and read the serial number
 * @param  SerialDebug Pointer to the debug port.
 * @retval true if successful false in case of failure
 */
bool atecc608a_init(TypeSerial *SerialDebug)
{
    uint8_t status;
    cfg_ateccx08a_i2c_default.atcai2c.slave_address = ATECC608A_TNGTLS_I2C_ADDR;
    status = atcab_init(&cfg_ateccx08a_i2c_default);
    if (status == ATCA_SUCCESS)
    {
#if SERIAL_DEBUG
        SSerial_printf(SerialDebug, "ATECC608A init done\r\n");
#endif
        return true;
    }
    else
    {
#if SERIAL_DEBUG
        SSerial_printf(SerialDebug, "ATECC608A init 0X%02X\r\n", status);
#endif
        return false;
    }
}

/**
 * @brief  Test encryption/decryption process
 * @param  SerialDebug Pointer to the debug port.
 * @retval true if successful false in case of failure
 */
bool atecc608a_test_enc_dec(TypeSerial *SerialDebug)
{
    uint8_t plaintext[128] = {0};
    uint8_t cipherText[128] = {0};
    uint8_t decryptedText[128] = {0};
    uint8_t authTag[AES_AUTH_TAG_SIZE] = {0};
    uint8_t iv[AES_IV_LENGTH] = {0};
    bool isVerified = false;

    //Generate random 128 byte plain text for encryption
    for (int idx = 0; idx < 4; idx++)
    {
        if (!atecc608a_generate_random(plaintext + (idx * 32)))
        {
            SSerial_printf(SerialDebug, "Random data generation fail\r\n");
            return false;
        }
    }
    SSerial_printf(SerialDebug, "Plain text : ");
    for (int i = 0; i < 128; i++)
    {
        SSerial_printf(SerialDebug, "%02X", plaintext[i]);
    }
    SSerial_printf(SerialDebug, "\r\n");

    // Encrypt data
    if (!atecc608a_encrypt_data(ATCA_TEMPKEY_KEYID, plaintext, 128, cipherText, authTag, NULL, iv))
    {
        SSerial_printf(SerialDebug, "Encryption failed\r\n");
        return false;
    }

    SSerial_printf(SerialDebug, "IV : ");
    for (int i = 0; i < (AES_IV_LENGTH); i++)
    {
        SSerial_printf(SerialDebug, "%02X", iv[i]);
    }
    SSerial_printf(SerialDebug, "\r\n");
    SSerial_printf(SerialDebug, "Cipher text : ");
    for (int i = 0; i < 128; i++)
    {
        SSerial_printf(SerialDebug, "%02X", cipherText[i]);
    }
    SSerial_printf(SerialDebug, "\r\n");

    SSerial_printf(SerialDebug, "Auth tag : ");
    for (int i = 0; i < AES_AUTH_TAG_SIZE; i++)
    {
        SSerial_printf(SerialDebug, "%02X", authTag[i]);
    }
    SSerial_printf(SerialDebug, "\r\n");

    // Decrypt data
    if (!atecc608a_decrypt_data(ATCA_TEMPKEY_KEYID, cipherText, 128, iv, authTag, decryptedText, &isVerified))
    {
        SSerial_printf(SerialDebug, "Decryption failed \r\n");
        return false;
    }

    SSerial_printf(SerialDebug, "Decrypted data : ");
    for (int i = 0; i < 128; i++)
    {
        SSerial_printf(SerialDebug, "%02X", decryptedText[i]);
    }
    SSerial_printf(SerialDebug, "\r\n");

    if (isVerified == false)
    {
        SSerial_printf(SerialDebug, "Decryption authentication failed \r\n");
    }
    else
    {
        SSerial_printf(SerialDebug, "Decrypted data authenticated\r\n");
    }

    //Check if the plain text and decrypted texts match
    if (memcmp(plaintext, decryptedText, 128) == 0)
    {
        SSerial_printf(SerialDebug, "Decrypted text matches plain text\r\n");
        return true;
    }
    return false;
}

/**
 * @brief  Test ECDH key exchange process
 * @param  SerialDebug Pointer to the debug port.
 * @retval true if successful false in case of failure
 */
bool atecc608a_test_echd(TypeSerial *SerialDebug)
{
    uint8_t ephemeral_Pub_alice[64] = {0};
    uint8_t ephemeral_Pub_bob[64] = {0};
    uint8_t shared_alice[32] = {0};
    uint8_t shared_bob[32] = {0};
    uint8_t salt[32] = {0};
    uint8_t info[32] = {0};
    uint8_t aesKey[32] = {0};

    //Generate a ephemeral key pair on slot 2 and read the public key
    if (atecc608a_gen_ephemeral_key(ECDH_KEY_SLOT, ephemeral_Pub_alice))
    {
        SSerial_printf(SerialDebug, "Ephemeral Public key Alice : ");
        for (int i = 0; i < 64; i++)
        {
            SSerial_printf(SerialDebug, "%02X", ephemeral_Pub_alice[i]);
        }
        SSerial_printf(SerialDebug, "\r\n");
    }

    //Generate a ephemeral key pair on slot 3 and read the public key
    if (atecc608a_gen_ephemeral_key(ECDH_KEY_SLOT + 1, ephemeral_Pub_bob))
    {
        SSerial_printf(SerialDebug, "Ephemeral Public key Bob : ");
        for (int i = 0; i < 64; i++)
        {
            SSerial_printf(SerialDebug, "%02X", ephemeral_Pub_bob[i]);
        }
        SSerial_printf(SerialDebug, "\r\n");
    }

    //Derive shared secret using the private key in slot 2 and input public key corresponding to slot 3
    if (atecc608a_derive_shared_secret(ECDH_KEY_SLOT, ephemeral_Pub_bob, shared_alice, true))
    {
        SSerial_printf(SerialDebug, "Shared secret Alice: ");
        for (int i = 0; i < 32; i++)
        {
            SSerial_printf(SerialDebug, "%02X", shared_alice[i]);
        }
        SSerial_printf(SerialDebug, "\r\n");
    }

    // Derive shared secret using the private key in slot 3 and input public key corresponding to slot 2
    if (atecc608a_derive_shared_secret(ECDH_KEY_SLOT + 1, ephemeral_Pub_alice, shared_bob, true))
    {
        SSerial_printf(SerialDebug, "Shared secret Bob: ");
        for (int i = 0; i < 32; i++)
        {
            SSerial_printf(SerialDebug, "%02X", shared_bob[i]);
        }
        SSerial_printf(SerialDebug, "\r\n");
    }

    //Check if the secretes derived are the same
    if (memcmp(shared_alice, shared_bob, 32) == 0)
    {
        SSerial_printf(SerialDebug, "Derived secrets match\r\n");
    }
    else
    {
        return false;
    }

    //Load shared secret on to the Tempkey register
    atcab_nonce(shared_bob);

    //Salt for KDF
    atecc608a_generate_random(salt);

    //Info for KDF
    atecc608a_generate_random(info);

    //Derive AES key
    if (atecc608a_derive_symmetric_key(salt, info, aesKey, true))
    {
        SSerial_printf(SerialDebug, "AES key: ");
        for (int i = 0; i < 16; i++)
        {
            SSerial_printf(SerialDebug, "%02X", aesKey[i]);
        }
        SSerial_printf(SerialDebug, "\r\n");
    }
    return true;
}

/**
 * @brief  Test signing and signature verification process
 * @param  SerialDebug Pointer to the debug port.
 * @retval true if successful false in case of failure
 */
bool atecc608a_test_sign_verify(TypeSerial *SerialDebug)
{
    uint8_t rnd[32] = {0};
    uint8_t signature[64];
    uint8_t publicKey[64] = {0};
    bool signatureVerified = false;
    //Generate 32 byte random data to sign
    if (atecc608a_generate_random(rnd))
    {
        SSerial_printf(SerialDebug, "Data: ");
        for (int i = 0; i < 32; i++)
        {
            SSerial_printf(SerialDebug, "%02X", rnd[i]);
        }
        SSerial_printf(SerialDebug, "\r\n");
        //Sign the data with the secure element's private key
        if (atecc608a_sign_data(rnd, signature))
        {
            SSerial_printf(SerialDebug, "Signature : ");
            for (int i = 0; i < 64; i++)
            {
                SSerial_printf(SerialDebug, "%02X", signature[i]);
            }
            SSerial_printf(SerialDebug, "\r\n");
        }
    }

    //Read the device public key
    if (atecc608a_read_device_public_key(publicKey))
    {
        SSerial_printf(SerialDebug, "Public key: ");
        for (int i = 0; i < 64; i++)
        {
            SSerial_printf(SerialDebug, "%02X", publicKey[i]);
        }
        SSerial_printf(SerialDebug, "\r\n");
    }

    //Input data, signature and the public key to verify the signature
    if (atecc608a_verify_signature(rnd, signature, publicKey, &signatureVerified))
    {
        if (signatureVerified == true)
        {
            SSerial_printf(SerialDebug, "Sign verified\r\n");
        }
        else
        {
            SSerial_printf(SerialDebug, "Sign verification failed\r\n");
        }
    }
    return signatureVerified;
}

/**
 * @brief  Read the unique 72 bit ID
 * @param  serialNumber Pointer to the serial number
 * @retval true if successful false in case of failure
 */
bool atecc608a_read_serial_number(uint8_t *serialNumber)
{
    uint8_t status;
    status = atcab_read_serial_number(serialNumber);
    if (status == ATCA_SUCCESS)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/**
 * @brief  Write a 64 byte key to a specific key slot
 * @param  slot Slot number (8 to 15 are allowed)
 * @param  pubKey 64 byte key to write
 * @retval true if successful false in case of failure
 */
bool atecc608a_write_Public_Key(uint16_t slot, uint8_t *pubKey)
{
    uint8_t status;
    status = atcab_write_pubkey(slot, pubKey);
    if (status == ATCA_SUCCESS)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/**
 * @brief  Generate a random number
 * @param  rand Pointer to an array of 32 bytes of random data
 * @retval true if successful false in case of failure
 */
bool atecc608a_generate_random(uint8_t *rand)
{
    uint8_t status;
    status = atcab_random(rand);
    if (status == ATCA_SUCCESS)
    {
        return true;
    }
    return false;
}

/**
 * @brief  Load a key to the temp register
 * @param  key Pointer to an array of 32 bytes
 * @retval true if successful false in case of failure
 */
bool atecc608a_load_temp(uint8_t *key)
{
    uint8_t status;
    status = atcab_nonce(key);
    if (status == ATCA_SUCCESS)
    {
        return true;
    }
    return false;
}

/**
 * @brief  Generate a temporary key pair and return the public key
 * @param  keySlot Key slot in which the key pair is generated
 * @param  publicKey Pointer to public key
 * @retval true if successful false in case of failure
 */
bool atecc608a_gen_ephemeral_key(uint8_t keySlot, uint8_t *publicKey)
{
    uint8_t status;
    status = atcab_get_pubkey(keySlot, publicKey);
    if (status == ATCA_SUCCESS)
    {
        return true;
    }
    return false;
}

/**
 * @brief  Derive the ECDH shared secret
 * @param  keySlot Key slot in which the private key is stored
 * @param  publicKey Pointer to public key of the peer
 * @param  sharedSecret Pointer to the shared secret
 * @param  showClear If true, return the shared secret
 *                   If false, the shared secret is written to the tempKey register(in SRAM)
 *                   When stored in tempKey, the sharedSecret can be used for further operations (ex. KDF)
 * @retval true if successful false in case of failure
 */
bool atecc608a_derive_shared_secret(uint8_t keySlot, uint8_t *publicKey, uint8_t *sharedSecret, bool showClear)
{
    uint8_t status;
    if (showClear == true)
    {
        status = atcab_ecdh(keySlot, publicKey, sharedSecret);
    }
    else
    {
        status = atcab_ecdh_tempkey_output(keySlot, publicKey, sharedSecret);
    }

    if (status == ATCA_SUCCESS)
    {
        return true;
    }
    return false;
}

/**
 * @brief  Derive the symmetric key using the HKDF function
 * @param  keySlot Destination key slot where the AES key will be stored
 * @param  salt Pointer to 32 byte salt 
 * @param  info Pointer to 32 byte message 
 * @param  aesKey Pointer to the AES key
 * @param  showClear If true, return the shared secret
 *                   If false, the shared secret is written to the destination key slot
 *                   When stored in keyslot, the AES key can be used for further operations (ex. encryption/decryption)
 * @retval true if successful false in case of failure
 */
bool atecc608a_derive_symmetric_key(uint8_t *salt, uint8_t *info, uint8_t *aesKey, bool showClear)
{
    uint8_t status;
    uint8_t mode;
    uint16_t keyID = 0;
    //Temporary pseudo-random key
    uint8_t prk[32] = {0};

    uint32_t details = KDF_INFO_LENGTH << 24;
    details = details | KDF_DETAILS_HKDF_MSG_LOC_INPUT;

    mode = KDF_MODE_ALG_HKDF | KDF_MODE_TARGET_OUTPUT;

    //Perform expand operation first with IKM and salt
    status = atcab_kdf(mode, keyID, details, salt, prk, NULL);
    if (status != ATCA_SUCCESS)
    {
        return false;
    }

    //Load the PRK to the Tempkey register
    atcab_nonce(prk);

    if (showClear == true)
    {
        mode = KDF_MODE_ALG_HKDF | KDF_MODE_TARGET_OUTPUT;
    }
    else
    {
        mode = KDF_MODE_ALG_HKDF | KDF_MODE_TARGET_SLOT;
        keyID = AES_KEY_SLOT << 8;
    }
    //Perform extract operation PRK and info to generate OKM
    status = atcab_kdf(mode, keyID, details, info, aesKey, NULL);
    if (status == ATCA_SUCCESS)
    {
        if (showClear)
        {
            //Load the AES to the Tempkey register
            atcab_nonce(aesKey);
        }
        return true;
    }
    return false;
}

/**
 * @brief  Sign data using the device private key
 * @param  data Pointer to 32 byte data to be signed
 * @param  signature Pointer to 64 byte signature in R and S big-endian format 
 * @retval true if successful false in case of failure
 */
bool atecc608a_sign_data(uint8_t *data, uint8_t *signature)
{
    uint8_t status;
    status = atcab_sign(0, data, signature);
    if (status == ATCA_SUCCESS)
    {
        return true;
    }
    return false;
}

/**
 * @brief  Generate 32 byte SHA256 digest for the given data 
 * @param  data Pointer to data
 * @param  dataLength Length of input data
 * @param  digest Pointer to the 32 byte digest
 * @retval true if successful false in case of failure
 */
bool atecc608a_generate_SHA256_digest(uint8_t *data, int dataLength, uint8_t *digest)
{
    uint8_t status;
    status = atcab_hw_sha2_256(data, dataLength, digest);
    if (status == ATCA_SUCCESS)
    {
        return true;
    }
    return false;
}

/**
 * @brief  Verify if the data was signed by the private key corresponding to the input public key
 * @param  message Pointer to 32 byte data to be signed
 * @param  signature Pointer to signature
 * @param  publicKey Pointer to public key
 * @param  isVerifed true if the signature verification passes
 * @retval true if successful false in case of failure
 */
bool atecc608a_verify_signature(uint8_t *message, const uint8_t *signature, const uint8_t *publicKey, bool *isVerified)
{
    uint8_t status;
    status = atcab_verify_extern(message, signature, publicKey, isVerified);
    if (status == ATCA_SUCCESS)
    {
        return true;
    }
    return false;
}

/**
 * @brief  Read the public key of the device
 * @param  publicKey Pointer to public key
 * @retval true if successful false in case of failure
 */
bool atecc608a_read_device_public_key(uint8_t *publicKey)
{
    uint8_t status;
    status = atcab_get_pubkey(DEVICE_PRIMARY_KEY_SLOT, publicKey);
    if (status == ATCA_SUCCESS)
    {
        return true;
    }
    return false;
}

/**
 * @brief  Encrypt data using AES-GCM
 * @param  keySlot Key slot in which the AES key is stored
 * @param  plainText data to encrypt
 * @param  dataLenth Length of data to encrypt (Multiple of block length = 16bytes)
 * @param  cipherText Pointer to encrypted data
 * @param  authTag Pointer to authentication tag
 * @param  iv_fixed Pointer to fixed part of the iv
 * @param  iv Pointer to the generated iv
 * @retval true if successful false in case of failure
 */
bool atecc608a_encrypt_data(uint32_t keySlot, uint8_t *plainText, uint16_t dataLength, uint8_t *cipherText, uint8_t *authTag, uint8_t *iv_fixed, uint8_t *iv)
{
    uint8_t status;
    atca_aes_gcm_ctx_t ctx;

    //Datalength has to be a multiple of 16 bytes
    if ((dataLength == 0) || (dataLength % AES_BLOCK_LENGTH != 0))
    {
        return false;
    }

    //Initialize the AES engine with the IV
    status = atcab_aes_gcm_init_rand(&ctx, keySlot, 0, AES_IV_RND_SIZE, iv_fixed, AES_IV_FIXED_SIZE, iv);
    if (status != ATCA_SUCCESS)
    {
        return false;
    }

    uint8_t numberOfBlocks = dataLength / AES_BLOCK_LENGTH;
    //Encrypt the data blockwise
    for (int idx = 0; idx < numberOfBlocks; idx++)
    {
        status = atcab_aes_gcm_encrypt_update(&ctx, (plainText + (idx * AES_BLOCK_LENGTH)), AES_BLOCK_LENGTH, (cipherText + (idx * AES_BLOCK_LENGTH)));
        if (status != ATCA_SUCCESS)
        {
            return false;
        }
    }
    status = atcab_aes_gcm_encrypt_finish(&ctx, authTag, AES_AUTH_TAG_SIZE);
    if (status != ATCA_SUCCESS)
    {
        return false;
    }

    return true;
}

/**
 * @brief  Encrypt data using AES-GCM
 * @param  keySlot Key slot in which the AES key is stored
 * @param  cipherText Pointer to encrypted data
 * @param  dataLenth Length of data to encrypt (Multiple of block length = 16bytes)
 * @param  iv Pointer to the iv
 * @param  authTag Pointer to authentication tag
 * @param  plainText decrypted data
 * @param  isVerified Bool status of authentication
 * @retval true if successful false in case of failure
 */
bool atecc608a_decrypt_data(uint32_t keySlot, uint8_t *cipherText, uint16_t dataLength, uint8_t *iv, uint8_t *authTag, uint8_t *plaintext, bool *isVerified)
{
    uint8_t status;
    atca_aes_gcm_ctx_t ctx;

    //Datalength has to be a multiple of 16 bytes
    if ((dataLength == 0) || (dataLength % AES_BLOCK_LENGTH != 0))
    {
        return false;
    }

    //Initialize the AES engine with the IV
    status = atcab_aes_gcm_init(&ctx, keySlot, 0, iv, AES_IV_LENGTH);
    if (status != ATCA_SUCCESS)
    {
        return false;
    }

    uint8_t numberOfBlocks = dataLength / AES_BLOCK_LENGTH;

    //Encrypt the data blockwise
    for (int idx = 0; idx < numberOfBlocks; idx++)
    {
        status = atcab_aes_gcm_decrypt_update(&ctx, (cipherText + (idx * AES_BLOCK_LENGTH)), AES_BLOCK_LENGTH, (plaintext + (idx * AES_BLOCK_LENGTH)));
        if (status != ATCA_SUCCESS)
        {
            return false;
        }
    }
    status = atcab_aes_gcm_decrypt_finish(&ctx, authTag, AES_AUTH_TAG_SIZE, isVerified);
    if (status != ATCA_SUCCESS)
    {
        return false;
    }

    return true;
}

/**
 * @brief  Read and print certificates from the secure element
 * @param  SerialDebug Pointer to the debug port.
 * @retval none
 */
void atecc608a_print_certs(TypeSerial *SerialDebug)
{
    int atca_status = ATCACERT_E_SUCCESS;
    uint8_t *signer_cert = NULL;
    size_t signer_cert_size;
    uint8_t signer_public_key[SIGNER_PUBLIC_KEY_MAX_LEN];
    uint8_t *device_cert = NULL;
    size_t device_cert_size;
    uint8_t cert_sn[CERT_SN_MAX_LEN];
    size_t cert_sn_size;
    uint8_t *file_list = NULL;
    char *device_cert_filename = NULL;
    char *signer_cert_filename = NULL;
    uint32_t sector_buffer[MAX_TLS_CERT_LENGTH];
    char pem_cert[1024];
    size_t pem_cert_size;
    do
    {
        // Clear cert chain buffer
        memset(sector_buffer, 0xFF, sizeof(sector_buffer));

        // Use the end of the sector buffer to temporarily hold the data to save RAM
        file_list = ((uint8_t *)sector_buffer) + (sizeof(sector_buffer) - TLS_FILE_NAME_MAX * 2);
        signer_cert = file_list - SIGNER_CERT_MAX_LEN;
        device_cert = signer_cert - DEVICE_CERT_MAX_LEN;

        // Init the file list
        memset(file_list, 0, TLS_FILE_NAME_MAX * 2);
        device_cert_filename = (char *)&file_list[0];
        signer_cert_filename = (char *)&file_list[TLS_FILE_NAME_MAX];

        // Uncompress the signer certificate from the ATECCx08A device
        signer_cert_size = SIGNER_CERT_MAX_LEN;
        atca_status = atcacert_read_cert(&g_tngtls_cert_def_1_signer, NULL,
                                         signer_cert, &signer_cert_size);
        if (atca_status != ATCACERT_E_SUCCESS)
        {
            SSerial_printf(SerialDebug, "Certificates not found %i\r\n", atca_status);
            // Break the do/while loop
            break;
        }
        pem_cert_size = sizeof(pem_cert);
        atcacert_encode_pem_cert(signer_cert, signer_cert_size, pem_cert, &pem_cert_size);
        SSerial_printf(SerialDebug, "Signer Cert : \r\n%s\r\n", pem_cert);
        // Get the signer's public key from its certificate
        atca_status = atcacert_get_subj_public_key(&g_tngtls_cert_def_1_signer, signer_cert,
                                                   signer_cert_size, signer_public_key);
        if (atca_status != ATCACERT_E_SUCCESS)
        {
            SSerial_printf(SerialDebug, "atcacert_get_subj_public_key failed 0X%02X\r\n", atca_status);
            // Break the do/while loop
            break;
        }

        // Uncompress the device certificate from the ATECCx08A device.
        device_cert_size = DEVICE_CERT_MAX_LEN;
        atca_status = atcacert_read_cert(&g_tngtls_cert_def_3_device, signer_public_key,
                                         device_cert, &device_cert_size);
        if (atca_status != ATCACERT_E_SUCCESS)
        {
            SSerial_printf(SerialDebug, "atcacert_read_cert failed 0X%02X\r\n", atca_status);
            // Break the do/while loop
            break;
        }
        pem_cert_size = sizeof(pem_cert);
        atcacert_encode_pem_cert(device_cert, device_cert_size, pem_cert, &pem_cert_size);
        SSerial_printf(SerialDebug, "Device Cert : \r\n%s\r\n", pem_cert);

        atca_status = atcacert_get_subj_key_id(&g_tngtls_cert_def_3_device, device_cert,
                                               device_cert_size, subject_key_id);
        if (atca_status != ATCACERT_E_SUCCESS)
        {
            SSerial_printf(SerialDebug, "atcacert_get_subj_key_id failed 0X%02X\r\n", atca_status);
            // Break the do/while loop
            break;
        }

        // Get the device certificate SN for the filename
        cert_sn_size = sizeof(cert_sn);
        atca_status = atcacert_get_cert_sn(&g_tngtls_cert_def_3_device, device_cert,
                                           device_cert_size, cert_sn, &cert_sn_size);
        if (atca_status != ATCACERT_E_SUCCESS)
        {
            SSerial_printf(SerialDebug, "atcacert_get_cert_sn failed 0X%02X\r\n", atca_status);
            // Break the do/while loop
            break;
        }

        // Build the device certificate filename
        strcat(device_cert_filename, bin2hex(cert_sn, cert_sn_size));

        SSerial_printf(SerialDebug, "Device cert serial number : %s \r\n", device_cert_filename);

        device_cert = NULL; // Make sure we don't use this now that it has moved

        // Get the signer certificate SN for the filename
        cert_sn_size = sizeof(cert_sn);
        atca_status = atcacert_get_cert_sn(&g_tngtls_cert_def_1_signer, signer_cert,
                                           signer_cert_size, cert_sn, &cert_sn_size);
        if (atca_status != ATCACERT_E_SUCCESS)
        {

            SSerial_printf(SerialDebug, "atcacert_get_cert_sn failed 0X%02X\r\n", atca_status);
            // Break the do/while loop
            break;
        }

        // Build the signer certificate filename
        strcat(signer_cert_filename, bin2hex(cert_sn, cert_sn_size));
        SSerial_printf(SerialDebug, "Signer cert serial number : %s \r\n", signer_cert_filename);

        file_list = NULL;
        signer_cert_filename = NULL;
        device_cert_filename = NULL;
    } while (false);
}

/**
 * @brief  Convert binary to hex
 * @param  data Pointer to data
 * @param  data_size Length of data
 * @retval true if successful false in case of failure
 */
static const char *bin2hex(const void *data, size_t data_size)
{
    static char buf[256];
    static char hex[] = "0123456789abcdef";
    const uint8_t *data8 = data;

    if (data_size * 2 > sizeof(buf) - 1)
        return "[buf too small]";

    for (size_t i = 0; i < data_size; i++)
    {
        buf[i * 2 + 0] = hex[(*data8) >> 4];
        buf[i * 2 + 1] = hex[(*data8) & 0xF];
        data8++;
    }
    buf[data_size * 2] = 0;

    return buf;
}

/**
 * @brief  Convert hex string to bin array
 * @param  hex Pointer to hex string
 * @param  hex_size Length of hex string
 * @param  bin Pointer to bin array
 * @param  bin_size Length of bin data
 * @retval true if successful false in case of failure
 */
bool atecc608a_hex2bin(const char *hex, size_t hex_size, uint8_t *bin, size_t *bin_size)
{
    if (hex_size % 2 == 1)
    {
        return false;
    }

    *bin_size = hex_size / 2;

    for (size_t i = 0, j = 0; j < *bin_size; i += 2, j++)
    {
        bin[j] = (hex[i] % 32 + 9) % 25 * 16 + (hex[i + 1] % 32 + 9) % 25;
    }
    return true;
}