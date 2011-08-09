#include "i2c.h"

//setup del bus i2c
void i2c_setup(void)
{
        //accensione del blocco i2c
        write_bit_register(AHBCLKCTRL, 5, 1);
        
        //reset della periferica
        write_bit_register(PRESETCTRL, 1, 1); 
        
        //impostazione della frequenza
        //STANDARD MODE - IMPOSTAZIONE A 100 KHz
        write_value_register(I2SCLL, 0, 15, 0x3c);
        write_value_register(I2SCLH, 0, 15, 0x3c);
                
        //FAST MODE PLUS - IMPOSTAZIONE AD 1 MHz
        //write_value_register(I2SCLL, 0, 15, 0x6);
        //write_value_register(I2SCLH, 0, 15, 0x6);
        
        //entro nella modalità di MASTER TRASMITTER MODE
        //azzero il bit di ASSERT ACK (è l'unico bit in cui non è specificato un valore di default)
        write_bit_register(I2C0CONCLR, 2, 1);
        
        //abilito l'interfaccia I2C
        write_bit_register(I2C0CONSET, 6, 1);       
        
}

//funzione di invio segnale di START. In caso di errore viene ritornato un valore diverso da 0,
//altrimenti si ritorna il valore 0.
int i2c_send_start(void)
{
        //imposto il bit STA = 1
        write_bit_register(I2C0CONSET, 5, 1);
        
        //controllo quando il bit SI diventa 1 e se il codice di stato è 0x08 tutto è andato a buon fine
        while ( !(((*I2C0CONSET) & (0x08)) != 0) )
        {
        }
        
        //quando l'SI bit diventa 1 lo start bit è stato inviato e si controlla lo stato
        uint32_t valore = *I2STAT;

        return (valore - 0x08);
}

//funzione utilizzata per indirizzare lo slave alla comunicazione (lettura o scrittura)
//in caso di errore viene ritornato un valore diverso da 0 altrimenti viene ritornato 0.
int i2c_address_slave(uint32_t address)
{      
       
        //--- FAST MODE PLUS --- Comunicazione al sensore di temperatura
        /*volatile int i = 0;
        
        //mando una parola per mettere lo slave in High Speed mode.
        *I2DAT = 0x0A;
        
        //resetto l'SI bit a 0 per trasmettere la parola
        *I2C0CONCLR |= 0x08;
        
        //attendo un ritardo e reimposto l'SI bit ad 1
        for (i = 0; i < 3000; i++)
        {}
        
        *I2C0CONSET |= 0x08;*/
        //--- FAST MODE PLUS ---
                
        //scrivo l'indirizzo da trasmettere nell'apposito registro di trasmissione
        *I2DAT = address;
        
        //resetto l'SI bit a 0
        *I2C0CONCLR |= 0x08;
        
        *(volatile uint32_t *) 0x5003003c = 0x0e;
        
        //si attende che SI torni ad 1 e quando succede si verifica lo stato
        while ( !(((*I2C0CONSET) & (0x08)) != 0) )
        {
        }
        
        *(volatile uint32_t *) 0x5003003c |= 0x0d;
        
        //si legge lo stato, è tutto OK se è pari a 0x18
        uint32_t valore = *I2STAT;
        
        return (valore - 0x18);
        
}

