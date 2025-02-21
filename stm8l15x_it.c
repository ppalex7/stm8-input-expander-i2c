#include "stm8l15x_it.h"
#include "stm8l15x.h"

extern volatile uint8_t InputState;

INTERRUPT_HANDLER(NonHandledInterrupt,0)
{
    return;
}

INTERRUPT_HANDLER(EXTIB_G_IRQHandler, 6)
{
    InputState = (uint8_t) (GPIOB->IDR & 0b11110000);
    // Raise PC4 as "incoming request pending" flag
    GPIOC->ODR |= (0b1 << 4);
}