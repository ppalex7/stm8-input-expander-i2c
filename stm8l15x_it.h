/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM8L15x_IT_H
#define __STM8L15x_IT_H

#include "stm8l15x.h"

void _stext(void); /* RESET startup routine */
INTERRUPT void NonHandledInterrupt(void);
INTERRUPT void I2C1_SPI2_IRQHandler(void); /* I2C1 / SPI2 */
INTERRUPT void EXTID_H_IRQHandler(void); /* EXTI PORTD / EXTI PORTH*/
INTERRUPT void DMA1_CHANNEL0_1_IRQHandler(void); /* DMA1 Channel0/1*/

INTERRUPT void EXTI0567_IRQHandler(void); /* common handler for EXTI PIN0; 5-7 */

#endif /* __STM8L15x_IT_H */