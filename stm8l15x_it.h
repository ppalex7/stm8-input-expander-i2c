/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM8L15x_IT_H
#define __STM8L15x_IT_H

#include "stm8l15x.h"

void _stext(void); /* RESET startup routine */
INTERRUPT void NonHandledInterrupt(void);

#endif /* __STM8L15x_IT_H */