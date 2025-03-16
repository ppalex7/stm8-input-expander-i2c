#include "stm8l15x_it.h"
#include "stm8l15x.h"

extern volatile uint16_t g_input_state;
// ( PORTD_BUTTONS << 8 ) | PORTB_BUTTONS

INTERRUPT_HANDLER(NonHandledInterrupt, 0)
{
    return;
}

INTERRUPT_HANDLER(EXTI0567_IRQHandler, 8 /*13-15*/)
{
    uint8_t new_state_low;
    new_state_low = (uint8_t)((uint8_t)(~GPIOB->IDR) & 0b11100001);
    // pack into bits 7-0
    g_input_state = new_state_low | (g_input_state & 0xFF00);
    // Raise PC4 as "incoming request pending" flag
    GPIOC->ODR |= (0b1 << 4);
    // clear pending interrupt bit
    EXTI->SR1 = 0b11100001;
}

INTERRUPT_HANDLER(EXTID_H_IRQHandler, 7)
{
    uint8_t new_state_high;
    new_state_high = (uint8_t)((uint8_t)(~GPIOD->IDR) & 0b00000001);
    // pack into bits 15-8
    g_input_state = (new_state_high << 8) | (g_input_state & 0x00FF);
    // Raise PC4 as "incoming request pending" flag
    GPIOC->ODR |= (0b1 << 4);
    // clear pending interrupt bit PDF
    EXTI->SR2 = 0b00000010;
}

INTERRUPT_HANDLER(I2C1_SPI2_IRQHandler, 29)
{
    static uint16_t tx;
    static uint16_t sending_input_state;
    static uint8_t bytes_sent;

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

    if (sr3 == (I2C_SR3_TRA | I2C_SR3_BUSY))
    {
        // transmitter
        if (sr1 == (I2C_SR1_TXE | I2C_SR1_ADDR))
        {
            // address matched
            tx = g_input_state;
            sending_input_state = tx;
            bytes_sent = 0;
        }
        else if (sr1 == I2C_SR1_TXE || sr1 == (I2C_SR1_TXE | I2C_SR1_BTF))
        {
            // data register empty
            if (bytes_sent < sizeof(tx))
            {
                I2C1->DR = (uint8_t)tx;
                tx = tx >> 8;
                bytes_sent++;
            }
            else
            {
                if (sending_input_state == g_input_state)
                {
                    // actual data was sent, clear flag
                    GPIOC->ODR &= (uint8_t)~(0b1 << 4);
                }
            }
        }
    }
}