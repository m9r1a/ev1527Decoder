/*
 * ev1527.h
 *
 *  Created on: Oct 11, 2023
 *      Author: m.reza
 */

#ifndef INC_EV1527_H_
#define INC_EV1527_H_
typedef void (*RfCallbackFunc_t)(uint32_t );

void ev1527_IsrRoutine(uint16_t GPIO_Pin);
void ev1527_Init(IRQn_Type irq, uint16_t pin , GPIO_TypeDef* gpio ,RfCallbackFunc_t fun);
void ev1527_Process(void);
void ev1527_Disable(void);
void ev1527_Enable(void);
#endif /* INC_EV1527_H_ */
