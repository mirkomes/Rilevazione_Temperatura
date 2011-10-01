#include "main.h"
#include "i2c.h"

//funzione di scrittura di N misurazioni effettuate in memoria eprom
void task_pw(void)
{
        //buffer circolare in memoria
        struct temp *buf = __temp_start[0];
        
        //prelevo l'indirizzo di scrittura in memoria
        //verifico che non sia arrivato al limite della memoria
        uint16_t addrs = (buf->last_address[0] < 0x7fbf) ? buf->last_address[0] : 0x0;
        
        puts("Indirizzo memoria: ");
        printhex(addrs);
        putc('\n');
        
        //indirizzamento della memoria in scrittura
        i2c_address_slave_start(MEMORY_WRITE);
        
        puts("Inizio scrittura memoria\n");
        //mando l'indirizzo di inizio scrittura (prima MSB e poi LSB)
        *I2DAT = ((addrs & 0xff00) >> 8);
        *I2C0CONCLR = 0x08;
        
        while (*I2STAT != 0x28)
        {
	      //aspetto l'ack del MSB address
	     if (*I2STAT == 0x0)
	     {
		   //si è verificato un bus-error
		   i2c_send_stop();
		   return;
	     }
        }
        
        *I2DAT = (addrs & 0x00ff);
        *I2C0CONCLR = 0x08;
        
        while (*I2STAT != 0x28)
        {
	      //aspetto l'ack del LSB address
	     if (*I2STAT == 0x0)
	     {
		   //si è verificato un bus-error
		   i2c_send_stop();
		   return;
	     }
        }
        
        //inizio scrittura della pagina byte per byte
        int i = 0;
        uint16_t *cbuf = (uint16_t *)buf->read();
        
        for (i = 0; i<NUM_MEASUREMENTS; i++)
        {
	      printhex((cbuf[i] & 0xff00) >> 8);
	      *I2DAT = ((cbuf[i] & 0xff00) >> 8); //MSB della misurazione
	      *I2C0CONCLR = 0x08;
	      
	      while (*I2STAT != 0x28)
	      {
		    //attesa ack MSB misura i-esima
		    if (*I2STAT == 0x0)
		    {
			  //si è verificato un bus-error
			  i2c_send_stop();
			  return;
		    }
	      }
	      
	      printhex(cbuf[i] & 0x00ff);
	      *I2DAT = (cbuf[i] & 0x00ff); //LSB della misurazione
	      *I2C0CONCLR = 0x08;
	      
	      while (*I2STAT != 0x28)
	      {
		    //attesa ack LSB misura i-esima
		    if (*I2STAT == 0x0)
		    {
			  //si è verificato un bus-error
			  i2c_send_stop();
			  return;
		    }
	      }
	      
	      printhex(cbuf[i]);
	      putc('\n');
        }
        
        //fine della comunicazione
        i2c_send_stop();
        
        //aggiornamento del prossimo indirizzo di memoria da scrivere
        //se ci sono errori sul bus l'aggiornamento non viene fatto e al prossimo ciclo vengono sovrascritti i
        //dati errati
        buf->last_address[0] = addrs + NUM_MEASUREMENTS*2;
        
        puts("Fine scrittura memoria\n");
}


struct task task_page_write = {
        .name = "Scrittura pagina in EPROM.",
        .period = NUM_MEASUREMENTS*HZ,
        .next_run = (NUM_MEASUREMENTS-1)*HZ,
        .f = task_pw
};

declare_task(task_page_write);