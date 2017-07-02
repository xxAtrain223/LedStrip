/*
             LUFA Library
     Copyright (C) Dean Camera, 2017.

  dean [at] fourwalledcubicle [dot] com
           www.lufa-lib.org
*/

/*
  Copyright 2017  Dean Camera (dean [at] fourwalledcubicle [dot] com)

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

/** \file
 *
 *  Main source file for the VirtualSerial demo. This file contains the main tasks of
 *  the demo and is responsible for the initial application hardware configuration.
 */
#include <avr/wdt.h>
#include "VirtualSerial.h"
#include "light_ws2812.c"
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "atmel_bootloader.c"


/** LUFA CDC Class driver interface configuration and state information. This structure is
 *  passed to all CDC Class driver functions, so that multiple instances of the same class
 *  within a device can be differentiated from one another.
 */
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

/** Standard file stream for the CDC interface when set up, so that the virtual CDC COM port can be
 *  used like any regular character stream in the C APIs.
 */
 
#define led_len 120
static FILE USBSerialStream;

int uart_bytes_remaining;

struct cRGB led_default[led_len];
struct cRGB led_white[led_len];
struct cRGB led_cylon[led_len];
struct cRGB led_temp[1];

char command_buffer[32];
char command_buffer_index;
char command_buffer_len;

int led_addr = 0;

static inline int is_odd_A(int x) { return x & 1; }

/** Main program entry point. This routine contains the overall program flow, including initial
 *  setup of all components and the main program loop.
 */
