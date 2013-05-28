/***********************************************************************
Copyright (C) 2012 Manuel Mancera

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*************************************************************************/

// *************************************************************************************************
// Include section
#include <string.h>
#include "bsp.h"
#include "mrfi.h"
#include "bsp_leds.h"
#include "bsp_buttons.h"
#include "nwk_types.h"
#include "nwk_api.h"
#include "nwk_frame.h"
#include "nwk.h"
#include "ioCCxx10_bitdef.h" 
#include "packet.h"
#include "authentication.h"
#include "aes.h"

#include "hal_types.h"
#include "usb_uart.h"
#include "hal_board.h"
#include "hal_led.h"
#include "hal_uart.h"

void authentication_config(void);
void authentication_main(u32 random);
void authentication_receive(void);
void authentication_retrieve(packet_t *packet);
void AuthRxModeOn(void);
void AuthRxModeOff(void);
void radio_transmit(void);


u8 key_ap[16] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F};
u8 data2usb(u8 data[6]);
u8 paquete_recibido[41] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                           0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                           0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
u8 auth_usb[18] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
packet_t packet;
void send_example(void);
void send_rand(u32 rand);
void send_ack_radio(u8 randaes[16]);

/*extern unsigned char usb_buffer[USB_MAX_MESSAGE_LENGTH+2];
extern unsigned char usb_sendack;
extern void usb_decode(void);*/

// *************************************************************************************************
// Global section
u8 packet_ok = 0;
u8 packet_id_rand = 0;
u8 envia_ejemplo = 0;

u8 auth_data_tx[PACKET_BEACON_LENGTH];//[PACKET_TRANSMIT_LENGTH];
u8 auth_data_example[26];
u8 auth_data_rx[PACKET_RECEIVE_LENGTH];
u8 auth_ptr_tx = 0;
u8 auth_ptr_rx = 0;


volatile unsigned char auth_flag;

// *************************************************************************************************
// @fn          authentication_config
// @brief       Configures the Radio Settings for AuthenticationSystem
// @param       none
// @return      none
// *************************************************************************************************
void authentication_config(void)
{                        

   	//Set the SYNC words to be used
   	SYNC1      =   0xD3; // SYNC1: high nyte of Sync Word
   	SYNC0      =   0x91;  // SYNC0: low nyte of Sync Word
   	//Setup radio
   	FSCTRL1     = 0x09;   // Frequency synthesizer control.
   	FSCTRL0     = 0x00;   // Frequency synthesizer control.
	// 869.525MHz
        FREQ2     = 0x24; // frequency control word, high byte 
        FREQ1     = 0x3A; // frequency control word, middle byte 
        FREQ0     = 0xEE; // frequency control word, low byte 
   	CHANNR      = 0x00;                  // Channel number.
   	//PA_TABLE0   = 0x8C;               // PA output power setting.
        PA_TABLE0   = 0x84;
        
        MDMCFG4   = 0x6B; // modem configuration 
        MDMCFG3   = 0xA3; // modem configuration 
        MDMCFG2   = 0x13; // modem configuration 
  	MDMCFG1   = 0x23;               // Modem configuration.
   	MDMCFG0   = 0x11;               // Modem configuration.
   
   	DEVIATN   = 0x43;   // Modem deviation setting (when FSK modulation is enabled).
   	FREND1    = 0xB6;   // Front end RX configuration.
   	FREND0    = 0x10;   // Front end TX configuration.

   	MCSM0     = 0x18;   // Main Radio Control State Machine configuration. CONFIGURADO.
   	MCSM1     = 0x30;  
   	MCSM2     = 0x07;

   	WOREVT1   = 0x87;
   	WOREVT0   = 0x6B;
   	WORCTL    = 0xF8;
   	FOCCFG    = 0x1D;   // Frequency Offset Compensation Configuration.
   	BSCFG     = 0x1C;   // Bit synchronization Configuration.
   	AGCCTRL2  = 0xC7;   // AGC control.
   	AGCCTRL1  = 0x00;   // AGC control.
   	AGCCTRL0  = 0xB2;   // AGC control.
   	FSCAL3    = 0xEA;   // Frequency synthesizer calibration.
   	FSCAL2    = 0x2A;   // Frequency synthesizer calibration.
   	FSCAL1    = 0x00;   // Frequency synthesizer calibration.
   	FSCAL0    = 0x1F;   // Frequency synthesizer calibration.
   	FSTEST    = 0x59;
   	TEST2     = 0x81;   // Various test settings.
   	TEST1     = 0x35;   // Various test settings.
   	TEST0     = 0x09;   // Various test settings.
   	PTEST     = 0x7F;
   	AGCTEST   = 0x88;
   	IOCFG2    = 0x29;
   	IOCFG1    = 0x1E;
   	IOCFG0    = 0x1B;
   
   	PKTCTRL1  = 0x04;   // Packet automation control. CONFIGURADO.
   	PKTCTRL0  = 0x05;   // Packet automation control. Bit(1:0) 01-> Variable packet len mode. 00 -> PKTLEN Register. and CRC Enable
   	ADDR      = 0x00;   // Device address.
   	PKTLEN    = 0xFF;   // Packet length. 	Si ponemos 0xFF en el primer byte a recibir/transmitir (RFD) debemos indicar el tamaño.


	//
	// Enable interrupts.
	//
	RFIF = 0;
	IEN2 |= IEN2_RFIE;			// General RF interrupts
	RFTXRXIE = 1;				// CPU interrupts for RF TX done / RX ready
	RFIM |= RFIF_IRQ_DONE |		// packet completion
			RFIF_IRQ_TXUNF |	// tx underflow
			RFIF_IRQ_RXOVF |	// rx overflow
			RFIF_IRQ_SFD |		// start frame delimiter
			RFIF_IRQ_TIMEOUT;	// rx timeout

	//
	// Go into idle state.
	//
	RFST = RFST_SIDLE;
	while (MARCSTATE != MARC_STATE_IDLE);
}

