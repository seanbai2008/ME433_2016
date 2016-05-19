#include<xc.h>                      // processor SFR definitions Special Function registers
#include<sys/attribs.h> 
#include "i2c.h"
#include <math.h>
#include "ILI9163C.h"


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
    
    SPI1_init();
    LCD_init();
    
    LCD_clearScreen(0x000);
    LCD_drawPixel(1,1,0xffff);
    LCD_drawPixel(100,50,0xffff);  
    LCD_drawPixel(100,100,0xffff);
    __builtin_enable_interrupts();
    // Main while loop, try to make LED blink every 1ms
    
    
    char message[10]; 
    int number = 1337;
    unsigned short number1 = 0b1111111111111111;
    sprintf(message,"Hello world %d!  Fan Bai", number1);
    display_message(message,28,20);
    while (1){
        ;
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


