#include "main.h"
#include "i2c.h"

//lettura dati dalla memoria ed elaborazione per invio alla seriale
void read_mem(void)
{
        puts("Read memory\n");
        //leggo dalla ram l'indirizzo finale dove sono stati salvati dati in memoria
        struct temp *buf = __temp_start[0];
        uint16_t addrs = buf->last_address[0] - NUM_MEASUREMENTS*2;
        
        //indirizzamento della memoria in modalità scrittura
        i2c_address_slave_start(MEMORY_WRITE);
        
        //invio del byte più significativo dell'indirizzo da leggere
        *I2DAT = (addrs & 0xff00) >> 8;
        *I2C0CONCLR = 0x08;
        
        while (*I2STAT != 0x28)
        {
	     if (*I2STAT == 0x0)
	     {
		   //si è verificato un bus-error
		   i2c_send_stop();
		   return;
	     }
        }
        
        //invio del byte meno significativo dell'indirizzo da leggere
        *I2DAT = addrs & 0x00ff;
        *I2C0CONCLR = 0x08;
        
        while (*I2STAT != 0x28)
        {
	     if (*I2STAT == 0x0)
	     {
		   //si è verificato un bus-error
		   i2c_send_stop();
		   return;
	     }
        }
        
        //abilitazione invio di ACK da parte del master
        *I2C0CONSET |= 0x04;
        
        //invio di START bit più indirizzamento in lettura
        *I2C0CONSET |= 0x20;
        *I2C0CONCLR = 0x08;
        
        while (*I2STAT != 0x10)
        {
	     if (*I2STAT == 0x0)
	     {
		   //si è verificato un bus-error
		   i2c_send_stop();
		   return;
	     }
        }
        
        *I2DAT = MEMORY_READ; //modalità di lettura del sensore
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
        
        //aspetto i dati in arrivo dalla memoria
        int i = 0;
        for ( i = 0; i < NUM_MEASUREMENTS; i++)
        {
	      //primo byte misurazione temperatura
	      *I2C0CONCLR = 0x08;
	      while (*I2STAT != 0x50)
	      {
		    if (*I2STAT == 0x0)
		    {
			  //si è verificato un bus-error
			  i2c_send_stop();
			  return;
		    }
	      }
	      puts("Dato memoria 1: ");
	      printhex(*I2DAT);
	      putc('\n');
	      
	      //secondo byte misurazione temperatura
	      *I2C0CONCLR = 0x08;
	      while (*I2STAT != 0x50)
	      {
		    if (*I2STAT == 0x0)
		    {
			  //si è verificato un bus-error
			  i2c_send_stop();
			  return;
		    }
	      }
	      puts("Dato memoria 2: ");
	      printhex(*I2DAT);
	      putc('\n');
        }
        
        //per l'ultimo dato della lista si disabilità l'invio di ACK da parte del master
        *I2C0CONCLR = 0x08;
        while (*I2STAT != 0x50)
        {
	     if (*I2STAT == 0x0)
	     {
		   //si è verificato un bus-error
		   i2c_send_stop();
		   return;
	     }
        }
        
        puts("Primo byte ultimo dato memoria: ");
        printhex(*I2DAT);
        putc('\n');
        
        *I2C0CONCLR = 0x0c;
        while (*I2STAT != 0x58)
        {
	     if (*I2STAT == 0x0)
	     {
		   //si è verificato un bus-error
		   i2c_send_stop();
		   return;
	     }
        }
        
        puts("Primo byte ultimo dato memoria: ");
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