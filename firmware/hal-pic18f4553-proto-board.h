/**
 * This file is based on the Microchip CDC Basic example code, and is
 * distributed under sections 2.b, 2.c  and 2.d
 *
 * See README in root folder and Microchi[ Application Solutions User Agreement
 */

#ifndef HAL_PIC18F4553_PROTO_BOARD_H
#define HAL_PIC18F4553_PROTO_BOARD_H

#include <p18cxxx.h>
#include <timers.h>
#include <portb.h>

/*******************************************************************/
/******** USB stack hardware selection options *********************/
/*******************************************************************/
//This section is the set of definitions required by the MCHPFSUSB
//  framework.  These definitions tell the firmware what mode it is
//  running in, and where it can find the results to some information
//  that the stack needs.
//These definitions are required by every application developed with
//  this revision of the MCHPFSUSB framework.  Please review each
//  option carefully and determine which options are desired/required
//  for your application.

//#define USE_SELF_POWER_SENSE_IO
#define tris_self_power     TRISCbits.TRISC2    // Input
#define self_power          1

//#define USE_USB_BUS_SENSE_IO
#define tris_usb_bus_sense  TRISCbits.TRISC2    // Input
#define USB_BUS_SENSE       1 

/*******************************************************************/
/*******************************************************************/
/*******************************************************************/
/******** Application specific definitions *************************/
/*******************************************************************/
/*******************************************************************/
/*******************************************************************/

//#define PROGRAMMABLE_WITH_USB_HID_BOOTLOADER

/** Board definition ***********************************************/
//These defintions will tell the main() function which board is
//  currently selected.  This will allow the application to add
//  the correct configuration bits as wells use the correct
//  initialization functions for the board.  These defitions are only
//  required in the stack provided demos.  They are not required in
//  final application design.
#define PROJECT_BOARD PIC18F4553_PROTO_BOARD
#define CLOCK_FREQ 16000000
#define GetSystemClock() CLOCK_FREQ

/** LED ************************************************************/
#define mInitAllLEDs() \
    { \
        LATE &= 0b11111100; \
        TRISE &= 0b11111100; \
    }

#define mLED_1              LATEbits.LATE0
#define mLED_2              LATEbits.LATE1

#define mGetLED_1()         mLED_1
#define mGetLED_2()         mLED_2

#define mLED_1_On()         mLED_1 = 1;
#define mLED_2_On()         mLED_2 = 1;

#define mLED_1_Off()        mLED_1 = 0;
#define mLED_2_Off()        mLED_2 = 0;

#define mLED_1_Toggle()     mLED_1 = !mLED_1;
#define mLED_2_Toggle()     mLED_2 = !mLED_2;

//#define EnableWakeupInterrupt() \
//    { \
//        OpenRB0INT(FALLING_EDGE_INT); \
//    }
//#define DisableWakeupInterrupt() \
//    { \
//        CloseRB0INT(); \
//    }
//#define mWakeupInterruptFlag INTCONbits.INT0IF

#define mReceiverPinMask (1 << 4)
/* need to read whole port to update latches */
#define ReadReceiver() (PORTB & mReceiverPinMask)
#define mReceiverInterruptFlag INTCONbits.RBIF
#define SetupInterruptOnChange() \
    { \
        /* set all pins to output, except receiver pin */ \
        TRISB = mReceiverPinMask;  \
        /* receiver is active-low */ \
        EnablePullups(); \
        /* enable interrupt with high priority */ \
        OpenPORTB(PORTB_CHANGE_INT_ON & PORTB_INT_PRIO_HIGH); \
    }

#define MAX_TIMER_TICK_NS ((1000000L / (16384L / 1000L)) / 2) /* sample 1/16384 of a second reliably */
#define MIN_TIMER_WRAPAROUND_NS 1000000L /* 1ms */

#define NS_PER_CLOCK (1000L / (CLOCK_FREQ / 1000000L))

#if (256L * NS_PER_CLOCK) > MIN_TIMER_WRAPAROUND_NS

    /* see if an 8-bit timer is enough */

    #define MAX_TIMER (1 << 8)
    #define TIMER_BITS T0_8BIT

    #if 256L * NS_PER_CLOCK < MAX_TIMER_TICK_NS
        #define TIMER_PRESCALER T0_PS_1_256
    #elif 128L * NS_PER_CLOCK < MAX_TIMER_TICK_NS
        #define TIMER_PRESCALER T0_PS_1_128
    #elif 64L * NS_PER_CLOCK < MAX_TIMER_TICK_NS
        #define TIMER_PRESCALER T0_PS_1_64
    #elif 32L * NS_PER_CLOCK < MAX_TIMER_TICK_NS
        #define TIMER_PRESCALER T0_PS_1_32
    #elif 16L * NS_PER_CLOCK < MAX_TIMER_TICK_NS
        #define TIMER_PRESCALER T0_PS_1_16
    #elif 8L * NS_PER_CLOCK < MAX_TIMER_TICK_NS
        #define TIMER_PRESCALER T0_PS_1_8
    #elif 4L * NS_PER_CLOCK < MAX_TIMER_TICK_NS
        #define TIMER_PRESCALER T0_PS_1_4
    #elif 2L * NS_PER_CLOCK < MAX_TIMER_TICK_NS
        #define TIMER_PRESCALER T0_PS_1_2
    #elif 1L * NS_PER_CLOCK < MAX_TIMER_TICK_NS
        #define TIMER_PRESCALER T0_PS_1_1
    #else
        #error Clock not fast enough
    #endif

#elif (65536L * NS_PER_CLOCK) > MIN_TIMER_WRAPAROUND_NS

    /* see if an 16-bit timer is enough */

    #define MAX_TIMER (1 << 16)
    #define TIMER_BITS T0_16BIT

    #if 256L * NS_PER_CLOCK < MAX_TIMER_TICK_NS
        #define TIMER_PRESCALER T0_PS_1_256
    #elif 128L * NS_PER_CLOCK < MAX_TIMER_TICK_NS
        #define TIMER_PRESCALER T0_PS_1_128
    #elif 64L * NS_PER_CLOCK < MAX_TIMER_TICK_NS
        #define TIMER_PRESCALER T0_PS_1_64
    #elif 32L * NS_PER_CLOCK < MAX_TIMER_TICK_NS
        #define TIMER_PRESCALER T0_PS_1_32
    #elif 16L * NS_PER_CLOCK < MAX_TIMER_TICK_NS
        #define TIMER_PRESCALER T0_PS_1_16
    #elif 8L * NS_PER_CLOCK < MAX_TIMER_TICK_NS
        #define TIMER_PRESCALER T0_PS_1_8
    #elif 4L * NS_PER_CLOCK < MAX_TIMER_TICK_NS
        #define TIMER_PRESCALER T0_PS_1_4
    #elif 2L * NS_PER_CLOCK < MAX_TIMER_TICK_NS
        #define TIMER_PRESCALER T0_PS_1_2
    #elif 1L * NS_PER_CLOCK < MAX_TIMER_TICK_NS
        #define TIMER_PRESCALER T0_PS_1_1
    #else
        #error Clock not fast enough
    #endif

#else
    #error Clock too fast for min. timer wraparound
#endif

#define SetupTimer() \
    { \
        OpenTimer0(TIMER_BITS & TIMER_PRESCALER & TIMER_INT_OFF & T0_SOURCE_INT); \
    }

#define ReadTimer() ReadTimer0()

#define TIMER_TICK_NS (TIMER_PRESCALER * NS_PER_CLOCK)

#endif  //HAL_PIC18F4553_PROTO_BOARD_H
