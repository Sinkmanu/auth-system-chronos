// *************************************************************************************************
//
//	Copyright (C) 2009 Texas Instruments Incorporated - http://www.ti.com/ 
//	 
//	 
//	  Redistribution and use in source and binary forms, with or without 
//	  modification, are permitted provided that the following conditions 
//	  are met:
//	
//	    Redistributions of source code must retain the above copyright 
//	    notice, this list of conditions and the following disclaimer.
//	 
//	    Redistributions in binary form must reproduce the above copyright
//	    notice, this list of conditions and the following disclaimer in the 
//	    documentation and/or other materials provided with the   
//	    distribution.
//	 
//	    Neither the name of Texas Instruments Incorporated nor the names of
//	    its contributors may be used to endorse or promote products derived
//	    from this software without specific prior written permission.
//	
//	  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
//	  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
//	  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//	  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
//	  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
//	  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
//	  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//	  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//	  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
//	  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
//	  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// *************************************************************************************************
// Radio core access functions. Taken from TI reference code for CC430.
// *************************************************************************************************

// system
#include "project.h"

// driver
#include "rf1a.h"
#include "timer.h"
//#include "simpliciti/Components/mrfi/smartrf/CC430/smartrf_CC430.h"
#include "smartrf.h"
#include "display.h"

// logic
#include "rfsimpliciti.h"
//pfs
#ifndef ELIMINATE_BLUEROBIN
#include "bluerobin.h"
#endif

//#define PACKET_LEN	26
#define PACKET_LEN	14
#define PACKET_LEN_OK	26
#define RSSI_IDX	(PACKET_LEN)
#define CRC_LQI_IDX	(PACKET_LEN+1)
#define  CRC_OK		(BIT7)          // CRC_OK bit 

u8 beacon = 0;
u8 packet_rx_ok[28];
u8 packet_rx[16]; //PACKET_LEN + 2
u8 RxBufferLength;
//u8 packet_message[4];

// *************************************************************************************************
// Extern section

// SimpliciTI CC430 radio ISR - located in SimpliciTi library
extern void MRFI_RadioIsr(void);

// BlueRobin CC430 radio ISR - located in BlueRobin library
extern void BlueRobin_RadioISR_v(void);

//Variables

u8 receiving = 0;

