#include "main.h"


void main(void)
{
       
       //abilitazione dei led per debugging
       *(volatile uint32_t *)0x50038000 = 0x0f;
       *(volatile uint32_t *) 0x5003003c = 0xfff;
       
       //putstring("PROVA");
       //char *s = getstring(3);
       
       //invio di un bit di start
       int stato = i2c_send_start();
       
       //se si è verificato un errore si esce dall'applicazione e si accende 1 led
       if (stato != 0)
       {
	     *(volatile uint32_t *) 0x5003003c = 0x0e;
	     return;
       }
       
       //invio dell'indirizzo del sensore di temperatura in modalità scrittura
       stato = i2c_address_slave(TEMP_WRITE);
       
       
       if (stato == 0)
	*(volatile uint32_t *) 0x5003003c = 0x0;
       else
	*(volatile uint32_t *) 0x5003003c = 0x01;
       
	     
       
}