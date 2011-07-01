#include <stdint.h>
#include "io.h"


void serial_setup(void)
{
        
        //accendo il clock per poter configurare correttamente i pin di trasmissione e di ricezione
        write_bit_register(UART_BLOCK_CONTROL, 16, 1);

        //configuro i pin della GPIO1 in modalità di TX e RX
        write_bit_register(GPIO1_6, 0, 1);
        write_bit_register(GPIO1_7, 0, 1);
        
        //setto il clock divider a 1 (gira a 12 Mhz)
        uint32_t clock_divider = 0x1;
        write_value_register(UART_CLOCK_DIV, 0, 7, clock_divider);
        
        //accensione del blocco UART (devo impostare il bit 12 di UART_BLOCK_CONTROL)
        write_bit_register(UART_BLOCK_CONTROL, 12, 1);
        //accensione delle porte GPIO (devo impostare il bit 6 di UART_BLOCK_CONTROL)
        write_bit_register(UART_BLOCK_CONTROL, 6, 1);
        
        //configuro il LINE CONTROL REGISTER (no parità, no break control, 8 bit di carattere, 1 bit di stop).
        //impostazione del DIVISOR LATCH REGISTER (con DLAB = 1)
        uint32_t lcr = 0x83;
        write_value_register(UART_LCR, 0, 7, lcr);
        
        //imposto la velocità di trasmissione a 115200kbps (115384kbps)
        uint32_t divmulval = 0x85;
        write_value_register(UART_DIVMULVAL, 0, 7, divmulval);
        uint32_t dlm = 0x0;
        uint32_t dll = 0x4;
        write_value_register(UART_DLL, 0, 7, dll);
        write_value_register(UART_DLM, 0, 7, dlm);
        
        //disabilito l'accesso al DLR (impongo DLAB = 0)
        write_bit_register(UART_LCR, 7, 0);
        
        //disabilitazione completa degli interrupts associati alla UART
        uint32_t uart_interrupts_enable = 0x0;
        write_value_register(UART_IER, 0, 9, uart_interrupts_enable);
        
        //abilitazione delle fifo di trasmissione e di ricezione e reset delle stesse
        //la dimensione della fifo in ricezione è impostata ad 8 caratteri
        //uint32_t fifo_control = 0x87;
        uint32_t fifo_control = 0x00;
        write_value_register(UART_FCR, 0, 7, fifo_control);
        
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

//scrive il valore di un bit in un registro specificato
void write_bit_register(volatile uint32_t *reg, int offset, int value)
{
        uint32_t mask = (0x1 << offset);
        uint32_t new_value = *reg;
        if(value)
	      new_value |= mask;
        else
	      new_value &= ~mask;
                
        *reg = new_value;
                   
        return;
}

//scrive un valore a 32 bit all'interno di un registro specificato toccando solo i bit interessati
//(si parte dallo start_bit meno significativo fino all'end_bit più significativo)
void write_value_register(volatile uint32_t *reg, int start_bit, int end_bit, uint32_t value)
{
        
        //leggo il valore attuale salvato sul registro
        uint32_t register_value = *reg;
        
        //sostituisco il valore attuale con il nuovo valore da scrivere
        register_value = value;
        
        //generazione della maschera di sicurezza da mettere in and con il nuovo valore
        int i = 0;
        uint32_t mask = (0x1 << start_bit);
        for (i = start_bit + 1; i <= end_bit; i++)
        {
	      mask |= (0x1 << i);
        }
        
        register_value &= mask;
        //salvataggio del valore sul registro
        *reg = register_value;
        return;
}