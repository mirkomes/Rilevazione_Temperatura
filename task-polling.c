#include "main.h"
#include "gpio.h"
#include "i2c.h"

//polling di eventuali pulsanti premuti. Si da maggiore priorità al pulsante di reset (BUT-2)
void polling_buttons(void)
{
       //controllo se è stato premuto il pulsante di reset (BUT-2)
       uint8_t valore = (*GPIO1_4 >> 4);
       
       if (!valore)
       {
	     //se è stato premuto allora si resetta il last address in memoria e in eprom
	     struct temp *buf = __temp_start[0];
	     buf->last_address[0] = 0x0;
	     
	     //reset del valore in memoria eprom
	     i2c_address_slave_start(MEMORY_WRITE);
	     
	     //il valore viene salvato a partire dall'indirizzo 0x7fe0
	     
	     *I2DAT = 0x7f;
	     *I2C0CONCLR = 0x08;
        
	      while (*I2STAT != 0x28)
	      {
		    //aspetto l'ack
		    if (*I2STAT == 0x0)
		    {
			  //si è verificato un bus-error
			  i2c_send_stop();
			  return;
		    }
	      }
	      
	      *I2DAT = 0xe0;
	      *I2C0CONCLR = 0x08;
        
	      while (*I2STAT != 0x28)
	      {
		    //aspetto l'ack
		    if (*I2STAT == 0x0)
		    {
			  //si è verificato un bus-error
			  i2c_send_stop();
			  return;
		    }
	      }
	      
	      //invio del dato alla memoria
	      
	      int i;
	      
	      //si azzerano 4 byte a partire da 0x7fe0
	      for ( i = 0; i < 4; i++)
	      {
		    *I2DAT = 0x0;
		    *I2C0CONCLR = 0x08;
		    while (*I2STAT != 0x28)
		    {
			  //aspetto l'ack
			  if (*I2STAT == 0x0)
			  {
				//si è verificato un bus-error
				i2c_send_stop();
				return;
			  }
		    }
	      }
	      
	      i2c_send_stop();
	      
	      puts("Reset OK");
	      
	      //se è premuto il pulsante di reset, l'altro pulsante viene ignorato
	      return;
       }
       
       valore = *GPIO2_9 >> 9;
       //controllo se è stato premuto il pulsante di scaricamento dati (BUT-1)
       if(!valore)
       {
	     //scaricamento dei dati memorizzati nella eprom
	     //lo scaricamento inizia a partire dall'indirizzo contenuto nella posizione di memoria
	     //0x7fe0-0x7fe1 fino all'indirizzo contenuto in 0x7fe2-0x7fe3.
	     //questi valori vengono reimpostati a zero solamente dopo un reset del sistema
	     uint16_t address_start = read_16bit_data(0x7fe0);
	     printhex(address_start);
	     uint16_t address_end = read_16bit_data(0x7fe2);
	     printhex(address_end);
	     
	     //inizio ciclo di lettura
	     //la lettura avviene in maniera sequenziale
	     
	     uint16_t numero_cicli = (address_end - address_start)/2;
	     if (numero_cicli > 0)
	     {
		    i2c_address_slave_start(MEMORY_WRITE);
			  
		    //invio del byte più significativo dell'indirizzo da leggere
		    *I2DAT = (address_start & 0xff00) >> 8;
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
		    *I2DAT = address_start & 0x00ff;
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
		    
		    int i;
		    uint16_t misura;
		    for (i = 0; i < numero_cicli - 1; i++)
		    {
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
				misura = (*I2DAT << 8) & 0x0f00;
				
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
				misura |= *I2DAT;
				printdec(misura);
				putc('\n');
		    }
		    
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

		    misura = (*I2DAT << 8) & 0x0f00;

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
		    
		    misura |= *I2DAT;
		    printdec(misura);
		    putc('\n');
		    
		    i2c_send_stop();
	     }
	     write_16bit_data(address_end, 0x7fe0);
        }
}

struct task task_polling = {
        .name = "Polling buttons.",
        .f = polling_buttons,
        .next_run = HZ,
        .period = HZ/8
};

declare_task(task_polling);