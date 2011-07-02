#include <stdint.h>

extern void putchar(int c);
extern void putstring(char *s);
extern void serial_setup(void);
extern void write_bit_register(volatile uint32_t *reg, int offset, int value);
extern void write_value_register(volatile uint32_t *reg, int start_bit, int end_bit, uint32_t value);

#define REG(x) ((volatile uint32_t *)(x))

#define UART_THR		REG(0x40008000)
#define UART_RBR		REG(0x40008000)
#define UART_IER		REG(0x40008004)
#define UART_IIR		REG(0x40008008) 
#define UART_FCR		REG(0x40008008) 
#define UART_LCR		REG(0x4000800c)
#define UART_LSR		REG(0x40008014)
#define UART_LSR_THRE	0x20
#define UART_LSR_RDR	0x01

#define UART_DLL		REG(0x40008000) 
#define UART_DLM		REG(0x40008004) 
#define UART_DIVMULVAL	REG(0x40008028) 


#define AHBCLKCTRL	REG(0x40048080)

#define UART_CLOCK_DIV	REG(0x40048098)

#define GPIO1_6		REG(0x400440A4)
#define GPIO1_7		REG(0x400440A8)