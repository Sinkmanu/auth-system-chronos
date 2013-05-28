// *************************************************************************************************
// AES encryption and decryption functions.
// *************************************************************************************************

// *************************************************************************************************
// Include section
#include "aes.h"
#include "ioCCxx10_bitdef.h"

// *************************************************************************************************
// Define section
#define DELAY 0x0f

// *************************************************************************************************
// Prototype section
void aes_encrypt(u8 *data, u8 *key);
void aes_decrypt(u8 *data, u8 *key);
void aes_decryptCBC(u8 *data,u8 *key);

// *************************************************************************************************
// @fn          aes_encrypt
// @brief       AES 128-bit Encryption.
// @param       data - to be encrypted, key - used to encrypt
// @return      none
// *************************************************************************************************
void aes_encrypt(u8 *data, u8 *key)
{
	u8 i = 0, delay = DELAY;

	// Load key
	ENCCS = ENCCS_CMD_LDKEY | ENCCS_ST;
	for (i = 0; i < AES_KEY_SIZE; i++)
	{
		ENCDI = key[i]; 
	}
	

	// Load IV
	ENCCS = ENCCS_CMD_LDIV | ENCCS_ST;
	for (i = 0; i < AES_KEY_SIZE; i++)
	{
		ENCDI = 0;
	}

	// Load data
	ENCCS = ENCCS_CMD_ENC | ENCCS_ST;
	for (i = 0; i < AES_KEY_SIZE; i++)
	{
		ENCDI = data[i];
	}

	while(delay--);

	// Read data
	for(i = 0; i < AES_KEY_SIZE; i++)
	{
		data[i] = ENCDO; 
	}

}

// *************************************************************************************************
// @fn          aes_decrypt
// @brief       AES 128-bit Decryption.
// @param       data - to be decrypted, key - used to decrypt
// @return      none
// *************************************************************************************************
void aes_decrypt(u8 *data, u8 *key)
{
	u8 i = 0, delay = DELAY;

	// Load key
	ENCCS =   ENCCS_MODE_ECB | ENCCS_CMD_LDKEY | ENCCS_ST;
	for (i = 0; i < AES_KEY_SIZE; i++)
	{
		ENCDI = key[i]; 
	}

	// Load IV
	ENCCS = ENCCS_MODE_ECB | ENCCS_CMD_LDIV | ENCCS_ST;
	for (i = 0; i < AES_KEY_SIZE; i++)
	{
		ENCDI = 0;
	}

	// Load data
	ENCCS = ENCCS_MODE_ECB | ENCCS_CMD_DEC | ENCCS_ST;
	for (i = 0; i < AES_KEY_SIZE; i++)
	{
		ENCDI = data[i];
	}

	while(delay--);

	// Read data
	for (i = 0; i < AES_KEY_SIZE; i++)
	{
		data[i] = ENCDO; 
	}
}

void aes_decryptCBC(u8 *data,u8 *key)
{
  u8 buffer[16];
  u8 buffer_d[16];
  u8 i;
  for(i=0;i<16;i++)
  {
    buffer[i] = data[i];
    buffer_d[i] = data[i];
  }
  aes_decrypt(buffer,key);
  for(i=0;i<16;i++) data[i] = buffer[i] ^ 0x00; //^ 0;
  for(i=0;i<16;i++) buffer[i] = data[16+i];
  aes_decrypt(buffer,key);
  for(i=0;i<16;i++) data[i+16] = buffer[i]^buffer_d[i];
}