void AuthRxModeOn(void)
{

  RFST = RFST_SIDLE;
  while (MARCSTATE != MARC_STATE_IDLE);

  auth_ptr_rx = 0;
  
  /* send strobe to enter receive mode */
  RFST = RFST_SRX;
  while (MARCSTATE != MARC_STATE_RX);
}

void AuthRxModeOff(void)
{
  /*disable receive (IM_DONE) interrupts */
  RFIM &= ~(RFIM_IM_DONE | RFIF_IRQ_TXUNF | RFIF_IRQ_RXOVF | RFIF_IRQ_SFD | RFIF_IRQ_TIMEOUT);

  /* flush the receive FIFO of any residual data */
  /* no need for flush. only 1 byte deep fifo. */

  /* clear receive interrupt */
  S1CON &= ~(S1CON_RFIF_1 | S1CON_RFIF_0); /* Clear MCU interrupt flag */
  RFIF &= ~RFIF_IRQ_DONE;           /* Clear the interrupt at the source */

}

void radio_receive(void)
{
  RFST = RFST_SIDLE;
  while (MARCSTATE != MARC_STATE_IDLE);
  
  //prepare
  auth_ptr_rx = 0;
  
  RFST = RFST_SRX;
  while(MARCSTATE != MARC_STATE_RX);
}

u8 example_packet_tx[26] = { 25,                  //Length
                            0xFF,0xFF,0xFF,0xFF,  //Dst
                            0x02,0x02,0x02,0x02,  //Src
                            0x02,                 //type
                            0x00,0x02,0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x07,0x08,0x09,0x0A,0x0B,0x0C};  //Payload

void radio_transmit(void)
{
  RFST = RFST_SIDLE;
  
  while (MARCSTATE != MARC_STATE_IDLE);
  
  auth_ptr_tx = 0;
  RFST = RFST_STX;
  while (MARCSTATE != MARC_STATE_TX);
  
}

