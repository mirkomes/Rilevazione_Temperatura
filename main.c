#include "main.h"
#include "i2c.h"

void main(void)
{
       
       //abilitazione dei led per debugging
       *(volatile uint32_t *)0x50038000 = 0x0f;
       *(volatile uint32_t *) 0x5003003c = 0xfff;
       
       //invio di un bit di start
       //se si è verificato un errore si esce dall'applicazione e si accende 1 led
       if (i2c_send_start() != 0)
       {
	     *(volatile uint32_t *) 0x5003003c = 0x0d;
	     return;
       }
       
       //invio dell'indirizzo del sensore di temperatura in modalità scrittura
       
       if (i2c_address_slave(TEMP_WRITE) == 0)
	*(volatile uint32_t *) 0x5003003c = 0x0;
       else
	*(volatile uint32_t *) 0x5003003c = 0x01;
       
	     
       
}