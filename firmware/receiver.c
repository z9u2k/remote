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
#include "queue.h"

#include "USB/usb.h"

#include "HardwareProfile.h"

/** bit to turn on if this was a silence period */
#define SILENCE_MASK 0x8000

/* receiver active-low */
#define SILENCE mReceiverPinMask
#define PULSE 0

#if defined(__18CXX)
    #pragma udata
#endif

/** timer value at last edge */
static UINT16 last_time = 0;

#if defined(__18CXX)
    #pragma code
#endif

void ProcessReceiver(void) {

    // cache time
    UINT16 now;
    int sample;
    UINT32 duration;

    // sample pin
    ClearMismatch();
    sample = ReceiverPin;

    // update time
    now = ReadTimer();

    // calculate time delta, take wrap-arounds into account
    duration = (now >= last_time) ?
        (now - last_time) : (MAX_TIMER - last_time + now);

    // ticks to microseconds
    duration *= TIMER_TICK_NS / 1000;

    // microseconds to 1/16384 of a second, truncated to 16-bits
    duration = duration * 16384L / 1000000L;
    duration &= 0xffff;

    // shouldn't really happen, but makes lirc sad
    if (duration == 0) {
        return;
    }
    
    if (sample == PULSE) {
        // pulse period stared - silence period ended
        duration |= SILENCE_MASK;
    }

    // advance time marker
    last_time = now;

    QueuePut(duration & 0xff);
    QueuePut((duration >> 8) & 0xff);

    // LED
    if (USBDeviceState == CONFIGURED_STATE) {
        if (sample == PULSE) {
            LED_On();
        } else {
            LED_Off();
        }
    }
}
