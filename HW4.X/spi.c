//spi.c contains the function which initilize SPI1 of pic32mx250f128b
//and write & read function
#include<xc.h> 
#include "spi.h"

void initSPI1(void){
 
    SPI1BUF;                  // clear the rx buffer by reading from it
    SPI1BRG = 0b1;            // fastest baud rate
    SPI1STATbits.SPIROV = 0;  // clear the overflow bit
    SPI1CONbits.CKE = 1; 
    SPI1CONbits.MODE32 = 0;   // use 8 bit mode
    SPI1CONbits.MODE16 = 1;
    SPI1CONbits.MSTEN = 1;    // master operation
    SPI1CONbits.ON = 1;       // turn on spi 1
}

unsigned char SPI1_IO(unsigned char write1,unsigned char write2){
    SPI1BUF = (write1<<8)|write2;
    while(!SPI1STATbits.SPIRBF){
        ;
    }
    return SPI1BUF;
}

void setVoltage(unsigned char channel, float voltage){
    unsigned char outputA = 0;
    unsigned char outputB = 0;
    int temp = (int)(voltage/3.3*255);
    unsigned char a = (unsigned char)(temp);
    if(channel == 'A'){
        outputA = (0b0011<<4)|(a>>4);
        outputB = a<<4;
    }
    if(channel == 'B'){
        outputA = (0b1011<<4)|(a>>4);
        outputB = a<<4;
    }
    CS = 0;
    SPI1_IO(outputA,outputB);      
    CS = 1;
}


