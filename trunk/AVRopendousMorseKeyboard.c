/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.

  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com
*/

/*
  Copyright 2008 Denver Gingerich (denver [at] ossguy [dot] com)
  Additional code for Morse version by Opendous Inc.

  Permission to use, copy, modify, and distribute this software
  and its documentation for any purpose and without fee is hereby
  granted, provided that the above copyright notice appear in all
  copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaim all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/

/*
	AVRopendous Morse keyboard functionality by Opendous Inc.
	Based on the Keyboard demonstration application by Denver Gingerich,
	itself based on the example MyUSB Mouse demonstration app by Dean Camera.
*/

/*
	Keyboard demonstration application, using endpoint interrupts. This
	gives a simple reference application for implementing a USB Keyboard
	using the basic USB HID drivers in all modern OSes (i.e. no special
	drivers required). Control requests are also fully interrupt driven.

	On startup the system will automatically enumerate and function
	as a keyboard when the USB connection to a host is present.

	Time between HWB button presses determines which letter is typed.
*/

/*
	USB Mode:           Device
	USB Class:          Human Interface Device (HID)
	USB Subclass:       Keyboard
	Relevant Standards: USBIF HID Standard
	                    USBIF HID Usage Tables
	Usable Speeds:      Low Speed Mode, Full Speed Mode
*/
#define DEBUG




#ifdef DEBUG
#include "log.h"
#define puts(...) USB_puts(__VA_ARGS__)


#else
#include "AVRopendousMorseKeyboard.h"
#define puts(...)

#endif


/* Project Tags, for reading out using the ButtLoad project */
BUTTLOADTAG(ProjName,     "AVRopendous Morse Keyboard");
BUTTLOADTAG(BuildTime,    __TIME__);
BUTTLOADTAG(BuildDate,    __DATE__);
BUTTLOADTAG(MyUSBVersion, "MyUSB V" MYUSB_VERSION_STRING);

/* Global Variables */
USB_KeyboardReport_Data_t KeyboardReportData = {Modifier: 0, KeyCode: 0};
uint8_t temp = 0;
uint16_t timer1Value = 0;
uint16_t timer1OverflowCount = 0;
uint8_t firstEdge = 1;
uint8_t havePulse = 0;
uint16_t timer1Val1 = 0;
uint16_t timer1Val2 = 0;
uint16_t timer1pulselength = 0;


int main(void)
{
	/* Disable watchdog if enabled by bootloader/fuses */
	MCUSR = ~(1 << WDRF); /* Clear watchdog flag */
	wdt_reset(); /* Reset the watchdog */

	/* Disable Clock Division */
	SetSystemClockPrescaler(0);

	/* Hardware Initialization */
	DDRD = 0;
	PORTD = 0xFF; //input
	DDRB = 0;
	PORTB = 0xFF; //input
	DDRC = 0;
	PORTC |= (1 << PC2) | (1 << PC4) | (1 << PC5) | (1 << PC6) | (1 << PC7); //input only on PC2,4,5,6,7


	// enable Timer1
	TCCR1A = 0; //start the timer
	//TCCR1B = (1 << CS10); // no Timer1 prescaling, use CLK
	//TCCR1B = (1 << CS12); // prescale Timer1 by CLK/256
	TCCR1B = ((1 << CS12) | (1 << CS10)); // prescale Timer1 by CLK/1024 for 7812.5 Hz
	timer1Value = TCNT1; // TCNT1 needs to be read to start the timer
	TIMSK1 = (1 << TOIE1); // enable Timer1 overflow interrupt


	// enable interrupt for a change on PD7/HWB
	EICRB = ((0 << ISC71) | (1 << ISC70));
	EIMSK |= (1 << INT7);


	/* Initialize USB Subsystem  -- this also features a call to sei() to enable interrupts */
	USB_Init();

	/* Main program code loop */
	for (;;)
	{
		/* No main code -- all USB code is interrupt driven */
	}
}

EVENT_HANDLER(USB_Connect)
{
	/* Indicate USB enumerating */
}

EVENT_HANDLER(USB_Reset)
{
	/* Select the control endpoint */
	Endpoint_SelectEndpoint(ENDPOINT_CONTROLEP);

	/* Enable the endpoint SETUP interrupt ISR for the control endpoint */
	USB_INT_Enable(ENDPOINT_INT_SETUP);
}

EVENT_HANDLER(USB_Disconnect)
{
	/* Indicate USB not ready */
}

EVENT_HANDLER(USB_ConfigurationChanged)
{
	/* Setup Keyboard Keycode Report Endpoint */
	Endpoint_ConfigureEndpoint(KEYBOARD_EPNUM, EP_TYPE_INTERRUPT,
		                       ENDPOINT_DIR_IN, KEYBOARD_EPSIZE,
	                           ENDPOINT_BANK_SINGLE);

	/* Enable the endpoint IN interrupt ISR for the report endpoint */
	USB_INT_Enable(ENDPOINT_INT_IN);

	/* Setup Keyboard LED Report Endpoint */
	Endpoint_ConfigureEndpoint(KEYBOARD_LEDS_EPNUM, EP_TYPE_INTERRUPT,
		                       ENDPOINT_DIR_OUT, KEYBOARD_EPSIZE,
	                           ENDPOINT_BANK_SINGLE);

	/* Enable the endpoint OUT interrupt ISR for the LED report endpoint */
	USB_INT_Enable(ENDPOINT_INT_OUT);

	/* Indicate USB connected and ready */
}

