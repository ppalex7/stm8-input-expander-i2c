/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM8L15x_IT_H
#define __STM8L15x_IT_H

#include "stm8l15x.h"

void _stext(void); /* RESET startup routine */
INTERRUPT void NonHandledInterrupt(void);
INTERRUPT void I2C1_SPI2_IRQHandler(void); /* I2C1 / SPI2 */

INTERRUPT void EXTI4567_IRQHandler(void); /* common handler for EXTI PIN4-7 */

#endif /* __STM8L15x_IT_H */