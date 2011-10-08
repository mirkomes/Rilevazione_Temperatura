#include "main.h"
#include "i2c.h"

void task_temperatura(void)
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
		   return;
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
		   return;
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
		   return;
	     }
        }       
       
       temp |= (*I2DAT >> 4);
       
       i2c_send_stop(); //fine della lettura di temperatura
       
       //salvataggio della temperatura nel buffer circolare in memoria
       struct temp *buf = __temp_start[0];
       buf->write(&temp);
       
       printhex(temp);
       putc('\n');
}

struct task task_temp = {
        .f = task_temperatura,
        .name = "Leggi-Temperatura",
        .period = HZ, //leggo la temperatura una volta al secondo
        .next_run = 0,
};

static uint16_t c_buffer[16];
static short b_index[1];
static uint16_t l_addrs[1];

void write_buffer(void *arg)
{
        //scrittura dei valori nel buffer
        uint16_t *p = (uint16_t *) arg;
        c_buffer[b_index[0]++] = p[0]; //valore di temperatura da scrivere
        b_index[0] = (b_index[0] > 0xf) ? 0x0 : b_index[0];
}

void *read_buffer(void)
{
        return (void *) c_buffer;
}

void index_init(void)
{
        b_index[0] = 0x0; //azzeramento dell'indice per sicurezza
        
        l_addrs[0] = read_16bit_data(0x7fe2);
        
        puts("E' stato prelevato il dato: ");
        printhex(l_addrs[0]);
}

struct temp measures_buffer = {
        .valori = c_buffer,
        .write = write_buffer,
        .read = read_buffer,
        .init = index_init,
        .index = b_index,
        .last_address = l_addrs
};

declare_task(task_temp);
declare_temp(measures_buffer);
