#include "main.h"
#include "i2c.h"

//lettura dati dalla memoria ed elaborazione per invio alla seriale
void read_mem(void)
{
        //leggo dalla ram l'indirizzo finale dove sono stati salvati dati in memoria
        struct temp *buf = __temp_start[0];
        uint16_t addrs = buf->last_address[0] - NUM_MEASUREMENTS*2*NUM_PAGES;
#if DEBUG
	puts("Indirizzo:");
	printhex(addrs);
	putc('\n');
#endif
        
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
        uint16_t misura = 0x0;
        int i = 0;
        for ( i = 0; i < NUM_MEASUREMENTS*NUM_PAGES - 1; i++)
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
	      misura = (*I2DAT & 0xf) << 8;
	      
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
	      misura |= (*I2DAT & 0x0ff);
	      printdec(misura);
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
        
        misura = (*I2DAT & 0xf) << 8;
        
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
        
        misura |= (*I2DAT & 0x0ff);
	printdec(misura);
        putc('\n');
        
        i2c_send_stop();
        
}

struct task task_read_memory = {
        .name = "Read memory data.",
        .f = read_mem,
        .next_run = HZ*(NUM_MEASUREMENTS)*NUM_PAGES,
        .period = HZ*NUM_MEASUREMENTS*NUM_PAGES
};

//se la lettura automatica è disabilitata allora il task non viene istanziato
# if AUTOMATIC_MEMORY_READ
  declare_task(task_read_memory);
#endif