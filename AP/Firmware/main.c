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
// Main function
// *************************************************************************************************


// *************************************************************************************************
// Include section
#include "project.h"
#include "timer1.h"
#include "BM_API.h"
#include "simpliciti.h"
#include "flash.h"

#include "hal_types.h"
#include "usb_uart.h"
#include "hal_board.h"
#include "hal_led.h"
#include "hal_uart.h"

#include "AuthSystem/authentication.h"


// *************************************************************************************************
// Defines section

// Service USB process directly during initial enumeration
#define ENUMERATION_TIME    (20000u)


// *************************************************************************************************
// Global Variable section

// Normal operation
unsigned char system_status        = HW_IDLE;
/* Auth System Protocol */
unsigned char authentication_on = 0;
unsigned char authentication_start_now = 0;
unsigned char send_packet = 0;
unsigned char authentication_listen = 0;
unsigned char send_ack = 0;
u32 random;

//Auth Variables
u8 data_id[18];
u8 packet_ack[16];

// SimpliciTI Sync variables
u8 simpliciti_sync_buffer[BM_SYNC_DATA_LENGTH];
u8 simpliciti_sync_buffer_status;

// Test
u8  frequoffset    = 0;


// *************************************************************************************************
// Extern variable section
extern u8 rf_tx_over;     // Flag to indicate last byte has been sent
extern unsigned char usb_buffer[USB_MAX_MESSAGE_LENGTH+2];
extern unsigned char usb_sendack;


// *************************************************************************************************
// Extern functions section

// SimpliciTI RF ISR
extern void MRFI_RfIsr(void);

// *************************************************************************************************
// Function prototype section
void simpliciti_config(void);

// *************************************************************************************************
// Implementation

void main(void)
{  
 u16 enumeration = 0;
  u16 i = 0;
 // u8  usb_service_is_disabled = 0;
  u8  cal;

#ifdef CC1111EM
  // P1.1 (LED) to output, default state is on
  P1DIR |= BIT1; 
  LED_ON;
  
  #ifdef DEBUG_OUTPUT  
    // P2.0 to output
    P2DIR |= BIT0; 
    P2_0 &= ~BIT0; 
    
    // P0.5 - P0.0 to output
    P0DIR |= BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0;
    P0_5 &= ~BIT0; 
    P0_4 &= ~BIT0; 
    P0_3 &= ~BIT0; 
    P0_2 &= ~BIT0; 
    P0_1 &= ~BIT0; 
    P0_0 &= ~BIT0; 
  #endif  

#else
  // P1.0 (LED) to output '1'
  P1DIR |= BIT0; 
  LED_ON;
  // P1.4 (TP) to output '0'
  P1DIR |= BIT4; 
  TP_L;
  
  // P0.0 - P0.5 to output (unused)
  P0DIR |= BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0;
  // P1.5, P1.6, P1.7 to output (unused)
  P1DIR |= BIT5 | BIT6 | BIT7;
  // P2.0, P2.3, P2.4 to output (unused)
  P2DIR |= BIT0 | BIT3 | BIT4;
#endif
  
  // Clock control
  // Default settings are 0x5C
  SLEEP &= ~(BIT2 | BIT1 | BIT0);   // PM0, both OSC ON   
  asm("NOP");
  while(!(SLEEP & (BIT6 | BIT5)));  // Wait for both OSC to be stable
  for (i=0; i<20000; i++);          // Simple delay
  CLKCON &= ~BIT6;                  // Crystal on 
  CLKCON &= ~(0x3F);                // clkspd=000, tickspd=000
  CLKCON |= BIT3 | BIT4 | BIT5;     // clkspd = 24MHz, tickspd = 24MHz/128=187500Hz
  while(!(SLEEP & (BIT6 | BIT5)));  // Wait for both OSC to be stable
  for (i=0; i<20000; i++);          // Simple delay  
  
  // Reset simpliciti_data
  simpliciti_data[0] = 0xFF;
  
  // Get calibration data from memory - range check added to prevent wrong calibration
  cal = flash_byte_read(0x7FF0);
  if (cal == 0x00) frequoffset = flash_byte_read(0x7FF2);
  if ((frequoffset > 30) && (frequoffset < (256-30))) frequoffset = 0;
  
  // Priority levels: USB, Timer1 (3) -> RF (3) -> Timer4 (1) -> others (0)
  IP1 |= BIT5 + BIT1 + BIT0 + BIT4;
  IP0 |= BIT5 + BIT1 + BIT0;

#ifdef TX_SIMPLE_TEST
  // TX out only
  test_on=1;
  rftest_radio_init(); 
  start_continuous_tx(); 
  while(1); 
#endif
#ifdef TX_MP_TEST
  test_on=1;
  test_step = 1;
  test_step_over = 0;
#endif
  
  // Init USB driver and enable global IRQ
  usbUartInit(115200);
  
  // Service USB functions normally during enumeration phase
  while ( enumeration < ENUMERATION_TIME ) 
  {
    usbUartProcess();
    enumeration++;
  }
  LED_OFF;
  
  // After enumeration start Timer4 IRQ to service USB driver from now on
  // f=187500Hz/64/5=3kHz/5 --> 1.7ms / IRQ  
  T4CCTL0 = 0x44;
  T4CC0 = 0x04;
  T4CTL = 0xDE;
  INT_ENABLE(INUM_T4, INT_ON);  

  // Enable interrupts
  INT_GLOBAL_ENABLE(TRUE);  
  
   // Main control loop
  LED_OFF;
  authentication_config();
  while(1) 
  { 
    if (send_packet) 
    {
      authentication_on = 0;
      system_status = HW_SENDING;
      send_rand(random);
      send_packet=0;
    }
    else if (authentication_on)
    {
       send_packet = 0;
       system_status = HW_AUTHENTICATION;
       //authentication_config();
       send_rand(random);
       authentication_main(random);
     }
     else if (authentication_listen)
     {
       authentication_listen = 0;
       system_status = HW_AUTHENTICATION;
       send_rand(random);
       authentication_main(random);
      }
     else if (send_ack)
     {
       send_ack = 0;
       system_status = HW_AUTHENTICATION;
       send_ack_radio(packet_ack);
     }
      else {
        system_status = HW_IDLE;
    }
  }

}

