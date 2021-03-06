// I2C Master utilities, 100 kHz, using polling rather than interrupts
// The functions must be callled in the correct order as per the I2C protocol
// Change I2C2 to the I2C channel you are using
// I2C pins need pull-up resistors, 2k-10k
#include<xc.h>
#include "i2c.h"

void i2c_master_setup(void) {
    I2C2BRG = 53;            // 400kHZ I2CBRG = [1/(2*Fsck) - PGD]*Pblck - 2  53                                // look up PGD for your PIC32
    I2C2CONbits.ON = 1;               // turn on the I2C2 module
}

// Start a transmission on the I2C bus
void i2c_master_start(void) {
    I2C2CONbits.SEN = 1;            // send the start bit
    while(I2C2CONbits.SEN) { ; }    // wait for the start bit to be sent
}

void i2c_master_restart(void) {     
    I2C2CONbits.RSEN = 1;           // send a restart 
    while(I2C2CONbits.RSEN) { ; }   // wait for the restart to clear
}

void i2c_master_send(unsigned char byte) { // send a byte to slave
    I2C2TRN = byte;                   // if an address, bit 0 = 0 for write, 1 for read
    while(I2C2STATbits.TRSTAT) { ; }  // wait for the transmission to finish
    if(I2C2STATbits.ACKSTAT) {        // if this is high, slave has not acknowledged
   // ("I2C2 Master: failed to receive ACK\r\n");

    }



}

unsigned char i2c_master_recv(void) { // receive a byte from the slave
    I2C2CONbits.RCEN = 1;             // start receiving data
    int i = 100;
    while(!I2C2STATbits.RBF) {

        Nop();
        Nop();
        Nop();    
    }    // wait to receive the data
    return I2C2RCV;                   // read and return the data
}

void i2c_master_ack(int val) {        // sends ACK = 0 (slave should send another byte)
                                      // or NACK = 1 (no more bytes requested from slave)
    I2C2CONbits.ACKDT = val;          // store ACK/NACK in ACKDT
    I2C2CONbits.ACKEN = 1;            // send ACKDT
    while(I2C2CONbits.ACKEN) { ; }    // wait for ACK/NACK to be sent
}

void i2c_master_stop(void) {          // send a STOP:
    I2C2CONbits.PEN = 1;                // comm is complete and master relinquishes bus
    while(I2C2CONbits.PEN) { ; }        // wait for STOP to complete
}

void write8(unsigned char address, unsigned char data){
   
    i2c_master_start();
    i2c_master_send(MCP23008_ADDRESS<<1);
    i2c_master_send(address);
    i2c_master_send(data);
    i2c_master_stop();

}
unsigned char read8(unsigned char address){
    unsigned char result;
    i2c_master_start();
    i2c_master_send(MCP23008_ADDRESS<<1);
    i2c_master_send(address);
    i2c_master_restart();
    i2c_master_send(MCP23008_ADDRESS<<1|0b1);
    result = i2c_master_recv();
    i2c_master_ack(1);
    i2c_master_stop();
    return result;
}

void initExpander(void){
    write8(MCP23008_IOCON,0b00111000);
    write8(MCP23008_IODIR,0b11110000);
    write8(MCP23008_OLAT,0b00000000); 
    
}
void setExpander(int pin, int level){
    int i;
    unsigned char output = 0b1;
    unsigned char pin_status = read8(MCP23008_GPIO);
    output = output << pin;
    if (level == 1) write8(MCP23008_OLAT,pin_status|output); 
    else if(level == 0)   write8(MCP23008_OLAT,pin_status&(~output)); 

}
unsigned char getExpander(int pin){
    int i;
    unsigned char output = 0b1;
    unsigned char pin_status = read8(MCP23008_GPIO); 
    output = output << pin;
    return (output&pin_status)>>pin;
}
