/*
             LUFA Library
     Copyright (C) Dean Camera, 2015.

  dean [at] fourwalledcubicle [dot] com
           www.lufa-lib.org
*/

/*
  Copyright 2015  Dean Camera (dean [at] fourwalledcubicle [dot] com)

  Permission to use, copy, modify, distribute, and sell this
  software and its documentation for any purpose is hereby granted
  without fee, provided that the above copyright notice appear in
  all copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaims all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/



#include "VirtualSerial.h"

extern USB_ClassInfo_CDC_Device_t VirtualSerial_CDC_Interface;  //at the bottom of this file
// It is essential, but the documentation didn't give me the information I needed to create one
//so I used the one from the example, but I don't feel good about it.

#define CONNECTED (VirtualSerial_CDC_Interface.State.ControlLineStates.HostToDevice & CDC_CONTROL_LINE_OUT_DTR)


static FILE USBSerialStream;  //This will become stdin and stdout


void bruces_usb_init(void);


int main(void)
{
	bruces_usb_init();  // get the USB stuff going


	while(1)
	{
               while(!CONNECTED)
               {
                  USB_USBTask();
                  CDC_Device_USBTask(&VirtualSerial_CDC_Interface);
               }

	} 
}


/* Minimal configuration for USB I/O through stdin and stdout.  Disable watchdog timer, */
/* call USB_init in LUFA library, creating a blocking stream (note it is important to be       */
/* blocked most of the time for the USB stuff to work correctly), and set stdin and stdout  */
/* to point to it. Next it enables interrupts and waits for the host to open the serial port     */

void bruces_usb_init(void)
{                   
	wdt_disable();     //Make sure the Watchdog doesn't reset us
	USB_Init();  // Lufa library call to initialize USB
	/* Create a stream for the interface */
	CDC_Device_CreateBlockingStream(&VirtualSerial_CDC_Interface,&USBSerialStream);
            stdin=&USBSerialStream;  //By setting stdin and stdout to point to the stream we
            stdout=&USBSerialStream; //can use regular printf and scanf calls
	GlobalInterruptEnable();      // interrupts need to be enabled
 }

/* The following two event handlers are important.  When the USB interface  */
/* gets the event, we need to use the CDC routines to deal with them           */ 

/* When the USB library throws a Configuration Changed event we need to call */
/* CDC_Device_ConfigureEndpoints                                                                    */
void EVENT_USB_Device_ConfigurationChanged(void)
{    CDC_Device_ConfigureEndpoints(&VirtualSerial_CDC_Interface); }

/* When we get a Control Request from the USB library we need to call         */
/* CDC_Device_ProcessControlRequest.                                                        */
void EVENT_USB_Device_ControlRequest(void)
{  CDC_Device_ProcessControlRequest(&VirtualSerial_CDC_Interface); }

/** This is taken directly from the VirtualSerial example.  Even with the aid of the  *
* Documentation I could not figure out all that is going on here.                             *
* It bothers me and I would *LOVE* to have someone find more information         *
*                                                                                                                               */
USB_ClassInfo_CDC_Device_t VirtualSerial_CDC_Interface =
	{
		.Config =
			{
				.ControlInterfaceNumber   = INTERFACE_ID_CDC_CCI,
				.DataINEndpoint           =
					{
						.Address          = CDC_TX_EPADDR,
						.Size             = CDC_TXRX_EPSIZE,
						.Banks            = 1,
					},
				.DataOUTEndpoint =
					{
						.Address          = CDC_RX_EPADDR,
						.Size             = CDC_TXRX_EPSIZE,
						.Banks            = 1,
					},
				.NotificationEndpoint =
					{
						.Address          = CDC_NOTIFICATION_EPADDR,
						.Size             = CDC_NOTIFICATION_EPSIZE,
						.Banks            = 1,
					},
			},
	};



