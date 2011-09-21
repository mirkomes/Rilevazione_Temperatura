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
        
        //FAST MODE - IMPOSTAZIONE A 400KHz
        *I2SCLL |= 0xf;
        *I2SCLH |= 0xf;
        
        //abilito l'interfaccia I2C
        *I2C0CONSET |= 0x40;
        
}

int i2c_temp(int write)
{
        //invio dello start bit
        i2c_send_start();
        
        int err = (i2c_address_slave(TEMP_WRITE) == 0) ? 0 : 1;
        
        return err;
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

//funzione utilizzata per indirizzare lo slave alla comunicazione (lettura o scrittura)
//in caso di errore viene ritornato un valore diverso da 0 altrimenti viene ritornato 0.
int i2c_address_slave(uint32_t address)
{      
        
        //scrivo lo slave address
        *I2DAT = address;
	      
        //resetto STA e SI bit a 0
        *I2C0CONCLR = 0x28;
        
        //si attende che SI torni ad 1 e quando succede si verifica lo stato
        while ( !(((*I2C0CONSET) & (0x08)) != 0) )
        {
        }
        
        //si legge lo stato, è tutto OK se è pari a 0x18
        uint32_t valore = *I2STAT;
        
        printhex(*I2STAT);
        
        return (valore - 0x18);
        
}