EVENT_HANDLER(USB_UnhandledControlPacket)
{
	/* Handle HID Class specific requests */
	switch (bRequest)
	{
		case REQ_GetReport:
			if (bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				/* Ignore report type and ID number value */
				Endpoint_Discard_Word();

				/* Ignore unused Interface number value */
				Endpoint_Discard_Word();

				/* Read in the number of bytes in the report to send to the host */
				uint16_t wLength = Endpoint_Read_Word_LE();

				/* If trying to send more bytes than exist to the host, clamp the value at the report size */
				if (wLength > sizeof(KeyboardReportData))
				  wLength = sizeof(KeyboardReportData);

				Endpoint_ClearSetupReceived();

				/* Write the report data to the control endpoint */
				Endpoint_Write_Control_Stream_LE(&KeyboardReportData, wLength);

				/* Finalize the transfer, acknowedge the host error or success OUT transfer */
				Endpoint_ClearSetupOUT();
			}

			break;
	}
}


ISR(ENDPOINT_PIPE_vect)
{
	/* Check if the control endpoint has recieved a request */
	if (Endpoint_HasEndpointInterrupted(ENDPOINT_CONTROLEP))
	{
		/* Clear the endpoint interrupt */
		Endpoint_ClearEndpointInterrupt(ENDPOINT_CONTROLEP);

		/* Process the control request */
		USB_USBTask();

		/* Handshake the endpoint setup interrupt - must be after the call to USB_USBTask() */
		USB_INT_Clear(ENDPOINT_INT_SETUP);
	}

	/* Check if keyboard endpoint has interrupted */
	if (Endpoint_HasEndpointInterrupted(KEYBOARD_EPNUM))
	{


		if (go) {
			KeyboardReportData.KeyCode = buf[buf_i].KeyCode;
			KeyboardReportData.Modifier = buf[buf_i].Modifier;
			if (buf[buf_i].KeyCode == 0) {
				go = 0;
			}
			buf_i = (buf_i + 1) % MAXBUF;
		} else {
			buf_i = 0;


		/* TODO: process pulse lengths into characters */

			if (havePulse) { // only run this if we have a full pulse

				// if Timer1 has overflowed, make sure to do the right subtraction
				if (timer1Val2 > timer1Val1) {
					timer1pulselength = timer1Val2 - timer1Val1;
				} else {
					timer1pulselength = timer1Val1 - timer1Val2;
				}
				char s[50];
				sprintf(s, "%d;", timer1pulselength); // is that bad?
				puts(s);
				if (timer1pulselength > 3000) {
					KeyboardReportData.KeyCode =  0x04; //a
				} else {
					KeyboardReportData.KeyCode =  0x05; //b
				}

				havePulse = 0;
			}
		}



		/* Clear the Keyboard Report endpoint interrupt and select the endpoint */
		Endpoint_ClearEndpointInterrupt(KEYBOARD_EPNUM);
		Endpoint_SelectEndpoint(KEYBOARD_EPNUM);

		/* Write Keyboard Report Data */
		Endpoint_Write_Stream_LE(&KeyboardReportData, sizeof(KeyboardReportData));

		/* Handshake the IN Endpoint - send the data to the host */
		Endpoint_ClearCurrentBank();

		/* Clear the report data afterwards */
		KeyboardReportData.Modifier = 0;
		KeyboardReportData.KeyCode  = 0;

		/* Clear the endpoint IN interrupt flag */
		USB_INT_Clear(ENDPOINT_INT_IN);
	}

	/* Check if Keyboard LED status Endpoint has interrupted */
	if (Endpoint_HasEndpointInterrupted(KEYBOARD_LEDS_EPNUM))
	{
		/* Clear the endpoint OUT interrupt flag */
		USB_INT_Clear(ENDPOINT_INT_OUT);

		/* Clear the Keyboard LED Report endpoint interrupt and select the endpoint */
		Endpoint_ClearEndpointInterrupt(KEYBOARD_LEDS_EPNUM);
		Endpoint_SelectEndpoint(KEYBOARD_LEDS_EPNUM);

		/* Read in the LED report from the host */

		/* Handshake the OUT Endpoint - clear endpoint and ready for next report */
		Endpoint_ClearCurrentBank();
	}
}


ISR(INT7_vect)
{
	// this interrupt will run every time an edge (rising or falling) is detected on PD7
	// determine the time between edges and assign a character to it
	// since the Timer1 clock is running at 7812.5 Hz, that gives a maximum ~8.3 seconds per pulse
	// for now, process actual character assignment in USB interrupt to avoid to keep this ISR short

	timer1Value = TCNT1;

	// note that since HWB is pulled up, we need only worry about how long a LOW level lasts
	if (~PIND & (1 << PIND7)) { // LOW level edge - falling edge
		timer1Val1 = timer1Value;
		firstEdge = 0;
	} else if (PIND & (1 << PIND7)) { // HIGH level edge - rising edge
		timer1Val2 = timer1Value;

		// debounce key press (i.e, discard it if it is too short)
		// if Timer1 has overflowed, make sure to do the right subtraction
		if (timer1Val2 > timer1Val1) {
			timer1pulselength = timer1Val2 - timer1Val1;
		} else {
			timer1pulselength = timer1Val1 - timer1Val2;
		}

		// button press was too short - it is noise, so undo above
		if (timer1pulselength < 2) { // 7812 is 1 second, so 10 is ~0.0013s, or ~1.3ms, and 2 ~= 0.26ms
			firstEdge = 0;
			havePulse = 0;
		} else { // button press was correct, have a pulse's second edge
			firstEdge = 1;
			havePulse = 1;
		}
	}
}


ISR(TIMER1_OVF_vect)
{
	timer1OverflowCount++; // TODO: this overflows to zero automatically, right?
}
