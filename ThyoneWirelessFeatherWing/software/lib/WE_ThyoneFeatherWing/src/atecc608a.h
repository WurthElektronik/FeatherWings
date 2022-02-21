/**
 * \file
 * \brief File for the tyhpne-i radio module.
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

#ifndef ATECC608A_H
#define ATECC608A_H

/**         Includes         */
#include "ConfigPlatform.h"

#define ATECC608A_TNGTLS_I2C_ADDR 0x6A
#define SIGNER_PUBLIC_KEY_MAX_LEN 64
#define CERT_SN_MAX_LEN 32
#define MAX_TLS_CERT_LENGTH 1024
#define TLS_FILE_NAME_MAX 48
#define SIGNER_CERT_MAX_LEN (g_tngtls_cert_def_1_signer.cert_template_size + 8)
#define DEVICE_CERT_MAX_LEN 554
#define KDF_INFO_LENGTH 32

#define DEVICE_PRIMARY_KEY_SLOT 0x00 /*Permanent Primary authentication key for ext sign*/
#define ECDH_KEY_SLOT 0x02           /*Updatable secondary private key for use as ephemeral key*/
#define AES_KEY_SLOT 0x09            /*Write only AES key slot for symmetric encryption*/
#define AES_IV_RND_SIZE 12           /*Size of the random part of the AES IV*/
#define AES_IV_FIXED_SIZE 0          /*Size of the fixed part of the AES IV*/
#define AES_IV_LENGTH 12
#define AES_BLOCK_LENGTH 16  /*Length of each block for the AES encryption/decryption*/
#define AES_AUTH_TAG_SIZE 12 /*Size of the authentication tag*/

/**         Functions definition         */
void atecc608a_print_certs(TypeSerial *SerialDebug);
bool atecc608a_init(TypeSerial *SerialDebug);
bool atecc608a_test_sign_verify(TypeSerial *SerialDebug);
bool atecc608a_test_echd(TypeSerial *SerialDebug);
bool atecc608a_test_enc_dec(TypeSerial *SerialDebug);
bool atecc608a_load_temp(uint8_t *key);
bool atecc608a_write_Public_Key(uint16_t slot, uint8_t *pubKey);

bool atecc608a_read_serial_number(uint8_t *serialNumber);
bool atecc608a_generate_random(uint8_t *rand);
bool atecc608a_read_device_public_key(uint8_t *publicKey);
bool atecc608a_sign_data(uint8_t *data, uint8_t *signature);
bool atecc608a_verify_signature(uint8_t *message, const uint8_t *signature, const uint8_t *publicKey, bool *isVerified);
bool atecc608a_gen_ephemeral_key(uint8_t keySlot, uint8_t *publicKey);
bool atecc608a_derive_shared_secret(uint8_t keySlot, uint8_t *publicKey, uint8_t *sharedSecret, bool showClear);
bool atecc608a_derive_symmetric_key(uint8_t *salt, uint8_t *info, uint8_t *aesKey, bool showClear);
bool atecc608a_generate_SHA256_digest(uint8_t *data, int dataLength, uint8_t *digest);
bool atecc608a_decrypt_data(uint32_t keySlot, uint8_t *cipherText, uint16_t dataLength, uint8_t *iv, uint8_t *authTag, uint8_t *plaintext, bool *isVerified);
bool atecc608a_encrypt_data(uint32_t keySlot, uint8_t *plainText, uint16_t dataLength, uint8_t *cipherText, uint8_t *authTag, uint8_t *iv_fixed, uint8_t *iv);

bool atecc608a_hex2bin(const char *hex, size_t hex_size, uint8_t *bin, size_t *bin_size);
#ifdef __cplusplus
extern "C"
{
#endif

#ifdef __cplusplus
}
#endif

#endif /* ATECC608A_H */