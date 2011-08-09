#include "io.h"

extern void i2c_setup(void);
extern int i2c_send_start(void);
extern int i2c_address_slave(uint32_t address);

#define	I2SCLL		REG(0x40000014)
#define	I2SCLH		REG(0x40000010)
#define	I2C0CONSET	REG(0x40000000)
#define	I2C0CONCLR	REG(0x40000018)
#define	I2STAT		REG(0x40000004)
#define	I2DAT		REG(0x40000008)
