#include "stm8l15x.h"
#include "uart_logger.h"

#define I2C_ADDRESS 0x68
// on Arduino it would be 0x34 (>>1)

volatile uint16_t g_input_state;

void main(void)
{
    // set PortX 0 external interrupt sensivity to "rising and falling edge"
    EXTI->CR1 |= (0b11 << 0);
    // set PortX 765 external interrupt sensivity to "rising and falling edge"
    EXTI->CR2 |= (0b11 << 6) | (0b11 << 4) | (0b11 << 2);
    // set PORT D external interrupt sensivity to "rising and falling edge"
    EXTI->CR3 |= (0b11 << 2);

    // PB[7:4] are used for EXTI7-EXTI4 interrupt generation
    // PB[3:0] are used for EXTI3-EXTI0 interrupt generation
    // PD[3:0] are used for EXTID interrupt generation
    // reset the appropriate bits
    EXTI->CONF1 &= (uint8_t)(~(EXTI_CONF1_PDLIS | EXTI_CONF1_PBHIS | EXTI_CONF1_PBLIS));
    // then set the necessary ones
    EXTI->CONF1 |= EXTI_CONF1_PDLIS;
    // or just assign 0b00000100 to CONF1 if we don't care previous value

    // enable interrupts for PB0, PB5-PB7
    GPIOB->CR2 |= 0b11100001;
    // enable interrupts for PD0
    GPIOD->CR2 |= (0b1 << 0);

    // configure PC4 as output
    GPIOC->DDR |= (0b1 << 4);
    // configure PC4 as push-pull
    GPIOC->CR1 |= (0b1 << 4);

    // Feed clock to I2C
    CLK->PCKENR1 |= CLK_PCKENR1_I2C1;
    // Configure I2C frequency as 2MHz (default CKDIVR 8 with HSI 16Mhz)
    I2C1->FREQR = 0b00000010;
    // Configure I2C speed: 100 kHz
    // According to Table 91. I2C_CCR values for SCL frequency table:
    // 0x50 for 16 MHz input, we'v got 16/8=2 -> so 0x50 / 8 -> 0xA
    I2C1->CCRL = 0xA;
    // Enable peripheral
    I2C1->CR1 = I2C_CR1_PE;
    // Enable Acknoledgement
    I2C1->CR2 = I2C_CR2_ACK;
    // Configure own adress
    I2C1->OARL = I2C_ADDRESS;
    I2C1->OARH = I2C_OARH_ADDCONF;
    // Enable I2C buffer, event and error interrupts
    I2C1->ITR = I2C_ITR_ITBUFEN | I2C_ITR_ITEVTEN | I2C_ITR_ITERREN;

    // set UART BaudRate to 19200, so divider 2000000/19200 = 104 = 0x68 = 0b1101000
    configure_logger_peripheral(0x68u);

    enableInterrupts();

    log("device configured\n");

    while (1)
    {
        process_buffered_logs();
    }
}
