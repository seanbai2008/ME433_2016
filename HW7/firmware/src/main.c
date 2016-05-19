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
#include <xc.h> 
#include <sys/attribs.h> 
#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE


#include "system/common/sys_module.h"   // SYS function prototypes    
#include "i2c.h"
#include "ILI9163C.h"
unsigned char result[14];
signed short data[7];   //to store the data from LSM6DS33_ADDRESS



void __ISR(_TIMER_1_VECTOR, IPL7SOFT) update_data(void){ // _TIMER_1_VECTOR = 4 (p32mx795f512l.h)
    read_data(data,result);
    OC1RS = (int)((((float) (data[4]) * (2 / 32768.0)) + 1.0)*3000);
    OC2RS = (int)((((float) (data[5]) * (2 / 32768.0)) + 1.0)*3000);
//    setVoltage('B',1.5);
    TMR1 = 0;
	IFS0bits.T1IF = 0;              // clear interrupt flag
}
// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************

int main ( void )
{
    
    /* Initialize all MPLAB Harmony modules, including application(s). */
    SYS_Initialize ( NULL );
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
    
    T1CON= 0x0; // Stop Timer and clear control register;
    T1CONbits.TCKPS = 0b10;//prescalar = 1:64
    TMR1 = 0; // Clear timer register
    PR1 = 14999; // Load the period register

    IPC1bits.T1IP = 7;              // INT step 4: priority 7
    IPC1bits.T1IS = 0;              //             subpriority 0
    IFS0bits.T1IF = 0;              // INT step 5: clear interrupt flag
    IEC0bits.T1IE = 1;              // INT step 6: enable interrupt

    T1CONbits.ON = 1; // Start Timer 1  
    
    
    RPA0Rbits.RPA0R = 0b0101;//RA0 for OC1
    T2CONbits.TCKPS = 0b011;     // Timer3 prescaler N=8 (1:8)
	PR2 = 5999;              // period = (PR2+1) * N * 20.8333 ns = 1ms, 1kHz
	TMR2 = 0;                // initial TMR2 count is 0

	// Setting OC1 SFRs to PWM duty cycle 50%
	OC1CONbits.OCM = 0b110;        // PWM mode without fault pin; other OC1CON bits are defaults
	OC1RS = 3000;             // duty cycle = OC1RS/(PR2+1) = 50%
	OC1R = 3000;              // initialize before turning OC1 on; afterward it is read-only
	OC1CONbits.OCTSEL = 0;         // Set Timer2 is used for comparsion

    RPB8Rbits.RPB8R = 0b0101; //RB8 for OC2
    OC2CONbits.OCM = 0b110;
    OC2CONbits.OCTSEL = 0;
    OC2RS = 3000;             // duty cycle = OC1RS/(PR2+1) = 50%
	OC2R = 3000;              // initialize before turning OC1 on; afterward it is read-only
	// Turn on Timer3 and OC1
	T2CONbits.ON = 1;        // turn on Timer2
	OC1CONbits.ON = 1;       // turn on OC1
    OC2CONbits.ON = 1;

    
    TRISAbits.TRISA4 = 0;  // pin RA4 output
    LATAbits.LATA4 = 1;    // turn on the LED1
    i2c_master_setup();
    LSM6DS33_init();
    SPI1_init();
    LCD_init();
    
    LCD_clearScreen(0x000);
    
    __builtin_enable_interrupts();
    // Main while loop, try to make LED blink every 1ms
    
    char message[10]; 
    while ( true )
    {
        /* Maintain state machines of all polled MPLAB Harmony modules. */
        SYS_Tasks ( );
//        LCD_clearScreen(0x000);
//        _CP0_SET_COUNT(0); // Reset the core counter
//        while(_CP0_GET_COUNT() < 120000) {;}  //Core timer runs at half of the CPU 
//        sprintf(message,"x accel is: %3.3f y accel is: %3.3f", (float) (data[4]) * (2 / 32768.0),(float) (data[5]) * (2 / 32768.0));
//        display_message(message,28,20);


    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}


/*******************************************************************************
 End of File
*/