// *************************************************************************************************
// Decode received command, extract data and trigger actions.
// *************************************************************************************************
void usb_decode(void)
{
  u8 i,tam;
  // Check if start marker is set
  if (usb_buffer[PACKET_BYTE_START] != 0xFF) return;

  // Check command code
  switch (usb_buffer[PACKET_BYTE_CMD])
  {
    // Generic commands
    case BM_GET_PRODUCT_ID:   usb_buffer[PACKET_BYTE_FIRST_DATA+3] = (u8)(PRODUCT_ID>>24);
                              usb_buffer[PACKET_BYTE_FIRST_DATA+2] = (u8)(PRODUCT_ID>>16);
                              usb_buffer[PACKET_BYTE_FIRST_DATA+1] = (u8)(PRODUCT_ID>>8);
                              usb_buffer[PACKET_BYTE_FIRST_DATA]   = (u8)(PRODUCT_ID); 
                              break;
    case BM_GET_STATUS:       usb_buffer[PACKET_BYTE_FIRST_DATA] = system_status; // + 1; 
                              break;
    case START_AUTH:          /*if (!authentication_on) authentication_on = 1;
                              else authentication_on = 0;*/
                              if (authentication_listen) authentication_listen=0;
                              else {
                                random =  (usb_buffer[PACKET_BYTE_FIRST_DATA]*0x1000000) + 
                                         (usb_buffer[PACKET_BYTE_FIRST_DATA+1]*0x10000) + 
                                         (usb_buffer[PACKET_BYTE_FIRST_DATA+2]*0x100)  + 
                                          usb_buffer[PACKET_BYTE_FIRST_DATA+3] ;
                                authentication_listen = 1;
                              }
			      break;
    case SEND_AUTH:           if (!send_packet) 
                              {
                                 /* random = (u32)((usb_buffer[PACKET_BYTE_FIRST_DATA]<<24) + 
                                           (usb_buffer[PACKET_BYTE_FIRST_DATA+1]<<16) + 
                                           (usb_buffer[PACKET_BYTE_FIRST_DATA+2]<<8)  + 
                                            usb_buffer[PACKET_BYTE_FIRST_DATA+3]) ; */
                                random = (usb_buffer[PACKET_BYTE_FIRST_DATA]*0x1000000) + 
                                           (usb_buffer[PACKET_BYTE_FIRST_DATA+1]*0x10000) + 
                                           (usb_buffer[PACKET_BYTE_FIRST_DATA+2]*0x100)  + 
                                            usb_buffer[PACKET_BYTE_FIRST_DATA+3] ;
                                
                                  send_packet = 1;
                              }
                              break;
    case GET_ID_RAND:         tam = data2usb(data_id);
                              for (i=0; i<tam; i++) usb_buffer[PACKET_BYTE_FIRST_DATA+i] = data_id[i];
                              break;
                              
    case SEND_ACK:            if (!send_ack)
                              {
                                  send_ack = 1;
                                  for(i=0;i<16;i++) packet_ack[i] = usb_buffer[PACKET_BYTE_FIRST_DATA+i];
                              }
                              break;
    case STOP_AUTH:	      LED_OFF;
                              authentication_on = 0;
                              authentication_start_now = 0;
                              system_status = HW_IDLE;
                              break;
  }
  
  // Return packet with original data, but modified command byte (acknowledge)
  usb_sendack = 1;
  usb_buffer[PACKET_BYTE_CMD] = HW_NO_ERROR;
}

