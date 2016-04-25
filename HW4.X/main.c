#include<xc.h>                      // processor SFR definitions Special Function registers
#include<sys/attribs.h> 
#include "i2c.h"
#include "spi.h"
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



static volatile float waveA[100];  //to store the trajectory sent from matlab
static volatile float waveB[200];   //to store the trajectory measured by encoder

void makewaveform(void);

void __ISR(_TIMER_1_VECTOR, IPL7SOFT) updatewave(void){ // _TIMER_1_VECTOR = 4 (p32mx795f512l.h)
    static int count_a = 0, count_b = 0;
    
    setVoltage('A',waveA[count_a]);
    setVoltage('B',waveB[count_b]);
    
    count_a++;
    count_b++;
    
    if (count_a == 100) count_a = 0;
    if (count_b == 200) count_b = 0;
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
    
    TRISAbits.TRISA0 = 0;
    CS = 1;
    
    // set up RB4 pin as input Pin 
    TRISBbits.TRISB4 = 1;  // pin B4 input
    TRISBbits.TRISB13 = 0;

    // set up LED1 pin as a digital output Pin A4
    TRISAbits.TRISA4 = 0;  // pin RA4 output
    LATAbits.LATA4 = 1;    // turn on the LED1
    
    //assign SS1 to pin RA0
    //SS1Rbits.SS1R = 0b0000;
    //assign SDO1 to pin RA1
    RPA1Rbits.RPA1R= 0b0011;
    
    //assign SDI1 to pin RB8
    SDI1Rbits.SDI1R = 0b0100;
    
    //turn off analog pin RB2
    ANSELBbits.ANSB2 = 0;
    //turn off analog pin RB3
    ANSELBbits.ANSB3 = 0;

    initSPI1();
    i2c_master_setup();
    
    initExpander();
    makewaveform();
    
    T1CON= 0x0; // Stop Timer and clear control register;
    T1CONbits.TCKPS = 0b10;//prescalar = 1:16
    TMR1 = 0; // Clear timer register
    PR1 = 749; // Load the period register

    IPC1bits.T1IP = 7;              // INT step 4: priority 7
    IPC1bits.T1IS = 0;              //             subpriority 0
    IFS0bits.T1IF = 0;              // INT step 5: clear interrupt flag
    IEC0bits.T1IE = 1;              // INT step 6: enable interrupt

    T1CONbits.ON = 1; // Start Timer 1  
    
    
    __builtin_enable_interrupts();
    // Main while loop, try to make LED blink every 1ms
    while (1){
 
        
//        write8(MCP23008_OLAT,0b10000000);
//        write8(MCP23008_OLAT,0b01000000); 
//        write8(MCP23008_OLAT,0b00100000); 
//        setExpander(7, 1); 
//        setExpander(7, 0); 
//        setExpander(6, 1);
//        setExpander(6, 0);
//         setExpander(7, 1);
//        write8(MCP23008_OLAT,0b00000001);
//        setExpander(0, 1);
//        setExpander(0, 0);
        _CP0_SET_COUNT(0); // Reset the core counter
        while(_CP0_GET_COUNT() < 12000) {  //Core timer runs at half of the CPU 
            if (getExpander(7) == 0){ break; } 
        } // once the button is triggered the loop break

        if (getExpander(7)== 1){ 
            setExpander(0, 1); 
        } // inverse the LED1
        else{   
            setExpander(0, 0);  
        } // turn on the LED1
    }
}

void makewaveform(void){
    int i;
    for (i = 0;i<100;i++){
        waveA[i]= 3.3/2.0*sin(2*3.1415926*i/100)+3.3/2.0;
    }
    for (i = 0;i<200;i++){
        waveB[i]= 3.3*i/200.0;
    }
    
}