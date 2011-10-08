#include "io.h"

extern void i2c_setup(void);
extern void i2c_send_start(void);
extern void i2c_address_slave_start(uint32_t address);
extern void i2c_temp_config(void);
extern void i2c_send_stop(void);
extern void read_temp_config(void);
extern uint16_t read_16bit_data(uint16_t indirizzo);
extern void write_16bit_data(uint16_t valore, uint16_t indirizzo);

#define	I2SCLL		REG(0x40000014)
#define	I2SCLH		REG(0x40000010)
#define	I2C0CONSET	REG(0x40000000)
#define	I2C0CONCLR	REG(0x40000018)
#define	I2STAT		REG(0x40000004)
#define	I2DAT		REG(0x40000008)

//indirizzi dei dispositivi i2c collegati
#define	TEMP_WRITE	0x90
#define	TEMP_READ		0x91
#define	MEMORY_WRITE	0xa0
#define	MEMORY_READ	0xa1