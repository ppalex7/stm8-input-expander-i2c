#include "stm8l15x.h"

void main(void)
{
	// set PORT B external interrupt sensivity to "rising and falling edge"
	EXTI->CR3 &= (uint8_t)(~EXTI_CR3_PBIS);
	EXTI->CR3 |= (0b11 << 0);
	// PB[7:4] are used for EXTIB interrupt generation
	EXTI->CONF1 |= (0b1 << 1);

	// enable interrupts for PB4-PB7
	GPIOB->CR2 |= 0b11110000;

	enableInterrupts();

	while (1);
}
