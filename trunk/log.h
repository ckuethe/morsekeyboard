#ifndef _LOG_H_
#define _LOG_H_
#define MAXBUF 80
		#include <avr/io.h>
		#include <avr/wdt.h>

		#include "Descriptors.h"
		#include "AVRopendousMorseKeyboard.h"

extern void USB_puts(char msg[]);

extern USB_KeyboardReport_Data_t buf[];
extern uint8_t buf_i;
extern uint8_t go;




#endif
