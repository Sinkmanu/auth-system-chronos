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

#ifndef __AUTHENTICATION_H
#define __AUTHENTICATION_H

// Macros
#define getFlag(val, flag)		            ((val&flag)==flag)
#define setFlag(val, flag)		            (val|=flag)
#define clearFlag(val, flag)		            (val&=(~flag))
#define toggleFlag(val, flag)		            (val^=flag)


// Functions
extern void authentication_main(u32 random);
extern void authentication_config(void);
extern void AuthRxModeOn(void);
extern void AuthRxModeOff(void);
extern void radio_transmit(void);
extern void radio_receive(void);


extern u8 data2usb(u8 data[18]); //Ejemplo. Final = 16 bytes!
extern void send_example(void);
extern void send_rand(u32 rand);
extern void send_ack_radio(u8 randaes[16]);

extern void radio_rf_ISR(void);
extern void radio_rftxrx(void);

// Flag for status information and external control through USB driver
extern volatile unsigned char auth_flag;

#define AUTH_STATUS_LINKING     		    (BIT0) 	
#define AUTH_STATUS_LINKED	        	    (BIT1)
#define AUTH_STATUS_ERROR		            (BIT2)
#define AUTH_TRIGGER_SEND_DATA 	                    (BIT3)
#define AUTH_TRIGGER_RECEIVED_DATA 	            (BIT4)
#define AUTH_TRIGGER_STOP		            (BIT5)
#define AUTH_TRIGGER_SEND_CMD                       (BIT6)

#endif
