#include "main.h"
#include "i2c.h"

//lettura dati dalla memoria ed elaborazione per invio alla seriale
void read_mem(void)
{
        puts("Read memory\n");
        //leggo dalla ram l'indirizzo finale dove sono stati salvati dati in memoria
        //struct temp *buf = __temp_start[0];
        //uint16_t final_addrs = buf->last_address[0];
        
        //indirizzamento della memoria in modalità scrittura
        i2c_address_slave_start(MEMORY_WRITE);
        
        *I2DAT = 0x0;
        *I2C0CONCLR = 0x08;
        
        while (*I2STAT != 0x28)
        {
        
        }
        
        *I2DAT = 0x0;
        *I2C0CONCLR = 0x08;
        
        while (*I2STAT != 0x28)
        {
	      
        }
        
        //abilitazione invio di ACK da parte del master
        //*I2C0CONSET |= 0x04;
        
        //invio di START bit più indirizzamento in lettura
        i2c_send_start(); //start bit
        
        *I2DAT = MEMORY_READ; //modalità di lettura del sensore
        *I2C0CONCLR = 0x28;
       
        while (*I2STAT != 0x28)
        {
	     //attesa dell'ack in modalità lettura
	     if (*I2STAT == 0x0)
	     {
		   //si è verificato un bus-error
		   i2c_send_stop();
		   return;
	     }
        }
        
        
        puts("sono qui");
        //aspetto i dati in arrivo dalla memoria
        *I2C0CONCLR = 0x08;
        while (*I2STAT != 0x48)
        {
	      
        }
        
        puts("Dato memoria: ");
        printhex(*I2DAT);
        putc('\n');
        
        i2c_send_stop();
        
}

struct task task_read_memory = {
        .name = "Read memory data.",
        .f = read_mem,
        .next_run = HZ*(NUM_MEASUREMENTS)*NUM_PAGES,
        .period = HZ*NUM_MEASUREMENTS*NUM_PAGES
};

declare_task(task_read_memory);