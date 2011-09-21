#include <stdint.h>

#define QUARZO (12 * 1000 * 1000)

#define HZ 100

extern volatile unsigned long jiffies;

struct task {
	char *name;
	void *(*f)(void *);
	void (*init)(void *);
	unsigned long period;
	unsigned long next_run;
	void *arg;
};

extern struct task *__task_start[];
extern struct task *__task_end[];

#define declare_task(x) \
    struct task * \
    __attribute__((section(".task"))) \
    __task_ptr_##x = &x
