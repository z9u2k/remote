/*
 * Copyright (c) 2012 Itay Duvdevani
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include "HardwareProfile.h"

#if defined(__18CXX)
    #pragma config PLLDIV=1
    #pragma config CPUDIV=OSC1_PLL2
    #pragma config USBDIV=2
    #pragma config FOSC=HSPLL_HS
    #pragma config FCMEN=OFF
    #pragma config IESO=OFF
    #pragma config BOR=OFF
    #pragma config PWRT=OFF
    #pragma config VREGEN=ON
    #pragma config WDT=OFF
    #pragma config LPT1OSC=OFF
    #pragma config MCLRE=ON
    #pragma config LVP=OFF
    #pragma config XINST=OFF
#ifdef __DEBUG
    #pragma config DEBUG=ON
#else
    #pragma config DEBUG=OFF
#endif

    #pragma config CP0=OFF, CP1=OFF, CP2=OFF, CP3=OFF, CPB=OFF, CPD=OFF
    #pragma config WRT0=OFF, WRT1=OFF, WRT2=OFF, WRT3=OFF, WRTC=OFF, WRTB=OFF, WRTD=OFF
    #pragma config EBTR0=OFF, EBTR1=OFF, EBTR2=OFF, EBTR3=OFF, EBTRB=OFF

    #if defined(PROTO_BOARD)
        #pragma config PBADEN=OFF
    #else
        #error Unsupported board
    #endif

    #if defined(__18F4553)
        #if defined(PROTO_BOARD)
            #pragma config ICPRT=OFF
        #endif
    #elif defined(__18F2553)
        #if defined(PROTO_BOARD)
            /* TODO: specific pragmas */
        #endif
    #else
        #error "Unsupported PIC18"
    #endif
#else
    #error "Unsupported MCU"
#endif
