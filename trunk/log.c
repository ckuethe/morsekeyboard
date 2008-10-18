#ifndef _LOG_H_


#include "log.h"

USB_KeyboardReport_Data_t buf[] = {{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}};
uint8_t buf_i = 0;
uint8_t go = 0;

#endif

void USB_puts(char msg[]) {
	uint8_t i = 0;
	buf_i = 0;
	buf[i].KeyCode = msg[i];
	while (i<MAXBUF-1) {
		buf[i].Modifier = 0;
		if (((uint8_t)msg[i] <= 122) && ((uint8_t)msg[i] >= 97)) // a- z
			buf[i].KeyCode = (uint8_t)msg[i] - 93;
		else if (((uint8_t)msg[i] <= 90) && ((uint8_t)msg[i] >= 65)) { // A -Z
			buf[i].KeyCode = (uint8_t)msg[i] - 61;
			buf[i].Modifier = (1 << 5);
		}
		else if (((uint8_t)msg[i] <= 57) && ((uint8_t)msg[i] >= 49)) // 1 - 9
			buf[i].KeyCode = (uint8_t)msg[i] - 19;
		else if ((uint8_t)msg[i] == 48) // 0
			buf[i].KeyCode = 39;
		else if ((uint8_t)msg[i] == 32)
			buf[i].KeyCode = 44; // space
		else if ((uint8_t)msg[i] == 59) {
			buf[i].KeyCode = 40; //return
			buf[i+1].KeyCode = 0;
			buf[i+1].Modifier = 0;
			break;
		}
		else if (msg[i] == 0) {
			buf[i].KeyCode = 40; //return
			buf[i+1].KeyCode = 0;
			buf[i+1].Modifier = 0;
			break;

		}
		i = i + 1;
	}
	go = 1;

}

