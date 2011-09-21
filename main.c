#include "main.h"
#include "i2c.h"

void main(void)
{
       
       //abilitazione dei led per debugging
       *(volatile uint32_t *)0x50038000 = 0x0f;
       *(volatile uint32_t *) 0x5003003c = 0xfff;
       
       //FASE 1 - CONFIGURAZIONE DEI COMPONENTI HARDWARE
       
       //CONFIGURAZIONE DEL SENSORE DI TEMPERATURA
       //indirizzamento del sensore in scrittura
        i2c_address_slave_start(TEMP_WRITE);
       //configurazione del sensore di temperatura
       i2c_temp_config();
       //i2c STOP
       i2c_send_stop();
       
       //FASE 2 - LETTURA DEI DATI DI TEMPERATURA
       
       i2c_address_slave_start(TEMP_WRITE);
       read_temp_config(); //configurazione per la lettura della temperatura
       i2c_send_stop();
       
       //abilitazione invio di ACK da parte del master
       *I2C0CONSET |= 0x04;
       
       i2c_send_start(); //start bit
       
       *I2DAT = TEMP_READ; //modalità di lettura del sensore
       *I2C0CONCLR = 0x28;
       
       while (*I2STAT != 0x40)
       {
	     //attesa dell'ack in modalità lettura
        }
        
        uint32_t temp = 0x0;
        
       //ricezione del byte più significativo contenente la temperatura intera
       *I2C0CONCLR = 0x08;
       
       while (*I2STAT != 0x50)
       {
	     //attesa del primo byte
        }
       
       temp = *I2DAT;
       temp = temp << 4;
       
       //ricezione del byte meno significativo contenente la temperatura più fine
       *I2C0CONCLR = 0x08;
       while (*I2STAT != 0x50)
       {
	     //attesa del secondo byte
        }       
       
       temp |= (*I2DAT >> 4);
       printhex(temp);
       
}