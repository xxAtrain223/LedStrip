
#include "Serial/atmel_bootloader.c"
#include "Arduino/Stream.h"

#include "LedStripController.h"
#include "Commands.h"
#include "PythonInterpreter.h"
#include "EepromInteractors.h"
#include "TrigFunctions.h"

USB_ClassInfo_CDC_Device_t VirtualSerial_CDC_Interface =
    {
        {
            INTERFACE_ID_CDC_CCI,
            {
                CDC_TX_EPADDR,
                CDC_TXRX_EPSIZE,
                1,
            },
            {
                CDC_RX_EPADDR,
                CDC_TXRX_EPSIZE,
                1,
            },
            {
                CDC_NOTIFICATION_EPADDR,
                CDC_NOTIFICATION_EPSIZE,
                1,
            },
        },
    };

Stream comms(&VirtualSerial_CDC_Interface);
CmdMessenger cmdMessenger(comms);

struct cRGB leds[NUM_LEDS];

PyInt::Interpreter interp;

bool CalculateColors = true;

/** Main program entry point. This routine contains the overall program flow, including initial
 *  setup of all components and the main program loop.
 */

void debugLedOn()
{
    PORTD = 0x08;
}

void debugLedOff()
{
    PORTD = 0x00;
}

void debugLedToggle()
{
    if (PORTD == 0x08)
        PORTD = 0x00;
    else
        PORTD = 0x08;
}

int main(void)
{
    SetupHardware();
    init_millis(F_CPU);

    LEDs_SetAllLEDs(LEDMASK_USB_NOTREADY);
    GlobalInterruptEnable();

    DDRD = 0x08;  //Set Port D Pin 3 to output
    PORTD = 0x00; //Set Port D Pin 0-7 to low

    for (uint8_t i = 0; i < NUM_LEDS; i++)
    {
        leds[i].r = NUM_LEDS - i;
        leds[i].g = 0;
        leds[i].b = i;
    }

    //Display the leds
    ws2812_setleds(leds, NUM_LEDS);
    ws2812_setleds(leds, NUM_LEDS);

    attachCommandCallbacks();

    interp.Sin = sin8;
    interp.Cos = cos8;

    interp.Time = 0;
    interp.Index = 0;

    if (!isEepromReady())
        resetEeprom();
    getPattern(getCurrentPattern());

    while (true)
    {
        cmdMessenger.feedinSerialData();

        if (CalculateColors)
        {
            for (uint8_t i = 0; i < NUM_LEDS; i++)
            {
                interp.Index = i;

                leds[i].r = interp.execute(PyInt::r);
                leds[i].g = interp.execute(PyInt::g);
                leds[i].b = interp.execute(PyInt::b);
            }
            
            interp.Time = (interp.Time + 1) % 256;

            ws2812_setleds(leds, NUM_LEDS);
        }

        //Do LUFA Stuff
        CDC_Device_USBTask(&VirtualSerial_CDC_Interface);
        USB_USBTask();
    }
}

/** Configures the board hardware and chip peripherals for the demo's functionality. */
void SetupHardware(void)
{
    //Literally no idea what is happening in here
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
    LEDs_Init();
    USB_Init();
}

//Lufa event handlers
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
    //bool HostReady = (CDCInterfaceInfo->State.ControlLineStates.HostToDevice & CDC_CONTROL_LINE_OUT_DTR) != 0;
}
