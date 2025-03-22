#include "stm8l15x_it.h"
#include "uart_logger.h"
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
    logf("EXTI0567 handler: new state 0x%04hX\n", g_input_state);
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
    logf("EXTID handler: new state 0x%04hX\n", g_input_state);
    // clear pending interrupt bit PDF
    EXTI->SR2 = 0b00000010;
}

INTERRUPT_HANDLER(I2C1_SPI2_IRQHandler, 29)
{
    static uint16_t tx;
    static uint16_t sending_input_state;
    static uint8_t bytes_sent;

    uint8_t rx;

    uint8_t sr1;
    uint8_t sr2;
    uint8_t sr3;

    // Check for errors
    sr2 = I2C1->SR2;
    if (sr2)
    {
        // Reset error
        I2C1->SR2 = 0;

        if (sr2 & I2C_SR2_AF && bytes_sent == sizeof(tx))
        {
            // EV3_2
            // not a bug
            return;
        }
        else
        {
            logf("I2C error, SR2: 0x%02hX\n", sr2);
        }
    }

    sr1 = I2C1->SR1;
    sr3 = I2C1->SR3;
    logf("I2C event, SR1_SR3 values: 0x%04hX\n", (sr1 << 8) | sr3);

    // process S3 ignoring busy bit
    sr3 &= (uint8_t) (~I2C_SR3_BUSY);

    if (sr3 == I2C_SR3_TRA)
    {
        // transmitter
        if (sr1 & I2C_SR1_ADDR)
        {
            // address matched
            tx = g_input_state;
            sending_input_state = tx;
            bytes_sent = 0;
        }
        if (sr1 & I2C_SR1_TXE)
        {
            // data register empty
            if (bytes_sent < sizeof(tx))
            {
                I2C1->DR = (uint8_t)tx;
                tx = tx >> 8;
                bytes_sent++;
                logf("I2C: %d bytes sent\n", bytes_sent);
            }
            else
            {
                if (sending_input_state == g_input_state)
                {
                    // actual data was sent, clear flag
                    log("I2C: actual state transmitted, turn off \"pending request\" flag\n");
                    GPIOC->ODR &= (uint8_t)~(0b1 << 4);
                }
                else
                {
                    log("I2C: stale state transmitted, keep \"pending request\" flag on\n");
                }
            }
        }
    }
    if (sr3 == 0)
    {
        // receiver
        // do noting on address matched (sr1 & I2C_SR1_ADDR)
        if (sr1 & I2C_SR1_RXNE)
        {
            // data register empty
            rx = I2C1->DR;
            logf("I2C: received 0x%02hX, update PORTB\n", rx);
            rx &= 0b00011110;
            // we can overwrite other pins because they configured as input.
            GPIOB->ODR = rx;
        }
        if (sr1 & I2C_SR1_STOPF)
        {
            // EV4 (stop condition). respond with ack
            I2C1->CR2 |= I2C_CR2_ACK;
        }
    }
}

INTERRUPT_HANDLER(DMA1_CHANNEL0_1_IRQHandler, 2)
{
    on_dma_log_transfer_complete();
}