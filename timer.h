extern void timer_setup(void);

#define REG(x) ((volatile uint32_t *)(x))

//contatore 1
//timer control register
#define	TCNR	REG(0x40018004)
//timer counter register
#define	TCR	REG(0x40018008)
//timer prescale register
#define	TPR	REG(0x4001800c)

#define AHBCLKCTRL	REG(0x40048080)