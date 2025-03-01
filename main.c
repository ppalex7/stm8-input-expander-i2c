#include "stm8l15x.h"

#define I2C_ADDRESS 0x68
// on Arduino it would be 0x34 (>>1)

volatile uint8_t InputState;

void main(void)
{
	// set PORT B external interrupt sensivity to "rising and falling edge"
	EXTI->CR3 &= (uint8_t)(~EXTI_CR3_PBIS);
	EXTI->CR3 |= (0b11 << 0);
	// PB[7:4] are used for EXTIB interrupt generation
	EXTI->CONF1 |= (0b1 << 1);

	// enable interrupts for PB4-PB7
	GPIOB->CR2 |= 0b11110000;

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

    enableInterrupts();

	while (1);
}
