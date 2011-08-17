#include "io.h"
#include "gpio.h"

void serial_setup(void)
{
                
        //setto il clock divider a 1 (gira a 12 Mhz)
        *UART_CLOCK_DIV |= 0x1;
        
        //accensione del blocco UART (devo impostare il bit 12 di AHBCLKCTRL)
        *AHBCLKCTRL |= 0x1000;
        
        //configuro il LINE CONTROL REGISTER (no parità, no break control, 8 bit di carattere, 1 bit di stop).
        //impostazione del DIVISOR LATCH REGISTER (con DLAB = 1)
        *UART_LCR |= 0x83;
        
        //imposto la velocità di trasmissione a 115200kbps (115384kbps)
        *UART_DIVMULVAL |= 0x85;
        *UART_DLL |= 0x4;
        *UART_DLM &= 0x0;
        
        //disabilito l'accesso al DLR (impongo DLAB = 0)
        *UART_LCR &= 0x7f;
        
        //disabilitazione completa degli interrupts associati alla UART
        *UART_IER &= 0x0;
        
        //abilitazione delle fifo di trasmissione e di ricezione e reset delle stesse
        //la dimensione della fifo in ricezione è impostata ad 8 caratteri
        //uint32_t fifo_control = 0x87;
        *UART_FCR &= 0x0;
        
}

//lettura di un carattere
int getc(void)
{
        //aspetto che arrivi un dato valido
        while(!(*UART_LSR & UART_LSR_RDR))
	      ;
        
        //leggo il valore ed applico una maschera
        int value = *UART_RBR;
        
        if (value == '\r')
	      value = '\n';
        
        return value;
}

//lettura di una stringa
int gets(int lenght, char *stringaLetta)
{       
        int i;
        
        for(i = 0; i < lenght; i++)
        {
	      stringaLetta[i] = getc();
	      //se rilevo l'andamento a capo esco dal ciclo
	      if (stringaLetta[i] == '\n')
		    break;
        }
        //ritorno il numero di caratteri effettivamente letti
        return i;
}

//scrittura di un carattere sulla seriale
void putc(int c)
{
    if (c == '\n')
	putc('\r');

    while (!(*UART_LSR & UART_LSR_THRE))
	; //si aspetta che il THR sia vuoto prima di inviare il prossimo carattere
    *UART_THR = c;
}

//scrittura di una stringa sulla seriale
void puts(char *s)
{
        while(*s)
	      putc(*(s++));
}

void printdebug(uint32_t value)
{
        unsigned char buf[4];

        buf[0] = value >> 24;
        buf[1] = value >> 16;
        buf[2] = value >> 8;
        buf[3] = value;
        
        putc(buf[0]);
        putc(buf[1]);
        putc(buf[2]);
        putc(buf[3]);
}

void printhex(uint32_t val)
{
           int i, c;

           putc('<');
           for (i = 28; i >= 0; i -= 4) {
                   c = (val >> i) & 0xf;
                   if (c < 10)
                           putc('0' + c);
                   else
                           putc('a' + c - 10);
           }
           putc('>');
}