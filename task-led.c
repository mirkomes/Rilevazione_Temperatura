#include "main.h"
static int led_j = 7;

//polling di eventuali pulsanti premuti. Si da maggiore priorità al pulsante di reset (BUT-2)
void led_flashing(void)
{

	      switch (led_j)
	      {
		    case 0:
			  *(volatile uint32_t *)0x500203c0 = 0xfff;
			  *(volatile uint32_t *)0x50030038 = 0xf;
			  *(volatile uint32_t *)0x50030004 = 0x0;
			  break;
		    case 1:
			  *(volatile uint32_t *)0x500203c0 = 0xfff;
			  *(volatile uint32_t *)0x50030034 = 0xf;
			  *(volatile uint32_t *)0x50030008 = 0x0;
			  break;
		    case 2:
			  *(volatile uint32_t *)0x500203c0 = 0xfff;
			  *(volatile uint32_t *)0x5003002c = 0xf;
			  *(volatile uint32_t *)0x50030010 = 0x0;
			  break;
		    case 3:
			  *(volatile uint32_t *)0x500203c0 = 0xfff;
			  *(volatile uint32_t *)0x5003001c = 0xf;
			  *(volatile uint32_t *)0x50030020 = 0x0;
			  break;
		    case 4:
			  *(volatile uint32_t *)0x5003003c = 0xf;
			  *(volatile uint32_t *)0x50020380 = 0xfff;
			  *(volatile uint32_t *)0x50020040 = 0x0;
			  break;
		    case 5:
			  *(volatile uint32_t *)0x5003003c = 0xf;
			  *(volatile uint32_t *)0x50020340 = 0xfff;
			  *(volatile uint32_t *)0x50020080 = 0x0;
			  break;
		    case 6:
			  *(volatile uint32_t *)0x5003003c = 0xf;
			  *(volatile uint32_t *)0x500202c0 = 0xfff;
			  *(volatile uint32_t *)0x50020100 = 0x0;
			  break;
		    case 7:
			  *(volatile uint32_t *)0x5003003c = 0xf;
			  *(volatile uint32_t *)0x500201c0 = 0xfff;
			  *(volatile uint32_t *)0x50020200 = 0x0;
			  break;
		    default:
			  break;
	      }
	      led_j = (led_j==0) ? 7 : led_j - 1;
}

struct task task_led = {
        .name = "Led flashing.",
        .f = led_flashing,
        .next_run = 0,
        .period = HZ/4
};

declare_task(task_led);