#ifndef SPI_H__
#define SPI_H__

#define CS LATAbits.LATA0

void initSPI1(void);
unsigned char SPI1_IO(unsigned char write1, unsigned char write2);    // send a stop
void setVoltage(unsigned char channel, float voltage); 
#endif