// *************************************************************************************************
// @fn          radio_config
// @brief       Configuration radio core. 
// @param       none
// @return      none
// *************************************************************************************************
/*void radio_config(void)
{
	WriteSingleReg(IOCFG2, SMARTRF_SETTING_IOCFG2);
	// IOCFG0: GDO0 signals on PA power down signal to control RX/TX switch     
	//rf1a_write_single(IOCFG0D, SMARTRF_SETTING_IOCFG0D);
	// IOCFG1: GDO1 signals on RSSI_VALID
	WriteSingleReg(IOCFG1, SMARTRF_SETTING_IOCFG1); 
	// IOCFG0: GDO0 signals on PA power down signal to control RX/TX switch     
	WriteSingleReg(IOCFG0, SMARTRF_SETTING_IOCFG0);
	// FIFOTHR: RX/TX FIFO Threshold: 33 bytes in TX, 32 bytes in RX         
	WriteSingleReg(FIFOTHR, SMARTRF_SETTING_FIFOTHR);
	// SYNC1: high byte of Sync Word
	WriteSingleReg(SYNC1, SMARTRF_SETTING_SYNC1); 
	// SYNC0: low byte of Sync Word
	WriteSingleReg(SYNC0, SMARTRF_SETTING_SYNC0); 
	// PKTLEN: Packet Length in fixed mode, Maximum Length in variable-length mode
	WriteSingleReg(PKTLEN, SMARTRF_SETTING_PKTLEN);
	// PKTCTRL1: status bytes appended to the packet
	WriteSingleReg(PKTCTRL1, SMARTRF_SETTING_PKTCTRL1); 
	// PKTCTRL0: Fixed-Length Mode, CRC    
	WriteSingleReg(PKTCTRL0, SMARTRF_SETTING_PKTCTRL0); 
	// ADDR: Address for packet filtration
	WriteSingleReg(ADDR, SMARTRF_SETTING_ADDR);
	// CHANNR: 8-bit channel number, freq = base freq + CHANNR * channel spacing
	WriteSingleReg(CHANNR, SMARTRF_SETTING_CHANNR);
	// FSCTRL1: Frequency Synthesizer Control (refer to User's Guide/SmartRF Studio)
	WriteSingleReg(FSCTRL1, SMARTRF_SETTING_FSCTRL1);
	// FSCTRL0: Frequency Synthesizer Control (refer to User's Guide/SmartRF Studio)
	WriteSingleReg(FSCTRL0, SMARTRF_SETTING_FSCTRL0);
	// FREQ2: base frequency, high byte
	WriteSingleReg(FREQ2, SMARTRF_SETTING_FREQ2);
	// FREQ1: base frequency, middle byte
	WriteSingleReg(FREQ1, SMARTRF_SETTING_FREQ1);
	// FREQ0: base frequency, low byte      
	WriteSingleReg(FREQ0, SMARTRF_SETTING_FREQ0);
	// MDMCFG4: modem configuration (refer to User's Guide/SmartRF Studio)   
	WriteSingleReg(MDMCFG4, SMARTRF_SETTING_MDMCFG4);
	// MDMCFG3: modem configuration (refer to User's Guide/SmartRF Studio)
	WriteSingleReg(MDMCFG3, SMARTRF_SETTING_MDMCFG3);
	// MDMCFG2: modem configuration (refer to User's Guide/SmartRF Studio)    
	WriteSingleReg(MDMCFG2, SMARTRF_SETTING_MDMCFG2);
	// MDMCFG1: modem configuration (refer to User's Guide/SmartRF Studio)      
	WriteSingleReg(MDMCFG1, SMARTRF_SETTING_MDMCFG1);
	// MDMCFG0: modem configuration (refer to User's Guide/SmartRF Studio)
	WriteSingleReg(MDMCFG0, SMARTRF_SETTING_MDMCFG0);
	// DEVIATN: modem deviation setting (refer to User's Guide/SmartRF Studio)
	WriteSingleReg(DEVIATN, SMARTRF_SETTING_DEVIATN);
	// MCSM2: Main Radio Control State Machine Conf. : timeout for sync word search disabled      
	WriteSingleReg(MCSM2, SMARTRF_SETTING_MCSM2);
	// MCSM1: CCA signals when RSSI below threshold, stay in RX after packet has been received       
	WriteSingleReg(MCSM1, SMARTRF_SETTING_MCSM1);
	// MCSM0: Auto-calibrate when going from IDLE to RX or TX (or FSTXON )
	WriteSingleReg(MCSM0, SMARTRF_SETTING_MCSM0);
	// FOCCFG: Frequency Offset Compensation Conf.
	WriteSingleReg(FOCCFG, SMARTRF_SETTING_FOCCFG);
	// BSCFG: Bit Synchronization Conf.
	WriteSingleReg(BSCFG, SMARTRF_SETTING_BSCFG);    
	// AGCCTRL2: AGC Control    
	WriteSingleReg(AGCCTRL2, SMARTRF_SETTING_AGCCTRL2);
	// AGCCTRL1: AGC Control    
	WriteSingleReg(AGCCTRL1, SMARTRF_SETTING_AGCCTRL1); 
	// AGCCTRL0: AGC Control   
	WriteSingleReg(AGCCTRL0, SMARTRF_SETTING_AGCCTRL0);
	// WOREVT1: High Byte Event0 Timeout    
	WriteSingleReg(WOREVT1, SMARTRF_SETTING_WOREVT1); 
	// WOREVT0: High Byte Event0 Timeout    
	WriteSingleReg(WOREVT0, SMARTRF_SETTING_WOREVT0); 
	// WORCTL: Wave On Radio Control ****Feature unavailable in PG0.6****
	WriteSingleReg(WORCTRL, SMARTRF_SETTING_WORCTRL); 
	// FREND1: Front End RX Conf.
	WriteSingleReg(FREND1, SMARTRF_SETTING_FREND1);
	// FREND0: Front End TX Conf.    
	WriteSingleReg(FREND0, SMARTRF_SETTING_FREND0);
	// FSCAL3: Frequency Synthesizer Calibration (refer to User's Guide/SmartRF Studio)
	WriteSingleReg(FSCAL3, SMARTRF_SETTING_FSCAL3); 
	// FSCAL2: Frequency Synthesizer Calibration (refer to User's Guide/SmartRF Studio)    
	WriteSingleReg(FSCAL2, SMARTRF_SETTING_FSCAL2); 
	// FSCAL1: Frequency Synthesizer Calibration (refer to User's Guide/SmartRF Studio)      
	WriteSingleReg(FSCAL1, SMARTRF_SETTING_FSCAL1); 
	// FSCAL0: Frequency Synthesizer Calibration (refer to User's Guide/SmartRF Studio)     
	WriteSingleReg(FSCAL0, SMARTRF_SETTING_FSCAL0);
	//       
	WriteSingleReg(FSTEST, SMARTRF_SETTING_FSTEST);
	//
	WriteSingleReg(TEST2, SMARTRF_SETTING_TEST2);
	//
	WriteSingleReg(TEST1, SMARTRF_SETTING_TEST1);
	//
	WriteSingleReg(TEST0, SMARTRF_SETTING_TEST0);
}*/

