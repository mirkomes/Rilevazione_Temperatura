#include "gpio.h"
#include "io.h"

void gpio_setup(void)
{
        //accendo il clock per poter configurare correttamente i pin di trasmissione e di ricezione
        write_bit_register(AHBCLKCTRL, 16, 1);

        //configurazione pin UART
        write_bit_register(GPIO1_6, 0, 1);
        write_bit_register(GPIO1_7, 0, 1);
        
        //configurazione pin I2C
        //configurazione su UEXT (SCL)
        write_bit_register(GPIO0_4, 0, 1);
        //write_bit_register(GPIO0_4, 9, 1); //FAST MODE PLUS
        
        //configurazione su UEXT (SDA)
        write_bit_register(GPIO0_5, 0, 1);
        //write_bit_register(GPIO0_5, 9, 1); //FAST MODE PLUS
        
        //accensione delle porte GPIO (devo impostare il bit 6 di AHBCLKCTRL)
        write_bit_register(AHBCLKCTRL, 6, 1);
}