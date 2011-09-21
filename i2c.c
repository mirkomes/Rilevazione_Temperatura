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
        
        //imposto la velocità di conversione di temperatura a 8KHz
        *I2DAT = 0x60;
        *I2C0CONCLR = 0x08;
        
        while ( *I2STAT != 0x28 )
        {
	      //aspetto di ricevere l'ack per il byte appena inviato
        }
        
        *I2DAT = 0xe0; //bit CR0 = CR1 = 1
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
        while ( *I2STAT != 0x08 )
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

