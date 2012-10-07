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

#if defined(__18CXX)
    #pragma udata
#endif

/* queue buffer */
static volatile char queue[QUEUE_SIZE];

/* last position read */
static volatile BYTE read_head;

/* next position to write to */
static volatile BYTE write_head;

#if defined(__18CXX)
    #pragma code
#endif

/* mask to AND with, to modulu a number to queue size */
/* we're assuming the queue size is a power of 2 */
#define QUEUE_SIZE_MASK (QUEUE_SIZE - 1)

#define INCREMENT_MODULU(x) ((x + 1) & QUEUE_SIZE_MASK)

void QueueInit(void) {
    read_head = 0; /* this is the last position read */
    write_head = 1; /* this is the next position to write to */
}

void QueuePut(char data) {
    if (write_head == read_head) {
        /* discard overflowing data */
        return;
    }
    /* write, then increment */
    queue[write_head] = data;
    write_head = INCREMENT_MODULU(write_head);
}

BOOL QueueHas(void) {
    return write_head != INCREMENT_MODULU(read_head);
}

char QueueGet(void) {
    /* move to new read position first */
    read_head = INCREMENT_MODULU(read_head);
    return queue[read_head];
}