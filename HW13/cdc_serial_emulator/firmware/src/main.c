/*******************************************************************************
  MPLAB Harmony Project Main Source File

  Company:
    Microchip Technology Inc.
  
  File Name:
    main.c

  Summary:
    This file contains the "main" function for an MPLAB Harmony project.

  Description:
    This file contains the "main" function for an MPLAB Harmony project.  The
    "main" function calls the "SYS_Initialize" function to initialize the state 
    machines of all MPLAB Harmony modules in the system and it calls the 
    "SYS_Tasks" function from within a system-wide "super" loop to maintain 
    their correct operation. These two functions are implemented in 
    configuration-specific files (usually "system_init.c" and "system_tasks.c")
    in a configuration-specific folder under the "src/system_config" folder 
    within this project's top-level folder.  An MPLAB Harmony project may have
    more than one configuration, each contained within it's own folder under
    the "system_config" folder.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013-2014 released Microchip Technology Inc.  All rights reserved.

//Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
// DOM-IGNORE-END


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "system/common/sys_module.h"   // SYS function prototypes
#include<xc.h>                      // processor SFR definitions Special Function registers
#include<sys/attribs.h> 
#include <math.h>
#include <proc/p32mx250f128b.h>
#include "app.h"

// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************
extern int pwm1, pwm2;
int qq;

int main ( void )
{
    __builtin_disable_interrupts();
    
    // set the CP0 CONFIG register to indicate that
    // kseg0 is cacheable (0x3) or uncacheable (0x2)
    // see Chapter 2 "CPU for Devices with M4K Core"
    // of the PIC32 reference manual
    // no cache on this chip!
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210582);
    // 0 data RAM access wait states
    BMXCONbits.BMXWSDRM = 0x0;
    // enable multi vector interrupts
    INTCONbits.MVEC = 0x1;
    // disable JTAG to be able to use TDI, TDO, TCK, TMS as digital
    DDPCONbits.JTAGEN = 0;
    /* Initialize all MPLAB Harmony modules, including application(s). */
    SYS_Initialize ( NULL );
    
    RPB3Rbits.RPB3R = 0b0101;//RA0 for OC1
    T2CONbits.TCKPS = 0b011;     // Timer3 prescaler N=8 (1:8)
	PR2 = 5999;              // period = (PR2+1) * N * 20.8333 ns = 1ms, 1kHz
	TMR2 = 0;                // initial TMR2 count is 0

	// Setting OC1 SFRs to PWM duty cycle 50%
	OC1CONbits.OCM = 0b110;        // PWM mode without fault pin; other OC1CON bits are defaults
	OC1RS = 0;             // duty cycle = OC1RS/(PR2+1) = 50%
	OC1R = 0;              // initialize before turning OC1 on; afterward it is read-only
	OC1CONbits.OCTSEL = 0;         // Set Timer2 is used for comparsion

    RPA4Rbits.RPA4R = 0b0101; //RB8 for OC2
    OC4CONbits.OCM = 0b110;
    OC4CONbits.OCTSEL = 0;
    OC4RS = 0;             // duty cycle = OC1RS/(PR2+1) = 50%
	OC4R = 0;              // initialize before turning OC1 on; afterward it is read-only
    
	// Turn on Timer3 and OC1
	T2CONbits.ON = 1;        // turn on Timer2
	OC1CONbits.ON = 1;       // turn on OC1
    OC4CONbits.ON = 1;
    
    
    TRISBbits.TRISB4 = 0; //pw1 phase bit
    TRISBbits.TRISB2 = 0; //pw2 phase bit
    
    LATBbits.LATB4 = 1;
    LATBbits.LATB2 = 1;
    
    
    __builtin_enable_interrupts();
    
    while ( true )
    {
        /* Maintain state machines of all polled MPLAB Harmony modules. */
        SYS_Tasks ( );
        
        
        OC4RS = (int)(qq/255.0*6000.0);
//        pwm1 = (int)(100*exp(qq-300.0));
//        pwm2 =(int)(100*exp(340.0-qq));
//
//        if(pwm1>100) pwm1 = 100;
//        if(pwm2>100) pwm2 = 100;
//
//        if(pwm2<0) pwm2 = 0;
//        if(pwm2<0) pwm2 = 0;
//        
//        OC4RS = (int)(pwm1/100.0)*6000;
//        OC1RS = (int)(pwm2/100.0)*6000;
        
    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}


/*******************************************************************************
 End of File
*/

