#include <stdint.h>

#define QUARZO (12 * 1000 * 1000)

#define HZ 100

extern volatile unsigned long jiffies;

struct task {
        char *name;
        void (*f)(void *);
        unsigned long period;
        unsigned long next_run;
};

struct temp {
        uint16_t *valori; //buffer circolare contenente n misurazioni
        void (*write)(void *); //scrittura di un valore
        void *(*read)(void); //lettura di tutti i valori del buffer per poi salvarli in memoria I2C
        void (*init)(void); //azzeramento dell'indice
        short *index;
};

extern struct temp *__temp_start[];
extern struct temp *__temp_end[];

extern struct task *__task_start[];
extern struct task *__task_end[];

#define declare_task(x) \
    struct task * \
    __attribute__((section(".task"))) \
    __task_ptr_##x = &x

#define declare_temp(x) \
    struct temp * \
    __attribute__((section(".temp"))) \
    __temp_ptr_##x = &x