// *************************************************************************************************
// Forward RF IRQ to right handler 
// *************************************************************************************************
#pragma vector=RF_VECTOR
__interrupt void rf_ISR(void)
{
  //LED_ON;
  //if (authentication_on)
  //{
    radio_rf_ISR();  
  //}
}

// *************************************************************************************************
// Forward RF TX/RX IRQ to right handler 
// *************************************************************************************************
#pragma vector=RFTXRX_VECTOR
__interrupt void ISR_RFTXRX(void)
{
  //if (authentication_on)
  //{
    radio_rftxrx();    
  //}
}


// *************************************************************************************************
// Timer1 ISR
// *************************************************************************************************
#pragma vector=T1_VECTOR
__interrupt void t1Timer_ISR(void)
{
  // Clear IRCON.T1IF
  IRCON &= ~0x02;

  // Clear Timer 1 Channel 0-2 + overflow interrupt flag
  T1CTL &= ~0xF0;

  // Return immediately if timer not enabled
  if (!sTimer1.enable) return;

  // Return immediately if s/w int flag not reset
  if (sTimer1.iflag) return;
  
  // Set timer cycles, and call s/w int handler if no more cycles to set
  if (!set_timer1_cycles()) 
  {
    // Call BlueRobin TX function
    sTimer1.iflag = 1;  
  }
  
  // Increase global cycle count
  sTimer1.cycles++;
}


// *************************************************************************************************
// Timer4 ISR
// *************************************************************************************************
#pragma vector=T4_VECTOR
__interrupt void timer4_ISR(void)
{
  
  // Clear IRQ flag
  TIMIF &= ~BIT3;
  
  // Service USB functions
  usbUartProcess();
}


// *************************************************************************************************
// Assign all unused IRQ
// *************************************************************************************************
void catch_stray_irq(void)
{
  asm("nop");
}
#pragma vector=ADC_VECTOR
__interrupt void adc_ISR(void) { catch_stray_irq(); }
#pragma vector=URX0_VECTOR
__interrupt void urx0_ISR(void) { catch_stray_irq(); }
#pragma vector=URX1_VECTOR
__interrupt void urx1_ISR(void) { catch_stray_irq(); }
#pragma vector=ENC_VECTOR
__interrupt void enc_ISR(void) { catch_stray_irq(); }
#pragma vector=ST_VECTOR
__interrupt void st_ISR(void) { catch_stray_irq(); }
#pragma vector=UTX0_VECTOR
__interrupt void utx0_ISR(void) { catch_stray_irq(); }
#pragma vector=DMA_VECTOR
__interrupt void dma_ISR(void) { catch_stray_irq(); }
#pragma vector=T2_VECTOR
__interrupt void t2_ISR(void) { catch_stray_irq(); }
#pragma vector=UTX1_VECTOR
__interrupt void utx1_ISR(void) { catch_stray_irq(); }
#pragma vector=P1INT_VECTOR
__interrupt void p1_ISR(void) { catch_stray_irq(); }
#pragma vector=WDT_VECTOR
__interrupt void wdt_ISR(void) { catch_stray_irq(); }
