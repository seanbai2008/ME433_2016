#ifndef SPI_H__
#define SPI_H__

#define CS LATAbits.LATA0
#define A0 LATAbits.LATA0

void initSPI1(void);
unsigned char SPI1_IO(unsigned char write1, unsigned char write2);    // send a stop
void setVoltage(unsigned char channel, float voltage); 
void display_message(char message[], char x0, char y0);
#endif
