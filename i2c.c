#include "i2c.h"

//setup del bus i2c
void i2c_setup(void)
{        
        //accensione del blocco i2c
        *AHBCLKCTRL |= 0x20;
        
        //reset della periferica
        *PRESETCTRL |= 0x02;
        
        //reset dei bit
        //*I2C0CONCLR |= 0x20;
        *I2C0CONCLR |= 0x6c;
        
        //IMPOSTAZIONE A 100KHz
        *I2SCLL |= 0x03c;
        *I2SCLH |= 0x03c;
        
        //FAST MODE - IMPOSTAZIONE A 400KHz
        //*I2SCLL |= 0xf;
        //*I2SCLH |= 0xf;
        
        //abilito l'interfaccia I2C
        *I2C0CONSET |= 0x40;
        
}

//configurazione per la lettura della temperatura dal sensore
void read_temp_config(void)
{
                
        //invio dell'indirizzo del temperature register
        *I2DAT = 0x00;
        *I2C0CONCLR = 0x08;
        
        while ( *I2STAT != 0x28 )
        {
	      //aspetto di ricevere l'ack per il byte appena inviato
        } 
        
}

//configurazione del sensore di temperatura (necessario se serve una configurazione differente da
//quella gia impostata di default ossia conversioni automatiche di temperatura con frequenza di 4KHz)
void i2c_temp_config(void)
{
        
        //invio dell'indirizzo del registro di configurazione del sensore di temperatura
        //(configuration register = 0x01)
        //invio dell'indirizzo del configuration register
        *I2DAT = 0x01;
        *I2C0CONCLR = 0x08;
        
        while ( *I2STAT != 0x28 )
        {
	      //aspetto di ricevere l'ack per il byte appena inviato
        }
        
        //imposto la velocità di conversione di temperatura a 1Hz
        *I2DAT = 0x60;
        *I2C0CONCLR = 0x08;
        
        while ( *I2STAT != 0x28 )
        {
	      //aspetto di ricevere l'ack per il byte appena inviato
        }
        
        *I2DAT = 0x60; //bit CR0 = 1; bit CR1 = 0;
        *I2C0CONCLR = 0x08;
        
        while ( *I2STAT != 0x28 )
        {
	      //aspetto di ricevere l'ack per il byte appena inviato
        }
}

//funzione di invio segnale di START. In caso di errore viene ritornato un valore diverso da 0,
//altrimenti si ritorna il valore 0.
void i2c_send_start(void)
{      
        //imposto il bit STA = 1
        *I2C0CONSET |= 0x20;
        
        //controllo quando il bit SI diventa 1 e se il codice di stato è 0x08 tutto è andato a buon fine
        //while ( !(((*I2C0CONSET) & (0x08)) != 0) )
        while ( (*I2C0CONSET & 0x08) != 0x08)
        {
	      
        }
}

void i2c_send_stop(void)
{
        //condizione di stop
        *I2C0CONCLR = 0x08;
        *I2C0CONSET = 0x50;
        
        //attesa dell'invio della condizione di stop
        while (( *I2C0CONSET & 0x10 ) != 0x0)
        {
	      
        }
}

//funzione utilizzata per indirizzare lo slave alla comunicazione (lettura o scrittura) insieme ad un bit di start
void i2c_address_slave_start(uint32_t address)
{      
        //invio dello start bit
        i2c_send_start();
        
        //scrivo lo slave address
        *I2DAT = address;
	      
        //resetto STA e SI bit a 0
        *I2C0CONCLR = 0x28;
        
        //si attende che SI torni ad 1 e quando succede si verifica lo stato
        while (*I2STAT != 0x18)
        {

        }
}

//legge un valore di 2 byte all'indirizzo specificato
uint16_t read_16bit_data(uint16_t indirizzo)
{
            
        //si preleva l'indirizzo da cui bisogna iniziare a scrivere la memoria eprom con i dati di temperatura
        i2c_address_slave_start(MEMORY_WRITE);
        
        //invio del byte più significativo dell'indirizzo da leggere
        *I2DAT = (indirizzo & 0xff00) >> 8;
        *I2C0CONCLR = 0x08;
        
        while (*I2STAT != 0x28)
        {
	     if (*I2STAT == 0x0)
	     {
		   //si è verificato un bus-error
		   i2c_send_stop();
		   return 0x0;
	     }
        }
        
        //invio del byte meno significativo dell'indirizzo da leggere
        *I2DAT = indirizzo & 0x00ff;
        *I2C0CONCLR = 0x08;
        
        while (*I2STAT != 0x28)
        {
	     if (*I2STAT == 0x0)
	     {
		   //si è verificato un bus-error
		   i2c_send_stop();
		   return 0x0;
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
		   return 0x0;
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
		   return 0x0;
	     }
        }
        
        //ricezione della parte più significativa del dato
        *I2C0CONCLR = 0x08;
        while (*I2STAT != 0x50)
        {
	      if (*I2STAT == 0x0)
	      {
		    //si è verificato un bus-error
		    i2c_send_stop();
		    return 0x0;
	      }
	      
        }
        
        uint16_t data = (*I2DAT << 8) & 0xff00;
        
        //ricezione della parte meno significativa del dato
        *I2C0CONCLR = 0x0c;
        while (*I2STAT != 0x58)
        {
	     if (*I2STAT == 0x0)
	     {
		   //si è verificato un bus-error
		   i2c_send_stop();
		   return 0x0;
	     }
        }
        
        data |= *I2DAT;
        i2c_send_stop();
        
        return data;
}


//scrive un valore di 2 byte all'indirizzo specificato
void write_16bit_data(uint16_t valore, uint16_t indirizzo)
{
        //si preleva l'indirizzo da cui bisogna iniziare a scrivere la memoria eprom con i dati di temperatura
        i2c_address_slave_start(MEMORY_WRITE);

        *I2DAT = (indirizzo & 0xff00) >> 8;
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
        
        *I2DAT = indirizzo & 0x00ff;
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
        
        *I2DAT = (valore & 0xff00) >> 8; //parte più significativa
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
        
        *I2DAT = valore & 0x00ff; //parte meno significativa
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
        
        i2c_send_stop();
        
}