/**
 * This file is based on the Microchip CDC Basic example code, and is
 * distributed under sections 2.b, 2.c  and 2.d
 *
 * See README in root folder and Microchi[ Application Solutions User Agreement
 */

#ifndef HARDWARE_PROFILE_H
#define HARDWARE_PROFILE_H

#if !defined(PROJECT_BOARD)
    #if defined(__18CXX)
        #if defined(__18F4553)
            #if defined(PROTO_BOARD)
                #include "hal-pic18f4553-proto-board.h"
            #endif
        #elif defined(__18F2553)
            #if defined(PROTO_BOARD)
                #include "hal-pic18f2553-proto-board.h"
            #endif
        #endif
    #endif
#endif

#if !defined(PROJECT_BOARD)
    #error "Project board not defined"
#endif

#endif  //HARDWARE_PROFILE_H
