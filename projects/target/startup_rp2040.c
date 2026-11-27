/* startup_rp2040.c — minimal Cortex-M0+ startup for RP2040 flash boot.
 * Vector table lands at 0x10000100 (immediately after the 256-byte boot2),
 * which is where boot2's exit path jumps by default. */
#include <stdint.h>

extern uint32_t __etext, __data_start__, __data_end__, __bss_start__, __bss_end__;
extern int main(void);
void Reset_Handler(void);
void Default_Handler(void);

/* core vectors + 26 external IRQs — weak aliases cover the table */
void NMI_Handler(void)        __attribute__((weak, alias("Default_Handler")));
void HardFault_Handler(void)  __attribute__((weak, alias("Default_Handler")));
void SVC_Handler(void)        __attribute__((weak, alias("Default_Handler")));
void PendSV_Handler(void)     __attribute__((weak, alias("Default_Handler")));
void SysTick_Handler(void)    __attribute__((weak, alias("Default_Handler")));
void IRQ_Handler(void)        __attribute__((weak, alias("Default_Handler")));

extern uint32_t __StackTop;

__attribute__((section(".vectors"), used))
void * const vector_table[] = {
    &__StackTop,
    (void *)Reset_Handler,
    (void *)NMI_Handler,
    (void *)HardFault_Handler,
    0, 0, 0, 0, 0, 0, 0,
    (void *)SVC_Handler,
    0, 0,
    (void *)PendSV_Handler,
    (void *)SysTick_Handler,
    /* 26 external interrupts — all default */
    (void *)IRQ_Handler, (void *)IRQ_Handler, (void *)IRQ_Handler, (void *)IRQ_Handler,
    (void *)IRQ_Handler, (void *)IRQ_Handler, (void *)IRQ_Handler, (void *)IRQ_Handler,
    (void *)IRQ_Handler, (void *)IRQ_Handler, (void *)IRQ_Handler, (void *)IRQ_Handler,
    (void *)IRQ_Handler, (void *)IRQ_Handler, (void *)IRQ_Handler, (void *)IRQ_Handler,
    (void *)IRQ_Handler, (void *)IRQ_Handler, (void *)IRQ_Handler, (void *)IRQ_Handler,
    (void *)IRQ_Handler, (void *)IRQ_Handler, (void *)IRQ_Handler, (void *)IRQ_Handler,
    (void *)IRQ_Handler, (void *)IRQ_Handler,
};

void Reset_Handler(void)
{
    uint32_t *src = &__etext, *dst = &__data_start__;
    while (dst < &__data_end__) *dst++ = *src++;
    for (dst = &__bss_start__; dst < &__bss_end__; ) *dst++ = 0U;
    (void)main();
    for (;;) { }
}

void Default_Handler(void)
{
    for (;;) { }
}
