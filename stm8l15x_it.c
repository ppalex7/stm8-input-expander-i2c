#include "stm8l15x_it.h"
#include "stm8l15x.h"

extern volatile uint8_t InputState;

INTERRUPT_HANDLER(NonHandledInterrupt,0)
{
    return;
}

INTERRUPT_HANDLER(EXTI4567_IRQHandler, 12-15)
{
    InputState = (uint8_t) ((uint8_t)(~GPIOB->IDR) & 0b11110000);
    // Raise PC4 as "incoming request pending" flag
    GPIOC->ODR |= (0b1 << 4);
    // clear pending interrupt bit
    EXTI->SR1 = 0b11110000;
}

INTERRUPT_HANDLER(I2C1_SPI2_IRQHandler, 29)
{
    uint8_t data = InputState;
    uint8_t sr1;
    uint8_t sr3;

    // Check for errors
    if (I2C1->SR2)
    {
        // Reset error
        I2C1->SR2 = 0;
        // TODO: turn on error-LED
    }
    sr1 = I2C1->SR1;
    sr3 = I2C1->SR3;

    if ((sr1 & I2C_SR1_TXE) && (sr3 & (I2C_SR3_BUSY | I2C_SR3_TRA)))
    {
        I2C1->DR = data;
        if (InputState == data) {
            GPIOC->ODR &= (uint8_t) ~(0b1<<4);
        }
    }
}