// *************************************************************************************************
// RF TX/RX general IRQ providing status handling
// *************************************************************************************************
void radio_rf_ISR(void)
{
	//Clear CPU int flag
	S1CON &= ~(S1CON_RFIF_1 + S1CON_RFIF_0);
	
	if (RFIF & RFIF_IRQ_DONE) //Paquete recibido/transmitido
	{		
               /*if (auth_ptr_tx == PACKET_BEACON_LENGTH)//26)//PACKET_TRANSMIT_LENGTH) 
               {
                        RFST = RFST_SIDLE;
                        auth_ptr_tx = 0;
                }
                else if (auth_ptr_rx == PACKET_RECEIVE_LENGTH)//28) //PACKET_RECEIVE_LENGTH
                {
                   RFST = RFST_SIDLE;
                   packet_ok=1;
                   auth_ptr_rx = 0;
                }
                else  
                {
                    auth_ptr_tx = 0;
                    auth_ptr_rx = 0;
                    RFST = RFST_SIDLE;
                }*/
                if ((auth_ptr_tx == PACKET_BEACON_LENGTH) && (!envia_ejemplo))//26)//PACKET_TRANSMIT_LENGTH) 
                {
                        RFST = RFST_SIDLE;
                        auth_ptr_tx = 0;
                }
                else if ((auth_ptr_tx == 26) && (envia_ejemplo))
                {
                        RFST = RFST_SIDLE;
                        auth_ptr_tx = 0;
                        envia_ejemplo = 0;
                }
                else if (auth_ptr_rx == PACKET_RECEIVE_LENGTH)//28) //PACKET_RECEIVE_LENGTH
                {
                   RFST = RFST_SIDLE;
                   packet_ok=1;
                   auth_ptr_rx = 0;
                }
                else  
                {
                    auth_ptr_tx = 0;
                    auth_ptr_rx = 0;
                    RFST = RFST_SIDLE;
                }
	}
        
        if (RFIF & RFIF_IRQ_TXUNF)
        {
           auth_ptr_tx = 0;
           RFST = RFST_SIDLE;
        }
	
	if (RFIF & RFIF_IRQ_RXOVF)
	{
		// rx overflow, this should not happen, reset
		//radio_rx_index = 0;
                auth_ptr_rx = 0;
		RFST = RFST_SIDLE;
	}
	
	if (RFIF & RFIF_IRQ_SFD)
	{
		// Start Frame Delimiter
		// start receiving
		auth_ptr_rx = 0;
	}
	
	if (RFIF & RFIF_IRQ_TIMEOUT)
	{
		// reset
		RFST = RFST_SIDLE;
		auth_ptr_rx = 0;
	}
	//Clear IRQ
	RFIF = 0;
}

void radio_rftxrx(void)
{
	//u8 data;
        
        if (MARCSTATE == MARC_STATE_RX)
        {
	  if (auth_ptr_rx < PACKET_RECEIVE_LENGTH)//28)//PACKET_RECEIVE_LENGTH)
	  {
		auth_data_rx[auth_ptr_rx] = RFD;
		auth_ptr_rx++;
	  }
        }
        else if (MARCSTATE == MARC_STATE_TX)
        { 
            if ((auth_ptr_tx < PACKET_BEACON_LENGTH) && (!envia_ejemplo))//26)//PACKET_TRANSMIT_LENGTH)
              {
                  RFD = auth_data_tx[auth_ptr_tx];
                 auth_ptr_tx++;
              }else if ((auth_ptr_tx < 26) && (envia_ejemplo))
              {
                 RFD = auth_data_example[auth_ptr_tx];
                 auth_ptr_tx++;
                
              }
          //LED_ON;
        }
}

u8 check_packet(packet_t *packet)
{
  /*Aqui usamos python para comprobar la ID y RAND*/
  if (packet->type != 0x01) return PACKET_ERROR_TYPE;
  else return PACKET_SIN_ERROR;
}

//packet_d = AESap(ID XOR IV) - AESap(AESap(ID XOR IV) XOR AESw(RANDOM))
void decompress_packet(u8 *packet_d,packet_t *packet)
{
  u8 i;
  u8 data_encrypt[32];
  for (i=0;i<4;i++) packet->dst[i] = packet_d[i];
  for (i=0;i<4;i++) packet->src[i] = packet_d[4+i];
  packet->type = packet_d[8];
  for(i=0;i<32;i++) data_encrypt[i] = packet_d[9+i];
  aes_decryptCBC(data_encrypt,key_ap);
  packet->payload.random.id = (data_encrypt[0]*0x100) + data_encrypt[1];
  for(i=0;i<14;i++) packet->payload.random.padd_id[i] = data_encrypt[i+2]; //Deberian de ser todos 0x00
  for(i=0;i<16;i++) packet->payload.random.number_encrypt[i] = data_encrypt[i+16];
}

