#include "main.h"
#include "i2c.h"

void main(void)
{
       
       //abilitazione dei led per debugging
       *(volatile uint32_t *)0x50038000 = 0x0f;
       *(volatile uint32_t *) 0x5003003c = 0xfff;
       
       if (i2c_temp(1) == 0)
	*(volatile uint32_t *) 0x5003003c = 0x0;
       else
	*(volatile uint32_t *) 0x5003003c = 0x01;
       
}