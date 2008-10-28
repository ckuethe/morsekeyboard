/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com
*/

/*
  Copyright 2008 Denver Gingerich (denver [at] ossguy [dot] com)
  Additional code by Opendous Inc.

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
	Keyboard demonstration application by Denver Gingerich.

	This example is based on the MyUSB Mouse demonstration application,
	written by Dean Camera.
*/

/*
	Keyboard demonstration application, using endpoint interrupts. This
	gives a simple reference application for implementing a USB Keyboard
	using the basic USB HID drivers in all modern OSes (i.e. no special
	drivers required). Control requests are also fully interrupt driven.
	It is boot protocol compatible, and thus works under compatible BIOS as
	if it was a native keyboard (e.g. PS/2).
	
	On startup the system will automatically enumerate and function
	as a keyboard when the USB connection to a host is present. To use
	the keyboard example, manipulate the joystick to send the letters
	a, b, c, d and e. See the USB HID documentation for more information
	on sending keyboard event and keypresses.
*/

/*
	USB Mode:           Device
	USB Class:          Human Interface Device (HID)
	USB Subclass:       Keyboard
	Relevant Standards: USBIF HID Standard
	                    USBIF HID Usage Tables 
	Usable Speeds:      Low Speed Mode, Full Speed Mode
*/

#include "AVRopendousMorseKeyboard.h"
#include "RingBuff.h"

/* Project Tags, for reading out using the ButtLoad project */
BUTTLOADTAG(ProjName,     "MyUSB AVRopendous Keyboard App");
BUTTLOADTAG(BuildTime,    __TIME__);
BUTTLOADTAG(BuildDate,    __DATE__);
BUTTLOADTAG(MyUSBVersion, "MyUSB V" MYUSB_VERSION_STRING);

/* Global Variables */
bool      UsingReportProtocol = true;
uint8_t   IdleCount           = 0;
uint16_t  IdleMSRemaining     = 0;

/* Morse Keyboard related: */
uint8_t temp = 0;
uint16_t timer1Value = 0;
uint16_t timer1OverflowCount = 0;
uint8_t firstEdge = 1;
uint8_t havePulse = 0;
uint16_t timer1Val1 = 0;
uint16_t timer1Val2 = 0;
uint16_t timer1pulselength = 0;


RingBuff_t print_buffer;
RingBuff_t code_buffer;
RingBuff_t time_buffer;


int main(void)
{
	/* Disable watchdog if enabled by bootloader/fuses */
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	/* Disable Clock Division */
	SetSystemClockPrescaler(0);

	/* Hardware Initialization */
	DDRD = 0;
	PORTD = 0xFF; //input
	DDRB = 0;
	PORTB = 0xFF; //input
	DDRC = 0;
	PORTC |= (1 << PC2) | (1 << PC4) | (1 << PC5) | (1 << PC6) | (1 << PC7); //input only on PC2,4,5,6,7

	/* Millisecond timer initialization, with output compare interrupt enabled for the idle timing */
	OCR0A  = 0x7D;
	TCCR0A = (1 << WGM01);
	TCCR0B = ((1 << CS01) | (1 << CS00));
	TIMSK0 = (1 << OCIE0A);

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

	/* Ringbuffer Initialization */
	Buffer_Initialize(&print_buffer);
	Buffer_Initialize(&code_buffer);
	Buffer_Initialize(&time_buffer);

	/* Initialize USB Subsystem */
	USB_Init();

	/* Main program code loop */
	for (;;)
	{
		/* No main code -- all USB code is interrupt driven */
	}
}

EVENT_HANDLER(USB_Connect)
{
	/* Default to report protocol on connect */
	UsingReportProtocol = true;
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
	//
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

}