int main(void)
{
	command_buffer_index = 0;
    command_buffer_len = 0;
    
	for(led_addr = 0 ; led_addr < led_len ; led_addr++) {
		if(is_odd_A(led_addr)) {
				led_default[led_addr].r = 127; led_default[led_addr].g = 0; led_default[led_addr].b = 0;
		} else {
				led_default[led_addr].r = 0; led_default[led_addr].g = 0; led_default[led_addr].b = 127;
		}
		led_white[led_addr].r = 255;led_white[led_addr].g = 255;led_white[led_addr].b = 255;
		led_cylon[led_addr].r = 0;led_cylon[led_addr].g = 0;led_cylon[led_addr].b = 0;
	}

	led_cylon[0].r = 255;led_cylon[0].g = 0;led_cylon[0].b = 0;
	led_cylon[1].r = 255;led_cylon[1].g = 0;led_cylon[1].b = 0;
	led_cylon[2].r = 255;led_cylon[2].g = 0;led_cylon[2].b = 0;	
	
	//Setup LED animation timer
	TCCR0A = 0x00;
	TCCR0B = 0x05;
	
	
	
	SetupHardware();

	/* Create a regular character stream for the interface so that it can be used with the stdio.h functions */
	CDC_Device_CreateStream(&VirtualSerial_CDC_Interface, &USBSerialStream);

	LEDs_SetAllLEDs(LEDMASK_USB_NOTREADY);
	GlobalInterruptEnable();
	
	DDRD = 0x08;
	PORTD = 0x00;
	

	

	ws2812_setleds     (led_default,led_len);
	
	for (;;)
	{
		//CheckJoystickMovement();

		/* Must throw away unused bytes from the host, or it will lock up while waiting for the device */
		uart_bytes_remaining = CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);
        
        
		
		if (uart_bytes_remaining >= 0) {
                //CDC_Device_SendString(&VirtualSerial_CDC_Interface,"Hi! I'm an ATMEGA32U4! You pressed:");
				CDC_Device_SendByte(&VirtualSerial_CDC_Interface,uart_bytes_remaining);
				//CDC_Device_SendString(&VirtualSerial_CDC_Interface," \r\n");
				
                if ((uart_bytes_remaining == 10) || (uart_bytes_remaining == 13)) {
                
                    if (((command_buffer[0] == 's') || (command_buffer[0] == 'S')) && ((command_buffer[1] == 'l') || (command_buffer[1] == 'L'))) {
                    
                        led_default[command_buffer[3]-48].r = command_buffer[5]; 
                        led_default[command_buffer[3]-48].g = command_buffer[7]; 
                        led_default[command_buffer[3]-48].b = command_buffer[9];    
                    
                        ws2812_setleds     (led_default,led_len);
                        
                        CDC_Device_SendString(&VirtualSerial_CDC_Interface,"\r\nChanging LED Pin\r\n");
                    
                    } else 	if(command_buffer[0] == '1') {
                        PORTD = 0x08;
                        CDC_Device_SendString(&VirtualSerial_CDC_Interface,"\r\nDebug LED On\r\n");
                    } else if (command_buffer[0] == '0') {
                        PORTD = 0x00;
                        CDC_Device_SendString(&VirtualSerial_CDC_Interface,"\r\nDebug LED Off\r\n");
                    } else if(command_buffer[0] == 'a') {
                        ws2812_setleds(led_default,led_len);
                        CDC_Device_SendString(&VirtualSerial_CDC_Interface,"\r\nDefault Sequence\r\n");
                    } else if(command_buffer[0] == 'b') {
                        ws2812_setleds(led_white,led_len);
                        CDC_Device_SendString(&VirtualSerial_CDC_Interface,"\r\nWhite Sequence\r\n");
						
					} else if(command_buffer[0] == 'x') {
						TIMSK0 = 0;
						TCCR0B = 0;
						CDC_Device_SendString(&VirtualSerial_CDC_Interface,"\r\nJumping to DFU...\r\n");
						cli();
						MCUSR |= (1 << WDRF);
						wdt_enable(WDTO_15MS);
						while(1) {}
                    } else if(command_buffer[0] == 'g') {
                        TIMSK0 ^= 1; //Toggle the overflow interrupt                        enable for the shifting animation
                        CDC_Device_SendString(&VirtualSerial_CDC_Interface,"\r\nCylon Sequence\r\n");
                    } 
                    
                    for (command_buffer_index = 0 ; command_buffer_index < 32 ; command_buffer_index++) {
                        command_buffer[command_buffer_index] = 0;
                    }
                    
                    command_buffer_index = 0;
           
                } else {
                
                    if (command_buffer_index < 32) {
                     
                        command_buffer[command_buffer_index] = uart_bytes_remaining;
                        
                        command_buffer_index++;
                    
                    }
          
                }
               
				
		}

		if(TIMSK0 & 1) {
			ws2812_setleds(led_cylon,led_len);
		}
		
		
		CDC_Device_USBTask(&VirtualSerial_CDC_Interface);
		USB_USBTask();
	}
}

ISR(BADISR_vect) {}

ISR(TIMER0_OVF_vect) {
	led_temp[0].r = led_cylon[led_len].r; led_temp[0].g = led_cylon[led_len].g; led_temp[0].b = led_cylon[led_len].b;
	for(led_addr = led_len ; led_addr >= 0 ; led_addr--) {
		if(led_addr == 0) {
			led_cylon[led_addr].r = led_temp[0].r; 
			led_cylon[led_addr].g = led_temp[0].g; 
			led_cylon[led_addr].b = led_temp[0].b;
		} else {
			led_cylon[led_addr].r = led_cylon[led_addr-1].r; 
			led_cylon[led_addr].g = led_cylon[led_addr-1].g; 
			led_cylon[led_addr].b = led_cylon[led_addr-1].b;
		}
	}
}

