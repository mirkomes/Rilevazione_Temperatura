#include "timer.h"
#include "main.h"

//inizializzazione del timer che permetterà l'esecuzione dei vari task
void timer_setup(void)
{
        //abilitazione del blocco timer
        *AHBCLKCTRL |= 0x400;
        
        *TCNR = 0x01; //abilitazione del timer 1
        *TPR = QUARZO/HZ; //impostazione del prescaling. La frequenza del timer sarà uguale a HZ
}