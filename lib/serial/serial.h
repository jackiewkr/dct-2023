#ifndef __SERIAL_H
#define __SERIAL_H
/* Copy of dajp1's example serial printing library using the USB port */

void SERIAL_init( void );

void SERIAL_send( char* msg, int len );

void SERIAL_send_newline( void );

#endif //__SERIAL_H
