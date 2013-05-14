// *************************************************************************************************
//
// Actual revision: $Revision: $
// Revision label:  $Name: $
// Revision state:  $State: $
//
// *************************************************************************************************
// Radio core access functions. Taken from TI reference code for CC430.
// *************************************************************************************************

// *************************************************************************************************
// Prototype section
unsigned char Strobe(unsigned char strobe);
unsigned char ReadSingleReg(unsigned char addr);
void WriteSingleReg(unsigned char addr, unsigned char value);
void ReadBurstReg(unsigned char addr, unsigned char *buffer, unsigned char count);
void WriteBurstReg(unsigned char addr, unsigned char *buffer, unsigned char count);
void ResetRadioCore (void);
void WritePATable(unsigned char value);
void WaitForXT2(void);

//Nuevas
/*u8 rf1a_strobe(u8 strobe);

u8 rf1a_write_single(u8 addr, u8 value);

u8 rf1a_read_rx(u8 *data, u8 length);
u8 rf1a_write_tx(u8 *data, u8 length);

u8 rf1a_write_patable(u8 patable[8]);*/

