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

#ifndef __PACKET_H_
#define __PACKET_H_

// *************************************************************************************************
// Include section
#include "project.h"

// *************************************************************************************************
// Defines section

typedef struct packet {
	u8 dst[4];	// destino del paquete (Access Point)
	u8 src[4];	// ID del reloj
	u8 type;	// tipo de paquete (syn ack key)
	union {
		u8 raw[32];	
                struct { u16 id; u8 padd_id[14]; u8 number_encrypt[16]; } random;
	} payload;
	/*struct {
          u16 rssi:8;
          u16 lqi:7;
          u16 crc_ok:1;
	} status;	// checksum correcto o no, proporcionado por el firmware msp430. crc-16*/
} packet_t;

//#define PACKET_TRANSMIT_LENGTH 26
#define PACKET_TRANSMIT_LENGTH 42
#define PACKET_BEACON_LENGTH 14
//#define PACKET_TRANSMIT_LENGTH (sizeof(packet_t))
//#define PACKET_RECEIVE_LENGTH 28
#define PACKET_RECEIVE_LENGTH 44
//#define PACKET_RECEIVE_LENGTH (sizeof(packet_t) + 2)

#define PACKET_IS_ENCRYPTED(packet) (packet.type & 0x80)
#define PACKET_IS_SENT_BY_AP(packet) (packet.type & 0x10)
#define PACKET_IS_SENT_BY_WATCH(packet) (!(packet.type & 0x10))

#define PACKET_TYPE_AUTH 0x01 
#define PACKET_TYPE_OK 0x02
#define PACKET_TYPE_BEACON  0x03
#define PACKET_TYPE_REGISTER 0x04

// pattern used in sec packet to verify that door really have the correct AES key
#define PACKET_PATTERN_SEC "Hello World"

// Control de errores al recibir un paquete

#define PACKET_SIN_ERROR  (0u)
#define PACKET_ERROR_LENGTH (1u)
#define PACKET_ERROR_ADDR (2u)
#define PACKET_ERROR_TYPE (3u)

#endif /*_PACKET_H_*/
