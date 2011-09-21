#include "main.h"
#include "i2c.h"

void *task_temperatura(void *arg)
{
       //LETTURA DEI DATI DI TEMPERATURA
       
       //abilitazione invio di ACK da parte del master
       *I2C0CONSET |= 0x04;
       
       i2c_send_start(); //start bit
       
       *I2DAT = TEMP_READ; //modalità di lettura del sensore
       *I2C0CONCLR = 0x28;
       
       while (*I2STAT != 0x40)
       {
	     //attesa dell'ack in modalità lettura
	     
	     if (*I2STAT == 0x0)
	     {
		   //si è verificato un bus-error
		   i2c_send_stop();
		   return arg;
	     }
        }
        
        uint32_t temp = 0x0;
        
       //ricezione del byte più significativo contenente la temperatura intera
       *I2C0CONCLR = 0x08;
       
       while (*I2STAT != 0x50)
       {
	     //attesa del primo byte
	     if (*I2STAT == 0x0)
	     {
		   //si è verificato un bus-error
		   i2c_send_stop();
		   return arg;
	     }
        }
       
       temp = *I2DAT;
       temp = temp << 4;
       
       //ricezione del byte meno significativo contenente la temperatura più fine
       *I2C0CONCLR = 0x08;
       while (*I2STAT != 0x50)
       {
	     //attesa del secondo byte
	     if (*I2STAT == 0x0)
	     {
		   //si è verificato un bus-error
		   i2c_send_stop();
		   return arg;
	     }
        }       
       
       temp |= (*I2DAT >> 4);
       
       i2c_send_stop(); //fine della lettura di temperatura
       
       printhex(temp);
       putc('\n');
       return arg;
}

struct task task_temp = {
	.f = task_temperatura,
	.name = "Leggi-Temperatura",
	.period = HZ, //leggo la temperatura una volta al secondo
	.next_run = 1,
};

declare_task(task_temp);
