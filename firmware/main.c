/**
 * This file is based on the Microchip CDC Basic example code, and is
 * distributed under sections 2.b, 2.c  and 2.d
 *
 * See README in root folder and Microchi[ Application Solutions User Agreement
 */
#include "HardwareProfile.h"

#include "usb_config.h"

#include "USB/usb.h"
#include "USB/usb_device.h"
#include "USB/usb_function_cdc.h"

#include "queue.h"

static void InitializeSystem(void);
void ProcessIO(void);
void USBDeviceTasks(void);
void YourHighPriorityISRCode();
void YourLowPriorityISRCode();
void USBCBSendResume(void);
void BlinkUSBStatus(void);
void UserInit(void);
extern void ProcessReceiver(void);

#if defined(__18CXX)
	/*
     * On PIC18 devices, addresses 0x00, 0x08, and 0x18 are used for
     * the reset, high priority interrupt, and low priority interrupt vectors.
     * However, the current Microchip USB bootloader examples are intended to
     * occupy addresses 0x00-0x7FF or 0x00-0xFFF depending on which bootloader
     * is used.  Therefore, the bootloader code remaps these vectors to new
     * locations as indicated below.  This remapping is only necessary if you
     * wish to program the hex file generated from this project with the USB
     * bootloader.  If no bootloader is used, edit the usb_config.h file and
     * comment out the following defines:
     * /
	/* #define PROGRAMMABLE_WITH_USB_HID_BOOTLOADER */
	/* #define PROGRAMMABLE_WITH_USB_LEGACY_CUSTOM_CLASS_BOOTLOADER	*/
	#if defined(PROGRAMMABLE_WITH_USB_HID_BOOTLOADER)
		#define REMAPPED_RESET_VECTOR_ADDRESS			0x1000
		#define REMAPPED_HIGH_INTERRUPT_VECTOR_ADDRESS	0x1008
		#define REMAPPED_LOW_INTERRUPT_VECTOR_ADDRESS	0x1018
	#elif defined(PROGRAMMABLE_WITH_USB_MCHPUSB_BOOTLOADER)	
		#define REMAPPED_RESET_VECTOR_ADDRESS			0x800
		#define REMAPPED_HIGH_INTERRUPT_VECTOR_ADDRESS	0x808
		#define REMAPPED_LOW_INTERRUPT_VECTOR_ADDRESS	0x818
	#else	
		#define REMAPPED_RESET_VECTOR_ADDRESS			0x00
		#define REMAPPED_HIGH_INTERRUPT_VECTOR_ADDRESS	0x08
		#define REMAPPED_LOW_INTERRUPT_VECTOR_ADDRESS	0x18
	#endif
	
	#if defined(PROGRAMMABLE_WITH_USB_HID_BOOTLOADER) || defined(PROGRAMMABLE_WITH_USB_MCHPUSB_BOOTLOADER)
	extern void _startup (void);        /* See c018i.c in your C18 compiler dir */
	#pragma code REMAPPED_RESET_VECTOR = REMAPPED_RESET_VECTOR_ADDRESS
	void _reset (void) {
	    _asm goto _startup _endasm
	}
	#endif
	#pragma code REMAPPED_HIGH_INTERRUPT_VECTOR = REMAPPED_HIGH_INTERRUPT_VECTOR_ADDRESS
	void Remapped_High_ISR(void) {
	     _asm goto YourHighPriorityISRCode _endasm
	}
	#pragma code REMAPPED_LOW_INTERRUPT_VECTOR = REMAPPED_LOW_INTERRUPT_VECTOR_ADDRESS
	void Remapped_Low_ISR(void) {
	     _asm goto YourLowPriorityISRCode _endasm
	}
	
	#if defined(PROGRAMMABLE_WITH_USB_HID_BOOTLOADER)||defined(PROGRAMMABLE_WITH_USB_MCHPUSB_BOOTLOADER)
	/*
     * Note: If this project is built while one of the bootloaders has been
     * defined, but then the output hex file is not programmed with the
     * bootloader, addresses 0x08 and 0x18 would end up programmed with 0xFFFF.
     * As a result, if an actual interrupt was enabled and occured, the PC would
     * jump to 0x08 (or 0x18) and would begin executing "0xFFFF" (unprogrammed
     * space).  This executes as nop instructions, but the PC would eventually
     * reach the REMAPPED_RESET_VECTOR_ADDRESS (0x1000 or 0x800, depending upon
     * bootloader), and would execute the "goto _startup". This would
     * effective reset the application.
     *
     * To fix this situation, we should always deliberately place a
     * "goto REMAPPED_HIGH_INTERRUPT_VECTOR_ADDRESS" at address 0x08, and a
     * "goto REMAPPED_LOW_INTERRUPT_VECTOR_ADDRESS" at address 0x18.  When the
     * output hex file of this project is programmed with the bootloader, these
     * sections do not get bootloaded (as they overlap the bootloader space).
     * If the output hex file is not programmed using the bootloader, then the
     * below goto instructions do get programmed, and the hex file still works
     * like normal.  The below section is only required to fix this scenario.
     */
	#pragma code HIGH_INTERRUPT_VECTOR = 0x08
	void High_ISR(void) {
	     _asm goto REMAPPED_HIGH_INTERRUPT_VECTOR_ADDRESS _endasm
	}
	#pragma code LOW_INTERRUPT_VECTOR = 0x18
	void Low_ISR(void) {
	     _asm goto REMAPPED_LOW_INTERRUPT_VECTOR_ADDRESS _endasm
	}
	#endif	//end of "#if defined(PROGRAMMABLE_WITH_USB_HID_BOOTLOADER)||defined(PROGRAMMABLE_WITH_USB_LEGACY_CUSTOM_CLASS_BOOTLOADER)"

	#pragma code

    #pragma interrupt YourHighPriorityISRCode
	void YourHighPriorityISRCode() {
        #if defined(USB_INTERRUPT)
        USBDeviceTasks();
        #endif

        if (ReceiverInterruptFlag) {
            ProcessReceiver();
            ReceiverInterruptFlag = 0;
        }
	}

    #pragma interruptlow YourLowPriorityISRCode
	void YourLowPriorityISRCode() {
	}