void send_example(void)
{
  u8 i;
  u8 cad[16];
  envia_ejemplo = 1;
  
  for (i=0;i<16;i++) cad[i] = example_packet_tx[i+10];
  aes_encrypt(cad,key_ap);
  for (i=0;i<10;i++) auth_data_example[i]=example_packet_tx[i];
  for(i=0;i<16;i++) auth_data_example[i+10]=cad[i];
  radio_transmit();
  while (auth_ptr_tx != 0);
}

void send_rand(u32 rand)
{
  u8 i;
  auth_data_tx[0]=13;
  for (i=1;i<5;i++) auth_data_tx[i] = 0xFF; //DST
  for (i=5;i<9;i++) auth_data_tx[i] = 0x02; //SRC
  auth_data_tx[9]=PACKET_TYPE_BEACON;
  auth_data_tx[10]=(u8)(rand>>24);
  auth_data_tx[11]=(u8)(rand>>16);
  auth_data_tx[12]=(u8)(rand>>8);
  auth_data_tx[13]=(u8)(rand>>0);
  //for (i=14;i<PACKET_TRANSMIT_LENGTH;i++) auth_data_tx[i] = 0x00;
  radio_transmit();
  while (auth_ptr_tx != 0);
}

void send_ack_radio(u8 randaes[16])
{
   u8 i;
   envia_ejemplo = 1;
   auth_data_example[0]=25;
   for (i=1;i<5;i++) auth_data_example[i] = 0xFF; //DST
   for (i=5;i<9;i++) auth_data_example[i] = 0x02; //SRC
   auth_data_example[9] = PACKET_TYPE_OK;
   for(i=0;i<16;i++) auth_data_example[i+10] = randaes[i];
   radio_transmit();
   while (auth_ptr_tx != 0);
}

//Rerurn ID and AES(RAND)
u8 data2usb(u8 data[18])
{
    u8 i;
    if (packet_id_rand)
    {
      data[0]=(u8)(packet.payload.random.id >> 8);
      data[1]=(u8)(packet.payload.random.id);
      for(i=2;i<18;i++) data[i] = packet.payload.random.number_encrypt[i-2];
    } else {
      for (i=0;i<18;i++) data[i]=0x00;
    }
    packet_id_rand = 0;
    return 18;
}


// AP main routine
void authentication_main(u32 random)
{
  u8 i;
  u16 d_sleep = 0;
  
  AuthRxModeOn(); //Mode RX Start
  
  /* Espera recibir un paquete de autenticacion */
  for (d_sleep=0;d_sleep<10000;d_sleep++)
  {
        //if (auth_data_rx[9] == 0x01) LED_ON; //type
        //if (auth_data_rx[11] == 'O') LED_ON;
        if (packet_ok) //receive packet 
        {
                LED_ON;
          	//if (auth_data_rx[2] == 0xFF) radio_transmit();
                packet_ok = 0;

                for(i=0;i<41;i++) paquete_recibido[i] = auth_data_rx[i+1];
                
                decompress_packet(paquete_recibido,&packet);
                
                //send_rand(packet.payload.random.number);
                if (check_packet(&packet) == PACKET_SIN_ERROR)
                {
                  packet_id_rand = 1;
                  
                  //Comprobamos random para ver si es valido, si es valido sacamos el (ID,random) encriptado por USB
                  //en auth_usb se guarda la id[0:1] y el random encriptado[2:18]
                  /*for (d_sleep=0; d_sleep<50000; d_sleep++);
                  for (d_sleep=0; d_sleep<50000; d_sleep++);
                  for (d_sleep=0; d_sleep<50000; d_sleep++);
                  for (d_sleep=0; d_sleep<50000; d_sleep++);
                  for (d_sleep=0; d_sleep<50000; d_sleep++);
                  for (d_sleep=0; d_sleep<50000; d_sleep++);
                  send_example();*/
                }
                AuthRxModeOff();
                LED_OFF;
        }
  }
}
