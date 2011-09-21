#include "main.h"
#include "i2c.h"
#include "io.h"

void main(void)
{
       
       //abilitazione dei led per debugging
       *(volatile uint32_t *)0x50038000 = 0x0f;
       *(volatile uint32_t *) 0x5003003c = 0xfff;
       
       //FASE 1 - CONFIGURAZIONE DEI COMPONENTI HARDWARE I2C
       
       //CONFIGURAZIONE DEL SENSORE DI TEMPERATURA
       //indirizzamento del sensore in scrittura
        i2c_address_slave_start(TEMP_WRITE);
       //configurazione del sensore di temperatura
       i2c_temp_config();
       //i2c STOP
       i2c_send_stop();
       
       i2c_address_slave_start(TEMP_WRITE);
       read_temp_config(); //configurazione per la lettura della temperatura
       i2c_send_stop();
       
       //---------------------------------------------------
       
       unsigned long j; //variabile utilizzata per impostare i periodi iniziali di attivazione
       struct task **t, *trun; //strutture e puntatori a strutture task
       
       puts("Inizializzazione OK...\n");
       //inizializzazione delle prime attivazioni per ogni task
       j = jiffies + HZ/2;
       
        for (t = __task_start; t < __task_end; t++) {
	struct task *p = *t;
	puts(p->name); putc('\n');
	if (p->init)
	p->init(p->arg);
	p->next_run += j;
        }

        //ciclo infinito di selezione dei task
        while (1) {
	      //trun rappresenta il prossimo task ad essere eseguito, all'inizio è impostato a 0
	      trun = 0;
	      //si procede la selezione scandendo tutti i task e guardando chi è che ha il next_run più stringente
	      for (t = __task_start; t < __task_end; t++) {
		    struct task *p = *t;
		    if (!trun || p->next_run < trun->next_run)
		    trun = p;
	      }
	      //si aspetta che arrivi il momento di avviare il task
	      while (jiffies < trun->next_run)
		    ;
	      
	      //si avvia il task e si aggiorna il suo prossimo tempo di esecuzione
	      trun->arg = trun->f(trun->arg);
	      trun->next_run += trun->period;
        }
}