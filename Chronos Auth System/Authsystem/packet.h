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

#ifndef _PACKET_H_
#define _PACKET_H_

// *************************************************************************************************
// Include section
#include "project.h"

// *************************************************************************************************
// Defines section

typedef struct packet {
	u8 dst[4];	// destino del paquete (Access Point), por defecto broadcast
	u8 src[4];	// ID del reloj
	u8 type;	// tipo de paquete (auth,ok or beacon)
	union {
		u8 raw[32];
		struct { u8 raw_ok[16]; u8 raw_padd[16]; } ack;					//prueba
		struct { u16 id; u8 padd_id[14]; u32 number; u8 padd_rand[12]; } syn;
		struct { u32 number; u8 seq[28]; } random;
	} payload;
} packet_t;

#define PACKET_TRANSMIT_LENGTH sizeof(packet_t)  //26  //nueva-> 42
//#define PACKET_RECEIVE_LENGTH  (sizeof(packet_t) - 1) // 25 //nueva->41
#define PACKET_RECEIVE_LENGTH	25
#define PACKET_BEACON_LENGTH	13

#define PACKET_TYPE_AUTH 0x01 			// start a session cifrando el numero aleatorio
#define PACKET_TYPE_OK 0x02   			// respuesta del AP.
#define PACKET_TYPE_BEACON 0x03 		// random number packet
#define PACKET_TYPE_PASS 0x04 			// peticion de password
#define PACKET_TYPE_PING 0x05			// ping packet ?????

/*Control de errores al recibir un paquete*/

#define PACKET_SIN_ERROR	(0u)
#define PACKET_ERROR_LENGTH	(1u)
#define PACKET_ERROR_ADDR	(2u)
#define PACKET_ERROR_TYPE	(3u)
#define NO_BEACON		(4u)

#define SEQUENCE_PATTERN_OK	"Authenticate End"


#endif /*_PACKET_H_*/
