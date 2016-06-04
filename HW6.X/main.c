#include<xc.h>                      // processor SFR definitions Special Function registers
#include<sys/attribs.h> 
#include "i2c.h"
#include <math.h>


// DEVCFG0
#pragma config DEBUG = OFF // Background Debugger disabled
#pragma config JTAGEN = OFF // no jtag
#pragma config ICESEL = ICS_PGx1 // use PGED1 and PGEC1
#pragma config PWP = OFF // no write protect
#pragma config BWP = OFF // not boot write protect
#pragma config CP = OFF // no code protect

// DEVCFG1
#pragma config FNOSC = PRIPLL // Oscillator Selection: Primary oscillator w/ PLL
#pragma config FSOSCEN = OFF // Disable second osc to get pins back
#pragma config IESO = OFF // no switching clocks
#pragma config POSCMOD = HS // Primary Oscillator Mode: High Speed xtal
#pragma config OSCIOFNC = OFF // free up secondary osc pins
#pragma config FPBDIV = DIV_1 // Peripheral Bus Clock: Divide by 1
#pragma config FCKSM = CSDCMD // do not enable clock switch
#pragma config WDTPS = PS1048576 // slowest wdt
#pragma config WINDIS = OFF // no wdt window
#pragma config FWDTEN = OFF // wdt off by default
#pragma config FWDTWINSZ = WINSZ_25 // wdt window at 25%

// DEVCFG2 - get the CPU clock to 40MHz
#pragma config FPLLIDIV = DIV_2 // divide input clock to be in range 4-5MHz  8M / 2 = 4M
#pragma config FPLLMUL = MUL_24 // multiply clock after FPLLIDIV  4M * 24 = 96M  PLL Multiplier: Multiply by 20
#pragma config FPLLODIV = DIV_2 // divide clock by 2 to output on pin 96M / 2 = 48M
#pragma config UPLLIDIV = DIV_2 // USB clock  8M / 2 = 4M
#pragma config UPLLEN = ON // USB clock on


// DEVCFG3
#pragma config USERID = 0 // some 16bit userid
#pragma config PMDL1WAY = ON // not multiple reconfiguration, check this
#pragma config IOL1WAY = ON // not multimple reconfiguration, check this
#pragma config FUSBIDIO = ON // USB pins controlled by USB module
#pragma config FVBUSONIO = ON // controlled by USB module


unsigned char result[14];
static volatile signed short data[7];   //to store the data from LSM6DS33_ADDRESS

void __ISR(_TIMER_1_VECTOR, IPL7SOFT) update_data(void){ // _TIMER_1_VECTOR = 4 (p32mx795f512l.h)
    read_data(data,result);
    

    
    OC1RS = (int)((((float) (data[4]) * (2 / 32768.0)) + 1.0)*3000);
    OC2RS = (int)((((float) (data[5]) * (2 / 32768.0)) + 1.0)*3000);
//    setVoltage('B',1.5);
    TMR1 = 0;
	IFS0bits.T1IF = 0;              // clear interrupt flag
}

int main(void) {
    
    //Startup code to run as fast as possible and get pins back from bad defaults
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
//    int number = 1337;
//    unsigned short number1 = 0b1111111111111111;
//    sprintf(message,"Hello world %d!  Fan Bai", number1);
//    display_message(message,28,20);
    while (1){
        LCD_clearScreen(0x000);
        _CP0_SET_COUNT(0); // Reset the core counter
        while(_CP0_GET_COUNT() < 120000) {;}  //Core timer runs at half of the CPU 
        sprintf(message,"x accel is: %3.3f y accel is: %3.3f", (float) (data[4]) * (2 / 32768.0),(float) (data[5]) * (2 / 32768.0));
        display_message(message,28,20);
    }
//        read_data(data);  
//
////        unsigned char me = read8(WHO_AM_I);
//        if(data == 0b01101001){
//            
//            LATAbits.LATA4 =0;
//          
//        }
//        _CP0_SET_COUNT(0); // Reset the core counter
//        while(_CP0_GET_COUNT() < 12000) {  //Core timer runs at half of the CPU 
//            if (PORTBbits.RB4 == 0){ break; } 
//        } // once the button is triggered the loop break
//        
//        if (PORTBbits.RB4 == 1){ 
//            LATAbits.LATA4 = 1; 
//        } // inverse the LED11
//    }
}


