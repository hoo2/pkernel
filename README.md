# pkernel
A small RTOS for ARM Cortex M3/4

pkernel is a very small multitasking kernel (RTOS) for Cortex M3/M4 microcontrollers, written in C in most parts and in assembly in as less code segments as possible

# Features

Some of the features of pkernel are:
* Supports privileged and unprivileged processes.
* Processes can exit
* A process can create processes
* Provide a very basic Unix-like cron capability for process time scheduling.
* Supports services. Services are small functions pkernel calls from inside SysTick ISR in a strict periodical manner
* Supports sleep and stop mode.
* Supports service running mode. pkernel runs without any call to processes and in only keeps alive services in sleep mode
* Support syscalls like: exit(), sleep(), wait(), signal(), lock(), unlock()
* Provide a very basic memory management via malloc-free (use with care).

# A small example
```C
#include "pkernel.h"
void pr_1 (void) {
   // process 1 code
   while (1) {
      // usually inside a loop
   }
}
void pr_2 (void) {
   // process 2 code
   while (1) {
      // usually inside a loop
   }
}
int main (void) {
   // init with 320bytes kernel stack, TICK_FREQ for system frequency and MCU clock = CLOCK
   kinit ((size_t)320, CLOCK, TICK_FREQ);  
   knew (&pr_1, (size_t)320, 1, 0);    // 320 byte stack for pr_1(), set "nice" flag and not set "fit" flag
   knew (&pr_2, (size_t)320, 1, 0);    // 320 byte stack for pr_1(), set "nice" flag and not set "fit" flag
   krun ();    // start pkernel
   while (1);  // Unreachable.
}

```
In the above example we create 2 process functions pr_1() and pr_2(). In the main function we initialize pkernel with the desired memory size for privileged process stack (idle process for example), the desired pkernel frequency (TICK_FREQ) and the hardware CLOCK of the MCU.
Next we register the two processes with 320 bytes of stack for each and run the kernel. After that point we will never return to main.
