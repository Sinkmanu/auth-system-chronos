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

#ifndef AUTHENTICATION_H_
#define AUTHENTICATION_H_

// *************************************************************************************************
// Include section

// *************************************************************************************************
// Defines section

#define TIMEOUT_SECONDS							(3u)
#define ADDR_AUTH		0x11223344
#define ADDR_AP			0x02020202

// *************************************************************************************************
// Prototypes section
extern void reset_auth(void);
extern void mx_auth(u8 line);
extern void sx_auth(u8 line);
extern void display_auth(u8 line, u8 update);

//Auth System Protocol

// *************************************************************************************************
// Global Variable section
struct Datos
{
	u8 view;
	u8 comunication_state; //Comunicación activada/desactivada
	u16 id;
	u8 key[16];
};


// *************************************************************************************************
// Extern section

#endif /*AUTHENTICATION_H_*/
