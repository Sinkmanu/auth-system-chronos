// *************************************************************************************************
// AES encryption and decryption functions.
// *************************************************************************************************

#ifndef _DRIVER_AES_H_
#define _DRIVER_AES_H_

// *************************************************************************************************
// Include section
#include "project.h"

// *************************************************************************************************
// Define section
#define AES_KEY_SIZE (16u)

// *************************************************************************************************
// Prototype section
extern void aes_encrypt(u8 *data, u8 *key);
extern void aes_decrypt(u8 *data, u8 *key);
extern void aes_decryptCBC(u8 *data,u8 *key);

#endif // _DRIVER_AES_H_


