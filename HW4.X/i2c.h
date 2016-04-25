#ifndef I2C_H__
#define I2C_H__

#define    FUNC_CFG_ACCESS    0x01
#define    FIFO_CTRL1         0x06
#define    FIFO_CTRL2         0x07
#define    FIFO_CTRL3         0x08
#define    FIFO_CTRL4         0x09
#define    FIFO_CTRL5         0x0A
#define    ORIENT_CFG_G       0x0B
#define    INT1_CTRL          0x0D
#define    NT2_CTRL           0x0E
#define    WHO_AM_I           0x0F
#define    CTRL1_XL           0x10
#define    CTRL2_G            0x11
#define    CTRL3_C            0x12
#define    CTRL4_C            0x13
#define    CTRL5_C            0x14
#define    CTRL6_C            0x15
#define    CTRL7_G            0x16
#define    CTRL8_XL           0x17
#define    CTRL9_XL           0x18
#define    CTRL10_C           0x19
#define    WAKE_UP_SRC        0x1B
#define    TAP_SRC            0x1C
#define    D6D_SRC            0x1D
#define    STATUS_REG         0x1E
#define    OUT_TEMP_L         0x20
#define    OUT_TEMP_H         0x21
#define    OUTX_L_G           0x22
#define    OUTX_H_G           0x23
#define    OUTY_L_G           0x24
#define    OUTY_H_G           0x25
#define    OUTZ_L_G           0x26
#define    OUTZ_H_G           0x27
#define    OUTX_L_XL          0x28
#define    OUTX_H_XL          0x29
#define    OUTY_L_XL          0x2A
#define    OUTY_H_XL          0x2B
#define    OUTZ_L_XL          0x2C
#define    OUTZ_H_XL          0x2D
#define    FIFO_STATUS1       0x3A
#define    FIFO_STATUS2       0x3B
#define    FIFO_STATUS3       0x3C
#define    FIFO_STATUS4       0x3D
#define    FIFO_DATA_OUT_L    0x3E
#define    FIFO_DATA_OUT_H    0x3F
#define    TIMESTAMP0_REG     0x40
#define    TIMESTAMP1_REG     0x41
#define    TIMESTAMP2_REG     0x42
#define    STEP_TIMESTAMP_L   0x49
#define    STEP_TIMESTAMP_H   0x4A
#define    STEP_COUNTER_L     0x4B
#define    STEP_COUNTER_H     0x4C
#define    FUNC_SRC           0x53
#define    TAP_CFG            0x58
#define    TAP_THS_6D         0x59
#define    INT_DUR2           0x5A
#define    WAKE_UP_THS        0x5B
#define    WAKE_UP_DUR        0x5C
#define    FREE_FALL          0x5D
#define    MD1_CFG            0x5E
#define    MD2_CFG            0x5F
// Header file for i2c_master_noint.c
// helps implement use I2C1 as a master without using interrupts

void i2c_master_setup(void);              // set up I2C 1 as a master, at 100 kHz

void i2c_master_start(void);              // send a START signal
void i2c_master_restart(void);            // send a RESTART signal
void i2c_master_send(unsigned char byte); // send a byte (either an address or data)
unsigned char i2c_master_recv(void);      // receive a byte of data
void i2c_master_ack(int val);             // send an ACK (0) or NACK (1)
void i2c_master_stop(void);               // send a stop

void write8(unsigned char address, unsigned char data);
unsigned char read8(unsigned char address);

void readAcc(void);
void readGyro(void);
#endif