/** Configures the board hardware and chip peripherals for the demo's functionality. */
void SetupHardware(void)
{
#if (ARCH == ARCH_AVR8)
	/* Disable watchdog if enabled by bootloader/fuses */
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	/* Disable clock division */
	clock_prescale_set(clock_div_1);
#elif (ARCH == ARCH_XMEGA)
	/* Start the PLL to multiply the 2MHz RC oscillator to 32MHz and switch the CPU core to run from it */
	XMEGACLK_StartPLL(CLOCK_SRC_INT_RC2MHZ, 2000000, F_CPU);
	XMEGACLK_SetCPUClockSource(CLOCK_SRC_PLL);

	/* Start the 32MHz internal RC oscillator and start the DFLL to increase it to 48MHz using the USB SOF as a reference */
	XMEGACLK_StartInternalOscillator(CLOCK_SRC_INT_RC32MHZ);
	XMEGACLK_StartDFLL(CLOCK_SRC_INT_RC32MHZ, DFLL_REF_INT_USBSOF, F_USB);

	PMIC.CTRL = PMIC_LOLVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_HILVLEN_bm;
#endif

	/* Hardware Initialization */
	//Joystick_Init();
	LEDs_Init();
	USB_Init();
}

/** Checks for changes in the position of the board joystick, sending strings to the host upon each change. */
/*
void CheckJoystickMovement(void)
{
	uint8_t     JoyStatus_LCL = Joystick_GetStatus();
	char*       ReportString  = NULL;
	static bool ActionSent    = false;

	if (JoyStatus_LCL & JOY_UP)
	  ReportString = "Joystick Up\r\n";
	else if (JoyStatus_LCL & JOY_DOWN)
	  ReportString = "Joystick Down\r\n";
	else if (JoyStatus_LCL & JOY_LEFT)
	  ReportString = "Joystick Left\r\n";
	else if (JoyStatus_LCL & JOY_RIGHT)
	  ReportString = "Joystick Right\r\n";
	else if (JoyStatus_LCL & JOY_PRESS)
	  ReportString = "Joystick Pressed\r\n";
	else
	  ActionSent = false;

	if ((ReportString != NULL) && (ActionSent == false))
	{
		ActionSent = true;

		// Write the string to the virtual COM port via the created character stream 
		fputs(ReportString, &USBSerialStream);

		// Alternatively, without the stream: 
		// CDC_Device_SendString(&VirtualSerial_CDC_Interface, ReportString);
	}
}
*/

/** Event handler for the library USB Connection event. */
void EVENT_USB_Device_Connect(void)
{
	LEDs_SetAllLEDs(LEDMASK_USB_ENUMERATING);
}

/** Event handler for the library USB Disconnection event. */
void EVENT_USB_Device_Disconnect(void)
{
	LEDs_SetAllLEDs(LEDMASK_USB_NOTREADY);
}

/** Event handler for the library USB Configuration Changed event. */
void EVENT_USB_Device_ConfigurationChanged(void)
{
	bool ConfigSuccess = true;

	ConfigSuccess &= CDC_Device_ConfigureEndpoints(&VirtualSerial_CDC_Interface);

	LEDs_SetAllLEDs(ConfigSuccess ? LEDMASK_USB_READY : LEDMASK_USB_ERROR);
}

/** Event handler for the library USB Control Request reception event. */
void EVENT_USB_Device_ControlRequest(void)
{
	CDC_Device_ProcessControlRequest(&VirtualSerial_CDC_Interface);
}

/** CDC class driver callback function the processing of changes to the virtual
 *  control lines sent from the host..
 *
 *  \param[in] CDCInterfaceInfo  Pointer to the CDC class interface configuration structure being referenced
 */
void EVENT_CDC_Device_ControLineStateChanged(USB_ClassInfo_CDC_Device_t *const CDCInterfaceInfo)
{
	/* You can get changes to the virtual CDC lines in this callback; a common
	   use-case is to use the Data Terminal Ready (DTR) flag to enable and
	   disable CDC communications in your application when set to avoid the
	   application blocking while waiting for a host to become ready and read
	   in the pending data from the USB endpoints.
	*/
	bool HostReady = (CDCInterfaceInfo->State.ControlLineStates.HostToDevice & CDC_CONTROL_LINE_OUT_DTR) != 0;
}
