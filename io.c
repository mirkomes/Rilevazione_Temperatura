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

//funzione di traduzione di un dato di temperatura da esadecimale ad ASCII con relativa stampa
void printdec(uint16_t val)
{
        //segno della temperatura
        //in caso di temperatura negativa si fa il complemento a 2 e si prepara il segno meno da stampare
        char sign = (val & (0x1 << 11)) ? '-' : '+';
        
        //se il valore è negativo allora preparo il complemento a 2 su cui lavorare
        val = (val & (0x1 << 11)) ? (~(val | 0xf000)) + 1 : val;
        //trasformazione del valore esadecimale in ASCII
        //separazione della parte intera dalla parte decimale
        uint8_t intero = (val & 0x0ff0) >> 4;
        uint8_t decimale = (val & 0x000f);
        
        //preparazione del valore delle centinaia in ASCII
        char centinaia = 0;
        if ((intero - 100) >= 0)
        {
	      centinaia = 1 + '0'; //valore ASCII corrispondente ad 1
	      intero = intero - 100;
        }
        
        //preparazione del valore delle unità in ASCII
        char i, unita;
        for(i = 90; i >= 0; i -= 10)
        {
	      if ((intero - i) >= 0)
	      {
		    unita = (intero - i) + '0'; //valore ASCII dell'unità
		    break;
	      }
        }
        
        char decine = ((intero - (unita - '0'))/10) + '0'; //valore ASCII delle decine
        char c1, c2, c3, c4 = 0;
        
        //gestione della parte decimale
        //caso di temperature positive
        
        switch(decimale)
        {
	      case 0:
		    c1 = c2 = c3 = c4 = '0';
		    break;
	      case 1:
		    c1 = '0';
		    c2 = '0' + 6;
		    c3 = '0' + 2;
		    c4 = '0' + 5;
		    break;
	      case 2:
		    c1 = '0' + 1;
		    c2 = '0' + 2;
		    c3 = '0' + 5;
		    c4 = '0';
		    break;
	      case 3:
		    c1 = '0' + 1;
		    c2 = '0' + 8;
		    c3 = '0' + 7;
		    c4 = '0' + 5;
		    break;
	      case 4:
		    c1 = '0' + 2;
		    c2 = '0' + 5;
		    c3 = '0';
		    c4 = '0';
		    break;
	      case 5:
		    c1 = '0' + 3;
		    c2 = '0' + 1;
		    c3 = '0' + 2;
		    c4 = '0' + 5;
		    break;
	      case 6:
		    c1 = '0' + 3;
		    c2 = '0' + 7;
		    c3 = '0' + 5;
		    c4 = '0';
		    break;
	      case 7:
		    c1 = '0' + 4;
		    c2 = '0' + 3;
		    c3 = '0' + 7;
		    c4 = '0' + 5;
		    break;
	      case 8:
		    c1 = '0' + 5;
		    c2 = '0';
		    c3 = '0';
		    c4 = '0';
		    break;
	      case 9:
		    c1 = '0' + 5;
		    c2 = '0' + 6;
		    c3 = '0' + 2;
		    c4 = '0' + 5;
		    break;
	      case 10:
		    c1 = '0' + 6;
		    c2 = '0' + 2;
		    c3 = '0' + 5;
		    c4 = '0';
		    break;
	      case 11:
		    c1 = '0' + 6;
		    c2 = '0' + 8;
		    c3 = '0' + 7;
		    c4 = '0' + 5;
		    break;
	      case 12:
		    c1 = '0' + 7;
		    c2 = '0' + 5;
		    c3 = '0';
		    c4 = '0';
		    break;
	      case 13:
		    c1 = '0' + 8;
		    c2 = '0' + 1;
		    c3 = '0' + 2;
		    c4 = '0' + 5;
		    break;
	      case 14:
		    c1 = '0' + 8;
		    c2 = '0' + 7;
		    c3 = '0' + 5;
		    c4 = '0';
		    break;
	      case 15:
		    c1 = '0' + 9;
		    c2 = '0' + 3;
		    c3 = '0' + 7;
		    c4 = '0' + 5;
		    break;
        }
        
        //stampo i valori su seriale
        putc(sign);
        if (centinaia > 0)
	      putc(centinaia);
        putc(decine);
        putc(unita);
        putc(',');
        putc(c1);
        putc(c2);
        putc(c3);
        putc(c4);
        
}