// *************************************************************************************************
// @fn          radio_reset
// @brief       Reset radio core. 
// @param       none
// @return      none
// *************************************************************************************************
void radio_reset(void)
{
	volatile u16 i;
	u8 x;
	
	// Reset radio core
	Strobe(RF_SRES);
	// Wait before checking IDLE 
	for (i=0; i<100; i++);
	do {
		x = Strobe(RF_SIDLE);
	} while ((x&0x70)!=0x00);  
	
	// Clear radio error register
	RF1AIFERR = 0;
}


// *************************************************************************************************
// @fn          radio_powerdown
// @brief       Put radio to SLEEP mode. 
// @param       none
// @return      none
// *************************************************************************************************
void radio_powerdown(void)
{
  /* Chip bug: Radio does not come out of this SLEEP when put to sleep
   * using the SPWD cmd. However, it does wakes up if SXOFF was used to
   * put it to sleep.
   */
	// Powerdown radio
	Strobe(RF_SIDLE);
	Strobe(RF_SPWD);	
}



// *************************************************************************************************
// @fn          radio_sxoff
// @brief       Put radio to SLEEP mode (XTAL off only). 
// @param       none
// @return      none
// *************************************************************************************************
void radio_sxoff(void)
{
  /* Chip bug: Radio does not come out of this SLEEP when put to sleep
   * using the SPWD cmd. However, it does wakes up if SXOFF was used to
   * put it to sleep.
   */
	// Powerdown radio
	Strobe(RF_SIDLE);
	Strobe(RF_SXOFF);	
}


// *************************************************************************************************
// @fn          open_radio
// @brief       Prepare radio for RF communication. 
// @param       none
// @return      none
// *************************************************************************************************
void open_radio(void)
{
	// Reset radio core
	radio_reset();

	// Enable radio IRQ
	RF1AIFG &= ~BIT4;                         // Clear a pending interrupt
  	RF1AIE  |= BIT4;                          // Enable the interrupt  	
}




// *************************************************************************************************
// @fn          close_radio
// @brief       Shutdown radio for RF communication. 
// @param       none
// @return      none
// *************************************************************************************************
void close_radio(void)
{
	// Disable radio IRQ
	RF1AIFG = 0;
	RF1AIE  = 0; 
		
	// Reset radio core
	radio_reset();
	
	// Put radio to sleep
	radio_powerdown();
}


void radio_transmit(u8 *data, u8 length)
{	
	RF1AIES |= BIT9;                         
  	RF1AIFG &= ~BIT9;                         // Clear pending interrupts
  	RF1AIE |= BIT9;                           // Enable TX end-of-packet interrupt
 
	WriteBurstReg(RF_TXFIFOWR, data, length);    //Enviamos los datos a la direccion de la TXFIFO

	/*display_chars(LCD_SEG_L1_2_0, itoa(ReadSingleReg(TXBYTES), 3, 0), SEG_ON); //Estado actual de la FIFOTX, bytes ocupados (42=LENGHT + DATA)
	Timer0_A4_Delay(CONV_MS_TO_TICKS(500));	*/
 
  	Strobe( RF_STX );                         // Strobe STX --> enable TX
}


void radio_receive_on(void)
{
 
	/*RF1AIFG &= ~BITA;
  	RF1AIE  |= BITA;*/

  	RF1AIES |= BIT9;                          // Falling edge of RFIFG9
  	RF1AIFG &= ~BIT9;                         // Clear a pending interrupt
  	RF1AIE  |= BIT9;                          // Enable the interrupt 
	//Ponemos modo RX después de activar la interrupcion del vector 10/9
	receiving = 1;

	/*clear_line(LINE1);
	display_chars(LCD_SEG_L1_2_0, itoa(ReadSingleReg(RXBYTES), 3, 0), SEG_ON);  //Estado actual de la FIFORX, bytes ocupados*/

	Strobe(RF_SRX);   //Strobe SRX --> enable RX
	//while (ReadSingleReg(MARCSTATE) != 0x0D); //wait for Radio to go RX
}

void radio_receive_off(void)
{
	RF1AIE &= ~BIT9;                          // Disable RX interrupts
	RF1AIFG &= ~BIT9;                         // Clear pending IFG
 
	receiving = 0;
	// It is possible that ReceiveOff is called while radio is receiving a packet.
	// Therefore, it is necessary to flush the RX FIFO after issuing IDLE strobe
	// such that the RXFIFO is empty prior to receiving a packet.
	Strobe(RF_SIDLE);
	Strobe(RF_SFRX);   
}