#endif

#if defined(__18CXX)
    #pragma code
#endif

#if defined(__18CXX)
void main(void)
#else
int main(void)
#endif
{   
    InitializeSystem();

    while(1) {
        #if defined(USB_INTERRUPT)
            if(USB_BUS_SENSE && (USBGetDeviceState() == DETACHED_STATE)) {
                USBDeviceAttach();
            }
        #endif

        #if defined(USB_POLLING)
        USBDeviceTasks();
        #endif

        ProcessIO();
    }
}

/**
 * prepare system on boot
 */
static void InitializeSystem(void) {
    #if defined(__18CXX)
        SetupTimer();
        INTCONbits.GIEH = 1;
    #endif
    
    InitLED();
    LED_Off();

    InitReceiver();
    ReceiverOff();
    
    USBDeviceInit();
}

/**
 * blink LED, send data on queue
 */
void ProcessIO(void) {

    BlinkUSBStatus();
    
    if ((USBDeviceState < CONFIGURED_STATE) || (USBSuspendControl == 1)) {
        return;
    }

    if (USBUSARTIsTxTrfReady()) {
        int len = 0;
        char temp[QUEUE_SIZE];
        while (QueueHas()) {
            temp[len++] = QueueGet();
        }

		if (len > 0) {
			putUSBUSART(temp, len);
		}
	}

    CDCTxService();
}

/**
 * update LED status
 */
void BlinkUSBStatus(void) {

    #define BLINK_COUNT (0xffff/16)

    static UINT16 led_count = 0xffff;
    static USB_DEVICE_STATE lastState = DETACHED_STATE;
    static BOOL blinkConfiguredSequence = FALSE;

    led_count++;
    
    if (USBSuspendControl == 1) {
        LED_Off();
    } else {
        if (USBGetDeviceState() < CONFIGURED_STATE) {
            if (blinkConfiguredSequence) {
                blinkConfiguredSequence = FALSE;
            }

            if (lastState == CONFIGURED_STATE) {
                ReceiverOff();
            }

            if (led_count == 0) {
                LED_On();
            } else if (led_count == BLINK_COUNT) {
                LED_Off();
            }
        } else {
            if (lastState != CONFIGURED_STATE) {
                QueueInit();
                ReceiverOn();
                LED_On();
                blinkConfiguredSequence = TRUE;
                led_count = 0;
            } else if (blinkConfiguredSequence && led_count == 0) {
                blinkConfiguredSequence = FALSE;
                LED_Off();
            }
        }
    }

    lastState = USBGetDeviceState();
}
