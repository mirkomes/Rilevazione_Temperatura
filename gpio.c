#include "gpio.h"
#include "io.h"

void gpio_setup(void)
{
        //accendo il clock per poter configurare correttamente i pin di trasmissione e di ricezione
        *AHBCLKCTRL |= 0x010000;

        //configurazione pin UART
        *GPIO1_6 |= 0x01;
        *GPIO1_7 |= 0x01;
        
        //configurazione pin I2C
        //configurazione su UEXT (SCL)
        *GPIO0_4 |= 0x01;
        //write_bit_register(GPIO0_4, 9, 1); //FAST MODE PLUS
        
        //configurazione su UEXT (SDA)
        *GPIO0_5 |= 0x01;
        //write_bit_register(GPIO0_5, 9, 1); //FAST MODE PLUS
        
        //pin out per i2c
        *GPIO0DIR |= 0x30;
        
        //accensione delle porte GPIO (devo impostare il bit 6 di AHBCLKCTRL)
        *AHBCLKCTRL |= 0x40;
}