//return tamaño real del paquete, quitando RSSI,LQI+CRC
u8 radio_receive(u8 *data)
{
	//Aquí entra si el paquete tiene el CRC OK
	u8 i;
	request.flag.radio_received = 0;
	if (beacon)
	{
		 for(i=1;i<RxBufferLength-2;i++) data[i-1] = packet_rx[i];
		 beacon = 0;
	}
	else for(i=1;i<RxBufferLength-2;i++) data[i-1] = packet_rx_ok[i];
	return (i-1);
}


// *************************************************************************************************
// @fn          GDOx_ISR
// @brief       GDO0/2 ISR to detect received packet. 
//				In BlueRobin mode:  capture packet end time and decode received packet
//				In SimpliciTI mode: go to SimpliciTI handler
// @param       none
// @return      none
// *************************************************************************************************
//pfs 
#ifdef __GNUC__
#include <signal.h>
interrupt (CC1101_VECTOR) radio_ISR(void)
#else
#pragma vector=CC1101_VECTOR
__interrupt void radio_ISR(void)
#endif
{
	u8 rf1aivec = RF1AIV;

	if (rf1aivec == RF1AIV_RFIFG9) //Interrupcion para cuando esta transmitiendo y/o recibiendo (Packet received or sent)
	{
		if (receiving)
		{
			RxBufferLength = ReadSingleReg(RXBYTES);
			/*display_chars(LCD_SEG_L1_2_0, itoa(RxBufferLength, 3, 0), SEG_ON); //16
			Timer0_A4_Delay(CONV_MS_TO_TICKS(500));*/
			if (RxBufferLength == 16) 
			{
				ReadBurstReg(RF_RXFIFORD, packet_rx, RxBufferLength);
				beacon = 1;
				if (packet_rx[CRC_LQI_IDX] & CRC_OK)
				{
				 	request.flag.radio_received = 1; //CRC_OK ?
				}
			}
			else if (RxBufferLength == 28) 
			{
				ReadBurstReg(RF_RXFIFORD, packet_rx_ok, RxBufferLength);
				beacon = 0;
				if (packet_rx_ok[27] & CRC_OK)
				{
				 	request.flag.radio_received = 1; //CRC_OK ?
				}
			}

			/*clear_line(LINE2);
			packet_rx = 	{25,
					0xFF,0xFF,0xFF,0xFF,
					0x02,0x02,0x02,0x02,
					'H','O','L','A', 
					'M','A','N','U','E','L','1','2',3','4','5','6','7',RSSI,LQI+CRC};

			packet_message[0] = packet_rx[13];	//H
			packet_message[1] = packet_rx[14];	//O
			packet_message[2] = packet_rx[15];  	//L
			packet_message[3] = packet_rx[16];	//A  
			display_chars(LCD_SEG_L2_3_0,packet_message, SEG_ON); 
			Timer0_A4_Delay(CONV_MS_TO_TICKS(500));*/
		}
		else //Trasmitiendo Fin de interrupcion TX
		{	
			RF1AIES &= ~BIT9; 
			RF1AIFG &= ~BIT9;
			RF1AIE &= ~BIT9;
		}
	}
	else if (rf1aivec == RF1AIV_RFIFG10) //Interrupcion para cuando esta recibiendo <-- NO FUNCIONA. ERRATASHEET RF1A8, solo funciona una vez
	{
		//RF1AIES &= ~BITA;
		RF1AIFG &= ~BITA;
		RF1AIE &= ~BITA;
		//request.flag.radio_received = 1;
	}
	// Forward to SimpliciTI interrupt service routine
	else if (is_rf())
	{
		MRFI_RadioIsr();
	}
	else // BlueRobin packet end interrupt service routine
	{		
		if (rf1aivec == RF1AIV_RFIFG9)
		{    
		//pfs
		;
		#ifndef ELIMINATE_BLUEROBIN
			if ((sBlueRobin.state == BLUEROBIN_SEARCHING) || (sBlueRobin.state == BLUEROBIN_CONNECTED))
			{
				BlueRobin_RadioISR_v();
			}
		#endif
		}
		else if (rf1aivec == RF1AIV_NONE) // RF1A interface interrupt (error etc.)
		{
			asm("	nop"); // break here
		}
	}
	// Exit from LPM3/LPM4 on RETI
	__bic_SR_register_on_exit(LPM3_bits);
}
