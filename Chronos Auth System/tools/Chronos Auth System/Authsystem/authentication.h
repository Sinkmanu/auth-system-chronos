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