EVENT_HANDLER(USB_UnhandledControlPacket)
{
	/* Handle HID Class specific requests */
	switch (bRequest)
	{
		case REQ_GetReport:
			if (bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				USB_KeyboardReport_Data_t KeyboardReportData;

				/* Create the next keyboard report for transmission to the host */
				GetNextReport(&KeyboardReportData);

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
		case REQ_SetReport:
			if (bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				Endpoint_ClearSetupReceived();

				/* Wait until the LED report has been sent by the host */
				while (!(Endpoint_IsSetupOUTReceived()));

				/* Read in the LED report from the host */
				uint8_t LEDStatus = Endpoint_Read_Byte();

				/* Process the incomming LED report */
				ProcessLEDReport(LEDStatus);

				/* Clear the endpoint data */
				Endpoint_ClearSetupOUT();

				/* Wait until the host is ready to receive the request confirmation */
				while (!(Endpoint_IsSetupINReady()));

				/* Handshake the request by sending an empty IN packet */
				Endpoint_ClearSetupIN();
			}

			break;
		case REQ_GetProtocol:
			if (bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				Endpoint_ClearSetupReceived();

				/* Write the current protocol flag to the host */
				Endpoint_Write_Byte(UsingReportProtocol);

				/* Send the flag to the host */
				Endpoint_ClearSetupIN();
			}

			break;
		case REQ_SetProtocol:
			if (bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				/* Read in the wValue parameter containing the new protocol mode */
				uint16_t wValue = Endpoint_Read_Word_LE();

				/* Set or clear the flag depending on what the host indicates that the current Protocol should be */
				UsingReportProtocol = (wValue != 0x0000);

				Endpoint_ClearSetupReceived();

				/* Send an empty packet to acknowedge the command */
				Endpoint_ClearSetupIN();
			}

			break;
		case REQ_SetIdle:
			if (bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				/* Read in the wValue parameter containing the idle period */
				uint16_t wValue = Endpoint_Read_Word_LE();

				Endpoint_ClearSetupReceived();

				/* Get idle period in MSB */
				IdleCount = (wValue >> 8);

				/* Send an empty packet to acknowedge the command */
				Endpoint_ClearSetupIN();
			}

			break;
		case REQ_GetIdle:
			if (bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
			{		
				Endpoint_ClearSetupReceived();

				/* Write the current idle duration to the host */
				Endpoint_Write_Byte(IdleCount);

				/* Send the flag to the host */
				Endpoint_ClearSetupIN();
			}

			break;
	}
}

ISR(TIMER0_COMPA_vect, ISR_BLOCK)
{
	/* One millisecond has elapsed, decrement the idle time remaining counter if it has not already elapsed */
	if (IdleMSRemaining)
	  IdleMSRemaining--;
}

uint8_t itokeycode(uint8_t i) {
    uint8_t res = 28; // y
    switch (i)
    {
        case 10:
                res = 0x04; // a
                break;
        case 11:
                res = 0x05;
                break;
        case 12:
                res = 0x06;
                break;
        case 13:
                res = 0x07;
                break;
        case 14:
                res = 0x08;
                break;
        case 15:
                res = 0x09;
                break;
        case 0:
                res = 39; //0
                break;
        case 1:
                res = 30;
                break;
        case 2:
                res = 31;
                break;
        case 3:
                res = 32;
                break;
        case 4:
                res = 33;
                break;
        case 5:
                res = 34;
                break;
        case 6:
                res = 35;
                break;
        case 7:
                res = 36;
                break;
        case 8:
                res = 37;
                break;
        case 9:
                res = 38;
                break;
        default:
                res = 27; //x
                break;
    }
    return res;
}

void USBputs_i(uint8_t i) 
{
    uint8_t u = (i & 0xf0) >> 4;
    uint8_t l = (i & 0x0f);
    if (u) 
        Buffer_StoreElement(&print_buffer, (uint8_t)itokeycode(u));
    Buffer_StoreElement(&print_buffer, (uint8_t)itokeycode(l));
}

void USBputs_i16(uint16_t i) 
{
    uint8_t u = (i & 0xff00) >> 8;     
    uint8_t l = (i & 0x00ff);     
    if (u) 
        USBputs_i(u);
    USBputs_i(l);
}

void USBputs(char msg[]) 
{
    int i = 0;
    uint8_t prevch = 0;
    while (msg[i] != 0) {
        if (prevch == (uint8_t)msg[i]) {
			Buffer_StoreElement(&print_buffer, (uint8_t)0);
            prevch = 0;
            continue;
        }
		if (((uint8_t)msg[i] <= 122) && ((uint8_t)msg[i] >= 97)) // a- z
			Buffer_StoreElement(&print_buffer, (uint8_t)msg[i] - 93);
		else if (((uint8_t)msg[i] <= 90) && ((uint8_t)msg[i] >= 65)) { // A -Z
			Buffer_StoreElement(&print_buffer, (uint8_t)msg[i] - 61);
			//.Modifier = (1 << 5);
		}
		else if (((uint8_t)msg[i] <= 57) && ((uint8_t)msg[i] >= 49)) // 1 - 9
			Buffer_StoreElement(&print_buffer, (uint8_t)msg[i] - 19);
		else if ((uint8_t)msg[i] == 48) // 0
			Buffer_StoreElement(&print_buffer, (uint8_t)39);
		else if ((uint8_t)msg[i] == 32)
			Buffer_StoreElement(&print_buffer, (uint8_t)44); //space
        prevch = (uint8_t)msg[i];
        i++;


    }
}

bool GetNextReport(USB_KeyboardReport_Data_t* ReportData)
{
	uint8_t InputChanged = true;

	/* Clear the report contents */
	memset(ReportData, 0, sizeof(USB_KeyboardReport_Data_t));

	ReportData->KeyCode[0] = 0x00;

    if (print_buffer.Elements) {
        ReportData->KeyCode[0] = (uint8_t)Buffer_GetElement(&print_buffer);
    } else {

        /* TODO: process pulse lengths into characters */
        if (havePulse) { // only run this if we have a full pulse
            // if Timer1 has overflowed, make sure to do the right subtraction
            if (timer1Val2 > timer1Val1) {
                timer1pulselength = timer1Val2 - timer1Val1;
            } else {
                timer1pulselength = timer1Val1 - timer1Val2;
            }

            if (timer1pulselength > 3000) {
                ReportData->KeyCode[0] =  0x04; //a
            } else {
                ReportData->KeyCode[0] =  0x05; //b
            }

            havePulse = 0;
        }



        /* keep keyboard code to aid testing */
            if (~PIND & (1 << PIND0)) {
            Buffer_StoreElement(&print_buffer, 0x1E); 
            Buffer_StoreElement(&print_buffer, 0x1F); 
            Buffer_StoreElement(&print_buffer, 0x20); 
        } else if (~PIND & (1 << PIND1)) {
            USBputs("aaaaa this is it aaaaa pin --> ");
            USBputs_i(0x14);
        } else if (~PIND & (1 << PIND2)) {
            ReportData->KeyCode[0] = 0x06; //c
        } else if (~PIND & (1 << PIND3)) {
            ReportData->KeyCode[0] = 0x07; //d
        } else if (~PIND & (1 << PIND4)) {
            ReportData->KeyCode[0] = 0x08; //e
        } else if (~PIND & (1 << PIND5)) {
            ReportData->KeyCode[0] = 0x09; //f
        } else if (~PIND & (1 << PIND6)) {
            ReportData->KeyCode[0] = 0x0A; //g
        /* do not deal with H as it is Keyer input */

        } else if (~PINB & (1 << PINB0)) {
            ReportData->KeyCode[0] = 0x0C; //i
        } else if (~PINB & (1 << PINB1)) {
            ReportData->KeyCode[0] = 0x0D; //j
        } else if (~PINB & (1 << PINB2)) {
            ReportData->KeyCode[0] = 0x0E; //k
        } else if (~PINB & (1 << PINB3)) {
            ReportData->KeyCode[0] = 0x0F; //l
        } else if (~PINB & (1 << PINB4)) {
            ReportData->KeyCode[0] = 0x10; //m
        } else if (~PINB & (1 << PINB5)) {
            ReportData->KeyCode[0] = 0x11; //n
        } else if (~PINB & (1 << PINB6)) {
            ReportData->KeyCode[0] = 0x12; //o
        } else if (~PINB & (1 << PINB7)) {
            ReportData->KeyCode[0] = 0x13; //p

        } else if (~PINC & (1 << PINC2)) {
            ReportData->KeyCode[0] = 0x18; //u
        } else if (~PINC & (1 << PINC4)) {
            ReportData->KeyCode[0] = 0x17; //t
        } else if (~PINC & (1 << PINC5)) {
            ReportData->KeyCode[0] = 0x16; //s
        } else if (~PINC & (1 << PINC6)) {
            ReportData->KeyCode[0] = 0x15; //r
        } else if (~PINC & (1 << PINC7)) {
            ReportData->KeyCode[0] = 0x14; //q
        }

     }
	/* Return whether the new report is different to the previous report or not */
	return InputChanged;
}


ISR(INT7_vect)
{
	// this interrupt will run every time an edge (rising or falling) is detected on PD7
	// determine the time between edges and assign a character to it
	// since the Timer1 clock is running at 7812.5 Hz, that gives a maximum ~8.3 seconds per pulse
	// process actual character assignment elsewhere to keep this ISR short

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


void ProcessLEDReport(uint8_t LEDReport)
{
	if (LEDReport & 0x01) {  // NUM Lock
		// do nothing, no LEDs connected
	}

	if (LEDReport & 0x02) { // CAPS Lock
		// do nothing, no LEDs connected
	}

	if (LEDReport & 0x04) { // SCROLL Lock
		// do nothing, no LEDs connected
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
		USB_KeyboardReport_Data_t KeyboardReportData;
		bool                      SendReport;
	
		/* Create the next keyboard report for transmission to the host */
		SendReport = GetNextReport(&KeyboardReportData);
	
		/* Check if the idle period is set and has elapsed */
		if (IdleCount && !(IdleMSRemaining))
		{
			/* Idle period elapsed, indicate that a report must be sent */
			SendReport = true;
			
			/* Reset the idle time remaining counter, must multiply by 4 to get the duration in milliseconds */
			IdleMSRemaining = (IdleCount << 2);
		}

		/* Check to see if a report should be issued */
		if (SendReport)
		{
			/* Select the Keyboard Report Endpoint */
			Endpoint_SelectEndpoint(KEYBOARD_EPNUM);

			/* Clear the endpoint IN interrupt flag */
			USB_INT_Clear(ENDPOINT_INT_IN);

			/* Clear the Keyboard Report endpoint interrupt */
			Endpoint_ClearEndpointInterrupt(KEYBOARD_EPNUM);

			/* Write Keyboard Report Data */
			Endpoint_Write_Stream_LE(&KeyboardReportData, sizeof(KeyboardReportData));

			/* Handshake the IN Endpoint - send the data to the host */
			Endpoint_ClearCurrentBank();
		}
	}

	/* Check if Keyboard LED status Endpoint has interrupted */
	if (Endpoint_HasEndpointInterrupted(KEYBOARD_LEDS_EPNUM))
	{
		/* Select the Keyboard LED Report Endpoint */
		Endpoint_SelectEndpoint(KEYBOARD_LEDS_EPNUM);

		/* Clear the endpoint OUT interrupt flag */
		USB_INT_Clear(ENDPOINT_INT_OUT);

		/* Clear the Keyboard LED Report endpoint interrupt */
		Endpoint_ClearEndpointInterrupt(KEYBOARD_LEDS_EPNUM);

		/* Read in the LED report from the host */
		uint8_t LEDStatus = Endpoint_Read_Byte();

		/* Process the incomming LED report */
		ProcessLEDReport(LEDStatus);

		/* Handshake the OUT Endpoint - clear endpoint and ready for next report */
		Endpoint_ClearCurrentBank();
	}
}